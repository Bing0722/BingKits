#pragma once

#include <BKits/Common-inl.h>
#include <memory>
#include <string>

// clang-format on
namespace BKits {
namespace Base {

template <typename T> class buffer {
private:
  T *ptr_;
  size_t size_;
  size_t capacity_;

  using grow_fun = void (*)(buffer &buf, size_t capacity);
  // ==
  // typedef void (*grow_fun)(buffer& buf, size_t capacity);
  grow_fun grow_;

protected:
  BKITS_CONSTEXPR buffer(grow_fun grow, size_t sz) noexcept
      : size_(sz), capacity_(sz), grow_(grow) {}

  constexpr buffer(grow_fun grow, T *p = nullptr, size_t sz = 0,
                   size_t cap = 0) noexcept
      : ptr_(p), size_(sz), capacity_(cap), grow_(grow) {}

  BKITS_CONSTEXPR20 ~buffer() = default;
  buffer(buffer &&) = default;

  BKITS_CONSTEXPR void set(T *buf_data, size_t buf_capacity) noexcept {
    ptr_ = buf_data;
    capacity_ = buf_capacity;
  }

public:
  using value_type = T;
  using const_reference = const T &;

  buffer(const buffer &) = delete;
  void operator=(const buffer &) = delete;

  auto begin() noexcept -> T * { return ptr_; }
  auto end() noexcept -> T * { return ptr_ + size_; }

  auto begin() const noexcept -> const T * { return ptr_; }
  auto end() const noexcept -> const T * { return ptr_ + size_; }

  constexpr auto size() const noexcept -> size_t { return size_; }
  constexpr auto capacity() const noexcept -> size_t { return capacity_; }

  BKITS_CONSTEXPR auto data() noexcept -> T * { return ptr_; }
  BKITS_CONSTEXPR auto data() const noexcept -> const T * { return ptr_; }

  BKITS_CONSTEXPR void clear() { size_ = 0; }

  BKITS_CONSTEXPR void try_resize(size_t count) {
    try_reserve(count);
    size_ = min_of(count, capacity_);
  }

  BKITS_CONSTEXPR void try_reserve(size_t new_capacity) {
    if (new_capacity > capacity_)
      grow_(*this, new_capacity);
  }

  BKITS_CONSTEXPR void push_back(const T &value) {
    try_reserve(size_ + 1);
    ptr_[size_++] = value;
  }

  template <typename U>
#if !BKITS_MSC_VERSION || BKITS_MSC_VERSION >= 1940
  BKITS_CONSTEXPR20
#endif
      void append(const U *begin, const U *end) {
    while (begin != end) {
      auto count = detail::to_unsigned(end - begin);
      try_reserve(size_ + count);
      auto free_cap = capacity_ - size_;
      if (free_cap < count)
        count = free_cap;
      T *out = ptr_ + size_;
      for (size_t i = 0; i < count; ++i)
        out[i] = begin[i];
      size_ += count;
      begin += count;
    }
  }

  template <typename Idx> BKITS_CONSTEXPR auto operator[](Idx index) -> T & {
    return ptr_[index];
  }

  template <typename Idx>
  BKITS_CONSTEXPR auto operator[](Idx index) const -> const T & {
    return ptr_[index];
  }
};

enum { inline_buffer_size = 500 };

template <typename T, size_t SIZE = inline_buffer_size,
          typename Allocator = detail::allocator<T>>
class basic_memory_buffer : public detail::buffer<T> {
private:
  T store_[SIZE];

  Allocator alloc_;

  BKITS_CONSTEXPR20 void deallocate() {
    T *data = this->data();
    if (data != store_)
      alloc_.deallocate(data, this->capacity());
  }

  static BKITS_CONSTEXPR20 void grow(detail::buffer<T> &buf, size_t size) {
    detail::abort_fuzzing_if(size > 5000);
    auto &self = static_cast<basic_memory_buffer &>(buf);
    const size_t max_size =
        std::allocator_traits<Allocator>::max_size(self.alloc_);
    size_t old_capacity = buf.capacity();
    size_t new_capacity = old_capacity + old_capacity / 2;
    if (size > new_capacity)
      new_capacity = size;
    else if (new_capacity > max_size)
      new_capacity = detail::max_of(size, max_size);

    T *old_data = buf.data();
    T *new_data = self.alloc_.allocate(new_capacity);
    detail::assume(buf.size() <= new_capacity);
    memcpy(new_data, old_data, buf.size() * sizeof(T));
    self.set(new_data, new_capacity);
    if (old_data != self.store_)
      self.alloc_.deallocate(old_data, old_capacity);
  }

public:
  using value_type = T;
  using const_reference = const T &;

  BKITS_CONSTEXPR explicit basic_memory_buffer(
      const Allocator &alloc = Allocator())
      : detail::buffer<T>(grow), alloc_(alloc) {
    this->set(store_, SIZE);
    if (detail::is_constant_evaluated())
      detail::fill_n(store_, SIZE, T());
  }

  BKITS_CONSTEXPR20 ~basic_memory_buffer() { deallocate(); }

private:
  BKITS_CONSTEXPR20 void move(basic_memory_buffer &other) {
    alloc_ = std::move(other.alloc_);
    T *data = other.data();
    size_t size = other.size(), capacity = other.capacity();
    if (data == other.store_) {
      this->set(store_, capacity);
      detail::copy<T>(other.store_, other.store_ + size, store_);
    } else {
      this->set(data, capacity);
      other.set(other.store_, 0);
      other.clear();
    }
    this->resize(size);
  }

public:
  BKITS_CONSTEXPR20 basic_memory_buffer(basic_memory_buffer &&other) noexcept
      : detail::buffer<T>(grow) {
    move(other);
  }

  auto operator=(basic_memory_buffer &&other) noexcept
      -> basic_memory_buffer & {
    CBIN_ASSERT(this != &other, "");
    deallocate();
    move(other);
    return *this;
  }

  auto get_allocator() const -> Allocator { return alloc_; }

  BKITS_CONSTEXPR void resize(size_t count) { this->try_resize(count); }

  void reserve(size_t new_capacity) { this->try_reserve(new_capacity); }

  using detail::buffer<T>::append;
  template <typename ContiguousRange>
  BKITS_CONSTEXPR20 void append(const ContiguousRange &range) {
    append(range.data(), range.data() + range.size());
  }
};

using memory_buffer = basic_memory_buffer<char>;

template <size_t SIZE>
auto to_string(const basic_memory_buffer<char, SIZE> &buf) -> std::string {
  auto size = buf.size();
  detail::assume(size < std::string().max_size());
  return {buf.data(), size};
}

} // namespace Base
} // namespace BKits
// clang-format on

#pragma once

#include <BKits/Common-inl.h>
#include <ostream>

// clang-format on
namespace BKits {
namespace Base {

template <typename Char> class basic_string_view {
private:
  const Char *data_;
  size_t size_;

public:
  using value_type = Char;
  using iterator = const Char *;

  constexpr basic_string_view() noexcept : data_(nullptr), size_(0) {}

  constexpr basic_string_view(const Char *s, size_t count) noexcept
      : data_(s), size_(count) {}

  constexpr basic_string_view(nullptr_t) = delete;

  constexpr basic_string_view(const Char *s) : data_(s) {
#if BKITS_HAS_BUILTIN(__builtin_strlen)
    if (std::is_same<Char, char>::value) {
      size_ = __builtin_strlen(s);
      return;
    }
#endif
    size_t len = 0;
    while (*s++)
      ++len;
    size_ = len;
  }

  template <typename S,
            std::enable_if_t<detail::is_std_string_like<S>::value &&
                                 std::is_same_v<typename S::value_type, Char>,
                             int> = 0>
  constexpr basic_string_view(const S &s) noexcept
      : data_(s.data()), size_(s.size()) {}

  constexpr auto data() const noexcept -> const Char * { return data_; }

  constexpr auto size() const noexcept -> size_t { return size_; }

  constexpr auto begin() const noexcept -> iterator { return data_; }

  constexpr auto end() const noexcept -> iterator { return data_ + size_; }

  constexpr auto operator[](size_t pos) const noexcept -> const Char & {
    return data_[pos];
  }

  constexpr void remove_prefix(size_t n) noexcept {
    data_ += n;
    size_ -= n;
  }

  constexpr void remove_suffix(size_t n) noexcept { size_ -= n; }

  constexpr auto start_with(basic_string_view<Char> sv) const noexcept -> bool {
    return size_ >= sv.size_ && detail::compare(data_, sv.data_, size_) == 0;
  }

  constexpr auto start_with(Char c) const noexcept -> bool {
    return size_ >= 1 && *data_ == c;
  }

  constexpr auto start_with(const Char *s) const -> bool {
    return start_with(basic_string_view<Char>(s));
  }

  constexpr auto compare(basic_string_view other) const -> int {
    int result =
        detail::compare(data_, other.data_, min_of(size_, other.size_));
    if (result != 0)
      return result;
    return size_ == other.size_ ? 0 : (size_ < other.size_ ? -1 : 1);
  }

  constexpr friend auto operator==(basic_string_view lhs, basic_string_view rhs)
      -> bool {
    return lhs.compare(rhs) == 0;
  }

  constexpr friend auto operator!=(basic_string_view lhs, basic_string_view rhs)
      -> bool {
    return lhs.compare(rhs) != 0;
  }
  constexpr friend auto operator<(basic_string_view lhs, basic_string_view rhs)
      -> bool {
    return lhs.compare(rhs) < 0;
  }
  constexpr friend auto operator<=(basic_string_view lhs, basic_string_view rhs)
      -> bool {
    return lhs.compare(rhs) <= 0;
  }
  constexpr friend auto operator>(basic_string_view lhs, basic_string_view rhs)
      -> bool {
    return lhs.compare(rhs) > 0;
  }
  constexpr friend auto operator>=(basic_string_view lhs, basic_string_view rhs)
      -> bool {
    return lhs.compare(rhs) >= 0;
  }
};

template <typename Char>
std::basic_ostream<Char> &
operator<<(std::basic_ostream<Char> &os,
           const detail::basic_string_view<Char> &sv) {
  return os.write(sv.data(), sv.size());
}

using string_view = detail::basic_string_view<char>;

} // namespace Base
} // namespace BKits
// clang-format on

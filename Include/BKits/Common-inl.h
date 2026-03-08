#pragma once

#include <BKits/Base.h>

#include <cstdio>
#include <cstring>
#include <limits>
#include <new>
#include <stdlib.h>
#include <type_traits>

// clang-format off
#ifndef BKITS_THROW
#  if BKITS_USE_EXCEPTIONS
#    if BKITS_MSC_VERSION || defined(__NVCC__)
BKITS_BEGIN_NAMESPACE
namespace detail {
template <typename Exception>
inline void do_throw(const Exception& x) {
    volatile bool b = true;
    if (b) throw x;
}
}
BKITS_END_NAMESPACE
#      define BKITS_THROW(x) detail::do_throw(x)
#    else
#      define BKITS_THROW(x) throw x
#    endif
#  else
#    define BKITS_THROW\
        detail::assert_fail(__FILE__, __LINE__, (x).what())
#  endif
#endif

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

// 编译期三元运算符
template <bool B, typename T, typename F>
using conditional_t = typename std::conditional<B, T, F>::type;

// 布尔值包装
template <bool B>
using bool_constant = std::integral_constant<bool, B>;

// 类型修饰符移除
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
template <typename T>
using remove_const_t = typename std::remove_const<T>::type;
template <typename T>
using remove_cvref_t = typename std::remove_cv<remove_reference_t<T>>::typep;

// 转无符号类型
template <typename T>
using make_unsigned_t = typename std::make_unsigned<T>::type;

// 获取枚举底层类型
template <typename T>
using underlying_t = typename std::underlying_type<T>::type;

// 类型退化
template <typename T>
using decay_t = typename std::decay<T>::type;

// 空指针类型
using nullptr_t = decltype(nullptr);

#if BKITS_MSC_VERSION
template <typename ...>
struct void_t_impl {
    using type = void;
};
template <typename... T>
using void_t = typename void_t_impl<T...>::type;
#else
template <typename...>
using void_t = void;
#endif

struct monostate {
    constexpr monostate() {}
};

#ifdef BKITS_DOC
#  define BKITS_ENABLE_IF(...)
#else
#  define BKITS_ENABLE_IF(...) enable_if_t<(__VA_ARGS__), int> = 0
#endif

BKITS_BEGIN_NAMESPACE
namespace detail
{

template <typename... T>
BKITS_CONSTEXPR void ignore_unused(const T&...) {}

constexpr auto is_constant_evaluated(bool default_value = false) noexcept -> bool {
#if BKITS_CPLUSPLUS >= 202002L && BKITS_GLIBCXX_RELEASE >= 12 && \
    (BKITS_CLANG_VERSION >= 1400 && BKITS_CLANG_VERSION < 1500)
    ignore_unused(default_value);
    return __builtin_is_constant_evaluated();
#elif defined(__cpp_lib_is_constant_evaluated)
    ignore_unused(default_value);
    return std::is_constant_evaluated();
#else
    return default_value;
#endif
}

template <typename T>
BKITS_ALWAYS_INLINE constexpr auto const_check(T val) -> T {
    return val;
}

inline void assert_fail(const char* file, int line, const char* message) {
    fprintf(stderr, "%s:%d: assertion failed: %s", file, line, message);
    abort();
}

#if defined(BKITS_ASSERT)
#elif defined(NDEBUG)
#  define BKITS_ASSERT(condition, message) \
        ignore_unused(condition, message)
#else
#  define BKITS_ASSERT(condition, message) \
        ((condition) ? (void)0 : assert_fail(__FILE__, __LINE__, (message)))
#endif

template <typename Int>
BKITS_CONSTEXPR auto to_unsigned(Int value) -> make_unsigned_t<Int> {
    BKITS_ASSERT(std::is_unsigned<Int>::value || value >= 0, "negative value");
    return static_cast<make_unsigned_t<Int>>(value);
}

template <typename T, typename Enable = void>
struct is_std_string_like : std::false_type {};

template <typename T>
struct is_std_string_like<T, void_t<decltype(std::declval<T>().find_first_of(
                                    typename T::value_type(), 0))>>
    : std::is_convertible<decltype(std::declval<T>().data()),
                        const typename T::value_type*> {};

template <typename T>
constexpr auto min_of(T a, T b) -> T { return a < b ? a : b; }

template <typename T>
constexpr auto max_of(T a, T b) -> T { return a > b ? a : b; }

template <typename T>
constexpr const char* narrow(const T*) { return nullptr; }
constexpr BKITS_ALWAYS_INLINE const char* narrow(const char* s) { return s; }

template <typename Char>
BKITS_CONSTEXPR auto compare(const Char* s1, const Char* s2, std::size_t n) -> int {
    if (!is_constant_evaluated() && sizeof(Char) == 1) return memcpy(s1, s2, n);
    for (; n != 0; ++s1, ++s2, --n) {
        if (*s1 < *s2) return -1;
        if (*s1 > *s2) return 1;
    }
    return 0;
}

namespace adl {
using namespace std;

template <typename Container>
auto invoke_back_inserter() -> decltype(back_inserter(std::declval<Container&>()));
}

template <typename It, typename Enable = std::true_type>
struct is_back_insert_interator : std::false_type {};

template <typename It>
struct is_back_insert_interator<
    It, bool_constant<std::is_same<
        decltype(adl::invoke_back_inserter<typename It::container_type>()),
        It>::value>> : std::true_type {};

template <typename OutputIt>
inline BKITS_CONSTEXPR20 auto get_container(OutputIt it) ->
  typename OutputIt::container_type& {
    struct accessor : OutputIt {
        BKITS_CONSTEXPR20 accessor(OutputIt base) : OutputIt(base) {}
        using OutputIt::container;
    };
    return *accessor(it).container;
}

template <typename T>
constexpr auto max_value() -> T {
    return (std::numeric_limits<T>::max)();
}

template <typename T>
constexpr auto num_bits() -> int {
    return std::numeric_limits<T>::digits;
}

template <typename T>
struct allocator {
    using value_type = T;

    T* allocate(size_t n) {
        BKITS_ASSERT(n <= max_value<size_t>() / sizeof(T), "");
        T* p = static_cast<T*>(malloc(n * sizeof(T)));
        if (!p) BKITS_THROW(std::bad_alloc());
        return p;
    }

    void deallocate(T* p, size_t) { free(p); }
};

BKITS_CONSTEXPR inline void abort_fuzzing_if(bool condition) {
    ignore_unused(condition);
#ifdef BKITS_FUZZ
    if (condition) throw std::runtime_error("fuzzing limit reached");
#endif
}
BKITS_INLINE void assume(bool condition) {
    (void)condition;
#if BKITS_HAS_BUILTIN(__builtin_assume) && !BKITS_ICC_VERSION
    __builtin_assume(condition);
#elif BKITS_GCC_VERSION
    if (!condition) __builtin_unreachable();
#endif
}

template <typename OutputIt, typename Size, typename T>
BKITS_CONSTEXPR auto fill_n(OutputIt out, Size count, const T& value) -> OutputIt {
    for (Size i = 0; i < count; ++i) *out++ = value;
    return out;
}

template <typename T, typename Size>
BKITS_CONSTEXPR20 auto fill_n(T* out, Size count, char value) -> T* {
    if (is_constant_evaluated()) return fill_n<T*, Size, T>(out, count, value);
    std::memset(out, value, to_unsigned(count));
    return out + count;
}

template<typename T>
class basic_string_view;

template<typename T>
class buffer;

template <typename T>
class basic_appender {
protected:
    buffer<T>* container;
public:
    using container_type = buffer<T>;

    BKITS_CONSTEXPR basic_appender(buffer<T>& buf) : container(&buf) {}

    BKITS_CONSTEXPR20 auto operator=(T c) -> basic_appender& {
        container->push_back(c);
        return *this;
    }
    BKITS_CONSTEXPR20 auto operator*() -> basic_appender& { return *this; }
    BKITS_CONSTEXPR20 auto operator++() -> basic_appender& { return *this; }
    BKITS_CONSTEXPR20 auto operator++(int) -> basic_appender { return *this; }
};

template <typename T>
struct is_back_insert_interator<basic_appender<T>> : std::true_type {};

template <typename OutputIt, typename InputIt, typename = void>
struct has_back_insert_iterator_container_append : std::false_type {};

template <typename OutputIt, typename InputIt>
struct has_back_insert_iterator_container_append<
    OutputIt, InputIt,
    void_t<decltype(get_container(std::declval<OutputIt>())
                        .append(std::declval<InputIt>(),
                                std::declval<InputIt>()))>> : std::true_type {};

template <typename T, typename InputIt, typename OutputIt,
        BKITS_ENABLE_IF(is_back_insert_interator<OutputIt>::value &&
                            has_back_insert_iterator_container_append<
                                OutputIt, InputIt>::value)>
BKITS_CONSTEXPR20 auto copy(InputIt begin, InputIt end, OutputIt out)
    -> OutputIt {
    get_container(out).append(begin, end);
    return out;
}

template <typename T, typename InputIt, typename OutputIt,
        BKITS_ENABLE_IF(is_back_insert_interator<OutputIt>::value &&
                        !has_back_insert_iterator_container_append<
                            OutputIt, InputIt>::value)>
BKITS_CONSTEXPR20 auto copy(InputIt begin, InputIt end, OutputIt out)
    -> OutputIt {
    auto& c = get_container(out);
    c.insert(c.end(), begin, end);
    return out;
}

template <typename T, typename InputIt, typename OutputIt,
        BKITS_ENABLE_IF(!is_back_insert_interator<OutputIt>::value)>
BKITS_CONSTEXPR auto copy(InputIt begin, InputIt end, OutputIt out) -> OutputIt {
    while (begin != end) *out++ = static_cast<T>(*begin++);
    return out;
}

template <typename T, typename V, typename OutputIt>
BKITS_CONSTEXPR auto copy(basic_string_view<V> s, OutputIt out) -> OutputIt {
    return copy<T>(s.begin(), s.end(), out);
}

} // namespace detail
BKITS_END_NAMESPACE
// clang-format on

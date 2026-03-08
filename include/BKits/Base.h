#pragma once

// clang-format off

#define BKITS_BEGIN_NAMESPACE   \
    namespace BKits {

#define BKITS_END_NAMESPACE     \
    }

// Detect compiler versions.
#if defined(__clang__) && !defined(__ibmxl__)
#  define BKITS_CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#else
#  define BKITS_CLANG_VERSION 0
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#  define BKITS_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#  define BKITS_GCC_VERSION 0
#endif

#if defined(__ICL)
#  define BKITS_ICC_VERSION __ICL
#elif defined(__INTEL_COMPILER)
#  define BKITS_ICC_VERSION __INTEL_COMPILER
#else
#  define BKITS_ICC_VERSION 0
#endif

#ifdef _MSVC_LANG
#  define BKITS_CPLUSPLUS _MSVC_LANG
#else
#  define BKITS_CPLUSPLUS __cplusplus
#endif

// Detect standard library versions.
#ifdef _GLIBCXX_RELEASE
#  define BKITS_GLIBCXX_RELEASE _GLIBCXX_RELEASE
#else
#  define BKITS_GLIBCXX_RELEASE 0
#endif

#ifdef _LIBCPP_VERSION
#  define BKITS_LIBCPP_VERSION _LIBCPP_VERSION
#else
#  define BKITS_LIBCPP_VERSION 0
#endif

#if defined(_MSC_VER)
#  define BKITS_MSC_VERSION _MSC_VER
#else
#  define BKITS_MSC_VERSION 0
#endif

// Detect __has_*.
#ifdef __has_feature
#  define BKITS_HAS_FEATURE(x) __has_feature(x)
#else
#  define BKITS_HAS_FEATURE(x) 0
#endif

#ifdef __has_include
#  define BKITS_HAS_INCLUDE(x) __has_include(x)
#else
#  define BKITS_HAS_INCLUDE(x) 0
#endif

#ifdef __has_builtin
#  define BKITS_HAS_BUILTIN(x) __has_builtin(x)
#else
#  define BKITS_HAS_BUILTIN(x) 0
#endif

#ifdef __has_cpp_attribute
#  define BKITS_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define BKITS_HAS_CPP_ATTRIBUTE(x) 0
#endif

#define BKITS_HAS_CPP14_ATTRIBUTE(attribute) \
    (BKITS_CPLUSPLUS >= 201402L && BKITS_HAS_CPP_ATTRIBUTE(attribute))

#define BKITS_HAS_CPP17_ATTRIBUTE(attribute) \
    (BKITS_CPLUSPLUS >= 201703L && BKITS_HAS_CPP_ATTRIBUTE(attribute))

#ifdef BKITS_USE_CONSTEXPR
#elif BKITS_GCC_VERSION >= 702 && BKITS_CPLUSPLUS >= 201402L
#  define BKITS_USE_CONSTEXPR 1
#elif BKITS_ICC_VERSION
#  define BKITS_USE_CONSTEXPR 0  
#elif BKITS_HAS_FEATURE(cxx_relaxed_constexpr) || BKITS_MSC_VERSION >= 1912
#  define BKITS_USE_CONSTEXPR 1
#else
#  define BKITS_USE_CONSTEXPR 0
#endif

#if !defined(__cpp_lib_is_constant_evaluated)
#  define BKITS_USE_CONSTEVAL 0
#elif BKITS_CPLUSPLUS < 201709L
#  define BKITS_USE_CONSTEVAL 0
#elif BKITS_GLIBCXX_RELEASE && BKITS_GLIBCXX_RELEASE < 10
#  define BKITS_USE_CONSTEVAL 0
#elif BKITS_LIBCPP_VERSION && BKITS_LIBCPP_VERSION < 10000
#  define BKITS_USE_CONSTEVAL 0
#elif defined(__apple_build_version__) && __apple_build_version__ < 14000029L
#  define BKITS_USE_CONSTEVAL 0
#elif BKITS_MSC_VERSION && BKITS_MSC_VERSION < 1929
#  define BKITS_USE_CONSTEVAL 0
#elif defined(__cpp_consteval)
#  define BKITS_USE_CONSTEVAL 1
#elif BKITS_GCC_VERSION >= 1002 || BKITS_CLANG_VERSION >= 1101
#  define BKITS_USE_CONSTEVAL 1
#else
#  define BKITS_USE_CONSTEVAL 0
#endif

#if BKITS_USE_CONSTEVAL
#  define BKITS_CONSTEVAL consteval
#  define BKITS_CONSTEXPR20 constexpr
#else
#  define BKITS_CONSTEVAL
#  define BKITS_CONSTEXPR20
#endif

#if BKITS_USE_CONSTEXPR
#  define BKITS_CONSTEXPR constexpr
#else
#  define BKITS_CONSTEXPR
#endif

#ifdef BKITS_USE_EXCEPTIONS
// 
#else
#  define BKITS_USE_EXCEPTIONS 1
#endif

#if BKITS_USE_EXCEPTIONS
#  define BKITS_TRY try
#  define BKITS_CATCH(x) catch(x)
#else
#  define BKITS_TRY if (true)
#  define BKITS_CATCH(x) if (false)
#endif

#ifdef BKITS_ALWAYS_INLINE
#elif BKITS_GCC_VERSION || BKITS_CLANG_VERSION
#  define BKITS_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#  define BKITS_ALWAYS_INLINE inline
#endif

#if defined(NDEBUG) || defined(BKITS_GCC_OPTIMIZED)
#  define BKITS_INLINE BKITS_ALWAYS_INLINE
#else
#  define BKITS_INLINE inline
#endif

#ifdef _WIN32
#  define BKITS_WIN32 1
#else
#  define BKITS_WIN32 0
#endif

enum { is_utf8_enabled = "\u00A7"[1] == '\xA7' };
enum { use_utf8 = !BKITS_WIN32 || is_utf8_enabled };

#ifndef BKITS_UNICODE 
#  define BKITS_UNICODE 1
#endif

static_assert(!BKITS_UNICODE || use_utf8, "Unicode support requires compiling with /utf-8");

// clang-format on
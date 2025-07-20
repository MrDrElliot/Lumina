#pragma once

#ifndef REFLECTION_PARSER

/** Branch prediction hints */
#ifndef LIKELY						/* Hints compiler that expression is likely to be true */
    #if ( defined(__clang__) || defined(__GNUC__) ) && (PLATFORM_UNIX)	// effect of these on non-Linux platform has not been analyzed as of 2016-03-21
        #define LIKELY(x)			__builtin_expect(!!(x), 1)
    #else
        // the additional "!!" is added to silence "warning: equality comparison with exteraneous parenthese" messages on android
        #define LIKELY(x)			(!!(x))
    #endif
#endif

#ifndef UNLIKELY					/* Hints compiler that expression is unlikely to be true, allows (penalized by worse performance) expression to be true */
    #if ( defined(__clang__) || defined(__GNUC__) ) && (PLATFORM_UNIX)	// effect of these on non-Linux platform has not been analyzed as of 2016-03-21
        #define UNLIKELY(x)			__builtin_expect(!!(x), 0)
    #else
        // the additional "!!" is added to silence "warning: equality comparison with exteraneous parenthese" messages on android
        #define UNLIKELY(x)			(!!(x))
    #endif
#endif

/* Macro wrapper for the consteval keyword which isn't yet present on all compilers - constexpr
   can be used as a workaround but is less strict and so may let some non-consteval code pass */
#if defined(__cpp_consteval)
    #define LE_CONSTEVAL consteval
#else
    #define LE_CONSTEVAL constexpr
#endif

/** Promise expression is true. Compiler can optimize accordingly with undefined behavior if wrong. Static analyzers understand this.  */
#ifndef ASSUME
    #if defined(__clang__)
        #define ASSUME(x) __builtin_assume(x)
    #elif defined(_MSC_VER)
        #define ASSUME(x) __assume(x)
    #else
        #define ASSUME(x)
    #endif
#endif

#define NODISCARD [[nodiscard]]

// If we don't have a platform-specific define for the TEXT macro, define it now.
#if !defined(TEXT)
    #if PLATFORM_TCHAR_IS_UTF8CHAR
        #define TEXT_PASTE(x) UTF8TEXT(x)
    #else
        #define TEXT_PASTE(x) WIDETEXT(x)
    #endif
    #define TEXT(x) TEXT_PASTE(x)
#endif


#define UTF8TEXT_PASTE(x)  u8 ## x
#define UTF16TEXT_PASTE(x) u ## x
#if PLATFORM_WIDECHAR_IS_CHAR16
    #define WIDETEXT_PASTE(x)  UTF16TEXT_PASTE(x)
#else
    #define WIDETEXT_PASTE(x)  L ## x
#endif

#define UTF16TEXT(x) UTF16TEXT_PASTE(x)
#define WIDETEXT(str) WIDETEXT_PASTE(str)

#endif
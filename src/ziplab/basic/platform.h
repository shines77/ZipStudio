
#ifndef ZIPLAB_BASIC_PLATFORM_H
#define ZIPLAB_BASIC_PLATFORM_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "ziplab/config/config.h"

//
// Arch
//
#if defined(_M_X64) || defined(_M_IX64) || defined(_M_AMD64) \
 || defined(__amd64__) || defined(__x86_64__) \
 || defined(__ia64) || defined(__ia64__) || defined(__IA64__) \
 || defined(__itanium__) || defined(_M_IA64)
  #define ZIPLAB_IS_X86           1
  #define ZIPLAB_IS_X86_64        1
  #define ZIPLAB_WORD_LEN         64
#elif defined (_M_IX86) || defined(__i386__) \
 || defined(_X86_) || defined(__ia86__) || defined(i386)
  #define ZIPLAB_IS_X86           1
  #define ZIPLAB_IS_X86_I386      1
  #define ZIPLAB_WORD_LEN         32
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM64__) || defined(__arm64__) \
  || defined(__arm64) || defined(__AARCH64EL__)
  #define ZIPLAB_IS_ARM           1
  #define ZIPLAB_IS_ARM64         1
  #define ZIPLAB_WORD_LEN         64
#elif defined(__aarch32__) || defined(_M_ARM) || defined(__ARM__) || defined(__arm__) || \
    defined(__ARM_ARCH) || defined(__TARGET_ARCH_ARM) || \
    defined(__thumb__) || defined(__TARGET_ARCH_THUMB) || \
    defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__) || \
    defined(__ARM_ARCH_4T__) || defined(__ARM_ARCH_4__)
  #define ZIPLAB_IS_ARM           1
  #define ZIPLAB_IS_ARM32         1
  #define ZIPLAB_WORD_LEN         32
#elif defined(_M_MPPC)
  // Power Macintosh PowerPC
  #define ZIPLAB_WORD_LEN         32
#elif defined(_M_PPC)
  // PowerPC
  #define ZIPLAB_WORD_LEN         32
#endif

#ifndef ZIPLAB_IS_ARM
#if defined(__ARM_ARCH) || defined(__TARGET_ARCH_ARM) || \
    defined(__TARGET_ARCH_THUMB) || defined(_M_ARM) || \
    defined(__arm__) || defined(__arm64) || defined(__thumb__) || \
    defined(_M_ARM64) || defined(__aarch64__) || defined(__AARCH64EL__) || \
    defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__) || \
    defined(__ARM_ARCH_4T__) || defined(__ARM_ARCH_4__)
#define ZIPLAB_IS_ARM   1
#else
#define ZIPLAB_IS_ARM   0
#endif
#endif // !ZIPLAB_IS_ARM

#ifndef ZIPLAB_WORD_LEN
  #if defined(WIN32) || defined(_WIN32)
    #define ZIPLAB_WORD_LEN       32
  #elif defined(WIN64) || defined(_WIN64)
    #define ZIPLAB_WORD_LEN       64
  #elif defined(__64BIT__) || defined(_LP64) || defined(__LP64__) || (SIZE_MAX == UINT64_MAX)
    #define ZIPLAB_WORD_LEN       64
  #else
    #define ZIPLAB_WORD_LEN       (sizeof(size_t) * 8)
  #endif
#endif // !ZIPLAB_WORD_LEN

/**
 Config of jstd project, per target platform.
 */

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which jstd uses.
#define ZIPLAB_OS_UNKNOWN             0
// Windows family
#define ZIPLAB_OS_WIN32               1
#define ZIPLAB_OS_WIN64               2
#define ZIPLAB_OS_WINCE               3
#define ZIPLAB_OS_WINRT               4
#define ZIPLAB_OS_WP8                 5
// Linux family
#define ZIPLAB_OS_UNIX                20
#define ZIPLAB_OS_LINUX               21
// Apple family
#define ZIPLAB_OS_MAC                 40
#define ZIPLAB_OS_IOS                 41
// Mobile family
#define ZIPLAB_OS_ANDROID             50
#define ZIPLAB_OS_BLACKBERRY          60
// Other systems
#define ZIPLAB_OS_MARMALADE           100
#define ZIPLAB_OS_BADA                101
#define ZIPLAB_OS_NACL                102
#define ZIPLAB_OS_EMSCRIPTEN          103
#define ZIPLAB_OS_TIZEN               104

// Determine target platform by compile environment macro.
#undef  ZIPLAB_TARGET_OS
#define ZIPLAB_TARGET_OS              ZIPLAB_OS_UNKNOWN

#ifndef ZIPLAB_OS_NAME
#define ZIPLAB_OS_NAME(X)             ZIPLAB_OS_##X
#endif

#ifndef ZIPLAB_CHECK_OS
    #define ZIPLAB_CHECK_OS(X)        ((ZIPLAB_OS_##X) && (ZIPLAB_TARGET_OS == ZIPLAB_OS_##X))
#endif // ZIPLAB_CHECK_OS

// marmalade
#if defined(MARMALADE)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_MARMALADE
    #ifndef ZIPLAB_IS_MARMALADE
    #define ZIPLAB_IS_MARMALADE       1
    #endif
#endif

// bada
#if defined(SHP)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_BADA
    #ifndef ZIPLAB_IS_BADA
    #define ZIPLAB_IS_BADA            1
    #endif
#endif

// native client
#if defined(__native_client__)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_NACL
    #ifndef ZIPLAB_IS_NACL
    #define ZIPLAB_IS_NACL            1
    #endif
#endif

// Emscripten
#if defined(EMSCRIPTEN)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_EMSCRIPTEN
    #ifndef ZIPLAB_IS_EMSCRIPTEN
    #define ZIPLAB_IS_EMSCRIPTEN      1
    #endif
#endif

// tizen
#if defined(TIZEN)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_TIZEN
    #ifndef ZIPLAB_IS_TIZEN
    #define ZIPLAB_IS_TIZEN           1
    #endif
#endif

// qnx
#if defined(__QNX__)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_BLACKBERRY
    #ifndef ZIPLAB_IS_BLACKBERRY
    #define ZIPLAB_IS_BLACKBERRY      1
    #endif
#endif

// android
#if defined(ANDROID)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_ANDROID
    #ifndef ZIPLAB_IS_ANDROID
    #define ZIPLAB_IS_ANDROID         1
    #endif
#endif

// mac
#if defined(__APPLE__) || defined(__APPLE_CC__)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_MAC
    #ifndef ZIPLAB_IS_OS_MAC
    #define ZIPLAB_IS_OS_MAC          1
    #endif
#endif

// iphone
#if defined(ZIPLAB_TARGET_OS_IPHONE)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_IOS
    #ifndef ZIPLAB_IS_OS_IPHONE
    #define ZIPLAB_IS_OS_IPHONE       1
    #endif
#endif

// ipad, perhaps this precompiled entry is invalid.
#if defined(ZIPLAB_TARGET_OS_IPAD)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_IOS
    #ifndef ZIPLAB_IS_OS_IPAD
    #define ZIPLAB_IS_OS_IPAD         1
    #endif
#endif

// linux
#if defined(__linux__) || defined(__linux) || defined(LINUX)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_LINUX
    // ZIPLAB_IS_LINUX will be defined below
    #ifndef ZIPLAB_IS_OS_LINUX
    #define ZIPLAB_IS_OS_LINUX        1
    #endif
#elif defined(__unix__) || defined(__unix) || defined(UNIX)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_UNIX
    // ZIPLAB_IS_UNIX will be defined below
    #ifndef ZIPLAB_IS_OS_UNIX
    #define ZIPLAB_IS_OS_UNIX         1
    #endif
#endif

// WinCE (Windows CE)
#if defined(WCE) || defined(_WCE) || defined(WINCE) || defined(_WIN32_WCE)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_WINCE
    #ifndef ZIPLAB_IS_WINCE
    #define ZIPLAB_IS_WINCE           1
    #endif
#endif

// WinRT (Windows Store App)
#if defined(WINRT) || defined(_WINRT)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_WINRT
    #ifndef ZIPLAB_IS_WINRT
    #define ZIPLAB_IS_WINRT           1
    #endif
#endif

// WP8 (Windows Phone 8 App)
#if defined(WP8) || defined(_WP8)
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_WP8
    #ifndef ZIPLAB_IS_WP8
    #define ZIPLAB_IS_WP8             1
    #endif
#endif

#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
    // win64
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_WIN64
    #ifndef ZIPLAB_IS_WIN64
    #define ZIPLAB_IS_WIN64           1
    #endif
#elif defined(WIN32) || defined(_WIN32) || defined(_WINDOWS) || defined(__WIN32__) || defined(__NT__)
    // win32
    #undef  ZIPLAB_TARGET_OS
    #define ZIPLAB_TARGET_OS          ZIPLAB_OS_WIN32
    #ifndef ZIPLAB_IS_WIN32
    #define ZIPLAB_IS_WIN32           1
    #endif
#endif

#if (defined(ZIPLAB_IS_WIN32) || defined(ZIPLAB_IS_WIN64)) && !(defined(__GNUC__) || defined(__MINGW__) \
    || defined(__MINGW32__) || defined(__MINGW64__))
#ifndef ZIPLAB_IS_WIN
#define ZIPLAB_IS_WIN                 (ZIPLAB_CHECK_OS(WIN32) || ZIPLAB_CHECK_OS(WIN64))
#endif
#endif

#if defined(ZIPLAB_IS_WIN32) || defined(ZIPLAB_IS_WIN64) || defined(ZIPLAB_IS_WINCE) \
 || defined(ZIPLAB_IS_WINRT) || defined(ZIPLAB_IS_WP8)
#ifndef ZIPLAB_IS_WINFAMILY
#define ZIPLAB_IS_WINFAMILY           (ZIPLAB_CHECK_OS(WIN32) || ZIPLAB_CHECK_OS(WIN64) \
                                  || ZIPLAB_CHECK_OS(WINCE) || ZIPLAB_CHECK_OS(WINRT) \
                                  || ZIPLAB_CHECK_OS(WP8))
#endif
#endif

#if defined(ZIPLAB_IS_OS_UNIX)
#ifndef ZIPLAB_IS_UNIX
#define ZIPLAB_IS_UNIX                ZIPLAB_CHECK_OS(UNIX)
#endif
#endif

#if defined(ZIPLAB_IS_OS_LINUX)
#ifndef ZIPLAB_IS_LINUX
#define ZIPLAB_IS_LINUX               ZIPLAB_CHECK_OS(LINUX)
#endif
#endif

#if defined(ZIPLAB_IS_OS_IPHONE) || defined(ZIPLAB_IS_OS_IPAD)
#ifndef ZIPLAB_IS_IOS
#define ZIPLAB_IS_IOS                 ZIPLAB_CHECK_OS(IOS)
#endif
#endif

#if defined(ZIPLAB_IS_OS_MAC)
#ifndef ZIPLAB_IS_MAC
#define ZIPLAB_IS_MAC                 ZIPLAB_CHECK_OS(MAC)
#endif
#endif

#if defined(ZIPLAB_IS_IOS) || defined(ZIPLAB_IS_MAC)
#ifndef ZIPLAB_IS_APPLE
#define ZIPLAB_IS_APPLE               (ZIPLAB_CHECK_OS(IOS) || ZIPLAB_CHECK_OS(MAC))
#endif
#endif

// for DOXYGEN
#if defined(DOXYGEN)
  #ifndef ZIPLAB_IS_DOXYGEN
    #define ZIPLAB_IS_DOXYGEN         1
  #endif
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if (!defined(ZIPLAB_TARGET_OS)) || (ZIPLAB_TARGET_OS == ZIPLAB_OS_UNKNOWN)
    #error "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif

#if (ZIPLAB_TARGET_OS == ZIPLAB_OS_WIN32 || ZIPLAB_TARGET_OS == ZIPLAB_OS_WIN64)
#pragma warning (disable:4127)
#pragma warning (disable:4996)
#endif // ZIPLAB_OS_WIN32

#endif // ZIPLAB_BASIC_PLATFORM_H

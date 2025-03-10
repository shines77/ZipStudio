#ifndef ZIPLAB_BASIC_STDDEF_H
#define ZIPLAB_BASIC_STDDEF_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "ziplab/config/config.h"

////////////////////////////////////////////////////////////////////////////////

#if __is_identifier(__wchar_t)
  // __wchar_t is not a reserved keyword
  #if !defined(_MSC_VER)
    typedef wchar_t __wchar_t;
  #endif // !_MSC_VER
#endif

////////////////////////////////////////////////////////////////////////////////
//
// C++ compiler macro define
// See: http://www.cnblogs.com/zyl910/archive/2012/08/02/printmacro.html
//

//
// Using unlikely/likely macros from boost?
// See: http://www.boost.org/doc/libs/1_60_0/boost/config/compiler/clang.hpp
//
// See:
//      http://www.boost.org/doc/libs/1_60_0/boost/config/compiler/gcc.hpp10
//      http://www.boost.org/doc/libs/1_60_0/boost/config/compiler/clang.hpp4
//      http://www.boost.org/doc/libs/1_60_0/boost/config/compiler/intel.hpp2
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// LLVM Branch Weight Metadata
// See: http://llvm.org/docs/BranchWeightMetadata.html
//
////////////////////////////////////////////////////////////////////////////////

//
// Since gcc 2.96 or Intel C++ compiler 8.0
//
// I'm not sure Intel C++ compiler 8.0 was the first version to support these builtins,
// update the condition if the version is not accurate. (Andrey Semashev)
//
#if (defined(__GNUC__) && ((__GNUC__ == 2 && __GNUC_MINOR__ >= 96) || (__GNUC__ >= 3))) \
    || ((defined(__GNUC__) && (__INTEL_CXX_VERSION >= 800)))
  #define ZIPLAB_SUPPORT_LIKELY        1
#elif defined(__clang__)
  //
  // clang: GCC extensions not implemented yet
  // See: http://clang.llvm.org/docs/UsersManual.html#gcc-extensions-not-implemented-yet
  //
  #if defined(__has_builtin)
    #if __has_builtin(__builtin_expect)
      #define ZIPLAB_SUPPORT_LIKELY    1
    #endif // __has_builtin(__builtin_expect)
  #endif // defined(__has_builtin)
#endif // ZIPLAB_SUPPORT_LIKELY

//
// Sample: since clang 3.4
//
#if defined(__clang__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 4))
    // This is a sample macro.
#endif

//
// Branch prediction hints
//
#if defined(ZIPLAB_SUPPORT_LIKELY) && (ZIPLAB_SUPPORT_LIKELY != 0)
#ifndef ziplab_likely
#define ziplab_likely(expr)             __builtin_expect(!!(expr), 1)
#endif
#ifndef ziplab_unlikely
#define ziplab_unlikely(expr)           __builtin_expect(!!(expr), 0)
#endif
#ifndef ziplab_switch_likely
#define ziplab_switch_likely(expr, v)   __builtin_expect(!!(expr), (v))
#endif
#else
#ifndef ziplab_likely
#define ziplab_likely(expr)             (expr)
#endif
#ifndef ziplab_unlikely
#define ziplab_unlikely(expr)           (expr)
#endif
#ifndef ziplab_switch_likely
#define ziplab_switch_likely(expr, v)   (expr)
#endif
#endif // likely() & unlikely()

#if defined(ZIPLAB_SUPPORT_LIKELY) && (ZIPLAB_SUPPORT_LIKELY != 0)
#ifndef ZIPLAB_LIKELY
#define ZIPLAB_LIKELY(expr)             __builtin_expect(!!(expr), 1)
#endif
#ifndef ZIPLAB_UNLIKELY
#define ZIPLAB_UNLIKELY(expr)           __builtin_expect(!!(expr), 0)
#endif
#ifndef ZIPLAB_SWITCH_LIKELY
#define ZIPLAB_SWITCH_LIKELY(expr, v)   __builtin_expect(!!(expr), (v))
#endif
#else
#ifndef ZIPLAB_LIKELY
#define ZIPLAB_LIKELY(expr)             (expr)
#endif
#ifndef ZIPLAB_UNLIKELY
#define ZIPLAB_UNLIKELY(expr)           (expr)
#endif
#ifndef ZIPLAB_SWITCH_LIKELY
#define ZIPLAB_SWITCH_LIKELY(expr, v)   (expr)
#endif
#endif // ZIPLAB_LIKELY() & ZIPLAB_UNLIKELY()

//
// From: https://hackage.haskell.org/package/LibClang-3.4.0/src/llvm/include/llvm/Support/Compiler.h
//
// __builtin_assume_aligned() is support by GCC >= 4.7 and clang >= 3.6.
//

/// \macro ZIPLAB_ASSUME_ALIGNED
/// \brief Returns a pointer with an assumed alignment.
#if __has_builtin(__builtin_assume_aligned) && __CLANG_PREREQ(3, 6)
# define ZIPLAB_ASSUME_ALIGNED(ptr, alignment)  __builtin_assume_aligned((ptr), (alignment))
#elif __has_builtin(__builtin_assume_aligned) && __GNUC_PREREQ(4, 7)
# define ZIPLAB_ASSUME_ALIGNED(ptr, alignment)  __builtin_assume_aligned((ptr), (alignment))
#elif defined(LLVM_BUILTIN_UNREACHABLE)
// Clang 3.4 does not support __builtin_assume_aligned().
# define ZIPLAB_ASSUME_ALIGNED(ptr, alignment) \
           (((uintptr_t(ptr) % (alignment)) == 0) ? (ptr) : (LLVM_BUILTIN_UNREACHABLE, (ptr)))
#elif defined(__GNUC__) || __has_builtin(__builtin_unreachable)
// If gcc does not support __builtin_assume_aligned().
# define ZIPLAB_ASSUME_ALIGNED(ptr, alignment) \
           (((uintptr_t(ptr) % (alignment)) == 0) ? (ptr) : (__builtin_unreachable, (ptr)))
#else
# define ZIPLAB_ASSUME_ALIGNED(ptr, alignment)  ((void *)(ptr))
#endif // ZIPLAB_ASSUME_ALIGNED

//
// From boost::unordered
//
#if !defined(NDEBUG)
#define ZIPLAB_ASSUME(cond)   assert(cond)
#elif __has_builtin(__builtin_assume)
#define ZIPLAB_ASSUME(cond)   __builtin_assume(cond)
#elif defined(__GNUC__) || __has_builtin(__builtin_unreachable)
#define ZIPLAB_ASSUME(cond)                       \
    do {                                        \
        if (!(cond)) __builtin_unreachable();   \
    } while (0)
#elif defined(_MSC_VER)
#define ZIPLAB_ASSUME(cond)   __assume(cond)
#else
#define ZIPLAB_ASSUME(cond)                   \
    do {                                    \
        static_cast<void>(false &&(cond));  \
    } while (0)
#endif // ZIPLAB_ASSUME

// Thread sanitizer
#ifndef ZIPLAB_THREAD_SANITIZER
#if __has_feature(thread_sanitizer) || defined(__SANITIZE_THREAD__)
#define ZIPLAB_THREAD_SANITIZER
#endif
#endif // ZIPLAB_THREAD_SANITIZER

#if defined(__GNUC__) && !defined(__GNUC_STDC_INLINE__) && !defined(__GNUC_GNU_INLINE__)
  #define __GNUC_GNU_INLINE__   1
#endif

/**
 * For inline, force-inline and no-inline define.
 */
#ifndef ZIPLAB_FORCED_INLINE
#  if defined(_MSC_VER)
#    define ZIPLAB_FORCED_INLINE  __forceinline
#  elif defined(__GNUC__) && (__GNUC__ > 3)
     // Clang also defines __GNUC__ (as 4)
#    define ZIPLAB_FORCED_INLINE  inline __attribute__((__always_inline__))
#  else
#    define ZIPLAB_FORCED_INLINE  inline
#  endif
#endif // ZIPLAB_FORCED_INLINE

#ifndef ZIPLAB_NO_INLINE
#  if defined(_MSC_VER)
#    define ZIPLAB_NO_INLINE      __declspec(noinline)
#  elif defined(__GNUC__) && __GNUC__ > 3
     // Clang also defines __GNUC__ (as 4)
#    if defined(__CUDACC__)
       // nvcc doesn't always parse __noinline__,
       // see: https://svn.boost.org/trac/boost/ticket/9392
#      define ZIPLAB_NO_INLINE    __attribute__((noinline))
#    elif defined(__HIP__)
       // See https://github.com/boostorg/config/issues/392
#      define ZIPLAB_NO_INLINE    __attribute__((noinline))
#    else
#      define ZIPLAB_NO_INLINE    __attribute__((__noinline__))
#    endif
#  else
#    define ZIPLAB_NO_INLINE
#  endif
#endif // ZIPLAB_NO_INLINE

#if defined(_MSC_VER) && !defined(__clang__)

#define ZIPLAB_INLINE             __inline
//#define ZIPLAB_FORCED_INLINE      __forceinline
//#define ZIPLAB_NO_INLINE          __declspec(noinline)

#define ZIPLAB_RESTRICT           __restrict

#elif (defined(__GNUC__) || defined(__clang__))

#define ZIPLAB_INLINE             inline __attribute__((gnu_inline))
//#define ZIPLAB_FORCED_INLINE      inline __attribute__((always_inline))
//#define ZIPLAB_NO_INLINE          __attribute__((noinline))

#define ZIPLAB_RESTRICT           __restrict__

#else // Unknown compiler

#define ZIPLAB_INLINE             inline
//#define ZIPLAB_FORCED_INLINE      inline
//#define ZIPLAB_NO_INLINE

#define ZIPLAB_RESTRICT

#endif // _MSC_VER

//
// Aligned prefix and suffix declare
//
#if defined(_MSC_VER)
#ifndef ZIPLAB_ALIGNED_PREFIX
#define ZIPLAB_ALIGNED_PREFIX(n)       __declspec(align(n))
#endif
#ifndef ZIPLAB_ALIGNED_SUFFIX
#define ZIPLAB_ALIGNED_SUFFIX(n)
#endif
#else
#ifndef ZIPLAB_ALIGNED_PREFIX
#define ZIPLAB_ALIGNED_PREFIX(n)
#endif
#ifndef ZIPLAB_ALIGNED_SUFFIX
#define ZIPLAB_ALIGNED_SUFFIX(n)       __attribute__((aligned(n)))
#endif
#endif // ALIGNED(n)

/*********************************************************************************
    GCC:

    uint32_t fill_cache(void) __attribute__((naked)); // Declaration
    attribute should be specified in declaration not in implementation

    uint32_t fill_cache(void)
    {
        __asm__ ("addi 3, 0, 0\n");  // R3 = 0
        // More asm here modifying R3 and filling the cache lines.
    }

*********************************************************************************/

#if defined(_MSC_VER) && !defined(__clang__)
#define ZIPLAB_NAKED_DECL_PREFIX       __declspec(naked)
#define ZIPLAB_NAKED_DECL_SUFFIX
#elif defined(__attribute__)
#define ZIPLAB_NAKED_DECL_PREFIX
#define ZIPLAB_NAKED_DECL_SUFFIX       __attribute__((naked))
#else
#define ZIPLAB_NAKED_DECL_PREFIX
#define ZIPLAB_NAKED_DECL_SUFFIX
#endif

#ifndef ZIPLAB_CDECL
#define ZIPLAB_CDECL
#if defined(_MSC_VER) && !defined(__clang__)
#define ZIPLAB_CDECL_PREFIX     __cdecl
#define ZIPLAB_CDECL_SUFFIX
#else
#define ZIPLAB_CDECL_PREFIX
#define ZIPLAB_CDECL_SUFFIX     __attribute__((__cdecl__))
#endif
#endif // ZIPLAB_CDECL

#if !defined(__cplusplus) || (defined(_MSC_VER) && (_MSC_VER < 1400))
  #ifndef nullptr
    #define nullptr     ((void *)NULL)
  #endif
#endif // __cplusplus

#ifndef ZIPLAB_UNUSED
#define ZIPLAB_UNUSED(var) \
    do { \
        (void)var; \
    } while (0)
#endif

#define STD_IOS_RIGHT(width, var) \
    std::right << std::setw(width) << (var)

#define STD_IOS_LEFT(width, var) \
    std::left << std::setw(width) << (var)

#define STD_IOS_DEFAULT() \
    std::left << std::setw(0)

#ifndef ZIPLAB_ASSERT
#ifdef _DEBUG
#define ZIPLAB_ASSERT(express)            assert(!!(express))
#else
#define ZIPLAB_ASSERT(express)            ((void)0)
#endif
#endif // ZIPLAB_ASSERT

#ifndef ZIPLAB_ASSERT_EX
#ifdef _DEBUG
#define ZIPLAB_ASSERT_EX(express, text)   assert(!!(express))
#else
#define ZIPLAB_ASSERT_EX(express, text)   ((void)0)
#endif
#endif // ZIPLAB_ASSERT

#ifndef ZIPLAB_STATIC_ASSERT
#if (__cplusplus < 201103L) || (defined(_MSC_VER) && (_MSC_VER < 1800))
#define ZIPLAB_STATIC_ASSERT(express, text)       assert(!!(express))
#else
#define ZIPLAB_STATIC_ASSERT(express, text)       static_assert(!!(express), text)
#endif
#endif // ZIPLAB_STATIC_ASSERT

//
// Little-Endian or Big-Endian
//
#define ZIPLAB_LITTLE_ENDIAN  1234
#define ZIPLAB_BIG_ENDIAN     4321

/*
* My best guess at if you are big-endian or little-endian.
* This may need adjustment.
*/
#ifndef ZIPLAB_ENDIAN
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)) || \
    (defined(i386) || defined(__i386__) || defined(__i486__) || \
     defined(__i586__) || defined(__i686__) || defined(vax) || defined(MIPSEL) || \
     defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || \
     defined(__amd64__) || defined(__x86_64__) || defined (_M_IX86))
  #define ZIPLAB_ENDIAN  ZIPLAB_LITTLE_ENDIAN
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)) || \
      (defined(sparc) || defined(POWERPC) || defined(mc68000) || defined(sel))
  #define ZIPLAB_ENDIAN  ZIPLAB_BIG_ENDIAN
#else
  #define ZIPLAB_ENDIAN  ZIPLAB_LITTLE_ENDIAN
#endif // __BYTE_ORDER
#endif // !ZIPLAB_ENDIAN

#endif // ZIPLAB_BASIC_STDDEF_H

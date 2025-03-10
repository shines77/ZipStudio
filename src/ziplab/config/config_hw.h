
#ifndef ZIPLAB_CONFIG_CONFIG_HW_H
#define ZIPLAB_CONFIG_CONFIG_HW_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "ziplab/config/config_hw_use.h"

//
// ZIPLAB_USE_HW_DETECT define in "ziplab/config/config_hw_use.h"
//
#if defined(ZIPLAB_USE_HW_DETECT) && (ZIPLAB_USE_HW_DETECT != 0)
#include "ziplab/config/config_hw_detect.h"
#endif

////////////////////////////////////////////////////////////////////////////////

// MMX

#ifdef ZIPLAB_HAVE_MMX
#error "ZIPLAB_HAVE_MMX" cannot be directly set
#elif defined(__MMX__) || defined(HAVE_MMX)
#define ZIPLAB_HAVE_MMX
#elif defined(_M_X64) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 1))
#define ZIPLAB_HAVE_MMX
#endif // ZIPLAB_HAVE_MMX

// SSE

//
// ZIPLAB_HAVE_SSE is used for compile-time detection of SSE support.
// See https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html for an overview of
// which architectures support the various x86 instruction sets.
//
#ifdef ZIPLAB_HAVE_SSE
#error "ZIPLAB_HAVE_SSE" cannot be directly set
#elif defined(__SSE__) || defined(HAVE_SSE)
#define ZIPLAB_HAVE_SSE
#elif defined(_M_X64) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 1))
//
// MSVC only defines _M_IX86_FP for x86 32-bit code, and _M_IX86_FP >= 1
// indicates that at least SSE was targeted with the /arch:SSE option.
// All x86-64 processors support SSE, so support can be assumed.
// https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
//
#define ZIPLAB_HAVE_SSE
#endif // ZIPLAB_HAVE_SSE

//
// ZIPLAB_HAVE_SSE2 is used for compile-time detection of SSE2 support.
// See https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html for an overview of
// which architectures support the various x86 instruction sets.
//
#ifdef ZIPLAB_HAVE_SSE2
#error "ZIPLAB_HAVE_SSE2" cannot be directly set
#elif defined(__SSE2__) || defined(HAVE_SSE2)
#define ZIPLAB_HAVE_SSE2
#elif defined(_M_X64) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
//
// MSVC only defines _M_IX86_FP for x86 32-bit code, and _M_IX86_FP >= 2
// indicates that at least SSE2 was targeted with the /arch:SSE2 option.
// All x86-64 processors support SSE2, so support can be assumed.
// https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
//
#define ZIPLAB_HAVE_SSE2
#endif // ZIPLAB_HAVE_SSE2

#ifdef ZIPLAB_HAVE_SSE3
#error "ZIPLAB_HAVE_SSE3" cannot be directly set
#elif defined(__SSE3__) || defined(HAVE_SSE3)
#define ZIPLAB_HAVE_SSE3
#endif

//
// ZIPLAB_HAVE_SSSE3 is used for compile-time detection of SSSE3 support.
// See https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html for an overview of
// which architectures support the various x86 instruction sets.
//
// MSVC does not have a mode that targets SSSE3 at compile-time. To use SSSE3
// with MSVC requires either assuming that the code will only every run on CPUs
// that support SSSE3, otherwise __cpuid() can be used to detect support at
// runtime and fallback to a non-SSSE3 implementation when SSSE3 is unsupported
// by the CPU.
//
#ifdef ZIPLAB_HAVE_SSSE3
#error "ZIPLAB_HAVE_SSSE3" cannot be directly set
#elif defined(__SSSE3__) || defined(HAVE_SSSE3)
#define ZIPLAB_HAVE_SSSE3
#endif

#ifdef ZIPLAB_HAVE_SSE4_1
#error "ZIPLAB_HAVE_SSE4_1" cannot be directly set
#elif defined(__SSE4_1__) || defined(HAVE_SSE4_1)
#define ZIPLAB_HAVE_SSE4_1
#endif

#ifdef ZIPLAB_HAVE_SSE4_2
#error "ZIPLAB_HAVE_SSE4_2" cannot be directly set
#elif defined(__SSE4_2__) || defined(HAVE_SSE4_2)
#define ZIPLAB_HAVE_SSE4_2
#endif

// AES

#ifdef ZIPLAB_HAVE_AES
#error "ZIPLAB_HAVE_AES" cannot be directly set
#elif defined(__AES__) || defined(HAVE_AES)
#define ZIPLAB_HAVE_AES
#endif

// AVX

#ifdef ZIPLAB_HAVE_AVX
#error "ZIPLAB_HAVE_AVX" cannot be directly set
#elif defined(__AVX__) || defined(HAVE_AVX)
#define ZIPLAB_HAVE_AVX
#endif

#ifdef ZIPLAB_HAVE_AVX2
#error "ZIPLAB_HAVE_AVX2" cannot be directly set
#elif defined(__AVX2__) || defined(HAVE_AVX2)
#define ZIPLAB_HAVE_AVX2
#endif

// FMA3

#ifdef ZIPLAB_HAVE_FMA3
#error "ZIPLAB_HAVE_FMA3" cannot be directly set
#elif defined(__FMA3__) || defined(HAVE_FMA3)
#define ZIPLAB_HAVE_FMA3
#endif

// AVX512

#ifdef ZIPLAB_HAVE_AVX512F
#error "ZIPLAB_HAVE_AVX512F" cannot be directly set
#elif defined(__AVX512F__) || defined(HAVE_AVX512F)
#define ZIPLAB_HAVE_AVX512F
#endif

#ifdef ZIPLAB_HAVE_AVX512VL
#error "ZIPLAB_HAVE_AVX512VL" cannot be directly set
#elif defined(__AVX512VL__) || defined(HAVE_AVX512VL)
#define ZIPLAB_HAVE_AVX512VL
#endif

#ifdef ZIPLAB_HAVE_AVX512CD
#error "ZIPLAB_HAVE_AVX512CD" cannot be directly set
#elif defined(__AVX512CD__) || defined(HAVE_AVX512CD)
#define ZIPLAB_HAVE_AVX512CD
#endif

#ifdef ZIPLAB_HAVE_AVX512ER
#error "ZIPLAB_HAVE_AVX512ER" cannot be directly set
#elif defined(__AVX512ER__) || defined(HAVE_AVX512ER)
#define ZIPLAB_HAVE_AVX512ER
#endif

#ifdef ZIPLAB_HAVE_AVX512PF
#error "ZIPLAB_HAVE_AVX512PF" cannot be directly set
#elif defined(__AVX512PF__) || defined(HAVE_AVX512PF)
#define ZIPLAB_HAVE_AVX512PF
#endif

#ifdef ZIPLAB_HAVE_AVX512BW
#error "ZIPLAB_HAVE_AVX512BW" cannot be directly set
#elif defined(__AVX512BW__) || defined(HAVE_AVX512BW)
#define ZIPLAB_HAVE_AVX512BW
#endif

#ifdef ZIPLAB_HAVE_AVX512DQ
#error "ZIPLAB_HAVE_AVX512DQ" cannot be directly set
#elif defined(__AVX512DQ__) || defined(HAVE_AVX512DQ)
#define ZIPLAB_HAVE_AVX512DQ
#endif

#ifdef ZIPLAB_HAVE_AVX512IFMA
#error "ZIPLAB_HAVE_AVX512IFMA" cannot be directly set
#elif defined(__AVX512IFMA__) || defined(HAVE_AVX512IFMA)
#define ZIPLAB_HAVE_AVX512IFMA
#endif

#ifdef ZIPLAB_HAVE_AVX512VBMI
#error "ZIPLAB_HAVE_AVX512VBMI" cannot be directly set
#elif defined(__AVX512VBMI__) || defined(HAVE_AVX512VBMI)
#define ZIPLAB_HAVE_AVX512VBMI
#endif

#ifdef ZIPLAB_HAVE_AVX512BF16
#error "ZIPLAB_HAVE_AVX512BF16" cannot be directly set
#elif defined(__AVX512BF16__) || defined(HAVE_AVX512BF16)
#define ZIPLAB_HAVE_AVX512BF16
#endif

#ifdef ZIPLAB_HAVE_AVX512FP16
#error "ZIPLAB_HAVE_AVX512FP16" cannot be directly set
#elif defined(__AVX512FP16__) || defined(HAVE_AVX512FP16)
#define ZIPLAB_HAVE_AVX512FP16
#endif

#ifdef ZIPLAB_HAVE_AVX512VNNI
#error "ZIPLAB_HAVE_AVX512VNNI" cannot be directly set
#elif defined(__AVX512VNNI__) || defined(HAVE_AVX512VNNI)
#define ZIPLAB_HAVE_AVX512VNNI
#endif

#ifdef ZIPLAB_HAVE_AVX512VPOPCNTDQ
#error "ZIPLAB_HAVE_AVX512VPOPCNTDQ" cannot be directly set
#elif defined(__AVX512VPOPCNTDQ__) || defined(HAVE_AVX512VPOPCNTDQ)
#define ZIPLAB_HAVE_AVX512VPOPCNTDQ
#endif

#ifdef ZIPLAB_HAVE_AVX512BITALG
#error "ZIPLAB_HAVE_AVX512BITALG" cannot be directly set
#elif defined(__AVX512BITALG__) || defined(HAVE_AVX512BITALG)
#define ZIPLAB_HAVE_AVX512BITALG
#endif

#ifdef ZIPLAB_HAVE_AVX512VP2INTERSECT
#error "ZIPLAB_HAVE_AVX512VP2INTERSECT" cannot be directly set
#elif defined(__AVX512VP2INTERSECT__) || defined(HAVE_AVX512VP2INTERSECT)
#define ZIPLAB_HAVE_AVX512VP2INTERSECT
#endif

#ifdef ZIPLAB_HAVE_AVX5124FMAPS
#error "ZIPLAB_HAVE_AVX5124FMAPS" cannot be directly set
#elif defined(__AVX5124FMAPS__) || defined(HAVE_AVX5124FMAPS)
#define ZIPLAB_HAVE_AVX5124FMAPS
#endif

#ifdef ZIPLAB_HAVE_AVX5124VNNIW
#error "ZIPLAB_HAVE_AVX5124VNNIW" cannot be directly set
#elif defined(__AVX5124VNNIW__) || defined(HAVE_AVX5124VNNIW)
#define ZIPLAB_HAVE_AVX5124VNNIW
#endif

#ifdef ZIPLAB_HAVE_AVX5124VBMI2
#error "ZIPLAB_HAVE_AVX5124VBMI2" cannot be directly set
#elif defined(__AVX5124VBMI2__) || defined(HAVE_AVX5124VBMI2)
#define ZIPLAB_HAVE_AVX5124VBMI2
#endif

#ifdef ZIPLAB_HAVE_AVX5124GFNI
#error "ZIPLAB_HAVE_AVX5124GFNI" cannot be directly set
#elif defined(__AVX5124GFNI__) || defined(HAVE_AVX5124GFNI)
#define ZIPLAB_HAVE_AVX5124GFNI
#endif

#ifdef ZIPLAB_HAVE_AVX5124VPCLMULQDQ
#error "ZIPLAB_HAVE_AVX5124VPCLMULQDQ" cannot be directly set
#elif defined(__AVX5124VPCLMULQDQ__) || defined(HAVE_AVX5124VPCLMULQDQ)
#define ZIPLAB_HAVE_AVX5124VPCLMULQDQ
#endif

//
// This is the only predef define needed for boost, so pull it
// out here so we don't need to include all of predef.
//
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
#define ZIPLAB_ARCH_ARM   1
#else
#define ZIPLAB_ARCH_ARM   0
#endif // ZIPLAB_ARCH_ARM

//
// ZIPLAB_HAVE_ARM_ACLE is used for compile-time detection of ACLE (ARM
// C language extensions).
//
#ifdef ZIPLAB_HAVE_ARM_ACLE
#error "ZIPLAB_HAVE_ARM_ACLE" cannot be directly set
//
// __cls, __rbit were added quite late in clang. They are not supported
// by GCC as well. __cls can be replaced with __builtin_clrsb but clang does
// not recognize cls instruction in latest versions.
// TODO(b/233604649): Relax to __builtin_clrsb and __builtin_bitreverse64 (note
// that the latter is not supported by GCC).
//
#elif (defined(__ARM_ACLE) || defined(HAVE_ARM_ACLE)) && defined(__clang__) &&  \
    __has_builtin(__builtin_arm_cls64) &&                                       \
    __has_builtin(__builtin_arm_rbit64)
#define ZIPLAB_HAVE_ARM_ACLE
#endif

//
// ZIPLAB_HAVE_ARM_NEON is used for compile-time detection of NEON (ARM SIMD).
//
#ifdef ZIPLAB_HAVE_ARM_NEON
#error "ZIPLAB_HAVE_ARM_NEON" cannot be directly set
#elif defined(__ARM_NEON) || defined(HAVE_ARM_NEON)
#define ZIPLAB_HAVE_ARM_NEON
#endif

////////////////////////////////////////////////////////////////////////////////

#endif // ZIPLAB_CONFIG_CONFIG_HW_H

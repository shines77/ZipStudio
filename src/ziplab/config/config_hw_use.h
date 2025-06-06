
#ifndef ZIPLAB_CONFIG_CONFIG_HW_USE_H
#define ZIPLAB_CONFIG_CONFIG_HW_USE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//
// Whether include "/config/config_hw_detect.h" header file ?
//
#define ZIPLAB_USE_HW_DETECT      1

//
// You can manually decide whether the SIMD instruction is enabled or not,
// even if the hardware supports that instruction set.
//
#define ZIPLAB_USE_MMX            1
#define ZIPLAB_USE_SSE            1
#define ZIPLAB_USE_SSE2           1
#define ZIPLAB_USE_SSE3           1
#define ZIPLAB_USE_SSSE3          1
#define ZIPLAB_USE_SSE4_1         1
#define ZIPLAB_USE_SSE4_2         1
#define ZIPLAB_USE_AVX            1
#define ZIPLAB_USE_AVX2           1
#define ZIPLAB_USE_AVX512         1

#define ZIPLAB_USE_F16C           1
#define ZIPLAB_USE_RDRND          1
#define ZIPLAB_USE_AES            1
#define ZIPLAB_USE_SHA            1

#endif // ZIPLAB_CONFIG_CONFIG_HW_CONFIG_H

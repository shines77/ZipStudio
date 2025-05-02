#ifndef ZIPLAB_JSTD_BITS_HPP
#define ZIPLAB_JSTD_BITS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <type_traits>
#include <limits.h>
#include <assert.h>

#include "ziplab/basic/stddef.h"

#if (defined(_MSC_VER) && (_MSC_VER >= 1500)) && !defined(__clang__) // >= MSVC 2008
#include <intrin.h>

#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)
#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || defined(_M_ARM64)
  #pragma intrinsic(_BitScanReverse64)
  #pragma intrinsic(_BitScanForward64)
#endif

#ifndef __POPCNT__
  #define __POPCNT__
#endif
#endif // (_MSC_VER && _MSC_VER >= 1500)

// defined(__GNUC__) && (__GNUC__ * 1000 + __GNUC_MINOR__ >= 4005)
#if defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
#include <x86intrin.h>
#endif

#if (jstd_cplusplus >= 2020L)
#include <bit>
#endif

//
// For SSE 4.2, _mm_popcnt_u32(), _mm_popcnt_u64(), etc.
//
#include "ziplab/arch/x86_intrin.h"

namespace jstd {

template <typename SizeType>
struct make_size_t {
    typedef typename std::conditional<(sizeof(SizeType) <= 4),
                                      std::uint32_t,
                                      std::uint64_t>::type type;
    typedef typename std::make_unsigned<type>::type unsigned_type;
    typedef typename std::make_signed<type>::type   signed_type;
};

} // namespace jstd

namespace jstd {
namespace Bits {

//
// popcount() algorithm
//
// See: http://www.cnblogs.com/Martinium/articles/popcount.html
// See: https://en.wikipedia.org/wiki/Hamming_weight
// See: https://stackoverflow.com/questions/757059/position-of-least-significant-bit-that-is-set
//
// See: https://zhuanlan.zhihu.com/p/341488123
//

//
// Look-up table Popcount
//

static const char * const s_bitsPerByte =
    "\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"
    "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
    "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
    "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
    "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
    "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
    "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
    "\4\5\5\6\5\6\6\7\5\6\6\7\6\7\7\x8";

static inline
uint32_t lookup_popcnt8(uint8_t n)
{
    uint32_t popcnt8 = static_cast<uint32_t>(s_bitsPerByte[n]);
    return popcnt8;
}

static inline
uint32_t lookup_popcnt16(uint16_t n)
{
    uint32_t popcnt16 = 0;
    popcnt16 += static_cast<uint32_t>(s_bitsPerByte[(n     ) & 0x00FFu]);
    popcnt16 += static_cast<uint32_t>(s_bitsPerByte[(n >> 8) & 0x00FFu]);
    return popcnt16;
}

static inline
uint32_t lookup_popcnt32(uint32_t n)
{
    uint32_t popcnt32 = 0;
    popcnt32 += static_cast<uint32_t>(s_bitsPerByte[(n      ) & 0x000000FFu]);
    popcnt32 += static_cast<uint32_t>(s_bitsPerByte[(n >> 8 ) & 0x000000FFu]);
    popcnt32 += static_cast<uint32_t>(s_bitsPerByte[(n >> 16) & 0x000000FFu]);
    popcnt32 += static_cast<uint32_t>(s_bitsPerByte[(n >> 24) & 0x000000FFu]);
    return popcnt32;
}

static inline
uint32_t lookup_popcnt64(uint64_t n)
{
    uint32_t low  = static_cast<uint32_t>((n      ) & 0x00000000FFFFFFFFull);
    uint32_t high = static_cast<uint32_t>((n >> 32) & 0x00000000FFFFFFFFull);

    uint32_t popcnt64_l = 0, popcnt64_h = 0;

    popcnt64_l += static_cast<uint32_t>(s_bitsPerByte[(low       ) & 0x000000FFu]);
    popcnt64_h += static_cast<uint32_t>(s_bitsPerByte[(high      ) & 0x000000FFu]);

    popcnt64_l += static_cast<uint32_t>(s_bitsPerByte[(low  >> 8 ) & 0x000000FFu]);
    popcnt64_h += static_cast<uint32_t>(s_bitsPerByte[(high >> 8 ) & 0x000000FFu]);

    popcnt64_l += static_cast<uint32_t>(s_bitsPerByte[(low  >> 16) & 0x000000FFu]);
    popcnt64_h += static_cast<uint32_t>(s_bitsPerByte[(high >> 16) & 0x000000FFu]);

    popcnt64_l += static_cast<uint32_t>(s_bitsPerByte[(low  >> 24) & 0x000000FFu]);
    popcnt64_h += static_cast<uint32_t>(s_bitsPerByte[(high >> 24) & 0x000000FFu]);

    return (popcnt64_l + popcnt64_h);
}

//
// Parallel Popcount
//
static inline
uint32_t parallel_popcnt32(uint32_t n)
{
    n = (n & 0x55555555UL) + ((n >>  1U) & 0x55555555UL);
    n = (n & 0x33333333UL) + ((n >>  2U) & 0x33333333UL);
    n = (n & 0x0F0F0F0FUL) + ((n >>  4U) & 0x0F0F0F0FUL);
    n = (n & 0x00FF00FFUL) + ((n >>  8U) & 0x00FF00FFUL);
    n = (n & 0x0000FFFFUL) + ((n >> 16U) & 0x0000FFFFUL);
    assert(n >= 0 && n <= 32);
    return n;
}

static inline
uint32_t parallel_popcnt64(uint64_t n)
{
    n = (n & 0x5555555555555555ULL) + ((n >>  1U) & 0x5555555555555555ULL);
    n = (n & 0x3333333333333333ULL) + ((n >>  2U) & 0x3333333333333333ULL);
    n = (n & 0x0F0F0F0F0F0F0F0FULL) + ((n >>  4U) & 0x0F0F0F0F0F0F0F0FULL);
    n = (n & 0x00FF00FF00FF00FFULL) + ((n >>  8U) & 0x00FF00FF00FF00FFULL);
    n = (n & 0x0000FFFF0000FFFFULL) + ((n >> 16U) & 0x0000FFFF0000FFFFULL);
    n = (n & 0x00000000FFFFFFFFULL) + ((n >> 32U) & 0x00000000FFFFFFFFULL);
    assert(n >= 0 && n <= 64);
    return static_cast<uint32_t>(n);
}

static inline
uint32_t parallel_popcnt(size_t n)
{
#if (ZIPLAB_WORD_LEN == 32)
    return parallel_popcnt32(static_cast<uint32_t>(n));
#else
    return parallel_popcnt64(static_cast<uint64_t>(n));
#endif
}

/*****************************************************************

  Hacker Popcount
  From: Book of Hacker's Delight [https://www.hackersdelight.org/]

*****************************************************************/

static inline
uint32_t hacker_popcnt32(uint32_t n)
{
    n -=  ((n >> 1U) & 0x55555555U);
    n  = (((n >> 2U) & 0x33333333U) + (n & 0x33333333U));
    n  = (((n >> 4U) + n) & 0x0F0F0F0FU);
    n +=   (n >> 8U);
    n +=   (n >> 16U);
    n  = (n & 0x0000003FU);
    assert(n >= 0 && n <= 32);
    return n;
}

static inline
uint32_t hacker_popcnt64(uint64_t n)
{
    n -=  ((n >> 1U) & 0x5555555555555555ULL);
    n  = (((n >> 2U) & 0x3333333333333333ULL) + (n & 0x3333333333333333ULL));
    n  = (((n >> 4U) + n) & 0x0F0F0F0F0F0F0F0FULL);
    n +=   (n >> 8U);
    n +=   (n >> 16U);
    n +=   (n >> 32U);
    n  = (n & 0x000000000000007FULL);
    assert(n >= 0 && n <= 64);
    return static_cast<uint32_t>(n);
}

static inline
uint32_t hacker_popcnt(size_t n)
{
#if (ZIPLAB_WORD_LEN == 32)
    return hacker_popcnt32(static_cast<uint32_t>(n));
#else
    return hacker_popcnt64(static_cast<uint64_t>(n));
#endif
}

/////////////////////////////////////////////////////
//
// nifty popcount
//
/////////////////////////////////////////////////////

static inline
uint32_t nifty_popcnt32(uint32_t n)
{
    static const uint32_t MAX_UINT32 = std::numeric_limits<uint32_t>::max();
    static const uint32_t MASK_01010101 = MAX_UINT32 / 3;
    static const uint32_t MASK_00110011 = MAX_UINT32 / 5;
    static const uint32_t MASK_00001111 = MAX_UINT32 / 17;
    n = (n & MASK_01010101) + ((n >> 1) & MASK_01010101);
    n = (n & MASK_00110011) + ((n >> 2) & MASK_00110011);
    n = (n & MASK_00001111) + ((n >> 4) & MASK_00001111);
    return (n % 255);
}

static inline
uint32_t nifty_popcnt64(uint64_t n)
{
    static const uint64_t MAX_UINT64 = std::numeric_limits<uint64_t>::max();
    static const uint64_t MASK_01010101 = MAX_UINT64 / 3;
    static const uint64_t MASK_00110011 = MAX_UINT64 / 5;
    static const uint64_t MASK_00001111 = MAX_UINT64 / 17;
    n = (n & MASK_01010101) + ((n >> 1) & MASK_01010101);
    n = (n & MASK_00110011) + ((n >> 2) & MASK_00110011);
    n = (n & MASK_00001111) + ((n >> 4) & MASK_00001111);
    return (n % 511);
}

static inline
uint32_t nifty_popcnt(size_t n)
{
#if (ZIPLAB_WORD_LEN == 32)
    return nifty_popcnt32(static_cast<uint32_t>(n));
#else
    return nifty_popcnt64(static_cast<uint64_t>(n));
#endif
}

/*****************************************************************************

Hakmem Popcount

Consider a 3 bit number as being
    4a+2b+c
if we shift it right 1 bit, we have
    2a+b
subtracting this from the original gives
    2a+b+c
if we shift the original 2 bits right we get
    a
and so with another subtraction we have
    a+b+c
which is the number of bits in the original number.

Suitable masking  allows the sums of  the octal digits  in a 32 bit  number to
appear in  each octal digit.  This  isn't much help  unless we can get  all of
them summed together.   This can be done by modulo  arithmetic (sum the digits
in a number by  molulo the base of the number minus  one) the old "casting out
nines" trick  they taught  in school before  calculators were  invented.  Now,
using mod 7 wont help us, because our number will very likely have more than 7
bits set.   So add  the octal digits  together to  get base64 digits,  and use
modulo 63.   (Those of you  with 64  bit machines need  to add 3  octal digits
together to get base512 digits, and use mod 511.)

This is HACKMEM 169, as used in X11 sources.
Source: MIT AI Lab memo, late 1970's.

*****************************************************************************/

// hakmem popcnt 32
static inline
uint32_t hakmem_popcnt32(uint32_t n)
{
    uint32_t tmp;
    tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);
    return (((tmp + (tmp >> 3)) & 030707070707) % 63U);
}

// hakmem popcnt 64
static inline
uint32_t hakmem_popcnt64(uint64_t n)
{
    uint64_t tmp;
    tmp = n - ((n >> 1) & 01333333333333333333333) - ((n >> 2) & 01111111111111111111111);
    return (((tmp + (tmp >> 3)) & 0707070707070707070707) % 127U);
}

static inline
uint32_t hakmem_popcnt(size_t n)
{
#if (ZIPLAB_WORD_LEN == 32)
    return hakmem_popcnt32(static_cast<uint32_t>(n));
#else
    return hakmem_popcnt64(static_cast<uint64_t>(n));
#endif
}

///////////////////////////////////////////////////////////////////

//
// Assembly Popcount
//

static inline
uint32_t assembly_popcnt32(uint32_t n)
{
    int popcnt = _mm_popcnt_u32(static_cast<unsigned int>(n));
    return static_cast<uint32_t>(popcnt);
}

static inline
uint32_t assembly_popcnt64(uint64_t n)
{
    long long popcnt = _mm_popcnt_u64(static_cast<unsigned long long>(n));
    return static_cast<uint32_t>(popcnt);
}

static inline
uint32_t assembly_popcnt8(uint8_t n)
{
    uint32_t popcnt = assembly_popcnt32(static_cast<uint32_t>(n));
    return popcnt;
}

static inline
uint32_t assembly_popcnt16(uint16_t n)
{
    uint32_t popcnt = assembly_popcnt32(static_cast<uint32_t>(n));
    return popcnt;
}

static inline
uint32_t assembly_popcnt(size_t n)
{
#if (ZIPLAB_WORD_LEN == 32)
    return assembly_popcnt32(static_cast<uint32_t>(n));
#else
    return assembly_popcnt64(static_cast<uint64_t>(n));
#endif
}

///////////////////////////////////////////////////////////////////

static inline
uint32_t popcnt8(uint32_t n)
{
    return assembly_popcnt8(n);
}

static inline
uint32_t popcnt16(uint32_t n)
{
    return assembly_popcnt16(n);
}

static inline
uint32_t popcnt32(uint32_t n)
{
    return assembly_popcnt32(n);
}

static inline
uint32_t popcnt64(uint64_t n)
{
    return assembly_popcnt64(n);
}

static inline
uint32_t popcnt(size_t n)
{
#if (ZIPLAB_WORD_LEN == 32)
    return popcnt32(static_cast<uint32_t>(n));
#else
    return popcnt64(static_cast<uint64_t>(n));
#endif
}

///////////////////////////////////////////////////////////////////

static inline
uint32_t __internal_ctz(uint32_t n)
{
    return 0;
}

static inline
uint32_t __internal_ctzll(uint64_t n)
{
    return 0;
}

static inline
uint32_t __internal_clz(uint32_t n)
{
    return 0;
}

static inline
uint32_t __internal_clzll(uint64_t n)
{
    return 0;
}

///////////////////////////////////////////////////////////////////

static inline
uint32_t bsf32(uint32_t n)
{
    assert(n != 0);
    ZIPLAB_ASSUME(n != 0);
#if (jstd_cplusplus >= 2020L)
    return (uint32_t)(31 - std::countr_zero(n));
#elif (defined(_MSC_VER) && (_MSC_VER >= 1500)) && !defined(__clang__)
    unsigned long index;
    ::_BitScanForward(&index, (unsigned long)n);
    return (uint32_t)index;
#elif defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
  #if __has_builtin(__builtin_ctz)
    return (uint32_t)__builtin_ctz(n);
  #elif defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
    return (uint32_t)__builtin_ctz(n);
  #elif defined(__GNUC__) || __has_builtin(__bsfd) || (__clang_major__ >= 12)
    // gcc: __bsfd(n)
    return __bsfd(n);
  #else
    return (uint32_t)__internal_ctz(n);
  #endif
#else
    return (uint32_t)__internal_ctz(n);
#endif
}

static inline
uint32_t bsf64(uint64_t n)
{
    assert(n != 0);
    ZIPLAB_ASSUME(n != 0);
    return (uint32_t)__internal_ctzll(n);
}

static inline
uint32_t bsf(size_t n)
{
#if (ZIPLAB_WORD_LEN == 32)
    return bsf32(static_cast<uint32_t>(n));
#else
    return bsf64(static_cast<uint64_t>(n));
#endif
}

static inline
uint32_t bsr32(uint32_t n)
{
    assert(n != 0);
    ZIPLAB_ASSUME(n != 0);
#if (jstd_cplusplus >= 2020L)
    return (uint32_t)(31 - std::countl_zero(n));
#elif (defined(_MSC_VER) && (_MSC_VER >= 1500)) && !defined(__clang__)
    unsigned long index;
    ::_BitScanReverse(&index, (unsigned long)n);
    return (uint32_t)index;
#elif defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
  #if __has_builtin(__builtin_clz)
    return (uint32_t)(31 - __builtin_clz(n));
  #elif defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
    return (uint32_t)(31 - __builtin_clz(n));
  #elif defined(__GNUC__) || __has_builtin(__bsrd) || (__clang_major__ >= 12)
    // gcc: __bsrd(n)
    return __bsrd(n);
  #else
    return (uint32_t)(31 - __internal_clz(n));
  #endif
#else
    return (uint32_t)(31 - __internal_clz(n));
#endif
}

static inline
uint32_t bsr64(uint64_t n)
{
    assert(n != 0);
    ZIPLAB_ASSUME(n != 0);
    return (uint32_t)(63 - __internal_clzll(n));
}

static inline
uint32_t bsr(size_t n)
{
#if (ZIPLAB_WORD_LEN == 32)
    return bsr32(static_cast<uint32_t>(n));
#else
    return bsr64(static_cast<uint64_t>(n));
#endif
}

template <typename SizeType>
inline
std::uint32_t bitScanForward(SizeType n)
{
    static_assert(std::is_integral<SizeType>::value,
                  "Error: jstd::Bits::bitScanForward(SizeType n) -- n must be a integral type.");
    typedef typename jstd::make_size_t<SizeType>::type size_type;
    size_type u = static_cast<size_type>(n);
    assert(u != 0);
    if (sizeof(size_type) <= 4) {
        return bsf32(static_cast<uint32_t>(u));
    } if (sizeof(size_type) == 8) {
        return bsf64(static_cast<uint64_t>(u));
    } else {
        return bsf(n);
    }
}

template <typename SizeType>
inline
std::uint32_t bitScanReverse(SizeType n)
{
    static_assert(std::is_integral<SizeType>::value,
                  "Error: jstd::Bits::bitScanReverse(SizeType n) -- n must be a integral type.");
    typedef typename jstd::make_size_t<SizeType>::type size_type;
    size_type u = static_cast<size_type>(n);
    assert(u != 0);
    if (sizeof(size_type) <= 4) {
        return bsr32(static_cast<uint32_t>(u));
    } if (sizeof(size_type) == 8) {
        return bsr64(static_cast<uint64_t>(u));
    } else {
        return bsr(n);
    }
}

static inline
unsigned int countTrailingZeros32(unsigned int x) {
#if defined(__BMI1__)
    return (unsigned int)_tzcnt_u32(x);
#else
    if (ziplab_likely(x != 0))
        return (unsigned int)bsf32(x);
    else
        return 32;
#endif
}

static inline
unsigned int countTrailingZeros64(uint64_t x) {
#if defined(__BMI1__)
    return (unsigned int)_tzcnt_u64(x);
#else
    if (ziplab_likely(x != 0))
        return (unsigned int)bsf64(x);
    else
        return 64;
#endif
}

static inline
unsigned int countLeadingZeros32(unsigned int x) {
#if defined(__BMI1__)
    return (unsigned int)_lzcnt_u32(x);
#else
    if (ziplab_likely(x != 0))
        return (unsigned int)(31u - bsr32(x));
    else
        return 32;
#endif
}

static inline
unsigned int countLeadingZeros64(uint64_t x) {
#if defined(__BMI1__)
    return (unsigned int)_lzcnt_u64(x);
#else
    if (ziplab_likely(x != 0))
        return (unsigned int)(63u - bsr64(x));
    else
        return 64;
#endif
}

template <typename SizeType>
inline
std::uint32_t countTrailingZeros(SizeType n)
{
    static_assert(std::is_integral<SizeType>::value,
                  "Error: jstd::Bits::countTrailingZeros(SizeType n) -- n must be a integral type.");
    typedef typename jstd::make_size_t<SizeType>::type size_type;
    size_type u = static_cast<size_type>(n);
    assert(u != 0);
    if (sizeof(size_type) <= 4) {
        return countTrailingZeros32(static_cast<uint32_t>(u));
    } else {
        assert(sizeof(size_type) == 8);
        return countTrailingZeros64(static_cast<uint64_t>(u));
    }
}

template <typename SizeType>
inline
std::uint32_t countLeadingZeros(SizeType n)
{
    static_assert(std::is_integral<SizeType>::value,
                  "Error: jstd::Bits::countLeadingZeros(SizeType n) -- n must be a integral type.");
    typedef typename jstd::make_size_t<SizeType>::type size_type;
    size_type u = static_cast<size_type>(n);
    assert(u != 0);
    if (sizeof(size_type) <= 4) {
        return countLeadingZeros32(static_cast<uint32_t>(u));
    } else {
        assert(sizeof(size_type) == 8);
        return countLeadingZeros64(static_cast<uint64_t>(u));
    }
}

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4146)
#endif

//
// Get the least significant 1 bit (LS1B)
//
static inline
uint32_t ls1b32(uint32_t x) {
    return (x & (uint32_t)-x);
}

static inline
uint64_t ls1b64(uint64_t x) {
    return (x & (uint64_t)-x);
}

static inline
size_t ls1b(size_t x) {
    return (x & (size_t)-x);
}

//
// Get the most significant 1 bit (MS1B)
//
static inline
uint32_t ms1b32(uint32_t x) {
    return (x != 0) ? (uint32_t)(1ul << bsr32(x)) : 0;
}

static inline
uint64_t ms1b64(uint64_t x) {
    return (x != 0) ? (uint64_t)(1ull << bsr64(x)) : 0;
}

static inline
size_t ms1b(size_t x) {
    return (x != 0) ? ((size_t)(1u) << bsr(x)) : 0;
}

//
// Clear the lowest bit (the least significant 1 bit, LS1B)
//
static inline
uint32_t clearLowestBit32(uint32_t x) {
    // _blsr_u32(n)
    return (x & (uint32_t)(x - 1));
}

static inline
uint64_t clearLowestBit64(uint64_t x) {
    // _blsr_u64(n)
    return (x & (uint64_t)(x - 1));
}

static inline
size_t clearLowestBit(size_t x) {
    return (x & (size_t)(x - 1));
}

//
// log2_int()
//
static inline
uint32_t log2_int_32(uint32_t n) {
    if (ziplab_likely(n > 1))
        return (uint32_t)(bsr32(n - 1) + 1);
    else
        return n;
}

static inline
uint64_t log2_int_64(uint64_t n) {
    if (ziplab_likely(n > 1))
        return (uint64_t)(bsr64(n - 1) + 1);
    else
        return n;
}

static inline
size_t log2_int(size_t n) {
#if (ZIPLAB_WORD_LEN == 32)
    return log2_int_32(n);
#else
    return log2_int_64(n);
#endif
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif

} // namespace Bits
} // namespace jstd

#endif // ZIPLAB_JSTD_BITS_HPP

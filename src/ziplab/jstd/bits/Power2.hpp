#ifndef ZIPLAB_JSTD_POWER2_HPP
#define ZIPLAB_JSTD_POWER2_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <cstdint>
#include <cstddef>
#include <cstdbool>
#include <assert.h>

#include "ziplab/basic/stddef.h"
#include "ziplab/jstd/bits/Bits.hpp"

namespace jstd {
namespace Power2 {

template <typename T>
static inline bool is_power2(T n) {
    return ((n & (n - 1)) == 0);
}

//
// round_to_pow2(n)
//
template <size_t MinValue>
static inline
uint32_t round_to(uint32_t n)
{
    if (n >= 2) {
#if defined(__GNUC__) || defined(__clang__)
        n--;
        assert(n > 0);
        uint32_t leading_zeros = __builtin_clz(n);
        uint32_t highest_bit_pos = 31 - leading_zeros;

        uint32_t power2 = 1 << (highest_bit_pos + 1);
        return power2;
#elif defined(_MSC_VER)
        n--;
        assert(n > 0);
        unsigned long highest_bit_pos;
        _BitScanReverse(&highest_bit_pos, n);

        uint32_t power2 = 1 << (highest_bit_pos + 1);
        return power2;
#else
        uint32_t power2 = 1;
        while (power2 < n) {
            power2 <<= 1;
        }
        return power2;
#endif
    } else {
        // If it's 0 or 1, return directly.
        return n;
    }
}

template <size_t MinValue>
static inline
size_t round_to(size_t n)
{
    if (!is_power2(n)) {
        //
    }
    return n;
}

//
// round_up_pow2(n)
//

template <uint32_t MinValue = 0>
static inline
uint32_t round_up_32(uint32_t n)
{
    if (!is_power2(n)) {
        if ((MinValue >= 2) || (n >= 2)) {
#if defined(__GNUC__) || defined(__clang__)
            n--;
            assert(n > 0);
            uint32_t leading_zeros = __builtin_clz(n);
            uint32_t highest_bit_pos = 31 - leading_zeros;

            uint32_t power2 = 1 << (highest_bit_pos + 1);
            return power2;
#elif defined(_MSC_VER)
            n--;
            assert(n > 0);
            unsigned long highest_bit_pos;
            _BitScanReverse(&highest_bit_pos, n);

            uint32_t power2 = 1u << (highest_bit_pos + 1);
            return power2;
#else
            uint32_t power2 = 1;
            while (power2 < n) {
                power2 <<= 1;
            }
            return power2;
#endif
        } else {
            // If it's 0 or 1, return directly.
            return n;
        }
    }
    return n;
}

template <uint64_t MinValue = 0>
static inline
uint64_t round_up_64(uint64_t n)
{
    if (!is_power2(n)) {
        if ((MinValue >= 2) || (n >= 2)) {
#if defined(__GNUC__) || defined(__clang__)
            n--;
            assert(n > 0);
            uint32_t leading_zeros = __builtin_clzll(n);
            uint32_t highest_bit_pos = 31 - leading_zeros;

            uint64_t power2 = 1 << (highest_bit_pos + 1);
            return power2;
#elif defined(_MSC_VER)
            n--;
            assert(n > 0);
            unsigned long highest_bit_pos;
            _BitScanReverse64(&highest_bit_pos, n);

            uint64_t power2 = 1ull << (highest_bit_pos + 1);
            return power2;
#else
            uint64_t power2 = 1;
            while (power2 < n) {
                power2 <<= 1;
            }
            return power2;
#endif
        } else {
            // If it's 0 or 1, return directly.
            return n;
        }
    }
    return n;
}

template <size_t MinValue = 0>
static inline
size_t round_up(size_t n)
{
#if ZIPLAB_WORD_LEN == 32
    return round_up_32(static_cast<uint32_t>(n));
#else
    return round_up_64(static_cast<uint64_t>(n));
#endif
}

} // namespace Power2
} // namespace jstd

#endif // ZIPLAB_JSTD_POWER2_HPP

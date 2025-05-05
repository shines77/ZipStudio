#ifndef ZIPLAB_JSTD_POWER2_HPP
#define ZIPLAB_JSTD_POWER2_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <cstdint>
#include <cstddef>
#include <cstdbool>
#include <type_traits>
#include <assert.h>

#include "ziplab/basic/stddef.h"
#include "ziplab/jstd/bits/Bits.hpp"

namespace jstd {
namespace Power2 {

template <typename T>
static inline
bool is_power2(T n) {
    return ((n & (n - 1)) == 0);
}

template <typename T>
static inline
bool is_power2_not_0(T n) {
    return (is_power2(n) && (n != 0));
}

//
// round_to_pow2(n)
//
template <size_t MinValue, bool NonPower2 = false>
static inline
uint32_t round_to_32(uint32_t n)
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

/*********************************************************

  N = round_to(n) = |Log2(n)|

  eg:
     round_to(0) = 0, round_to(1) = 1, round_to(2) = 2,
     round_to(4) = 4, round_to(5) = 4, round_to(6) = 4,
     round_to(7) = 4, round_to(8) = 8,

*********************************************************/

template <size_t MinValue, bool NonPower2 = false>
static inline
size_t round_to(size_t n)
{
    if (!is_power2(n)) {
        //
    }
    return n;
}

template <uint32_t MinValue = 0, bool NonPower2 = false>
static inline
uint32_t round_up_32(uint32_t n)
{
    if (NonPower2 || !is_power2(n)) {
        if ((MinValue >= 2) || (n >= 2)) {
#if defined(__GNUC__) || defined(__clang__)
            n--;
            assert(n > 0);
            uint32_t leading_zeros = __builtin_clz(n);
            uint32_t highest_bit_pos = 31 - leading_zeros;

            uint32_t power2 = 1u << (highest_bit_pos + 1);
            return power2;
#elif defined(_MSC_VER)
            n--;
            assert(n > 0);
            unsigned long highest_bit_pos;
            ::_BitScanReverse(&highest_bit_pos, n);

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

template <uint64_t MinValue = 0, bool NonPower2 = false>
static inline
uint64_t round_up_64(uint64_t n)
{
    if (NonPower2 || !is_power2(n)) {
        if ((MinValue >= 2) || (n >= 2)) {
#if defined(__GNUC__) || defined(__clang__)
            n--;
            assert(n > 0);
            uint32_t leading_zeros = __builtin_clzll(n);
            uint32_t highest_bit_pos = 31 - leading_zeros;

            uint64_t power2 = 1ull << (highest_bit_pos + 1);
            return power2;
#elif defined(_MSC_VER)
            n--;
            assert(n > 0);
            unsigned long highest_bit_pos;
            ::_BitScanReverse64(&highest_bit_pos, n);

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

/*********************************************************

  N = round_up(n) = 2 ^ (|Log2(n - 1)| + 1)

  eg:
     round_up(0) = 0, round_up(1) = 1, round_up(2) = 2,
     round_up(4) = 4, round_up(5) = 8, round_up(6) = 8,
     round_up(7) = 8, round_up(8) = 8,

  Similar to std::bit_ceil(x) in C++ 20.

  https://en.cppreference.com/w/cpp/numeric/bit_ceil

*********************************************************/

template <typename SizeType, SizeType MinValue = 0, bool NonPower2 = false>
inline
typename jstd::make_size_t<SizeType>::type
round_up(SizeType x)
{
    static_assert(std::is_integral<SizeType>::value,
                  "Error: jstd::Power2::round_up(SizeType n) -- n must be a integral type.");
    typedef typename jstd::make_size_t<SizeType>::type  size_type;
#ifndef NDEBUG
    typedef typename std::make_signed<size_type>::type  signed_type;
#endif
    static constexpr size_type kMaxNumber = (std::numeric_limits<size_type>::max)();
    static constexpr size_type kMaxValue = kMaxNumber / 2 + 1;

    size_type n = static_cast<size_type>(x);
    if ((n < kMaxValue) || (sizeof(size_type) > 4)) {
        if (NonPower2) {
            assert(!Power2::is_power2(n));
            if ((MinValue > 0) || (n > 0)) {
                assert(n > 0);
                std::uint32_t ms1b_pos = Bits::bitScanReverse(n);
                return (static_cast<size_type>(1) << (ms1b_pos + 1));
            } else {
                return static_cast<size_type>(n);
            }
        } else {
            if ((MinValue >= 2) || (n >= 2)) {
                assert(signed_type(n - 1) > 0);
                std::uint32_t ms1b_pos = Bits::bitScanReverse(n - 1);
                return (static_cast<size_type>(1) << (ms1b_pos + 1));
            } else {
                return static_cast<size_type>(n);
            }
        }
    }
    else {
        return kMaxNumber;
    }
}

} // namespace Power2
} // namespace jstd

#endif // ZIPLAB_JSTD_POWER2_HPP

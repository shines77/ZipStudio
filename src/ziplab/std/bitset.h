#ifndef ZIPLAB_STD_BITSET_HPP
#define ZIPLAB_STD_BITSET_HPP

#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>
#include <climits>      // For CHAR_BIT
#include <algorithm>    // For std::min(), std::max()
#include <stdexcept>    // For std::exception

namespace jstd {

static const char * const s_bitsPerByteTable =
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

//
// Store fixed-length sequence of Boolean elements
//
template <std::size_t Bits>
class bitset
{
public:
    using value_type = typename std::conditional<(Bits <= 32), std::uint32_t, std::uint64_t>::type;
    using size_type = std::size_t;
    using ssize_type = std::ptrdiff_t;

    // The number of bits per word
    static constexpr size_type kBitsPerWord = sizeof(value_type) * CHAR_BIT;
    static constexpr size_type kBitsPerByte = CHAR_BIT;

    // The total size of packing bits into words
    static constexpr size_type kOrigTotalWords = (Bits + (kBitsPerWord - 1)) / kBitsPerWord;
    static constexpr size_type kOrigTotalBytes = (Bits + (kBitsPerByte - 1)) / kBitsPerByte;

    static constexpr size_type kTotalWords = (Bits != 0) ? kOrigTotalWords : 1;
    static constexpr size_type kTotalBytes = (Bits != 0) ? kOrigTotalBytes : 1;

    static_assert((kTotalWords > 0), "kTotalWords must be greater than 0");
    static_assert((kTotalBytes > 0), "kTotalBytes must be greater than 0");

    static constexpr ssize_type ksTotalWords = static_cast<ssize_type>(kTotalWords);

    // Full words and rest bits
    static constexpr size_type kFullWords = Bits / kBitsPerWord;
    static constexpr size_type kRestBits  = Bits % kBitsPerWord;

    static constexpr value_type kValue1   = static_cast<value_type>(1);
    static constexpr value_type kFullMask = static_cast<value_type>(-1);
    static constexpr value_type kLastMask = (kRestBits != 0) ? ((kValue1 << kRestBits) - kValue1) : 0;

    // Need mask ?
    static constexpr bool bNeedMask = (Bits < kBitsPerWord);
    static constexpr value_type kMask = static_cast<value_type>((kValue1 << (bNeedMask ? Bits : 0)) - kValue1);

    // Class reference
    class reference {
        // Proxy for an element
        friend class bitset<Bits>;

    public:
        ~reference() noexcept {
            // Destroy the object
        }

        reference & operator = (bool value) noexcept {
            // Assign Boolean to element
            bitset_->set(pos_, value);
            return *this;
        }

        reference & operator = (const reference & rhs) noexcept {
            // Assign reference to element
            bitset_->set(pos_, bool(rhs));
            return *this;
        }

        reference & flip() noexcept {
            // Complement stored element
            bitset_->flip(pos_);
            return *this;
        }

        bool operator ~ () const noexcept {
            // Return complemented element
            return !bitset_->test(pos_);
        }

        operator bool() const noexcept {
            // Return element
            return bitset_->test(pos_);
        }

    private:
        reference() noexcept : bitset_(nullptr), pos_(0) {
            // Default construct
        }

        reference(bitset<Bits> & bset, size_t pos)
            : bitset_(&bset), pos_(pos) {
            // Construct from bitset reference and position
        }

        // Pointer to the bitset
        bitset<Bits> *  bitset_;
        // Position of element in bitset
        size_type       pos_;
    };

private:
    // The set of bits
    value_type array_[kTotalWords];

public:
	constexpr bitset() noexcept : array_() {
        // Construct with all false values
    }

	constexpr bitset(value_type value) noexcept
		: array_{ static_cast<value_type>(bNeedMask ? (value & kMask) : value) } {
        // Construct from bits in value_type
    }

	reference operator [] (size_type pos) {
        // Subscript mutable sequence
		validate(pos);
		return reference(*this, pos);
    }

	bool operator == (const bitset & right) const noexcept {
        // Test for bitset equality
		for (ssize_type pos = 0; pos < ksTotalWords; pos++) {
			if (array_[pos] != right._get_word(pos))
				return false;
        }
		return true;
    }

	bool operator != (const bitset & right) const noexcept {
        // Test for bitset not equality
		return !(*this == right);
    }

	bitset & operator &= (const bitset & right) noexcept {
        // And in right
		for (ssize_type pos = 0; pos < ksTotalWords; pos++) {
			array_[pos] &= right._get_word(pos);
        }
		return *this;
    }

	bitset & operator |= (const bitset & right) noexcept {
        // Or in right
		for (ssize_type pos = 0; pos < ksTotalWords; pos++) {
			array_[pos] |= right._get_word(pos);
        }
		return *this;
    }

	bitset & operator ^= (const bitset & right) noexcept {
        // Xor in right
		for (ssize_type pos = 0; pos < ksTotalWords; pos++) {
			array_[pos] ^= right._get_word(pos);
        }
		return *this;
    }

	bitset & operator <<= (size_type pos) noexcept {
        // Shift left by pos
        assert(pos < Bits);
        ssize_type word_shift = get_index(pos);
        if (word_shift != 0) {
            // Shift left by words
            for (ssize_type index = kTotalWords - 1; index >= 0; index--) {
                array_[index] = (index >= word_shift) ?
                    array_[index - word_shift] : static_cast<value_type>(0);
            }
        }

        value_type bit_shift = get_shift(pos);
        if (bit_shift != 0) {
            // 0 < bit_shift < kBitsPerWord, shift by bits
            value_type bit_rshift = kBitsPerWord - bit_shift;
            for (ssize_type index = kTotalWords - 1; index >= 0; index--) {
                array_[index] = static_cast<value_type>((array_[index] << bit_shift) | (array_[index - 1] >> bit_rshift));
            }
            array_[0] <<= bit_shift;
        }

        trim();
        return *this;
    }

	bitset & operator >>= (size_type pos) noexcept {
        // Shift right by pos, first by words then by bits
        assert(pos < Bits);
        ssize_type word_shift = get_index(pos);
        if (word_shift != 0) {
            // Shift right by words
            for (ssize_type index = 0; index < kTotalWords; index++) {
                array_[index] = ((kTotalWords - index) >= word_shift) ?
                    array_[index + word_shift] : static_cast<value_type>(0);
            }
        }

        value_type bit_shift = get_shift(pos);
        if (bit_shift != 0) {
            // 0 < bit_shift < kBitsPerWord, shift by bits
            value_type bit_lshift = kBitsPerWord - bit_shift;
            for (ssize_type index = 0; index < (kTotalWords - 1); index++) {
                array_[index] = static_cast<value_type>((array_[index] >> bit_shift) | (array_[index + 1] << bit_lshift));
            }
            array_[kFullWords] >>= bit_shift;
        }
		return *this;
    }

    constexpr const char * data() const noexcept {
        return reinterpret_cast<const char *>(&array_[0]);
    }

    constexpr size_type bytes() const noexcept {
	    return kTotalBytes;
	}

    constexpr size_type words() const noexcept {
	    return kTotalWords;
	}

    constexpr size_type size() const noexcept {
        // Return size of bitset
	    return Bits;
	}

    value_type get_word(ssize_type index) const {
        // Get word at pos
		if (index >= ksTotalWords) {
            // index out of range
			throw_index_out_of_range();
        }
        return _get_word(index);
    }

	size_type count() const noexcept {
        // Count number of set bits
		const unsigned char * ptr = (const unsigned char *)(const void *)array_;
		const unsigned char * const last = ptr + sizeof(array_);
		size_type total_count = 0;
		for ( ; ptr != last; ptr++) {
			total_count += static_cast<size_type>(s_bitsPerByteTable[*ptr]);
        }
		return total_count;
    }

    size_type count_byte(ssize_type pos) const noexcept {
        // Count number of bits at the specified pos
        assert(index < kTotalBytes);
		const unsigned char * first = (const unsigned char *)(const void *)array_;
		const unsigned char * const ptr = first + pos;
		return static_cast<size_type>(s_bitsPerByteTable[*ptr]);
    }

    size_type count_word(ssize_type index) const noexcept {
        // Count number of set bits at the specified word
        assert(index < kTotalWords);
		const unsigned char * ptr = (const unsigned char *)(const void *)&array_[index];
		const unsigned char * const last = ptr + sizeof(value_type);
		size_type total_count = 0;
		for ( ; ptr != last; ptr++) {
			total_count += static_cast<size_type>(s_bitsPerByteTable[*ptr]);
        }
		return total_count;
    }

	bool test(size_type pos) const {
        // Test if bit at pos is set
		if (pos >= Bits) {
            // pos out of range
			throw_out_of_range();
        }
		return subscript(pos);
    }

	bool all() const noexcept {
        // Test if all bits set
#if 1
		for (size_type pos = 0; pos < kFullWords; pos++) {
			if (array_[pos] != kFullMask)
                return false;
        }
        // Check last element if need
        if (kRestBits != 0) {
            return (array_[kFullWords] == kLastMask);
        } else {
            return true;
        }
#else
		return (count() == size());
#endif
    }

	bool any() const noexcept {
        // Test if any bits are set
		for (ssize_type pos = 0; pos < ksTotalWords; pos++) {
			if (array_[pos] != 0)
				return true;
        }
		return false;
    }

	bool none() const noexcept {
        // Test if no bits are set
		return (!any());
    }

	bitset & set() noexcept {
        // Set all bits true
		fill_all(static_cast<value_type>(~0));
		return *this;
    }

	bitset & set(size_type pos, bool value = true) {
        // Set bit at pos to _Val
		if (pos >= Bits) {
            // pos out of range
			throw_out_of_range();
        }
        ssize_type index = get_index(pos);
        value_type shift = get_shift(pos);
		if (value)
			array_[index] |= static_cast<value_type>(1) << shift;
		else
			array_[index] &= ~(static_cast<value_type>(1) << shift);
		return *this;
    }

	bitset & reset() noexcept {
        // Set all bits false
		fill_all();
		return *this;
    }

	bitset & reset(size_type pos) {
        // Set bit at pos to false
		return set(pos, false);
    }

	bitset operator ~ () const noexcept {
        // Flip all bits
		return (bitset(*this).flip());
    }

	bitset & flip() noexcept {
        // Flip all bits
		for (ssize_type pos = 0; pos < ksTotalWords; pos++) {
			array_[pos] = ~array_[pos];
        }

		trim();
		return *this;
    }

	bitset & flip(size_type pos) {
        // Flip bit at pos
		if (pos >= Bits) {
            // pos out of range
			throw_out_of_range();
        }
        ssize_type index = get_index(pos);
        value_type shift = get_shift(pos);
		array_[index] ^= static_cast<value_type>(1) << shift;
		return *this;
    }

    bitset operator << (size_type pos) const noexcept {
        // Return bitset shifted left by pos
        return (bitset(*this) <<= pos);
    }

    bitset operator >> (size_type pos) const noexcept {
        // Return bitset shifted right by pos
        return (bitset(*this) >>= pos);
    }

private:
    // pos / kBitsPerWord
    inline ssize_type get_index(size_type pos) const {
        return static_cast<ssize_type>(pos / kBitsPerWord);
    }

    // pos % kBitsPerWord
    inline value_type get_shift(size_type pos) const {
        return static_cast<value_type>(pos % kBitsPerWord);
    }

    inline value_type _get_word(ssize_type index) const {
        assert(index < ksTotalWords);
        return array_[index];
    }

	constexpr bool subscript(size_type pos) const {
        // Subscript nonmutable sequence
        ssize_type index = get_index(pos);
        value_type shift = get_shift(pos);
		return ((array_[index] & (static_cast<value_type>(1) << shift)) != 0);
    }

    void validate(size_type pos) {
        assert(pos < kTotalWords);
    }

    void trim() {
        // Clear any trailing bits in last word
        // kRestBits = Bits % kBitsPerWord
        trim_if(std::bool_constant<(Bits == 0) || (kRestBits != 0)>());
    }

	void trim_if(std::true_type) {
        // Bits to trim, remove them
        value_type shift = static_cast<value_type>(kRestBits);
		array_[kFullWords] &= (static_cast<value_type>(1) << shift) - 1;
    }

	void trim_if(std::false_type) {
        // No bits to trim, do nothing
    }

    void fill_all(value_type value = 0) {
        // Set all words to _Wordval
		for (ssize_type pos = 0; pos < ksTotalWords; pos++) {
			array_[pos] = value;
        }
		if (value != 0) {
			trim();
        }
    }

    bool subscript(size_type pos) {
        return true;
    }

	[[noreturn]] void throw_invalid_string_element() const {
        // Report invalid string element in bitset conversion
		throw std::invalid_argument("invalid jstd::bitset<N> char");
    }

	[[noreturn]] void throw_overflow() const {
        // Report converted value too big to represent
		throw std::overflow_error("jstd::bitset<N> overflow");
    }

	[[noreturn]] void throw_out_of_range() const {
        // Report bit index out of range
		throw std::out_of_range("invalid jstd::bitset<N> position");
    }

	[[noreturn]] void throw_index_out_of_range() const {
        // Report bit index out of range
		throw std::out_of_range("invalid jstd::bitset<N> index");
    }
};

} // namespace jstd

#endif // ZIPLAB_STD_BITSET_HPP

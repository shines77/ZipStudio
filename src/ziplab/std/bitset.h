#ifndef ZIPLAB_STD_BITSET_HPP
#define ZIPLAB_STD_BITSET_HPP

#pragma once

#include <cstdint>
#include <cstddef>
#include <bitset>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "ziplab/lz77/lz77.hpp"
#include "ziplab/lz77/lzDictHashmap.hpp"

namespace jstd {

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

    static_assert((Bits != 0), "<Bits> don't allow less than 1.");

    // The number of bits per word
    static constexpr size_type kBitsPerWord = sizeof(value_type) * CHAR_BIT;
    static constexpr size_type kBitsPerByte = CHAR_BIT;

    // The total size of packing bits into words
    static constexpr size_type kOrigTotalWords = (Bits + (kBitsPerWord - 1)) / kBitsPerWord;
    static constexpr size_type kOrigTotalBytes = (Bits + (kBitsPerByte - 1)) / kBitsPerByte;

    static constexpr size_type kTotalWords = (Bits != 0) ? kOrigTotalWords : 1;
    static constexpr size_type kTotalBytes = (Bits != 0) ? kOrigTotalBytes : 1;

    static constexpr ssize_type ksTotalWords = static_cast<ssize_type>(kTotalWords);

    // Full words and rest bits
    static constexpr size_type kFullWords = Bits / kBitsPerWord;
    static constexpr size_type kRestBits = Bits % kBitsPerWord;

    static constexpr value_type kValue1 = static_cast<value_type>(1);

    // Need mask ?
    static constexpr bool bNeedMask = (Bits < kBitsPerWord);
    static constexpr size_type kMask = static_cast<size_type>((kValue1 << (bNeedMask ? Bits : 0)) - kValue1);

    // Class reference
    class reference {
        // proxy for an element
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

    const char * data() const noexcept {
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

	size_type count() const noexcept {
        // Count number of set bits
		const unsigned char * const bitsPerByteTable =
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
		const unsigned char * ptr = (const unsigned char *)(const void *)array_;
		const unsigned char * const last = ptr + sizeof(array_);
		size_type total_count = 0;
		for ( ; ptr != last; ptr++) {
			total_count += bitsPerByteTable[*ptr];
        }
		return total_count;
    }

    value_type get_word(ssize_type index) {
		if (index >= ksTotalWords) {
            // index out of range
			throw_index_out_of_range();
        }
        return _get_word(index);
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
		return (count() == size());
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
		tidy(static_cast<value_type>(~0));
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
		tidy();
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

private:
    inline ssize_type get_index(size_type pos) const {
        return static_cast<ssize_type>(pos / kBitsPerWord);
    }

    inline value_type get_shift(size_type pos) const {
        return static_cast<value_type>(pos % kBitsPerWord);
    }

    inline value_type _get_word(ssize_type index) {
        assert(index < ksTotalWords);
        return array_[index];
    }

    void validate() {
        //
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

    void tidy(value_type value = 0) {
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

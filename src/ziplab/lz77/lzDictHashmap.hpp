#ifndef ZIPLAB_LZ77_LZDICTHASHMAP_HPP
#define ZIPLAB_LZ77_LZDICTHASHMAP_HPP

#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "ziplab/lz77/lz77.hpp"

namespace ziplab {

template <typename T, std::size_t SizeBits>
class LZDictHashmap {
public:
    using offset_type = T;
    using size_type = std::size_t;

    static constexpr size_type kCapacity = static_cast<size_type>(1) << SizeBits;
    static constexpr size_type kOffsetMask = static_cast<size_type>(kCapacity - 1);

    static_assert((SizeBits >= 1), "LZDictHashmap<T, N>'s SizeBits must be greater than or equal to 1.");

    static_assert((kCapacity >= 2), "LZDictHashmap<T, N>'s capacity must be greater than or equal to 2.");
    // Always satisfied
    static_assert(((kCapacity & (kCapacity - 1)) == 0), "LZDictHashmap<T, N>'s capacity must be is a power of 2.");

private:
    offset_type *   prev_;
    offset_type *   head_;

public:
    LZDictHashmap()
        : prev_(nullptr), 
          head_(nullptr) {
        init(kCapacity);
    }

    ~LZDictHashmap() {
        destroy();
    }

    size_type capacity() const {
        return kCapacity;
    }

    offset_type * prev() {
        return prev_;
    }

    offset_type * head() {
        return head_;
    }

    const offset_type * prev() const {
        return prev_;
    }

    const offset_type * head() const {
        return head_;
    }

    offset_type prev(offset_type offset) const {
        offset &= kOffsetMask;
        assert(head_ != nullptr);
        return *(prev_ + offset);
    }

    offset_type head(offset_type offset) const {
        offset &= kOffsetMask;
        assert(head_ != nullptr);
        return *(head_ + offset);
    }

private:
    void init(size_type capacity) {
        assert(capacity > 0);
        assert((capacity & (capacity - 1)) == 0);
        offset_type * new_data = new offset_type[capacity * 2];
        prev_ = new_data;
        head_ = new_data + capacity;
    }

    void destroy() {
        if (prev_ != nullptr) {
            delete[] prev_;
            prev_ = nullptr;
        }
    }
};

} // namespace ziplab

#endif // ZIPLAB_LZ77_LZDICTHASHMAP_HPP

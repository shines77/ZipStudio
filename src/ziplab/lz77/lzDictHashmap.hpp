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

template <typename T>
class LZDictHashmap {
public:
    using offset_type = T;
    using size_type = std::size_t;

private:
    size_type       mask_;
    offset_type *   prev_;
    offset_type *   head_;

public:
    LZDictHashmap(size_type capacity)
        : mask_(static_cast<size_type>(capacity - 1)),
          prev_(nullptr), 
          head_(nullptr) {
        if (capacity == 0) {
            throw std::runtime_error("LZDictHashmap<T>'s capacity must be greater than 0.");
        }
        if ((capacity & (capacity - 1)) != 0) {
            throw std::runtime_error("LZDictHashmap<T>'s capacity must be is a power of 2.");
        }
        init(capacity);
    }

    ~LZDictHashmap() {
        destroy();
    }

    size_type capacity() const {
        return (mask_ + 1);
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
        offset &= mask_;
        assert(head_ != nullptr);
        return *(prev_ + offset);
    }

    offset_type head(offset_type offset) const {
        offset &= mask_;
        assert(head_ != nullptr);
        return *(head_ + offset);
    }

private:
    void init(size_type capacity) {
        assert(capacity > 0);
        assert((capacity & (capacity - 1)) == 0);
        offset_type * new_data = new offset_type[capacity * 2];
        mask_ = static_cast<size_type>(capacity - 1);
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

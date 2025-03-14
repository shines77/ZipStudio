#ifndef ZIPLAB_STREAM_INPUTSTREAM_HPP
#define ZIPLAB_STREAM_INPUTSTREAM_HPP

#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>

#include <assert.h>

namespace ziplab {

class InputStream
{
public:
    using size_type     = std::size_t;
    using diff_type     = std::ptrdiff_t;
    using pos_type      = std::size_t;
    using offset_type   = std::intptr_t;

private:
    const char * data_;
    size_type    size_;

public:
    InputStream() : data_(nullptr), size_(0) {
    }
    InputStream(size_type capacity) : data_(nullptr), size_(0) {
        reserve(capacity);
    }

    InputStream(const InputStream & src) : data_(nullptr), size_(0) {
        copy_data(src);
    }
    InputStream(InputStream && src) : data_(nullptr), size_(0) {
        swap(src);
    }

    ~InputStream() {
        destroy();
    }

    bool is_valid() const { return (data() != nullptr); }
    bool is_empty() const { return (size() == 0); }

    char * data() { return const_cast<char *>(data_); }
    const char * data() const { return data_; }

    size_type size() const { return size_; }

    void destroy() {
        release();
    }

    void reserve(size_type capacity) {
        if (capacity > size()) {
            destroy();
            allocate(capacity);
        }
    }

    void resize(size_type new_size) {
        if (new_size != size() && new_size > 0) {
            destroy();
            allocate(new_size);
            clear_data();
        }
    }

    void clear() {
        if (data() != nullptr && size() > 0) {
            clear_data();
        }
    }

    void copy(const InputStream & src) {
        if (std::addressof(src) != this) {
            destroy();
            copy_data(src);
        }
    }

    void swap(InputStream & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

private:
    const char * allocate(size_type capacity) {
        assert(data_ == nullptr);
        assert(size_ == 0);
        const char * new_data = new char[capacity];
        data_ = new_data;
        size_ = capacity;
        return new_data;
    }

    void release() {
        if (data_ != nullptr) {
            delete[] data_;
            data_ = nullptr;
            size_ = 0;
        }
    }

    inline void clear_data() {
        assert(data() != nullptr);
        assert(size() > 0);
        std::memset((void *)data(), 0, size());
    }

    inline void copy_data(const InputStream & src) {
        assert(data() == nullptr);
        assert(size() == 0);
        if (src.data() != nullptr && src.size() != 0) {
            size_type new_size = src.size();
            const char * new_data = allocate(new_size);
            std::memcpy((void *)new_data, (const void *)src.data(), new_size);
        }
    }

    inline void swap_data(InputStream & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->data_, other.data_);
        swap(this->size_, other.size_);
    }
};

} // namespace ziplab

#endif // ZIPLAB_STREAM_INPUTSTREAM_HPP

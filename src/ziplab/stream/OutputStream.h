#ifndef ZIPLAB_STREAM_OUTPUTSTREAM_HPP
#define ZIPLAB_STREAM_OUTPUTSTREAM_HPP

#pragma once

#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

#include "ziplab/basic/stddef.h"
#include "ziplab/stream/MemoryBuffer.h"

namespace ziplab {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class BasicOutputStream
{
public:
    using char_type     = CharT;
    using traits_type   = Traits;

    using size_type     = std::size_t;
    using diff_type     = std::ptrdiff_t;
    using int_type      = typename traits_type::int_type;
    using pos_type      = typename traits_type::pos_type;
    using offset_type   = typename traits_type::off_type;

    using string_type = std::basic_string<char_type, traits_type>;
    using vector_type = std::vector<char_type>;

    using memory_buffer_t = BasicMemoryBuffer<char_type, traits_type>;

private:
    const char * data_;
    size_type    size_;

public:
    BasicOutputStream() : data_(nullptr), size_(0) {
    }
    BasicOutputStream(size_type capacity) : data_(nullptr), size_(0) {
        reserve(capacity);
    }

    BasicOutputStream(const BasicOutputStream & src) : data_(nullptr), size_(0) {
        copy_data(src);
    }
    BasicOutputStream(BasicOutputStream && src) : data_(nullptr), size_(0) {
        swap(src);
    }

    ~BasicOutputStream() {
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

    void copy(const BasicOutputStream & src) {
        if (std::addressof(src) != this) {
            destroy();
            copy_data(src);
        }
    }

    void swap(BasicOutputStream & other) {
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

    inline void copy_data(const BasicOutputStream & src) {
        assert(data() == nullptr);
        assert(size() == 0);
        if (src.data() != nullptr && src.size() != 0) {
            size_type new_size = src.size();
            const char * new_data = allocate(new_size);
            std::memcpy((void *)new_data, (const void *)src.data(), new_size);
        }
    }

    inline void swap_data(BasicOutputStream & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->data_, other.data_);
        swap(this->size_, other.size_);
    }
};

using OutputStream = BasicOutputStream<char, std::char_traits<char>>;
using WOutputStream = BasicOutputStream<wchar_t, std::char_traits<wchar_t>>;

} // namespace ziplab

#endif // ZIPLAB_STREAM_OUTPUTSTREAM_HPP

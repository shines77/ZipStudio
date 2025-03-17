#ifndef ZIPLAB_STREAM_INPUTSTREAM_HPP
#define ZIPLAB_STREAM_INPUTSTREAM_HPP

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

#include "ziplab/stream/MemoryBuffer.h"

namespace ziplab {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class BasicInputStream
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
    memory_buffer_t buffer_;

public:
    BasicInputStream() : buffer_() {
    }
    BasicInputStream(size_type capacity) : buffer_(capacity) {
    }

    BasicInputStream(const memory_buffer_t & src) : buffer_(src) {
    }
    BasicInputStream(memory_buffer_t && src) : buffer_(std::forward<memory_buffer_t>(src)) {
    }

    BasicInputStream(const char_type * data, size_type size) : buffer_(data, size) {
    }

    template <size_type N>
    BasicInputStream(const char_type (&data)[N]) : buffer_(data, N) {
    }

    template <size_type N>
    BasicInputStream(const std::array<string_type, N> & strings) : buffer_(strings) {
    }

    BasicInputStream(const string_type & src) : buffer_(src) {
    }

    BasicInputStream(const vector_type & src) : buffer_(src) {
    }

    template <typename Container>
    BasicInputStream(const Container & src) : buffer_(src) {
    }

    BasicInputStream(const BasicInputStream & src) : buffer_(src.buffer()) {
    }
    BasicInputStream(BasicInputStream && src)
        : buffer_(std::forward<memory_buffer_t>(src.buffer())) {
    }

    ~BasicInputStream() {
        destroy();
    }

    bool is_valid() const { return (buffer_.data() != nullptr); }
    bool is_empty() const { return (buffer_.size() == 0); }

    char_type * data() { return const_cast<char_type *>(buffer_.data()); }
    const char_type * data() const { return buffer_.data(); }

    size_type size() const { return buffer_.size(); }

    memory_buffer_t & buffer() { return buffer_; }
    const memory_buffer_t & buffer() const { return buffer_; }

    void destroy() {
        buffer_.destroy();
    }

    void reserve(size_type new_capacity) {
        buffer_.reserve(new_capacity);
    }

    void resize(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        buffer_.resize(new_size, fill_new, init_val);
    }

    void clear() {
        buffer_.clear();
    }

    void copy(const BasicInputStream & src) {
        buffer_.copy(src);
    }

    void swap(BasicInputStream & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

private:
    inline void clear_data() {
        //
    }

    inline void copy_data(const BasicInputStream & src) {
        //
    }

    inline void swap_data(BasicInputStream & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->buffer_, other.buffer_);
    }
};

using InputStream = BasicInputStream<char, std::char_traits<char>>;
using WInputStream = BasicInputStream<wchar_t, std::char_traits<wchar_t>>;

} // namespace ziplab

#endif // ZIPLAB_STREAM_INPUTSTREAM_HPP

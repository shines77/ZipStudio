#ifndef ZIPLAB_STREAM_MEMORYVIEW_HPP
#define ZIPLAB_STREAM_MEMORYVIEW_HPP

#pragma once

#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>
#include <cstring>  // For std::strlen()
#include <memory>

#include "ziplab/basic/stddef.h"
#include "ziplab/stream/MemoryBuffer.h"

namespace ziplab {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class BasicMemoryView
{
public:
    using char_type     = CharT;
    using traits_type   = Traits;

    using memory_buffer_t = BasicMemoryBuffer<char_type, traits_type>;

    using size_type     = typename memory_buffer_t::size_type;
    using diff_type     = typename memory_buffer_t::diff_type;
    using index_type    = typename memory_buffer_t::index_type;
    using int_type      = typename traits_type::int_type;
    using pos_type      = typename traits_type::pos_type;
    using offset_type   = typename traits_type::off_type;

    using string_type = std::basic_string<char_type, traits_type>;
    using vector_type = std::vector<char_type>;

private:
    // It's allowed that when the data pointer is non-zero, the data size is zero.
    const char_type * data_;
    size_type         size_;

public:
    BasicMemoryView() : data_(nullptr), size_(0) {
    }
    BasicMemoryView(const char_type * data, size_type size)
        : data_(data), size_(size) {
    }

    template <size_type N>
    BasicMemoryView(const char_type (&data)[N])
        : data_(data), size_(N) {
    }

    BasicMemoryView(const string_type & src)
        : data_(src.c_str()), size_(src.size()) {
    }

    BasicMemoryView(const memory_buffer_t & buffer)
        : data_(buffer.data()), size_(buffer.size()) {
    }

    BasicMemoryView(const BasicMemoryView & src)
        : data_(src.data()), size_(src.size()) {
    }

    template <size_type N>
    BasicMemoryView(const std::array<string_type, N> & strings)
        : data_(static_cast<const char_type *>(strings[0])), size_(N) {
    }

    ~BasicMemoryView() {
    }

    inline bool is_valid() const { return (data() != nullptr); }
    inline bool is_empty() const { return (size() == 0); }

    char * data() { return const_cast<char *>(data_); }
    const char * data() const { return data_; }

    size_type size() const { return size_; }
    index_type ssize() const { return static_cast<index_type>(size_); }

    char_type * begin() { return data(); }
    const char_type * begin() const { return data(); }

    char_type * end() { return (data() + size()); }
    const char_type * end() const { return (data() + size()); }

    void destroy() {
        // Do nothing !
    }

    void reserve(size_type new_capacity) {
        // Do nothing !
    }

    void resize(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        // Do nothing !
    }

    void keep_reserve(size_type new_capacity) {
        // Do nothing !
    }

    void keep_resize(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        // Do nothing !
    }

    void clear() {
        if (is_valid() && !is_empty()) {
            clear_data();
        }
    }

    void swap(BasicMemoryView & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicMemoryView & lhs, BasicMemoryView & rhs) {
        lhs.swap(rhs);
    }

private:
    inline void clear_data() {
        assert(data() != nullptr);
        assert(size() > 0);
        std::memset((void *)data(), 0, size());
    }

    inline void swap_data(BasicMemoryView & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->data_, other.data_);
        swap(this->size_, other.size_);
    }
};

using MemoryView  = BasicMemoryView<char, std::char_traits<char>>;
using WMemoryView = BasicMemoryView<wchar_t, std::char_traits<wchar_t>>;

} // namespace ziplab

namespace std {

template <typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicMemoryView<CharT, Traits> & lhs,
                 ziplab::BasicMemoryView<CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_MEMORYVIEW_HPP

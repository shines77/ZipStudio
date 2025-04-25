#ifndef ZIPLAB_STREAM_IOSTREAMROOT_HPP
#define ZIPLAB_STREAM_IOSTREAMROOT_HPP

#pragma once

#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <type_traits>  // For std::forward<T>()
#include <fstream>
#include <sstream>

#include "ziplab/basic/stddef.h"
#include "ziplab/stream/MemoryBuffer.h"
#include "ziplab/stream/MemoryView.h"

namespace ziplab {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
class BasicIOStreamRoot
{
public:
    using buffer_type   = MemoryBufferT;
    using char_type     = CharT;
    using traits_type   = Traits;

    using this_type = BasicIOStreamRoot<buffer_type, char_type, traits_type>;

    using memory_buffer_t = BasicMemoryBuffer<char_type, true, traits_type>;
    using memory_view_t   = BasicMemoryView<char_type, traits_type>;

    using size_type     = typename memory_buffer_t::size_type;
    using diff_type     = typename memory_buffer_t::diff_type;
    using index_type    = typename memory_buffer_t::index_type;
    using int_type      = typename traits_type::int_type;
    using pos_type      = typename traits_type::pos_type;
    using offset_type   = typename traits_type::off_type;

    using string_type = std::basic_string<char_type, traits_type>;
    using vector_type = std::vector<char_type>;

protected:
    buffer_type & buffer_;
    index_type    pos_;

public:
    BasicIOStreamRoot(buffer_type & buffer) : buffer_(buffer), pos_(0) {
    }

    BasicIOStreamRoot(const BasicIOStreamRoot & src)
        : buffer_(src.buffer()), pos_(src.pos()) {
    }
    BasicIOStreamRoot(BasicIOStreamRoot && src)
        : buffer_(std::forward<memory_buffer_t>(src.buffer())),
          pos_(src.pos()) {
    }

    ~BasicIOStreamRoot() {
        destroy();
    }

    // Property
    bool is_valid() const { return buffer_.is_valid(); }
    bool is_empty() const { return buffer_.is_empty(); }

    char_type * data() { return buffer_.data(); }
    const char_type * data() const { return buffer_.data(); }

    size_type size() const { return buffer_.size(); }
    size_type capacity() const { return buffer_.capacity(); }
    index_type pos() const { return pos_; }

    index_type ssize() const { return buffer_.ssize(); }
    index_type scapacity() const { return buffer_.scapacity(); }
    size_type upos() const { return static_cast<size_type>(pos_); }

    memory_buffer_t & buffer() { return buffer_; }
    const memory_buffer_t & buffer() const { return buffer_; }

    // Position
    char_type * current() { return (buffer_.data() + pos()); }
    const char_type * current() const { return (buffer_.data() + pos()); }

    char_type * begin() { return buffer_.data(); }
    const char_type * begin() const { return buffer_.data(); }

    char_type * end() { return (buffer_.data() + size()); }
    const char_type * end() const { return (buffer_.data() + size()); }

    char_type * tail() { return (buffer_.data() + buffer_.size()); }
    const char_type * tail() const { return (buffer_.data() + buffer_.size()); }

    void destroy() {
        buffer_.destroy();
        reset();
    }

    void reset() {
        pos_ = 0;
    }

    size_type calc_capacity(size_type new_size) {
        size_type new_capacity = 0;
        return new_capacity;
    }

    //
    // Only ensure to reserve space for at least N elements
    // and discards existing data, without initializing new elements.
    //
    void prepare(size_type new_capacity) {
        buffer_.prepare(new_capacity);
        assert(size() <= capacity());
        assert(pos() <= ssize());
    }

    //
    // Ensure to reserve space for at least N elements
    // and preserving existing data, without initializing new elements.
    //
    void reserve(size_type new_capacity) {
        buffer_.reserve(new_capacity);
        assert(size() <= capacity());
        assert(pos() <= ssize());
    }

    //
    // Expand space for delta_size elements
    // and preserving existing data, without initializing new elements.
    //
    bool grow(size_type delta_size) {
        return buffer_.grow(delta_size);
    }

    //
    // Expand the space to at least N elements
    // and discards existing data, initialize new elements with default values.
    //
    // equivalent to: clear() and resize(n).
    //
    // In C++ 23, similar method is void resize_and_overwrite(size_type n, F && generator).
    //
    void resize_discard(size_type new_size, char_type init_val = 0) {
        buffer_.resize_discard(new_size, init_val);
        reset();
    }

    //
    // Expand the space to at least N elements
    // and preserving existing data, initialize new elements with default values.
    //
    void resize(size_type new_size, char_type init_val = 0) {
        buffer_.resize(new_size, init_val);
        if (pos() > ssize()) {
            pos_ = ssize();
        }
    }

    void clear() {
        buffer_.clear();
        reset();
    }

    void copy(const BasicIOStreamRoot & src) {
        if (std::addressof(src) != this) {
            copy_data(src);
        }
    }

    void swap(BasicIOStreamRoot & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicIOStreamRoot & lhs, BasicIOStreamRoot & rhs) {
        lhs.swap(rhs);
    }

    bool is_underflow() const {
        return (pos() < 0);
    }

    template <typename T>
    bool is_underflow(const T & val) const {
        ZIPLAB_UNUSED(val);
        return ((pos() - sizeof(val)) < 0);
    }

    bool is_overflow() const {
        return (pos() > ssize());
    }

    template <typename T>
    bool is_overflow(const T & val) const {
        ZIPLAB_UNUSED(val);
        return ((pos() + sizeof(val)) > ssize());
    }

    // Cursor
    this_type & backward(size_type step) {
        pos_ -= static_cast<index_type>(step);
        return *this;
    }

    this_type & forward(size_type step) {
        pos_ += static_cast<index_type>(step);
        return *this;
    }

    this_type & rewind(offset_type offset) {
        pos_ -= static_cast<index_type>(offset);
        return *this;
    }

    this_type & skip(offset_type offset) {
        pos_ += static_cast<index_type>(offset);
        return *this;
    }

    void seek_to_begin() {
        pos_ = 0;
    }

    void seek_to_end() {
        pos_ = ssize();
    }

    void seek_to_tail() {
        pos_ = scapacity();
    }

    void seek_to(index_type pos) {
        pos_ = pos;
    }

protected:
    inline void clear_data() {
        //
    }

    inline void copy_data(const BasicIOStreamRoot & src) {
        assert(std::addressof(src) != this);
        buffer_.copy(src.buffer());
        pos_ = src.pos();
    }

    inline void swap_data(BasicIOStreamRoot & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->buffer_, other.buffer_);
        swap(this->pos_, other.pos_);
        swap(this->size_, other.size_);
    }

private:
    //
};

using IOStreamRoot  = BasicIOStreamRoot< BasicMemoryBuffer<char, true, std::char_traits<char> >, char, std::char_traits<char>>;
using WIOStreamRoot = BasicIOStreamRoot< BasicMemoryBuffer<wchar_t, true, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

using IOStreamRootView  = BasicIOStreamRoot< BasicMemoryView<char, std::char_traits<char> >, char, std::char_traits<char>>;
using WIOStreamRootView = BasicIOStreamRoot< BasicMemoryView<wchar_t, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

} // namespace ziplab

namespace std {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicIOStreamRoot<MemoryBufferT, CharT, Traits> & lhs,
                 ziplab::BasicIOStreamRoot<MemoryBufferT, CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_IOSTREAMROOT_HPP

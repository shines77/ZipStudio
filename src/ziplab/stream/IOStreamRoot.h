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

#if USE_MEMORY_STORAGE
    using memory_buffer_t = BasicMemoryBuffer< BasicMemoryStorage<char_type, traits_type> >;
    using memory_view_t   = BasicMemoryView< BasicMemoryStorage<char_type, traits_type> >;
#else
    using memory_buffer_t = BasicMemoryBuffer<char_type, traits_type>;
    using memory_view_t   = BasicMemoryView<char_type, traits_type>;
#endif

    using size_type     = typename memory_buffer_t::size_type;
    using diff_type     = typename memory_buffer_t::diff_type;
    using index_type    = typename memory_buffer_t::index_type;
    using int_type      = typename traits_type::int_type;
    using pos_type      = typename traits_type::pos_type;
    using offset_type   = typename traits_type::off_type;

    using string_type = std::basic_string<char_type, traits_type>;
    using vector_type = std::vector<char_type>;

protected:
    buffer_type buffer_;
    index_type  pos_;
    size_type   size_;

public:
    BasicIOStreamRoot() : buffer_(), pos_(0) {
    }
    BasicIOStreamRoot(size_type capacity) : buffer_(capacity), pos_(0) {
    }

    BasicIOStreamRoot(const memory_buffer_t & buffer)
        : buffer_(buffer), pos_(0) {
    }
    BasicIOStreamRoot(memory_buffer_t && buffer)
        : buffer_(std::forward<memory_buffer_t>(buffer)), pos_(0) {
    }

    BasicIOStreamRoot(const char_type * data, size_type size)
        : buffer_(data, size), pos_(0) {
    }

    template <size_type N>
    BasicIOStreamRoot(const char_type (&data)[N])
        : buffer_(data, N), pos_(0) {
    }

    template <size_type N>
    BasicIOStreamRoot(const std::array<string_type, N> & strings)
        : buffer_(strings), pos_(0) {
    }

    BasicIOStreamRoot(const string_type & src)
        : buffer_(src), pos_(0) {
    }

    BasicIOStreamRoot(const vector_type & src)
        : buffer_(src), pos_(0) {
    }

    template <typename Container>
    BasicIOStreamRoot(const Container & src)
        : buffer_(src), pos_(0) {
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
    char_type * begin() { return buffer_.data(); }
    const char_type * begin() const { return buffer_.data(); }

    char_type * end() { return (buffer_.data() + size()); }
    const char_type * end() const { return (buffer_.data() + size()); }

    char_type * tail() { return (buffer_.data() + buffer_.size()); }
    const char_type * tail() const { return (buffer_.data() + buffer_.size()); }

    char_type * current() { return (buffer_.data() + pos()); }
    const char_type * current() const { return (buffer_.data() + pos()); }

    void destroy() {
        buffer_.destroy();
        reset();
    }

    void reset() {
        pos_ = 0;
    }

    void reserve(size_type new_capacity) {
        buffer_.reserve(new_capacity);
        assert(size() <= capacity());
        assert(pos() <= ssize());
    }

    void resize(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        buffer_.resize(new_size, fill_new, init_val);
        reset();
    }

    void keep_reserve(size_type new_capacity) {
        buffer_.keep_reserve(new_capacity);
        assert(size() <= capacity());
        assert(pos() <= ssize());        
    }

    void keep_resize(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        buffer_.keep_resize(new_size, fill_new, init_val);
        if (size() > capacity()) {
            size_ = capacity();
        }
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

    index_type rewind(offset_type offset) {
        pos_ -= static_cast<index_type>(offset);
        return pos_;
    }

    index_type skip(offset_type offset) {
        pos_ += static_cast<index_type>(offset);
        return pos_;
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

#if USE_MEMORY_STORAGE

using IOStreamRoot  = BasicIOStreamRoot< BasicMemoryBuffer< BasicMemoryStorage<char, std::char_traits<char>> >, char, std::char_traits<char> >;
using WIOStreamRoot = BasicIOStreamRoot< BasicMemoryBuffer< BasicMemoryStorage<wchar_t, std::char_traits<wchar_t>> >, wchar_t, std::char_traits<wchar_t> >;

using IOStreamRootView  = BasicIOStreamRoot< BasicMemoryView< BasicMemoryStorage<char, std::char_traits<char>> >, char, std::char_traits<char>>;
using WIOStreamRootView = BasicIOStreamRoot< BasicMemoryView< BasicMemoryStorage<wchar_t, std::char_traits<wchar_t>> >, wchar_t, std::char_traits<wchar_t> >;

#else

using IOStreamRoot  = BasicIOStreamRoot< BasicMemoryBuffer<char, std::char_traits<char> >, char, std::char_traits<char>>;
using WIOStreamRoot = BasicIOStreamRoot< BasicMemoryBuffer<wchar_t, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

using IOStreamRootView  = BasicIOStreamRoot< BasicMemoryView<char, std::char_traits<char> >, char, std::char_traits<char>>;
using WIOStreamRootView = BasicIOStreamRoot< BasicMemoryView<wchar_t, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

#endif

} // namespace ziplab

namespace std {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicIOStreamRoot<MemoryBufferT, CharT, Traits> & lhs,
                 ziplab::BasicIOStreamRoot<MemoryBufferT, CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_IOSTREAMROOT_HPP

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

    static constexpr size_type nend = static_cast<size_type>(-1);

protected:
    buffer_type & buffer_;

public:
    BasicIOStreamRoot(buffer_type & buffer) : buffer_(buffer) {
    }

    BasicIOStreamRoot(const BasicIOStreamRoot & src)
        : buffer_(src.buffer()) {
    }
    BasicIOStreamRoot(BasicIOStreamRoot && src)
        : buffer_(std::forward<memory_buffer_t>(src.buffer())) {
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

    index_type ssize() const { return buffer_.ssize(); }
    index_type scapacity() const { return buffer_.scapacity(); }

    memory_buffer_t & buffer() { return buffer_; }
    const memory_buffer_t & buffer() const { return buffer_; }

    // Position
    char_type * begin() { return buffer_.begin(); }
    const char_type * begin() const { return buffer_.begin(); }

    char_type * end() { return buffer_.end(); }
    const char_type * end() const { return buffer_.end(); }

    char_type * current() { return buffer_.current(); }
    const char_type * current() const { return buffer_.current(); }

    char_type * tail() { return buffer_.tail() }
    const char_type * tail() const { return buffer_.tail(); }

    void destroy() {
        buffer_.destroy();
    }

    //
    // Only ensure to reserve space for at least N elements
    // and discards existing data, without initializing new elements.
    //
    void prepare(size_type new_capacity) {
        buffer_.prepare(new_capacity);
    }

    //
    // Ensure to reserve space for at least N elements
    // and preserving existing data, without initializing new elements.
    //
    void reserve(size_type new_capacity) {
        buffer_.reserve(new_capacity);
    }

    //
    // Expand space for delta_size elements
    // and preserving existing data, without initializing new elements.
    //
    void grow(size_type delta_size) {
        buffer_.grow(delta_size);
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
    }

    //
    // Expand the space to at least N elements
    // and preserving existing data, initialize new elements with default values.
    //
    void resize(size_type new_size, char_type init_val = 0) {
        buffer_.resize(new_size, init_val);
    }

    void clear() {
        buffer_.clear();
    }

    void clear_all() {
        buffer_.clear_all();
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

    // Bound checking
    bool is_overflow() const {
        return buffer_.is_overflow();
    }

    template <typename T>
    bool is_overflow(const T & val) const {
        ZIPLAB_UNUSED(val);
        return buffer_.is_overflow(val);
    }

    // Unsafe write
    inline void unsafeWrite(const char_type * data, size_type size) {
        buffer_.unsafeWrite(data, size);
    }

    template <typename Allocator>
    void unsafeWrite(const std::basic_string<char_type, traits_type, Allocator> & str) {
        unsafeWrite(str.c_str(), str.size());
    }

    template <typename StringViewLike>
    void unsafeWrite(const StringViewLike & sv) {
        unsafeWrite(sv.c_str(), sv.size());
    }

    void unsafeWrite(const memory_buffer_t & buffer) {
        unsafeWrite(buffer.data(), buffer.size());
    }

    void unsafeWrite(const memory_view_t & buffer) {
        unsafeWrite(buffer.data(), buffer.size());
    }

    template <typename Allocator>
    void unsafeWrite(const std::basic_string<char_type, traits_type, Allocator> & str,
                     size_type pos, size_type count = nend) {
        if (pos < str.size()) {
            size_type remaining = static_cast<size_type>(str.size() - pos);
            if (count == nend)
                count = remaining;
            else
                count = (std::min)(remaining, count);
            unsafeWrite(str.c_str() + pos, count);
        }
    }

    template <typename StringViewLike>
    void unsafeWrite(const StringViewLike & sv, size_type pos, size_type count = nend) {
        if (pos < sv.size()) {
            size_type remaining = static_cast<size_type>(sv.size() - pos);
            if (count == nend)
                count = remaining;
            else
                count = (std::min)(remaining, count);
            unsafeWrite(sv.c_str() + pos, count);
        }
    }

    // Safety write
    void write(const char_type * data, size_type size) {
        buffer_.write(data, size);
    }

    template <typename Allocator>
    void write(const std::basic_string<char_type, traits_type, Allocator> & str) {
        write(str.c_str(), str.size());
    }

    void write(const memory_buffer_t & buffer) {
        write(buffer.data(), buffer.size());
    }

    void write(const memory_view_t & buffer) {
        write(buffer.data(), buffer.size());
    }

    template <typename Allocator>
    void write(const std::basic_string<char_type, traits_type, Allocator> & str,
               size_type pos, size_type count = nend) {
        if (pos < str.size()) {
            size_type remaining = static_cast<size_type>(str.size() - pos);
            if (count == nend)
                count = remaining;
            else
                count = (std::min)(remaining, count);
            write(str.c_str() + pos, count);
        }
    }

    template <typename StringViewLike>
    void write(const StringViewLike & sv, size_type pos, size_type count = nend) {
        if (pos < sv.size()) {
            size_type remaining = static_cast<size_type>(sv.size() - pos);
            if (count == nend)
                count = remaining;
            else
                count = (std::min)(remaining, count);
            write(sv.c_str() + pos, count);
        }
    }

protected:
    inline void copy_data(const BasicIOStreamRoot & src) {
        assert(std::addressof(src) != this);
        buffer_.copy(src.buffer());
    }

    inline void swap_data(BasicIOStreamRoot & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->buffer_, other.buffer_);
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

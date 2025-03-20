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
#include "ziplab/stream/MemoryStorage.h"
#include "ziplab/stream/MemoryBuffer.h"

namespace ziplab {

template <typename MemoryStorageT>
class BasicMemoryView : public MemoryStorageT
{
public:
#if USE_MEMORY_STORAGE
    using memory_storage_t  = MemoryStorageT;
    using super_type        = MemoryStorageT;

    using char_type     = typename super_type::char_type;
    using traits_type   = typename super_type::traits_type;

    using size_type     = typename super_type::size_type;
    using diff_type     = typename super_type::diff_type;
    using index_type    = typename super_type::index_type;

    using this_type       = BasicMemoryView<memory_storage_t>;
    using memory_buffer_t = BasicMemoryBuffer<memory_storage_t>;
#else
    using char_type     = CharT;
    using traits_type   = Traits;

    using size_type     = std::size_type;
    using diff_type     = std::ptrdiff_t;
    using index_type    = std::intptr_t;

    using this_type       = BasicMemoryView<char_type, traits_type>;
    using memory_buffer_t = BasicMemoryBuffer<char_type, traits_type>;
#endif

    using int_type      = typename traits_type::int_type;
    using pos_type      = typename traits_type::pos_type;
    using offset_type   = typename traits_type::off_type;

    using string_type   = std::basic_string<char_type, traits_type>;
    using vector_type   = std::vector<char_type>;

private:
#if !defined(USE_MEMORY_STORAGE) || (USE_MEMORY_STORAGE == 0)
    // It's allowed that when the data pointer is non-zero, the data size is zero.
    const char_type * data_;
    size_type         size_;
#endif

public:
    BasicMemoryView() : super_type() {
    }
    BasicMemoryView(const char_type * data, size_type size)
        : super_type(data, size) {
    }
    BasicMemoryView(const char_type * data, size_type size, size_type capacity)
        : super_type(data, size, capacity) {
    }

    template <size_type N>
    BasicMemoryView(const char_type (&data)[N])
        //: data_(data), size_(N) {
        : super_type(data, N) {
    }

    BasicMemoryView(const string_type & src)
        //: data_(src.c_str()), size_(src.size()) {
        : super_type(src.c_str(), src.size(), src.capacity()) {
    }

    BasicMemoryView(const memory_buffer_t & buffer)
        //: data_(buffer.data()), size_(buffer.size()) {
        : super_type(buffer.storage()) {
    }

    BasicMemoryView(const BasicMemoryView & src)
        //: data_(src.data()), size_(src.size()) {
        : super_type(src.storage()) {
    }

    template <size_type N>
    BasicMemoryView(const std::array<string_type, N> & strings)
        //: data_(static_cast<const char_type *>(strings[0])), size_(N) {
        : super_type(static_cast<const char_type *>(strings[0]), N) {
    }

    ~BasicMemoryView() {
    }

#if USE_MEMORY_STORAGE
    super_type & storage() {
        return *static_cast<super_type *>(this);
    }

    const super_type & storage() const {
        return *static_cast<super_type *>(const_cast<this_type *>(this);
    }
#endif

#if 0
    inline bool is_valid() const { return (data() != nullptr); }
    inline bool is_empty() const { return (size() == 0); }

    char_type * data() { return const_cast<char_type *>(data_); }
    const char_type * data() const { return data_; }

    size_type size() const { return size_; }
    index_type ssize() const { return static_cast<index_type>(size_); }

    char_type * begin() { return data(); }
    const char_type * begin() const { return data(); }

    char_type * end() { return (data() + size()); }
    const char_type * end() const { return (data() + size()); }
#endif

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
        std::memset((void *)data(), 0, capacity() * sizeof(char_type));
    }

    inline void swap_data(BasicMemoryView & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->storage(), other.storage());
    }
};

using MemoryView  = BasicMemoryView< BasicMemoryStorage<char, std::char_traits<char>> >;
using WMemoryView = BasicMemoryView< BasicMemoryStorage<wchar_t, std::char_traits<wchar_t>> >;

using MutableMemoryView  = BasicMemoryView< BasicMutableMemoryStorage<char, std::char_traits<char>> >;
using WMutableMemoryView = BasicMemoryView< BasicMutableMemoryStorage<wchar_t, std::char_traits<wchar_t>> >;

} // namespace ziplab

namespace std {

#if USE_MEMORY_STORAGE

template <typename MemoryStorageT>
inline void swap(ziplab::BasicMemoryView<MemoryStorageT> & lhs,
                 ziplab::BasicMemoryView<MemoryStorageT> & rhs) {
    lhs.swap(rhs);
}

#else

template <typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicMemoryView<CharT, Traits> & lhs,
                 ziplab::BasicMemoryView<CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

#endif

} // namespace std

#endif // ZIPLAB_STREAM_MEMORYVIEW_HPP

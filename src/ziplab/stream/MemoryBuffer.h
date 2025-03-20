#ifndef ZIPLAB_STREAM_MEMORYBUFFER_HPP
#define ZIPLAB_STREAM_MEMORYBUFFER_HPP

#pragma once

#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>
#include <cstring>  // For std::strlen()
#include <memory>
#include <limits>   // For std::min(), std::max()

#include "ziplab/basic/stddef.h"
#include "ziplab/stream/MemoryStorage.h"

namespace ziplab {

// Forward declaration
template <typename MemoryStorageT>
class BasicMemoryView;

#if USE_MEMORY_STORAGE
template <typename MemoryStorageT>
class BasicMemoryBuffer : public MemoryStorageT
#else
template <typename CharT, typename Traits = std::char_traits<CharT>>
class BasicMemoryBuffer
#endif
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

    using this_type     = BasicMemoryBuffer<memory_storage_t>;
    using memory_view_t = BasicMemoryView<memory_storage_t>;
#else
    using char_type     = CharT;
    using traits_type   = Traits;

    using size_type     = std::size_type;
    using diff_type     = std::ptrdiff_t;
    using index_type    = std::intptr_t;

    using this_type     = BasicMemoryBuffer<char_type, traits_type>;
    using memory_view_t = BasicMemoryView<char_type, traits_type>;
#endif // USE_MEMORY_STORAGE

    using int_type      = typename traits_type::int_type;
    using pos_type      = typename traits_type::pos_type;
    using offset_type   = typename traits_type::off_type;

    using string_type = std::basic_string<char_type, traits_type>;
    using vector_type = std::vector<char_type>;

private:
#if !defined(USE_MEMORY_STORAGE) || (USE_MEMORY_STORAGE == 0)
    // It's allowed that when the data pointer is non-zero, the data size is zero.

    // Why change the order of member variables data and size,
    // because it is for higher efficiency in the input and output streams.
    size_type         size_;
    const char_type * data_;
#endif

public:
#if USE_MEMORY_STORAGE
    BasicMemoryBuffer() : super_type() {
    }
#else
    BasicMemoryBuffer() : size_(0), data_(nullptr) {
    }
#endif // USE_MEMORY_STORAGE

    BasicMemoryBuffer(size_type capacity) : BasicMemoryBuffer() {
        if (capacity > 0) {
            reserve_impl<true, false>(capacity);
        }
    }

    BasicMemoryBuffer(const memory_view_t & src) : BasicMemoryBuffer() {
        assgin_and_copy_data<true>(src.data(), src.size());
    }

    BasicMemoryBuffer(const char_type * data, size_type size) : BasicMemoryBuffer() {
        assgin_and_copy_data<true>(data, size);
    }

    template <size_type N>
    BasicMemoryBuffer(const char_type (&data)[N]) : BasicMemoryBuffer() {
        assgin_and_copy_data<true>(data, N);
    }

    template <size_type N>
    BasicMemoryBuffer(const std::array<string_type, N> & strings) : BasicMemoryBuffer() {
        assgin_and_copy_data_from_container<true>(strings);
    }

    BasicMemoryBuffer(const string_type & src) : BasicMemoryBuffer() {
        assgin_and_copy_data<true>(src.c_str(), src.size());
    }

    BasicMemoryBuffer(const vector_type & src) : BasicMemoryBuffer() {
        assgin_and_copy_data_from_container<true>(src);
    }

    template <typename Container>
    BasicMemoryBuffer(const Container & src) : BasicMemoryBuffer() {
        assgin_and_copy_data_from_container<true>(src);
    }

    BasicMemoryBuffer(const BasicMemoryBuffer & src) : BasicMemoryBuffer() {
        assgin_and_copy_data<true>(src.data(), src.size());
    }
    BasicMemoryBuffer(BasicMemoryBuffer && src) : BasicMemoryBuffer() {
        swap_data(src);
    }

    // Allow the use of polymorphism when deriving from this class.
    ~BasicMemoryBuffer() {
        destroy();
    }

#if USE_MEMORY_STORAGE
    super_type & storage() {
        return *static_cast<super_type *>(this);
    }

    const super_type & storage() const {
        return *static_cast<super_type *>(const_cast<this_type *>(this));
    }
#endif

#if !defined(USE_MEMORY_STORAGE) || (USE_MEMORY_STORAGE == 0)
    bool is_valid() const { return (data() != nullptr); }
    bool is_empty() const { return (size() == 0); }

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
        if (this->data_ != nullptr) {
            delete[] this->data_;
            this->data_ = nullptr;
            this->setCapacity(0);
        }
    }

    // Ensure reserved space of the specified size, but do not perform any data initialization,
    // and don't keep the old data.
    void reserve(size_type new_capacity) {
        // If the new capacity is less than or equal to the old size, do nothing!
        if (new_capacity > this->size()) {
            // Allow the new capacity is 0.
            reserve_impl<false, false>(new_capacity);
        }
    }

    // Allocate a new buffer of specified size and don't keep the old data.
    void resize(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        resize_impl<false>(new_size, fill_new, init_val);
    }

    // Ensure reserved space of the specified size, but do not perform any data initialization,
    // and keep the old data.
    void keep_reserve(size_type new_capacity) {
        // If the new capacity is less than or equal to the old size, do nothing!
        if (new_capacity > this->size()) {
            // Allow the new capacity is 0.
            reserve_impl<false, true>(new_capacity);
        }
    }

    // Allocate a new buffer of specified size and keep the old data.
    void keep_resize(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        resize_impl<true>(new_size, fill_new, init_val);
    }

    void clear() {
        if (this->is_valid() && !this->is_empty()) {
            clear_data();
        }
    }

    template <size_type N>
    void copy(const char_type (&src_data)[N]) {
        copy_impl(this->data(), this->size(), src_data, N);
    }

    void copy(const char_type * src_data, size_type src_size) {
        copy_impl(this->data(), this->size(), src_data, src_size);
    }

    void copy(const BasicMemoryBuffer & src) {
        if (std::addressof(src) != this) {
            copy(src.data(), src.size());
        }
    }

    void copy(const string_type & src) {
        copy(src.c_str(), src.size());
    }

    void copy(const vector_type & src) {
        copy_from_container_impl(this->data(), this->size(), src);
    }

    template <typename Container>
    void copy_from_container(const Container & src) {
        copy_from_container_impl(this->data(), this->size(), src);
    }

    void swap(BasicMemoryBuffer & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicMemoryBuffer & lhs, BasicMemoryBuffer & rhs) {
        lhs.swap(rhs);
    }

private:
    inline const char_type * allocate(size_type capacity) {
        // It's allowed that when the data pointer is non-zero, the data size is zero.
        assert(capacity >= 0);
        const char_type * new_data = new char_type[capacity];
        assert(new_data != nullptr);
        return new_data;
    }

    inline void deallocate() {
        assert(this->data_ != nullptr);
        delete[] this->data_;
    }

    template <bool isInitialize, bool needReserve>
    inline void reserve_impl(size_type new_capacity) {
        // Allow the new capacity is 0.
        const char_type * new_data = allocate(new_capacity);
        size_type copy_size = (std::min)(new_capacity, this->size());
        // If necessary, copy the old data to new data.
        if (!isInitialize && this->is_valid()) {
            if (needReserve && (copy_size > 0)) {
                assert(new_data != nullptr);
                std::memcpy((void *)new_data, (const void *)this->data(), copy_size * sizeof(char_type));
            }
            deallocate();
        }
        this->data_ = new_data;
#if USE_MEMORY_STORAGE
        this->setCapacity(new_capacity);
#else
        this->size_ = new_capacity;
#endif
    }

    // Allocate a new buffer of specified size and keep the old data.
    template <bool needReserve>
    void resize_impl(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        // If the new size is equal to the old size, do nothing!
        if (new_size != this->size()) {
            // Allow the new size is 0.
            const char_type * new_data = allocate(new_size);
            size_type copy_size;
            if (needReserve)
                copy_size = (std::min)(new_size, this->size());
            else
                copy_size = 0;
            // If necessary, copy the old data to new data.
            if (this->is_valid()) {
                if (needReserve && (copy_size > 0)) {
                    assert(new_data != nullptr);
                    std::memcpy((void *)new_data, (const void *)this->data(), copy_size * sizeof(char_type));
                }
                deallocate();
            }
            // If necessary, fill the remaining part with the specified default value.
            if (fill_new && (new_size > copy_size)) {
                size_type remain_size = new_size - copy_size;
                assert(new_data != nullptr);
                assert(remain_size > 0);
                std::memset((void *)(new_data + copy_size), (int)init_val, remain_size * sizeof(char_type));
            }
            this->data_ = new_data;
#if USE_MEMORY_STORAGE
            this->setCapacity(new_size);
#else
            this->size_ = new_size;
#endif
        } else {
            // If necessary, fill the remaining part with the specified default value.
            if (fill_new && (this->is_valid() && !this->is_empty())) {
                assert(this->data() != nullptr);
                assert(this->size() > 0);
                assert(new_size == this->size());
                std::memset((void *)this->data(), (int)init_val, this->size() * sizeof(char_type));
            }
        }
    }

    inline void clear_data() {
        assert(this->data() != nullptr);
        assert(this->size() > 0);
        std::memset((void *)this->data(), 0, this->size() * sizeof(char_type));
    }

    inline void copy_data(const char_type * dest_data, size_type dest_size,
                          const char_type * src_data, size_type src_size) {
        ZIPLAB_UNUSED(dest_size);
        // If the source data is not null and is not empty, then copy the data.
        if (src_data != nullptr && src_size > 0) {
            assert(dest_data != nullptr);
            assert(dest_size > 0);
            assert(dest_size == src_size);
            std::memcpy((void *)dest_data, (const void *)src_data, src_size * sizeof(char_type));
        }
    }

    template <typename Container>
    inline void copy_data_from_container(const Container & src) {
        using const_iterator = typename Container::const_iterator;
        assert(this->data() != nullptr);
        assert(this->size() == src.size());
        const char_type * dest = this->data();
        size_type count = 0;
        for (const_iterator iter = src.cbegin(); iter != src.cend(); ++iter) {
            *dest = *iter;
            dest++;
            count++;
        }
        assert(count == this->size());
    }

    template <bool isInitialize>
    inline void assgin_and_copy_data(const char_type * src_data, size_type src_size) {
        if (isInitialize) {
            // It is necessary to ensure that the data is empty.
            assert(this->data() == nullptr);
            assert(this->size() == 0);
        }
        // If the source data is not null and is not empty, then copy the data.
        if (src_data != nullptr) {
            // Allow the new size is 0.
            size_type new_size = src_size;
            const char_type * new_data = allocate(new_size);
            this->data_ = new_data;
#if USE_MEMORY_STORAGE
            this->setCapacity(new_size);
#else
            this->size_ = new_size;
#endif

            if (src_size > 0) {
                std::memcpy((void *)new_data, (const void *)src_data, src_size * sizeof(char_type));
            }
        } else if (!isInitialize) {
            // Reset the status when it's not initializing.
            this->data_ = nullptr;
#if USE_MEMORY_STORAGE
            this->setCapacity(0);
#else
            this->size_ = 0;
#endif
        }
    }

    template <bool isInitialize, typename Container>
    inline void assgin_and_copy_data_from_container(const Container & src) {
        if (isInitialize) {
            // It is necessary to ensure that the data is empty.
            assert(this->data() == nullptr);
            assert(this->size() == 0);
        }

        // Allow the new size is 0.
        size_type new_size = src.size();
        const char_type * new_data = allocate(new_size);
        this->data_ = new_data;
#if USE_MEMORY_STORAGE
        this->setCapacity(new_size);
#else
        this->size_ = new_size;
#endif

        // If the source vector is not empty, then copy the data.
        copy_data_from_container(src);
    }

    inline void copy_impl(const char_type * dest_data, size_type dest_size,
                          const char_type * src_data, size_type src_size) {
        assert(dest_data != src_data);
        if (dest_size != src_size) {
            // If the source size is not equal to the data size, destroy and reallocate a new buffer,
            // then copy the data from source data.
            if (this->is_valid()) {
                deallocate();
            }
            assgin_and_copy_data<false>(src_data, src_size);
        } else {
            // If the source size is equal to the data size,
            // copy the data from source data when the destination size is greater than 0.
            if (this->is_valid()) {
                // Whether the destination size is empty, check it in copy_data() function.
                copy_data(dest_data, dest_size, src_data, src_size);
            }
        }
    }

    template <typename Container>
    inline void copy_from_container_impl(const char_type * dest_data, size_type dest_size,
                                         const Container & src) {
        if (dest_size != src.size()) {
            // If the source size is not equal to the data size, destroy and reallocate a new buffer,
            // then copy the data from source data.
            if (this->is_valid()) {
                deallocate();
            }
            assgin_and_copy_data_from_container<false>(src);
        } else {
            // If the source size is equal to the data size,
            // copy the data from source data when the destination size is greater than 0.
            if (this->is_valid()) {
                // Whether the destination size is empty, check it in copy_data() function.
                copy_data_from_container(src);
            }
        }
    }

    inline void swap_data(BasicMemoryBuffer & other) {
        assert(std::addressof(other) != this);
        using std::swap;
#if USE_MEMORY_STORAGE
        swap(this->storage(), other.storage());
#else
        swap(this->data_, other.data_);
        swap(this->size_, other.size_);
#endif
    }
};

/*
template <typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(BasicMemoryBuffer<CharT, Traits> & lhs, BasicMemoryBuffer<CharT, Traits> & rhs) {
    lhs.swap(rhs);
}
//*/

#if USE_MEMORY_STORAGE

using MemoryBuffer  = BasicMemoryBuffer< BasicMemoryStorage<char, std::char_traits<char>> >;
using WMemoryBuffer = BasicMemoryBuffer< BasicMemoryStorage<wchar_t, std::char_traits<wchar_t>> >;

using MutableMemoryBuffer  = BasicMemoryBuffer< BasicMutableMemoryStorage<char, std::char_traits<char>> >;
using WMutableMemoryBuffer = BasicMemoryBuffer< BasicMutableMemoryStorage<wchar_t, std::char_traits<wchar_t>> >;

#else

using MemoryBuffer  = BasicMemoryBuffer<char, std::char_traits<char>>;
using WMemoryBuffer = BasicMemoryBuffer<wchar_t, std::char_traits<wchar_t>>;

#endif // USE_MEMORY_STORAGE

} // namespace ziplab

namespace std {

#if USE_MEMORY_STORAGE

template <typename MemoryStorageT>
inline void swap(ziplab::BasicMemoryBuffer<MemoryStorageT> & lhs,
                 ziplab::BasicMemoryBuffer<MemoryStorageT> & rhs) {
    lhs.swap(rhs);
}

#else

template <typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicMemoryBuffer<CharT, Traits> & lhs,
                 ziplab::BasicMemoryBuffer<CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

#endif // USE_MEMORY_STORAGE

} // namespace std

#endif // ZIPLAB_STREAM_MEMORYBUFFER_HPP

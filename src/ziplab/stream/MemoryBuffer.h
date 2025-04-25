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
#include "ziplab/jstd/bits/Power2.hpp"

namespace ziplab {

// Forward declaration
template <typename CharT, typename Traits>
class BasicMemoryView;

template <typename CharT, bool IsMutable = true, typename Traits = std::char_traits<CharT>>
class BasicMemoryBuffer
{
public:
    using char_type     = CharT;
    using traits_type   = Traits;

    using size_type     = std::size_t;
    using diff_type     = std::ptrdiff_t;
    using index_type    = std::intptr_t;

    using this_type     = BasicMemoryBuffer<char_type, IsMutable, traits_type>;
    using memory_view_t = BasicMemoryView<char_type, traits_type>;

    using int_type      = typename traits_type::int_type;
    using pos_type      = typename traits_type::pos_type;
    using offset_type   = typename traits_type::off_type;

    using string_type = std::basic_string<char_type, traits_type>;
    using vector_type = std::vector<char_type>;

    static constexpr bool kIsMutable = IsMutable;
    static constexpr size_type kMinCapacity = 2;

private:
    // It's not allowed that when the data pointer is non-zero, the data size is zero.
    const char_type * data_;
    size_type         size_;
    size_type         capacity_;

public:
    BasicMemoryBuffer() : data_(nullptr), size_(0), capacity_(0) {}

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

    constexpr bool is_fixed() const { return !kIsMutable; }

    bool is_valid() const { return (data() != nullptr); }
    bool is_empty() const { return (size() == 0); }

    char_type * data() { return const_cast<char_type *>(data_); }
    const char_type * data() const { return data_; }

    size_type size() const { return size_; }
    size_type capacity() const { return capacity_; }

    index_type ssize() const { return static_cast<index_type>(size_); }
    index_type scapacity() const { return static_cast<index_type>(capacity_); }

    char_type * begin() { return data(); }
    const char_type * begin() const { return data(); }

    char_type * end() { return (data() + size()); }
    const char_type * end() const { return (data() + size()); }

    char_type * tail() { return (data() + capacity()); }
    const char_type * tail() const { return (data() + capacity()); }

    void destroy() {
        if (this->data_ != nullptr) {
            delete[] this->data_;
            this->data_ = nullptr;
            this->size_ = 0;
            if (!this->is_fixed()) {
                this->capacity_ = 0;
            }
        }
    }

    //
    // Only ensure to reserve space for at least N elements
    // and discards existing data, without initializing new elements.
    //
    void prepare(size_type new_capacity) {
        // If new capacity is less than or equal to old size, do nothing!
        if (new_capacity > this->size()) {
            // Allow new capacity equal to 0.
            reserve_impl<false, false>(new_capacity);
        }
    }

    //
    // Expand space for at least delta_size elements
    // and preserving existing data, without initializing new elements.
    //
    bool grow(size_type delta_size) {
        size_type new_size = this->size() + delta_size;
        if (ziplab_likely(new_size <= this->capacity())) {
            return false;
        } else {
            size_type new_capacity = this->capacity() * 2;
            reserve_impl<false, true>(new_capacity);
            return true;
        }
    }

    //
    // Ensure to reserve space for at least N elements
    // and preserving existing data, without initializing new elements.
    //
    void reserve(size_type new_capacity) {
        // If new capacity is less than or equal to old size, do nothing!
        if (new_capacity > this->size()) {
            // Allow new capacity equal to 0.
            reserve_impl<false, true>(new_capacity);
        }
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
        resize_impl<false>(new_size, init_val);
    }

    //
    // Expand the space to at least N elements
    // and preserving existing data, initialize new elements with default values.
    //
    void resize(size_type new_size, char_type init_val = 0) {
        resize_impl<true>(new_size, init_val);
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
    // Normalize capacity size, don't allowing a capacity of 0.
    static inline size_type round_capacity(size_type capacity) {
        // If not a power of two
        assert(capacity > 0);
        if ((capacity & (capacity - 1)) != 0) {
            capacity = (std::min)(capacity, kMinCapacity);
            capacity = jstd::Power2::round_up<kMinCapacity>(capacity);
        }
        return capacity;
    }

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

    //
    // Ensure to reserve space for at least N elements
    // and (preserving / discard) existing data, without initializing new elements.
    //
    template <bool IsInitialize, bool NeedPreserve>
    inline void reserve_impl(size_type new_capacity) {
        if (!this->is_fixed()) {
            // Normalize capacity size, allowing a capacity of 0.
            new_capacity = round_capacity(new_capacity);
        }

        const char_type * new_data = allocate(new_capacity);
        size_type copy_size = (std::min)(new_capacity, this->size());

        // If necessary, copy old data to new data.
        if (!IsInitialize && this->is_valid()) {
            if (NeedPreserve && (copy_size > 0)) {
                assert(new_data != nullptr);
                std::memcpy((void *)new_data, (const void *)this->data(), copy_size * sizeof(char_type));
            }
            deallocate();
        }
        this->data_ = new_data;
        this->capacity_ = new_capacity;
    }

    //
    // Expand the space to at least N elements
    // and (preserving / discard) existing data, initialize new elements with default values.
    //
    template <bool NeedPreserve>
    void resize_impl(size_type new_size, char_type init_val = 0) {
        if (new_size != this->size()) {
            // Allow new size equal to 0.
            const char_type * new_data = allocate(new_size);
            size_type copy_size;
            if (NeedPreserve)
                copy_size = (std::min)(new_size, this->size());
            else
                copy_size = 0;
            if (this->is_valid()) {
                // If necessary, copy old data to new data.
                if (NeedPreserve && (copy_size > 0)) {
                    assert(new_data != nullptr);
                    std::memcpy((void *)new_data, (const void *)this->data(), copy_size * sizeof(char_type));
                }
                deallocate();
            }
            // If necessary, fill remaining part with default value.
            size_type remain_size = new_size - copy_size;
            if (static_cast<diff_type>(remain_size) > 0) {
                assert(new_data != nullptr);
                assert(new_size > copy_size);
                std::memset((void *)(new_data + copy_size), (int)init_val, remain_size * sizeof(char_type));
            }
            this->data_ = new_data;
            this->capacity_ = new_size;
        } else {
            // If new size is equal to old size, do nothing!
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

    template <bool IsInitialize>
    inline void assgin_and_copy_data(const char_type * src_data, size_type src_size) {
        if (IsInitialize) {
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
            this->capacity_ = new_size;

            if (src_size > 0) {
                std::memcpy((void *)new_data, (const void *)src_data, src_size * sizeof(char_type));
            }
        } else if (!IsInitialize) {
            // Reset the status when it's not initializing.
            this->data_ = nullptr;
            this->capacity_ = 0;
        }
    }

    template <bool IsInitialize, typename Container>
    inline void assgin_and_copy_data_from_container(const Container & src) {
        if (IsInitialize) {
            // It is necessary to ensure that the data is empty.
            assert(this->data() == nullptr);
            assert(this->size() == 0);
        }

        // Allow the new size is 0.
        size_type new_size = src.size();
        const char_type * new_data = allocate(new_size);
        this->data_ = new_data;
        this->capacity_ = new_size;

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

    inline void swap_data(this_type & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->data_, other.data_);
        swap(this->size_, other.size_);
        swap(this->capacity_, other.capacity_);
    }
};

using MemoryBuffer  = BasicMemoryBuffer<char, true, std::char_traits<char>>;
using WMemoryBuffer = BasicMemoryBuffer<wchar_t, true, std::char_traits<wchar_t>>;

} // namespace ziplab

namespace std {

template <typename CharT, bool IsMutable, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicMemoryBuffer<CharT, IsMutable, Traits> & lhs,
                 ziplab::BasicMemoryBuffer<CharT, IsMutable, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_MEMORYBUFFER_HPP

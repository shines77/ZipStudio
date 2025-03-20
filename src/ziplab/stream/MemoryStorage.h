#ifndef ZIPLAB_STREAM_MEMORYSTORAGE_HPP
#define ZIPLAB_STREAM_MEMORYSTORAGE_HPP

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

#define USE_MEMORY_STORAGE  1

namespace ziplab {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class BasicMemoryStorage {
public:
    using char_type     = CharT;
    using traits_type   = Traits;

    using size_type     = std::size_t;
    using diff_type     = std::ptrdiff_t;
    using index_type    = std::streamsize;

protected:
    // Why mayebe change the order of member variables data and size,
    // because it is for higher efficiency in the input and output streams.
    const char_type * data_;
    size_type         capacity_;

public:
    BasicMemoryStorage() : data_(nullptr), capacity_(0) {
    }
    BasicMemoryStorage(const char_type * data, size_type size)
        : data_(data), capacity_(size) {
    }
    BasicMemoryStorage(const char_type * data, size_type size, size_type capacity)
        : data_(data), capacity_(capacity) {
    }
    BasicMemoryStorage(const BasicMemoryStorage & src) :
        data_(src.data()), size_(src.capacity()) {
    }
    ~BasicMemoryStorage() {
    }

    BasicMemoryStorage & operator = (const BasicMemoryStorage & rhs) {
        data_ = rhs.data();
        capacity_ = rhs.capacity();
        return *this;
    }

    bool is_valid() const { return (data() != nullptr); }
    bool is_empty() const { return (size() == 0); }

    char_type * data() { return const_cast<char_type *>(data_); }
    const char_type * data() const { return data_; }

    size_type size() const { return capacity_; }
    size_type capacity() const { return capacity_; }

    index_type ssize() const { return static_cast<index_type>(capacity_); }
    index_type scapacity() const { return static_cast<index_type>(capacity_); }

    char_type * begin() { return data(); }
    const char_type * begin() const { return data(); }

    char_type * end() { return (data() + size()); }
    const char_type * end() const { return (data() + size()); }

    char_type * tail() { return (data() + capacity()); }
    const char_type * tail() const { return (data() + capacity()); }

    void setData(const char_type * data) {
        data_ = data;
    }

    void setSize(size_type size) {
        capacity_ = size;
    }

    void setCapacity(size_type capacity) {
        capacity_ = capacity;
    }

    void setStorage(const char_type * data, size_type size) {
        data_ = data;
        capacity_ = size;
    }

    void setStorage(const char_type * data, size_type size, size_type capacity) {
        data_ = data;
        capacity_ = capacity;
    }

    void swap(BasicMemoryStorage & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicMemoryStorage & lhs, BasicMemoryStorage & rhs) {
        lhs.swap(rhs);
    }

private:
    inline void swap_data(BasicMemoryStorage & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->data_, other.data_);
        swap(this->capacity_, other.capacity_);
    }
};

template <typename CharT, typename Traits = std::char_traits<CharT>>
class BasicMutableMemoryStorage {
public:
    using char_type     = CharT;
    using traits_type   = Traits;

    using size_type     = std::size_t;
    using diff_type     = std::ptrdiff_t;
    using index_type    = std::streamsize;

protected:
    // Why mayebe change the order of member variables data and size,
    // because it is for higher efficiency in the input and output streams.
    const char_type * data_;
    size_type         size_;
    size_type         capacity_;

public:
    BasicMutableMemoryStorage() : data_(nullptr), size_(0) {
    }
    BasicMutableMemoryStorage(const char_type * data, size_type size)
        : data_(data), size_(size), capacity_(size) {
    }
    BasicMutableMemoryStorage(const char_type * data, size_type size, size_type capacity)
        : data_(data), size_(size), capacity_(capacity) {
    }
    BasicMutableMemoryStorage(const BasicMutableMemoryStorage & src) :
        data_(src.data()), size_(src.size()), capacity_(src.capacity()) {
    }
    ~BasicMutableMemoryStorage() {
    }

    BasicMutableMemoryStorage & operator = (const BasicMutableMemoryStorage & rhs) {
        data_ = rhs.data();
        size_ = rhs.size();
        capacity_ = rhs.capacity();
        return *this;
    }

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

    void setData(const char_type * data) {
        data_ = data;
    }

    void setSize(size_type size) {
        size_ = size;
    }

    void setCapacity(size_type capacity) {
        capacity_ = capacity;
    }

    void setStorage(const char_type * data, size_type size) {
        data_ = data;
        size_ = size;
        capacity_ = size;
    }

    void setStorage(const char_type * data, size_type size, size_type capacity) {
        data_ = data;
        size_ = size;
        capacity_ = capacity;
    }

    void swap(BasicMutableMemoryStorage & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicMutableMemoryStorage & lhs, BasicMutableMemoryStorage & rhs) {
        lhs.swap(rhs);
    }

private:
    inline void swap_data(BasicMutableMemoryStorage & other) {
        assert(std::addressof(other) != this);
        using std::swap;
        swap(this->data_, other.data_);
        swap(this->size_, other.size_);
        swap(this->capacity_, other.capacity_);
    }
};

using MemoryStorage  = BasicMemoryStorage<char, std::char_traits<char>>;
using WMemoryStorage = BasicMemoryStorage<wchar_t, std::char_traits<wchar_t>>;

using MutableMemoryStorage  = BasicMutableMemoryStorage<char, std::char_traits<char>>;
using WMutableMemoryStorage = BasicMutableMemoryStorage<wchar_t, std::char_traits<wchar_t>>;

} // namespace ziplab

namespace std {

template <typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicMemoryStorage<CharT, Traits> & lhs,
                 ziplab::BasicMemoryStorage<CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

template <typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicMutableMemoryStorage<CharT, Traits> & lhs,
                 ziplab::BasicMutableMemoryStorage<CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_MEMORYSTORAGE_HPP

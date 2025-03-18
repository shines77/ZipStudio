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

#include "ziplab/basic/stddef.h"
#include "ziplab/stream/MemoryBuffer.h"
#include "ziplab/stream/MemoryView.h"

namespace ziplab {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
class BasicInputStream
{
public:
    using buffer_type   = MemoryBufferT;
    using char_type     = CharT;
    using traits_type   = Traits;

    using memory_buffer_t = BasicMemoryBuffer<char_type, traits_type>;
    using memory_view_t   = BasicMemoryView<char_type, traits_type>;

    using size_type     = typename memory_buffer_t::size_type;
    using diff_type     = typename memory_buffer_t::diff_type;
    using index_type    = typename memory_buffer_t::index_type;
    using int_type      = typename traits_type::int_type;
    using pos_type      = typename traits_type::pos_type;
    using offset_type   = typename traits_type::off_type;

    using string_type = std::basic_string<char_type, traits_type>;
    using vector_type = std::vector<char_type>;

private:
    buffer_type buffer_;
    index_type pos_;
    size_type size_;

public:
    BasicInputStream() : buffer_(), pos_(0), size_(0) {
    }
    BasicInputStream(size_type capacity) : buffer_(capacity), pos_(0), size_(0) {
    }

    BasicInputStream(const memory_buffer_t & buffer)
        : buffer_(buffer), pos_(0), size_(buffer.size()) {
    }
    BasicInputStream(memory_buffer_t && buffer)
        : buffer_(std::forward<memory_buffer_t>(buffer)), pos_(0), size_(buffer.size()) {
    }

    BasicInputStream(const char_type * data, size_type size)
        : buffer_(data, size), pos_(0), size_(size) {
    }

    template <size_type N>
    BasicInputStream(const char_type (&data)[N])
        : buffer_(data, N), pos_(0), size_(N) {
    }

    template <size_type N>
    BasicInputStream(const std::array<string_type, N> & strings)
        : buffer_(strings), pos_(0), size_(N) {
    }

    BasicInputStream(const string_type & src)
        : buffer_(src), pos_(0), size_(src.size()) {
    }

    BasicInputStream(const vector_type & src)
        : buffer_(src), pos_(0), size_(src.size()) {
    }

    template <typename Container>
    BasicInputStream(const Container & src)
        : buffer_(src), pos_(0), size_(src.size()) {
    }

    BasicInputStream(const BasicInputStream & src)
        : buffer_(src.buffer()), pos_(src.pos()), size_(src.size()) {
    }
    BasicInputStream(BasicInputStream && src)
        : buffer_(std::forward<memory_buffer_t>(src.buffer())),
          pos_(src.pos()), size_(src.size()) {
    }

    ~BasicInputStream() {
        destroy();
    }

    bool is_valid() const { return buffer_.is_valid(); }
    bool is_empty() const { return buffer_.is_empty(); }

    char_type * data() { return buffer_.data(); }
    const char_type * data() const { return buffer_.data(); }

    index_type pos() const { return pos_; }
    size_type size() const { return size_; }
    size_type capacity() const { return buffer_.size(); }

    size_type upos() const { return static_cast<size_type>(pos_); }
    index_type ssize() const { return static_cast<index_type>(size_); }
    index_type scapacity() const { return buffer_.ssize(); }

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
        size_ = 0;
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

    void copy(const BasicInputStream & src) {
        buffer_.copy(src);
        pos_ = src.pos();
        size_ = src.size();
    }

    void swap(BasicInputStream & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicInputStream & lhs, BasicInputStream & rhs) {
        lhs.swap(rhs);
    }

    bool is_overflow() const {
        return (pos() < ssize());
    }

    template <typename T>
    bool is_overflow(const T & val) const {
        ZIPLAB_UNUSED(val);
        return ((pos() + sizeof(val)) <= ssize());
    }

    index_type back(offset_type offset) {
        pos_ -= static_cast<index_type>(offset);
        return pos_;
    }

    index_type skip(offset_type offset) {
        pos_ += static_cast<index_type>(offset);
        return pos_;
    }

    void seekToBegin() {
        pos_ = 0;
    }

    void seekToEnd() {
        pos_ = ssize();
    }

    void seekTo(index_type pos) {
        pos_ = pos;
    }

    // Safety skip value
    template <typename T>
    bool skipValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(pos() >= 0);
        if ((pos_ + step) <= ssize()) {
            ZIPLAB_UNUSED(val);
            pos_ += step;
            return true;
        } else {
            return false;
        }
    }

    bool skipBool(bool & b) {
        std::uint8_t byte = static_cast<std::uint8_t>(b);
        bool success = skipValue(byte);
        b = (byte != 0);
        return success;
    }

    bool skipChar(char & ch) {
        return skipValue(ch);
    }

    bool skipUChar(unsigned char & ch) {
        return skipValue(ch);
    }

    bool skipWChar(wchar_t & wch) {
        return skipValue(wch);
    }

    bool skipSByte(std::int8_t & sbyte) {
        return skipValue(sbyte);
    }

    bool skipByte(std::uint8_t & byte) {
        return skipValue(byte);
    }

    bool skipInt8(std::int8_t & val) {
        return skipValue(val);
    }

    bool skipUInt8(std::uint8_t & val) {
        return skipValue(val);
    }

    bool skipInt16(std::int16_t & val) {
        return skipValue(val);
    }

    bool skipUInt16(std::uint16_t & val) {
        return skipValue(val);
    }

    bool skipInt32(std::int32_t & val) {
        return skipValue(val);
    }

    bool skipUInt32(std::uint32_t & val) {
        return skipValue(val);
    }

    bool skipInt64(std::int64_t & val) {
        return skipValue(val);
    }

    bool skipUInt64(std::uint64_t & val) {
        return skipValue(val);
    }

    bool skipSizeT(std::size_t & val) {
        return skipValue(val);
    }

    bool skipFloat(float & val) {
        return skipValue(val);
    }

    bool skipDouble(double & val) {
        return skipValue(val);
    }

    bool skipVoidPtr(void * & pt) {
        return skipValue(pt);
    }

    template <typename T>
    bool skipPtr(T * & pt) {
        return skipValue(pt);
    }

    // Unsafe skip value
    template <typename T>
    void usSkipValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(pos() >= 0);
        assert((pos_ + step) <= ssize());
        ZIPLAB_UNUSED(val);
        pos_ += step;
    }

    void skipBool() {
        std::uint8_t byte;
        usSkipValue(byte);
        ZIPLAB_UNUSED(byte);
    }

    void skipChar() {
        char ch;
        usSkipValue(ch);
        ZIPLAB_UNUSED(ch);
    }

    void skipUChar() {
        unsigned char ch;
        usSkipValue(ch);
        ZIPLAB_UNUSED(ch);
    }

    void skipWChar() {
        wchar_t ch;
        usSkipValue(ch);
        ZIPLAB_UNUSED(ch);
    }

    void skipSByte() {
        std::int8_t sbyte;
        usSkipValue(sbyte);
        ZIPLAB_UNUSED(sbyte);
    }

    void skipByte() {
        std::uint8_t byte;
        usSkipValue(byte);
        ZIPLAB_UNUSED(byte);
    }

    void skipInt8() {
        std::int8_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipUInt8() {
        std::uint8_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipInt16() {
        std::int16_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipUInt16() {
        std::uint16_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipInt32() {
        std::int32_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipUInt32() {
        std::uint32_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipInt64() {
        std::int64_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipUInt64() {
        std::uint64_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipSizeT() {
        std::size_t val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipFloat() {
        float val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipDouble() {
        double val;
        usSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipVoidPtr() {
        void * pt;
        usSkipValue(pt);
        ZIPLAB_UNUSED(pt);
    }

    template <typename T>
    void skipPtr() {
        T * pt;
        usSkipValue(pt);
        ZIPLAB_UNUSED(pt);
    }

    // Safety peek value
    template <typename T>
    bool peekValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(pos() >= 0);
        if ((pos_ + step) <= ssize()) {
            val = *(reinterpret_cast<T *>(buffer_.data() + pos_));
            return true;
        } else {
            return false;
        }
    }

    bool peekBool(bool & b) {
        std::uint8_t byte = static_cast<std::uint8_t>(b);
        bool success = peekValue(byte);
        b = (byte != 0);
        return success;
    }

    bool peekChar(char & ch) {
        return peekValue(ch);
    }

    bool peekUChar(unsigned char & ch) {
        return peekValue(ch);
    }

    bool peekWChar(wchar_t & wch) {
        return peekValue(wch);
    }

    bool peekSByte(std::int8_t & sbyte) {
        return peekValue(sbyte);
    }

    bool peekByte(std::uint8_t & byte) {
        return peekValue(byte);
    }

    bool peekInt8(std::int8_t & val) {
        return peekValue(val);
    }

    bool peekUInt8(std::uint8_t & val) {
        return peekValue(val);
    }

    bool peekInt16(std::int16_t & val) {
        return peekValue(val);
    }

    bool peekUInt16(std::uint16_t & val) {
        return peekValue(val);
    }

    bool peekInt32(std::int32_t & val) {
        return peekValue(val);
    }

    bool peekUInt32(std::uint32_t & val) {
        return peekValue(val);
    }

    bool peekInt64(std::int64_t & val) {
        return peekValue(val);
    }

    bool peekUInt64(std::uint64_t & val) {
        return peekValue(val);
    }

    bool peekSizeT(std::size_t & val) {
        return peekValue(val);
    }

    bool peekFloat(float & val) {
        return peekValue(val);
    }

    bool peekDouble(double & val) {
        return peekValue(val);
    }

    bool peekVoidPtr(void * & pt) {
        return peekValue(pt);
    }

    template <typename T>
    bool peekPtr(T * & pt) {
        return peekValue(pt);
    }

    // Unsafe peek value
    template <typename T>
    void usPeekValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(pos() >= 0);
        assert((pos_ + step) <= ssize());
        val = *(reinterpret_cast<T *>(buffer_.data() + pos_));
    }

    bool peekBool() {
        std::uint8_t byte;
        usPeekValue(byte);
        return (byte != 0);
    }

    char peekChar() {
        char ch;
        usPeekValue(ch);
        return ch;
    }

    unsigned char peekUChar() {
        unsigned char ch;
        usPeekValue(ch);
        return ch;
    }

    wchar_t peekWChar() {
        wchar_t ch;
        usPeekValue(ch);
        return ch;
    }

    std::int8_t peekSByte() {
        std::int8_t sbyte;
        usPeekValue(sbyte);
        return sbyte;
    }

    std::uint8_t peekByte() {
        std::uint8_t byte;
        usPeekValue(byte);
        return byte;
    }

    std::int8_t peekInt8() {
        std::int8_t val;
        usPeekValue(val);
        return val;
    }

    std::uint8_t peekUInt8() {
        std::uint8_t val;
        usPeekValue(val);
        return val;
    }

    std::int16_t peekInt16() {
        std::int16_t val;
        usPeekValue(val);
        return val;
    }

    std::uint16_t peekUInt16() {
        std::uint16_t val;
        usPeekValue(val);
        return val;
    }

    std::int32_t peekInt32() {
        std::int32_t val;
        usPeekValue(val);
        return val;
    }

    std::uint32_t peekUInt32() {
        std::uint32_t val;
        usPeekValue(val);
        return val;
    }

    std::int64_t peekInt64() {
        std::int64_t val;
        usPeekValue(val);
        return val;
    }

    std::uint64_t peekUInt64() {
        std::uint64_t val;
        usPeekValue(val);
        return val;
    }

    std::size_t peekSizeT() {
        std::size_t val;
        usPeekValue(val);
        return val;
    }

    float peekFloat() {
        float val;
        usPeekValue(val);
        return val;
    }

    double peekDouble() {
        double val;
        usPeekValue(val);
        return val;
    }

    void * peekVoidPtr() {
        void * pt;
        usPeekValue(pt);
        return pt;
    }

    template <typename T>
    T * peekPtr() {
        T * pt;
        usPeekValue(pt);
        return pt;
    }

    // Safety read value
    template <typename T>
    bool readValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(pos() >= 0);
        if ((pos_ + step) <= ssize()) {
            val = *(reinterpret_cast<T *>(buffer_.data() + pos_));
            pos_ += step;
            return true;
        } else {
            return false;
        }
    }

    bool readBool(bool & b) {
        std::uint8_t byte = static_cast<std::uint8_t>(b);
        bool success = readValue(byte);
        b = (byte != 0);
        return success;
    }

    bool readChar(char & ch) {
        return readValue(ch);
    }

    bool readUChar(unsigned char & ch) {
        return readValue(ch);
    }

    bool readWChar(wchar_t & wch) {
        return readValue(wch);
    }

    bool readSByte(std::int8_t & sbyte) {
        return readValue(sbyte);
    }

    bool readByte(std::uint8_t & byte) {
        return readValue(byte);
    }

    bool readInt8(std::int8_t & val) {
        return readValue(val);
    }

    bool readUInt8(std::uint8_t & val) {
        return readValue(val);
    }

    bool readInt16(std::int16_t & val) {
        return readValue(val);
    }

    bool readUInt16(std::uint16_t & val) {
        return readValue(val);
    }

    bool readInt32(std::int32_t & val) {
        return readValue(val);
    }

    bool readUInt32(std::uint32_t & val) {
        return readValue(val);
    }

    bool readInt64(std::int64_t & val) {
        return readValue(val);
    }

    bool readUInt64(std::uint64_t & val) {
        return readValue(val);
    }

    bool readSizeT(std::size_t & val) {
        return readValue(val);
    }

    bool readFloat(float & val) {
        return readValue(val);
    }

    bool readDouble(double & val) {
        return readValue(val);
    }

    bool readVoidPtr(void * & pt) {
        return readValue(pt);
    }

    template <typename T>
    bool readPtr(T * & pt) {
        return readValue(pt);
    }

    // Unsafe read value
    template <typename T>
    void usReadValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(pos() >= 0);
        assert((pos_ + step) <= ssize());
        val = *(reinterpret_cast<T *>(buffer_.data() + pos_));
        pos_ += step;
    }

    bool readBool() {
        std::uint8_t byte;
        usReadValue(byte);
        return (byte != 0);
    }

    char readChar() {
        char ch;
        usReadValue(ch);
        return ch;
    }

    unsigned char readUChar() {
        unsigned char ch;
        usReadValue(ch);
        return ch;
    }

    wchar_t readWChar() {
        wchar_t ch;
        usReadValue(ch);
        return ch;
    }

    std::int8_t readSByte() {
        std::int8_t sbyte;
        usReadValue(sbyte);
        return sbyte;
    }

    std::uint8_t readByte() {
        std::uint8_t byte;
        usReadValue(byte);
        return byte;
    }

    std::int8_t readInt8() {
        std::int8_t val;
        usReadValue(val);
        return val;
    }

    std::uint8_t readUInt8() {
        std::uint8_t val;
        usReadValue(val);
        return val;
    }

    std::int16_t readInt16() {
        std::int16_t val;
        usReadValue(val);
        return val;
    }

    std::uint16_t readUInt16() {
        std::uint16_t val;
        usReadValue(val);
        return val;
    }

    std::int32_t readInt32() {
        std::int32_t val;
        usReadValue(val);
        return val;
    }

    std::uint32_t readUInt32() {
        std::uint32_t val;
        usReadValue(val);
        return val;
    }

    std::int64_t readInt64() {
        std::int64_t val;
        usReadValue(val);
        return val;
    }

    std::uint64_t readUInt64() {
        std::uint64_t val;
        usReadValue(val);
        return val;
    }

    std::size_t readSizeT() {
        std::size_t val;
        usReadValue(val);
        return val;
    }

    float readFloat() {
        float val;
        usReadValue(val);
        return val;
    }

    double readDouble() {
        double val;
        usReadValue(val);
        return val;
    }

    void * readVoidPtr() {
        void * pt;
        usReadValue(pt);
        return pt;
    }

    template <typename T>
    T * readPtr() {
        T * pt;
        usReadValue(pt);
        return pt;
    }

protected:
    //

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
        swap(this->pos_, other.pos_);
        swap(this->size_, other.size_);
    }
};

using InputStreamBuffer  = BasicInputStream<BasicMemoryBuffer<char, std::char_traits<char>>, char, std::char_traits<char>>;
using WInputStreamBuffer = BasicInputStream<BasicMemoryBuffer<wchar_t, std::char_traits<wchar_t>>, wchar_t, std::char_traits<wchar_t>>;

using InputStreamView  = BasicInputStream<BasicMemoryView<char, std::char_traits<char>>, char, std::char_traits<char>>;
using WInputStreamView = BasicInputStream<BasicMemoryView<wchar_t, std::char_traits<wchar_t>>, wchar_t, std::char_traits<wchar_t>>;

} // namespace ziplab

namespace std {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicInputStream<MemoryBufferT, CharT, Traits> & lhs,
                 ziplab::BasicInputStream<MemoryBufferT, CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_INPUTSTREAM_HPP

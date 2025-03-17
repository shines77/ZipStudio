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
    size_type pos_;

public:
    BasicInputStream() : buffer_(), pos_(0) {
    }
    BasicInputStream(size_type capacity) : buffer_(capacity), pos_(0) {
    }

    BasicInputStream(const memory_buffer_t & src) : buffer_(src), pos_(0) {
    }
    BasicInputStream(memory_buffer_t && src)
        : buffer_(std::forward<memory_buffer_t>(src)), pos_(0) {
    }

    BasicInputStream(const char_type * data, size_type size) : buffer_(data, size), pos_(0) {
    }

    template <size_type N>
    BasicInputStream(const char_type (&data)[N]) : buffer_(data, N), pos_(0) {
    }

    template <size_type N>
    BasicInputStream(const std::array<string_type, N> & strings) : buffer_(strings), pos_(0) {
    }

    BasicInputStream(const string_type & src) : buffer_(src), pos_(0) {
    }

    BasicInputStream(const vector_type & src) : buffer_(src), pos_(0) {
    }

    template <typename Container>
    BasicInputStream(const Container & src) : buffer_(src), pos_(0) {
    }

    BasicInputStream(const BasicInputStream & src) : buffer_(src.buffer()), pos_(0) {
    }
    BasicInputStream(BasicInputStream && src)
        : buffer_(std::forward<memory_buffer_t>(src.buffer())), pos_(0) {
    }

    ~BasicInputStream() {
        destroy();
    }

    bool is_valid() const { return buffer_.is_valid(); }
    bool is_empty() const { return buffer_.is_empty(); }

    char_type * data() { return buffer_.data(); }
    const char_type * data() const { return buffer_.data(); }

    size_type size() const { return buffer_.size(); }
    size_type pos() const { return pos_; }

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

    void reserve_and_keep(size_type new_capacity) {
        buffer_.reserve_and_keep(new_capacity);
    }

    void resize_and_keep(size_type new_size, bool fill_new = true, char_type init_val = 0) {
        buffer_.resize_and_keep(new_size, fill_new, init_val);
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

    bool is_overflow() const {
        return (pos() < buffer_.size());
    }

    template <typename T>
    bool is_overflow(const T & type) const {
        ZIPLAB_UNUSED(type);
        return ((pos() + sizeof(type)) <= buffer_.size());
    }

    // Safety peek value
    template <typename T>
    bool peekValue(T & val) {
        static constexpr size_type step = sizeof(T);
        if ((pos_ + step) <= buffer_.size()) {
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
        static constexpr size_type step = sizeof(T);
        assert((pos_ + step) <= buffer_.size());
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
        static constexpr size_type step = sizeof(T);
        if ((pos_ + step) <= buffer_.size()) {
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
        static constexpr size_type step = sizeof(T);
        assert((pos_ + step) <= buffer_.size());
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
    template <typename T>
    bool readPtrType(T *& pt) {
        static constexpr size_type step = sizeof(T *);
        if ((pos_ + step) <= buffer_.size()) {
            pt = *(static_cast<T **>(buffer_.data() + pos_));
            pos_ += step;
            return true;
        } else {
            return false;
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

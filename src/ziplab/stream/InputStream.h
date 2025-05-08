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
#include "ziplab/stream/IOStreamRoot.h"

namespace ziplab {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
class BasicInputStream : public BasicIOStreamRoot<MemoryBufferT, CharT, Traits>
{
public:
    using buffer_type   = MemoryBufferT;
    using char_type     = CharT;
    using traits_type   = Traits;

    using super_type    = BasicIOStreamRoot<buffer_type, char_type, traits_type>;
    using this_type     = BasicInputStream<buffer_type, char_type, traits_type>;

    using memory_buffer_t = BasicMemoryBuffer<char_type, MemoryBufferT::kIsMutable, traits_type>;
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
    index_type  pos_;

public:
    BasicInputStream(buffer_type & buffer) : super_type(buffer), pos_(0) {
    }

    BasicInputStream(const BasicInputStream & src)
        : super_type(src.buffer()), pos_(src.pos()) {
    }
    BasicInputStream(BasicInputStream && src)
        : super_type(std::forward<memory_buffer_t>(src.buffer())),
          pos_(src.pos()) {
    }

    ~BasicInputStream() {
        //destroy();
    }

    // Get the base class pointrt
    super_type * super(this_type * derived) {
        return static_cast<super_type *>(derived);
    }

    const super_type * super(const this_type * derived) const {
        return static_cast<const super_type *>(const_cast<this_type *>(derived));
    }

    // Get the base class reference
    super_type & super_ref(this_type * derived) {
        return *static_cast<super_type *>(derived);
    }

    const super_type & super_ref(const this_type * derived) const {
        return *static_cast<const super_type *>(const_cast<this_type *>(derived));
    }

    void copy(const BasicInputStream & src) {
        if (std::addressof(src) != this) {
            copy_data(src);
        }
    }

    void swap(BasicInputStream & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicInputStream & lhs, BasicInputStream & rhs) {
        lhs.swap(rhs);
    }

    // Bound checking
    bool is_underflow() const {
        return (pos() < 0);
    }

    template <typename T>
    bool is_underflow(const T & val) const {
        ZIPLAB_UNUSED(val);
        return ((pos() - sizeof(val)) < 0);
    }

    using super_type::is_overflow;

    using super_type::unsafeWrite;
    using super_type::write;

    // Unsafe write
    void unsafeWrite(const BasicInputStream & out) {
        this->unsafeWrite(out.data(), out.size());
    }

    // Safety write
    bool write(const BasicInputStream & out) {
        return this->write(out.data(), out.size());
    }

    // Unsafe skip value
    template <typename T>
    void unsafeSkipValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(this->pos() >= 0);
        assert((pos_ + step) <= this->ssize());
        ZIPLAB_UNUSED(val);
        pos_ += step;
    }

    void skipBool() {
        std::uint8_t byte;
        unsafeSkipValue(byte);
        ZIPLAB_UNUSED(byte);
    }

    void skipChar() {
        char ch;
        unsafeSkipValue(ch);
        ZIPLAB_UNUSED(ch);
    }

    void skipUChar() {
        unsigned char ch;
        unsafeSkipValue(ch);
        ZIPLAB_UNUSED(ch);
    }

    void skipWChar() {
        wchar_t ch;
        unsafeSkipValue(ch);
        ZIPLAB_UNUSED(ch);
    }

    void skipSByte() {
        std::int8_t sbyte;
        unsafeSkipValue(sbyte);
        ZIPLAB_UNUSED(sbyte);
    }

    void skipByte() {
        std::uint8_t byte;
        unsafeSkipValue(byte);
        ZIPLAB_UNUSED(byte);
    }

    void skipInt8() {
        std::int8_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipUInt8() {
        std::uint8_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipInt16() {
        std::int16_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipUInt16() {
        std::uint16_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipInt32() {
        std::int32_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipUInt32() {
        std::uint32_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipInt64() {
        std::int64_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipUInt64() {
        std::uint64_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipSizeT() {
        std::size_t val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipFloat() {
        float val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipDouble() {
        double val;
        unsafeSkipValue(val);
        ZIPLAB_UNUSED(val);
    }

    void skipVoidPtr() {
        void * pt;
        unsafeSkipValue(pt);
        ZIPLAB_UNUSED(pt);
    }

    template <typename T>
    void skipPtr() {
        T * pt;
        unsafeSkipValue(pt);
        ZIPLAB_UNUSED(pt);
    }

    // Safety skip value
    template <typename T>
    bool skipValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(this->pos() >= 0);
        if ((pos_ + step) <= this->ssize()) {
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

    // Safety peek value
    template <typename T>
    bool peekValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(this->pos() >= 0);
        if ((pos_ + step) <= this->ssize()) {
            val = *(reinterpret_cast<T *>(this->buffer_.data() + pos_));
            return true;
        } else {
            val = T();
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
    void unsafePeekValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(this->pos() >= 0);
        assert((pos_ + step) <= this->ssize());
        val = *(reinterpret_cast<T *>(this->buffer_.data() + pos_));
    }

    bool peekBool() {
        std::uint8_t byte;
        unsafePeekValue(byte);
        return (byte != 0);
    }

    char peekChar() {
        char ch;
        unsafePeekValue(ch);
        return ch;
    }

    unsigned char peekUChar() {
        unsigned char ch;
        unsafePeekValue(ch);
        return ch;
    }

    wchar_t peekWChar() {
        wchar_t ch;
        unsafePeekValue(ch);
        return ch;
    }

    std::int8_t peekSByte() {
        std::int8_t sbyte;
        unsafePeekValue(sbyte);
        return sbyte;
    }

    std::uint8_t peekByte() {
        std::uint8_t byte;
        unsafePeekValue(byte);
        return byte;
    }

    std::int8_t peekInt8() {
        std::int8_t val;
        unsafePeekValue(val);
        return val;
    }

    std::uint8_t peekUInt8() {
        std::uint8_t val;
        unsafePeekValue(val);
        return val;
    }

    std::int16_t peekInt16() {
        std::int16_t val;
        unsafePeekValue(val);
        return val;
    }

    std::uint16_t peekUInt16() {
        std::uint16_t val;
        unsafePeekValue(val);
        return val;
    }

    std::int32_t peekInt32() {
        std::int32_t val;
        unsafePeekValue(val);
        return val;
    }

    std::uint32_t peekUInt32() {
        std::uint32_t val;
        unsafePeekValue(val);
        return val;
    }

    std::int64_t peekInt64() {
        std::int64_t val;
        unsafePeekValue(val);
        return val;
    }

    std::uint64_t peekUInt64() {
        std::uint64_t val;
        unsafePeekValue(val);
        return val;
    }

    std::size_t peekSizeT() {
        std::size_t val;
        unsafePeekValue(val);
        return val;
    }

    float peekFloat() {
        float val;
        unsafePeekValue(val);
        return val;
    }

    double peekDouble() {
        double val;
        unsafePeekValue(val);
        return val;
    }

    void * peekVoidPtr() {
        void * pt;
        unsafePeekValue(pt);
        return pt;
    }

    template <typename T>
    T * peekPtr() {
        T * pt;
        unsafePeekValue(pt);
        return pt;
    }

    // Safety read value
    template <typename T>
    bool readValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(this->pos() >= 0);
        if ((pos_ + step) <= this->ssize()) {
            val = *(reinterpret_cast<T *>(this->buffer_.data() + pos_));
            pos_ += step;
            return true;
        } else {
            val = T();
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
    void unsafeReadValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(this->pos() >= 0);
        assert((pos_ + step) <= this->ssize());
        val = *(reinterpret_cast<T *>(this->buffer_.data() + pos_));
        pos_ += step;
    }

    bool readBool() {
        std::uint8_t byte;
        unsafeReadValue(byte);
        return (byte != 0);
    }

    char readChar() {
        char ch;
        unsafeReadValue(ch);
        return ch;
    }

    unsigned char readUChar() {
        unsigned char ch;
        unsafeReadValue(ch);
        return ch;
    }

    wchar_t readWChar() {
        wchar_t ch;
        unsafeReadValue(ch);
        return ch;
    }

    std::int8_t readSByte() {
        std::int8_t sbyte;
        unsafeReadValue(sbyte);
        return sbyte;
    }

    std::uint8_t readByte() {
        std::uint8_t byte;
        unsafeReadValue(byte);
        return byte;
    }

    std::int8_t readInt8() {
        std::int8_t val;
        unsafeReadValue(val);
        return val;
    }

    std::uint8_t readUInt8() {
        std::uint8_t val;
        unsafeReadValue(val);
        return val;
    }

    std::int16_t readInt16() {
        std::int16_t val;
        unsafeReadValue(val);
        return val;
    }

    std::uint16_t readUInt16() {
        std::uint16_t val;
        unsafeReadValue(val);
        return val;
    }

    std::int32_t readInt32() {
        std::int32_t val;
        unsafeReadValue(val);
        return val;
    }

    std::uint32_t readUInt32() {
        std::uint32_t val;
        unsafeReadValue(val);
        return val;
    }

    std::int64_t readInt64() {
        std::int64_t val;
        unsafeReadValue(val);
        return val;
    }

    std::uint64_t readUInt64() {
        std::uint64_t val;
        unsafeReadValue(val);
        return val;
    }

    std::size_t readSizeT() {
        std::size_t val;
        unsafeReadValue(val);
        return val;
    }

    float readFloat() {
        float val;
        unsafeReadValue(val);
        return val;
    }

    double readDouble() {
        double val;
        unsafeReadValue(val);
        return val;
    }

    void * readVoidPtr() {
        void * pt;
        unsafeReadValue(pt);
        return pt;
    }

    template <typename T>
    T * readPtr() {
        T * pt;
        unsafeReadValue(pt);
        return pt;
    }

protected:
    inline void clear_data() {
        //
    }

    inline void copy_data(const BasicInputStream & src) {
        super_ref(this).copy_data(super_ref(&src));
    }

    inline void swap_data(BasicInputStream & other) {
        assert(std::addressof(other) != this);
        //super_ref(this).swap_data(super_ref(&src));
        using std::swap;
        swap(super_ref(this), super_ref(&other));
    }

private:
    //
};

using InputStream  = BasicInputStream< BasicMemoryBuffer<char, true, std::char_traits<char> >, char, std::char_traits<char>>;
using WInputStream = BasicInputStream< BasicMemoryBuffer<wchar_t, true, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

using InputStreamView  = BasicInputStream< BasicMemoryView<char, std::char_traits<char> >, char, std::char_traits<char>>;
using WInputStreamView = BasicInputStream< BasicMemoryView<wchar_t, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

} // namespace ziplab

namespace std {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicInputStream<MemoryBufferT, CharT, Traits> & lhs,
                 ziplab::BasicInputStream<MemoryBufferT, CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_INPUTSTREAM_HPP

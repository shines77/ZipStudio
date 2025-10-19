#ifndef ZIPLAB_STREAM_SEQUENTIAL_OUTPUTSTREAM_HPP
#define ZIPLAB_STREAM_SEQUENTIAL_OUTPUTSTREAM_HPP

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
#include "ziplab/stream/IOStreamRoot.h"

namespace ziplab {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
class BasicSequentialOutputStream : public BasicIOStreamRoot<MemoryBufferT, CharT, Traits>
{
public:
    using buffer_type   = MemoryBufferT;
    using char_type     = CharT;
    using traits_type   = Traits;

    using super_type    = BasicIOStreamRoot<buffer_type, char_type, traits_type>;
    using this_type     = BasicSequentialOutputStream<buffer_type, char_type, traits_type>;

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

private:
    //

public:
    BasicSequentialOutputStream(buffer_type & buffer) : super_type(buffer) {
    }

    BasicSequentialOutputStream(const BasicSequentialOutputStream & src)
        : super_type(src.super()) {
    }
    BasicSequentialOutputStream(BasicSequentialOutputStream && src)
        : super_type(std::forward<super_type>(src.super())) {
    }

    ~BasicSequentialOutputStream() {
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

    void copy(const BasicSequentialOutputStream & src) {
        if (std::addressof(src) != this) {
            copy_data(src);
        }
    }

    void swap(BasicSequentialOutputStream & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicSequentialOutputStream & lhs, BasicSequentialOutputStream & rhs) {
        lhs.swap(rhs);
    }

    using super_type::is_overflow;

    // Cursor
    this_type & backward(size_type step) {
        this->buffer_.backward(step);
        return *this;
    }

    this_type & forward(size_type step) {
        this->buffer_.forward(step);
        return *this;
    }

    this_type & rewind(size_type count) {
        this->buffer_.rewind(count);
        return *this;
    }

    this_type & skip(size_type count) {
        this->buffer_.skip(count);
        return *this;
    }

    // Seek to
    void seek_to_begin() {
        this->buffer_.seek_to_begin();
    }

    void seek_to_end() {
        this->buffer_.seek_to_end();
    }

    void seek_to_tail() {
        this->buffer_.seek_to_tail();
    }

    void seek_to(index_type pos) {
        this->buffer_.seek_to(pos);
    }

    using super_type::unsafeWrite;
    using super_type::write;

    // Unsafe write
    void unsafeWrite(const BasicSequentialOutputStream & out) {
        this->unsafeWrite(out.data(), out.size());
    }

    // Safety write
    bool write(const BasicSequentialOutputStream & out) {
        return this->write(out.data(), out.size());
    }

    // Unsafe write value
    template <typename T>
    void unsafeWriteValue(const T & val) {
        this->buffer_.unsafeWriteValue(val);
    }

    template <typename T>
    void unsafeWriteValue(T && val) {
        this->buffer_.unsafeWriteValue(std::forward<T>(val));
    }

    void unsafeWriteBool(std::uint8_t byte) {
        unsafeWriteValue(byte);
    }

    void unsafeWriteChar(char ch) {
        unsafeWriteValue(ch);
    }

    void unsafeWriteUChar(unsigned char ch) {
        unsafeWriteValue(ch);
    }

    void unsafeWriteWChar(wchar_t ch) {
        unsafeWriteValue(ch);
    }

    void unsafeWriteSByte(std::int8_t sbyte) {
        unsafeWriteValue(sbyte);

    }

    void unsafeWriteByte(std::uint8_t byte) {
        unsafeWriteValue(byte);
    }

    void unsafeWriteInt8(std::int8_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteUInt8(std::uint8_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteInt16(std::int16_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteUInt16(std::uint16_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteInt32(std::int32_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteUInt32(std::uint32_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteInt64(std::int64_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteUInt64(std::uint64_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteSizeT(std::size_t val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteFloat(float val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteDouble(double val) {
        unsafeWriteValue(val);
    }

    void unsafeWriteVoidPtr(void * pt) {
        unsafeWriteValue(pt);
    }

    template <typename T>
    void unsafeWritePtr(T * pt) {
        unsafeWriteValue(pt);
    }

    // Safety write value
    template <typename T>
    void writeValue(const T & val) {
        this->buffer_.writeValue(val);
    }

    template <typename T>
    void writeValue(T && val) {
        this->buffer_.writeValue(std::forward<T>(val));
    }

    bool writeBool(bool b) {
        std::uint8_t byte = static_cast<std::uint8_t>(b);
        bool success = writeValue(byte);
        b = (byte != 0);
        return success;
    }

    bool writeChar(char ch) {
        return writeValue(ch);
    }

    bool writeUChar(unsigned char ch) {
        return writeValue(ch);
    }

    bool writeWChar(wchar_t wch) {
        return writeValue(wch);
    }

    bool writeSByte(std::int8_t sbyte) {
        return writeValue(sbyte);
    }

    bool writeByte(std::uint8_t byte) {
        return writeValue(byte);
    }

    bool writeInt8(std::int8_t val) {
        return writeValue(val);
    }

    bool writeUInt8(std::uint8_t val) {
        return writeValue(val);
    }

    bool writeInt16(std::int16_t val) {
        return writeValue(val);
    }

    bool writeUInt16(std::uint16_t val) {
        return writeValue(val);
    }

    bool writeInt32(std::int32_t val) {
        return writeValue(val);
    }

    bool writeUInt32(std::uint32_t val) {
        return writeValue(val);
    }

    bool writeInt64(std::int64_t val) {
        return writeValue(val);
    }

    bool writeUInt64(std::uint64_t val) {
        return writeValue(val);
    }

    bool writeSizeT(std::size_t val) {
        return writeValue(val);
    }

    bool writeFloat(float val) {
        return writeValue(val);
    }

    bool writeDouble(double val) {
        return writeValue(val);
    }

    bool writeVoidPtr(void * pt) {
        return writeValue(pt);
    }

    template <typename T>
    bool writePtr(T * pt) {
        return writeValue(pt);
    }

protected:
    inline void clear_data() {
        //
    }

    inline void copy_data(const BasicSequentialOutputStream & src) {
        super_ref(this).copy_data(super_ref(&src));
    }

    inline void swap_data(BasicSequentialOutputStream & other) {
        assert(std::addressof(other) != this);
        //super_ref(this).swap_data(super_ref(&src));
        using std::swap;
        swap(super_ref(this), super_ref(&other));
    }

private:
    //
};

using SequentialOutputStream  = BasicSequentialOutputStream< BasicMemoryBuffer<char, true, std::char_traits<char> >, char, std::char_traits<char>>;
using WSequentialOutputStream = BasicSequentialOutputStream< BasicMemoryBuffer<wchar_t, true, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

using SequentialOutputStreamView  = BasicSequentialOutputStream< BasicMemoryView<char, std::char_traits<char> >, char, std::char_traits<char>>;
using WSequentialOutputStreamView = BasicSequentialOutputStream< BasicMemoryView<wchar_t, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

} // namespace ziplab

namespace std {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicSequentialOutputStream<MemoryBufferT, CharT, Traits> & lhs,
                 ziplab::BasicSequentialOutputStream<MemoryBufferT, CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_SEQUENTIAL_OUTPUTSTREAM_HPP

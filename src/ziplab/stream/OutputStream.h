#ifndef ZIPLAB_STREAM_OUTPUTSTREAM_HPP
#define ZIPLAB_STREAM_OUTPUTSTREAM_HPP

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
class BasicOutputStream : public BasicIOStreamRoot<MemoryBufferT, CharT, Traits>
{
public:
    using buffer_type   = MemoryBufferT;
    using char_type     = CharT;
    using traits_type   = Traits;

    using super_type    = BasicIOStreamRoot<buffer_type, char_type, traits_type>;
    using this_type     = BasicOutputStream<buffer_type, char_type, traits_type>;

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

private:
    index_type pos_;

public:
    BasicOutputStream() : super_type(), pos_(0) {
    }
    BasicOutputStream(size_type capacity) : super_type(capacity), pos_(0) {
    }

    BasicOutputStream(const memory_buffer_t & buffer)
        : super_type(buffer), pos_(0) {
    }
    BasicOutputStream(memory_buffer_t && buffer)
        : super_type(std::forward<memory_buffer_t>(buffer)), pos_(0) {
    }

    BasicOutputStream(const char_type * data, size_type size)
        : super_type(data, size), pos_(0) {
    }

    template <size_type N>
    BasicOutputStream(const char_type (&data)[N])
        : super_type(data, N), pos_(0) {
    }

    template <size_type N>
    BasicOutputStream(const std::array<string_type, N> & strings)
        : super_type(strings), pos_(0) {
    }

    BasicOutputStream(const string_type & src)
        : super_type(src), pos_(0) {
    }

    BasicOutputStream(const vector_type & src)
        : super_type(src), pos_(0) {
    }

    template <typename Container>
    BasicOutputStream(const Container & src)
        : super_type(src), pos_(0) {
    }

    BasicOutputStream(const BasicOutputStream & src)
        : super_type(src.buffer()), pos_(src.pos()) {
    }
    BasicOutputStream(BasicOutputStream && src)
        : super_type(std::forward<memory_buffer_t>(src.buffer())),
          pos_(src.pos()) {
    }

    ~BasicOutputStream() {
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

    void copy(const BasicOutputStream & src) {
        if (std::addressof(src) != this) {
            copy_data(src);
        }
    }

    void swap(BasicOutputStream & other) {
        if (std::addressof(other) != this) {
            swap_data(other);
        }
    }

    friend inline void swap(BasicOutputStream & lhs, BasicOutputStream & rhs) {
        lhs.swap(rhs);
    }

    bool write(const char_type * data, size_type size) {
        index_type s_size = static_cast<index_type>(size);
        if ((pos_ + s_size) <= this->ssize()) {
            char_type * current = this->buffer_.data() + pos_;
#if 0
            std::memcpy((void *)current, (const void *)data, size * sizeof(char_type));
#else
            traits_type::copy(current, data, size);
#endif
            pos_ += s_size;
            return true;
        } else {
            return false;
        }
    }

    template <typename Allocator>
    bool write(const std::basic_string<char_type, traits_type, Allocator> & str) {
        return write(str.c_str(), str.size());
    }

    bool write(const memory_buffer_t & buffer) {
        return write(buffer.data(), buffer.size());
    }

    bool write(const memory_view_t & buffer) {
        return write(buffer.data(), buffer.size());
    }

    bool write(const BasicOutputStream & out) {
        return write(out.data(), out.size());
    }

    // Safety write value
    template <typename T>
    bool writeValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(this->pos() >= 0);
        if ((pos_ + step) <= this->ssize()) {
            char_type * current = this->buffer_.data() + pos_;
            *(reinterpret_cast<T *>(current)) = val;
            pos_ += step;
            return true;
        } else {
            return false;
        }
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

    // Unsafe write value
    template <typename T>
    void unsafeWriteValue(T & val) {
        static constexpr index_type step = sizeof(T);
        assert(this->pos() >= 0);
        assert((pos_ + step) <= this->ssize());
        char_type * current = this->buffer_.data() + pos_;
        *(reinterpret_cast<T *>(current)) = val;
        pos_ += step;
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

protected:
    inline void clear_data() {
        //
    }

    inline void copy_data(const BasicOutputStream & src) {
        super_ref(this).copy_data(super_ref(&src));
    }

    inline void swap_data(BasicOutputStream & other) {
        assert(std::addressof(other) != this);
        //super_ref(this).swap_data(super_ref(&src));
        using std::swap;
        swap(super_ref(this), super_ref(&other));
    }

private:
    //
};

#if USE_MEMORY_STORAGE

using OutputStream  = BasicOutputStream< BasicMemoryBuffer< BasicMemoryStorage<char, std::char_traits<char>> >, char, std::char_traits<char> >;
using WOutputStream = BasicOutputStream< BasicMemoryBuffer< BasicMemoryStorage<wchar_t, std::char_traits<wchar_t>> >, wchar_t, std::char_traits<wchar_t> >;

using OutputStreamView  = BasicOutputStream< BasicMemoryView< BasicMemoryStorage<char, std::char_traits<char>> >, char, std::char_traits<char>>;
using WOutputStreamView = BasicOutputStream< BasicMemoryView< BasicMemoryStorage<wchar_t, std::char_traits<wchar_t>> >, wchar_t, std::char_traits<wchar_t> >;

#else

using OutputStream  = BasicOutputStream< BasicMemoryBuffer<char, std::char_traits<char> >, char, std::char_traits<char>>;
using WOutputStream = BasicOutputStream< BasicMemoryBuffer<wchar_t, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

using OutputStreamView  = BasicOutputStream< BasicMemoryView<char, std::char_traits<char> >, char, std::char_traits<char>>;
using WOutputStreamView = BasicOutputStream< BasicMemoryView<wchar_t, std::char_traits<wchar_t> >, wchar_t, std::char_traits<wchar_t>>;

#endif

} // namespace ziplab

namespace std {

template <typename MemoryBufferT, typename CharT, typename Traits = std::char_traits<CharT>>
inline void swap(ziplab::BasicOutputStream<MemoryBufferT, CharT, Traits> & lhs,
                 ziplab::BasicOutputStream<MemoryBufferT, CharT, Traits> & rhs) {
    lhs.swap(rhs);
}

} // namespace std

#endif // ZIPLAB_STREAM_OUTPUTSTREAM_HPP

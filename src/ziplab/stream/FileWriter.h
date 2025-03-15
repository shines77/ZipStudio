#ifndef ZIPLAB_STREAM_FILEWRITER_HPP
#define ZIPLAB_STREAM_FILEWRITER_HPP

#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>

#include <assert.h>

namespace ziplab {

class FileWriter
{
public:
    using char_type = char;
    using string_type = std::string;
    using traits_type = typename string_type::traits_type;
    using size_type = std::size_t;
    using int_type = typename traits_type::int_type;
    using pos_type = typename traits_type::pos_type;
    using offset_type = typename traits_type::off_type;

    using open_mode_t = std::ios_base::openmode;

    static const open_mode_t kDefaultWriteMode = std::ios::out | std::ios::binary | std::ios::trunc;

    static const size_type KB = 1024;
    static const size_type MB = 1024 * KB;
    static const size_type kWriteBuffSize = 128 * KB;

    static const size_type kInvalidSize = static_cast<size_type>(-1);

private:
    std::ofstream ofs_;
    std::string content_;
    size_type buffer_size_;
    std::string filename_;
    size_type filesize_;

public:
    FileWriter() : ofs_(), buffer_size_(kWriteBuffSize), filesize_(0) {
    }

    FileWriter(const char * filename, open_mode_t mode = kDefaultWriteMode) :
        ofs_(filename, mode),
        buffer_size_(kWriteBuffSize),
        filename_(filename),
        filesize_(0) {
    }

    FileWriter(const std::string & filename, open_mode_t mode = kDefaultWriteMode) :
        ofs_(filename, mode),
        buffer_size_(kWriteBuffSize),
        filename_(filename),
        filesize_(0) {
    }

    FileWriter(const FileWriter & rhs) = delete;

    FileWriter(FileWriter && other) :
        ofs_(std::move(other.ofs_)),
        content_(std::move(other.content_)),
        buffer_size_(other.buffer_size_),
        filename_(std::move(other.filename_)),
        filesize_(other.filesize_) {
    }

    ~FileWriter() {
        close();
    }

    bool is_opened() const { return ofs_.is_open(); }
    bool is_good() const { return ofs_.good(); }
    bool is_bad() const { return ofs_.bad(); }
    
    std::string & content() { return content_; }
    const std::string & content() const { return content_; }
    size_type contentSize() const { return content_.size(); }

    std::string & filename() { return filename_; }
    const std::string & filename() const { return filename_; }
    size_type fileSize() const { return filesize_; }

    size_type getBuffSize() const { return buffer_size_; }

    void setBuffSize(size_type buffer_size) {
        buffer_size_ = buffer_size;
    }

    bool open(const char * filename, open_mode_t mode = kDefaultWriteMode) {
        if (!is_opened()) {
            ofs_.open(filename, mode);
        }
        return is_opened();
    }

    bool open(const std::string & filename, open_mode_t mode = kDefaultWriteMode) {
        return open(filename.c_str(), mode);
    }

    void flush() {
        if (is_opened()) {
            ofs_.flush();
        }
    }

    void close() {
        if (is_opened()) {
            ofs_.close();
        }
    }

    size_type writeFile(std::string & content, size_type writeBuffSize = 0) {
        size_type totolWriteBytes = 0;

        if (ofs_.good()) {
            ofs_.seekp(0, std::ios::end);

            writeBuffSize = (writeBuffSize != 0) ? writeBuffSize : kWriteBuffSize;
            const char * writeBuf = content.c_str();
            size_type totalSize = content.size();
            size_type remainBytes = totalSize;

            while (remainBytes > 0 && ofs_.good()) {
                size_type writeBytes = (remainBytes > writeBuffSize) ? writeBuffSize : remainBytes;
                ofs_.write(writeBuf, writeBytes);
                writeBuf += writeBytes;
                totolWriteBytes += writeBytes;
                remainBytes -= writeBytes;
            }

            assert(totolWriteBytes == totalSize);
        }

        return totolWriteBytes;
    }

    size_type writeFile() {
        return writeFile(content_, buffer_size_);
    }

private:
    //
};

} // namespace ziplab

#endif // ZIPLAB_STREAM_FILEWRITER_HPP

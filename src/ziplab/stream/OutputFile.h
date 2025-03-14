#ifndef ZIPLAB_STREAM_OUTPUTFILE_HPP
#define ZIPLAB_STREAM_OUTPUTFILE_HPP

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

class OutputFile
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
    static const size_type kReadBuffSize = 64 * KB;

    static const size_type kInvalidSize = static_cast<size_type>(-1);

private:
    std::ofstream ofs_;
    std::string content_;
    size_type buffer_size_;
    std::string filename_;
    size_type filesize_;

public:
    OutputFile() : ofs_(), buffer_size_(kReadBuffSize), filesize_(0) {
    }

    OutputFile(const char * filename, open_mode_t mode = kDefaultWriteMode) :
        ofs_(filename, mode),
        buffer_size_(kReadBuffSize),
        filename_(filename),
        filesize_(0) {
    }

    OutputFile(const std::string & filename, open_mode_t mode = kDefaultWriteMode) :
        ofs_(filename, mode),
        buffer_size_(kReadBuffSize),
        filename_(filename),
        filesize_(0) {
    }

    OutputFile(const OutputFile & rhs) = delete;

    OutputFile(OutputFile && other) :
        ofs_(std::move(other.ofs_)),
        content_(std::move(other.content_)),
        buffer_size_(other.buffer_size_),
        filename_(std::move(other.filename_)),
        filesize_(other.filesize_) {
    }

    ~OutputFile() {
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

    size_type writeFile(std::string & content, size_type readBuffSize = 0) {
        size_type totolReadBytes = 0;

        if (ofs_.good()) {
            ofs_.seekg(0, std::ios::end);
            size_type totalFileSize = (size_type)ofs_.tellg();
            filesize_ = totalFileSize;

            content.clear();
            content.reserve(totalFileSize);
            
            ofs_.seekg(0, std::ios::beg);

            readBuffSize = (readBuffSize != 0) ? readBuffSize : kReadBuffSize;
            std::unique_ptr<char> readBuf(new char[readBuffSize]);

            while (!ofs_.eof()) {
                ofs_.read(readBuf.get(), readBuffSize);
                std::streamsize readBytes = ofs_.gcount();
                if (readBytes > 0) {
                    content.append(readBuf.get(), readBytes);
                    totolReadBytes += readBytes;
                } else {
                    break;
                }
            }

            assert(totolReadBytes == totalFileSize);
        }

        return totolReadBytes;
    }

    size_type writeFile() {
        return writeFile(content_, buffer_size_);
    }

private:
    //
};

} // namespace ziplab

#endif // ZIPLAB_STREAM_OUTPUTFILE_HPP

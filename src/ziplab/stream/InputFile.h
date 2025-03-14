#ifndef ZIPLAB_STREAM_INPUTFILE_HPP
#define ZIPLAB_STREAM_INPUTFILE_HPP

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

class InputFile
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

    static const open_mode_t kDefaultReadMode = std::ios::in | std::ios::binary;

    static const size_type KB = 1024;
    static const size_type MB = 1024 * KB;
    static const size_type kReadBuffSize = 64 * KB;

    static const size_type kInvalidSize = static_cast<size_type>(-1);

private:
    std::ifstream ifs_;
    std::string content_;
    size_type buffer_size_;
    std::string filename_;
    size_type filesize_;

public:
    InputFile() : ifs_(), buffer_size_(kReadBuffSize), filesize_(0) {
    }

    InputFile(const char * filename, open_mode_t mode = kDefaultReadMode) :
        ifs_(filename, mode),
        buffer_size_(kReadBuffSize),
        filename_(filename),
        filesize_(0) {
        readFile();
    }

    InputFile(const std::string & filename, open_mode_t mode = kDefaultReadMode) :
        ifs_(filename, mode),
        buffer_size_(kReadBuffSize),
        filename_(filename),
        filesize_(0) {
        readFile();
    }

    InputFile(const InputFile & rhs) = delete;

    InputFile(InputFile && other) :
        ifs_(std::move(other.ifs_)),
        content_(std::move(other.content_)),
        buffer_size_(other.buffer_size_),
        filename_(std::move(other.filename_)),
        filesize_(other.filesize_) {
    }

    ~InputFile() {
        close();
    }

    bool is_opened() const { return ifs_.is_open(); }
    bool is_good() const { return ifs_.good(); }
    bool is_bad() const { return ifs_.bad(); }
    
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

    bool open(const char * filename, open_mode_t mode = kDefaultReadMode) {
        if (!is_opened()) {
            ifs_.open(filename, mode);
        }
        return is_opened();
    }

    bool open(const std::string & filename, open_mode_t mode = kDefaultReadMode) {
        return open(filename.c_str(), mode);
    }

    void close() {
        if (is_opened()) {
            ifs_.close();
        }
    }

    size_type readFile(std::string & content, size_type readBuffSize = 0) {
        size_type totolReadBytes = 0;

        if (ifs_.good()) {
            ifs_.seekg(0, std::ios::end);
            size_type totalFileSize = (size_type)ifs_.tellg();
            filesize_ = totalFileSize;

            content.clear();
            content.reserve(totalFileSize);
            
            ifs_.seekg(0, std::ios::beg);

            readBuffSize = (readBuffSize != 0) ? readBuffSize : kReadBuffSize;
            std::unique_ptr<char> readBuf(new char[readBuffSize]);

            while (!ifs_.eof()) {
                ifs_.read(readBuf.get(), readBuffSize);
                std::streamsize readBytes = ifs_.gcount();
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

    size_type readFile() {
        return readFile(content_, buffer_size_);
    }

private:
    //
};

} // namespace ziplab

#endif // ZIPLAB_STREAM_INPUTFILE_HPP

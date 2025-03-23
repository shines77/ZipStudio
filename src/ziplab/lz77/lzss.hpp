#ifndef ZIPLAB_LZ77_LZSS_HPP
#define ZIPLAB_LZ77_LZSS_HPP

#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>

#include "ziplab/lz77/lz77.hpp"

namespace ziplab {

template <std::size_t WindowBits, std::size_t LookAheadBits>
class LZSSCompressor {
public:
    using size_type = std::size_t;

    static constexpr size_type kWindowSize = static_cast<size_type>(1 << WindowBits);
    static constexpr size_type kLookAheadSize = static_cast<size_type>(1 << LookAheadBits);

    static_assert((kWindowSize >= 16), "The kWindowSize must be equal to or more than 16.");
    static_assert((WindowBits > LookAheadBits), "The WindowBits must be more than LookAheadBits.");
    static_assert((LookAheadBits > 1), "The LookAheadBits must be more than 1.");

    static constexpr size_type kWindowMask = kWindowSize - 1;
    static constexpr size_type kLengthMask = kLookAheadSize - 1;

    static constexpr size_type kMinLookAhead = 3;
    static constexpr size_type kMaxLookAhead = kMinLookAhead + kLookAheadSize - 1;

    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    //

public:
    LZSSCompressor() {
        //
    }

    ~LZSSCompressor() {
        //
    }

    // Compress data
    std::vector<LZByte> compress(const std::vector<LZByte> & data) {
        static constexpr size_type kWordLen = sizeof(size_type);
        static constexpr size_type kMaxFlag = static_cast<size_type>(1) << (kWordLen - 1);

        size_type data_len = data.size();
        std::vector<size_type> flag_array;

        size_type flag_len = (data_len + kWordLen - 1) /  kWordLen;
        flag_array.reserve(flag_len);

        std::vector<LZByte> compressed;

        size_type flag_bits = 0;
        size_type flag_bit = 1;
        size_type pos = 0;
        while (pos < data_len) {
            // Calculate the boundary of the sliding window.
            size_type search_start = (pos > kWindowSize) ? (pos - kWindowSize) : 0;
            size_t max_lookahead = (std::min)(pos + kLookAheadSize, data_len);

            size_type match_pos = npos;
            size_type match_len = 0;

            if (match_pos != npos) {
                flag_bits |= flag_bit;
            }
            if (flag_bit != kMaxFlag) {
                flag_bit <<= 1;
            } else {
                flag_array.push_back(flag_bits);
                flag_bit = 1;
            }
            
        }

        if (flag_bit != 1) {
            flag_array.push_back(flag_bits);
        }

        return compressed;
    }

    // Decompress data
    std::vector<LZByte> decompress(const std::vector<LZByte> & compressed_data) {
        std::vector<LZByte> compressed;

        return compressed;
    }

    // Compress file
    void compressFile(const std::string & inputFile, const std::string & outputFile) {
        //
    }

    // Decompress file
    void decompressFile(const std::string & inputFile, const std::string & outputFile) {
        //
    }
};

} // namespace ziplab

#endif // ZIPLAB_LZ77_LZSS_HPP

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
#include <stdexcept>

#include "ziplab/lz77/lz77.hpp"
#include "ziplab/lz77/lzDictHashmap.hpp"

namespace ziplab {

template <std::size_t WindowBits, std::size_t LookAheadBits>
class LZSSCompressor {
public:
    using size_type = std::size_t;
    using offset_type = std::uint16_t;

    static constexpr size_type kWindowSize = static_cast<size_type>(1 << WindowBits);
    static constexpr size_type kLookAheadSize = static_cast<size_type>(1 << LookAheadBits);

    // WindowBits = [4, 16], WindowSize = [16, 65536]
    static_assert((kWindowSize >= 16), "The kWindowSize must be greater than or equal to or 16.");
    static_assert((kWindowSize <= 65536), "The kWindowSize must be less than or equal to 65536.");

    // WindowBits > LookAheadBits
    static_assert((WindowBits > LookAheadBits), "The WindowBits must be greater than LookAheadBits.");

    // LookAheadBits >= 2, LookAheadSize >= 4
    static_assert((LookAheadBits > 1), "The LookAheadBits must be greater than 1.");

    static constexpr size_type kWindowMask = kWindowSize - 1;
    static constexpr size_type kLengthMask = kLookAheadSize - 1;

    static constexpr size_type kMinLookAhead = 3;
    static constexpr size_type kMaxLookAhead = kMinLookAhead + kLookAheadSize - 1;

    static constexpr size_type kFirstHashKeyLen = 3;
    static constexpr size_type kSecondHashKeyLen = 6;

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
    std::string compress(const std::string & input) {
        static constexpr size_type kWordLen = sizeof(size_type);
        static constexpr size_type kMaxFlag = static_cast<size_type>(1) << (kWordLen - 1);

        size_type data_len = input.size();
        std::vector<size_type> flag_array;

        LZDictHashmap<offset_type> firstHashmap(kWindowSize * 2);
        LZDictHashmap<offset_type> secondHashmap(kWindowSize * 2);

        size_type flag_len = (data_len + kWordLen - 1) /  kWordLen;
        flag_array.reserve(flag_len);

        std::string compressed;

        size_type flag_bits = 0;
        size_type flag_bit = 1;
        size_type pos = 0;
        while (pos < data_len) {
            // Calculate the boundary of the sliding window.
            size_type window_start = (pos > kWindowSize) ? (pos - kWindowSize) : 0;
            size_type window_size = (std::min)(kWindowSize, pos);
            size_type lookahead_size = (std::min)(kLookAheadSize, data_len - pos);
            size_type lookahead_last = (std::min)(pos + kLookAheadSize, data_len);

            const char * window = input.data() + window_start;
            const char * lookahead = input.data() + pos;

            size_type match_pos = npos;
            size_type match_len = find_match(window, lookahead, window_size, lookahead_size);

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
    std::string decompress(const std::string & compressed_data) {
        std::string decompressed;

        return decompressed;
    }

    // Compress file
    void compressFile(const std::string & inputFile, const std::string & outputFile) {
        //
    }

    // Decompress file
    void decompressFile(const std::string & inputFile, const std::string & outputFile) {
        //
    }

private:
    size_type find_match(const char * window, const char * lookahead,
                         size_type window_size, size_type lookahead_size) {
        return 0;
    }
};

} // namespace ziplab

#endif // ZIPLAB_LZ77_LZSS_HPP

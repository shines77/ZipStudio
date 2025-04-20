#ifndef ZIPLAB_LZ77_LZSS_HPP
#define ZIPLAB_LZ77_LZSS_HPP

#pragma once

#include <cstdint>
#include <cstddef>
#include <bitset>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "ziplab/jstd/bitset.hpp"
#include "ziplab/lz77/lz77.hpp"
#include "ziplab/lz77/lzDictHashmap.hpp"

#include "ziplab/stream/MemoryBuffer.h"
#include "ziplab/stream/InputStream.h"
#include "ziplab/stream/OutputStream.h"

namespace ziplab {

template <std::size_t WindowBits, std::size_t LookAheadBits>
class LZSSCompressor {
public:
    using size_type = std::size_t;
    using ssize_type = std::intptr_t;
    using offset_type = std::uint16_t;

    static constexpr size_type kWindowBits = WindowBits;
    static constexpr size_type kLookAheadBits = LookAheadBits;

    static constexpr size_type kWindowSize = static_cast<size_type>(1 << kWindowBits);
    static constexpr size_type kLookAheadSize = static_cast<size_type>(1 << kLookAheadBits);

    // WindowBits = [4, 16], WindowSize = [16, 65536]
    static_assert((kWindowSize >= 16), "The kWindowSize must be greater than or equal to or 16.");
    static_assert((kWindowSize <= 65536), "The kWindowSize must be less than or equal to 65536.");

    // WindowBits > LookAheadBits
    static_assert((kWindowBits > kLookAheadBits), "The WindowBits must be greater than LookAheadBits.");

    // LookAheadBits >= 2, LookAheadSize >= 4
    static_assert((kLookAheadBits > 1), "The LookAheadBits must be greater than 1.");

    static constexpr size_type kWindowMask = kWindowSize - 1;
    static constexpr size_type kLengthMask = kLookAheadSize - 1;

    static constexpr size_type kMinMatchLength = 3;
    static constexpr size_type kMaxMatchLength = kLookAheadSize - 1;
    static constexpr size_type kMaxLookAheadSize = kMinMatchLength + kMaxMatchLength;

    static constexpr size_type kL1HashKeyLen = 3;
    static constexpr size_type kL2HashKeyLen = 6;

    //
    // The total data size of block, to adaptive the size of L1 or L2 cache.
    // We make the total data size of block less than or equal to 16 KB or 32 KB.
    //
    // The data used includes:
    //
    // the window chars = [kWindowSize], maybe is 4096 bytes,
    // the hashmap size = [kWindowSize * 2 * sizeof(uint16_t)], maybe is 16384 bytes,
    // the data of block, associated with flag bits, maybe is <= 16384 bytes,
    // the flags of block, = [kBlockDataSize / 8], maybe is 2048 bytes.
    //
    // Total size <= 4096 + 16384 + 16384 + 2048 <= 38 KB, (when kBlockDataSize = 16 KB)
    // Total size <= 4096 + 16384 + 32768 + 4096 <= 56 KB, (when kBlockDataSize = 32 KB)
    //
    static constexpr size_type kBlockDataSize = 16 * 1024;

    static_assert(((kBlockDataSize & (kBlockDataSize - 1)) == 0),
                  "The kBlockDataSize must be is a power of 2.");

    // A flag is followed by 1 or 2 bytes of data.
    static constexpr size_type kBlockFlagSize = kBlockDataSize / 2;

    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    struct MatchResult {
        size_type match_len;
        size_type match_pos;

        MatchResult() : match_len(0), match_pos(npos) {}

        MatchResult(size_type match_len, size_type match_pos)
            : match_len(match_len), match_pos(match_pos) {}

        MatchResult(const MatchResult & src)
            : match_len(src.match_len), match_pos(src.match_pos) {
        }
    };

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    //
    // Fixed warning: ISO C++ prohibits anonymous structs
    //
#endif

    union PackedPair {
        struct Parts {
            std::uint8_t low;
            std::uint8_t high;
        } parts;
        std::uint16_t value;

        PackedPair() : value(0) {}
        explicit PackedPair(std::uint16_t value) : value(value) {}

        PackedPair(std::uint8_t low, std::uint8_t high)
            : parts{low, high} {}

        PackedPair(const MatchResult & result)
            : value(make_pair(result)) {
        }

        PackedPair(const PackedPair & src) : value(src.value) {}

        PackedPair & operator = (std::uint16_t rhs) {
            value = rhs;
            return *this;
        }

        static std::uint16_t make_pair(const MatchResult & result) {
            size_type match_len = result.match_len - kMinMatchLength;
            assert(match_len < kMaxMatchLength);
            assert(result.match_pos < kWindowSize);
            return make_pair(match_len, result.match_pos);
        }

        static std::uint16_t make_pair(size_type match_len, size_type match_pos) {
            return static_cast<std::uint16_t>((match_len << kWindowBits) | match_pos);
        }
    };

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

public:
    LZSSCompressor() {
        //
    }

    ~LZSSCompressor() {
        //
    }

    // Compress data
    int plain_compress(const std::string & input_data, MemoryBuffer & compressed_data) {
        static constexpr size_type kWordBytes = sizeof(size_type);
        static constexpr size_type kWordBits = kWordBytes * CHAR_BIT;
        static constexpr size_type kMaxFlag = static_cast<size_type>(1) << (kWordBits - 1);
        static constexpr size_type kBlockFlagBytes = (kBlockFlagSize + kWordBytes - 1) /  kWordBytes;

        int err_code = 0;
        OutputStream compressed_os(compressed_data);

        size_type data_size = input_data.size();
        if (ziplab_likely(data_size != 0)) {
            //LZDictHashmap<offset_type, WindowBits> L1_hashmap;

            jstd::bitset<kBlockFlagSize> flag_bits;
            MemoryBuffer block_data;
            OutputStream block_os(block_data);

            block_data.prepare(kBlockDataSize);

            size_type pos = 0;
            assert(data_size > 0);
            do {
                size_type remaining_size = data_size - pos;
                size_type block_capacity = (remaining_size > kBlockDataSize) ? kBlockDataSize : remaining_size;
                size_type block_pos = pos;
                size_type block_end = pos + block_capacity;
                while (block_pos < block_end) {
                    // Calculate the boundary of the sliding window.
                    ssize_type window_first = block_pos - kWindowSize;  // It's can be negative
                    size_type window_start = (window_first >= 0) ? window_first : 0;
                    size_type window_size = (std::min)(kWindowSize, block_pos);
                    size_type lookahead_size = (std::min)(kMaxLookAheadSize, data_size - block_pos);
                    size_type lookahead_last = (std::min)(pos + kMaxLookAheadSize, data_size);

                    const char * window = input_data.data() + window_start;
                    const char * lookahead = input_data.data() + block_pos;

                    MatchResult match_result = plain_find_match(window, lookahead, window_size, lookahead_size);
                    if (ziplab_likely(match_result.match_len < kMinMatchLength)) {
                        // Literal
                        block_os.unsafeWriteByte(input_data[block_pos++]);
                        block_os.unsafeWriteByte(input_data[block_pos++]);
                    } else {
                        // A pair of (MatchPos, MatchLength) - [distance, length]
                        PackedPair packedPair(match_result);
                        block_os.unsafeWriteByte(packedPair.parts.low);
                        block_os.unsafeWriteByte(packedPair.parts.high);

                        assert(match_result.match_pos != npos);
                        flag_bits.set(block_pos);

                        block_pos += match_result.match_len;
                    }
                }

                // Output flag bits and block data
                size_type flag_capacity = 0;
                if (compressed_os.grow(flag_capacity + block_capacity)) {
                    output_flag_bits(compressed_os, flag_bits, block_capacity);
                    compressed_os.write(block_data);
                }

                flag_bits.reset();
                block_data.clear();

                pos = block_end;
            } while (pos < data_size);
        }

        return err_code;
    }

    // Decompress data
    int plain_decompress(const MemoryBuffer & compressed_data, MemoryBuffer & decompressed_data) {
        int err_code = 0;

        OutputStream decompressed(decompressed_data);
        ZIPLAB_UNUSED(compressed_data);

        return err_code;
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
    inline size_type output_flag_bits(OutputStream & compressedOs,
                                      const jstd::bitset<kBlockFlagSize> & flag_bits,
                                      size_type flag_capacity) {
        assert(flag_capacity <= kBlockFlagSize);
        size_type num_bytes = (flag_capacity + (CHAR_BIT - 1)) / CHAR_BIT;

        // Allocate the size of data to be added in advance
        compressedOs.reserve(compressedOs.size() + num_bytes);

        //assert(num_bytes == flag_bits.bytes());
        compressedOs.write(flag_bits.data(), num_bytes);

        return num_bytes;
    }

    MatchResult plain_find_match(const char * window, const char * lookahead,
                                 size_type window_size, size_type lookahead_size) {
        assert(window != nullptr);
        assert(lookahead != nullptr);

        if (window_size < (kMinMatchLength - 1)) {
            return { 0, 0 };
        }

        assert(window_size > 0);
        assert(lookahead_size > 0);

        size_type best_match_len = kMinMatchLength - 1;
        size_type best_offset = npos;

        for (size_type pos = 0; pos < window_size; pos++) {
            const char * window_start = window + pos;

            size_type match_len = 0;
            do  {
                if (window_start[match_len] == lookahead[match_len])
                    match_len++;
                else
                    break;
            } while (match_len < lookahead_size && match_len < window_size);

            if (match_len > best_match_len) {
                best_match_len = match_len;
                best_offset = pos;

                // If the matching length has reached the maximum lookahead size, return directly.
                if (match_len >= lookahead_size)
                    break;
            }
        }

        return { best_match_len, best_offset };
    }

    MatchResult find_match(const char * window, const char * lookahead,
                           size_type window_size, size_type lookahead_size) {
        size_type best_match_len = kMinMatchLength - 1;
        size_type best_offset = npos;

        return { best_match_len, best_offset };
    }
};

} // namespace ziplab

#endif // ZIPLAB_LZ77_LZSS_HPP

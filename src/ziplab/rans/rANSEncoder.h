
#ifndef ZIPLAB_RANS_RANSENCODER_HPP
#define ZIPLAB_RANS_RANSENCODER_HPP

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

#include "ziplab/rans/rANS.h"

#include "ziplab/stream/MemoryBuffer.h"
#include "ziplab/stream/InputStream.h"
#include "ziplab/stream/OutputStream.h"

#include <windows.h>

namespace ziplab {

template <typename CharT>
class rANSEncoder64 {
public:
    using char_type = CharT;
    using size_type = std::size_t;
    using ssize_type = std::intptr_t;
    using offset_type = std::uint16_t;

    static const size_type kTotalFreq = 65536;
    //static const size_type kInitState = 0x7FFFFFFFu;
    static const size_type kInitState = 0x80000000u;
    //static const size_type kMaxState  = 0xFFFFFFFFFFFFFFFFull;
    static const size_type kMaxState  = 0x8000000000000000ull;

    using Symbol = std::uint8_t;

public:
    rANSEncoder64() {
        //
    }

    virtual ~rANSEncoder64() {
        //
    }

private:
    void count_freq(const std::string & input_data, std::uint32_t * freqs,
                    std::uint32_t * cumuls, std::size_t freq_size) {
        for (std::size_t i = 0; i < freq_size; i++) {
            freqs[i] = 0;
        }

        // Count symbol frequency
        // 13, 10, 2, 1
        for (const std::uint8_t ch : input_data) {
            freqs[ch]++;
        }

        // Calc symbol cumulative frequency
        // 13, 23, 25, 26
        cumuls[0] = 0;
        for (std::size_t i = 0; i < (freq_size - 1); i++) {
            cumuls[i + 1] = cumuls[i] + freqs[i];
        }
    }

    void init_symbol_stats(SymbolStats & stats, std::uint32_t * freqs,
                           std::uint32_t * cumuls, std::size_t freq_size) {
        // Get min symbol: 'A'
        for (std::size_t i = 0; i < freq_size; i++) {
            if (freqs[i] > 0) {
                stats.min_symbol = static_cast<std::uint8_t>(i);
                break;
            }
        }

        // Get max symbol: 'D'
        for (std::size_t i = freq_size - 1; i >= 0; i--) {
            if (freqs[i] > 0) {
                stats.max_symbol = static_cast<std::uint8_t>(i);
                break;
            }
        }

        // Total frequency
        // 26
        stats.total_freq = cumuls[kMaxSymbol];
        if (stats.total_freq == 0) return;

        // Rescale the frequency
        // 32768, 25206, 5041, 2520
        for (std::size_t i = 0; i < freq_size; i++) {
            stats.symbols[i].freq = freqs[i] * kTotalFreq / stats.total_freq;
        }
        // For stats.symbols[kSymbolTotal]
        stats.symbols[freq_size].freq = 0;

        // Calc the scaled cumulative frequency
        // 13, 23, 25, 26
        stats.symbols[0].cumul = 0;
        for (std::size_t i = 0; i < (freq_size - 1); i++) {
            stats.symbols[i + 1].cumul = stats.symbols[i].cumul + stats.symbols[i].freq;
        }
        // For stats.symbols[kSymbolTotal]
        stats.symbols[freq_size].cumul = stats.symbols[freq_size - 1].cumul + stats.symbols[freq_size - 1].freq;

        size_type scale_total_freq = stats.symbols[kMaxSymbol].cumul;

        // Get freq errors
        double freq_errs[kSymbolTotal];
        for (std::size_t i = 0; i < freq_size; i++) {
            freq_errs[i] = (double)freqs[i] * kTotalFreq / stats.total_freq - stats.symbols[i].freq;
        }

        // Adjust the scaled frequency
        for (std::size_t n = scale_total_freq; n < kTotalFreq; n++) {
            double max_errs = 0.0;
            std::size_t adjust_symbol = static_cast<std::size_t>(-1);
            for (std::size_t i = 0; i < freq_size; i++) {
                if (freq_errs[i] > max_errs) {
                    max_errs = freq_errs[i];
                    adjust_symbol = i;
                }
            }
            if (adjust_symbol != static_cast<std::size_t>(-1)) {
                stats.symbols[adjust_symbol].freq++;
                freq_errs[adjust_symbol] = (double)freqs[adjust_symbol] * kTotalFreq / stats.total_freq
                                            - stats.symbols[adjust_symbol].freq;
            } else {
                break;
            }
        }

        // Recalc symbol cumulative frequency
        // 32768, 25206, 5041, 2521
        stats.symbols[0].cumul = 0;
        for (std::size_t i = 0; i < (freq_size - 1); i++) {
            stats.symbols[i + 1].cumul = stats.symbols[i].cumul + stats.symbols[i].freq;
        }
        // For stats.symbols[kSymbolTotal]
        stats.symbols[freq_size].cumul = stats.symbols[freq_size - 1].cumul + stats.symbols[freq_size - 1].freq;

        scale_total_freq = stats.symbols[kMaxSymbol].cumul;
        assert(scale_total_freq == kTotalFreq);
    }

    void write_symbol_stats(const SymbolStats & stats, OutputStream & output_os) {
        // Write symbol range: [min_symbol, max_symbol]
        output_os.writeUInt8(stats.min_symbol);
        output_os.writeUInt8(stats.max_symbol);

        // Write symbol sacled frequencies
        for (Symbol symbol = stats.min_symbol; symbol <= stats.max_symbol; symbol++) {
            std::uint16_t freq = static_cast<std::uint16_t>(stats.symbols[symbol].freq);
            output_os.writeUInt16(freq);
        }
    }

public:
    /*
        // Process four states at once using the AVX2 instruction set (pseudocode)
        __m256i states = _mm256_load_si256(state_ptr);
        __m256i mask = _mm256_cmpgt_epi32(states, threshold_vec);
        while (_mm256_movemask_epi8(mask)) {

            // Batch extract and write low byte data
            __m256i bytes = _mm256_and_si256(states, _mm256_set1_epi32(0xFF));
            _mm256_storeu_si256(output_ptr, bytes);

            // Parallel right shift by 8 bits
            states = _mm256_srli_epi32(states, 8);
            mask = _mm256_cmpgt_epi32(states, threshold_vec);
        }
    //*/
    std::uint64_t encode(const SymbolStats & stats, std::uint64_t state, Symbol symbol,
                         OutputStream & output_os) {
        std::uint32_t freq = stats.symbols[symbol].freq;
        std::uint32_t cumul = stats.symbols[symbol].cumul;

        // Normalize
        while (state >= (kMaxState / (kTotalFreq + 1)) * freq) {
            std::uint32_t low32 = static_cast<std::uint32_t>(state & 0x00000000FFFFFFFFull);
            output_os.writeUInt32(low32);
            state >>= 32;
        }

        std::uint64_t quotient = state / freq;
        std::uint64_t remainder = state % freq;
        std::uint64_t nextState = quotient * kTotalFreq + cumul + remainder;
        return nextState;
    }

    void finish(std::uint64_t state, OutputStream & output_os) {
        if (state != 0) {
            std::uint32_t low32 = static_cast<std::uint32_t>(state & 0x00000000FFFFFFFFull);
            output_os.writeUInt32(low32);
            if (state > 0x00000000FFFFFFFFull) {
                std::uint32_t high32 = static_cast<std::uint32_t>(state >> 32);
                output_os.writeUInt32(high32);
            }
        }
        output_os.writeUInt32(0);
    }

    int compress(const std::string & input_data, MemoryBuffer & compressed_data) {
        int err_code = 0;
        OutputStream output_os(compressed_data);

        size_type data_size = input_data.size();
        if (ziplab_likely(data_size != 0)) {
            std::uint32_t freqs[kSymbolTotal];
            std::uint32_t cumuls[kSymbolTotal];
            count_freq(input_data, freqs, cumuls, kSymbolTotal);

            SymbolStats stats(kSymbolTotal);
            init_symbol_stats(stats, freqs, cumuls, kSymbolTotal);

            // Output symbol stats
            write_symbol_stats(stats, output_os);

            // Write the content size
            output_os.writeUInt32(static_cast<std::uint32_t>(data_size));
            // Write the compressed data size
            output_os.writeUInt32(0);

            // Start encode
            std::uint64_t state = kInitState;
            for (auto iter = input_data.rbegin(); iter != input_data.rend(); ++iter) {
                Symbol symbol = static_cast<Symbol>(*iter);
                std::string str(1, symbol);
                ::OutputDebugString(str.c_str());
                state = encode(stats, state, symbol, output_os);
            }

            finish(state, output_os);
        }

        return err_code;
    }
};

} // namespace ziplab

#endif // ZIPLAB_RANS_RANSENCODER_HPP

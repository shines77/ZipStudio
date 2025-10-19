
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

namespace ziplab {

template <typename CharT>
class rANSEncoder64 {
public:
    using char_type = CharT;
    using size_type = std::size_t;
    using ssize_type = std::intptr_t;
    using offset_type = std::uint16_t;

    static const size_type kMaxCumulFreq = 65536;
    static const size_type kInitState = 0x7FFFFFFFu;

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

        // Get the scaled frequency
        // 32768, 25206, 5041, 2520
        for (std::size_t i = 0; i < freq_size; i++) {
            stats.symbols[i].freq = freqs[i] * kMaxCumulFreq / stats.total_freq;
        }

        // Calc the scaled cumulative frequency
        // 13, 23, 25, 26
        stats.symbols[0].cumul = 0;
        for (std::size_t i = 0; i < (freq_size - 1); i++) {
            stats.symbols[i + 1].cumul = stats.symbols[i].cumul + stats.symbols[i].freq;
        }
        size_type scale_total_freq = stats.symbols[kMaxSymbol].cumul;

        // Get freq errors
        double freq_errs[kSymbolTotal];
        for (std::size_t i = 0; i < freq_size; i++) {
            freq_errs[i] = (double)freqs[i] * kMaxCumulFreq / stats.total_freq - stats.symbols[i].freq;
        }

        // Adjust the scaled frequency
        for (std::size_t n = scale_total_freq; n < kMaxCumulFreq; n++) {
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
                freq_errs[adjust_symbol] = (double)freqs[adjust_symbol] * kMaxCumulFreq / stats.total_freq
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
        scale_total_freq = stats.symbols[kMaxSymbol].cumul;
        assert(scale_total_freq == kMaxCumulFreq);
    }

public:
    int compress(const std::string & input_data, MemoryBuffer & compressed_data) {
        int err_code = 0;
        OutputStream compressed_os(compressed_data);

        size_type data_size = input_data.size();
        if (ziplab_likely(data_size != 0)) {
            std::uint32_t freqs[kSymbolTotal];
            std::uint32_t cumuls[kSymbolTotal];
            count_freq(input_data, freqs, cumuls, kSymbolTotal);

            SymbolStats stats(kSymbolTotal);
            init_symbol_stats(stats, freqs, cumuls, kSymbolTotal);
        }

        return err_code;
    }
};

} // namespace ziplab

#endif // ZIPLAB_RANS_RANSENCODER_HPP

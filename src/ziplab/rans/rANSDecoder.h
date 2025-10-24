
#ifndef ZIPLAB_RANS_RANSDECODER_HPP
#define ZIPLAB_RANS_RANSDECODER_HPP

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
class rANSDecoder64 {
public:
    using char_type = CharT;
    using size_type = std::size_t;
    using ssize_type = std::intptr_t;
    using offset_type = std::uint16_t;

    static const size_type kTotalFreq = 65536;
    static const size_type kInitState = 0x7FFFFFFFu;
    static const size_type kMaxState  = 0xFFFFFFFFFFFFFFFFull;

    using Symbol = std::uint8_t;

public:
    rANSDecoder64() {
        //
    }

    virtual ~rANSDecoder64() {
        //
    }

private:
    void read_symbol_stats(SymbolStats & stats, InputStream & input_is) {
        // Read symbol range: [min_symbol, max_symbol]
        stats.min_symbol = input_is.readUInt8();
        stats.max_symbol = input_is.readUInt8();

        // Write symbol sacled frequencies
        for (size_type symbol = stats.min_symbol; symbol <= (size_type)stats.max_symbol; symbol++) {
            std::uint16_t freq = input_is.readUInt16();
            stats.symbols[symbol].freq = static_cast<std::uint32_t>(freq);
        }
    }

    void init_symbol_stats(SymbolStats & stats) {
        // Calc symbol cumulative frequency
        stats.symbols[0].cumul = 0;
        for (std::size_t i = 0; i < kSymbolTotal - 1; i++) {
            stats.symbols[i + 1].cumul = stats.symbols[i].cumul + stats.symbols[i].freq;
        }
        size_type scale_total_freq = stats.symbols[kMaxSymbol].cumul;
        stats.total_freq = static_cast<std::uint32_t>(scale_total_freq);
        assert(scale_total_freq == kTotalFreq);
    }

    void read_state_data(InputStream & input_is, std::vector<std::uint32_t> & compressed_states) {
        compressed_states.clear();
        while (!input_is.is_overflow()) {
            std::uint32_t state32 = input_is.readUInt32();
            if (state32 == 0)
                break;
            compressed_states.push_back(state32);
        };
    }

public:
    std::uint64_t decode(const SymbolStats & stats, std::uint64_t state, std::size_t & symbol,
                         std::vector<std::uint32_t>::reverse_iterator iter, OutputStream & output_os) {
        std::uint32_t slot = state % kTotalFreq;

        symbol = 0;
        while (slot >= stats.symbols[symbol + 1].cumul) {
            ++symbol;
            if (symbol > kMaxSymbol) {
                return 0;
            }
        }

        std::uint32_t freq = stats.symbols[symbol].freq;
        std::uint32_t cumul = stats.symbols[symbol].cumul;

        std::uint64_t remainder = slot - cumul;
        std::uint64_t nextState = state * freq / kTotalFreq + remainder;

        // Normalize
        while (state < kInitState) {
            std::uint32_t low32 = static_cast<std::uint32_t>(state & 0x00000000FFFFFFFFull);
            state = (state << 32) | (*iter & 0xFFFFFFFFul);
            ++iter;
        }

        return nextState;
    }

    int decompress(MemoryBuffer & compressed_data, MemoryBuffer & decompressed_data) {
        int err_code = 0;
        InputStream input_is(compressed_data);
        OutputStream output_os(decompressed_data);

        size_type data_size = compressed_data.size();
        if (ziplab_likely(data_size != 0)) {
            SymbolStats stats(kSymbolTotal);
            read_symbol_stats(stats, input_is);
            init_symbol_stats(stats);

            size_type content_size = static_cast<size_type>(input_is.readUInt32());
            size_type compressed_size = static_cast<size_type>(input_is.readUInt32());

            std::vector<std::uint32_t> data_states;
            read_state_data(input_is, data_states);

            std::uint64_t state = 0;

            // Read init state
            ssize_type count = 0;
            for (auto iter = data_states.rbegin(); iter != data_states.rend(); ++iter) {
                state = (state << 32) | (*iter);
                count++;
                if (count >= 2)
                    break;
            }

            for (; count > 0; count--) {
                data_states.pop_back();
            }

            for (auto iter = data_states.rbegin(); iter != data_states.rend();) {
                std::size_t symbol;
                state = decode(stats, state, symbol, iter, output_os);
                if (state != 0) {
                    output_os.writeUInt8(static_cast<Symbol>(symbol));
                }
            }
        }

        return err_code;
    }
};

} // namespace ziplab

#endif // ZIPLAB_RANS_RANSDECODER_HPP

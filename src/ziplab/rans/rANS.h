#ifndef ZIPLAB_RANS_RANS_H
#define ZIPLAB_RANS_RANS_H

#pragma once

#include <cstdint>
#include <cstddef>

#include <vector>

namespace ziplab {

static const std::size_t kSymbolTotal = 256;
static const std::size_t kMaxSymbol = kSymbolTotal - 1;

struct SymbolInfo {
    // Freqence info
    std::uint32_t freq;
    std::uint32_t cumul;

    SymbolInfo() : freq(0), cumul(0) { }
};

struct SymbolStat {
    // Freqence info
    std::uint32_t freq;
    std::uint32_t cumul;

    // Fast div
    std::uint32_t inverse;
    std::uint32_t bias;
    std::uint32_t shift;

    SymbolStat() : freq(0), cumul(0), inverse(0), bias(0), shift(0) {
        //
    }

};

struct SymbolStats {
    std::vector<SymbolStat> symbols;
    std::uint32_t total_freq;
    std::uint8_t min_symbol;
    std::uint8_t max_symbol;

    SymbolStats(std::size_t symbolTotal) :
        total_freq(0),
        min_symbol(static_cast<std::uint8_t>(symbolTotal - 1)),
        max_symbol(0) {
        symbols.resize(symbolTotal + 1);
    }

    ~SymbolStats() {
        symbols.clear();
    }
};

} // namespace ziplab

#endif // ZIPLAB_RANS_RANS_H

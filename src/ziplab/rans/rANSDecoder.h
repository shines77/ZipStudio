
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

    static const size_type kMaxCumulFreq = 65536;
    static const size_type kInitState = 0x7FFFFFFFu;

public:
    rANSDecoder64() {
        //
    }

    virtual ~rANSDecoder64() {
        //
    }

private:
    //

public:
    int decompress(const MemoryBuffer & compressed_data, MemoryBuffer & decompressed_data) {
        int err_code = 0;
        OutputStream compressed_os(decompressed_data);

        size_type data_size = compressed_data.size();
        if (ziplab_likely(data_size != 0)) {
            //
        }

        return err_code;
    }
};

} // namespace ziplab

#endif // ZIPLAB_RANS_RANSDECODER_HPP

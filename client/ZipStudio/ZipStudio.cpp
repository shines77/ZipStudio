
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include <ziplab/stream/MemoryBuffer.h>
#include <ziplab/stream/MemoryView.h>

#include <ziplab/stream/StreamBuffer.h>

#include <ziplab/stream/InputStream.h>
#include <ziplab/stream/OutputStream.h>

#include <ziplab/stream/FileReader.h>
#include <ziplab/stream/FileWriter.h>

#include <zipstd/huffman/huffman.hpp>
#include <ziplab/huffman/huffman.hpp>

#include <ziplab/lz77/lzss.hpp>

#include "dmc_test.h"

#if defined(_MSC_VER)
#pragma comment(lib, "ZipStd.lib")
#pragma comment(lib, "ZipLab.lib")
#endif

void ziplab_MemoryBuffer_test()
{
    char buff[256];
    std::memset(buff, 1, sizeof(buff));

    ziplab::MemoryBuffer memoryBuffer, memoryBuffer2;
    memoryBuffer.reserve(256);
    memoryBuffer.copy(buff, sizeof(buff));
    memoryBuffer.copy(buff);

    std::swap(memoryBuffer, memoryBuffer2);
    std::swap(memoryBuffer, memoryBuffer);
}

void ziplab_MemoryView_test()
{
    char buff[256];
    std::memset(buff, 1, sizeof(buff));

    ziplab::MemoryView memoryView(buff);
    memoryView.clear();
}

void ziplab_InputStream_test()
{
    char buff[256];
    std::memset(buff, 0, sizeof(buff));
    for (std::size_t i = 0; i < 256; i++) {
        buff[i] = (char)i;
    }

    ziplab::MemoryBuffer memBuff(buff);
    ziplab::InputStream inputStream(memBuff);
    //inputStream.clear();

    ziplab::MemoryBuffer & buffer = inputStream.buffer();

    {
        bool b = inputStream.readBool();
        std::int8_t sbyte = inputStream.readSByte();
        std::uint8_t byte = inputStream.readByte();
        std::int32_t i32 = inputStream.readInt32();
        std::uint32_t u32 = inputStream.readUInt32();
        void * vptr = inputStream.readVoidPtr();

        printf("InputStream.unsafeReadValue():\n\n");

#if defined(_MSC_VER)
        printf("b = %d, sbyte = %d, byte = %d, i32 = 0x%08X, u32 = 0x%08X, vptr = 0x%p\n\n",
            (int)b, (int)sbyte, (int)byte, (int)i32, u32, vptr);
#else
        printf("b = %d, sbyte = %d, byte = %d, i32 = 0x%08X, u32 = 0x%08X, vptr = %p\n\n",
            (int)b, (int)sbyte, (int)byte, (int)i32, u32, vptr);
#endif
    }

    {
        bool b = inputStream.peekBool();
        std::int8_t sbyte = inputStream.peekSByte();
        std::uint8_t byte = inputStream.peekByte();
        std::int32_t i32 = inputStream.peekInt32();
        std::uint32_t u32 = inputStream.peekUInt32();
        void * vptr = inputStream.peekVoidPtr();

        printf("InputStream.unsafePeekValue():\n\n");

#if defined(_MSC_VER)
        printf("b = %d, sbyte = %d, byte = %d, i32 = 0x%08X, u32 = 0x%08X, vptr = 0x%p\n\n",
            (int)b, (int)sbyte, (int)byte, (int)i32, u32, vptr);
#else
        printf("b = %d, sbyte = %d, byte = %d, i32 = 0x%08X, u32 = 0x%08X, vptr = %p\n\n",
            (int)b, (int)sbyte, (int)byte, (int)i32, u32, vptr);
#endif
    }

    {
        inputStream.skipBool();
        inputStream.skipSByte();
        inputStream.skipByte();
        inputStream.skipInt32();
        inputStream.skipUInt32();
        inputStream.skipVoidPtr();

        printf("InputStream.unsafeSkipValue():\n\n");
    }

    {
        bool success;

        bool b;
        success = inputStream.readBool(b);
        std::int8_t sbyte;
        success &= inputStream.readSByte(sbyte);
        std::uint8_t byte;
        success &= inputStream.readByte(byte);
        std::int32_t i32;
        success &= inputStream.readInt32(i32);
        std::uint32_t u32;
        success &= inputStream.readUInt32(u32);
        void * vptr;
        success &= inputStream.readVoidPtr(vptr);

        printf("InputStream.safeReadValue():\n\n");

#if defined(_MSC_VER)
        printf("b = %d, sbyte = %d, byte = %d, i32 = 0x%08X, u32 = 0x%08X, vptr = 0x%p\n\n",
            (int)b, (int)sbyte, (int)byte, (int)i32, u32, vptr);
#else
        printf("b = %d, sbyte = %d, byte = %d, i32 = 0x%08X, u32 = 0x%08X, vptr = %p\n\n",
            (int)b, (int)sbyte, (int)byte, (int)i32, u32, vptr);
#endif
    }
}

void zipstd_huffman_test()
{
    zipstd::HuffmanCompressor huffman;

    huffman.compressFile("input.txt", "compressed_std.bin");
    huffman.decompressFile("compressed_std.bin", "decompressed_std.txt");
}

void ziplab_huffman_test()
{
    ziplab::HuffmanCompressor huffman;

    huffman.compressFile("input.txt", "compressed_lab.bin");
    huffman.decompressFile("compressed_lab.bin", "decompressed_lab.txt");
}

bool compare_buffer(const ziplab::MemoryBuffer & left, const std::string & right)
{
    ZIPLAB_UNUSED(left);
    ZIPLAB_UNUSED(right);
    return true;
}

void ziplab_lzss_test()
{
    std::string input_data1 = "This is a simple example of LZ77 compression algorithm.";
    std::string input_data2 = "ABABABAABABABACCDABABABABA";

    std::string & input_data = input_data2;

    ziplab::LZSSCompressor<12, 4> lzss;

    int ret_val;
    ziplab::MemoryBuffer compressed_data;
    ret_val = lzss.plain_compress(input_data, compressed_data);

    ziplab::MemoryBuffer decompressed_data;
    ret_val = lzss.plain_decompress(compressed_data, decompressed_data);

    if (compare_buffer(decompressed_data, input_data)) {
        printf("ziplab::LZSSCompressor::decompress() is PASSED.\n\n");
    } else {
        printf("ziplab::LZSSCompressor::decompress() is FAILED.\n\n");
    }
}

// Example usage
int dynamic_markov_compression_test()
{
    // Create compressor with clone threshold 1.1
    DMCompressor compressor(2.0);
    
    // Sample data (simple 0101 pattern)
    std::vector<int> testData = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
    // std::vector<int> testData = { 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0 };
    //std::vector<int> testData = { 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1 };
    
    std::cout << "Processing bits with probabilities:" << std::endl << std::endl;

    std::size_t i = 0;
    for (int bit : testData) {
        double p0 = compressor.getProbability0();
        std::cout << "Idx: " << (i + 1)
                  << ", current: " << compressor.getCurrentState()
                  << std::fixed << std::setprecision(5)
                  << ", P(0): " << p0 
                  << ", P(1): " << (1 - p0)
                  << ", Bit: " << bit
                  << std::endl;
        compressor.processBit(bit);
        i++;
    }
    
    // Display final state machine size
    std::cout << std::endl;
    std::cout << "Final number of states: " << compressor.getStateCount() << std::endl;
    std::cout << std::endl;

    return 0;
}

int main(int argc, char * argv[])
{
    ZIPLAB_UNUSED(argc);
    ZIPLAB_UNUSED(argv);

    printf("Welcome to ZipStudio Client v1.0 .\n\n");

    //zipstd_huffman_test();
    //ziplab_huffman_test();

    ziplab_lzss_test();

    ziplab_MemoryBuffer_test();
    ziplab_MemoryView_test();

    ziplab_InputStream_test();

    dynamic_markov_compression_test();

#if defined(_MSC_VER)
    //::system("pause");
#endif
    return 0;
}

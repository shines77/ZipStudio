
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include <zipstd/huffman/huffman.hpp>
#include <ziplab/huffman/huffman.hpp>

#include <ziplab/stream/MemoryBuffer.h>
#include <ziplab/stream/MemoryView.h>

#include <ziplab/stream/StreamBuffer.h>

#include <ziplab/stream/InputStream.h>
#include <ziplab/stream/OutputStream.h>

#include <ziplab/stream/FileReader.h>
#include <ziplab/stream/FileWriter.h>

#if defined(_MSC_VER)
#pragma comment(lib, "ZipStd.lib")
#pragma comment(lib, "ZipLab.lib")
#endif

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

    ziplab::InputStream inputStream(buff);
    //inputStream.clear();

    ziplab::MemoryBuffer & buffer = inputStream.buffer();

    bool b = inputStream.readBool();
    std::int8_t sbyte = inputStream.readByte();
    std::uint8_t byte = inputStream.readSByte();
    std::int32_t i32 = inputStream.readInt32();
    std::uint32_t u32 = inputStream.readUInt32();
    void * vptr = inputStream.readVoidPtr();

#if defined(_MSC_VER)
    printf("b = %d, sbyte = %d, byte = %d, i32 = 0x%08X, u32 = 0x%08X, vptr = 0x%p\n\n",
           (int)b, (int)sbyte, (int)byte, (int)i32, u32, vptr);
#else
    printf("b = %d, sbyte = %d, byte = %d, i32 = 0x%08X, u32 = 0x%08X, vptr = %p\n\n",
           (int)b, (int)sbyte, (int)byte, (int)i32, u32, vptr);
#endif
}

int main(int argc, char * argv[])
{
    printf("Welcome to ZipStudio Client v1.0 .\n\n");

    //zipstd_huffman_test();
    //ziplab_huffman_test();

    ziplab_MemoryBuffer_test();
    ziplab_MemoryView_test();

    ziplab_InputStream_test();

#if defined(_MSC_VER)
    ::system("pause");
#endif
    return 0;
}

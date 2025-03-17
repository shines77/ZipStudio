
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
    char Buff[256];
    std::memset(Buff, 1, sizeof(Buff));

    ziplab::MemoryBuffer memoryBuffer;
    memoryBuffer.reserve(256);
    memoryBuffer.copy(Buff, sizeof(Buff));
    memoryBuffer.copy(Buff);
}

void ziplab_MemoryView_test()
{
    char Buff[256];
    std::memset(Buff, 1, sizeof(Buff));

    ziplab::MemoryView memoryView(Buff);
    memoryView.clear();
}

int main(int argc, char * argv[])
{
    printf("Welcome to ZipStudio Client v1.0 .\n\n");

    //zipstd_huffman_test();
    //ziplab_huffman_test();

    ziplab_MemoryBuffer_test();
    ziplab_MemoryView_test();

    return 0;
}

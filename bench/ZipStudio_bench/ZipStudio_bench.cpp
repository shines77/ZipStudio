
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include <ziplab/huffman/huffman.hpp>

#if defined(_MSC_VER)
#pragma comment(lib, "ZipStd.lib")
#pragma comment(lib, "ZipLab.lib")
#endif

int main(int argc, char * argv[])
{
    ZIPLAB_UNUSED(argc);
    ZIPLAB_UNUSED(argv);

    printf("Welcome to ZipStudio Client v1.0 .\n\n");

    ziplab::HuffmanCompressor huffman;

    huffman.compressFile("input.txt", "compressed.bin");
    huffman.decompressFile("compressed.bin", "decompressed.txt");

    return 0;
}

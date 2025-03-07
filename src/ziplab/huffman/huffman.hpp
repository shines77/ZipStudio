#ifndef ZIPLAB_HUFFMAN_HPP
#define ZIPLAB_HUFFMAN_HPP

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <memory>

namespace ziplab {

using HuffmanByte = unsigned char;

class HuffmanNode {
public:
    HuffmanByte   data;
    std::uint32_t freq;

    HuffmanNode * left;
    HuffmanNode * right;

    HuffmanNode(HuffmanByte data, std::uint32_t freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

struct HuffmanCompare
{
    inline bool operator () (HuffmanNode * a, HuffmanNode * b) {
        return (a->freq > b->freq);
    }
};

class HuffmanCompressor {
public:
    using FreqMap = std::unordered_map<HuffmanByte, std::uint32_t>;
    using CodeMap = std::unordered_map<HuffmanByte, std::string>;

    // Compress data
    std::vector<HuffmanByte> compress(const std::vector<HuffmanByte>& data);

    // Decompress data
    std::vector<HuffmanByte> decompress(const std::vector<HuffmanByte>& compressed_data);

    // Compress file
    void compressFile(const std::string & inputFile, const std::string & outputFile);

    // Decompress file
    void decompressFile(const std::string & inputFile, const std::string & outputFile);

private:
    // Build Huffman tree
    HuffmanNode * buildHuffmanTree(const FreqMap & freqMap);

    // Build Huffman tree
    HuffmanNode * buildHuffmanTree(const std::vector<HuffmanByte> & data);

    // Generate encoding table
    void generateHuffmanCodes(HuffmanNode * node, const std::string & code, CodeMap & codes);

    // Serialize Huffman tree
    std::vector<HuffmanByte> serializeTree(HuffmanNode * root);

    // Deserialize Huffman tree
    HuffmanNode * deserializeTree(const std::vector<HuffmanByte> & tree_data, std::size_t & index);
};

} // namespace ziplab

#endif // ZIPLAB_HUFFMAN_HPP
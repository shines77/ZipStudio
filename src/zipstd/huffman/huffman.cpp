
#include "zipstd/huffman/huffman.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <bitset>
#include <algorithm>

#include <assert.h>

namespace zipstd {

HuffmanNode *
HuffmanCompressor::buildHuffmanTree(const FreqMap & freqMap)
{
    // Create priority queue
    HuffmanCompare comp;
    std::priority_queue<HuffmanNode *,
                        std::vector<HuffmanNode *>,
                        decltype(comp)> pq(comp);

    // Add all characters to queue
    for (const auto & iter : freqMap) {
        pq.push(new HuffmanNode(iter.first, iter.second));
    }

    // Build Huffman tree
    while (pq.size() >= 2) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();

        auto parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    return (pq.empty() ? nullptr : pq.top());
}

HuffmanNode *
HuffmanCompressor::buildHuffmanTree(const std::vector<HuffmanByte> & data)
{
    // Calculate frequencies
    FreqMap freqMap;
    for (HuffmanByte c : data) {
        freqMap[c]++;
    }

    return buildHuffmanTree(freqMap);
}

void HuffmanCompressor::generateHuffmanCodes(HuffmanNode * node,
                                             const std::string & code,
                                             CodeMap & codes)
{
    assert(node != nullptr);
    if (!isLeaf(node)) {
        // Internal node
        assert(node->left != nullptr);
        generateHuffmanCodes(node->left,  code + "0", codes);
        
        assert(node->right != nullptr);
        generateHuffmanCodes(node->right, code + "1", codes);
    } else {
        // Leaf node
        codes[node->data] = code;
    }
}

std::vector<HuffmanByte>
HuffmanCompressor::serializeTree(HuffmanNode * node)
{
    std::vector<HuffmanByte> result;
    assert(node != nullptr);

    // Use preorder traversal to serialize the tree
    if (!isLeaf(node)) {
        result.push_back(0);  // Internal node marker
        assert(node->left != nullptr);
        auto left_ser  = serializeTree(node->left);
        assert(node->right != nullptr);
        auto right_ser = serializeTree(node->right);
        result.insert(result.end(), left_ser.begin(),  left_ser.end());
        result.insert(result.end(), right_ser.begin(), right_ser.end());
    } else {
        result.push_back(1);  // Leaf node marker
        result.push_back(node->data);
    }

    return result;
}

HuffmanNode *
HuffmanCompressor::deserializeTree(const std::vector<HuffmanByte> & tree_data, std::size_t & index) {
    if (index >= tree_data.size()) {
        return nullptr;
    }

    if (tree_data[index] == 0) {  // Internal node
        index++;
        assert(index < tree_data.size());

        auto node = new HuffmanNode('\0', 0);

        node->left  = deserializeTree(tree_data, index);
        assert(node->left != nullptr);
        assert(index < tree_data.size());

        node->right = deserializeTree(tree_data, index);
        assert(node->right != nullptr);
        assert(index <= tree_data.size());

        return node;
    } else {  // Leaf node
        index++;
        assert(index < tree_data.size());

        auto node = new HuffmanNode(tree_data[index], 0);
        index++;
        return node;
    }
}

std::vector<HuffmanByte>
HuffmanCompressor::compress(const std::vector<HuffmanByte> & data)
{
    std::vector<HuffmanByte> compressed;
    if (data.empty()) return compressed;

    // Build Huffman tree
    auto root = buildHuffmanTree(data);
    assert(root != nullptr);

    // Generate encoding table
    CodeMap codes;
    generateHuffmanCodes(root, "", codes);

    // Serialize tree structure
    auto tree_data = serializeTree(root);

    // Compress data
    std::string bits;

    // Add tree size
    std::size_t tree_size = tree_data.size();
    for (std::size_t i = 0; i < sizeof(std::size_t); ++i) {
        compressed.push_back(static_cast<HuffmanByte>((tree_size >> (i * 8)) & 0xFF));
    }

    // Add original data size
    std::size_t data_size = data.size();
    for (std::size_t i = 0; i < sizeof(std::size_t); ++i) {
        compressed.push_back(static_cast<HuffmanByte>((data_size >> (i * 8)) & 0xFF));
    }

    // Add tree data
    compressed.insert(compressed.end(), tree_data.begin(), tree_data.end());

    // Compress data body
    for (HuffmanByte c : data) {
        bits += codes[c];
        while (bits.length() >= 8) {
            std::bitset<8> byte(bits.substr(0, 8));
            compressed.push_back(static_cast<HuffmanByte>(byte.to_ulong()));
            bits = bits.substr(8);
        }
    }

    // Handle remaining bits
    if (!bits.empty()) {
        while (bits.length() < 8) {
            bits += '0';
        }
        std::bitset<8> byte(bits);
        compressed.push_back(static_cast<HuffmanByte>(byte.to_ulong()));
    }

    return compressed;
}

std::vector<HuffmanByte>
HuffmanCompressor::decompress(const std::vector<HuffmanByte> & compressed_data)
{
    if (compressed_data.size() < 2 * sizeof(std::size_t)) {
        return {};
    }

    std::size_t pos = 0;

    // Read tree size
    std::size_t tree_size = 0;
    for (std::size_t i = 0; i < sizeof(std::size_t); ++i) {
        tree_size |= static_cast<std::size_t>(compressed_data[pos++]) << (i * 8);
    }

    // Read original data size
    std::size_t data_size = 0;
    for (std::size_t i = 0; i < sizeof(std::size_t); ++i) {
        data_size |= static_cast<std::size_t>(compressed_data[pos++]) << (i * 8);
    }

    // Read and rebuild tree
    std::vector<HuffmanByte> tree_data(compressed_data.begin() + pos,
                                       compressed_data.begin() + pos + tree_size);
    size_t tree_index = 0;
    auto root = deserializeTree(tree_data, tree_index);
    pos += tree_size;

    // Decompress data
    std::vector<HuffmanByte> decompressed;
    auto current = root;
    std::string bits;

    while (decompressed.size() < data_size && pos < compressed_data.size()) {
        std::bitset<8> byte(compressed_data[pos++]);
        bits += byte.to_string();

        while (!bits.empty() && (decompressed.size() < data_size)) {
            if (current == nullptr)
                current = root;

            if (bits[0] == '0') {
                current = current->left;
            } else {
                current = current->right;
            }

            if ((current != nullptr) && isLeaf(current)) {
                decompressed.push_back(current->data);
                current = root;
            }

            bits = bits.substr(1);
        }
    }

    return decompressed;
}

void HuffmanCompressor::compressFile(const std::string & inputFile, const std::string & outputFile)
{
    std::ifstream inFile(inputFile, std::ios::binary);
    std::ofstream outFile(outputFile, std::ios::binary);

    // Calculate char frequencies
    FreqMap freqMap;
    char ch;
    while (inFile.get(ch)) {
        freqMap[(HuffmanByte)ch]++;
    }

    // Build Huffman tree
    HuffmanNode * root = buildHuffmanTree(freqMap);

    // Generate encoding table
    CodeMap huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // Serialize tree structure
    outFile << freqMap.size() << std::endl;
    for (auto & pair : huffmanCodes) {
        outFile << pair.first << " " << pair.second << std::endl;
    }

    // Reset the input file pointer
    inFile.clear();
    inFile.seekg(0);

    // Compresse the data
    std::stringstream compressedData;
    while (inFile.get(ch)) {
        compressedData << huffmanCodes[ch];
    }

    // Write the compressed data to output file
    std::string compressedStr = compressedData.str();
    outFile << compressedStr;

    // Cleanup
    inFile.close();
    outFile.close();
    if (root) {
        delete root;
    }
}

void HuffmanCompressor::decompressFile(const std::string & inputFile, const std::string & outputFile)
{
    std::ifstream inFile(inputFile, std::ios::binary);
    std::ofstream outFile(outputFile, std::ios::binary);

    // Read the encode table
    std::size_t size;
    inFile >> size;

    // ignore the NewLinw chars
    inFile.ignore();

    std::unordered_map<std::string, char> codeMap;
    char ch;
    std::string code;
    for (std::size_t i = 0; i < size; ++i) {
        inFile.get(ch);
        std::getline(inFile, code);
        codeMap[code] = ch;
    }

    // Decompress the data
    std::stringstream compressedData;
    char bit;
    while (inFile.get(bit)) {
        compressedData << bit;
    }
    std::string compressedStr = compressedData.str();

    // Decode data
    std::string decodedStr;
    std::string currentCode;
    for (char bit : compressedStr) {
        currentCode += bit;
        if (codeMap.find(currentCode) != codeMap.end()) {
            decodedStr += codeMap[currentCode];
            currentCode.clear();
        }
    }

    // Write the decode data to output file
    outFile << decodedStr;

    // Cleanup
    inFile.close();
    outFile.close();
}

} // namespace zipstd

// huffman_compressor.cpp
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>
#include <sstream>
#include <sys/stat.h>
#include <chrono>

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    int id; // For stable comparison
    static int globalId;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr), id(globalId++) {}
};

int Node::globalId = 0;

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq == b->freq ? a->id > b->id : a->freq > b->freq;
    }
};

Node* buildHuffmanTree(const std::unordered_map<char, int>& freqMap) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;
    for (const auto& [ch, freq] : freqMap)
        minHeap.push(new Node(ch, freq));

    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();

        Node* merged = new Node('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        minHeap.push(merged);
    }
    return minHeap.top();
}

void generateCodes(Node* root, const std::string& path, std::unordered_map<char, std::string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->ch] = path;
        return;
    }
    generateCodes(root->left, path + "0", codes);
    generateCodes(root->right, path + "1", codes);
}

std::string encodeText(const std::string& text, const std::unordered_map<char, std::string>& codes) {
    std::string result;
    for (char c : text) result += codes.at(c);
    return result;
}

void writeCompressedFile(const std::string& filename, const std::string& bitstring, const std::unordered_map<char, int>& freqMap) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open output file.");

    out.put((char)freqMap.size());
    for (const auto& [ch, freq] : freqMap) {
        out.put(ch);
        out.write(reinterpret_cast<const char*>(&freq), sizeof(freq));
    }

    int count = 0;
    unsigned char byte = 0;
    for (char bit : bitstring) {
        byte = (byte << 1) | (bit - '0');
        count++;
        if (count == 8) {
            out.put(byte);
            count = 0;
            byte = 0;
        }
    }
    if (count > 0) {
        byte <<= (8 - count);
        out.put(byte);
    }
    out.close();
}

std::string readCompressedFile(const std::string& filename, std::unordered_map<char, int>& freqMap) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open compressed file.");

    unsigned char mapSize = in.get();
    for (int i = 0; i < mapSize; ++i) {
        char ch = in.get();
        int freq;
        in.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        freqMap[ch] = freq;
    }

    std::string bits;
    char byte;
    while (in.get(byte)) {
        for (int i = 7; i >= 0; --i)
            bits += ((byte >> i) & 1) ? '1' : '0';
    }
    in.close();
    return bits;
}

std::string decodeText(const std::string& bits, Node* root) {
    std::string decoded;
    Node* current = root;
    for (char bit : bits) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }
    return decoded;
}

void showStats(const std::string& origFile, const std::string& compFile) {
    struct stat oStat, cStat;
    if (stat(origFile.c_str(), &oStat) || stat(compFile.c_str(), &cStat)) {
        std::cerr << "Error getting file sizes.\n";
        return;
    }
    std::cout << "Original: " << oStat.st_size << " bytes\n";
    std::cout << "Compressed: " << cStat.st_size << " bytes\n";
    std::cout << "Compression Ratio: " << (100.0 * cStat.st_size / oStat.st_size) << "%\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage:\n  Compress:    " << argv[0] << " -c input.txt output.huff\n  Decompress:  " << argv[0] << " -d input.huff output.txt\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string inFile = argv[2];
    std::string outFile = argv[3];

    try {
        if (mode == "-c") {
            std::ifstream in(inFile);
            if (!in) throw std::runtime_error("Cannot open input file.");
            std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            in.close();

            std::unordered_map<char, int> freqMap;
            for (char c : text) freqMap[c]++;

            Node* root = buildHuffmanTree(freqMap);
            std::unordered_map<char, std::string> codes;
            generateCodes(root, "", codes);
            std::string bitstring = encodeText(text, codes);

            auto start = std::chrono::high_resolution_clock::now();
            writeCompressedFile(outFile, bitstring, freqMap);
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Compression completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
            showStats(inFile, outFile);

        } else if (mode == "-d") {
            std::unordered_map<char, int> freqMap;
            std::string bitstring = readCompressedFile(inFile, freqMap);

            Node* root = buildHuffmanTree(freqMap);
            std::string decoded = decodeText(bitstring, root);

            auto start = std::chrono::high_resolution_clock::now();
            std::ofstream out(outFile);
            if (!out) throw std::runtime_error("Cannot open output file.");
            out << decoded;
            out.close();
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Decompression completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
        } else {
            std::cerr << "Unknown mode: " << mode << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

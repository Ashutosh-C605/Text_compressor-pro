#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <sys/stat.h> 
#include <chrono>


struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

void generateCodes(Node* root, std::string code, std::unordered_map<char, std::string>& codes) {
    if (!root) return;

    if (!root->left && !root->right) {
        codes[root->ch] = code;
    }

    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Encode text using the Huffman codes
std::string encodeText(const std::string& text, const std::unordered_map<char, std::string>& codes) {
    std::string encoded = "";
    for (char c : text) {
        encoded += codes.at(c);
    }
    return encoded;
}


void writeEncodedToFile(const std::string& encoded, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << filename << "\n";
        return;
    }
    outFile << encoded;
    outFile.close();
    std::cout << "Encoded data written to " << filename << "\n";
}


std::string decodeText(const std::string& encoded, Node* root) {
    std::string decoded = "";
    Node* current = root;

    for (char bit : encoded) {
        if (bit == '0')
            current = current->left;
        else if (bit == '1')
            current = current->right;
        else {
            std::cerr << "Invalid bit in encoded string.\n";
            return "";
        }

        // Leaf node
        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;  // Go back to root for next char
        }
    }

    return decoded;
}
std::string readEncodedFromFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening file for reading: " << filename << "\n";
        return "";
    }
    std::string encoded((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
    inFile.close();
    return encoded;
}


void writeBitsToFile(const std::string& bitString, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    int count = 0;
    unsigned char byte = 0;

    for (char bit : bitString) {
        byte = (byte << 1) | (bit - '0');
        count++;

        if (count == 8) {
            out.put(byte);
            count = 0;
            byte = 0;
        }
    }

    // If remaining bits < 8, pad with 0s on the right
    if (count > 0) {
        byte = byte << (8 - count);
        out.put(byte);
    }

    out.close();
    std::cout << "Bit-level compressed data written to " << filename << "\n";
}
std::string readBitsFromFile(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Error opening file for reading: " << filename << "\n";
        return "";
    }

    std::string bitString = "";
    char byte;

    while (in.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            bitString += ((byte >> i) & 1) ? '1' : '0';
        }
    }

    in.close();
    return bitString;
}



void showCompressionStats(const std::string& originalFile, const std::string& compressedFile) {
    struct stat origStat, compStat;

    if (stat(originalFile.c_str(), &origStat) != 0 || stat(compressedFile.c_str(), &compStat) != 0) {
        std::cerr << "Error getting file sizes.\n";
        return;
    }

    std::cout << "\n--- Compression Stats ---\n";
    std::cout << "Original size   : " << origStat.st_size << " bytes\n";
    std::cout << "Compressed size : " << compStat.st_size << " bytes\n";

    double ratio = (double)compStat.st_size / origStat.st_size * 100.0;
    std::cout << "Compression ratio: " << ratio << "%\n";
}

int main() {
    std::ifstream inputFile("input_main.txt");
    if (!inputFile) {
        std::cerr << "Error: Could not open input_main.txt\n";
        return 1;
    }

    // Read entire file content into string
    std::string text((std::istreambuf_iterator<char>(inputFile)),
                      std::istreambuf_iterator<char>());
    inputFile.close();

    if (text.empty()) {
        std::cerr << "Input file is empty.\n";
        return 1;
    }

    // Step 1: Count frequencies
    std::unordered_map<char, int> freqMap;
    for (char c : text) {
        freqMap[c]++;
    }

    // Step 2: Build min-heap
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;
    for (auto pair : freqMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    // Step 3: Build Huffman Tree
    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();

        Node* merged = new Node('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;

        minHeap.push(merged);
    }

    Node* root = minHeap.top();

    // Step 4: Generate codes
    std::unordered_map<char, std::string> codes;
    generateCodes(root, "", codes);

    // Print codes
    std::cout << "Huffman Codes:\n";
    for (auto pair : codes) {
        if (pair.first == '\n') std::cout << "\\n";
        else if (pair.first == ' ') std::cout << "' '";
        else std::cout << pair.first;
        std::cout << ": " << pair.second << "\n";
    }
     auto startCompress = std::chrono::high_resolution_clock::now();    //compression time starts
        // Encode the whole text using Huffman codes
    std::string encodedText = encodeText(text, codes);

    std::cout << "\nEncoded text as bits (first 100 bits shown):\n";
    std::cout << encodedText.substr(0, 100) << "...\n";

    writeBitsToFile(encodedText, "compressed.bin");

    auto endCompress = std::chrono::high_resolution_clock::now();
    auto compressDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endCompress - startCompress);
    std::cout << "Compression took " << compressDuration.count() << " ms\n";

    // --- Start Decompression Timer ---
    auto startDecompress = std::chrono::high_resolution_clock::now();
    // Read encoded string from file
    std::string encodedFromFile = readBitsFromFile("compressed.bin");


    // Decode it
    std::string decodedText = decodeText(encodedFromFile, root);
     auto endDecompress = std::chrono::high_resolution_clock::now();
    auto decompressDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endDecompress - startDecompress);
    std::cout << "Decompression took " << decompressDuration.count() << " ms\n";
    
    std::cout << "\nDecoded text (first 200 chars):\n";
    std::cout << decodedText.substr(0, 200) << "\n";
    showCompressionStats("input_main.txt", "compressed.bin");

    return 0;
}

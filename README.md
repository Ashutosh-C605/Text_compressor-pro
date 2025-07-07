# 🔠 Text Compressor using Huffman Coding

This project implements a lossless text compression and decompression tool using the **Huffman Coding** algorithm in C++. It reads an input text file, compresses it into a binary format, and later decompresses it back to the original text — demonstrating efficient file size reduction.

---

## 🛠 Features

- 📄 Reads input text from a file (`input_main.txt`)
- 📉 Compresses text using Huffman Coding
- 🗂 Saves compressed output as a binary file (`compressed.bin`)
- 📈 Displays compression statistics (original vs compressed size)
- ♻️ Decompresses binary file to retrieve the original content
- ⏱ Measures compression and decompression time
- 💡 Displays Huffman codes and partial encoded/decoded output

---

## 📂 File Structure

Text-Compressor/
- ├── input_main.txt # Original input file
- ├── compressed.bin # Compressed binary output
- ├── main.cpp # Main compression-decompression logic
- └── README.md # This file

---

## 🚀 How to Run

### 1. 🧾 Prepare Input
Add the text you want to compress in a file named `input_main.txt` in the project root.

### 2. 🧱 Compile the Code
Use any C++ compiler (like g++, clang++):

```bash
g++ main.cpp -o compressor

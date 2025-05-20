# Huffman Text Compressor

This project implements a Huffman coding based text file compressor and decompressor in C++.

## Features

- Builds Huffman tree based on character frequencies
- Encodes and decodes text using Huffman codes
- Performs bit-level file I/O for efficient compression
- Shows compression statistics (original size, compressed size, compression ratio)
- Command line interface for compressing and decompressing files

## Usage

1. Put your input text in `input_text.txt`
2. Run the compressor executable to generate `compressed.bin`
3. Run the decompressor executable to decode back to original text

## Requirements

- C++ compiler (e.g., g++, MSVC)
- C++11 or later

## How It Works

- Reads the input file and counts character frequencies
- Builds a min-heap to construct the Huffman tree
- Generates binary Huffman codes for each character
- Writes compressed bits to a binary file
- Reads compressed file and decodes using the Huffman tree

## Author

Ashutosh Chaturvedi

---

Feel free to customize it further!

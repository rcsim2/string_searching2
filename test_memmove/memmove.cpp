// memmove.cpp
// Created on: 2026-10-19
// Uses Sliding Window (memmove) to search for a string in a file.
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>
#include <iomanip>

struct perf {
    std::chrono::steady_clock::time_point start_;
    perf() : start_(std::chrono::steady_clock::now()) {}
    double elapsed() const {
        auto stop = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = stop - start_;
        return elapsed_seconds.count();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <pattern>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string_view pattern = argv[2];

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    const size_t chunkSize = 64 * 1024 * 1024; // 64 MB buffer
    const size_t overlap = pattern.length() > 1 ? pattern.length() - 1 : 0;
    
    std::vector<char> buffer(chunkSize + overlap);
    std::vector<size_t> positions;

    perf timer;

    size_t fileOffset = 0;
    size_t tailLength = 0;

    while (file) {
        // Retain the overlap tail from previous read
        file.read(buffer.data() + tailLength, chunkSize);
        size_t bytesRead = file.gcount();
        if (bytesRead == 0) break;

        size_t totalValidBytes = tailLength + bytesRead;
        std::string_view sv(buffer.data(), totalValidBytes);

        // Search within current window
        size_t pos = sv.find(pattern);
        while (pos != std::string_view::npos) {
            positions.push_back(fileOffset + pos);
            pos = sv.find(pattern, pos + 1);
        }

        // Prepare overlap for next iteration
        if (totalValidBytes >= overlap && overlap > 0) {
            tailLength = overlap;
            std::memmove(buffer.data(), buffer.data() + totalValidBytes - overlap, overlap);
            fileOffset += (totalValidBytes - overlap);
        } else {
            fileOffset += totalValidBytes;
            tailLength = 0;
        }
    }

    double elapsed_sec = timer.elapsed();

    if (positions.empty()) {
        std::cout << "String not found.\n";
    } else {
        std::cout << "[";
        for (size_t i = 0; i < positions.size(); ++i) {
            std::cout << positions[i] << (i + 1 < positions.size() ? ", " : "");
        }
        std::cout << "]\n";
    }

    std::cout << "Count: " << positions.size() << "\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Time taken: " << elapsed_sec * 1000.0 << " ms\n";

    return 0;
}
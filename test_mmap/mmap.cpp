// mmap.cpp
// Created on: 2026-10-19
// Uses Memory-Mapped Files (mmap) to search for a string in a file.
#include <iostream>
#include <string_view>
#include <vector>
#include <chrono>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

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

    const char* filename = argv[1];
    std::string_view pattern = argv[2];

    // Open file descriptor
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    // Get exact file size
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        std::cerr << "Error getting file size\n";
        close(fd);
        return 1;
    }
    size_t filesize = sb.st_size;

    // Map file into virtual memory
    char* mapped = static_cast<char*>(
        mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0)
    );
    if (mapped == MAP_FAILED) {
        std::cerr << "Memory mapping failed\n";
        close(fd);
        return 1;
    }

    // Tell OS we will access this sequentially (enables aggressive read-ahead)
    madvise(mapped, filesize, POSIX_MADV_SEQUENTIAL);

    // Create string_view over mapped region
    std::string_view sv(mapped, filesize);
    std::vector<size_t> positions;

    perf timer;

    size_t pos = sv.find(pattern);
    while (pos != std::string_view::npos) {
        positions.push_back(pos);
        pos = sv.find(pattern, pos + 1);
    }

    double elapsed_sec = timer.elapsed();

    // Cleanup memory map and descriptor
    munmap(mapped, filesize);
    close(fd);

    // Output results
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
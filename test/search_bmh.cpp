#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <functional>

// ALAS: macOS libc++ does not implement boyer_moore_horspool_searcher

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
    std::string pattern = argv[2];

    // Read file entirely into memory
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return 1;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string content;
    content.resize(size);
    if (!file.read(content.data(), size)) {
        std::cerr << "Error: Could not read file content.\n";
        return 1;
    }

    std::vector<size_t> positions;

    // Start timer
    perf timer;

    // Create the Boyer-Moore-Horspool searcher object
    // (Precomputes the shift table once before searching)
    auto searcher = std::boyer_moore_horspool_searcher(
        pattern.begin(), pattern.end()
    );

    auto it = content.begin();
    auto end = content.end();

    // Find all occurrences
    while (it != end) {
        auto found = std::search(it, end, searcher);
        if (found == end) break;

        size_t pos = std::distance(content.begin(), found);
        positions.push_back(pos);
        it = found + 1; // Advance by 1 to catch subsequent/overlapping occurrences
    }

    double elapsed_sec = timer.elapsed();

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
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>

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

    // Read the file entirely into memory
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

    std::string_view sv(content);
    std::vector<size_t> positions;

    // Start performance timer for the search operation
    perf timer;

    size_t pos = sv.find(pattern);
    while (pos != std::string_view::npos) {
        positions.push_back(pos);
        pos = sv.find(pattern, pos + 1); // Advance by 1 to catch all overlapping/sequential occurrences
    }

    double elapsed_time = timer.elapsed();

    // Output results matching your requested format
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
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time taken: " << elapsed_time * 1000.0 << " ms\n";

    return 0;
}
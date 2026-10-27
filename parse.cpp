#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parses strings like "1-4,10,12" into a dynamically allocated array of integers
// Returns the array and sets 'count' to the total number of elements.
int* parse_algorithms(const char *str, int *count) {
    *count = 0;
    int capacity = 10;
    int *arr = (int *)malloc(capacity * sizeof(int));
    if (!arr) return NULL;

    // Duplicate the string because strtok modifies the string it parses
    char *dup = strdup(str);
    if (!dup) {
        free(arr);
        return NULL;
    }

    char *token = strtok(dup, ",");
    while (token != NULL) {
        char *hyphen = strchr(token, '-');
        
        if (hyphen != NULL) {
            // It's a range (e.g., "1-4")
            *hyphen = '\0'; // Temporarily split the string at the hyphen
            int start = (int)strtol(token, NULL, 10);
            int end = (int)strtol(hyphen + 1, NULL, 10);

            if (start <= end) {
                for (int v = start; v <= end; v++) {
                    if (*count >= capacity) {
                        capacity *= 2;
                        int *temp = (int *)realloc(arr, capacity * sizeof(int));
                        if (!temp) { free(dup); free(arr); return NULL; }
                        arr = temp;
                    }
                    arr[(*count)++] = v;
                }
            }
        } else {
            // It's a single number (e.g., "10")
            int val = (int)strtol(token, NULL, 10);
            if (*count >= capacity) {
                capacity *= 2;
                int *temp = (int *)realloc(arr, capacity * sizeof(int));
                if (!temp) { free(dup); free(arr); return NULL; }
                arr = temp;
            }
            arr[(*count)++] = val;
        }
        token = strtok(NULL, ",");
    }

    free(dup);
    return arr;
}


/*
// C++ Alternative:
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// Parses strings like "1-4,10,12" into a std::vector<int>
std::vector<int> parse_algorithms(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string token;

    // Split by comma ','
    while (std::getline(ss, token, ',')) {
        size_t hyphen_pos = token.find('-');
        
        if (hyphen_pos != std::string::npos) {
            // It's a range (e.g., "1-4")
            std::string start_str = token.substr(0, hyphen_pos);
            std::string end_str = token.substr(hyphen_pos + 1);

            int start = std::stoi(start_str);
            int end = std::stoi(end_str);

            if (start <= end) {
                for (int v = start; v <= end; ++v) {
                    result.push_back(v);
                }
            }
        } else {
            // It's a single number (e.g., "10")
            if (!token.empty()) {
                result.push_back(std::stoi(token));
            }
        }
    }

    return result;
}
*/
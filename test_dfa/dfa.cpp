// See: https://www.youtube.com/watch?v=5KJtYMjeY4U
// And: https://github.com/ComputerBread/algorithms/blob/3dc56d77ab070957ae985931be65aede9c05f04f/DFA-string-matching/dfa.js
#include <iostream>
#include <vector>
#include <string>

// Helper function to convert a 2D vector to a JSON-like string (mimicking JS JSON.stringify)
std::string vector2DToJson(const std::vector<std::vector<int>>& mat) {
    std::string s = "[\n";
    for (size_t i = 0; i < mat.size(); ++i) {
        s += "[";
        for (size_t j = 0; j < mat[i].size(); ++j) {
            s += std::to_string(mat[i][j]);
            if (j + 1 < mat[i].size()) s += ",";
        }
        s += "]";
        if (i + 1 < mat.size()) s += ",\n";
    }
    s += "\n]\n";
    return s;
}

// Helper function to print a 1D vector of integers
void printVector(const std::vector<int>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i + 1 < vec.size()) std::cout << ",";
    }
    std::cout << "]" << std::endl;
}

// Construct the Deterministic Finite Automaton (DFA) for pattern matching
std::vector<std::vector<int>> constructDFA(const std::string& pattern) {
    int m = pattern.length();
    std::vector<int> P(m);
    for (int i = 0; i < m; ++i) {
        P[i] = static_cast<unsigned char>(pattern[i]);
    }
    int alphabetLength = 128; // ASCII

    // Create transition matrix (m+1 states, alphabet length letters)
    std::vector<std::vector<int>> transitions(m + 1, std::vector<int>(alphabetLength, 0));

    // Except the first letter of pattern, which leads to state 1.
    transitions[0][P[0]] = 1;

    int x = 0; // state in which automaton is after reading P[1..q-1]

    for (int q = 1; q <= m; q++) {
        for (int i = 0; i < alphabetLength; i++) {
            transitions[q][i] = transitions[x][i];
        }
        if (q < m) {
            transitions[q][P[q]] = q + 1;
        }
        x = transitions[x][P[q]];
    }
    return transitions;
}

// Perform string matching using the constructed DFA
std::vector<int> dfa_str_matching(const std::string& pattern, const std::string& text) {
    int n = text.length();
    int m = pattern.length();
    std::vector<int> occurrences;
    std::vector<std::vector<int>> dfa = constructDFA(pattern);
    
    std::vector<int> T(n);
    for (int i = 0; i < n; ++i) {
        T[i] = static_cast<unsigned char>(text[i]);
    }

    int state = 0;
    for (int i = 0; i < n; i++) {
        state = dfa[state][T[i]];
        if (state == m) {
            occurrences.push_back(i - m + 1);
        }
    }
    return occurrences;
}

int main() {
    // TEST: construct a dfa
    // Equivalent to console.table(JSON.stringify(constructDFA("ababac")))
    std::cout << "ababac" << std::endl;
    std::cout << vector2DToJson(constructDFA("ababac")) << std::endl;

    // Equivalent to console.log(JSON.stringify(constructDFA("abcabdab")))
    std::cout << "abcabdab" << std::endl;
    std::cout << vector2DToJson(constructDFA("abcabdab")) << std::endl;


    // SEARCH: construct a dfa and do a search
    std::string text = "aaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaabbbbbbbbbbbbbbbbbbbabaaaaaababbbbbaba";
    
    // Equivalent to console.log(dfa_str_matching("aba", text))
    printVector(dfa_str_matching("aba", text));


    // CHECK:
    // Equivalent to text.slice(27, 30) (substr takes start index and length)
    std::cout << text.substr(27, 30 - 27) << std::endl;

    // Equivalent to text.slice(55, 58)
    std::cout << text.substr(55, 58 - 55) << std::endl;

    // Equivalent to text.slice(62, 65)
    std::cout << text.substr(62, 65 - 62) << std::endl;

    // Equivalent to text.slice(70, 73)
    std::cout << text.substr(70, 73 - 70) << std::endl;

    return 0;
}
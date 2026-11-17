# string_searching2

A command-line program that searches a file for the first occurrence of a
case-sensitive string and reports its byte offset.

## Build

With a C++17 compiler:

```sh
clang++ -std=c++17 search.cpp -o search
```

## Usage

```sh
./search <filename> <search-string>
```

For example, search the included sample data for its first frame header:

```sh
./search frames.txt "[Frame 1]"
```

On success, the program prints the byte position and elapsed search time. If
the string is not found, it prints `String not found` and exits with status 1.

## Implementation

The file is read in 16 KiB blocks. Uses Abrash's brute force memchr()/memcmp() by default. `algorithm.h` contains other string-search algorithms for comparison.

## Output

```
rg@MacBook Air M1 string_searching2 % ./search frames.txt "[Frame 868]" -n 10000 -a 1-13
[Frame 868] found at position 136011
Elapsed time: 2.63892 ms

Algos to run: 1 2 3 4 5 6 7 8 9 10 11 12 13 

1. SearchForString
String found at position: 136011
Number of searches: 10000
Total elapsed time: 250.14 ms
Average elapsed time: 0.025014 ms

2. bmsearch
String found at position: 136011
Number of searches: 10000
Total elapsed time: 1114.70 ms
Average elapsed time: 0.111470 ms

3. strsearch
String found at position: 136011
Number of searches: 10000
Total elapsed time: 523.21 ms
Average elapsed time: 0.052321 ms

4. bmh_search
String found at position: 136011
Number of searches: 10000
Total elapsed time: 753.16 ms
Average elapsed time: 0.075316 ms

5. SearchBF
String found at position: 136011
Number of searches: 10000
Total elapsed time: 4864.83 ms
Average elapsed time: 0.486483 ms

6. SearchAUT
String found at position: 136011
Number of searches: 10000
Total elapsed time: 8021.55 ms
Average elapsed time: 0.802155 ms

7. SearchBM
String found at position: 136011
Number of searches: 10000
Total elapsed time: 1040.43 ms
Average elapsed time: 0.104043 ms

8. SearchKMP
String found at position: 136011
Number of searches: 10000
Total elapsed time: 6014.08 ms
Average elapsed time: 0.601408 ms

9. SearchHORSPOOL
String found at position: 136011
Number of searches: 10000
Total elapsed time: 860.04 ms
Average elapsed time: 0.086004 ms

10. SearchGS
String found at position: 136011
Number of searches: 10000
Total elapsed time: 6019.95 ms
Average elapsed time: 0.601995 ms

11. SearchQS
String found at position: 136011
Number of searches: 10000
Total elapsed time: 640.51 ms
Average elapsed time: 0.064051 ms

12. SearchStdStringView
String found at position: 136011
Number of searches: 10000
Total elapsed time: 236.74 ms
Average elapsed time: 0.023674 ms

13. SearchKMP_Textbook
String found at position: 136011
Number of searches: 10000
Total elapsed time: 4496.05 ms
Average elapsed time: 0.449605 ms

--- String Search Performance Leaderboard ---
01. SearchStdStringView  - 0.023674 ms
02. SearchForString      - 0.025014 ms
03. strsearch            - 0.052321 ms
04. SearchQS             - 0.064051 ms
05. bmh_search           - 0.075316 ms
06. SearchHORSPOOL       - 0.086004 ms
07. SearchBM             - 0.104043 ms
08. bmsearch             - 0.111470 ms
09. SearchKMP_Textbook   - 0.449605 ms
10. SearchBF             - 0.486483 ms
11. SearchKMP            - 0.601408 ms
12. SearchGS             - 0.601995 ms
13. SearchAUT            - 0.802155 ms
rg@MacBook Air M1 string_searching2 % 
```

## Names

* **[Robert S. Boyer](https://en.wikipedia.org/wiki/Robert_S._Boyer) (b. 1946):** American computer scientist and UT Austin professor emeritus known for automated theorem proving. Co-invented the **Boyer–Moore string-search algorithm** in **1977**, introducing right-to-left matching to skip large blocks of text.
* **[J Strother Moore](https://en.wikipedia.org/wiki/J_Strother_Moore) (b. 1947):** American computer scientist and retired UT Austin department chair. Co-created the **Boyer–Moore string-search algorithm** (**1977**) and the ACM award-winning Nqthm / ACL2 theorem provers.
* **[Nigel Horspool](https://en.wikipedia.org/wiki/Nigel_Horspool) (b. 1948):** British-Canadian computer scientist and University of Victoria professor emeritus. Simplified the Boyer–Moore algorithm in **1980** by introducing the single bad-character skip table.
* **[Donald Knuth](https://en.wikipedia.org/wiki/Donald_Knuth) (b. 1938):** American mathematician and Stanford professor emeritus. Author of *The Art of Computer Programming*, creator of TeX, and co-inventor of the **Knuth–Morris–Pratt (KMP) algorithm** (**1977**).
* **[James H. Morris](https://en.wikipedia.org/wiki/James_H._Morris) (b. 1941):** American computer scientist and CMU professor emeritus. Introduced fundamental programming concepts (like lazy evaluation) and independently co-developed the **KMP algorithm** (**1977**).
* **[Vaughan Pratt](https://en.wikipedia.org/wiki/Vaughan_Pratt) (b. 1944):** Australian computer scientist, Stanford professor emeritus, and hardware pioneer (early Sun Microsystems workstations). Discovered the core KMP search mechanism in **1970**, co-publishing the **KMP algorithm** in **1977**.

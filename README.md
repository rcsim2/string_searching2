# string_searching2

`string_searching2` is a C++17 command-line benchmark and correctness check for
case-sensitive string searching in files. It is rooted in the original R/C Sim
Sikorsky flight-recorder code and compares implementations from classical
algorithms and C++ library code.

The program can:

- open a binary file and search for an exact, case-sensitive byte sequence;
- find the first occurrence and report its zero-based byte offset;
- search file data in 16 KiB (`0x4000`) restartable blocks so large files do not
  need to be loaded into memory all at once;
- run one or more implementations repeatedly for performance measurements;
- use a file search plus `memchr()`/`memcmp()` implementation as the primary
  fast path and compare it with several Boyer-Moore, Horspool, KMP, brute-force,
  DFA, and Quick Search variants;
- print an average-time leaderboard from fastest to slowest.

## Build

Use a C++17 compiler. On macOS, for example:

```sh
clang++ -std=c++17 search.cpp fsif.cpp graph.cpp parse.cpp -o search
```

The repository already contains a compiled `search` executable for convenience,
but building from source is recommended.

## Usage

```sh
./search <filename> <search-string> [-n <search-count>] [-a <algorithm-list>]
```

Arguments:

- `filename`: file to search. The search is binary-safe and does not treat text
  specially.
- `search-string`: exact, case-sensitive sequence to find. Quote strings or
  patterns containing spaces or shell metacharacters.
- `-n <search-count>`: number of times to repeat each selected search.
  The default is `10000`.
- `-a <algorithm-list>`: algorithms to run, as individual numbers or comma-separated
  ranges. The default is `1-13`.

Examples:

```sh
# Search the included sample file once for each algorithm, using the default count
./search frames.txt "[Frame 868]"

# Run algorithms 1 through 13 ten thousand times
./search frames.txt "[Frame 868]" -n 10000 -a 1-13

# Run only the Abrash implementation and std::string_view implementation
./search frames.txt "[Frame 868]" -n 1000 -a 1,12
```

Exit status `0` means the pattern was found for the selected run; status `1`
means it was not found or command-line arguments were invalid.

## Algorithm selection

| Number | Implementation | Family |
| ---: | --- | --- |
| 1 | `SearchForString` | Abrash `memchr()` / `memcmp()` optimized brute force |
| 2 | `bmsearch` | Boyer-Moore |
| 3 | `strsearch` | Pratt/Boyer-Moore snippet |
| 4 | `bmh_search` | Boyer-Moore-Horspool |
| 5 | `SearchBF` | Brute force |
| 6 | `SearchAUT` | Aho-like table/automaton implementation |
| 7 | `SearchBM` | Boyer-Moore |
| 8 | `SearchKMP` | Knuth-Morris-Pratt |
| 9 | `SearchHORSPOOL` | Horspool |
| 10 | `SearchGS` | Galil-Seiferas-style implementation |
| 11 | `SearchQS` | Quick Search |
| 12 | `SearchStdStringView` | C++ `std::string_view::find()` |
| 13 | `SearchKMP_Textbook` | Textbook Knuth-Morris-Pratt |

The benchmark repeats the complete block-search operation, not just an in-memory
library call. Results vary by CPU, compiler, optimization settings, file layout,
and whether the operating system already has the file cached. Use the absolute
and average times as comparisons for the same machine and run, rather than as
portable performance guarantees.

## Source layout

- `search.cpp`: command-line entry point, block management, benchmark loop, and
  primary optimized search implementation.
- `search.h`: declarations for the primary search function.
- `algorithm.h`: Boyer-Moore, Horspool, KMP, brute-force, and other comparison
  implementations.
- `fsif.cpp` / `fsif.h`: 512-byte overlapping-block file search adapted from
  Bob Stout's Code Snippets.
- `parse.cpp` / `parse.h`: parser for algorithm ranges and lists such as `1-13`
  and `1,4,10-12`.
- `frames.txt`: sample binary/text fixture containing frame markers.
- `test_all/`, `test_mmap/`, `test_memmove/`, and `test_dfa/`: focused experiments
  and standalone prototypes.
- `pdf/`: reference papers and algorithm notes.
- `web/`: interactive browser visualizers and generated web experiments.

## Implementation notes

The main search routine reads up to 16 KiB at a time and carries pattern-length
minus one bytes across block boundaries. This prevents a match that begins near
one block's end and continues into the next block from being missed.

The primary implementation first uses `memchr()` to find a possible first byte,
then checks the remaining bytes with `memcmp()`. The algorithms in `algorithm.h`
are included for comparison and may use different buffer-length conventions;
`search.cpp` distinguishes the valid start-position count from the complete
working-block length where necessary.

Before the repeated benchmark starts, the program uses `find_str_in_file()` as
an independent 512-byte-block check and prints the located position. The benchmark runs each selected implementation repeatedly and reports both the total and per-search elapsed time.

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

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

## Names

* **[Robert S. Boyer](https://en.wikipedia.org/wiki/Robert_S._Boyer) (b. 1946):** American computer scientist and UT Austin professor emeritus known for automated theorem proving. Co-invented the **Boyer–Moore string-search algorithm** in **1977**, introducing right-to-left matching to skip large blocks of text.
* **[J Strother Moore](https://en.wikipedia.org/wiki/J_Strother_Moore) (b. 1947):** American computer scientist and retired UT Austin department chair. Co-created the **Boyer–Moore string-search algorithm** (**1977**) and the ACM award-winning Nqthm / ACL2 theorem provers.
* **[Nigel Horspool](https://en.wikipedia.org/wiki/Nigel_Horspool) (b. 1948):** British-Canadian computer scientist and University of Victoria professor emeritus. Simplified the Boyer–Moore algorithm in **1980** by introducing the single bad-character skip table.
* **[Donald Knuth](https://en.wikipedia.org/wiki/Donald_Knuth) (b. 1938):** American mathematician and Stanford professor emeritus. Author of *The Art of Computer Programming*, creator of TeX, and co-inventor of the **Knuth–Morris–Pratt (KMP) algorithm** (**1977**).
* **[James H. Morris](https://en.wikipedia.org/wiki/James_H._Morris) (b. 1941):** American computer scientist and CMU professor emeritus. Introduced fundamental programming concepts (like lazy evaluation) and independently co-developed the **KMP algorithm** (**1977**).
* **[Vaughan Pratt](https://en.wikipedia.org/wiki/Vaughan_Pratt) (b. 1944):** Australian computer scientist, Stanford professor emeritus, and hardware pioneer (early Sun Microsystems workstations). Discovered the core KMP search mechanism in **1970**, co-publishing the **KMP algorithm** in **1977**.

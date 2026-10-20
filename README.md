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

The file is read in 16 KiB blocks. The active search implementation is the
Pratt-Boyer-Moore routine in `algorithm.h`; the headers also contain other
string-search algorithms used for comparison.

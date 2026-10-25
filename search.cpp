//-----------------------------------------------------------------------------
// File: search.cpp
//
// Desc: Searches file for a string. Adapted from Michael Abrash's Blackbook,
//		 Listing 5.1
//
// Note: Uses the memchr()/memcmp() method. Optimized brute-force. Blazingly fast.
//		 Beats Boyer-Moore.
//
// Copyright (c) 2000 Black Sphere Corp.
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 2026-10-19:
// RG: Some small changes to make it run on macOS. The original code was
// written for DOS/Windows.
//
// Taken from R/C Sim Sikorsky. SearchStringInFile() was called from the
// gameloop by ResetFilePointer() which set the file pointer to 
// [Frame g_iFrameCount]. There the m_matFileObjectMatrix was read.
// It was used for playing back recorded flights. 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Performance: (finding last frame in a 868 frame file)
//                                            2000 (P4)     2026 (M2)
// Michael Abrash's Blackbook
// SearchForString() using memchr()/memcmp(): 4.8 ms        0.023 ms
//
// Robert Sedgewick's Algorithms in C
// bmsearch:		                          5.8 ms        0.116 ms
//
// Bob Stout's Code Snippets
// find_str_in_file() using strncmp():      103.4 ms        2.803 ms
// strsearch:		                          6.9 ms        0.051 ms
// bmh_search:		                            ? ms        0.077 ms
//
// Thierry Lecroq's Exact String Matching Algorithms
// SearchBF:	                             30.2 ms        0.503 ms
// SearchAUT:		                            ? ms        0.806 ms
// SearchBM:		                          6.5 ms.       0.106 ms
// SearchTBM:		                          9.0 ms
// SearchTUNEDBM:	                          5.5 ms
// SearchKMP:	                             17.2 ms
// SearchHORSPOOL:	                          5.5 ms
// SearchRC:	                             10.8 ms
// SearchNSN:	                             11.3 ms
// SearchZT:	                             70.2 ms
// SearchBR:	                             71.3 ms
// SearchQS:		                          7.3 ms
// SearchSMITH:		                          8.1 ms
// SearchGS:	                             41.7 ms
// SearchRAITA:	                              5.3 ms
//-----------------------------------------------------------------------------

//#include "precompiled.h"
//#include "resource.h"
//#include "xfile.h"

//#define STRICT
//#define D3D_OVERLOADS

#include "search.h"

#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/fcntl.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <vector>
//#include <format>
//#include <iomanip>
#include <string_view>

struct perf {
    std::chrono::steady_clock::time_point start_;
    perf() : start_(std::chrono::steady_clock::now()) {}
    double elapsed() const {
        auto stop = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = stop - start_;
        return elapsed_seconds.count();
    }
};


struct BenchmarkResult {
    std::string name;
    double duration_ms;
};


/*
 * Program to search the file specified by the first command-line
 * argument for the string specified by the second command-line
 * argument. Performs the search by reading and searching blocks
 * of size BLOCK_SIZE.
 */
//#include <stdio.h>
//#include <fcntl.h>
//#include <string.h>
//#include <malloc.h>   // alloc.h for Borland compilers,
                        // malloc.h for Microsoft compilers
//#include <io.h>


#define BLOCK_SIZE  0x4000   /* we'll process the file in 16KB
                                 blocks */


// TODO: het zoek algoritme is zeer snel, nu nog een pointer naar
// de gevonden string...
// DONE

unsigned char *pdest = NULL;
long result = 0;

#include "fsif.h"
#include "graph.h"
#include "algorithm.h"



/* Searches the specified number of sequences in the specified buffer
   for matches to SearchString of SearchStringLength. Note that the
   calling code should already have shortened SearchLength if
   necessary to compensate for the distance from the end of the buffer
   to the last possible start of a matching sequence in the buffer */
//-----------------------------------------------------------------------------
// Name: SearchForString()
// Desc: Optimized brute-force string searching. Uses the memchr()/memcmp() method.
//		 Blazingly fast. Beats Boyer-Moore.
//-----------------------------------------------------------------------------
int SearchForString(unsigned char *Buffer, int SearchLength,
                    unsigned char *SearchString, int SearchStringLength)
{
	unsigned char *PotentialMatch;

    // NEW: save initial Buffer begin
	pdest = Buffer;	
	

    /* Search so long as there are potential-match locations
        remaining */
    while ( SearchLength ) {
        /* See if the first character of SearchString can be found */
        if ( (PotentialMatch =
                (unsigned char*)memchr(Buffer, *SearchString, SearchLength)) == NULL ) {
            break;   /* no matches in this buffer */
        }

        /* The first character matches; see if the rest of the string
            also matches */
        if ( SearchStringLength == 1 ) {
            // NEW: get position of search string in the buffer
            result = PotentialMatch - pdest;
            return(1);  /* that 1 matching character was the whole
                            search string, so we've got a match */
        } else {
            /* Check whether the remaining characters match */
            if ( !memcmp(PotentialMatch + 1, SearchString + 1,
                SearchStringLength - 1) ) {
                // NEW: get position of search string in the buffer
                result = PotentialMatch - pdest;
                return(1);  /* we've got a match */
            }
        }

        /* The string doesn't match; keep going by pointing past the
            potential-match location we just rejected */
        SearchLength -= PotentialMatch - Buffer + 1;
        Buffer = PotentialMatch + 1;

    }

   return(0);  /* no match found */
}



//-----------------------------------------------------------------------------
// Name: SearchStdStringView()
// Desc: Uses std::string_view::find(). Beats Abrash.
//-----------------------------------------------------------------------------
int SearchStdStringView(const unsigned char *buffer, int buffer_length,
                        const unsigned char *search_string, int search_string_length) 
{
    if (search_string_length <= 0 || buffer_length <= 0 || search_string_length > buffer_length) {
        return 0; // 0 = no match found
    }

    // Zero-copy wrap of the raw buffers
    std::string_view text(reinterpret_cast<const char*>(buffer), buffer_length);
    std::string_view pattern(reinterpret_cast<const char*>(search_string), search_string_length);

    size_t pos = text.find(pattern);
    
    if (pos == std::string_view::npos) {
        return 0; // 0 = no match found
    }

    // Store the 0-based offset into 'result' just like Abrash's code
    result = static_cast<int>(pos);

    return 1; // 1 = match found
}
///////////////////////////////////////





int main(int argc, char *argv[]) {
//-----------------------------------------------------------------------------
// Name: SearchStringInFile()
// Desc: Function to search a string in a file using restartable blocks.
// Note: This function starts searching from the current file pointer position and
//		 returns the position of the first occurence of the string relative to the
//		 current file pointer position
// Note: We were getting a memory leak when calling malloc() without free(). It's
//		 fixed now.
//-----------------------------------------------------------------------------
//long SearchStringInFile(FILE *fpFile, char *pszStr)
//{
    int Done;               /* indicates whether search is done */
    int Handle;             /* handle of file being searched */
    int WorkingLength;      /* length of current block */
    int SearchStringLength; /* length of string to search for */
    int BlockSearchLength;  /* length to search in current block */
    int Found;              /* indicates final search completion
                              status */
    int NextLoadCount;      /* # of bytes to read into next block,
                              accounting for bytes copied from the last
                              block */
    unsigned char *WorkingBlock;  /* block storage buffer */
    unsigned char *SearchString;  /* pointer to string to search for */
    unsigned char *NextLoadPtr;   /* offset at which to start loading
                                    the next block, accounting for
                                    bytes copied from the last
                                    block */

    // NEW: 
    //unsigned char *pdest2 = NULL;
    long result2 = 0;


    /* Check for the proper number of arguments */
    if ( argc != 3 ) {
        printf("Usage: search filename search-string\n");
        exit(1);
    }

    /* Try to open the file to be searched */
    if ( (Handle = open(argv[1], O_RDONLY /* | O_BINARY */)) == -1 ) {
        printf("Can't open file: %s\n", argv[1]);
        exit(1);
    }

  
    /* Calculate the length of text to search for */
    SearchString = (unsigned char*)argv[2];
    //SearchString = (unsigned char*)pszStr;
    SearchStringLength = strlen((const char*)SearchString);

    /* Try to get memory in which to buffer the data */
    // NOTE NOTE NOTE: 
    // Avoid memory leaks: malloc() must have a corresponding free()!!! 
    if ( (WorkingBlock = (unsigned char*)malloc(BLOCK_SIZE)) == NULL ) {
        printf("Can't get enough memory\n");
        exit(1);
        //return -2;
    }

    /* Load the first block at the start of the buffer, and try to fill
        the entire buffer */
    NextLoadPtr = WorkingBlock;
    NextLoadCount = BLOCK_SIZE;

    Done = 0;      /* not done with search yet */
    Found = 0;     /* assume we won't find a match */


   
    // NOTE: No, let's not do this. We want to allow searching for multiple
    // occurences of the search string. Therefore, we must allow starting
    // search from other positions in the file.
    // Caller can always reset file pointer to begin of file to find the first 
    // occurence of the string.
    // Actually, we should give a file offset option in a parameter
    // NEW: set file pointer to begin of file
    //fseek(fpFile, 0, SEEK_SET) ;

    /////////////////////////////////////////////
    // We keep this to check if the result matches with the other search algos.
    // NOTE: needs fsif.h and fsif.cpp
    // TEST: find_str_in_file()
    FILE *stream;
    if( (stream = fopen(argv[1], "rb")) == NULL ) {
		printf("The file %s was not opened\n", argv[1]);
        exit(1);
    }
    perf p0;
    long lStatusFlag = find_str_in_file(stream, 0, argv[2]);

    switch(lStatusFlag) {
		case -1:        
			printf("%s not found\n\n", argv[2] );
			break;
		case -2:
			printf(	"Not enough memory to perform search\n"	);
			break;
		default:
			printf("%s found at position %ld\n", argv[2], lStatusFlag );
            std::cout << "Elapsed time: " << p0.elapsed() * 1000.0 << " ms" << std::endl;
            std::cout << std::endl;
	}	
    /////////////////////////////////////////////


    // TEST: restart the search using goto.
    // Ugly but works
    // NOTE: need "externalConsole": true in launch.json otherwise Debug Console
    // will crash. It cannot handle all the output from the loop. It will crash
    // after a few iterations. The external console can handle it.
    int number_of_searches = 0;
    double elapsed_time = 0.0;
    int search_algo = 1;
    std::vector<BenchmarkResult> results;
    
start:
    if (lseek(Handle, 0, SEEK_SET) == -1) {
        perror("lseek");
        return 1;
    }

    WorkingBlock = static_cast<unsigned char*>(malloc(BLOCK_SIZE));
    if (WorkingBlock == nullptr) {
        printf("Can't get enough memory\n");
        return 1;
    }

    NextLoadPtr = WorkingBlock;
    NextLoadCount = BLOCK_SIZE;
    Done = 0;
    Found = 0;
    result2 = 0;
    ///////////////////


    perf p1;

    /* Search the file in BLOCK_SIZE chunks */
    do {
        //MessageBeep(-1);
            
        // NOTE: The search always starts from the current file pointer position.
        // Before doing the *first* fread() we should get the current file pointer (ftell())
        // and save this value. We should then add this value to result2 + result to report
        // the correct position of the string in the file. This would allow a user of this
        // function to search for multiple occurences of the string in a while loop.
        // However, because we should only do that the first time of the multiple search
        // we cannot really do that here. The caller himself must do those calculations.
        // See: BuildFrameIndexTable()
	   

        /* Read in however many bytes are needed to fill out the block
            (accounting for bytes copied over from the last block), or
            the rest of the bytes in the file, whichever is less */
        if ( (WorkingLength = read(Handle, NextLoadPtr,
            NextLoadCount)) == -1 ) {
            // NEW:
            //   WorkingLength = fread(NextLoadPtr, sizeof(char),
            //         NextLoadCount, fpFile);
            //   if ( ferror(fpFile) ) {
            printf("Error reading file %s\n", argv[1]);
            exit(1);
            //MessageBox(NULL, "Error reading file", "R/C Sim Sikorsky Error Message", MB_OK|MB_ICONERROR);
            //return -1;
        }	
		
		

        /* If we didn't read all the bytes we requested, we're done
            after this block whether we find a match or not */
        if ( WorkingLength != NextLoadCount ) {
            Done = 1;
        }

        /* Account for any bytes we copied from the end of the last
            block in the total length of this block */
        WorkingLength += NextLoadPtr - WorkingBlock;

        /* Calculate the number of bytes in this block that could
            possibly be the start of a matching sequence that lies
            entirely in this block (sequences that run off the end of the
            block will be transferred to the next block and found when
            that block is searched) */
        // NOTE: The +1 counts the starting position where the search string
        // exactly reaches the end of the block.
        // For a block of length 5 and a search string of length 3, the
        // valid start positions are 0, 1, and 2 — three positions total:
        // 5 - 3 + 1 = 3 
        // NOTE: - and + have same precedence, so they are evaluated left to right.
        // Without the +1, the final valid position would be skipped.
        // BlockSearchLength is the number of possible starting positions.
        if ( (BlockSearchLength =
               WorkingLength - SearchStringLength + 1) <= 0 ) {
            Done = 1;   /* too few characters in this block for there
                            to be any possible matches, so this is the
                            final block and we're done without finding a
                            match */
        } else {
            /* Search this block */

            int search_result = 0;
            //int search_algo = 1; 

            switch (search_algo) {
                case 1:
                    // NOTE: needs BlockSearchLength
                    search_result = SearchForString(WorkingBlock, BlockSearchLength, SearchString, SearchStringLength);
                    break;
                case 2:
                    search_result = bmsearch((char*)WorkingBlock, WorkingLength, (char*)SearchString, SearchStringLength);
                    break;
                case 3:
                    search_result = strsearch((char*)WorkingBlock, WorkingLength, (char*)SearchString, SearchStringLength);
                    break;
                case 4:
                    search_result = bmh_search((char*)WorkingBlock, WorkingLength, (char*)SearchString, SearchStringLength);
                    break;
                case 5:
                    search_result = SearchBF((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength);
                    break;
                case 6:
                    search_result = SearchAUT((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength);
                    break;
                case 7:
                    search_result = SearchBM((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength);
                    break;
                case 8:
                    search_result = SearchKMP((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength);
                    break;
                case 9:
                    search_result = SearchHORSPOOL((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength);
                    break;
                case 10:
                    search_result = Algorithm::SearchGS((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength);
                    break;
                case 11:
                    search_result = SearchQS((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength);
                    break;
                case 12:
                    search_result = SearchStdStringView(WorkingBlock, WorkingLength, SearchString, SearchStringLength);
                    break;
            }

            // Michael Abrash's Blackbook
            // Optimized brute-force
            //
            // NOTE: SearchForString takes the number of candidate match starts, so keep
            // passing BlockSearchLength.
            // SearchForString uses its SearchLength argument as the number of bytes memchr
            // scans for the pattern’s first character. Each hit is a candidate start; memcmp
            // then checks whether the rest of the pattern matches.
            // For example, with 10 valid bytes and a 4-byte pattern, the pattern can start at
            // offsets 0 through 6—7 candidates. BlockSearchLength is 7, so the function checks
            // only those starts. If it scanned all 10 bytes (WorkingLength), it could find the
            // first pattern character at offset 7, 8, or 9, where the full pattern doesn’t fit.
            // That’s why SearchForString should receive BlockSearchLength, while algorithms
            // that expect the total text-buffer length should receive WorkingLength.
            //
            //if ( SearchForString(WorkingBlock, BlockSearchLength, SearchString, SearchStringLength) )
            

            // Robert Sedgewick's Algorithms in C
            // Boyer-Moore
            //
            // NOTE: Must use WorkingLength here instead of BlockSearchLength. 
            // Otherwise we fail to find the search string when it is at the very end
            // of the file.
            // See comment in algorithm.h
            // WorkingLength vs BlockSearchLength:
            // * WorkingLength is the number of valid bytes currently in the buffer. It includes any bytes carried over from the previous block.
            // * BlockSearchLength is the number of valid starting positions where the entire search string could fit in those bytes.
            // So: BlockSearchLength = WorkingLength - SearchStringLength + 1;
            //
            // ERROR: if ( bmsearch((char*)WorkingBlock, BlockSearchLength, (char*)SearchString, SearchStringLength) )
            //if ( bmsearch((char*)WorkingBlock, WorkingLength, (char*)SearchString, SearchStringLength) )
            

            // Bob Stout's Code Snippets
            // Pratt-Boyer-Moore
            // ERROR: finds incorrect position for [Frame 868]
            // FIXED: see comment in algorithm.h
            // NOTE: Must use WorkingLength here instead of BlockSearchLength.
            //if ( strsearch((char*)WorkingBlock, WorkingLength, (char*)SearchString, SearchStringLength) )
            // Boyer-Moore-Horspool
            //if ( bmh_search((char*)WorkingBlock, WorkingLength, (char*)SearchString, SearchStringLength) )
            

            // Thierry Lecroq's Exact String Matching Algorithms
            //if ( SearchBF((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            // ERROR: if ( SearchAUT((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            // We need WorkingLength not BlockSearchLength. Otherwise we fail to find
            // the search string when it is at the very end of the file.
            //if ( SearchAUT((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchBM((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchTBM((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchTUNEDBM((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchKMP((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchHORSPOOL((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchRC((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchNSN((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )		 
            //if ( SearchZT((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchBR((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchQS((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchSMITH((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( Algorithm::SearchGS((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            //if ( SearchRAITA((char*)SearchString, SearchStringLength, (char*)WorkingBlock, WorkingLength) )
            
            if ( search_result == 1 )
            {
                Found = 1;     /* we've found a match */
                Done = 1;
                // NOTE NOTE NOTE:
                // No memory leaks!!!
                free( WorkingBlock );
            } else {
                /* Copy any bytes from the end of the block that start
                potentially-matching sequences that would run off the
                end of the block over to the next block */
                if ( SearchStringLength > 1 ) {
                memcpy(WorkingBlock,
                    WorkingBlock + BLOCK_SIZE - SearchStringLength + 1,
                    SearchStringLength - 1);
                }	

			

                /* Set up to load the next bytes from the file after the
                bytes copied from the end of the current block */
                NextLoadPtr = WorkingBlock + SearchStringLength - 1;
                NextLoadCount = BLOCK_SIZE - SearchStringLength + 1;


                // NEW: keep track of number of bytes loaded from file
                result2 += NextLoadCount;

                //TCHAR msg1[512];
                //sprintf(msg1, "%i ", result2);
                //MessageBox(NULL,msg1,"QQQ",MB_OK);
			
            }
        }
    } while ( !Done );

    /* Report the results */
    if ( Found ) {
        //printf("String found\n");
        // return position of search string in the file
        //return result2 + result;
    //    printf("String found at position: %ld\n", (long)(result2 + result));
        //std::cout << "Elapsed time: " << p1.elapsed() << "s" << std::endl;
    //    std::cout << "Elapsed time: " << p1.elapsed() * 1000.0 << " ms" << std::endl;
        
        // TEST:
        number_of_searches++;
        elapsed_time += p1.elapsed() * 1000.0;
    //    printf("Number of searches: %d\n", number_of_searches);
    //    printf("Total elapsed time: %.2f ms\n\n", elapsed_time);
        if (number_of_searches < 10'000)
            goto start;

        switch (search_algo)
        {
        case 1:
            printf("1. SearchForString\n");
            results.push_back({"SearchForString", elapsed_time / number_of_searches});
            break;
        case 2:
            printf("2. bmsearch\n");
            results.push_back({"bmsearch", elapsed_time / number_of_searches});
            break;
        case 3:
            printf("3. strsearch\n");
            results.push_back({"strsearch", elapsed_time / number_of_searches});
            break;
        case 4:
            printf("4. bmh_search\n");
            results.push_back({"bmh_search", elapsed_time / number_of_searches});
            break;
        case 5:
            printf("5. SearchBF\n");
            results.push_back({"SearchBF", elapsed_time / number_of_searches});
            break;
        case 6:
            printf("6. SearchAUT\n");
            results.push_back({"SearchAUT", elapsed_time / number_of_searches});
            break;
        case 7:
            printf("7. SearchBM\n");
            results.push_back({"SearchBM", elapsed_time / number_of_searches});
            break;
        case 8:
            printf("8. SearchKMP\n");
            results.push_back({"SearchKMP", elapsed_time / number_of_searches});
            break;
        case 9:
            printf("9. SearchHORSPOOL\n");
            results.push_back({"SearchHORSPOOL", elapsed_time / number_of_searches});
            break;
        case 10:
            printf("10. SearchGS\n");
            results.push_back({"SearchGS", elapsed_time / number_of_searches});
            break;
        case 11:
            printf("11. SearchQS\n");
            results.push_back({"SearchQS", elapsed_time / number_of_searches});
            break;
        case 12:
            printf("12. SearchStdStringView\n");
            results.push_back({"SearchStdStringView", elapsed_time / number_of_searches});
            break;
        }
        printf("String found at position: %ld\n", (long)(result2 + result));
        printf("Number of searches: %d\n", number_of_searches);
        printf("Total elapsed time: %.2f ms\n", elapsed_time);
        printf("Average elapsed time: %.6f ms\n\n", elapsed_time / number_of_searches);    
        number_of_searches = 0;
        elapsed_time = 0.0;
        search_algo++;
        if (search_algo <= 12)
            goto start;
            
        // Print benchmark results //////////////
        // Sort by time (ascending: fastest to slowest)
        std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
            return a.duration_ms < b.duration_ms;
        });

        // Print the sorted leaderboard
        std::cout << "--- String Search Performance Leaderboard ---\n";
        int rank = 1;
        for (const auto& r : results) {
            // std::cout << std::setw(2) << std::setfill(' ') << rank++ << ". " // 01, 02, etc.
            //         << std::left << std::setw(15) << r.name               // Left-align name in a 15-char column
            //         << " - " 
            //         << std::right << std::fixed << std::setprecision(6) << r.duration_ms << " ms\n";            // Right-align time
            // Cut that chevron hell
            // ALAS: Apple clang version 14.0.0 has no format yet
            //std::cout << std::format("{:2}. {:<15} - {:.6f} ms\n", rank++, r.name, r.duration_ms);
            printf("%02d. %-20s - %.6f ms\n", rank++, r.name.c_str(), r.duration_ms);
        }
        /////////////////////////////////////////

        return 0; // Exit code 0: Success
    } else {
        // String not found. All algos should give the same result.
        switch (search_algo)
        {
        case 1:
            printf("1. SearchForString\n");
            break;
        case 2:
            printf("2. bmsearch\n");
            break;
        case 3:
            printf("3. strsearch\n");
            break;
        case 4:
            printf("4. bmh_search\n");
            break;
        case 5:
            printf("5. SearchBF\n");
            break;
        case 6:
            printf("6. SearchAUT\n");
            break;
        case 7:
            printf("7. SearchBM\n");
            break;
        case 8:
            printf("8. SearchKMP\n");
            break;
        case 9:
            printf("9. SearchHORSPOOL\n");
            break;
        case 10:
            printf("10. SearchGS\n");
            break;
        case 11:
            printf("11. SearchQS\n");
            break;
        case 12:
            printf("12. SearchStdStringView\n");
            break;
        }
        printf("String not found\n\n");
        number_of_searches = 0;
        elapsed_time = 0.0;
        search_algo++;
        if (search_algo <= 12)
            goto start;

        //MessageBox(NULL,"String not found","QQQ!",MB_OK);
        //return -1;
        return 1; // Exit code 1: Not found / logical failure
    }

    //exit(Found);   /* return the found/not found status as the
    //                  DOS errorlevel */
}

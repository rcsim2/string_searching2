//-----------------------------------------------------------------------------
// File: search.cpp
//
// Desc: Searches file for a string. Adapted from Michael Abrash's Blackbook,
//		 Listing 5.1
//
// Note: uses the memchr()/memcmp() method. Optimized brute-force. Blazingly fast.
//		 Beats Boyer-Moore.
//
// Copyright (c) 2000 Black Sphere Corp.
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 2026-10-19:
// RG: Some small changes to make it run on macOS. The original code was
// written for DOS/Windows.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Performance: (finding last frame in a 868 frame file)
//
// Michael Abrash's Blackbook                 2000 (P4)     2026 (M2)
// SearchForString() using memchr()/memcmp(): 4.8 ms        0.07 ms
//
// Robert Sedgewick's Algorithms in C
// bmsearch:		                          5.8 ms        0.25 ms
//
// Bob Stout's Code Snippets
// find_str_in_file() using strncmp():      103.4 ms
// strsearch:		                          6.9 ms
// bmh_search:		                            ? ms
//
// Thierry Lecroq's Exact String Matching Algorithms
// SearchBF:	                             30.2 ms
// SearchAUT:		                            ? ms
// SearchBM:		                          6.5 ms
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


struct perf {
    std::chrono::steady_clock::time_point start_;
    perf() : start_(std::chrono::steady_clock::now()) {}
    double elapsed() const {
        auto stop = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = stop - start_;
        return elapsed_seconds.count();
    }
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


#define BLOCK_SIZE  0x4000   /* we'll process the file in 16Kb
                                 blocks */


// TODO: het zoek algoritme is zeer snel, nu nog een pointer naar
// de gevonden string...
// DONE

unsigned char *pdest = NULL;
long result = 0;



#include "algorithm.h"



/* Searches the specified number of sequences in the specified buffer
   for matches to SearchString of SearchStringLength. Note that the
   calling code should already have shortened SearchLength if
   necessary to compensate for the distance from the end of the buffer
   to the last possible start of a matching sequence in the buffer */
//-----------------------------------------------------------------------------
// Name: SearchForString()
// Desc: Optimized brute-force string searching. uses the memchr()/memcmp() method.
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
        if ( (BlockSearchLength =
               WorkingLength - SearchStringLength + 1) <= 0 ) {
            Done = 1;   /* too few characters in this block for there
                            to be any possible matches, so this is the
                            final block and we're done without finding a
                            match */
        } else {
            /* Search this block */

            // Michael Abrash's Blackbook
            // Optimized brute-force
            //if ( SearchForString(WorkingBlock, BlockSearchLength, SearchString, SearchStringLength) )
            
            // Robert Sedgewick's Algorithms in C
            // Boyer-Moore
            //if ( bmsearch((char*)WorkingBlock, BlockSearchLength, (char*)SearchString, SearchStringLength) )
            
            // Bob Stout's Code Snippets
            // Pratt-Boyer-Moore
            //if ( strsearch((char*)WorkingBlock, BlockSearchLength, (char*)SearchString, SearchStringLength) )
            // Boyer-Moore-Horspool
            if ( bmh_search((char*)WorkingBlock, BlockSearchLength, (char*)SearchString, SearchStringLength) )
            
            // Thierry Lecroq's Exact String Matching Algorithms
            //if ( SearchBF((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchAUT((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchBM((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchTBM((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchTUNEDBM((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchKMP((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchHORSPOOL((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchRC((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchNSN((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )		 
            //if ( SearchZT((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchBR((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchQS((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchSMITH((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( Algorithm::SearchGS((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
            //if ( SearchRAITA((char*)SearchString, SearchStringLength, (char*)WorkingBlock, BlockSearchLength) )
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
        printf("String found at position: %ld\n", (long)(result2 + result));
        //std::cout << "Elapsed time: " << p1.elapsed() << "s" << std::endl;
        std::cout << "Elapsed time: " << p1.elapsed() * 1000.0 << " ms" << std::endl;
        return 0; // Exit code 0: Success
    } else {
        printf("String not found\n");
        //MessageBox(NULL,"String not found","QQQ!",MB_OK);
        //return -1;
        return 1; // Exit code 1: Not found / logical failure
    }

    //exit(Found);   /* return the found/not found status as the
    //                  DOS errorlevel */
}

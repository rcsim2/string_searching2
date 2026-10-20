//-----------------------------------------------------------------------------
// File: fsif.cpp
//
// Desc: Find String In File. Taken and adapted from Bob Stout's Code Snippets.
//
// Note: We have taken this file from the Cloves program that was part of
//		 Blacksphere's infamous Szell program suite. It was used there to Find and Replace
//		 string sequences in batch files. We will only use it to Find strings.
// Note: uses brute-force technique using strncmp(). Slow.
//
//
// Copyright (c) 2000 Black Sphere Corp.
//------------------------------------------------------------------------------
//#include "precompiled.h"
//#include "resource.h"
//#include "xfile.h"

//#define STRICT
//#define D3D_OVERLOADS

#include "fsif.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>


/* FSIF.C - Find and Replace string in file
*
* Partial author: Rik Goossens
*
* Note: will not find the string if it is
*       absolutely final in the file (i.e. it needs
*       a trailing CR LF (\n))
* FIX: (2026-10-20) will now find the string even if it is final in the file.
*/

//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <io.h> // for filelength()

#define	 BUFFERSIZE  512
#define	 TRUE  1
#define	 FALSE 0

/* prototype declaration */
long find_str_in_file(FILE *fpFile, long lOffset, char *pszStr);


//int main (void)
//{
//	FILE *stream;
//	char *filename = "C:\\Autoexec.bak";				// first \ is escape character
//	long startoffset = 0;
//	char *string = "\x0D\x0A" "CLS"	"\x0D\x0A" "SZ";	// Can't (and needn't) search for final "\x0D\x0A"
//	long lStatusFlag;
//	
//	
//	/* Open	for write (will	fail if	file does not exist) */
//	if( (stream = fopen(filename, "r+b")) == NULL ) {
//		printf(	"The file %s was not opened\n",	filename );
//		return 0;
//	} else {
//		printf(	"The file %s was opened\n", filename );
//	}
//	
//	do {	
//		lStatusFlag = find_str_in_file(stream, startoffset, string);
//		switch(lStatusFlag) {
//		case -1:
//			printf(	"%s not	found\n", string );
//			break;
//		case -2:
//			printf(	"Not enough memory to perform search\n"	);
//			break;
//		default:
//			printf(	"%s found and replaced at position %d\n", string, lStatusFlag );
//			fseek(stream, lStatusFlag, 0);
//			// replace with 11 spaces
//			fputs("01234567891", stream);
//		}
//		startoffset = lStatusFlag + 1;
//	} while	(lStatusFlag >=	0); 
//	return 0;
//}


/* Search algorithm:
* 1. Accounts for the borderline cases where the search string spans buffers by allocating
*    the double BUFFERSIZE and then shifting half bufferblocks
* 2. Uses strncmp() to scan the file instead of memchr()/memcmp(). This is the brute-force
*	 technique: it is better to check for a potential match first with memchr().
*/

/*=======================================\
|     FIND_STR_IN_FILE			  |-----------------------------------|
|\=======================================/
|
| Searches a binary file for a string.
|
| Returns the start offset for the first occurrence of the string.
|
|
|
|
|-----------------------------------------------------------------------------|
| CALL:
|    find_str_in_file(stream, startoffset, string);
|
| HEADERS:
|    stdio.h
|    string.h
|    malloc.h
|    io.h
|
| GLOBALE VARIABLES:
|    %
|
| ARGUMENTS:
|    fpFile	 : Pointer to the (open) binary	file
|    lOffset : Startoffset for search
|    pszStr	 : String to search for
|
| PROTOTYPE:
|    long find_str_in_file(FILE	*fpFile, long lOffset,
|				    char *pszStr);
|
| RETURN VALUE:
|    long lStatusFlag	: -1   : String	not found
|			  -2   : Not enough memory to perform search
|			  else : Offset	for string in file
|
| MODULE:
|    fsif.c
|-----------------------------------------------------------------------------|
|1994-03-18/Bac
|   -  Enhanced	error detection. Returning error value
|1995-06-19/Bac
|   -  Patch for returning position. Changed from blockwise to bytewise
|      calculation.
|1995-09-20/Bac
|  -  Ajusted secondary	search in last block
|
|1995-10-27/Bac
|  -  Released to public domain
|
|-----------------------------------------------------------------------------|
|1992-08-01/Erik Bachmann
\===========================================================================|*/

long find_str_in_file(FILE *fpFile, long lOffset, char *pszStr)
{
      long  lStatusFlag = -1l,            /* Status: -1 = not found     */
            lPosInFile  = 0l,             /* Position in file           */
            lCurrentPos = 0l;

      char  cFound      = FALSE,          /* Flag: FALSE = Not found (yet) */
            *pszBuffer  = NULL;           /* Buffer for fileinput       */

      int   iDataSize   = 0,              /* Size of data read from file*/
            iSector     = 0,              /* No of blocks read          */
            offset      = 0,              /* local counter              */
            iBufferSize = BUFFERSIZE,
            iNoBlocks   = 0;              /* No of blocks remaining     */

      /*----------------------------------------------------------------*/

      //lPosInFile = filelength(fileno(fpFile));  /* Find filesize        */
      // Portable way to get file size using standard C
      fseek(fpFile, 0, SEEK_END);
      lPosInFile = ftell(fpFile);
      fseek(fpFile, lOffset, SEEK_SET); // Jump back to your desired offset

      iNoBlocks = (int)(lPosInFile / (int) iBufferSize) ;
                                    /* Calculate remaining no of blocks */

      fseek(fpFile, lOffset, SEEK_SET) ;  /* Go to start offset */

      lCurrentPos = lOffset ;

      iNoBlocks = (int)((lPosInFile - lOffset) / (int) iBufferSize);
                                    /* Calculate remaining no of blocks */

      if (0 == (pszBuffer = (char *) calloc(2 * iBufferSize, sizeof(char))))
            return -2;              /* Allocate buffer                  */

      memset(pszBuffer, '\0', iBufferSize);

      iDataSize = fread(pszBuffer, sizeof(char), iBufferSize, fpFile);
                                    /* Read the first block             */

      while ((0 < iNoBlocks) && !cFound)
      {                             /* Repeat until EOF or found        */
            iSector++;              /* Counting no of blocks read       */
            iNoBlocks--;

            memset(&pszBuffer[iBufferSize], '\0', iBufferSize);
            iDataSize = fread(&pszBuffer[iBufferSize], sizeof(char),
                              iBufferSize, fpFile); /* Read next block  */

            for (offset = 0; offset < iBufferSize; offset++)
                                    /* Search first block               */
            {
                  if (0 == strncmp(&pszBuffer[ offset ],
                                   pszStr, strlen(pszStr)))
                  {                 /* Is the string placed here?       */
                        cFound = TRUE;    /* Yes -> set flag            */
                        break;
                  }
                  else  lCurrentPos++;    /* No -> Try again            */
            }

            memcpy(pszBuffer, &pszBuffer[ iBufferSize ], iBufferSize);
                                    /* Shift block left                 */
			//MessageBeep(-1);
      }

      /* Search the last Sector read if tag not found yet               */

      if (!cFound)
      {
            iSector++;              /* Counting no of blocks read       */

            // FIX: there was a bug here: now also checks last position in file.
            //for (offset = 0; offset < iDataSize - (int)strlen(pszStr); offset++)
            for (offset = 0; offset <= iDataSize - (int)strlen(pszStr); offset++)
                  if (0 == strncmp(&pszBuffer[ offset ], pszStr,
                                   strlen(pszStr)))
                  {                 /* Is the string placed in last block?*/
                        cFound = TRUE;    /* Found -> set flag          */
                        break;
                  }
                  else if (lCurrentPos < lPosInFile)
                                    /* Check for End of File            */
                        lCurrentPos++;    /* In file -> goto next       */
                  else  break;            /* End of File = quit         */
      }

      free(pszBuffer);              /* Free the allocated memory again  */

      if (TRUE == cFound)                 /* IF tag is found            */
            return ((long) lCurrentPos);  /* Return bytewise positon    */
      else  return lStatusFlag;           /* Return errorcode           */
}

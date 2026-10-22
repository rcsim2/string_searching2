//-----------------------------------------------------------------------------
// File: algorithm.h
//
// Desc: Several string search algorithms
//
//
// Copyright (c) 2000 Black Sphere Corp.
//-----------------------------------------------------------------------------
#pragma once
#ifndef __ALGORITHM_H
#define __ALGORITHM_H


#include <climits>

#ifndef __max
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif



// Boyer-Moore (From: Sedgewick) ///////////////////////////////
//#pragma warning( disable : 4018 ) // signed/unsigned mismatch

int *bminit(char *string, size_t sz)
{
	static int skip[256];
	/* register */ int i;
	for (i=0;i<256;i++)
		skip[i]=sz;
	for (i=0;i<sz;i++)
		skip[string[i]]=sz-i-1;
	return skip;
}

// NOTE: This algo is flaky.
// It cannot find [Frame 868] at the absolute end of the file.
// Also:
// Fails to find:
// [Frame 868]
// 0.000000
// But does find:
// [Frame 868]
// 0.000000X
// It needs the extra character after the 0.000000 to find it.
// It is the working space it needs.
// HELL: The error was in the calling code:
// bmsearch((char*)WorkingBlock, BlockSearchLength, (char*)SearchString, SearchStringLength)
// Must be:
// bmsearch((char*)WorkingBlock, WorkingLength, (char*)SearchString, SearchStringLength)
int bmsearch(char *buffer, size_t M, char *sequence, size_t N)
{
	int i,j,t;
	int *skip;
	skip=bminit(sequence,N);
	for (i=N-1,j=N-1;j>0;i--,j--)
		while(sequence[j]!=buffer[i])
		{
			t=skip[buffer[i]];
			i+=__max(t,N-j);
			if (i>=M)
				//return M;
				return 0;  // no match
			j=N-1;
		} //return i; 
		// NEW: get position of search string in the buffer
		result = i;
		return 1; // match		
}

//#pragma warning( default : 4018 )
////////////////////////////////////////////////////////////////



// Pratt-Boyer-Moore (From: Bob Stout's Snippets) //////////////
// NOTE: cannot find [Frame 1]...
// FIX: now it can find [Frame 1] (2026-10-20)
// We were calling init_search() too late in strsearch(). It must
// be called early because it sets the length of the search
// string (len). The search function uses len to determine how many characters
// to compare. If len is not set correctly, the search will not work properly.
// NOTE: Pratt is a misnomer.
// NOTE: Original snippet: 
// https://github.com/jscottb/c_snippets/blob/master/pbmsrch.c
// https://www.stjarnhimlen.se/snippets/pbmsrch.c
//
///* +++Date last modified: 05-Jul-1997 */
//
///*
//**        A Pratt-Boyer-Moore string search, written by Jerry Coffin
//**  sometime or other in 1991.  Removed from original program, and
//**  (incorrectly) rewritten for separate, generic use in early 1992.
//**  Corrected with help from Thad Smith, late March and early
//**  April 1992...hopefully it's correct this time. Revised by Bob Stout.
//**
//**  This is hereby placed in the Public Domain by its author.
//**
//**  10/21/93 rdg  Fixed bug found by Jeff Dunlop
//*/
//
//#include <stddef.h>
//#include <string.h>
//#include <limits.h>

static size_t table[UCHAR_MAX + 1];
static size_t len;
static char *findme;

/*
**  Call this with the string to locate to initialize the table
*/

void init_search(const char *string)
{
    size_t i;

    len = strlen(string);
    for (i = 0; i <= UCHAR_MAX; i++)                      /* rdg 10/93 */
          table[i] = len;
    for (i = 0; i < len; i++)
          table[(unsigned char)string[i]] = len - i - 1;
    findme = (char *)string;
}

/*
**  Call this with a buffer to search
*/

int strsearch(const char *string, const int stringlen, 
				const char *pattern, const int patternlen)
{
    // NOTE: call at start
    init_search(pattern);

    /* register */ size_t shift;
    /* register */ size_t pos = len - 1;
    char *here;
    // ERROR: need correct limit for search string length
    // NONO: this was not the error. It was calling init_search() too late.
    size_t limit=strlen(string);
    //size_t limit=stringlen;

	// NEW: save initial Buffer begin
	pdest = (unsigned char*)string;	

	// NEW: Preprocessing
    // ERROR: too late, must be called at start because it sets len
	//init_search(pattern);

	// NEW:
	result = 0;


    while (pos < limit)
    {
        while( pos < limit &&
              (shift = table[(unsigned char)string[pos]]) > 0)
        {
              pos += shift;
        }
        if (0 == shift)
        {
            if (0 == strncmp(findme,
                here = (char *)&string[pos-len+1], len))
            {
                //return(here);
                // NEW: get position of search string in the buffer
                result = (unsigned char*)here - pdest;
                return 1; // match	
            }
            else  pos++;
        }
    }
    //return NULL;
	return 0;  // no match
}
////////////////////////////////////////////////////////////////



// Boyer-Moore-Horspool (From: Bob Stout's Snippets) ///////////
///* +++Date last modified: 05-Jul-1997 */
//
///*
//**  Case-sensitive Boyer-Moore-Horspool pattern match
//**
//**  public domain by Raymond Gardner 7/92
//**
//**  limitation: pattern length + string length must be less than 32767
//**
//**  10/21/93 rdg  Fixed bug found by Jeff Dunlop
//*/
//#include <limits.h>                                         /* rdg 10/93 */
//#include <stddef.h>
//#include <string.h>

typedef unsigned char uchar;

#define LARGE 32767

static int patlen;
static int skip[UCHAR_MAX+1];                               /* rdg 10/93 */
static int skip2;
static uchar *pat;

void bmh_init(const char *pattern)
{
    int i, lastpatchar;

    pat = (uchar *)pattern;
    patlen = strlen(pattern);
    for (i = 0; i <= UCHAR_MAX; ++i)                  /* rdg 10/93 */
        skip[i] = patlen;
    for (i = 0; i < patlen; ++i)
        skip[pat[i]] = patlen - i - 1;
    lastpatchar = pat[patlen - 1];
    skip[lastpatchar] = LARGE;
    skip2 = patlen;                 /* Horspool's fixed second shift */
    for (i = 0; i < patlen - 1; ++i)
    {
        if (pat[i] == lastpatchar)
            skip2 = patlen - i - 1;
    }
}

int bmh_search(const char *string, const int stringlen, 
				 const char *pattern, const int patternlen)
{
    int i, j;
    char *s;

	// NEW: save initial Buffer begin
	pdest = (unsigned char*)string;	

	// NEW: Preprocessing
	bmh_init(pattern);

	// NEW:
	result = 0;

    i = patlen - 1 - stringlen;
    if (i >= 0)
        //return NULL;
		return 0;  // no match
    string += stringlen;
    for ( ;; )
    {
        while ( (i += skip[((uchar *)string)[i]]) < 0 )
            ;                           /* mighty fast inner loop */
        if (i < (LARGE - stringlen))
            //return NULL;
		    return 0;  // no match
        i -= LARGE;
        j = patlen - 1;
        s = (char *)string + (i - j);
        while (--j >= 0 && s[j] == pat[j])
            ;
        if ( j < 0 ) {                                   /* rdg 10/93 */
            //return s;                                /* rdg 10/93 */
		    // NEW: get position of search string in the buffer
		    result = (unsigned char*)s - pdest;
		    return 1; // match	
        }
		if ( (i += skip2) >= 0 )                        /* rdg 10/93 */
            //return NULL;                              /* rdg 10/93 */
			return 0;  // no match
    }
}
////////////////////////////////////////////////////////////////






// Thierry Lecroq's Exact String Matching Algorithms //////////////////////////
//#include "implementation.h"

//const int ASIZE = UCHAR_MAX + 1; // 256; // alphabet size (one byte: 2^8)
const int XSIZE = 512; // maximum search pattern size

#define EOS '\0'



// Brute Force (From: Thierry Lecroq) //////////////////////////
int SearchBF(char *x, int m, char *y, int n) { 
    char *yb; 
    /* Searching */
    // ERROR:
    //for (yb = y; *y != EOS; ++y)
    for (yb = y; n > 0; ++y, --n)
        if (memcmp(x, y, m) == 0) {
            //OUTPUT(y - yb);
            // NEW: get position of search string in the buffer
            result = y - yb;
            return 1; // match
        }
    return 0; // no match
}
////////////////////////////////////////////////////////////////


// Deterministic Finite Automaton  (From: Thierry Lecroq) //////
// NOTE: needs graph.h and graph.cpp
void preAut(char *x, int m, Graph aut) {
    int i, state, target, oldTarget;

    for (state = getInitial(aut), i = 0; i < m; ++i) {
        oldTarget = getTarget(aut, state, x[i]);
        target = newVertex(aut);
        setTarget(aut, state, x[i], target);
        copyVertex(aut, target, oldTarget);
        state = target;
    }
    setTerminal(aut, state);
}


int SearchAUT(char *x, int m, char *y, int n) {
    int j, state;
    Graph aut;

    /* Preprocessing */
    aut = newAutomaton(m + 1, (m + 1)*ASIZE);
    preAut(x, m, aut);

    /* Searching */
    for (state = getInitial(aut), j = 0; j < n; ++j) {
        state = getTarget(aut, state, y[j]);
        if (isTerminal(aut, state)) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            // NOTE: Adjust j (end of match) back to the start of the match
            result = j - m + 1;
            return 1; // match
        }
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////


// Boyer-Moore (From: Thierry Lecroq) //////////////////////////
void preBmBc(char *x, int m, int bmBc[]) {
    int i;
    
    for (i = 0; i < ASIZE; ++i)
        bmBc[i] = m;
    for (i = 0; i < m - 1; ++i)
        bmBc[x[i]] = m - i - 1;
}
 
 
void suffixes(char *x, int m, int *suff) {
    int f, g, i;
    
    suff[m - 1] = m;
    g = m - 1;
    for (i = m - 2; i >= 0; --i) {
        if (i > g && suff[i + m - 1 - f] < i - g)
            suff[i] = suff[i + m - 1 - f];
        else {
            if (i < g)
                g = i;
            f = i;
            while (g >= 0 && x[g] == x[g + m - 1 - f])
                --g;
            suff[i] = f - g;
        }
    }
}
 
void preBmGs(char *x, int m, int bmGs[]) {   
    int i, j, suff[XSIZE];
    
    suffixes(x, m, suff);
    
    for (i = 0; i < m; ++i)
        bmGs[i] = m;
    j = 0;
    for (i = m - 1; i >= -1; --i)
        if (i == -1 || suff[i] == i + 1)
            for (; j < m - 1 - i; ++j)
                if (bmGs[j] == m)
                bmGs[j] = m - 1 - i;
    for (i = 0; i <= m - 2; ++i)
        bmGs[m - 1 - suff[i]] = m - 1 - i;
}
 
 
int SearchBM(char *x, int m, char *y, int n) {
    int i, j, bmGs[XSIZE], bmBc[ASIZE];
    
    /* Preprocessing */
    preBmGs(x, m, bmGs);
    preBmBc(x, m, bmBc);
    
    /* Searching */
    j = 0;
    while (j <= n - m) {
        for (i = m - 1; i >= 0 && x[i] == y[i + j]; --i);
        if (i < 0) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match

            j += bmGs[0];
        }
        else
            j += __max(bmGs[i], bmBc[y[i + j]] - m + 1 + i);
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Turbo Boyer-Moore (From: Thierry Lecroq) ////////////////////
int SearchTBM(char *x, int m, char *y, int n) {
    int bcShift, i, j, shift, u, v, turboShift,
        bmGs[XSIZE], bmBc[ASIZE];

    /* Preprocessing */
    preBmGs(x, m, bmGs);
    preBmBc(x, m, bmBc);

    /* Searching */
    j = u = 0;
    shift = m;
    while (j <= n - m) {
        i = m - 1;
        while (i >= 0 && x[i] == y[i + j]) {
            --i;
            if (u != 0 && i == m - 1 - shift)
                i -= u;
        }
        if (i < 0) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match

            shift = bmGs[0];
            u = m - shift;
        }
        else {
            v = m - 1 - i;
            turboShift = u - v;
            bcShift = bmBc[y[i + j]] - m + 1 + i;
            shift = __max(turboShift, bcShift);
            shift = __max(shift, bmGs[i]);
            if (shift == bmGs[i])
                u = __min(m - shift, v);
            else {
            if (turboShift < bcShift)
                shift = __max(shift, u + 1);
            u = 0;
            }
        }
        j += shift;
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Tuned Boyer-Moore (From: Thierry Lecroq) ////////////////////
int SearchTUNEDBM(char *x, int m, char *y, int n) {
    int j, k, shift, bmBc[ASIZE];
    
    /* Preprocessing */
    preBmBc(x, m, bmBc);
    shift = bmBc[x[m - 1]];
    bmBc[x[m - 1]] = 0;
    memset(y + n, x[m - 1], m);
    
    /* Searching */
    j = 0;
    while (j < n) {
        k = bmBc[y[j + m -1]];
        while (k !=  0) {
            j += k; k = bmBc[y[j + m -1]];
            j += k; k = bmBc[y[j + m -1]];
            j += k; k = bmBc[y[j + m -1]];
        }
        if (memcmp(x, y + j, m - 1) == 0 && j < n) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match
        }
        j += shift;                          /* shift */
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Knuth-Morris-Pratt (From: Thierry Lecroq) ///////////////////
void preKmp(char *x, int m, int kmpNext[]) {
    int i, j;

    i = 0;
    j = kmpNext[0] = -1;
    while (i < m) {
        while (j > -1 && x[i] != x[j])
            j = kmpNext[j];
        i++;
        j++;
        if (x[i] == x[j])
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
    }
}


int SearchKMP(char *x, int m, char *y, int n) {
    int i, j, kmpNext[XSIZE];

    /* Preprocessing */
    preKmp(x, m, kmpNext);

    /* Searching */
    i = j = 0;
    while (j < n) {
        while (i > -1 && x[i] != y[j])
            i = kmpNext[i];
        i++;
        j++;
        if (i >= m) {
            //OUTPUT(j - i);
            // NEW: get position of search string in the buffer
            result = j - i;
            return 1; // match

            i = kmpNext[i];
        }
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Horspool (From: Thierry Lecroq) /////////////////////////////
int SearchHORSPOOL(char *x, int m, char *y, int n) {
    int j, bmBc[ASIZE];
    char c;

    /* Preprocessing */
    preBmBc(x, m, bmBc);

    /* Searching */
    j = 0;
    while (j <= n - m) {
        c = y[j + m - 1];
        if (x[m - 1] == c && memcmp(x, y + j, m - 1) == 0) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match
        }
        j += bmBc[c];
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Reverse Colussi (From: Thierry Lecroq) //////////////////////
void preRc(char *x, int m, int h[],
            int rcBc[ASIZE][XSIZE], int rcGs[]) {
    int a, i, j, k, q, r, s,
        hmin[XSIZE], kmin[XSIZE], link[XSIZE],
        locc[ASIZE], rmin[XSIZE];
    
    /* Computation of link and locc */
    for (a = 0; a < ASIZE; ++a)
        locc[a] = -1;
    link[0] = -1;
    for (i = 0; i < m - 1; ++i) {
        link[i + 1] = locc[x[i]];
        locc[x[i]] = i;
    }
    
    /* Computation of rcBc */
    for (a = 0; a < ASIZE; ++a)
        for (s = 1; s <= m; ++s) {
            i = locc[a];
            j = link[m - s];
            while (i - j != s && j >= 0)
                if (i - j > s)
                i = link[i + 1];
                else
                j = link[j + 1];
            while (i - j > s)
                i = link[i + 1];
            rcBc[a][s] = m - i - 1;
        }
    
    /* Computation of hmin */
    k = 1;
    i = m - 1;
    while (k <= m) {
        while (i - k >= 0 && x[i - k] == x[i])
            --i;
        hmin[k] = i;
        q = k + 1;
        while (hmin[q - k] - (q - k) > i) {
            hmin[q] = hmin[q - k];
            ++q;
        }
        i += (q - k);
        k = q;
        if (i == m)
            i = m - 1;
    }
    
    /* Computation of kmin */
    memset(kmin, 0, m * sizeof(int));
    for (k = m; k > 0; --k)
        kmin[hmin[k]] = k;
    
    /* Computation of rmin */
    for (i = m - 1; i >= 0; --i) {
        if (hmin[i + 1] == i)
            r = i + 1;
        rmin[i] = r;
    }
    
    /* Computation of rcGs */
    i = 1;
    for (k = 1; k <= m; ++k)
        if (hmin[k] != m - 1 && kmin[hmin[k]] == k) {
            h[i] = hmin[k];
            rcGs[i++] = k;
        }
    i = m-1;
    for (j = m - 2; j >= 0; --j)
        if (kmin[j] == 0) {
            h[i] = j;
            rcGs[i--] = rmin[j];
        }
    rcGs[m] = rmin[0];
}
 
 
int SearchRC(char *x, int m, char *y, int n) {
    int i, j, s, rcBc[ASIZE][XSIZE], rcGs[XSIZE], h[XSIZE];
    
    /* Preprocessing */
    preRc(x, m, h, rcBc, rcGs);
    
    /* Searching */
    j = 0;
    s = m;
    while (j <= n - m) {
        while (j <= n - m && x[m - 1] != y[j + m - 1]) {
            s = rcBc[y[j + m - 1]][s];
            j += s;
        }
        for (i = 1; i < m && x[h[i]] == y[j + h[i]]; ++i);
        if (i >= m) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match
        }
        s = rcGs[i];
        j += s;
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Not So Naive (From: Thierry Lecroq) //////////////////////
int SearchNSN(char *x, int m, char *y, int n) {
    int j, k, ell;
    
    /* Preprocessing */
    if (x[0] == x[1]) {
        k = 2;
        ell = 1;
    }
    else {
        k = 1;
        ell = 2;
    }
    
    /* Searching */
    j = 0;
    while (j <= n - m)
        if (x[1] != y[j + 1])
            j += k;
        else {
            if (memcmp(x + 2, y + j + 2, m - 2) == 0 &&
                x[0] == y[j]) {
                //OUTPUT(j);
                // NEW: get position of search string in the buffer
                result = j;
                return 1; // match
            }
            j += ell;
        }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Zhu-Takaoka (From: Thierry Lecroq) //////////////////////////
void preZtBc(char *x, int m, int ztBc[ASIZE][ASIZE]) {
    int i, j;

    for (i = 0; i < ASIZE; ++i)
        for (j = 0; j < ASIZE; ++j)
            ztBc[i][j] = m;
    for (i = 0; i < ASIZE; ++i)
        ztBc[i][x[0]] = m - 1;
    for (i = 1; i < m - 1; ++i)
        ztBc[x[i - 1]][x[i]] = m - 1 - i;
}


int SearchZT(char *x, int m, char *y, int n) {
    int i, j, ztBc[ASIZE][ASIZE], bmGs[XSIZE];

    /* Preprocessing */
    preZtBc(x, m, ztBc);
    preBmGs(x, m, bmGs);

    /* Searching */
    j = 0;
    while (j <= n - m) {
        i = m - 1;
        while (i < m && x[i] == y[i + j])
            --i;
        if (i < 0) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match

            j += bmGs[0];
        }
        else
            j += __max(bmGs[i],
                    ztBc[y[j + m - 2]][y[j + m - 1]]);
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////


// Berry-Ravindran (From: Thierry Lecroq) /////////////////////
void preBrBc(char *x, int m, int brBc[ASIZE][ASIZE]) {
    int a, b, i;

    for (a = 0; a < ASIZE; ++a)
        for (b = 0; b < ASIZE; ++b)
            brBc[a][b] = m + 2;
    for (a = 0; a < ASIZE; ++a)
        brBc[a][x[0]] = m + 1;
    for (i = 0; i < m - 1; ++i)
        brBc[x[i]][x[i + 1]] = m - i;
    for (a = 0; a < ASIZE; ++a)
        brBc[x[m - 1]][a] = 1;
}


int SearchBR(char *x, int m, char *y, int n) {
    int j, brBc[ASIZE][ASIZE];

    /* Preprocessing */
    preBrBc(x, m, brBc);

    /* Searching */
    y[n + 1] = '\0';
    j = 0;
    while (j <= n - m) {
        if (memcmp(x, y + j, m) == 0) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match
        }
        j += brBc[y[j + m]][y[j + m + 1]];
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////


// Quick Search  (From: Thierry Lecroq) ////////////////////////
void preQsBc(char *x, int m, int qsBc[]) {
    int i;

    for (i = 0; i < ASIZE; ++i)
        qsBc[i] = m + 1;
    for (i = 0; i < m; ++i)
        qsBc[x[i]] = m - i;
}


int SearchQS(char *x, int m, char *y, int n) {
    int j, qsBc[ASIZE];

    /* Preprocessing */
    preQsBc(x, m, qsBc);
    
    /* Searching */
    j = 0;
    while (j <= n - m) {
        if (memcmp(x, y + j, m) == 0) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match
        }
        j += qsBc[y[j + m]];               /* shift */
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Smith (From: Thierry Lecroq) ////////////////////////////////
int SearchSMITH(char *x, int m, char *y, int n) {
    int j, bmBc[ASIZE], qsBc[ASIZE];

    /* Preprocessing */
    preBmBc(x, m, bmBc);
    preQsBc(x, m, qsBc);

    /* Searching */
    j = 0;
    while (j<= n - m) {
        if (memcmp(x, y + j, m) == 0) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match
        }
        j += __max(bmBc[y[j + m - 1]], qsBc[y[j + m]]);
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////


// Raita (From: Thierry Lecroq) ////////////////////////////////
int SearchRAITA(char *x, int m, char *y, int n) {
    int j, bmBc[ASIZE];
    char c, firstCh, *secondCh, middleCh, lastCh;

    /* Preprocessing */
    preBmBc(x, m, bmBc);
    firstCh = x[0];
    secondCh = x + 1;
    middleCh = x[m/2];
    lastCh = x[m - 1];

    /* Searching */
    j = 0;
    while (j <= n - m) {
        c = y[j + m - 1];
        if (lastCh == c && middleCh == y[j + m/2] &&
            firstCh == y[j] &&
            memcmp(secondCh, y + j + 1, m - 2) == 0) {
            //OUTPUT(j);
            // NEW: get position of search string in the buffer
            result = j;
            return 1; // match
        }
        j += bmBc[c];
    }
    return 0; // no match
}
////////////////////////////////////////////////////////////////



// Galil-Seiferas (From: Thierry Lecroq) //////////////////////
// NOTE: cannot find [Frame 1]...
//namespace Algorithm
//{
//void newP1();
//
//char *x, *y;
//int k, m, n, p, p1, p2, q, q1, q2, s;
//
//void search() {
//   while (p <= n - m) {
//      while (p + s + q < n && x[s + q] == y[p + s + q])
//         ++q;
//      if (q == m - s && memcmp(x, y + p, s + 1) == 0) {
//         //OUTPUT(p);
//		 // NEW: get position of search string in the buffer
//		 result = p;
//	  }
//      if (q == p1 + q1) {
//         p += p1;
//         q -= p1;
//      }
//      else {
//         p += (q/k + 1);
//         q = 0;
//      }
//   }
//}
//
//
//void parse() {
//   while (1) {
//      while (x[s + q1] == x[s + p1 + q1])
//         ++q1;
//      while (p1 + q1 >= k*p1) {
//         s += p1;
//         q1 -= p1;
//      }
//      p1 += (q1/k + 1);
//      q1 = 0;
//      if (p1 >= p2)
//         break;
//   }
//   newP1();
//}
// 
//
//void newP2() {
//   while (x[s + q2] == x[s + p2 + q2] && p2 + q2 < k*p2)
//      ++q2;
//   if (p2 + q2 == k*p2)
//      parse();
//   else
//      if (s + p2 + q2 == m)
//         search();
//      else {
//         if (q2 == p1 + q1) {
//            p2 += p1;
//            q2 -= p1;
//         }
//         else {
//            p2 += (q2/k + 1);
//            q2 = 0;
//         }
//         newP2();
//      }
//}
// 
//
//void newP1() {
//   while (x[s + q1] == x[s + p1 + q1])
//      ++q1;
//   if (p1 + q1 >= k*p1) {
//      p2 = q1;
//      q2 = 0;
//      newP2();
//   }
//   else {
//      if (s + p1 + q1 == m)
//         search();
//      else {
//         p1 += (q1/k + 1);
//         q1 = 0;
//         newP1();
//      }
//   }
//}
//
//
//int SearchGS(char *argX, int argM, char *argY, int argN) {
//   // NEW:
//   result = 0;
//
//   x = argX;
//   m = argM;
//   y = argY;
//   n = argN;
//   k = 4;
//   p = q = s = q1 = p2 = q2 = 0;
//   p1 = 1;
//   newP1();   
//   
//   // NEW:
//   if (result)
//	   return 1; // match
//   else
//	   return 0;  // no match
//}
//
//} // END: namespace Galil-Seiferas
////////////////////////////////////////////////////////////////




#endif // __ALGORITHM_H
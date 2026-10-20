//-----------------------------------------------------------------------------
// File: search.h
//
// Desc: search header file
//
//
// Copyright (c) 2000 Black Sphere Corp.
//-----------------------------------------------------------------------------
#pragma once
#ifndef __SEARCH_H
#define __SEARCH_H


int SearchForString(unsigned char *Buffer, int SearchLength,
   unsigned char *SearchString, int SearchStringLength);

//long SearchStringInFile(FILE *fpFile, char *pszStr);


#endif // __SEARCH_H
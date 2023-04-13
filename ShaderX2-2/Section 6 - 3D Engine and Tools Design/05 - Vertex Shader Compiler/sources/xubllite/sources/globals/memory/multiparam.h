///////////////////////////////////////////////////////////////////////////
//
// MultiParam
// Copyright GP
// Created 24.11.00
//
///////////////////////////////////////////////////////////////////////////

#ifndef __MULTIPARAM__
#define __MULTIPARAM__

#include <globals/globals.h>
#include <stdarg.h>

const int maxtextlen          = 512;


// retrieve multi param definition (const char *f,...)
// example:
/*
void test(int a, const char *f,...)
{
   RetrievePrintf(f);
   // result is in __buf
}
/**/
 

#define MRetrievePrintf( f ) \
   char __buf[ maxtextlen ]; \
   va_list args;\
   va_start( args , f );\
   vsprintf( __buf , f , args );\
   va_end( args )
 
#endif




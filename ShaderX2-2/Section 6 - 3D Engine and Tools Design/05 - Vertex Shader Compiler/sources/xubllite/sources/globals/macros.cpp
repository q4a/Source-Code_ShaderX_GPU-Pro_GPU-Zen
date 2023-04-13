/////////////////////////////////////////////////////////////////
//
// Global macros
//
// Copyright GP
// Created 3.6.2000
//
/////////////////////////////////////////////////////////////////
//$XHEADER
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>


#include "macros.h"
#include <stdio.h>
#include <stdlib.h>

XDLL void KernelAbort( const char *msg , const char *msgadd , const char *file, int line)
{
   abort();
}

XDLL void KernelWarning( const char *msg , const char *msgadd , const char *file, int line)
{
}

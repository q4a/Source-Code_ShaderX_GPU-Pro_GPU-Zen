///////////////////////////////////////////////////////////////////////////
//
// Kernel
// Created 13.05.2001
// Copyright by GP
//
//$DONTCONVERTME
//
///////////////////////////////////////////////////////////////////////////

#ifndef __CORE_KERNEL__
#define __CORE_KERNEL__

#include "coreglobals.h"

XCOREDLL int      ShowMessage(const char *message);
XCOREDLL int      ShowMessage(const char *windowname,const char *message);

//
XCOREDLL void     TerminateApplication();

XCOREDLL void     Pause(int milisec);

#endif


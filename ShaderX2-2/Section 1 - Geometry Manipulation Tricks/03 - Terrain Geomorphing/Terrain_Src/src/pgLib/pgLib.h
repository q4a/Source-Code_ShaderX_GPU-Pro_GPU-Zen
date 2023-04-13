///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       
// creator:     Daniel Wagner
// description: This file must be included by
//              every class that wants to export
//              from the DLL
//
///////////////////////////////////////////////////////////////////
// $Id: pgLib.h,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////

#include <windows.h>

#ifndef PGLIB_API
#ifdef PGLIB_EXPORTS
#define PGLIB_API __declspec(dllexport)
#else
#define PGLIB_API __declspec(dllimport)
#endif
#endif

#pragma warning ( disable: 4251 )


typedef float pgRad;
typedef float pgDegree;

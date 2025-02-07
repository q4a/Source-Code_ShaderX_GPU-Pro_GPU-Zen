/*********************************************************************NVMH4****
Path:  SDK\LIBS\inc\nv_core
File:  nv_core.h

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:



******************************************************************************/

#ifndef _nv_core_h_
#define _nv_core_h_

#ifdef WIN32
#pragma warning (disable:4786) 
#endif

#include <map>
#include <string>
#include <iostream>

#ifndef _nv_coredecl_h_
#include "nv_coredecl.h"
#endif // _nv_coredecl_h_

#ifdef WIN32

#ifndef NV_CORE_PROJECT // defined if we are building nv_manip.lib

#ifndef NV_CORE_EXPORTS

#if _MSC_VER >= 1300

#ifdef _DLL
	///////////////////////////////////////////////*
    #pragma message("  nv_core_mtdll_vc7.lib       (MULTITHREADED DLL)\n")
    #pragma comment(lib,"nv_core_mtdll_vc7.lib")
#else
    #ifdef _MT
		///////////////////////////////////////////////*
	    #pragma message("  nv_core_mt_vc7.lib          (MULTITHREADED)\n")
        #pragma comment(lib,"nv_core_mt_vc7.lib")
    #else
		///////////////////////////////////////////////*
	    #pragma message("  nv_core_vc7.lib             (SINGLE THREADED)\n")
        #pragma comment(lib,"nv_core_vc7.lib")
    #endif
#endif
#else
#ifdef _DLL
	///////////////////////////////////////////////*
    #pragma message("  nv_core_mtdll.lib           (MULTITHREADED DLL)\n")
    #pragma comment(lib,"nv_core_mtdll.lib")
#else
    #ifdef _MT
		///////////////////////////////////////////////*
	    #pragma message("  nv_core_mt.lib              (MULTITHREADED)\n")
        #pragma comment(lib,"nv_core_mt.lib")
    #else
		///////////////////////////////////////////////*
	    #pragma message("  nv_core.lib                 (SINGLE THREADED)\n")
        #pragma comment(lib,"nv_core.lib")
    #endif
#endif
#endif
#endif
#endif // NV_CORE_PROJECT
#endif // WIN32


#ifndef _nv_attribute_h_
#include "nv_attribute.h"
#endif // _nv_attribute_h_

#endif // _nv_core_h_

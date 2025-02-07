/*********************************************************************NVMH3****
Path:  E:\nvidia\devrel\NVSDK\Common\include\nv_core
File:  nv_coredecl.h

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

#ifndef _nv_coredecl_h_
#define _nv_coredecl_h_

#ifdef NV_CORE_DLL

    #ifdef NV_CORE_EXPORTS
        #define DECLSPEC_NV_CORE __declspec(dllexport)
    #else
        #define DECLSPEC_NV_CORE __declspec(dllimport)
    #endif

#else
    #define DECLSPEC_NV_CORE
#endif

#endif  // _nv_coredecl_h_

/*********************************************************************NVMH4****
Path:  SDK\LIBS\inc\NV_D3DCommon
File:  SAMPLE_NVD3DVers.h

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

This is an example of the "NVD3DVers.h" which your projects can use to compile
the NV_D3DCommon utility classes for either D3D8 or D3D9.

This scheme is used so that only one version of those shared files needs to be
maintained, rather than having to maintain entirely separate D3D8 and D3D9
versions.
  
Each project should define it's own version of "NVD3DVers.h" and make sure the
file is in the #include path, so that when NV_D3DCommon files are built, the
appropriate "NVD3DVers.h" is included and used.


******************************************************************************/

#ifndef __NV_D3DVERS_H__
#define __NV_D3DVERS_H__


// Define either one of these, but not both

#define NV_USE_D3D8

// #define NV_USE_D3D9


/////////////////////////////////////////////////
// NVD3DCommonSwitch.h inludes the D3D headers and
// defines common types for the appropriate version
// of D3D.

#include <NVD3DCommonSwitch.h>


#endif		// #ifndef __NV_D3DVERS_H__










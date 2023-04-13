/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\FogPolygonVolumes
File:  NVD3DVers.h

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

A file which sets D3D version for NV shared utility classes for D3D like 
SimpleObject and ShaderManager.  
This is done so only one version of those shared files needs to be maintained, 
rather than having to maintain separate D3D8 and D3D9 versions.
  
Each project has it's own version of this include file.  This file is 
included by those shared utility files, so each project can set its own
version of D3D.


******************************************************************************/

#ifndef __NV_D3DVERS_H__
#define __NV_D3DVERS_H__


// Define either one of these to select D3D version

//#define NV_USE_D3D8

#define NV_USE_D3D9


//////////////////////////////////////////////////////////////////////////
// Include the header which #includes the appropriate version of D3D and
//  #defines common types used to switch the common classes between D3D8 
//  and D3D9

#include <NV_D3DCommon/NVD3DCommonSwitch.h>




#endif		// #ifndef __NV_D3DVERS_H__










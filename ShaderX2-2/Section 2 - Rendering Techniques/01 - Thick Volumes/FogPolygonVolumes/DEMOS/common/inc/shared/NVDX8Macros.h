/*********************************************************************NVMH4****
Path:  SDK\DEMOS\common\inc\shared
File:  NVDX8Macros.h

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


#ifndef __H_NVDX8MACROS_H
#define __H_NVDX8MACROS_H


//@#include <d3d8.h>


#ifndef DEL_PSH
#define DEL_PSH( pDev, hShader ) \
	{ if( hShader != 0 ) { pDev->DeletePixelShader( hShader ); hShader = 0; } }
#endif

#ifndef DEL_VSH
#define DEL_VSH( pDev, hShader ) \
	{ if( hShader != 0 ) { pDev->DeleteVertexShader( hShader ); hShader = 0; } }
#endif


/*
//@@@@@ USE NV_Common.h for these
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) \
	if ((a)) {(a)->Release(); (a) = NULL;}
#endif // SAFE_RELEASE

#ifndef SAFE_DELETE
#define SAFE_DELETE(a) \
	if ((a)) {delete(a); a = NULL;}
#endif // SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) \
	if ((a)) {delete [] (a); a = NULL;}
#endif // SAFE_DELETE_ARRAY

#ifndef ASSERT_IF_FAILED
#define ASSERT_IF_FAILED( hres )	\
{									\
	if( FAILED(hres) )				\
	   assert( false );				\
}
#endif // ASSERT_IF_FAILED

*/


#ifndef CHECK_D3DAPI
#define CHECK_D3DAPI(a)		\
{							\
	HRESULT hr = (a);		\
	if (hr != S_OK)			\
		assert(!#a);		\
}
#endif // CHECK_D3DAPI




#endif				// __H_NVDX8MACROS_H

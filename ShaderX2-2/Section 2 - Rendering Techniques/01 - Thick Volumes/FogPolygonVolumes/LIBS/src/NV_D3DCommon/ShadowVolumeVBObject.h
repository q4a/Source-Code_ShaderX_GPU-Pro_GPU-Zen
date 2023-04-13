/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  ShadowVolumeVBObject.h

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



#ifndef __ShadowVolume_VB_Object_89_H__
#define __ShadowVolume_VB_Object_89_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <NVD3DVers.h>
#include <NV_D3DCommon/SimpleVBObject.h>


class ShadowVolumeObject;


class ShadowVolumeVBObject : public SimpleVBObject
{
private:
	UINT	m_wLastTriIndex;				// last tri of all geometry
											//   including zero-area stitches tris
	UINT	m_wOriginalModelTriIndices;		// last tri of original model data
											//   No zero-area tris
public:

	HRESULT DrawIndexRange( UINT start, UINT end );
	HRESULT DrawBaseModel();
	HRESULT DrawZeroAreaTris();
	HRESULT DrawAllTris();

			// You must call ShadowVolumeObject8::BuildShadowVolume()
			//  before calling CreateFromShadowVolumeObject
	HRESULT	CreateFromShadowVolumeObject( ShadowVolumeObject * pObj, NV_D3DDEVICETYPE* pDev );

	HRESULT CreateFromSimpleObject( SimpleObject * pObj, NV_D3DDEVICETYPE*  pD3DDev ) {assert(false); return(E_FAIL); };


	ShadowVolumeVBObject();
	~ShadowVolumeVBObject();
};



#endif


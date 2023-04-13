/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  ShadowVolumeVBObject.cpp

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


#include <assert.h>

#include <shared/NV_Error.h>
#include <NV_D3DCommon/ShadowVolumeObject.h>
#include <NV_D3DCommon/ShadowVolumeVBObject.h>

#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p) { if(p != NULL) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef ASSERT_IF_FAILED
	#define ASSERT_IF_FAILED( hres )	\
	{									\
		if( FAILED(hres) )				\
		   assert( false );				\
	}
#endif


//////////////////////////////////////
// Construction/destruction

ShadowVolumeVBObject::ShadowVolumeVBObject()
:SimpleVBObject()
{
	m_wLastTriIndex = 0;
	m_wOriginalModelTriIndices = 0;
}


ShadowVolumeVBObject::~ShadowVolumeVBObject()
{
	// Base class destructor called automaticaly =)
}


//////////////////////////////////////



HRESULT ShadowVolumeVBObject::CreateFromShadowVolumeObject( ShadowVolumeObject* pObj,
														    NV_D3DDEVICETYPE* pDev )
{
	assert( pObj != NULL );
	assert( pDev != NULL );
	assert( pObj->IsValid() );

	HRESULT hr = S_OK;

	hr = SimpleVBObject::CreateFromSimpleObject( pObj, pDev );

	UINT	tot_tri, basetri;

	pObj->GetTriCounts( & tot_tri, & basetri );

	m_wLastTriIndex = tot_tri;
	m_wOriginalModelTriIndices = basetri;

	m_bIsValid = true;

	return( hr );
}



HRESULT ShadowVolumeVBObject::DrawIndexRange( UINT start, UINT end )
{
	// values should be the start and stop position in the 
	//  index buffer.
	// Prim count = ( end - start ) / 3 for TRIANGLELIST

	assert( m_pD3DDev != NULL );
	assert( m_bIsValid == true );
	assert( end >= start );

	HRESULT hr = S_OK;

	if( end > start )
	{
		switch( m_PrimType )
		{
		case D3DPT_TRIANGLELIST:

#ifdef NV_USING_D3D8
			hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof( SimpleObjVertex ) );
			ASSERT_IF_FAILED(hr);

			hr = m_pD3DDev->SetIndices( m_pIndexBuffer, 0 );
			ASSERT_IF_FAILED(hr);

			hr = m_pD3DDev->DrawIndexedPrimitive( m_PrimType, 0, m_wNumVerts, start, (end-start)/3 );
			ASSERT_IF_FAILED(hr);
#endif

#ifdef NV_USING_D3D9

			m_pD3DDev->SetVertexDeclaration( m_pVertexDeclaration );

			m_pD3DDev->SetStreamSource( 0, m_pVertexBuffer, 0, m_dwVertexSizeInBytes );

			m_pD3DDev->SetIndices( m_pIndexBuffer );

			m_pD3DDev->DrawIndexedPrimitive( m_PrimType,
												0,				// base vertex index
												0,				// min index - relative to base vertex index
												m_wNumVerts,	// num vertices used during call
												start,			// location in index array to start reading vertx
												(end-start)/3 );
#endif

			break;

		default:
			assert( false );
			hr = E_FAIL;

		}
	}

	return( hr );
}


HRESULT ShadowVolumeVBObject::DrawBaseModel()
{
	// Draw tris up to m_wOriginalModelTriIndices;
	HRESULT hr;
	hr = DrawIndexRange( 0, m_wOriginalModelTriIndices );
	return( hr );
}

HRESULT ShadowVolumeVBObject::DrawZeroAreaTris()
{
	// Draw only tris after the base model

	HRESULT hr = S_OK;

	hr = DrawIndexRange( m_wOriginalModelTriIndices, m_wLastTriIndex );

	return( hr );
}

HRESULT ShadowVolumeVBObject::DrawAllTris()
{
	return( Draw() );	
}
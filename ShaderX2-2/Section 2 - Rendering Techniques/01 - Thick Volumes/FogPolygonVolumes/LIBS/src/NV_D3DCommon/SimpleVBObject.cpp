/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  SimpleVBObject.cpp

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


#include <NV_D3DCommon/SimpleObject.h>
#include <NV_D3DCommon/SimpleVBObject.h>

#include <shared/NV_Error.h>
#include <shared/NV_Common.h>

#include <math.h>



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif




//////////////////////////////////////
// Construction/destruction

SimpleVBObject::SimpleVBObject()
{
	m_dwIndexSizeInBytes = 4;		// 32-bit indices

	m_pVertexBuffer = NULL;
	m_wNumVerts = 0;

	m_pIndexBuffer = NULL;
	m_wNumInd = 0;

	m_PrimType = D3DPT_TRIANGLESTRIP;
	m_bIsValid = false;


#ifdef NV_USING_D3D9
	m_pVertexDeclaration = NULL;
#endif

	m_pD3DDev = NULL;
}


SimpleVBObject::~SimpleVBObject()
{
	Free();
}


//////////////////////////////////////

HRESULT SimpleVBObject::Free()
{

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer );

#ifdef NV_USING_D3D9
	SAFE_RELEASE(m_pVertexDeclaration);
#endif

	if( m_pD3DDev != NULL )
	{
		SAFE_RELEASE( m_pD3DDev );
	}

	m_wNumVerts = 0;
	m_wNumInd = 0;

	m_PrimType = D3DPT_TRIANGLESTRIP;
	m_bIsValid = false;


	return( S_OK );
}


bool	SimpleVBObject::IsValid()
{
	return( m_bIsValid );
}


HRESULT SimpleVBObject::CreateVertexBuffer( UINT Length,
											DWORD Usage,
											DWORD FVF,
											D3DPOOL Pool,
											NV_VERTEXBUFFERTYPE** ppVertexBuffer,
											HANDLE* pHandle )
{
	HRESULT hr = S_OK;

	FAIL_IF_NULL( ppVertexBuffer );
	if( *ppVertexBuffer != NULL )
		SAFE_RELEASE( *ppVertexBuffer );


#ifdef NV_USING_D3D8

	hr = m_pD3DDev->CreateVertexBuffer( Length, Usage, FVF, Pool, ppVertexBuffer );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif
#ifdef NV_USING_D3D9

	hr = m_pD3DDev->CreateVertexBuffer( Length, Usage, NULL, Pool, ppVertexBuffer, NULL );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif

	return( hr );
}

HRESULT SimpleVBObject::CreateIndexBuffer(	UINT Length,
											DWORD Usage,
											D3DFORMAT Format,
											D3DPOOL Pool,
											NV_INDEXBUFFERTYPE** ppIndexBuffer,
											HANDLE* pHandle )
{
	HRESULT hr = S_OK;

	FAIL_IF_NULL( ppIndexBuffer );
	if( *ppIndexBuffer != NULL )
		SAFE_RELEASE( *ppIndexBuffer );

#ifdef NV_USING_D3D8

	hr = m_pD3DDev->CreateIndexBuffer( Length, Usage, Format, Pool, ppIndexBuffer );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif
#ifdef NV_USING_D3D9

	hr = m_pD3DDev->CreateIndexBuffer( Length, Usage, Format, Pool, ppIndexBuffer, NULL );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif

	return( hr );
}



HRESULT SimpleVBObject::UpdateIndicesFromSimpleObject( SimpleObject * pObj )
{
	HRESULT hr = S_OK;

	UINT			* pIndices;	
	int nind  = pObj->GetNumIndices();

	if( m_wNumInd < nind )
	{
		hr = E_FAIL;
		assert( false );
		return( hr );
	}

	DWORD lock_flags;
	if( m_bDynamic )
	{
		lock_flags = D3DLOCK_DISCARD;
	}
	else
	{
		lock_flags = 0;
	}

	hr = m_pIndexBuffer->Lock( 0, 0, (NV_VBDATAHANDLE) &pIndices, lock_flags );
	ASSERT_AND_RET_IF_FAILED(hr);


	memcpy( pIndices, pObj->m_pIndices, nind * m_dwIndexSizeInBytes );

//	int i;
//	for( i=0; i < nind; i++ )
//	{
//		pIndices[i] = pObj->m_pIndices[i];
//	}

	m_pIndexBuffer->Unlock();
	pIndices = 0;

	m_wNumInd   = nind;
	m_bIsValid  = true;	

	return( hr );
}


HRESULT SimpleVBObject::UpdateVerticesFromSimpleObject( SimpleObject * pObj )
{
	HRESULT hr = S_OK;

	SimpleObjVertex	* pVertices;
	
	int nvert = pObj->GetNumVerts();

	if( m_wNumVerts < nvert )
	{
		hr = E_FAIL;
		assert( false );
		return( hr );
	}


	DWORD lock_flags;
	if( m_bDynamic )
	{
		lock_flags = D3DLOCK_DISCARD;
	}
	else
	{
		lock_flags = 0;
	}


	hr = m_pVertexBuffer->Lock( 0, 0,			// offset, size
								(NV_VBDATAHANDLE) &pVertices,
								lock_flags );
	ASSERT_AND_RET_IF_FAILED(hr);


	memcpy( pVertices, pObj->m_pVertices, pObj->m_wNumVerts * sizeof( SimpleObjVertex ));

//	int i;
//	for( i=0; i < nvert; i++ )
//	{
//		pVertices[i].pos		= pObj->m_pVertices[i].pos;
//		pVertices[i].nrm		= pObj->m_pVertices[i].nrm;
//		pVertices[i].diffuse	= pObj->m_pVertices[i].diffuse;
//		pVertices[i].t0			= pObj->m_pVertices[i].t0;
//	}


	m_pVertexBuffer->Unlock();
	pVertices = 0;

	m_wNumVerts = nvert;
	m_bIsValid  = true;

	return( hr );
}


HRESULT SimpleVBObject::UpdateFromSimpleObject( SimpleObject * pObj )
{
	HRESULT hr = S_OK;

	UpdateVerticesFromSimpleObject( pObj );
	UpdateIndicesFromSimpleObject( pObj );

	return( hr );
}



HRESULT SimpleVBObject::CreateFromSimpleObject( SimpleObject * pObj,
												NV_D3DDEVICETYPE *  pD3DDev,
												VBUsage dynamic_or_static )
{

	Free();			// de-allocate if we're already created.


	assert( pD3DDev != NULL );
	// get the device
	m_pD3DDev = pD3DDev;
	pD3DDev->AddRef();			// released on Free()


	m_PrimType = pObj->m_PrimType;


	HRESULT hr;

	UINT nvert = pObj->GetNumVerts();
	UINT nind  = pObj->GetNumIndices();

	m_dwVertexSizeInBytes = sizeof( SimpleObjVertex );

	DWORD usage = D3DUSAGE_WRITEONLY;
	if( dynamic_or_static == SimpleVBObject::DYNAMIC )
	{
		usage = usage | D3DUSAGE_DYNAMIC;
		m_bDynamic = true;
	}
	else
	{
		m_bDynamic = false;
	}


	CreateVertexBuffer( nvert * m_dwVertexSizeInBytes,
						usage,
						SOBJVERT_FVF,			// func uses NULL for D3D9
						D3DPOOL_DEFAULT,
						&m_pVertexBuffer,
						NULL );

#ifdef NV_USING_D3D9

	// create declaration
	SimpleObjectVertexDecl	decl;

	hr = m_pD3DDev->CreateVertexDeclaration( decl.GetVShaderDeclaration(), & m_pVertexDeclaration );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif


	CreateIndexBuffer( nind * sizeof( UINT ),
						usage,
//						D3DFMT_INDEX16,
						D3DFMT_INDEX32,
						D3DPOOL_DEFAULT,
						&m_pIndexBuffer,
						NULL );

	m_wNumInd		= nind;
	m_wNumVerts		= nvert;

	UpdateFromSimpleObject( pObj );

	return( S_OK );
}




HRESULT SimpleVBObject::Draw()
{
	assert( m_pD3DDev != NULL );
	assert( m_bIsValid == true );

	HRESULT hr = S_OK;

	switch( m_PrimType )
	{
	case D3DPT_TRIANGLELIST:

		hr = Draw( 0, m_wNumInd / 3 );
		ASSERT_IF_FAILED(hr);

		break;

	case D3DPT_TRIANGLESTRIP:

		hr = Draw( 0, m_wNumInd - 2 );
		ASSERT_IF_FAILED(hr);

		break;

	case D3DPT_LINELIST :

		hr = Draw( 0, m_wNumInd / 2 );
		ASSERT_IF_FAILED(hr);

		break;

	default:
		assert( false );
		hr = E_FAIL;

	}


	return( hr );
}



HRESULT SimpleVBObject::Draw( UINT start_index, UINT primitive_count )
{
	HRESULT hr = S_OK;
	FAIL_IF_NULL( m_pD3DDev );
	if( m_bIsValid != true )
	{
		assert( false );
		return( E_FAIL );
	}
	FAIL_IF_NULL( m_pVertexBuffer );
	FAIL_IF_NULL( m_pIndexBuffer );

	////////////////////////////////


	switch( m_PrimType )
	{
	case D3DPT_TRIANGLELIST :
	case D3DPT_TRIANGLESTRIP :
	case D3DPT_LINELIST :

#ifdef NV_USING_D3D8

		hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, m_dwVertexSizeInBytes );
		ASSERT_IF_FAILED(hr);

		hr = m_pD3DDev->SetStreamSource(1, NULL, NULL );
		ASSERT_IF_FAILED(hr);
		hr = m_pD3DDev->SetStreamSource(2, NULL, NULL );
		hr = m_pD3DDev->SetStreamSource(3, NULL, NULL );

		hr = m_pD3DDev->SetIndices( m_pIndexBuffer, 0 );
		ASSERT_IF_FAILED(hr);

		hr = m_pD3DDev->DrawIndexedPrimitive( m_PrimType,
											  0, m_wNumVerts,
											  start_index, primitive_count );
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
                                            start_index,	// location in index array to start reading vertx
											primitive_count );

#endif

		break;

	case D3DPT_POINTLIST :
		FMsg("D3DPT_POINTLIST is not supported!\n");
		assert( false );
		hr = E_FAIL;
		break;

	default :
		FMsg("Unknown primitive type!\n");
		assert( false );
		hr = E_FAIL;

	}


	return( hr );
}
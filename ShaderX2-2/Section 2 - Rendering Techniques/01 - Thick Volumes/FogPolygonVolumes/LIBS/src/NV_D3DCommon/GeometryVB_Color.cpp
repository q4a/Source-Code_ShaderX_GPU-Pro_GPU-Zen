/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  GeometryVB_Color.cpp

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




#include <shared/NV_Common.h>
#include <shared/NV_Error.h>

#include <NVD3DVers.h>
#include <NV_D3DCommon/SimpleObject.h>
#include <NV_D3DCommon/SimpleVBObject.h>

#include <NV_D3DCommon/GeometryVB_Color.h>


GeometryVB_Color::GeometryVB_Color()
	: SimpleVBObject()
{

	m_bIsValid = false;
	m_pD3DDev = NULL;
	m_PrimType = D3DPT_TRIANGLESTRIP;

	m_dwVertexSizeInBytes = sizeof( VertexColor );

}


GeometryVB_Color::~GeometryVB_Color()
{
	HRESULT hr;
	hr = Free();
	ASSERT_IF_FAILED(hr);
}



HRESULT GeometryVB_Color::CreateFromSimpleObject( SimpleObject * pObj,
													NV_D3DDEVICETYPE*  pD3DDev )
{
	FAIL_IF_NULL( pObj );
	FAIL_IF_NULL( pD3DDev );

	HRESULT hr = S_OK;


	Free();			// de-allocate if we're already created.

	// get the device
	m_pD3DDev = pD3DDev;
	pD3DDev->AddRef();			// released on Free()

	m_PrimType = pObj->m_PrimType;


	int nvert = pObj->GetNumVerts();
	int nind  = pObj->GetNumIndices();


	VertexColor_Decl decl;
	DWORD fvf = decl.GetFVF();
	
	VertexColor		* pVertices;
	WORD			* pIndices;


#ifdef NV_USING_D3D9

	// create declaration
	hr = m_pD3DDev->CreateVertexDeclaration( decl.GetVShaderDeclaration(), 
											 & m_pVertexDeclaration );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif


	hr = CreateVertexBuffer( nvert * m_dwVertexSizeInBytes,
								D3DUSAGE_WRITEONLY,
								fvf,
								D3DPOOL_DEFAULT,
								&m_pVertexBuffer,
								NULL );
	ASSERT_AND_RET_IF_FAILED( hr );

	hr = CreateIndexBuffer( nind * sizeof( WORD ),
										D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,
										D3DPOOL_DEFAULT,
										&m_pIndexBuffer, 
										NULL	);
	ASSERT_AND_RET_IF_FAILED( hr );


	hr = m_pVertexBuffer->Lock( 0, 0, (NV_VBDATAHANDLE) &pVertices, 0 );
	if (FAILED(hr))
	{
		assert(false);
		return( hr );
	}


	hr = m_pIndexBuffer->Lock( 0, 0, (NV_VBDATAHANDLE) &pIndices, 0 );
	if (FAILED(hr))
	{
		assert(false);
		return( hr );
	}



	int i;

	for( i=0; i < nvert; i++ )
	{
		pVertices[i].Color		= pObj->m_pVertices[i].diffuse;
	}

	for( i=0; i < nind; i++ )
	{
		pIndices[i] = pObj->m_pIndices[i];
	}


	m_pVertexBuffer->Unlock();
	pVertices = 0;
	m_pIndexBuffer->Unlock();
	pIndices = 0;

	m_wNumVerts = nvert;
	m_wNumInd   = nind;
	m_bIsValid  = true;


	return( hr );
}




/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  GeometryVB_NormalsMarker.cpp

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

#include <NV_D3DCommon/SimpleObject.h>
#include <NV_D3DCommon/GeometryVB_NormalsMarker.h>



///////////////////////////////////////////////


NormalsMarker::NormalsMarker()
: GeometryVB_PC()
{


}


NormalsMarker::~NormalsMarker()
{
	Free();
}



HRESULT NormalsMarker::CreateFromSimpleObject( SimpleObject * pObj,
											   NV_D3DDEVICETYPE*  pD3DDev )
{
	HRESULT hr = S_OK;
	assert( pObj != NULL );
	assert( pD3DDev != NULL );

	m_pD3DDev = pD3DDev;
	pD3DDev->AddRef();			// released on Free()


	m_PrimType = D3DPT_LINELIST;

	int nvert = pObj->GetNumVerts() * 2;
	int nind  = nvert;


	VertexPC_Decl decl;
	DWORD fvf = decl.GetFVF();


#ifdef NV_USING_D3D8
	hr = m_pD3DDev->CreateVertexBuffer( nvert * m_dwVertexSizeInBytes,
										D3DUSAGE_WRITEONLY,
										fvf,
										D3DPOOL_DEFAULT,
										&m_pVertexBuffer );
	ASSERT_AND_RET_IF_FAILED( hr );

#endif
#ifdef NV_USING_D3D9

	// create declaration

	hr = m_pD3DDev->CreateVertexDeclaration( decl.GetVShaderDeclaration(), 
											 & m_pVertexDeclaration );
	ASSERT_AND_RET_IF_FAILED(hr);

	hr = m_pD3DDev->CreateVertexBuffer( nvert * m_dwVertexSizeInBytes,
										D3DUSAGE_WRITEONLY,
										fvf,
										D3DPOOL_DEFAULT,
										&m_pVertexBuffer,
										NULL );
	ASSERT_AND_RET_IF_FAILED( hr );
#endif


#ifdef NV_USING_D3D8

	hr = m_pD3DDev->CreateIndexBuffer( nind * sizeof( WORD ),
										D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,
										D3DPOOL_DEFAULT,
										&m_pIndexBuffer );
	ASSERT_AND_RET_IF_FAILED( hr );

#endif
#ifdef NV_USING_D3D9

	hr = m_pD3DDev->CreateIndexBuffer( nind * sizeof( WORD ),
										D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,
										D3DPOOL_DEFAULT,
										&m_pIndexBuffer,
										NULL	);
	ASSERT_AND_RET_IF_FAILED( hr );

#endif


	//////////////////////////////////////

	VertexPC		* pVertices;
	WORD			* pIndices;
	

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
	int nv;

	nv = 0;

	for( i=0; i < pObj->GetNumVerts(); i++ )
	{

		assert( nv < nvert );
		pVertices[nv].Position	= pObj->m_pVertices[i].pos;
		pVertices[nv].Color		= 0x000000FF;

		nv++;
		assert( nv < nvert );
		//@@@ dumb constant scale
		pVertices[nv].Position	= pObj->m_pVertices[i].pos + pObj->m_pVertices[i].nrm * 0.3f;
		pVertices[nv].Color		= 0x0000FF00;

		nv++;
	}

	for( i=0; i < nind; i++ )
	{
		pIndices[i] = i;
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






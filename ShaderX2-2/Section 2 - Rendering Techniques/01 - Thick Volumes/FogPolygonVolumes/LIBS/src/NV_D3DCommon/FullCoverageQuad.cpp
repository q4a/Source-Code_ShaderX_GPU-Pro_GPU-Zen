/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  FullCoverageQuad.cpp

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


#include <NV_D3DCommon/FullCoverageQuad.h>
#include <shared/NV_Common.h>
#include <shared/NV_Error.h>


/////////////////////////////////////////////////////////////


FullCoverageQuad::FullCoverageQuad()
{
	m_pD3DDev			= NULL;
	m_pVertexBuffer		= NULL;

#ifdef NV_USING_D3D9
	m_pVertexDeclaration = NULL;
#endif

}


FullCoverageQuad::~FullCoverageQuad()
{
	Free();
}

/////////////////////////////////////////////////////////////


HRESULT FullCoverageQuad::Free()
{	
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pD3DDev );

#ifdef NV_USING_D3D9
	SAFE_RELEASE( m_pVertexDeclaration );
#endif

	return( S_OK );
}


HRESULT FullCoverageQuad::Initialize( NV_D3DDEVICETYPE* pD3DDev )
{
	HRESULT hr;
	int i;

	// get the device
	assert( pD3DDev != NULL );

	m_pD3DDev = pD3DDev;
	m_pD3DDev->AddRef();			// released on Free()

	////////////////////////////////
	// create vertex buffer for the quad

#ifdef NV_USING_D3D8

	hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(QuadVertex),
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
										0, D3DPOOL_DEFAULT,
										&m_pVertexBuffer );
	ASSERT_AND_RET_IF_FAILED(hr);
#endif	

#ifdef NV_USING_D3D9

	// Create declaraction for QuadVertex :  vec3 pos, vec2 tex

    D3DVERTEXELEMENT9 decl[] =
    {
        { 0, 0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
		{ 0, 12,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
        D3DDECL_END()
    };

	hr = m_pD3DDev->CreateVertexDeclaration( decl,
											 & m_pVertexDeclaration );
	ASSERT_AND_RET_IF_FAILED(hr);


	hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(QuadVertex),
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
										0, D3DPOOL_DEFAULT,
										&m_pVertexBuffer,
										NULL );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif




	QuadVertex      *pBuff;

	if (m_pVertexBuffer)
	{
		hr = m_pVertexBuffer->Lock(	0, 4 * sizeof(QuadVertex),
									(NV_VBDATAHANDLE) &pBuff, 0);

		ASSERT_IF_FAILED(hr);

		if (FAILED(hr))
		{
			return hr;
		}


		float uv_base;
		float uv_max;

		uv_base = 0.0f;
		uv_max  = 1.0f; 

        for (i = 0; i < 4; ++i)
        {
            pBuff->Position = D3DXVECTOR3((i==0 || i==3) ? -1.0f : 1.0f,
                                          (i<2)          ? -1.0f : 1.0f,
                                          0.0f);
		    pBuff->Tex      = D3DXVECTOR2((i==0 || i==3) ? uv_base : uv_max , 
                                          (i<2)          ? uv_max : uv_base );
		    pBuff++;
        }
        m_pVertexBuffer->Unlock();
    }


	/////////////////////////////////////////////////
	// Set matrices so the quad created above will 
	//  exactly cover the render target

	D3DXMATRIX matView, matProj, matViewProj, matWorld;

	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	// Set World, View, Projection, and combination matrices.
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixOrthoLH(&matProj, 4.0f, 4.0f, 0.2f, 20.0f);

    D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

    // draw a single quad to texture:
    // the quad covers the whole "screen" exactly

	D3DXMatrixScaling(  &matWorld,		   2.0f, 2.0f, 1.0f);
	D3DXMatrixMultiply( &m_mWorldViewProj, &matWorld, &matViewProj);

	//////////////////////////////////////////////////
	// Use identity matrix for world & view matrices
	//  with fixed function vertex processing

	D3DXMatrixIdentity( & m_mIdentity );


	return( hr );
}


void  FullCoverageQuad::GetWorldViewProj( D3DXMATRIX * pOut )
{
	*pOut = m_mWorldViewProj;
}


void  FullCoverageQuad::SetViewMatrices()
{
	assert( m_pD3DDev != NULL );

	m_pD3DDev->SetTransform(D3DTS_PROJECTION,	&m_mWorldViewProj	);
	m_pD3DDev->SetTransform(D3DTS_VIEW,			&m_mIdentity		);
	m_pD3DDev->SetTransform(D3DTS_WORLD,		&m_mIdentity		);

}



HRESULT FullCoverageQuad::SetDefaultRendering()
{
	HRESULT hr = S_OK;

	hr = m_pD3DDev->SetPixelShader( 0 );
	ASSERT_IF_FAILED( hr );

#ifdef NV_USING_D3D8
	hr = m_pD3DDev->SetVertexShader( D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) );
	ASSERT_AND_RET_IF_FAILED( hr );

#endif
#ifdef NV_USING_D3D9
	hr = m_pD3DDev->SetVertexShader( NULL );
	ASSERT_AND_RET_IF_FAILED( hr );

	hr = m_pD3DDev->SetVertexDeclaration( m_pVertexDeclaration );
	ASSERT_AND_RET_IF_FAILED( hr );
	
#endif

	SetViewMatrices();

	return( hr );
}


HRESULT FullCoverageQuad::Draw()
{
	HRESULT hr;
	FAIL_IF_NULL( m_pD3DDev );

	SetDefaultRendering();

	hr = DrawWithoutSetRenderState();

	return( hr );
}


HRESULT FullCoverageQuad::DrawWithoutSetRenderState()
{
	HRESULT hr;
	FAIL_IF_NULL( m_pD3DDev );

	
#ifdef NV_USING_D3D8

	hr = m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));
	ASSERT_IF_FAILED(hr);

	hr = m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
	ASSERT_IF_FAILED( hr );

#endif

#ifdef NV_USING_D3D9

    hr = m_pD3DDev->SetVertexDeclaration( m_pVertexDeclaration );

    hr = m_pD3DDev->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(QuadVertex) );

    hr = m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

#endif


	return( hr );
}




/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  FullCoverageQuad.h

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

Defines a class to conveniently create and render a single quad which exactly
covers the render target
Use for procedural texture rendering and stencil shadow rendering.

This uses the legacy DX7 pipeline and fixed-function vertex processing.

Draw() will set new WORLD, VIEW, and PROJECTION matrices, so you must restore
these on your own.

******************************************************************************/


#ifndef __FULLCOVERAGEQUAD_89_H__
#define __FULLCOVERAGEQUAD_89_H__



#include <NVD3DVers.h>

class SimpleVBObject;

/////////////////////////////////////

class FullCoverageQuad
{
private:
	typedef struct tagQuadVertex
    {
	    D3DXVECTOR3 Position;
        D3DXVECTOR2 Tex;
    } QuadVertex;


	NV_D3DDEVICETYPE*			m_pD3DDev;
	NV_VERTEXBUFFERTYPE*		m_pVertexBuffer;

	D3DXMATRIX					m_mWorldViewProj;
	D3DXMATRIX					m_mIdentity;

#ifdef NV_USING_D3D9
    IDirect3DVertexDeclaration9*	m_pVertexDeclaration;
#endif


	void	SetViewMatrices();

public:


	HRESULT Initialize( NV_D3DDEVICETYPE* pD3DDev );
	HRESULT Free();

	HRESULT Draw();
	HRESULT DrawWithoutSetRenderState();

	HRESULT SetDefaultRendering();
	void	GetWorldViewProj( D3DXMATRIX * pOut );


	FullCoverageQuad();
	~FullCoverageQuad();
};


#endif			// __FULLCOVERAGEQUAD_89_H__













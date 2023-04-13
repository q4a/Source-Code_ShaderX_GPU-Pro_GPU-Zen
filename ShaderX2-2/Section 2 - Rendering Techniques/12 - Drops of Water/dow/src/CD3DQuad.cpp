#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <commctrl.h>
#include <math.h>

#include "CD3DQuad.h"

DWORD CD3DQuad::FVF = D3DFVF_XYZ |
							  D3DFVF_NORMAL |
							  D3DFVF_TEX3 |
							  D3DFVF_TEXCOORDSIZE2(0) |
							  D3DFVF_TEXCOORDSIZE2(1) |
							  D3DFVF_TEXCOORDSIZE2(2);


CD3DQuad::CD3DQuad(LPDIRECT3DDEVICE9 p)
{
	m_pd3dDevice=p;
	CD3DQuad::VERTEX vertices[] =
    {
        { -1.0,-1.0, 0.0, 0.0,0.0,1.0,  0.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0 }, // pos(x,y,z) n(x,y,z) color(rgb) tex(u,v) t(x,y,z) b(x,y,z)
        {  1.0,-1.0, 0.0, 0.0,0.0,1.0,  1.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0 },
        {  1.0, 1.0, 0.0, 0.0,0.0,1.0,  1.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0 },
		
        { -1.0,-1.0, 0.0, 0.0,0.0,1.0,  0.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0 },
        {  1.0, 1.0, 0.0, 0.0,0.0,1.0,  1.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0 },
        { -1.0, 1.0, 0.0, 0.0,0.0,1.0,  0.0,1.0, 1.0,0.0,0.0, 0.0,1.0,0.0 },
    };
	
    m_pd3dDevice->CreateVertexBuffer(6*sizeof(CD3DQuad::VERTEX),
									 0, CD3DQuad::FVF,
		                             D3DPOOL_DEFAULT, &m_QuadVB, NULL);
	
    VOID* pVertices;
    m_QuadVB->Lock( 0, 0, (void**)&pVertices, 0 );
    memcpy( pVertices, vertices, 6*sizeof(CD3DQuad::VERTEX) );
    m_QuadVB->Unlock();
}

CD3DQuad::~CD3DQuad()
{
	m_QuadVB->Release();
}

void CD3DQuad::Render()
{
	m_pd3dDevice->SetStreamSource( 0, m_QuadVB, 0, sizeof(CD3DQuad::VERTEX) );
	m_pd3dDevice->SetFVF( CD3DQuad::FVF );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );	
}

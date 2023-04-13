//-----------------------------------------------------------------------------
// File: main.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "resource.h"
#include "main.h"


#define MAP_SIZE      512
#define CATMAP_SIZE   32//128
#define PARTICLEMAP_WIDTH  16
#define PARTICLEMAP_HEIGHT 4

#define PASS_MOVE_BULLET		5
#define PASS_DISPLACEMENT_MESH	6
#define PASS_CREATE_TEXCOORD	7
#define PASS_CREATE_NORMAL		8
#define PASS_COMPUTE_ACC		9

typedef struct {
	FLOAT p[4];
	FLOAT tu, tv;
} TVERTEX;

typedef struct {
	FLOAT p[4];
	D3DCOLOR c;
} LVERTEX;

typedef struct {
	FLOAT p[4];
	D3DCOLOR color;
	FLOAT tu, tv;
} TLVERTEX;

typedef struct {
	FLOAT p[3];
	FLOAT tu, tv;
} BLUR_VERTEX;

typedef struct {
	FLOAT p[3];
	FLOAT n[3];
	FLOAT tu, tv;
	FLOAT du, dv;
} MESH_VERTEX;

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}


// ----------------------------------------------------------------------------
int CBullet::Create(D3DXVECTOR4 &x, D3DXVECTOR4 &v)
{
	if(BULLET_STATE_DEAD!=iState) return -1;
	iReq = BULLET_STATE_FLY;
	pos = x;
	pos.w=0.0f;
	velocity = v;
	velocity.w = 0;
	bSetPos = TRUE;

	return 0;
}
// ----------------------------------------------------------------------------
void CBullet::CreateMesh(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR *filename)
{
	pMesh->Create( pd3dDevice, filename );
	
	// Creation of the path volume
	float dt[2] = {(0.5f+(FLOAT)id)/PARTICLEMAP_WIDTH
				,   0.5f           /PARTICLEMAP_HEIGHT};
	pVolume->Create( pd3dDevice, pMesh->GetSysMemMesh(), dt);
}
// ----------------------------------------------------------------------------
void CBullet::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	iState=BULLET_STATE_DEAD;
	iReq = 0;
	if( pMesh->m_pSysMemMesh ){
		if( FAILED( pMesh->m_pSysMemMesh->CloneMesh(
						D3DXMESH_NPATCHES, decl,
						pd3dDevice, &pMesh->m_pLocalMesh ) ) )
			return;
		D3DXComputeNormals( pMesh->m_pLocalMesh, NULL );
		
		MESH_VERTEX* pVertices;
		pMesh->m_pLocalMesh->LockVertexBuffer( 0L, (LPVOID*)&pVertices );
		for(DWORD i=0;i<pMesh->m_pLocalMesh->GetNumVertices();i++){
			pVertices[i].du = (0.5f+(FLOAT)id)/PARTICLEMAP_WIDTH;
			pVertices[i].dv = 0.5f/PARTICLEMAP_HEIGHT;
		}
		pMesh->m_pLocalMesh->UnlockVertexBuffer();
	}
}

// ----------------------------------------------------------------------------
int CBullet::Update(FLOAT dt)
{
	int state = iState;
	switch(iState){
	case BULLET_STATE_DEAD:
		if(BULLET_STATE_FLY==iReq){
			iState = BULLET_STATE_FLY;
		}
		break;
	case BULLET_STATE_FLY:
		pos += velocity;
		pos.w=0.0f;
		// Dead check
		if(BULLET_STATE_BURN == iReq){
			iState = BULLET_STATE_BURN;
		}
		break;
	case BULLET_STATE_BURN:
	{
		if(BULLET_BURN_TIME<=fCnt){
			iState = BULLET_STATE_DEAD;
		}
	}
		break;
	}
	fCnt += dt;
	if(state != iState){iReq = 0;fCnt = 0;}
	return -1;
}
// ----------------------------------------------------------------------------
int CBullet::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	switch(iState){
	case BULLET_STATE_DEAD:
		return -1;
		break;
	case BULLET_STATE_FLY:
		pMesh->Render(pd3dDevice);
		break;
	case BULLET_STATE_BURN:
	{
		FLOAT t = fCnt/BULLET_BURN_TIME;

	    pd3dDevice->SetRenderState(  D3DRS_ALPHABLENDENABLE,   TRUE );
	    pd3dDevice->SetRenderState(  D3DRS_SRCBLEND,   D3DBLEND_INVSRCALPHA );
	    pd3dDevice->SetRenderState(  D3DRS_DESTBLEND,   D3DBLEND_SRCALPHA );
        for( DWORD i=0; i<pMesh->m_dwNumMaterials; i++ )
			pMesh->m_pMaterials[i].Diffuse.a = t;
		pMesh->Render(pd3dDevice);
	    pd3dDevice->SetRenderState(  D3DRS_ALPHABLENDENABLE,   FALSE );
	}
		break;
	}

	return 0;
}


//-----------------------------------------------------------------------------
// Name: UpdateCatMap()
// Desc: The pattern of fire is made with a technique of Arnold's cat map 
//-----------------------------------------------------------------------------
static VOID UpdateCatMap( LPDIRECT3DDEVICE9 pd3dDevice,
 LPDIRECT3DTEXTURE9 *ppTex, LPDIRECT3DSURFACE9 *ppSurf,
 DWORD id )
{
	pd3dDevice->SetRenderTarget(0, ppSurf[id]);
	pd3dDevice->SetDepthStencilSurface(NULL);

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,		D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	pd3dDevice->SetSamplerState (0,D3DSAMP_MAGFILTER ,	D3DTEXF_POINT );
	pd3dDevice->SetSamplerState (0,D3DSAMP_MINFILTER ,	D3DTEXF_POINT );

	float size = CATMAP_SIZE;
	TVERTEX Vertex[] = {
		//   x          y     z rhw  tu    tv
		{0.0f*size, 0.0f*size,0, 1, 0.0f, 0.0f,},
		{0.5f*size, 0.0f*size,0, 1, 1.0f, 0.5f,},
		{0.0f*size, 1.0f*size,0, 1, 1.0f, 1.0f,},

		{0.5f*size, 0.0f*size,0, 1, 0.0f, 0.5f,},
		{1.0f*size, 0.0f*size,0, 1, 1.0f, 1.0f,},
		{0.0f*size, 1.0f*size,0, 1, 0.0f, 1.0f,},

		{0.0f*size, 1.0f*size,0, 1, 0.0f, 0.0f,},
		{1.0f*size, 0.0f*size,0, 1, 1.0f, 0.0f,},
		{0.5f*size, 1.0f*size,0, 1, 1.0f, 0.5f,},

		{0.5f*size, 1.0f*size,0, 1, 0.0f, 0.5f,},
		{1.0f*size, 0.0f*size,0, 1, 0.0f, 0.0f,},
		{1.0f*size, 1.0f*size,0, 1, 1.0f, 1.0f,},
	};
	pd3dDevice->SetTexture( 0, ppTex[1-id] );	// ordinary
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetPixelShader(0);
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 4, Vertex, sizeof( TVERTEX ) );
}


//-----------------------------------------------------------------------------
// Name: UpdateParticleMap()
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateParticleMap()
{
	DWORD i;
	D3DXVECTOR4 v;
	D3DCOLOR color;

	//-----------------------------------------------------
	// Change rendering target
	//-----------------------------------------------------
	m_pd3dDevice->SetRenderTarget(0, m_pParticleSurf);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
	D3DVIEWPORT9 viewport = {0,0
					, PARTICLEMAP_WIDTH
					, PARTICLEMAP_HEIGHT
					, 0.0f,1.0f};
	m_pd3dDevice->SetViewport(&viewport);

	//-----------------------------------------------------
	// Add new bullet
	//-----------------------------------------------------
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

	LVERTEX Vertex[4] = {
		//        x                  y           z
		{{                0,                  0,0.5,1},},
		{{PARTICLEMAP_WIDTH,                  0,0.5,1},},
		{{                0, PARTICLEMAP_HEIGHT,0.5,1},},
		{{PARTICLEMAP_WIDTH, PARTICLEMAP_HEIGHT,0.5,1},},
	};
	for(i=0;i<m_pBullet->GetNum();i++){
		CBullet *pBullet = m_pBullet->GetObj(i);
		if(pBullet->IsSetPosition()){
			Vertex[0].p[0] = (float)i    ;
			Vertex[1].p[0] = (float)(i+1);
			Vertex[2].p[0] = (float)i    ;
			Vertex[3].p[0] = (float)(i+1);

			// position
			Vertex[0].p[1] =  0.0f;
			Vertex[1].p[1] =  0.0f;
			Vertex[2].p[1] =  1.0f;
			Vertex[3].p[1] =  1.0f;
			v = pBullet->GetPosition();
			v = (0.5f/m_TexWorldScale)*v+D3DXVECTOR4(0.5f,0.5f,0.5f,0.5f);
			color = D3DCOLOR_RGBA(DWORD(256.0f*v.x), DWORD(256.0f*v.y), DWORD(256.0f*v.z), DWORD(256.0f*v.w));
			Vertex[0].c = Vertex[1].c = Vertex[2].c = Vertex[3].c = color;
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertex, sizeof( LVERTEX ) );

			// velocity
			Vertex[0].p[1] = 1.0f;
			Vertex[1].p[1] = 1.0f;
			Vertex[2].p[1] = 2.0f;
			Vertex[3].p[1] = 2.0f;
			v = pBullet->GetVelocity();
			v = (0.5f/m_TexWorldScale)*v+D3DXVECTOR4(0.5f,0.5f,0.5f,0.5f);
			color = D3DCOLOR_RGBA(DWORD(256.0f*v.x), DWORD(256.0f*v.y), DWORD(256.0f*v.z), DWORD(256.0f*v.w));
			Vertex[0].c = Vertex[1].c = Vertex[2].c = Vertex[3].c = color;
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertex, sizeof( LVERTEX ) );

			pBullet->SetPosition(FALSE);
		}
	}
	m_pd3dDevice->EndScene();
	m_pd3dDevice->BeginScene();


	//-----------------------------------------------------
	// Add time differentiation
	//-----------------------------------------------------
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) );
	m_pEffect->SetTechnique( m_hTechnique );
	m_pEffect->Begin( NULL, 0 );
	m_pEffect->Pass( PASS_MOVE_BULLET );
	m_pEffect->SetTexture("ParticleMap", m_pParticleTex );

	typedef struct {FLOAT p[4]; FLOAT tu0, tv0; FLOAT tu1, tv1;} TVERTEX;
	TVERTEX TVertex[4] = {
		// x  y  z rhw tu tv
		{{                0,                   0,0, 1}, 0+0.5f/PARTICLEMAP_WIDTH, 0+0.5f/PARTICLEMAP_HEIGHT, 0+0.5f/PARTICLEMAP_WIDTH, 0+1.5f/PARTICLEMAP_HEIGHT,},
		{{PARTICLEMAP_WIDTH,                   0,0, 1}, 1+0.5f/PARTICLEMAP_WIDTH, 0+0.5f/PARTICLEMAP_HEIGHT, 1+0.5f/PARTICLEMAP_WIDTH, 0+1.5f/PARTICLEMAP_HEIGHT,},
		{{                0,PARTICLEMAP_HEIGHT-1,0, 1}, 0+0.5f/PARTICLEMAP_WIDTH, 1-0.5f/PARTICLEMAP_HEIGHT, 0+0.5f/PARTICLEMAP_WIDTH, 1+0.5f/PARTICLEMAP_HEIGHT,},
		{{PARTICLEMAP_WIDTH,PARTICLEMAP_HEIGHT-1,0, 1}, 1+0.5f/PARTICLEMAP_WIDTH, 1-0.5f/PARTICLEMAP_HEIGHT, 1+0.5f/PARTICLEMAP_WIDTH, 1+0.5f/PARTICLEMAP_HEIGHT,},
	};
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, TVertex, sizeof( TVERTEX ) );

	m_pEffect->End();
	m_pd3dDevice->EndScene();

	//-----------------------------------------------------
	// Clear the acceleration
	//-----------------------------------------------------
	m_pd3dDevice->BeginScene();

	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	LVERTEX ClearVertex[4] = {
		// x  y  z rhw color
		{{                0,2,0, 1}, 0x808080,},
		{{PARTICLEMAP_WIDTH,2,0, 1}, 0x808080,},
		{{                0,4,0, 1}, 0x808080,},
		{{PARTICLEMAP_WIDTH,4,0, 1}, 0x808080,},
	};
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, ClearVertex, sizeof( LVERTEX ) );
}
//-----------------------------------------------------------------------------
// Name: UpdateAcceleration()
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateAcceleration(DWORD id)
{
	//------------------------------------------------------------------------
	// Compute Texture coordinate
	//------------------------------------------------------------------------
	m_pd3dDevice->SetRenderTarget(0, m_pMinSurf[0]);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	m_pd3dDevice->SetTexture( 0, m_pTex );
	m_pd3dDevice->SetTexture( 1, m_pCoordTex );
	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) );
	typedef struct {FLOAT p[4]; FLOAT tu0, tv0; FLOAT tu1, tv1;} T2VERTEX;
	T2VERTEX T2Vertex[4] = {
		// x  y  z rhw tu tv
		{{  0,  0,0, 1}, 0, 0, 0, 0,},
		{{512,  0,0, 1}, 1, 0, 1, 0,},
		{{  0,512,0, 1}, 0, 1, 0, 1,},
		{{512,512,0, 1}, 1, 1, 1, 1,},
	};
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, T2Vertex, sizeof( T2VERTEX ) );

	//------------------------------------------------------------------------
	// Bluring
	//------------------------------------------------------------------------
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_ADD );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_ADD );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_CURRENT );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	typedef struct {FLOAT p[4]; FLOAT tu0, tv0; FLOAT tu1, tv1; FLOAT tu2, tv2; FLOAT tu3, tv3;} T4VERTEX;
	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX4
		| D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3) );

	m_pd3dDevice->SetRenderTarget(0, m_pMinSurf[1]);
	m_pd3dDevice->SetTexture( 0, m_pMinTex[0] );
	m_pd3dDevice->SetTexture( 1, m_pMinTex[0] );
	m_pd3dDevice->SetTexture( 2, m_pMinTex[0] );
	m_pd3dDevice->SetTexture( 3, m_pMinTex[0] );
	T4VERTEX Vertex128[4] = {
		// x  y  z rhw tu tv
		{{  0,  0,0, 1}, 0+1.0f/512.0f, 0+1.0f/512.0f, 0+3.0f/512.0f, 0+1.0f/512.0f, 0+1.0f/512.0f, 0+3.0f/512.0f, 0+3.0f/512.0f, 0+3.0f/512.0f,},
		{{128,  0,0, 1}, 1+1.0f/512.0f, 0+1.0f/512.0f, 1+3.0f/512.0f, 0+1.0f/512.0f, 1+1.0f/512.0f, 0+3.0f/512.0f, 1+3.0f/512.0f, 0+3.0f/512.0f,},
		{{  0,128,0, 1}, 0+1.0f/512.0f, 1+1.0f/512.0f, 0+3.0f/512.0f, 1+1.0f/512.0f, 0+1.0f/512.0f, 1+3.0f/512.0f, 0+3.0f/512.0f, 1+3.0f/512.0f,},
		{{128,128,0, 1}, 1+1.0f/512.0f, 1+1.0f/512.0f, 1+3.0f/512.0f, 1+1.0f/512.0f, 1+1.0f/512.0f, 1+3.0f/512.0f, 1+3.0f/512.0f, 1+3.0f/512.0f,},
	};
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertex128, sizeof( T4VERTEX ) );

	m_pd3dDevice->SetRenderTarget(0, m_pMinSurf[2]);
	m_pd3dDevice->SetTexture( 0, m_pMinTex[1] );
	m_pd3dDevice->SetTexture( 1, m_pMinTex[1] );
	m_pd3dDevice->SetTexture( 2, m_pMinTex[1] );
	m_pd3dDevice->SetTexture( 3, m_pMinTex[1] );
	T4VERTEX Vertex32[4] = {
		// x  y  z rhw tu tv
		{{  0,  0,0, 1}, 0+1.0f/128.0f, 0+1.0f/128.0f, 0+3.0f/128.0f, 0+1.0f/128.0f, 0+1.0f/128.0f, 0+3.0f/128.0f, 0+3.0f/128.0f, 0+3.0f/128.0f,},
		{{ 32,  0,0, 1}, 1+1.0f/128.0f, 0+1.0f/128.0f, 1+3.0f/128.0f, 0+1.0f/128.0f, 1+1.0f/128.0f, 0+3.0f/128.0f, 1+3.0f/128.0f, 0+3.0f/128.0f,},
		{{  0, 32,0, 1}, 0+1.0f/128.0f, 1+1.0f/128.0f, 0+3.0f/128.0f, 1+1.0f/128.0f, 0+1.0f/128.0f, 1+3.0f/128.0f, 0+3.0f/128.0f, 1+3.0f/128.0f,},
		{{ 32, 32,0, 1}, 1+1.0f/128.0f, 1+1.0f/128.0f, 1+3.0f/128.0f, 1+1.0f/128.0f, 1+1.0f/128.0f, 1+3.0f/128.0f, 1+3.0f/128.0f, 1+3.0f/128.0f,},
	};
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertex32, sizeof( T4VERTEX ) );

	m_pd3dDevice->SetRenderTarget(0, m_pMinSurf[3]);
	m_pd3dDevice->SetTexture( 0, m_pMinTex[2] );
	m_pd3dDevice->SetTexture( 1, m_pMinTex[2] );
	m_pd3dDevice->SetTexture( 2, m_pMinTex[2] );
	m_pd3dDevice->SetTexture( 3, m_pMinTex[2] );
	T4VERTEX Vertex8[4] = {
		// x  y  z rhw tu tv
		{{  0,  0,0, 1}, 0+1.0f/32.0f, 0+1.0f/32.0f, 0+3.0f/32.0f, 0+1.0f/32.0f, 0+1.0f/32.0f, 0+3.0f/32.0f, 0+3.0f/32.0f, 0+3.0f/32.0f,},
		{{  8,  0,0, 1}, 1+1.0f/32.0f, 0+1.0f/32.0f, 1+3.0f/32.0f, 0+1.0f/32.0f, 1+1.0f/32.0f, 0+3.0f/32.0f, 1+3.0f/32.0f, 0+3.0f/32.0f,},
		{{  0,  8,0, 1}, 0+1.0f/32.0f, 1+1.0f/32.0f, 0+3.0f/32.0f, 1+1.0f/32.0f, 0+1.0f/32.0f, 1+3.0f/32.0f, 0+3.0f/32.0f, 1+3.0f/32.0f,},
		{{  8,  8,0, 1}, 1+1.0f/32.0f, 1+1.0f/32.0f, 1+3.0f/32.0f, 1+1.0f/32.0f, 1+1.0f/32.0f, 1+3.0f/32.0f, 1+3.0f/32.0f, 1+3.0f/32.0f,},
	};
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertex8, sizeof( T4VERTEX ) );

	m_pd3dDevice->SetRenderTarget(0, m_pMinSurf[4]);
	m_pd3dDevice->SetTexture( 0, m_pMinTex[3] );
	m_pd3dDevice->SetTexture( 1, m_pMinTex[3] );
	m_pd3dDevice->SetTexture( 2, m_pMinTex[3] );
	m_pd3dDevice->SetTexture( 3, m_pMinTex[3] );
	T4VERTEX Vertex2[4] = {
		// x  y  z rhw tu tv
		{{  0,  0,0, 1}, 0+1.0f/8.0f, 0+1.0f/8.0f, 0+3.0f/8.0f, 0+1.0f/8.0f, 0+1.0f/8.0f, 0+3.0f/8.0f, 0+3.0f/8.0f, 0+3.0f/8.0f,},
		{{  2,  0,0, 1}, 1+1.0f/8.0f, 0+1.0f/8.0f, 1+3.0f/8.0f, 0+1.0f/8.0f, 1+1.0f/8.0f, 0+3.0f/8.0f, 1+3.0f/8.0f, 0+3.0f/8.0f,},
		{{  0,  2,0, 1}, 0+1.0f/8.0f, 1+1.0f/8.0f, 0+3.0f/8.0f, 1+1.0f/8.0f, 0+1.0f/8.0f, 1+3.0f/8.0f, 0+3.0f/8.0f, 1+3.0f/8.0f,},
		{{  2,  2,0, 1}, 1+1.0f/8.0f, 1+1.0f/8.0f, 1+3.0f/8.0f, 1+1.0f/8.0f, 1+1.0f/8.0f, 1+3.0f/8.0f, 1+3.0f/8.0f, 1+3.0f/8.0f,},
	};
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertex2, sizeof( T4VERTEX ) );

	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	//------------------------------------------------------------------------
	// Putting Acceleration
	//------------------------------------------------------------------------
	m_pd3dDevice->SetRenderTarget(0, m_pParticleSurf);
	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) );
	T2VERTEX Vertex[4] = {
		// x  y  z rhw tu tv
		// x              y    z rhw   tu   tv
		{{(float)id    , 2.0f,0.5,1}, 0.5f,0.5f, ((float)id+0.5f)/PARTICLEMAP_WIDTH, 1.5f/PARTICLEMAP_HEIGHT},
		{{(float)(id+1), 2.0f,0.5,1}, 0.5f,0.5f, ((float)id+0.5f)/PARTICLEMAP_WIDTH, 1.5f/PARTICLEMAP_HEIGHT},
		{{(float)id    , 3.0f,0.5,1}, 0.5f,0.5f, ((float)id+0.5f)/PARTICLEMAP_WIDTH, 1.5f/PARTICLEMAP_HEIGHT},
		{{(float)(id+1), 3.0f,0.5,1}, 0.5f,0.5f, ((float)id+0.5f)/PARTICLEMAP_WIDTH, 1.5f/PARTICLEMAP_HEIGHT},
	};
	m_pEffect->SetTechnique( m_hTechnique );
	m_pEffect->Begin( NULL, 0 );
	m_pEffect->Pass( PASS_COMPUTE_ACC );
	m_pEffect->SetTexture("CoordMap", m_pMinTex[4] );
	m_pEffect->SetTexture("NormalMap", m_pNormalTex );
	m_pEffect->SetTexture("VelocityMap", m_pParticleTex );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertex, sizeof( T2VERTEX ) );
	m_pEffect->End();

	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	m_pDecl						= NULL;
	m_pMeshBg					= new CD3DMesh();	
	m_pBullet					= new CBulletMgr();	
	m_fShootingStar				= 0;
	m_TexWorldScale				= 20.0f;

	m_pEffect					= NULL;
	m_hTechnique				= NULL;
	m_hmWVPbg					= NULL;
	m_hmWVP						= NULL;
	m_hvVel						= NULL;
	m_hfDisplacement			= NULL;

	m_pTex						= NULL;
	m_pSurf						= NULL;
	m_pTotalTex					= NULL;
	m_pTotalSurf				= NULL;
	m_pZ						= NULL;
	m_pBurnTex					= NULL;
	m_pBurnSurf					= NULL;
	m_pParticleTex				= NULL;
	m_pParticleSurf				= NULL;
	m_pCoordTex					= NULL;
	m_pCoordSurf				= NULL;
	m_pNormalTex				= NULL;
	m_pNormalSurf				= NULL;
	m_pMinTex[0]				= NULL;
	m_pMinTex[1]				= NULL;
	m_pMinTex[2]				= NULL;
	m_pMinTex[3]				= NULL;
	m_pMinTex[4]				= NULL;
	m_pMinSurf[0]				= NULL;
	m_pMinSurf[1]				= NULL;
	m_pMinSurf[2]				= NULL;
	m_pMinSurf[3]				= NULL;
	m_pMinSurf[4]				= NULL;
	m_pBlurTex					= NULL;
	m_pBlurSurf					= NULL;
	m_pSeedTex					= NULL;
	m_pFireTex[0]				= NULL;
	m_pFireTex[1]				= NULL;
	m_pFireSurf[0]				= NULL;
	m_pFireSurf[1]				= NULL;
	m_pCatTex[0]				= NULL;
	m_pCatTex[1]				= NULL;
	m_pCatSurf[0]				= NULL;
	m_pCatSurf[1]				= NULL;
	
	for(int i=0;i<BULLET_MAX;i++){
		m_pQuery[i][0]			= NULL;
		m_pQuery[i][1]			= NULL;
		m_bCheck[i][0]			= 0;
		m_bCheck[i][1]			= 0;
	}

	m_dwCreationWidth           = 500;
m_dwCreationWidth           = 6*128;
m_dwCreationWidth           = 8*128;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

	m_id = 0;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

	m_fMousePos[0]				= (FLOAT)m_dwCreationWidth/2;	
	m_fMousePos[1]				= (FLOAT)m_dwCreationHeight/2;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = -0.5f;
    m_fWorldRotY                =  0.5f*D3DX_PI;
	m_zoom						=  12.0f;
//	m_zoom						=  10.0f;
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Application destructor.  Paired with CMyD3DApplication()
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // TODO: perform one time initialization

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
    UNREFERENCED_PARAMETER( adapterFormat );
    UNREFERENCED_PARAMETER( backBufferFormat );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    // Vertex shader
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1)
    &&  0==(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
			return E_FAIL;

	// pixel shader
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )
		return E_FAIL;
    if(!(pCaps->DevCaps2 & D3DDEVCAPS2_DMAPNPATCH ) ||
       FAILED( m_pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal
										, pCaps->DeviceType
                                        ,  adapterFormat
                                        ,  D3DUSAGE_RENDERTARGET|D3DUSAGE_DMAP
                                        ,  D3DRTYPE_TEXTURE
                                        ,  D3DFMT_A32B32G32R32F ) ))
		return E_FAIL;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	HRESULT hr;

	// Vertex declaration
	if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( decl, &m_pDecl ) ) )
		return DXTRACE_ERR( "CreateVertexDeclaration", hr );
	
	// Read model
	m_pBullet->CreateMesh( m_pd3dDevice, "star.x" );

	// Load ground
	if( FAILED( hr = m_pMeshBg->Create(m_pd3dDevice, "map.x" )))
		return DXTRACE_ERR( "Load Mesh", hr );

    // Shader
    LPD3DXBUFFER pErr;
    if( FAILED( hr = D3DXCreateEffectFromFile(
                m_pd3dDevice, "hlsl.fx", NULL, NULL, 
                0, NULL, &m_pEffect, &pErr ))){
        MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer()
                    , "ERROR", MB_OK);
        return DXTRACE_ERR( "CreateEffectFromFile", hr );
    }
    m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
    m_hmWVPbg = m_pEffect->GetParameterByName( NULL, "mWVPbg" );
    m_hmWVP = m_pEffect->GetParameterByName( NULL, "mWVP" );
    m_hvVel  = m_pEffect->GetParameterByName( NULL, "vVelocity" );
	m_hfDisplacement = m_pEffect->GetParameterByName( NULL, "fDisplacement" );

	// the texture of a seed of the fire
	if( FAILED( hr = D3DXCreateTextureFromFile(
				m_pd3dDevice, "fire.bmp", &m_pSeedTex)))
		return DXTRACE_ERR( "Load Texture", hr );

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	HRESULT hr;
	DWORD i;
    D3DXMATRIX m;
	
	// query
	for(i=0;i<BULLET_MAX;i++){
		if (FAILED(hr = m_pd3dDevice->CreateQuery(
							D3DQUERYTYPE_OCCLUSION, &m_pQuery[i][0] )))
			return DXTRACE_ERR( "Create Query", hr );
		if (FAILED(hr = m_pd3dDevice->CreateQuery(
							D3DQUERYTYPE_OCCLUSION, &m_pQuery[i][1] )))
			return DXTRACE_ERR( "Create Query", hr );
	}

	// Mesh
	m_pMeshBg->RestoreDeviceObjects(m_pd3dDevice);

	// Displacement mesh
	m_pBullet->RestoreDeviceObjects(m_pd3dDevice);
	m_UserInput.bClick = FALSE;
	
	// Shader
	if( m_pEffect != NULL ) m_pEffect->OnResetDevice();

    // Restore the font
    m_pFont->RestoreDeviceObjects();

	// Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

	// Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        D3DZB_TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -m_zoom );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );

	D3DXVECTOR3 vEye	  = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
    D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    D3DXMatrixLookAtLH( &m_mUnderView, &vEye, &vLookatPt, &vUp );
    
    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

	D3DXMatrixOrthoLH(&m_mUnderProj
				, 12.0f, 12.0f			// width, height
				, -12.0f, 12.0f );		// min_z, max_z


	// screen width
	D3DVIEWPORT9 viewport;
	m_pd3dDevice->GetViewport(&viewport);
	m_Width  = viewport.Width;
	m_Height = viewport.Height;

	// Creation of rendering target
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(
						MAP_SIZE, MAP_SIZE, 
						D3DFMT_D16, D3DMULTISAMPLE_NONE, 0,
						TRUE, &m_pZ, NULL)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateTexture(
						MAP_SIZE, MAP_SIZE, 1, 
						D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT, &m_pTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pTex->GetSurfaceLevel(0, &m_pSurf)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateTexture(
						MAP_SIZE, MAP_SIZE, 1, 
						D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT, &m_pTotalTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pTotalTex->GetSurfaceLevel(0, &m_pTotalSurf)))
		return E_FAIL;

	// burning information
	if (FAILED(m_pd3dDevice->CreateTexture(
						MAP_SIZE, MAP_SIZE, 1, 
						D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT, &m_pBurnTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pBurnTex->GetSurfaceLevel(0, &m_pBurnSurf)))
		return E_FAIL;

	// burning information
	if (FAILED(m_pd3dDevice->CreateTexture(
						MAP_SIZE, MAP_SIZE, 1, 
						D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT, &m_pBlurTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pBlurTex->GetSurfaceLevel(0, &m_pBlurSurf)))
		return E_FAIL;

	// Phase space
	if (FAILED(m_pd3dDevice->CreateTexture(
					  PARTICLEMAP_WIDTH, PARTICLEMAP_HEIGHT, 1
					, D3DUSAGE_RENDERTARGET|D3DUSAGE_DMAP, D3DFMT_A32B32G32R32F
					, D3DPOOL_DEFAULT, &m_pParticleTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pParticleTex->GetSurfaceLevel(0, &m_pParticleSurf)))
		return E_FAIL;

	// Texture Coordinate
	if (FAILED(m_pd3dDevice->CreateTexture(
					  MAP_SIZE, MAP_SIZE, 1
					, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8
					, D3DPOOL_DEFAULT, &m_pCoordTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pCoordTex->GetSurfaceLevel(0, &m_pCoordSurf)))
		return E_FAIL;

	// Normal map
	if (FAILED(m_pd3dDevice->CreateTexture(
					  MAP_SIZE, MAP_SIZE, 1
					, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8
					, D3DPOOL_DEFAULT, &m_pNormalTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pNormalTex->GetSurfaceLevel(0, &m_pNormalSurf)))
		return E_FAIL;

	// Reduction map
	if (FAILED(m_pd3dDevice->CreateTexture(
					  512, 512, 1
					, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F
					, D3DPOOL_DEFAULT, &m_pMinTex[0], NULL)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateTexture(
					  128, 128, 1
					, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F
					, D3DPOOL_DEFAULT, &m_pMinTex[1], NULL)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateTexture(
					  32, 32, 1
					, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F
					, D3DPOOL_DEFAULT, &m_pMinTex[2], NULL)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateTexture(
					  8, 8, 1
					, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F
					, D3DPOOL_DEFAULT, &m_pMinTex[3], NULL)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateTexture(
					  2, 2, 1
					, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F
					, D3DPOOL_DEFAULT, &m_pMinTex[4], NULL)))
		return E_FAIL;
	if (FAILED(m_pMinTex[0]->GetSurfaceLevel(0, &m_pMinSurf[0]))) return E_FAIL;
	if (FAILED(m_pMinTex[1]->GetSurfaceLevel(0, &m_pMinSurf[1]))) return E_FAIL;
	if (FAILED(m_pMinTex[2]->GetSurfaceLevel(0, &m_pMinSurf[2]))) return E_FAIL;
	if (FAILED(m_pMinTex[3]->GetSurfaceLevel(0, &m_pMinSurf[3]))) return E_FAIL;
	if (FAILED(m_pMinTex[4]->GetSurfaceLevel(0, &m_pMinSurf[4]))) return E_FAIL;

	for(i=0;i<2;i++){
		// Seed of fire
		if (FAILED(m_pd3dDevice->CreateTexture(
							CATMAP_SIZE, CATMAP_SIZE, 1, 
							D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
							D3DPOOL_DEFAULT, &m_pCatTex[i], NULL)))
			return E_FAIL;
		if (FAILED(m_pCatTex[i]->GetSurfaceLevel(0, &m_pCatSurf[i])))
			return E_FAIL;
		// Fire
		if (FAILED(m_pd3dDevice->CreateTexture(
							MAP_SIZE, MAP_SIZE, 1, 
							D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
							D3DPOOL_DEFAULT, &m_pFireTex[i], NULL)))
			return E_FAIL;
		if (FAILED(m_pFireTex[i]->GetSurfaceLevel(0, &m_pFireSurf[i])))
			return E_FAIL;
	}

	//-------------------------------------------------------------------------
	// Initialization
	//-------------------------------------------------------------------------
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;
	m_pd3dDevice->BeginScene();
	m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
	m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
	m_pd3dDevice->GetViewport(&oldViewport);

	m_pd3dDevice->SetRenderTarget(0, m_pTotalSurf);
	m_pd3dDevice->SetDepthStencilSurface(m_pZ);
	D3DVIEWPORT9 map_viewport = {0,0, MAP_SIZE, MAP_SIZE, 0.0f,1.0f};
	m_pd3dDevice->SetViewport(&map_viewport);
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x000000, 1.0f, 0L );
	
	m_pd3dDevice->SetRenderTarget(0, m_pBurnSurf);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
	m_pd3dDevice->SetViewport(&map_viewport);
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x000000, 1.0f, 0L );
	
	m_pd3dDevice->SetRenderTarget(0, m_pFireSurf[0]);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
	m_pd3dDevice->SetViewport(&map_viewport);
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x000000, 1.0f, 0L );
	
	// Particle map
	m_pd3dDevice->SetRenderTarget(0, m_pParticleSurf);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x000000, 1.0f, 0L );
	
	// Texture coordinate
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
	m_pEffect->SetTechnique( m_hTechnique );
	m_pEffect->Begin( NULL, 0 );
	m_pEffect->Pass( PASS_CREATE_TEXCOORD );
	m = m_mUnderView * m_mUnderProj;
	m_pEffect->SetMatrix( m_hmWVPbg, &m );

	m_pd3dDevice->SetRenderTarget(0, m_pCoordSurf);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x000000, 1.0f, 0L );
	m_pMeshBg->Render( m_pd3dDevice );

	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );

	// NormalMap
	m_pEffect->Pass( PASS_CREATE_NORMAL );
	m_pd3dDevice->SetRenderTarget(0, m_pNormalSurf);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x000000, 1.0f, 0L );
	m_pMeshBg->Render( m_pd3dDevice );

	m_pEffect->End();

	// Catmap
	m_iCatId = 0;
	m_pd3dDevice->SetRenderTarget(0, m_pCatSurf[m_iCatId]);
	map_viewport.Width = map_viewport.Height = CATMAP_SIZE;
	m_pd3dDevice->SetViewport(&map_viewport);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	m_pd3dDevice->SetTexture( 0, m_pSeedTex);		// initialize
	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	TVERTEX Vertex[4] = {
		//  x        y     z rhw tu tv
		{CATMAP_SIZE,CATMAP_SIZE,0, 1, 0.0f, 1.0f,},
		{          0,CATMAP_SIZE,0, 1, 1.0f, 1.0f,},
		{          0,          0,0, 1, 1.0f, 0.0f,},
		{CATMAP_SIZE,          0,0, 1, 0.0f, 0.0f,},
	};
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
	
	m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
	m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
	m_pd3dDevice->SetViewport(&oldViewport);
	pOldBackBuffer->Release();
	pOldZBuffer->Release();
	m_pd3dDevice->EndScene();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	HRESULT hr;

    // Update user input state
    UpdateInput( &m_UserInput );

    // Update the camera state according to user input
    D3DXMATRIX m;
    D3DXMATRIX mCamera;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &mCamera, &matRotY, &matRotX );

	// Zooming
	if(m_UserInput.bZ && !m_UserInput.bX)
		m_zoom += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_zoom -= 0.01f;

    // View matrix
	D3DXVECTOR3 vEye    = D3DXVECTOR3( 0.0f, 0.0f, -m_zoom );
    D3DXVECTOR3 vLookat = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m, &vEye, &vLookat, &vUp );

	m_mView = mCamera * m;

	//-----------------------------------------------------------------------------
	// Sometimes shooting a star
	//-----------------------------------------------------------------------------
	m_fShootingStar -= 1.0f;
if(0){
//	if(m_fShootingStar<=0.0f){
		m_fShootingStar += 30.0f;
		D3DXVECTOR4 x, v;
		x.x = (10.0f/4096.0f)*(float)(rand()%0xfff)-5.0f;
		x.z = (10.0f/4096.0f)*(float)(rand()%0xfff)-5.0f;
		x.y = 10.0f;
		v.x = (10.0f/4096.0f)*(float)(rand()%0xfff)-5.0f;
		v.z = (10.0f/4096.0f)*(float)(rand()%0xfff)-5.0f;
		v.y = 0.0f;
		v = v - x;
		v.w=0;
		D3DXVec4Normalize(&v,&v);
		v *= BULLET_SPEED;
		m_pBullet->Add(x, v);
	}
	
	if( m_UserInput.bClick){ m_UserInput.bClick = 0;
		D3DXMATRIX mat = m_mView * m_mProj;
		// target point
		D3DXVECTOR4 c = D3DXVECTOR4(
					 2.0f*m_fMousePos[0]/(FLOAT)m_Width -1,
					-2.0f*m_fMousePos[1]/(FLOAT)m_Height+1,
					1.0f,1.0f);
		D3DXMatrixInverse(&mat, NULL, &mat);
		D3DXVec4Transform  (&c, &c, &mat);
		c /= c.w;

		D3DXVECTOR4 x = D3DXVECTOR4( 0, -0.5f, 0, 1);
		D3DXMatrixInverse(&mat, NULL, &m_mView);
		D3DXVec4Transform (&x, &x, &mat);
		// velocity
		D3DXVECTOR4 v = 100.0f*c-x;
		v.w=0;
		D3DXVec4Normalize(&v,&v);
		v *= BULLET_SPEED;

		m_pBullet->Add(x, v);
	}

	//-----------------------------------------------------------------------------
	// Check query
	//-----------------------------------------------------------------------------
	for(DWORD i=0;i<m_pBullet->GetNum();i++){
		DWORD num=1;
		if(m_bCheck[i][m_id]){m_bCheck[i][m_id]=0;
			while(S_OK !=(hr = m_pQuery[i][m_id]->GetData(
						&num, sizeof(DWORD), D3DGETDATA_FLUSH ))){
				if(D3DERR_DEVICELOST == hr) break;
			}
			if(0==num){
				m_pBullet->GetObj(i)->ReqState(BULLET_STATE_BURN);
			}
		}
	}
	//-----------------------------------------------------------------------------
	// Move bullets
	//-----------------------------------------------------------------------------
	m_pBullet->Update(m_fElapsedTime);
	
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bRotateDown  = ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bRotateLeft  = ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bRotateRight = ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );

	pUserInput->bZ = ( m_bActive && (GetAsyncKeyState( 'Z' ) & 0x8000) == 0x8000 );
	pUserInput->bX = ( m_bActive && (GetAsyncKeyState( 'X' ) & 0x8000) == 0x8000 );
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	DWORD i;
	D3DXMATRIX matIdentity;
    D3DXMATRIX m, mVP;
	D3DXMATRIX mW;
    D3DXMATRIX mVisibleProj;
	D3DXVECTOR4 v;
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;

	//---------------------------------------------------------
	// make matrix
	//---------------------------------------------------------
	mVP = m_mUnderView * m_mUnderProj;

	D3DXMatrixOrthoLH(&mVisibleProj
				, 30.0f, 30.0f			// width, height
				, -100.0f, 100.0f );		// min_z, max_z

	// Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		//---------------------------------------------------------------------
		// Back up a frame buffer
		//---------------------------------------------------------------------
		m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
		m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
		m_pd3dDevice->GetViewport(&oldViewport);

		//---------------------------------------------------------------------
		// Move bullet
		//---------------------------------------------------------------------
		UpdateParticleMap( );

		//---------------------------------------------------------------------
		// Update CatMap
		//---------------------------------------------------------------------
		m_iCatId = 1-m_iCatId;
		UpdateCatMap( m_pd3dDevice, m_pCatTex, m_pCatSurf, m_iCatId );

		//---------------------------------------------------------------------
		// Change the rendering target
		//---------------------------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, m_pSurf);
		m_pd3dDevice->SetDepthStencilSurface(m_pZ);
		// Change viewport
		D3DVIEWPORT9 viewport = {0,0      // 
						, MAP_SIZE // Width
						, MAP_SIZE // Height
						, 0.0f,1.0f};     // near, far
		m_pd3dDevice->SetViewport(&viewport);
		m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
			, 0x000000, 1.0f, 0L);

		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE , 0 );

		//---------------------------------------------------------------------
        // Render world
		//---------------------------------------------------------------------
		// Set the world matrix
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mUnderView );
	    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mUnderProj );

		m_pMeshBg->Render( m_pd3dDevice );

		m_pEffect->SetTechnique( m_hTechnique );

		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE , 0xf );
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		m_pd3dDevice->SetSamplerState (0,D3DSAMP_MAGFILTER ,	D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState (0,D3DSAMP_MINFILTER ,	D3DTEXF_LINEAR );

		m_pEffect->SetMatrix( m_hmWVPbg, &mVP );

		for(i=0;i<m_pBullet->GetNum();i++){
			CBullet *pBullet = m_pBullet->GetObj(i);
			if(BULLET_STATE_FLY == pBullet->GetState()){
				m_pd3dDevice->SetRenderTarget(0, m_pSurf);
				m_pd3dDevice->SetDepthStencilSurface(m_pZ);
				m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L);
				m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
				m_pd3dDevice->SetTexture(D3DDMAPSAMPLER, m_pParticleTex);
				m_pd3dDevice->SetNPatchMode(1);
				m_pd3dDevice->SetVertexDeclaration( m_pDecl );

				// Velocity
				v = 2.0f*pBullet->GetVelocity();
				m_pEffect->SetVector( m_hvVel, &v );

				m_pEffect->Begin( NULL, 0 );
				m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );
				m_pEffect->Pass( 0 );
				pBullet->RenderPathVolume( m_pd3dDevice );

				m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
				m_pEffect->Pass( 1 );
				pBullet->RenderPathVolume( m_pd3dDevice );
				m_pEffect->End( );

				m_pd3dDevice->SetNPatchMode(0);

				m_pd3dDevice->SetRenderTarget(0, m_pTotalSurf);
				m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
				m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
				m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);

				TVERTEX Vertex[4] = {
					//   x        y      z   rhw       tu                  tv
					{{MAP_SIZE,MAP_SIZE,0.1f, 1}, 1.0f+0.5f/MAP_SIZE, 1.0f+0.5f/MAP_SIZE,},
					{{       0,MAP_SIZE,0.1f, 1}, 0.0f+0.5f/MAP_SIZE, 1.0f+0.5f/MAP_SIZE,},
					{{       0,       0,0.1f, 1}, 0.0f+0.5f/MAP_SIZE, 0.0f+0.5f/MAP_SIZE,},
					{{MAP_SIZE,       0,0.1f, 1}, 1.0f+0.5f/MAP_SIZE, 0.0f+0.5f/MAP_SIZE,},
				};

				m_pd3dDevice->SetTexture( 0, m_pTex );
				m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
				m_pd3dDevice->SetVertexShader(NULL);
				m_pd3dDevice->SetPixelShader(0);
				m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );

				m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
				UpdateAcceleration(i);

				m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
			}
		}
        
		//-------------------------------------------------
		// Blur this result
		//-------------------------------------------------
		m_pd3dDevice->SetRenderTarget( 0, m_pBlurSurf );
		m_pd3dDevice->SetDepthStencilSurface( NULL );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );
		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE , 0xf );
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,	D3DTOP_ADD);
		m_pd3dDevice->SetTextureStageState(2,D3DTSS_COLOROP,	D3DTOP_ADD);
		m_pd3dDevice->SetTextureStageState(3,D3DTSS_COLOROP,	D3DTOP_ADD);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(3,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2,	D3DTA_CURRENT);
		m_pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG2,	D3DTA_CURRENT);
		m_pd3dDevice->SetTextureStageState(3,D3DTSS_COLORARG2,	D3DTA_CURRENT);

		m_pd3dDevice->SetTexture(0, m_pTotalTex);
		m_pd3dDevice->SetTexture(1, m_pTotalTex);
		m_pd3dDevice->SetTexture(2, m_pTotalTex);
		m_pd3dDevice->SetTexture(3, m_pTotalTex);
		m_pEffect->Begin( NULL, 0 );
		m_pEffect->Pass( 2 );
		BLUR_VERTEX BlurVertex[4] = {
			//  x  y   z    tu tv
			{ -1, -1, 0.1f,  0, 1,},
			{  1, -1, 0.1f,  1, 1,},
			{  1,  1, 0.1f,  1, 0,},
			{ -1,  1, 0.1f,  0, 0,},
		};
		m_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, BlurVertex, sizeof( BLUR_VERTEX ) );

		m_pd3dDevice->SetTexture(0, NULL);
		m_pd3dDevice->SetTexture(1, NULL);
		m_pd3dDevice->SetTexture(2, NULL);
		m_pd3dDevice->SetTexture(3, NULL);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,	D3DTOP_DISABLE);
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		//-------------------------------------------------
		// Create Fire
		//-------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, m_pFireSurf[m_iCatId]);
		m_pd3dDevice->SetDepthStencilSurface(m_pZ);
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
							0, 1.0f, 0L );

		m_pEffect->Pass( 3 );
		m = m_mView * m_mProj;
		m_pEffect->SetMatrix( m_hmWVP, &m );
		m_pEffect->SetMatrix( m_hmWVPbg, &mVP );
		m_pd3dDevice->SetTexture(1, m_pTotalTex);
		m_pd3dDevice->SetTexture(2, m_pCatTex[m_iCatId]);
		m_pMeshBg->Render( m_pd3dDevice );
		m_pEffect->End();

		m_pd3dDevice->SetTexture( 1, NULL );
		m_pd3dDevice->SetTexture( 2, NULL );

		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_MODULATE);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		m_pd3dDevice->SetSamplerState (0,D3DSAMP_MAGFILTER ,	D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState (0,D3DSAMP_MINFILTER ,	D3DTEXF_LINEAR );

		TLVERTEX FireVertex[4] = {
			//   x        y      z   rhw                                      tu                  tv
			{{MAP_SIZE,MAP_SIZE,0.1f, 1}, D3DCOLOR_RGBA(250,250,250,0), 1.0f+0.5f/MAP_SIZE, 1.0f+2.0f/MAP_SIZE,},
			{{       0,MAP_SIZE,0.1f, 1}, D3DCOLOR_RGBA(250,250,250,0), 0.0f+0.5f/MAP_SIZE, 1.0f+2.0f/MAP_SIZE,},
			{{       0,       0,0.1f, 1}, D3DCOLOR_RGBA(250,250,250,0), 0.0f+0.6f/MAP_SIZE, 0.0f+2.0f/MAP_SIZE,},
			{{MAP_SIZE,       0,0.1f, 1}, D3DCOLOR_RGBA(250,250,250,0), 1.0f+0.4f/MAP_SIZE, 0.0f+2.0f/MAP_SIZE,},
		};

		m_pd3dDevice->SetTexture( 0, m_pFireTex[1-m_iCatId] );	// ordinary
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetPixelShader(0);
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, FireVertex, sizeof( TLVERTEX ) );

		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);

		//-------------------------------------------------
		// Restore the frame buffer
		//-------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
		m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
		m_pd3dDevice->SetViewport(&oldViewport);
		pOldBackBuffer->Release();
		pOldZBuffer->Release();

		//---------------------------------------------------------------------
		// Visiblity test
		//---------------------------------------------------------------------
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							0x404080, 1.0f, 0L );

		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);

		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE , 0x0 );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		m_pEffect->SetTechnique( m_hTechnique );
		m_pEffect->Begin( NULL, 0 );
		m_pEffect->Pass( PASS_DISPLACEMENT_MESH );

		m_pd3dDevice->SetTexture(D3DDMAPSAMPLER, m_pParticleTex);
		m_pd3dDevice->SetNPatchMode(1);
		m_pd3dDevice->SetVertexDeclaration( m_pDecl );

		// transformation matrix
		m = m_mUnderView * mVisibleProj;
		D3DXMatrixTranspose( &m, &m );
		m_pd3dDevice->SetVertexShaderConstantF( 0, (float*)&m, 4 );
		// degree of the displacement mapping
		m_pEffect->SetFloat( m_hfDisplacement, m_TexWorldScale );
		// texture
		m_pd3dDevice->SetTexture(0, m_pBullet->GetObj(0)->GetMesh()->m_pTextures[0]);

		for(i=0;i<m_pBullet->GetNum();i++){
			CBullet *pBullet = m_pBullet->GetObj(i);
			if(BULLET_STATE_FLY == pBullet->GetState()){
				m_pQuery[i][m_id]->Issue( D3DISSUE_BEGIN );
				m_pd3dDevice->SetTexture(0, pBullet->GetMesh()->m_pTextures[0]);
				pBullet->GetMesh()->Render(m_pd3dDevice);				// rendering
				m_pQuery[i][m_id]->Issue( D3DISSUE_END );
				m_bCheck[i][m_id] = 1;
			}
		}
		m_id ^= 1;

		m_pd3dDevice->SetNPatchMode(0);
		m_pEffect->End();
		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE , 0xf );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
							0x404080, 1.0f, 0L );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pd3dDevice->SetSamplerState (0,D3DSAMP_MAGFILTER ,	D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState (0,D3DSAMP_MINFILTER ,	D3DTEXF_LINEAR );
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);

		//-------------------------------------------------
        // render world
		//-------------------------------------------------
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );
	    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

		m_pEffect->SetTechnique( m_hTechnique );
		m_pEffect->Begin( NULL, 0 );
		m_pEffect->Pass( 4 );

		m = m_mView * m_mProj;
		m_pEffect->SetMatrix( m_hmWVP, &m );
		m_pEffect->SetMatrix( m_hmWVPbg, &mVP );

		m_pd3dDevice->SetTexture(1, m_pBlurTex);

		m_pMeshBg->Render( m_pd3dDevice );
		m_pEffect->End();

		//-------------------------------------------------------------------------
		// Render Fire
		//-------------------------------------------------------------------------
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,    D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,  D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetPixelShader(0);
		FLOAT X = (FLOAT)this->m_rcWindowClient.right;
		FLOAT Y = (FLOAT)this->m_rcWindowClient.bottom;
		TVERTEX Vertex[4] = {
		//   x y z rhw  tu    tv
			{0,0,0, 1, 0.0f, 0.0f,},
			{X,0,0, 1, 1.0f, 0.0f,},
			{X,Y,0, 1, 1.0f, 1.0f,},
			{0,Y,0, 1, 0.0f, 1.0f,},
		};
		m_pd3dDevice->SetTexture( 0, m_pFireTex[m_iCatId] );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
		m_pd3dDevice->SetTexture(1, NULL);
		
		//-------------------------------------------------------------------------
		// Render bullets with the method of displacement mapping
		//-------------------------------------------------------------------------
		m_pEffect->SetTechnique( m_hTechnique );
		m_pEffect->Begin( NULL, 0 );
		m_pEffect->Pass( PASS_DISPLACEMENT_MESH );

		m_pd3dDevice->SetTexture(D3DDMAPSAMPLER, m_pParticleTex);
		m_pd3dDevice->SetNPatchMode(1);
		m_pd3dDevice->SetVertexDeclaration( m_pDecl );
		// transformation matrix
		m = m_mView * m_mProj;

		D3DXMatrixTranspose( &m, &m );
		m_pd3dDevice->SetVertexShaderConstantF( 0, (float*)&m, 4 );
		// degree of the displacement mapping
		m_pEffect->SetFloat( m_hfDisplacement, m_TexWorldScale );

		for(DWORD i=0;i<BULLET_MAX;i++){
			CBullet *pBullet = m_pBullet->GetObj(i);
			// texture
			m_pd3dDevice->SetTexture(0, pBullet->GetMesh()->m_pTextures[0]);
			// rendering
			pBullet->Render( m_pd3dDevice );
		}
		m_pd3dDevice->SetNPatchMode(0);
		m_pEffect->End();

#if 1 // For debugging, draw texture
		{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,    D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,  D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetPixelShader(0);
		for(DWORD l = 0; l < 8; l++){
			const float scale =128.0f;
			FLOAT X = (FLOAT)this->m_rcWindowClient.right;
			FLOAT Y = (FLOAT)this->m_rcWindowClient.bottom;
			TVERTEX Vertex[4] = {
				//  x       y    z rhw tu tv
				{X-(l+1)*scale,Y-scale,0, 1, 0.0f, 0.0f,},
				{X-(l+0)*scale,Y-scale,0, 1, 1.0f, 0.0f,},
				{X-(l+0)*scale,Y      ,0, 1, 1.0f, 1.0f,},
				{X-(l+1)*scale,Y      ,0, 1, 0.0f, 1.0f,},
			};
			switch(l){
//			case 0: m_pd3dDevice->SetTexture( 0, m_pMinTex[0] ); break;
//			case 1: m_pd3dDevice->SetTexture( 0, m_pMinTex[1] ); break;
//			case 2: m_pd3dDevice->SetTexture( 0, m_pMinTex[2] ); break;
//			case 3: m_pd3dDevice->SetTexture( 0, m_pMinTex[3] ); break;
//			case 4: m_pd3dDevice->SetTexture( 0, m_pMinTex[4] ); break;
			case 0: m_pd3dDevice->SetTexture( 0, m_pTex ); break;
			case 1: m_pd3dDevice->SetTexture( 0, m_pTotalTex ); break;
			case 2: m_pd3dDevice->SetTexture( 0, m_pBlurTex ); break;
			case 3: m_pd3dDevice->SetTexture( 0, m_pCatTex[0] ); break;
			case 4: m_pd3dDevice->SetTexture( 0, m_pFireTex[0] ); break;
			case 5:	m_pd3dDevice->SetTexture( 0, m_pParticleTex );	break;
			case 6: m_pd3dDevice->SetTexture( 0, m_pCoordTex ); break;
			case 7: m_pd3dDevice->SetTexture( 0, m_pNormalTex ); break;
			}
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		}
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
#endif

        // Render stats and help text  
        RenderText();

		// End the scene.
        m_pd3dDevice->EndScene();
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // Draw on the window tell the user that the app is loading
                // TODO: change as needed
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }
		case WM_MOUSEMOVE:
		{
			long x = (lParam << 16) >> 16;
			long y = lParam >> 16;
			m_fMousePos[0] = (FLOAT)x;
			m_fMousePos[1] = (FLOAT)y;
            break;
		}
		case WM_LBUTTONDOWN:
		{
			m_UserInput.bClick = TRUE;
            break;
		}
    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
	SAFE_RELEASE(m_pCatSurf[0]);
	SAFE_RELEASE(m_pCatSurf[1]);
	SAFE_RELEASE(m_pCatTex[0]);
	SAFE_RELEASE(m_pCatTex[1]);
	SAFE_RELEASE(m_pFireSurf[0]);
	SAFE_RELEASE(m_pFireSurf[1]);
	SAFE_RELEASE(m_pFireTex[0]);
	SAFE_RELEASE(m_pFireTex[1]);
	SAFE_RELEASE(m_pMinSurf[4]);
	SAFE_RELEASE(m_pMinSurf[3]);
	SAFE_RELEASE(m_pMinSurf[2]);
	SAFE_RELEASE(m_pMinSurf[1]);
	SAFE_RELEASE(m_pMinSurf[0]);
	SAFE_RELEASE(m_pMinTex[4]);
	SAFE_RELEASE(m_pMinTex[3]);
	SAFE_RELEASE(m_pMinTex[2]);
	SAFE_RELEASE(m_pMinTex[1]);
	SAFE_RELEASE(m_pMinTex[0]);
	SAFE_RELEASE(m_pNormalSurf);
	SAFE_RELEASE(m_pNormalTex);
	SAFE_RELEASE(m_pCoordSurf);
	SAFE_RELEASE(m_pCoordTex);
	SAFE_RELEASE(m_pParticleSurf);
	SAFE_RELEASE(m_pParticleTex);
	SAFE_RELEASE(m_pBlurSurf);
	SAFE_RELEASE(m_pBlurTex);
	SAFE_RELEASE(m_pBurnSurf);
	SAFE_RELEASE(m_pBurnTex);
	SAFE_RELEASE(m_pTotalSurf);
	SAFE_RELEASE(m_pTotalTex);
	SAFE_RELEASE(m_pSurf);
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pZ);

	for(int i=0;i<BULLET_MAX;i++){
		SAFE_RELEASE(m_pQuery[i][0]);
		SAFE_RELEASE(m_pQuery[i][1]);
		m_bCheck[i][0]			= 0;
		m_bCheck[i][1]			= 0;
	}


	// mesh
	m_pMeshBg->InvalidateDeviceObjects();
	m_pBullet->InvalidateDeviceObjects();

	// Shader
    if( m_pEffect != NULL ) m_pEffect->OnLostDevice();

    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // TODO: Cleanup any objects created in InitDeviceObjects()
	SAFE_RELEASE(m_pSeedTex);

	// Mesh
	m_pMeshBg->Destroy();
	m_pBullet->Destroy();
	
	SAFE_RELEASE( m_pDecl );

	SAFE_RELEASE( m_pEffect );    // Shader

    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    // TODO: Perform any final cleanup needed

	// Mesh
	SAFE_DELETE( m_pMeshBg );
	
	SAFE_DELETE( m_pBullet );

    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    return S_OK;
}





//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
// dow.cpp
//
//    Drops of Water source code - main application file
//
//                      ShaderX2 - www.shaderx2.com
//
//
//    Author:   Sylvain Lefebvre - 04/2003
//    ------
//
//    -= Check for updates at http://www.aracknea.net =-
//
//
//    File contents:
//    -------------
//       
//         Main application class. Built on top of a Direct3D SDK sample code (DirectX9).
//
//         The key methods are:
//
//            Render         - render one frame
//            RenderWetAreas - render the wet areas texture
//            PutSprite      - put a sprite in a texture layer
//            
//
//    Dependencies:
//    -------------
//
//         * DirectX 9 SDK
//         * Cg         - NVidia Cg         - www.cgshaders.org
//         * libTexture - load texture data - http://www.aracknea.net
//
//    Hardware requirements:
//    ----------------------
// 
//         ps 2.X        
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <commctrl.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "D3DFile.h"
#include "resource.h"
#include "MovieMaker.h"
#include "ScreenCapture.h"
//-----------------------------------------------------------------------------
#include "CDASplatch.h"
#include "CDARain.h"
#include "CDAShower.h"
//-----------------------------------------------------------------------------
#include <vector>
using namespace std;
//-----------------------------------------------------------------------------
#include "dow.h"
//-----------------------------------------------------------------------------
class alpha_height : public CTexture::alpha_functor
{
public:
    unsigned char computeAlpha(unsigned char r,unsigned char g,unsigned char b) const
    {
		return (r);
    }
};
//-----------------------------------------------------------------------------
CGprofile PixelProfile = CG_PROFILE_PS_2_X;
CGprofile VertexProfile = CG_PROFILE_VS_1_1;
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	CMyD3DApplication d3dApp;
	
	InitCommonControls();
	if( FAILED( d3dApp.Create( hInst ) ) )
		return 0;
	
	return d3dApp.Run();
}
//-----------------------------------------------------------------------------
// Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	// set window attributes
	m_strWindowTitle    = _T("Drop of Water demo application - ShaderX2");
	
	m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
	
	// load a font
	m_pFont                = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
	
	// set view point distance
	m_dDist             = -2.5;
	
	// init members
	m_PointVB           = NULL;
	m_Quad              = NULL;
	m_Cyl				= NULL;
	m_bWireframe        = false;
	m_bUniform          = false;
	m_bStop             = false;
	m_bHideDrops        = false;
	m_bStopDrops        = false;
	m_bShowWetAreas	    = false;
	m_bCylinder         = false;
	m_bRecording        = false;
	m_bShowHelp         = false;
	m_bClearWetAreas    = true;
	m_bHideText         = false;
	
	m_bDrawImpact       = false;
	m_Impact[0]         = 0.0;
	m_Impact[1]         = 0.0;
	
	// control texture repeat
	m_fRepeat = 1.0;
	
	// control drop refraction effect
	m_fMag=0.1f;
	m_fView=0.03f;
	
	// texture and surfaces
	m_pWetAreasSurface  = NULL;
	m_pColorMapTexture  = NULL;	
	m_pNormalMapTexture = NULL;
	
	// rotate object around Z axis
	m_dZAngle=0.0;
	
	// offset map
	for (int o=0;o<NB_OFFSET_MAP;o++)
	{
		m_pOffsetMap[o]=NULL;
		m_OffsetMap[o] = NULL;
		m_OffsetMapOverlap[o] = NULL;
	}
	// wet areas texture
	m_pWetAreasTexture=NULL;
	m_pWetAreasTexture=NULL;
	
	// selected offset map resolution
	m_iSelectedOffsetMapRes  = 32;
	
	// misc
	m_bMagEffectDemo    = false;
	
	// starting drop animator
	m_Animator=new CDAShower(this);
}
//-----------------------------------------------------------------------------
// Allocates the memory for texture sprites
//-----------------------------------------------------------------------------
void CMyD3DApplication::initDropsAndTexSprites()
{
	// init offset maps
	for (int o=0;o<NB_OFFSET_MAP;o++)
	{
		if (m_OffsetMap[o] != NULL)
			delete [] (m_OffsetMap[o]);
		if (m_OffsetMapOverlap[o] != NULL)
			delete [] (m_OffsetMapOverlap[o]);		
		// offset map resolution
		m_OffsetMapRes[o] = m_iSelectedOffsetMapRes;
		// offset map local data
		m_OffsetMap[o]        = new BYTE[m_OffsetMapRes[o]*m_OffsetMapRes[o]*4];
		// used to test offset map overlap
		m_OffsetMapOverlap[o] = new bool[m_OffsetMapRes[o]*m_OffsetMapRes[o]];
		memset(m_OffsetMapOverlap[o],0,sizeof(bool)*m_OffsetMapRes[o]*m_OffsetMapRes[o]);
	}
	
	// number of tiles in reference texture 
	// (in this case 2x2 tiles)
	m_dwRefTexNbTiles   = 2;
	
	// create the offset map texture in video memory
	CHECK_ERROR(CreateOffsetMap());
	// create the wet area texture in video memory
	CHECK_ERROR(CreateWetAreas());
	m_bClearWetAreas=true;
	
	// start drop animation
	m_Animator->init();
}
//-----------------------------------------------------------------------------
// load a texture to be displayed under the drop layers
//-----------------------------------------------------------------------------
void CMyD3DApplication::loadUnderlyingTexture(const char *tex_name,
											  const char *tex_nrm_name,
											  double scale)
{
	try
	{
		CTexture *tex;
		
		tex=CTexture::loadTexture(tex_name);
		if (m_pColorMapTexture != NULL)
			SAFE_RELEASE(m_pColorMapTexture);
		m_pColorMapTexture=d3dtex::d3dtexFromTexture(m_pd3dDevice,tex);
		if (tex_nrm_name != NULL)
		{
			delete (tex);
			tex=CTexture::loadTexture(tex_nrm_name);
		}
		tex->convertToNormal(scale);
		if (m_pNormalMapTexture != NULL)
			SAFE_RELEASE(m_pNormalMapTexture);
		m_pNormalMapTexture=d3dtex::d3dtexFromTexture(m_pd3dDevice,tex);
		delete (tex);
	}
	catch (CException& e)
	{
		MessageBox(NULL,e.getMsg(),"Error",MB_OK | MB_ICONSTOP);
		exit (-1);
	}
}
//-----------------------------------------------------------------------------
// Init everything (Cg programs, textures, create offset maps, ...)
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Init()
{
	WORD* pBaseIndex = NULL;
	
	m_pFont->RestoreDeviceObjects();
	
	// set ArcBall parameters
	m_ArcBall.SetWindow( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 2.0f );
	m_ArcBall.SetRadius( 10.0f );
	
	// ============
	// load textures
	try
	{
		CTexture *tex;
		
		tex=CTexture::loadTexture("drop.tga");
		tex->computeAlpha(alpha_height());
		tex->convertToNormal(30.0);
		m_pDropNormalTexture=d3dtex::d3dtexFromTexture(m_pd3dDevice,tex);
		delete (tex);
		m_pSplashTexture=d3dtex::d3dtexLoadTexture(m_pd3dDevice,"splash.tga");
	}
	catch (CException& e)
	{
		MessageBox(NULL,e.getMsg(),"Error",MB_OK | MB_ICONSTOP);
	}
	loadUnderlyingTexture("grid.tga","grid_relief.tga",7.0);
	
    // =================
	// init drops and texture sprites
	initDropsAndTexSprites();
	
    // =================
	// Cg init
    m_cgContext = cgCreateContext();
    cgD3D9SetDevice(m_pd3dDevice);
	
	// =================
	// load vertex program
    const char *profileOptsVP[] =
    {
		cgD3D9GetOptimalOptions(VertexProfile),
			NULL,
    };
    m_cgVertexProg = cgCreateProgramFromFile(m_cgContext, CG_SOURCE, 
		"vp_dow.cg", VertexProfile, "main", profileOptsVP);
	if (NULL == m_cgVertexProg)
	{
		MessageBox(NULL,"Vertex program compile error\nDoes your hardware support vs 1.1 ?","VP Error",MB_OK | MB_ICONSTOP);
		exit (-1);
	}
    if (FAILED(cgD3D9LoadProgram(m_cgVertexProg, true, 0)))
	{
		MessageBox(NULL,"Vertex program compile error\nDoes your hardware support vs 1.1 ?","VP Error",MB_OK | MB_ICONSTOP);
		exit (-1);
	}
    // get parameters
	m_cgProj = cgGetNamedParameter(m_cgVertexProg, "Proj");
	m_cgView = cgGetNamedParameter(m_cgVertexProg, "View");
	m_cgViewI = cgGetNamedParameter(m_cgVertexProg, "ViewI");
	m_cgViewerPos = cgGetNamedParameter(m_cgVertexProg, "ViewerPos");
	m_cgTScale = cgGetNamedParameter(m_cgVertexProg, "TScale");
	m_cgLightPos = cgGetNamedParameter(m_cgVertexProg, "LightPos");
	m_cgTexRepeat = cgGetNamedParameter(m_cgVertexProg, "TexRepeat");
	// set uniforms
	float scale=m_iSelectedOffsetMapRes/m_dwRefTexNbTiles;
    cgD3D9SetUniform(m_cgTScale,&scale);
	float lpos[4]={0.0,0.0,0.0,1.0};
    cgD3D9SetUniform(m_cgLightPos,&lpos);
	
	// =================
	// load drop layer fragment program
	
    const char *profileOptsFP[] =
    {
		cgD3D9GetOptimalOptions(PixelProfile),
			NULL,
    };
    m_cgFragmentProg = cgCreateProgramFromFile(m_cgContext, CG_SOURCE,
		"fp_dow_layer.cg", PixelProfile, "main", profileOptsFP);
	if (NULL == m_cgFragmentProg)
	{
		MessageBox(NULL,"Fragment program compile error\nDoes your hardware support ps 2.X ?","FP Error",MB_OK | MB_ICONSTOP);
		exit (-1);
	}
    if (FAILED(cgD3D9LoadProgram(m_cgFragmentProg, true, 0)))
	{
		MessageBox(NULL,"Fragment program compile error\nDoes your hardware support ps 2.X ?","FP Error",MB_OK | MB_ICONSTOP);
		exit (-1);
	}
    // get parameters
	m_cgOffsetMap = cgGetNamedParameter(m_cgFragmentProg, "OffsetMap");
	m_cgDropNrms = cgGetNamedParameter(m_cgFragmentProg, "DropNrms");
	m_cgColorMap = cgGetNamedParameter(m_cgFragmentProg, "ColorMap");
	m_cgMagCoeff = cgGetNamedParameter(m_cgFragmentProg, "MagCoeff");
	m_cgViewCoeff = cgGetNamedParameter(m_cgFragmentProg, "ViewCoeff");
	// sampler state	
	cgD3D9SetSamplerState(m_cgColorMap, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgColorMap, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgColorMap, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	cgD3D9SetSamplerState(m_cgDropNrms, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgDropNrms, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgDropNrms, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	// =================
	// load wet areas fragment program
    m_cgFragmentProgWetAreas = cgCreateProgramFromFile(m_cgContext, CG_SOURCE,
		"fp_dow_wet_areas.cg", PixelProfile, "main", profileOptsFP);
	if (NULL == m_cgFragmentProgWetAreas)
	{
		MessageBox(NULL,"Fragment program compile error (wet areas)","FP Error",MB_OK | MB_ICONSTOP);
	}
    if (FAILED(cgD3D9LoadProgram(m_cgFragmentProgWetAreas, true, 0)))
	{
		MessageBox(NULL,"Fragment program compile error (wet areas)","FP Error",MB_OK | MB_ICONSTOP);
	}
    // get parameters
	m_cgAreasOffsetMap = cgGetNamedParameter(m_cgFragmentProgWetAreas, "OffsetMap");
	m_cgAreasDropNrms = cgGetNamedParameter(m_cgFragmentProgWetAreas, "DropNrms");
	
	// =================
	// load wet surface fragment program
    m_cgFragmentProgFloor = cgCreateProgramFromFile(m_cgContext, CG_SOURCE,
		"fp_dow_surface.cg", PixelProfile, "main", profileOptsFP);
	if (NULL == m_cgFragmentProgWetAreas)
	{
		MessageBox(NULL,"Fragment program compile error (surface)","FP Error",MB_OK | MB_ICONSTOP);
	}
    if (FAILED(cgD3D9LoadProgram(m_cgFragmentProgFloor, true, 0)))
	{
		MessageBox(NULL,"Fragment program compile error (surface)","FP Error",MB_OK | MB_ICONSTOP);
	}
    // get parameters
	m_cgFloorColorMap = cgGetNamedParameter(m_cgFragmentProgFloor, "ColorMap");
	m_cgFloorNrmsMap = cgGetNamedParameter(m_cgFragmentProgFloor, "NrmsMap");
	m_cgFloorWetAreas = cgGetNamedParameter(m_cgFragmentProgFloor, "WetAreas");
	// sampler state
	cgD3D9SetSamplerState(m_cgFloorNrmsMap, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgFloorNrmsMap, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgFloorNrmsMap, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgFloorWetAreas, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgFloorWetAreas, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	cgD3D9SetSamplerState(m_cgFloorWetAreas, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	// ========================================
	// creates a quad
	if (NULL == m_Quad)
		delete (m_Quad);
	m_Quad=new CD3DQuad(m_pd3dDevice);
	
	// ========================================
	// creates a cylinder
	if (NULL == m_Cyl)
		delete (m_Cyl);
	m_Cyl=new CD3DCylinder(m_pd3dDevice);
	
	// ========================================
	// buffer for plot drawing
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( sizeof(DOWVERTEX),
		0, D3DFVF_DOWVERTEX,
		D3DPOOL_DEFAULT, &m_PointVB, NULL ) ) )
		return E_FAIL;
	
	
	// ========================================
	// set miscellaneous render states
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x00444444 );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		FALSE );
	
	// ========================================
	// set matrices
	FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspect, 0.01f, 10.0f );
	D3DXMatrixTranspose(&m_matProj, &m_matProj);
	
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matViewI);
	
	// ========================================
	// update menu state
	SetMenuStates();
	
	return S_OK;
}
//-----------------------------------------------------------------------------
// Draw an object (quad, cylinder, or anything you might add)
//-----------------------------------------------------------------------------
void CMyD3DApplication::Draw()
{
	if (m_bCylinder)
		m_Cyl->Render();
	else
		m_Quad->Render();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Render the wet areas texture
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderWetAreas()
{
	// Clear if needed
	if (m_bClearWetAreas)
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );
		m_bClearWetAreas=false;
	}
	
	// Set the projection matrix
	D3DXMATRIXA16 proj;
	D3DXMatrixOrthoOffCenterLH(&proj,-1.0,1.0,
		1.0,-1.0,
		-1.0,1.0);
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION,&proj);
	
	// Send matrices to Cg programs
	D3DXMATRIXA16 mat;
	D3DXMatrixTranspose(&mat, &proj);
	cgD3D9SetUniformMatrix(m_cgProj, &mat);
	
	D3DXMatrixIdentity(&mat);
	cgD3D9SetUniformMatrix(m_cgView, &mat);
	cgD3D9SetUniformMatrix(m_cgViewI, &mat);
	m_pd3dDevice->SetTransform(D3DTS_VIEW,&mat);
	m_pd3dDevice->SetTransform(D3DTS_WORLD,&mat);
	
	// Texture repeat
	float repeat=1.0;
    cgD3D9SetUniform(m_cgTexRepeat,&repeat);
	
	// Disable Z test and culling
	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	// Darken previous time step texture
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_Animator->dryingColor() );
	
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		// ====================
		// PASS 1 - old texture (darken by D3DRS_TEXTUREFACTOR)
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetPixelShader(NULL);
		
        m_pd3dDevice->SetTexture( 0, m_pWetAreasTexture );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		
		// render a textured quad
		m_Quad->Render();
		
		if (m_bDrawImpact)
		{
			// render a wet impact on the surface (for CDASplatch)
			m_bDrawImpact=false;
			VOID* pPlot;
			CHECK_ERROR(m_PointVB->Lock(0,sizeof(DOWVERTEX),(void**)&pPlot,0));
			DOWVERTEX plot;
			plot.x=m_Impact[0]*2.0-1.0;
			plot.y=m_Impact[1]*2.0-1.0;
			plot.z=0.0;
			memcpy(pPlot,(void *)&plot,sizeof(plot));
			m_PointVB->Unlock();
			float sz=400.0*m_dImpactSize*32.0/(double)m_iSelectedOffsetMapRes;
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_ONE );
			m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_ONE );
			m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&sz) );
			m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE);
			m_pd3dDevice->SetTexture( 0, m_pSplashTexture );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			m_pd3dDevice->SetStreamSource( 0, m_PointVB, 0, sizeof(DOWVERTEX) );
			m_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, 1 );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		}
		
		// ====================
		// PASS 2..n - drops
		// activate shaders
		cgD3D9BindProgram(m_cgVertexProg);
		cgD3D9SetTexture(m_cgAreasDropNrms,m_pDropNormalTexture);
		// For each layer
		for (int o=0;o<NB_OFFSET_MAP;o++)
		{
			// if empty, do nothing
			if (0 == m_NbSpritesInOffsetMap[o])
				continue;
			// else draw drops using Texture Sprites
			cgD3D9BindProgram(m_cgFragmentProgWetAreas);
			cgD3D9SetTexture(m_cgAreasOffsetMap,m_pOffsetMap[o]);
			m_Quad->Render();
		}
		
		m_pd3dDevice->EndScene();
	}
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	
	return S_OK;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Render scene
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	LPDIRECT3DSURFACE9 pBackBuffer;
	
	if (!m_bStop && !m_bMagEffectDemo)
	{
		// do one animation step
		// the animator calls PutSprite to update the offset map.
		m_Animator->step();		
		// render wet areas texture		
		// -> get current render target
		CHECK_ERROR(m_pd3dDevice->GetRenderTarget(0,&pBackBuffer));
		// -> set render target to texture
		CHECK_ERROR(m_pd3dDevice->SetRenderTarget(0,m_pWetAreasSurface));
		// -> render
		RenderWetAreas();
		// -> set render target to previous target
		CHECK_ERROR(m_pd3dDevice->SetRenderTarget(0,pBackBuffer));
		CHECK_ERROR(pBackBuffer->Release());
		
	}
	else if (m_bMagEffectDemo)
	{
		// clear wet floor
		CHECK_ERROR(m_pd3dDevice->GetRenderTarget(0,&pBackBuffer));
		CHECK_ERROR(m_pd3dDevice->SetRenderTarget(0,m_pWetAreasSurface));
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );
		CHECK_ERROR(m_pd3dDevice->SetRenderTarget(0,pBackBuffer));
		CHECK_ERROR(pBackBuffer->Release());
		// one drop in the middle
		ClearOffsetMap();
		PutSprite(0,0.5,0.5,0.0,1.0,true);
		UpdateOffsetMap();
	}
	
	// render surface with drops	
	cgD3D9SetUniformMatrix(m_cgProj, &m_matProj);
	cgD3D9SetUniformMatrix(m_cgView, &m_matView);
	cgD3D9SetUniformMatrix(m_cgViewI, &m_matViewI);
	
	// clear
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x000000ff, 1.0f, 0L );
	
	// texture repeat factor
	float repeat=m_fRepeat;
    cgD3D9SetUniform(m_cgTexRepeat,&repeat);
	
	// render
	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	// bind vertex program
	cgD3D9BindProgram(m_cgVertexProg);	
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		if (m_bWireframe)
		{
			// draw wireframe
			m_pd3dDevice->SetPixelShader(NULL);
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			Draw();
		}
		else if (m_bShowWetAreas)
		{
			m_pd3dDevice->SetPixelShader(NULL);
			m_pd3dDevice->SetTexture( 0, m_pWetAreasTexture );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			Draw();
		}
		else
		{
			// ==================
			// PASS 1 - render surface
			// activate shader
			cgD3D9BindProgram(m_cgFragmentProgFloor);
			cgD3D9SetTexture(m_cgFloorColorMap,m_pColorMapTexture);
			cgD3D9SetTexture(m_cgFloorWetAreas,m_pWetAreasTexture);
			cgD3D9SetTexture(m_cgFloorNrmsMap,m_pNormalMapTexture);
			// render the mesh
			Draw();
			// enable alpha blending
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
			m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );
			if (!m_bHideDrops)
			{
				// ==================
				// PASS 2..n - render drops
				// activate shaders
				cgD3D9SetTexture(m_cgColorMap,m_pColorMapTexture);
				cgD3D9SetTexture(m_cgDropNrms,m_pDropNormalTexture);
				cgD3D9SetUniform(m_cgMagCoeff,&m_fMag);
				cgD3D9SetUniform(m_cgViewCoeff,&m_fView);
				// For each layer
				for (int o=0;o<NB_OFFSET_MAP;o++)
				{
					// if empty do nothing
					if (0 == m_NbSpritesInOffsetMap[o])
						continue;
					// else draw drops using Texture Sprites
					cgD3D9BindProgram(m_cgFragmentProg);
					cgD3D9SetTexture(m_cgOffsetMap,m_pOffsetMap[o]);
					// render the mesh
					Draw();
				}
			}
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		}
		// dump out the FPS
		if (!m_bRecording && !m_bHideText)
		{
			static char str[64];
			
			m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
			int tot=0;
			for (int o=0;o<NB_OFFSET_MAP;o++)
			{
				sprintf(str,"%2d,",m_NbSpritesInOffsetMap[o]);
				m_pFont->DrawText( 2+o*40, 20, D3DCOLOR_ARGB(255,255,255,0), str );
				tot+=m_NbSpritesInOffsetMap[o];
			}
			sprintf(str,"(%3d drops)",tot);
			m_pFont->DrawText( 2+(o++)*40, 20, D3DCOLOR_ARGB(255,255,255,0), str );
			sprintf(str,"%3d lost",m_Animator->getLostDrops());
			m_pFont->DrawText( 2+o*40+80, 20, D3DCOLOR_ARGB(255,255,0,0), str );
			sprintf(str,"%3d merged",m_Animator->getMergedDrops());
			m_pFont->DrawText( 2+o*40+160, 20, D3DCOLOR_ARGB(255,255,0,0), str );
			sprintf(str,"mag=%f",m_fMag);
			m_pFont->DrawText( 2+o*40+260, 20, D3DCOLOR_ARGB(255,255,0,0), str );
			
			// help ?
			if( m_bShowHelp )
			{
				m_pFont->DrawText(  2, 60, D3DCOLOR_ARGB(255,0,0,0),
					_T("Keyboard controls:") );
				m_pFont->DrawText( 20, 80, D3DCOLOR_ARGB(255,0,0,0),
					_T("'o' 'p' zoom in/out\n")
					_T("'t' 'y' change magnification factor\n")
					);
			}
			else
			{
				m_pFont->DrawText(  2, 60, D3DCOLOR_ARGB(255,0,0,0), 
					_T("Press F1 for help") );
			}
		}
		
		m_pd3dDevice->EndScene();
	}
	
	if (m_bRecording)
		m_VCR.Snap();
	
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateWetAreas()
{
	// Create the map's surface
	CHECK_ERROR(m_pd3dDevice->CreateTexture(256,256,1,D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
		D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&(m_pWetAreasTexture),NULL));
	
	CHECK_ERROR(m_pWetAreasTexture->GetSurfaceLevel(0,&m_pWetAreasSurface));
	
	return S_OK;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//                           Texture Sprites
//*****************************************************************************
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateOffsetMap()
{
	for (int map=0;map<NB_OFFSET_MAP;map++)
	{
		// create the offset maps surfaces and texture objects
		if( FAILED( m_pd3dDevice->CreateTexture( m_OffsetMapRes[map], m_OffsetMapRes[map], 1, 0,
			D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &(m_pOffsetMap[map]), NULL ) ) )
		{
			MessageBox(NULL,"Offset Map creation failed !","Error",MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
	}
	ClearOffsetMap();
	UpdateOffsetMap();
	
	return S_OK;
}
//-----------------------------------------------------------------------------
void CMyD3DApplication::ClearOffsetMap()
{
	for (int map=0;map<NB_OFFSET_MAP;map++)
	{
		// fill in the offset map
		for( DWORD y0 = 0; y0 < m_OffsetMapRes[map]; y0++ )
		{
			for( DWORD x0 = 0; x0 < m_OffsetMapRes[map]; x0++ )
			{
				RefTexIJ2OffsetMapIJ(map,0,0, // (0,0) is an empty tile in the reference texture
					x0,y0,0.0,0.0,0.0,1.0,true);
			}
		}
		m_NbSpritesInOffsetMap[map]=0;
	}
}
//-----------------------------------------------------------------------------
// Compute the cell i,j of the offset map corresponding to the (u,v) texture
// coordinates
//-----------------------------------------------------------------------------
void CMyD3DApplication::IJFromUV(int map,double u,double v,int& i,int& j)
{
	double ugrid,vgrid;
	
	// we want (u,v) on tile center
	u-=0.5/m_OffsetMapRes[map];
	v-=0.5/m_OffsetMapRes[map];
	// ensure u and v are both > 0.0 (because of modulo operations)
	//   texture are cyclic with a period of 1.0: adding an integer to texture coordinates 
	//   does not change the final result
	if (u < 0.0)
		u=1.0+(u-(int)u);
	if (v < 0.0)
		v=1.0+(v-(int)v);
	
	// compute pos in grid
	ugrid=(u*m_OffsetMapRes[map]);
	vgrid=(v*m_OffsetMapRes[map]);
	i=(int)ugrid;
	j=(int)vgrid;
	i %= m_OffsetMapRes[map];
	j %= m_OffsetMapRes[map];
	if (i == m_OffsetMapRes[map]-1)
		i=0;
	if (j == m_OffsetMapRes[map]-1)
		j=0;
}
//-----------------------------------------------------------------------------
// Check if there is a sprite at (u,v) texture coordinates - all layers
//-----------------------------------------------------------------------------
bool CMyD3DApplication::Overlap(double u,double v)
{
	for (int o=0;o<NB_OFFSET_MAP;o++)
		if (Overlap(o,u,v))
			return (true);
		return (false);
}
//-----------------------------------------------------------------------------
// Check if there is a sprite at (u,v) texture coordinates - layer 'map'
//-----------------------------------------------------------------------------
bool CMyD3DApplication::Overlap(int map,double u,double v)
{
	int    i,j;
	
	IJFromUV(map,u,v,i,j);
	// Cell i,j
	if (m_OffsetMapOverlap[map][i+j*m_OffsetMapRes[map]])
		return (true);
	// Cell i+1,j
	if (m_OffsetMapOverlap[map][(i+1)+j*m_OffsetMapRes[map]])
		return (true);
	// Cell i,j+1
	if (m_OffsetMapOverlap[map][i+(j+1)*m_OffsetMapRes[map]])
		return (true);
	// Cell i+1,j+1
	if (m_OffsetMapOverlap[map][(i+1)+(j+1)*m_OffsetMapRes[map]])
		return (true);
	return (false);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Put a drop at (u,v) in the offset map 'map' with a given angle and scaling
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CMyD3DApplication::PutSprite(int map,double u,double v,double angle,double scale,bool force)
{
	double ugrid,vgrid;
	int    i,j;
	
	// check for overlapping
	if (Overlap(map,u,v) && !force)
		return (false);
	
	angle-=M_PI/2.0;
	// we want (u,v) on tile center
	u-=0.5/m_OffsetMapRes[map];
	v-=0.5/m_OffsetMapRes[map];
	// ensure u and v are both > 0.0 (because of modulo operations)
	//   texture are cyclic with a period of 1.0: adding an integer to texture coordinates 
	//   does not change the final result
	if (u < 0.0)
		u=1.0+(u-(int)u);
	if (v < 0.0)
		v=1.0+(v-(int)v);
	
	// compute pos in grid
	ugrid=(u*m_OffsetMapRes[map]);
	vgrid=(v*m_OffsetMapRes[map]);
	i=(int)ugrid;
	j=(int)vgrid;
	double du=ugrid-i;
	double dv=vgrid-j;
	// Cell i,j
	RefTexIJ2OffsetMapIJ(map,
		1,1, // drop is sprite at (1,1) in the reference texture
		i,j,
		du,dv,
		angle,scale);
	// Cell i+1,j
	RefTexIJ2OffsetMapIJ(map,
		1,1,
		i+1,j,
		du-1.0,dv,
		angle,scale);
	// Cell i,j+1
	RefTexIJ2OffsetMapIJ(map,
		1,1,
		i,j+1,
		du,dv-1.0,
		angle,scale);
	// Cell i+1,j+1
	RefTexIJ2OffsetMapIJ(map,
		1,1,
		i+1,j+1,
		du-1.0,dv-1.0,
		angle,scale);
	
	m_NbSpritesInOffsetMap[map]++;
	return (true);
}
//-----------------------------------------------------------------------------
// Compute the offset for sprite positionning: 
//    display sprite (si,sj) in cell (gi,gj) of the offset map 
//    with additional offset (delta_u,delta_v)
//-----------------------------------------------------------------------------
void CMyD3DApplication::RefTexIJ2OffsetMapIJ(int map,
											 int si,int sj,int gi,int gj,
											 double delta_u,double delta_v,
											 double angle,double scale,
											 bool clear)
{
	int     ti,tj;
	int     tu,tv;
	int     su,sv;
	int     du,dv;
	int     step;
	
	// step
	step=128/m_dwRefTexNbTiles;
	// coords
	gi %= m_OffsetMapRes[map];
	gj %= m_OffsetMapRes[map];
	ti=gi % m_dwRefTexNbTiles;
	tj=gj % m_dwRefTexNbTiles;
	tu=ti*step;
	tv=tj*step;
	su=si*step;
	sv=sj*step;
	du=(su-tu)-(int)(delta_u*(double)step);
	dv=(sv-tv)-(int)(delta_v*(double)step);
	// write to grid
	m_OffsetMap[map][(gi+gj*m_OffsetMapRes[map])*4  ]=(BYTE)du;
	m_OffsetMap[map][(gi+gj*m_OffsetMapRes[map])*4+1]=(BYTE)dv;
	m_OffsetMap[map][(gi+gj*m_OffsetMapRes[map])*4+2]=(BYTE)(angle*256.0/(2.0*M_PI));
	m_OffsetMap[map][(gi+gj*m_OffsetMapRes[map])*4+3]=(BYTE)(255.0*scale);
	
	m_OffsetMapOverlap[map][gi+gj*m_OffsetMapRes[map]]=!clear;
}
//-----------------------------------------------------------------------------
// Update offset map in video memory
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::UpdateOffsetMap()
{
	D3DLOCKED_RECT d3dlr;
	
	for (int map=0;map<NB_OFFSET_MAP;map++)
	{
		m_pOffsetMap[map]->LockRect( 0, &d3dlr, 0, 0 );
		
		BYTE* pDst = (BYTE *)d3dlr.pBits;
		
		for (UINT i=0;i<m_OffsetMapRes[map];i++)
		{
			for (UINT j=0;j<m_OffsetMapRes[map];j++)
			{
				pDst[i*4+j*d3dlr.Pitch+2]=m_OffsetMap[map][(i+j*m_OffsetMapRes[map])*4  ]; // r = du
				pDst[i*4+j*d3dlr.Pitch+1]=m_OffsetMap[map][(i+j*m_OffsetMapRes[map])*4+1]; // g = dv
				pDst[i*4+j*d3dlr.Pitch+0]=m_OffsetMap[map][(i+j*m_OffsetMapRes[map])*4+2]; // b = rotate
				pDst[i*4+j*d3dlr.Pitch+3]=m_OffsetMap[map][(i+j*m_OffsetMapRes[map])*4+3]; // a = scale
			}
		}
		m_pOffsetMap[map]->UnlockRect(0);
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//                                  Windows
//*****************************************************************************
//-----------------------------------------------------------------------------
// Move the viewpoint
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -m_dDist );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	
	D3DXVec3TransformCoord(&vFromPt,&vFromPt,m_ArcBall.GetRotationMatrix());
	D3DXVec3TransformCoord(&vFromPt,&vFromPt,m_ArcBall.GetTranslationMatrix());
	
	D3DXMatrixLookAtLH(&m_matView, &vFromPt, &vLookatPt, &vUpVec );
	
	D3DXMATRIX mat;
	D3DXMatrixRotationZ(&mat,m_dZAngle);
	D3DXMatrixMultiply(&m_matView,&mat,&m_matView);
	
	D3DXMatrixInverse(&m_matViewI, NULL, &m_matView);
	
	D3DXMatrixTranspose(&m_matView, &m_matView);
	D3DXMatrixTranspose(&m_matViewI, &m_matViewI);
	
	return S_OK;
}
//-----------------------------------------------------------------------------
// Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Pass mouse messages to the ArcBall so it can build internal matrices
	m_ArcBall.HandleMouseMessages( hWnd, uMsg, wParam, lParam );
	// Handle key presses
	switch( uMsg )
	{
		
	case WM_RBUTTONDOWN:
		
		break;
		
	case WM_RBUTTONUP:
		
		break;
		
	case WM_KEYDOWN:
		
		break;
		
	case WM_CHAR:
		if (wParam == 'o')
			m_dDist-=0.05;
		else if (wParam == 'p')
			m_dDist+=0.05;
		else if (wParam == 'k')
			m_dDist-=0.5;
		else if (wParam == 'l')
			m_dDist+=0.5;
		else if (wParam == 't')
			m_fMag-=0.01f;
		else if (wParam == 'y')
			m_fMag+=0.01f;
		else if (wParam == 'u')
			m_fView-=0.01f;
		else if (wParam == 'i')
			m_fView+=0.01f;
		else if (wParam == 'c')
			m_dZAngle-=M_PI/100.0;
		else if (wParam == 'v')
			m_dZAngle+=M_PI/100.0;
		else if (wParam == 'r')
		{
			if (!m_bRecording)
			{
				m_Animator->init();
				m_VCR.StartCapture(m_hWnd, m_dwCreationWidth,m_dwCreationHeight,"capture.avi");
				m_bRecording=true;
			}
			else
			{
				m_VCR.EndCapture();
				m_bRecording=false;
			}
		}
		else if (wParam == 'h')
			m_bHideText=!m_bHideText;
		else if (wParam == '\'')
		{
			RECT rect;

		    GetClientRect(m_hWnd,&rect);
			int w=rect.right-rect.left;
			int h=rect.bottom-rect.top;
			m_ScrCap.Snap(m_hWnd,w,h);
		}
		else if (wParam == 'n')
			m_bStopDrops=!m_bStopDrops;
		else
			m_Animator->interact(0,0,wParam);
		break;
		
	case WM_KEYUP:
		if (wParam != 'O' && wParam != 'P' && wParam != 'R')
			ReleaseCapture();		
		break;
		
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
            case IDM_WIREFRAME:
                m_bWireframe = !m_bWireframe;
                break;
            case IDM_RAIN_FALL:
				delete (m_Animator);
				m_Animator=new CDARain(this);
                m_Animator->init();
				break;
            case IDM_SHOWER:
				delete (m_Animator);
				m_Animator=new CDAShower(this);
                m_Animator->init();
				break;
            case IDM_SPLATCH:
				delete (m_Animator);
				m_Animator=new CDASplatch(this);
                m_Animator->init();
				break;
            case IDM_RESTART:
                m_Animator->init();
                break;
            case IDM_OBJECT:
                m_bCylinder=!m_bCylinder;
                break;
			case IDM_HIDEDROPS:
				m_bHideDrops=!m_bHideDrops;
				break;
			case IDM_STOPDROPS:
				m_bStopDrops=!m_bStopDrops;
				break;
            case IDM_RECORD:
				if (!m_bRecording)
				{
					m_bRecording=true;
					m_VCR.StartCapture(m_hWnd,m_dwCreationWidth,m_dwCreationHeight,"capture.avi");
					m_Animator->init();
				}
				else
				{
					m_VCR.EndCapture();
					m_bRecording=false;
				}
                break;
			case IDM_WET_AREAS:
				m_bShowWetAreas=!m_bShowWetAreas;
				break;
            case IDM_NEXT_TEX:
				{
					static int texn=1;
					switch (texn % 2)
					{
					case 0:
						loadUnderlyingTexture("grid.tga","grid_relief.tga",7.0);
						break;
					case 1:
						loadUnderlyingTexture("shaderx2.tga",NULL,7.0);
						break;
					}
					texn++;
				}
				break;
			case IDM_TOGGLEHELP:
				m_bShowHelp=!m_bShowHelp;
				break;
            case IDM_X2:
				m_fRepeat*=2.0;
                break;
            case IDM_DIV2:
				m_fRepeat/=2.0;
				m_fRepeat=max(1.0,m_fRepeat);
                break;
            case IDM_STOP:
                m_bStop = !m_bStop;
                break;
            case IDM_8x8:
				m_iSelectedOffsetMapRes=8;
				InvalidateDeviceObjects();
				RestoreDeviceObjects();
				break;
            case IDM_16x16:
				m_iSelectedOffsetMapRes=16;
				InvalidateDeviceObjects();
				RestoreDeviceObjects();
				break;
            case IDM_32x32:
				m_iSelectedOffsetMapRes=32;
				InvalidateDeviceObjects();
				RestoreDeviceObjects();
				break;
			}
			// Update the menus, in case any state changes occurred
			SetMenuStates();
		}
	}
	
	// Pass remaining messages to default handler
	return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}
//-----------------------------------------------------------------------------
// Update menu states
//-----------------------------------------------------------------------------
void CMyD3DApplication::SetMenuStates()
{
	HMENU hMenu = GetMenu( m_hWnd );
	
    CheckMenuItem( hMenu, IDM_WIREFRAME,
		m_bWireframe ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_UNIFORM,
		m_bUniform ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_STOP,
		m_bStop ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_8x8,
		m_iSelectedOffsetMapRes == 8 ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_16x16,
		m_iSelectedOffsetMapRes == 16 ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_32x32,
		m_iSelectedOffsetMapRes == 32 ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_RECORD,
		m_bRecording ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_HIDEDROPS,
		m_bHideDrops ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_STOPDROPS,
		m_bStopDrops ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_WET_AREAS,
		m_bShowWetAreas ? MF_CHECKED : MF_UNCHECKED );
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	m_pFont->InvalidateDeviceObjects();
	
	for (int o=0;o<NB_OFFSET_MAP;o++)
		SAFE_RELEASE(m_pOffsetMap[o]);
	
	SAFE_RELEASE(m_pColorMapTexture);
	SAFE_RELEASE(m_pDropNormalTexture);
	SAFE_RELEASE(m_pNormalMapTexture);
	SAFE_RELEASE(m_PointVB);
	SAFE_RELEASE(m_pWetAreasSurface);
	SAFE_RELEASE(m_pWetAreasTexture);
	SAFE_RELEASE(m_pSplashTexture);
	
	SAFE_DELETE(m_Quad);
	SAFE_DELETE(m_Cyl);
	
	// destroy cg context
    cgDestroyContext(m_cgContext);
    cgD3D9SetDevice(NULL);
	
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
										 D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	if( FAILED( m_pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		adapterFormat, D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
		D3DRTYPE_SURFACE, backBufferFormat ) ) )
	{
		return E_FAIL;
	}
	
	if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
	{
		if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
			return E_FAIL;
	}
	
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	// Init font
	m_pFont->InitDeviceObjects( m_pd3dDevice );
	
	SetMenuStates();
	
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	return (Init());
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pFont->DeleteDeviceObjects();
	
	for (int o=0;o<NB_OFFSET_MAP;o++)
	{
		if (m_pOffsetMap[o] != NULL)
			SAFE_RELEASE( m_pOffsetMap[o]);
		delete [] (m_OffsetMap[o]);
		delete [] (m_OffsetMapOverlap[o]);
	}
	if (m_pWetAreasTexture != NULL)
		SAFE_RELEASE(m_pWetAreasTexture);
	if (m_pWetAreasSurface != NULL)
		SAFE_RELEASE(m_pWetAreasSurface);
	
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	SAFE_DELETE( m_pFont );
	return S_OK;
}
//-----------------------------------------------------------------------------

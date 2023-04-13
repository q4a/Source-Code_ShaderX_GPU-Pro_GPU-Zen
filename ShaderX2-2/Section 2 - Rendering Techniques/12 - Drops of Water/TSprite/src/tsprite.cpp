//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
// TSprite.cpp
//
//    Texture Sprites demo source code
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
//         Built on top of a Direct3D SDK sample code (DirectX9).
//         Uses DirectX 9 HLSL - see TSprite.fx
//
//
//    Dependencies:
//    -------------
//
//         DirectX 9 SDK
//
//    Hardware requirements:
//    ----------------------
// 
//         ps 1.3 | ps 1.4 | ps 2.0
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <commctrl.h>
#include <math.h>
#include <stdio.h>
#include <D3DX9.h>
#include <iostream>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "D3DFile.h"
#include "resource.h"
//-----------------------------------------------------------------------------
// D3D vertex format
//-----------------------------------------------------------------------------
struct D3DVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	FLOAT       tu1, tv1;
	FLOAT       tu2, tv2;
	
	static const DWORD FVF;
};
// uses two texture coordinates
const DWORD D3DVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;
//-----------------------------------------------------------------------------
// struct for picking
//-----------------------------------------------------------------------------
struct INTERSECTION
{
	DWORD dwFace;                 // mesh face that was intersected
	FLOAT fBary1, fBary2;         // barycentric coords of intersection
	FLOAT fDist;                  // distance from ray origin to intersection
	FLOAT tu, tv;                 // texture coords of intersection
};

#define MAX_INTERSECTIONS 16
//-----------------------------------------------------------------------------
// Application class
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
protected:
	CD3DArcBall             m_ArcBall;
	double                  m_dDist;
	CD3DFont               *m_pFont;
	CD3DMesh               *m_pObject;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	bool                    m_bWireframe;
	int						m_iTech;
	LPDIRECT3DTEXTURE9      m_pTexture;
	LPD3DXEFFECT            m_pEffect;
	D3DXHANDLE              m_hTechnique;
	D3DXHANDLE              m_hTexture;
	D3DXHANDLE              m_hWorldMatrix;
	D3DXHANDLE              m_hViewMatrix;
	D3DXHANDLE              m_hProjectionMatrix;
	double                  m_LastSpritePos[4];
	int                     m_LastSpriteIndex[2];
	bool                    m_bShowHelp;
	
	// Texture Sprites
	BYTE                   *m_OffsetMap;
	LPDIRECT3DTEXTURE9      m_pOffsetMap;
	D3DXHANDLE              m_hOffsetMap;
	D3DXHANDLE              m_hOffsetMapRes;
	D3DXHANDLE              m_hRefTexNbTiles;
	UINT                    m_dwOffsetMapRes;
	UINT                    m_dwRefTexNbTiles;
	
protected:
	
	HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );
	HRESULT Render();
	HRESULT FrameMove();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects(); 
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();
	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	VOID    SetMenuStates();
	
	// Texture Sprites
	HRESULT CreateOffsetMap(bool clear);
	VOID    ClearOffsetMap();
	HRESULT UpdateOffsetMap();
	void    PutSprite(int si,int sj,double u,double v,double angle,double scale);
	void    RefTexIJ2OffsetMapIJ(int si,int sj,int gi,int gj,
		double decalx,double decaly,
		double angle,double scale);
	
	// Picking
	// -> callback
    void    OnIntersect(INTERSECTION *);
	// -> pick polygon under mouse cursor
	HRESULT Pick();
	
public:
	
	CMyD3DApplication();
};
//-----------------------------------------------------------------------------
// WinMain
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
	// inherited
	m_strWindowTitle    = _T("Texture Sprites");
	m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
	
	// intrinsic
	m_pFont                = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
	m_pObject              = new CD3DMesh();
	
	m_pVB             = NULL;
	m_pTexture		  = NULL;
	m_bWireframe      = false;
	m_dDist           = -12.0;
	m_bShowHelp       = false;
	
	m_LastSpritePos[0]=-1.0;
	m_LastSpritePos[1]=-1.0;
	m_LastSpritePos[2]=0.0;
	m_LastSpritePos[3]=1.0;
	m_LastSpriteIndex[0]=1;
	m_LastSpriteIndex[1]=1;
	
	// Texture Sprites
	m_pOffsetMap	  = NULL;
	m_dwOffsetMapRes  = 16; // can be changed to any power of 2
	m_dwRefTexNbTiles = 4; // 4 tiles in reference texture (see ref_tex.bmp)
	// offset map in local memory
	m_OffsetMap       = new BYTE[m_dwOffsetMapRes*m_dwOffsetMapRes*4];
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//                           Texture Sprites
//*****************************************************************************
//-----------------------------------------------------------------------------
// Create the offset map
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateOffsetMap(bool clear)
{
	// release old offset map
	if (NULL != m_pOffsetMap)
		SAFE_RELEASE(m_pOffsetMap);
	// create the offset map texture in video memory
	switch (m_iTech)
	{
	case 14:
	case 20:
		if( FAILED( m_pd3dDevice->CreateTexture( m_dwOffsetMapRes, m_dwOffsetMapRes, 1, 0,
			D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pOffsetMap, NULL ) ) )
		{
			MessageBox(NULL,"Offset Map creation failed !","Error",MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
		break;
	case 13:
		if( FAILED( m_pd3dDevice->CreateTexture( m_dwOffsetMapRes, m_dwOffsetMapRes, 1, 0,
			D3DFMT_V8U8, D3DPOOL_MANAGED, &m_pOffsetMap, NULL ) ) )
		{
			MessageBox(NULL,"Offset Map creation failed !","Error",MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
		break;
	}
	// clear offset map if aksed
	if (clear)
		ClearOffsetMap();
	else
		UpdateOffsetMap();
	
	return S_OK;
}
//-----------------------------------------------------------------------------
// Clear the offset map
//-----------------------------------------------------------------------------
void CMyD3DApplication::ClearOffsetMap( )
{
	// fill in the offset map
	for( DWORD y0 = 0; y0 < m_dwOffsetMapRes; y0++ )
	{
		for( DWORD x0 = 0; x0 < m_dwOffsetMapRes; x0++ )
		{
			RefTexIJ2OffsetMapIJ(0,0,x0,y0,0.0,0.0,0.0,1.0);
		}
	}
	UpdateOffsetMap();
}
//-----------------------------------------------------------------------------
// Put the sprite si,sj at (u,v) in the texture using the specified angle and scaling
//-----------------------------------------------------------------------------
void CMyD3DApplication::PutSprite(int si,int sj,double u,double v,double angle,double scale)
{
	double ugrid,vgrid;
	double du,dv;
	int    gi,gj;
	
	// we want (u,v) on tile center
	u-=0.5/m_dwOffsetMapRes;
	v-=0.5/m_dwOffsetMapRes;
	// ensure u and v are both > 0.0 (because of modulo operations)
	//   texture are cyclic with a period of 1.0: 
	//   adding an integer to texture coordinates 
	//   does not change the final result
	if (u < 0.0)
		u=1.0+(u-(int)u);
	if (v < 0.0)
		v=1.0+(v-(int)v);
	// compute pos in offset map
	ugrid=(u*m_dwOffsetMapRes);
	vgrid=(v*m_dwOffsetMapRes);
	// compute offset map cell index
	gi=(int)ugrid;
	gj=(int)vgrid;
	// compute texture coordinates relative to the cell
	du=ugrid-gi;
	dv=vgrid-gj;
	// cell i,j
	RefTexIJ2OffsetMapIJ(si,sj,
		gi,gj,
		du,dv,
		angle,scale);
	// cell i+1,j
	RefTexIJ2OffsetMapIJ(si,sj,
		gi+1,gj,
		du-1.0,dv,
		angle,scale);
	// cell i,j+1
	RefTexIJ2OffsetMapIJ(si,sj,
		gi,gj+1,
		du,dv-1.0,
		angle,scale);
	// cell i+1,j+1
	RefTexIJ2OffsetMapIJ(si,sj,
		gi+1,gj+1,
		du-1.0,dv-1.0,
		angle,scale);
	// update offset map in video memory
	UpdateOffsetMap();
}
//-----------------------------------------------------------------------------
// Compute the offset for sprite positionning: 
//    display sprite (si,sj) in cell (gi,gj) of the offset map 
//    with additional offset (delta_u,delta_v)
//-----------------------------------------------------------------------------
void CMyD3DApplication::RefTexIJ2OffsetMapIJ(int si,int sj,int gi,int gj,
											 double delta_u,double delta_v,
											 double angle,double scale)
{
	int     ti,tj;
	double  du,dv;
	int     step,idu,idv;
	
	// ensure gi,gj are in grid bounds
	gi %= m_dwOffsetMapRes;
	gj %= m_dwOffsetMapRes;
	// compute what tile would be here if we were not using an offset map
	ti=gi % m_dwRefTexNbTiles;
	tj=gj % m_dwRefTexNbTiles;
	// compute offset to apply in this cell
	du=(si-ti) - delta_u;
	dv=(sj-tj) - delta_v;
	// encoding du,dv as 8 bit integers (low precision !)
	step=128 / m_dwRefTexNbTiles;
	idu=(int)(du*step);
	idv=(int)(dv*step);
	// write to grid
	m_OffsetMap[(gi+gj*m_dwOffsetMapRes)*4  ]=(BYTE)idu;
	m_OffsetMap[(gi+gj*m_dwOffsetMapRes)*4+1]=(BYTE)idv;
	// transformation data in blue and alpha channel of offset map
	m_OffsetMap[(gi+gj*m_dwOffsetMapRes)*4+2]=(BYTE)(angle*256.0/360.0);
	m_OffsetMap[(gi+gj*m_dwOffsetMapRes)*4+3]=(BYTE)(255.0*scale);
}
//-----------------------------------------------------------------------------
// Update offset map in video memory
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::UpdateOffsetMap()
{
	D3DLOCKED_RECT d3dlr;
	
	m_pOffsetMap->LockRect( 0, &d3dlr, 0, 0 );
	
	BYTE* pDst = (BYTE *)d3dlr.pBits;
	
	for (UINT i=0;i<m_dwOffsetMapRes;i++)
	{
		for (UINT j=0;j<m_dwOffsetMapRes;j++)
		{
			switch (m_iTech)
			{
			case 14:
			case 20:
				pDst[i*4+j*d3dlr.Pitch+2]=m_OffsetMap[(i+j*m_dwOffsetMapRes)*4  ]; // r = du
				pDst[i*4+j*d3dlr.Pitch+1]=m_OffsetMap[(i+j*m_dwOffsetMapRes)*4+1]; // g = dv
				pDst[i*4+j*d3dlr.Pitch+0]=m_OffsetMap[(i+j*m_dwOffsetMapRes)*4+2]; // b = rotate
				pDst[i*4+j*d3dlr.Pitch+3]=m_OffsetMap[(i+j*m_dwOffsetMapRes)*4+3]; // a = scale
				break;
			case 13:
				pDst[i*2+j*d3dlr.Pitch+1]=m_OffsetMap[(i+j*m_dwOffsetMapRes)*4  ]; // r = du
				pDst[i*2+j*d3dlr.Pitch  ]=m_OffsetMap[(i+j*m_dwOffsetMapRes)*4+1]; // g = dv
				break;
			default:
				MessageBox(NULL,"Cannot run on your hardware - sorry","Error",MB_OK | MB_ICONSTOP);
			}
		}
	}
	m_pOffsetMap->UnlockRect(0);
	
	return S_OK;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//                              Direct3D
//*****************************************************************************
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
										 D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	if( FAILED( m_pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		adapterFormat, 0, D3DRTYPE_SURFACE, backBufferFormat ) ) )
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
	HRESULT hr;
	TCHAR strPath[MAX_PATH];
	
	m_pFont->InitDeviceObjects( m_pd3dDevice );
	
	// load the object
	if( FAILED( m_pObject->Create( m_pd3dDevice, _T("cube.x") ) ) )
		return D3DAPPERR_MEDIANOTFOUND;
	m_pObject->SetFVF( m_pd3dDevice, D3DVERTEX::FVF );
	m_pObject->UseMeshMaterials(false);
	
	// load the effect file
	if( FAILED( hr = DXUtil_FindMediaFileCb( strPath, sizeof(strPath), TEXT("tsprite.fx") ) ) )
		return hr;
	if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, strPath, NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, NULL) ) )
		return hr;
	
	// save frequently used handles
	
	// -> find highest usable technique
	bool none=true;
	HMENU hMenu = GetMenu( m_hWnd );
	
	if (m_pEffect->ValidateTechnique(m_pEffect->GetTechniqueByName(TEXT("TSprite1_3"))) == D3D_OK)
	{
		m_hTechnique = m_pEffect->GetTechniqueByName(TEXT("TSprite1_3"));
		m_iTech = 13;
		none=false;
	}
	else
		EnableMenuItem(hMenu,IDM_T13,MF_DISABLED);
	
	if (m_pEffect->ValidateTechnique(m_pEffect->GetTechniqueByName(TEXT("TSprite1_4"))) == D3D_OK)
	{
		m_hTechnique = m_pEffect->GetTechniqueByName(TEXT("TSprite1_4"));
		m_iTech = 14;
		none=false;
	}
	else
		EnableMenuItem(hMenu,IDM_T14,MF_DISABLED);
	
	if (m_pEffect->ValidateTechnique(m_pEffect->GetTechniqueByName(TEXT("TSprite2_0"))) == D3D_OK)
	{
		m_hTechnique = m_pEffect->GetTechniqueByName(TEXT("TSprite2_0"));
		m_iTech = 20;
		none=false;
	}
	else
		EnableMenuItem(hMenu,IDM_T20,MF_DISABLED);
	
	if (none)
	{
		MessageBox(NULL,"Your hardware does not support any of the available implementations. Sorry.","Error",MB_OK | MB_ICONSTOP);
		exit (-1);
	}
	
	// get HLSL shader parameters
	m_hTexture          = m_pEffect->GetParameterByName( NULL, TEXT("Tex0") );
	m_hOffsetMap        = m_pEffect->GetParameterByName( NULL, TEXT("Off0") );
	m_hOffsetMapRes     = m_pEffect->GetParameterByName( NULL, TEXT("OffsetMapRes") );
	m_hRefTexNbTiles    = m_pEffect->GetParameterByName( NULL, TEXT("RefTexNbTiles") );
	m_hWorldMatrix      = m_pEffect->GetParameterBySemantic( NULL, TEXT("WORLD") );
	m_hViewMatrix       = m_pEffect->GetParameterBySemantic( NULL, TEXT("VIEW") );
	m_hProjectionMatrix = m_pEffect->GetParameterBySemantic( NULL, TEXT("PROJECTION") );
	
	// create the offset map texture
	if( FAILED( CreateOffsetMap(true) ) )
		return E_FAIL;
	
	// make sure the render pass options are set in the menu
	SetMenuStates();
	
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	WORD* pBaseIndex = NULL;
	
	m_pFont->RestoreDeviceObjects();
	
	// set ArcBall parameters
	m_ArcBall.SetWindow( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 2.0f );
	m_ArcBall.SetRadius( 10.0f );
	
	if( FAILED( D3DUtil_CreateTexture( m_pd3dDevice, "leaves.bmp", 
		&m_pTexture, D3DFMT_A8R8G8B8 ) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	
    // restore device-memory objects for the mesh
	m_pObject->RestoreDeviceObjects(m_pd3dDevice);
	
	// create the vertex buffer for picking
	DWORD dwNumVertices = MAX_INTERSECTIONS * 3;
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( dwNumVertices*sizeof(D3DVERTEX),
		D3DUSAGE_WRITEONLY, D3DVERTEX::FVF,
		D3DPOOL_MANAGED, &m_pVB, NULL ) ) )
		return E_FAIL;
	
	// set miscellaneous render states
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x00444444 );
	
	// set the world matrix
	D3DXMATRIXA16 matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	m_pEffect->SetMatrix( m_hWorldMatrix, &matIdentity );
	
	// set the projection matrix
	D3DXMATRIXA16 matProj;
	FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	m_pEffect->SetMatrix( m_hProjectionMatrix, &matProj );
	
	// warn effect
	if( m_pEffect != NULL )
		m_pEffect->OnResetDevice();
	
	// update menus
	SetMenuStates();
	
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	m_pFont->InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVB );
	if( m_pTexture )
	{
		SAFE_RELEASE( m_pTexture );
	}
	
	if( m_pEffect != NULL )
		m_pEffect->OnLostDevice();
	
	m_pObject->InvalidateDeviceObjects();
	
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pFont->DeleteDeviceObjects();
	SAFE_RELEASE( m_pEffect );
	if (m_pOffsetMap != NULL)
		SAFE_RELEASE( m_pOffsetMap );
	m_pObject->Destroy();
	if (m_OffsetMap != NULL)
		delete [] (m_OffsetMap);
	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pObject);
	return S_OK;
}
//-----------------------------------------------------------------------------
// Move the viewpoint
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	D3DXMATRIXA16 matView;
	
	D3DXVECTOR3 vFromPt   = D3DXVECTOR3( m_dDist, 0.0f, 0.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	
	D3DXVec3TransformCoord(&vFromPt,&vFromPt,m_ArcBall.GetRotationMatrix());
	D3DXVec3TransformCoord(&vFromPt,&vFromPt,m_ArcBall.GetTranslationMatrix());
	
	D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	m_pEffect->SetMatrix( m_hViewMatrix, &matView );
	
	return S_OK;
}
//-----------------------------------------------------------------------------
// Render the scene
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	
	// picking
	Pick();
	
	// clear
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		0x000000ff, 1.0f, 0L );
	
	// render
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		UINT numPasses;
		
		// draw solid object
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		// using the Texture Sprites effect
		m_pEffect->SetTechnique( m_hTechnique );
		m_pEffect->Begin( &numPasses, 0 );
		for( UINT iPass = 0; iPass < numPasses; iPass++ )
		{
			m_pEffect->Pass( iPass );
			m_pEffect->SetTexture( m_hTexture, m_pTexture );
			m_pEffect->SetTexture( m_hOffsetMap, m_pOffsetMap );
			m_pEffect->SetFloat( m_hOffsetMapRes, m_dwOffsetMapRes );
			m_pEffect->SetFloat( m_hRefTexNbTiles, m_dwRefTexNbTiles );
			// render the mesh
			m_pObject->Render( m_pd3dDevice );
		}
		m_pEffect->End();
		
		if (m_bWireframe)
		{
			// draw wireframe
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			m_pObject->Render( m_pd3dDevice );
		}
		
		// Dump out the FPS
		m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
		m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );
		
		if( m_bShowHelp )
		{
			m_pFont->DrawText(  2, 60, D3DCOLOR_ARGB(255,0,0,0),
				_T("Keyboard controls:") );
			m_pFont->DrawText( 20, 80, D3DCOLOR_ARGB(255,0,0,0),
				_T("'o' 'p' zoom in/out\n")
				_T("'n'     next sprite\n")
				_T("'r'     clear offset map\n")
				_T("'+' '-' rotate sprite\n")
				_T("'[' ']' scale sprite\n")
				_T("SPACE   put sprite under mouse\n" ));
		}
		else
		{
			m_pFont->DrawText(  2, 60, D3DCOLOR_ARGB(255,0,0,0), 
				_T("Press F1 for help") );
		}
		
		m_pd3dDevice->EndScene();
	}
	
	return S_OK;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//                                  Windows
//*****************************************************************************
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
		else if (wParam == 'r')
		{
			ClearOffsetMap();
			// no last sprite
			m_LastSpritePos[0]=-1;
		}
		else if (wParam == '+')
		{
			// rotate last sprite
			if (m_LastSpritePos[0] > 0.0)
			{
				m_LastSpritePos[2]+=10;
				
				PutSprite(m_LastSpriteIndex[0],m_LastSpriteIndex[1],m_LastSpritePos[0],m_LastSpritePos[1],m_LastSpritePos[2],m_LastSpritePos[3]);
			}
		}
		else if (wParam == '-')
		{
			// rotate last sprite
			if (m_LastSpritePos[0] > 0.0)
			{
				m_LastSpritePos[2]-=10;
				
				PutSprite(m_LastSpriteIndex[0],m_LastSpriteIndex[1],m_LastSpritePos[0],m_LastSpritePos[1],m_LastSpritePos[2],m_LastSpritePos[3]);
			}
		}
		else if (wParam == ']')
		{
			if (m_LastSpritePos[0] > 0.0)
			{
				// scale last sprite
				if (m_LastSpritePos[3] < 1.0)
				{
					m_LastSpritePos[3]*=1.1;
					if (m_LastSpritePos[3] > 1.0)				
						m_LastSpritePos[3]=1.0;
					PutSprite(m_LastSpriteIndex[0],m_LastSpriteIndex[1],m_LastSpritePos[0],m_LastSpritePos[1],m_LastSpritePos[2],m_LastSpritePos[3]);
				}
			}
		}
		else if (wParam == '[')
		{
			// scale last sprite
			if (m_LastSpritePos[0] > 0.0)
			{
				m_LastSpritePos[3]*=0.9;
				PutSprite(m_LastSpriteIndex[0],m_LastSpriteIndex[1],m_LastSpritePos[0],m_LastSpritePos[1],m_LastSpritePos[2],m_LastSpritePos[3]);
			}
		}
		else if (wParam == 'n')
		{
			// use next sprite in reference texture
			static int sprt=1;
			if (m_LastSpritePos[0] > 0.0)
			{
				m_LastSpriteIndex[0]=(1 + sprt*2) % m_dwRefTexNbTiles;
				m_LastSpriteIndex[1]=(1 + ((int)(sprt/2))*2) % m_dwRefTexNbTiles;
				PutSprite(m_LastSpriteIndex[0],m_LastSpriteIndex[1],m_LastSpritePos[0],m_LastSpritePos[1],m_LastSpritePos[2],m_LastSpritePos[3]);
				sprt++;
			}
		}
		else
			SetCapture( hWnd );
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
            case IDM_T13:
				m_hTechnique=m_pEffect->GetTechniqueByName( TEXT("TSprite1_3") );
				m_iTech=13;
				CreateOffsetMap(false);
				break;
            case IDM_T14:
				m_hTechnique=m_pEffect->GetTechniqueByName( TEXT("TSprite1_4") );
				m_iTech=14;
				CreateOffsetMap(false);
				break;
            case IDM_T20:
				m_hTechnique=m_pEffect->GetTechniqueByName( TEXT("TSprite2_0") );
				m_iTech=20;
				CreateOffsetMap(false);
				break;
			case IDM_TOGGLEHELP:
				m_bShowHelp=!m_bShowHelp;
				break;
			case IDM_LEAVES:
				SAFE_RELEASE(m_pTexture);
				D3DUtil_CreateTexture( m_pd3dDevice, "leaves.bmp", &m_pTexture, D3DFMT_A8R8G8B8);
				break;
			case IDM_FLOWER:
				SAFE_RELEASE(m_pTexture);
				D3DUtil_CreateTexture( m_pd3dDevice, "flower.bmp", &m_pTexture, D3DFMT_A8R8G8B8);
				break;
			case IDM_8X8:
				m_dwOffsetMapRes=8;
				// delete old map
				delete [](m_OffsetMap);
				// create the new offset map
				m_OffsetMap = new BYTE[m_dwOffsetMapRes*m_dwOffsetMapRes*4];
				CreateOffsetMap(true);
				break;
			case IDM_16X16:
				m_dwOffsetMapRes=16;
				// delete old map
				delete [](m_OffsetMap);
				// create the new offset map
				m_OffsetMap = new BYTE[m_dwOffsetMapRes*m_dwOffsetMapRes*4];
				CreateOffsetMap(true);
				break;
			case IDM_32X32:
				m_dwOffsetMapRes=32;
				// delete old map
				delete [](m_OffsetMap);
				// create the new offset map
				m_OffsetMap = new BYTE[m_dwOffsetMapRes*m_dwOffsetMapRes*4];
				CreateOffsetMap(true);
				break;
			case IDM_OPENFILE:
				TCHAR g_strFilename[512]   = _T("");
				
				// Display the OpenFileName dialog. Then, try to load the specified file
				OPENFILENAME ofn = { sizeof(OPENFILENAME), NULL, NULL,
					_T(".X Files (.x)\0*.x\0\0"), 
					NULL, 0, 1, NULL, 0, g_strFilename, 512, 
					".", _T("Open Mesh File"), 
					OFN_FILEMUSTEXIST, 0, 1, NULL, 0, NULL, NULL };
				
				if( TRUE == GetOpenFileName( &ofn ) )
				{
					// Load the object
					m_pObject->Destroy();
					SAFE_DELETE(m_pObject);
					m_pObject = new CD3DMesh();
					if( FAILED( m_pObject->Create( m_pd3dDevice, _T(g_strFilename) ) ) )
					{
						m_pObject->Create( m_pd3dDevice,"cube.x");
						MessageBox(NULL,"Unable to load mesh file","Error",MB_OK | MB_ICONSTOP);
					}
					m_pObject->SetFVF( m_pd3dDevice, D3DVERTEX::FVF );
					m_pObject->UseMeshMaterials(false);
					m_pObject->RestoreDeviceObjects(m_pd3dDevice);
				}
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
void CMyD3DApplication::SetMenuStates()
{
	HMENU hMenu = GetMenu( m_hWnd );
	
    CheckMenuItem( hMenu, IDM_WIREFRAME,
		m_bWireframe ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_T13,
		m_iTech == 13 ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_T14,
		m_iTech == 14 ? MF_CHECKED : MF_UNCHECKED );
    CheckMenuItem( hMenu, IDM_T20,
		m_iTech == 20 ? MF_CHECKED : MF_UNCHECKED );
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//                                  Picking
//*****************************************************************************
//-----------------------------------------------------------------------------
void CMyD3DApplication::OnIntersect(INTERSECTION *hit)
{
	PutSprite(m_LastSpriteIndex[0],m_LastSpriteIndex[1],hit->tu,hit->tv,0.0,1.0);
	m_LastSpritePos[0]=hit->tu;
	m_LastSpritePos[1]=hit->tv;
	m_LastSpritePos[2]=0.0;
	m_LastSpritePos[3]=1.0;
	UpdateOffsetMap();
}
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Pick()
{
	D3DXVECTOR3  vPickRayDir;
	D3DXVECTOR3  vPickRayOrig;
	INTERSECTION intersection;
	
	int numIntersect = 0L;
	
	// Get the pick ray from the mouse position
	if( GetCapture() )
	{
		D3DXMATRIXA16 matProj;
		
		m_pEffect->GetMatrix( m_hProjectionMatrix, &matProj );
		//		if (m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj ) != D3D_OK)
		//			MessageBox(NULL,"GetTransform - d3d error !","Error",MB_OK | MB_ICONERROR);
		
		POINT ptCursor;
		GetCursorPos( &ptCursor );
		ScreenToClient( m_hWnd, &ptCursor );
		
		// Compute the vector of the pick ray in screen space
		D3DXVECTOR3 v;
		v.x =  ( ( ( 2.0f * ptCursor.x ) / m_d3dsdBackBuffer.Width  ) - 1 ) / matProj._11;
		v.y = -( ( ( 2.0f * ptCursor.y ) / m_d3dsdBackBuffer.Height ) - 1 ) / matProj._22;
		v.z =  1.0f;
		
		// Get the inverse view matrix
		D3DXMATRIXA16 matView, m;
		//		if (m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView ))
		//			MessageBox(NULL,"GetTransform - d3d error !","Error",MB_OK | MB_ICONERROR);
		m_pEffect->GetMatrix( m_hViewMatrix, &matView );
		D3DXMatrixInverse( &m, NULL, &matView );
		
		// Transform the screen space pick ray into 3D space
		vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		vPickRayOrig.x = m._41;
		vPickRayOrig.y = m._42;
		vPickRayOrig.z = m._43;
		
		// Get the picked triangle
		LPD3DXBASEMESH          pMesh = m_pObject->GetLocalMesh();
		LPDIRECT3DVERTEXBUFFER9 pVB;
		LPDIRECT3DINDEXBUFFER9  pIB;
		
		pMesh->GetVertexBuffer( &pVB );
		pMesh->GetIndexBuffer( &pIB );
		
		WORD*      pIndices;
		D3DVERTEX*    pVertices;
		
		pIB->Lock( 0, 0, (void**)&pIndices, 0 );
		pVB->Lock( 0, 0, (void**)&pVertices, 0 );
		
		// When calling D3DXIntersect, one can get just the closest intersection and not
		// need to work with a D3DXBUFFER.  Or, to get all intersections between the ray and 
		// the mesh, one can use a D3DXBUFFER to receive all intersections.  We show both
		// methods.
		
		// Collect only the closest intersection
		BOOL bHit;
		DWORD dwFace;
		FLOAT fBary1, fBary2, fDist;
		D3DXIntersect(pMesh, &vPickRayOrig, &vPickRayDir, &bHit, &dwFace, &fBary1, &fBary2, &fDist, NULL, NULL);
		if (bHit)
		{
			numIntersect = 1;
			intersection.dwFace = dwFace;
			intersection.fBary1 = fBary1;
			intersection.fBary2 = fBary2;
			intersection.fDist = fDist;
		}
		else
		{
			numIntersect = 0;
		}
		// Now, for the intersection, add a triangle to m_pVB and compute texture coordinates
		if( numIntersect > 0 )
		{
			D3DVERTEX* v;
			D3DVERTEX* vThisTri;
			WORD* iThisTri;
			D3DVERTEX  v1, v2, v3;
			INTERSECTION* pIntersection;
			
			if (m_pVB != NULL)
			{
				m_pVB->Lock( 0, 0, (void**)&v, 0 );	
				pIntersection = &intersection;
				
				vThisTri = &v[0];
				iThisTri = &pIndices[3*pIntersection->dwFace];
				// get vertices hit
				vThisTri[0] = pVertices[iThisTri[0]];
				vThisTri[1] = pVertices[iThisTri[1]];
				vThisTri[2] = pVertices[iThisTri[2]];
				
				// If all you want is the vertices hit, then you are done.  In this sample, we
				// want to show how to infer texture coordinates as well, using the BaryCentric
				// coordinates supplied by D3DXIntersect
				FLOAT dtu1 = vThisTri[1].tu1 - vThisTri[0].tu1;
				FLOAT dtu2 = vThisTri[2].tu1 - vThisTri[0].tu1;
				FLOAT dtv1 = vThisTri[1].tv1 - vThisTri[0].tv1;
				FLOAT dtv2 = vThisTri[2].tv1 - vThisTri[0].tv1;
				pIntersection->tu = vThisTri[0].tu1 + pIntersection->fBary1 * dtu1 + pIntersection->fBary2 * dtu2;
				pIntersection->tv = vThisTri[0].tv1 + pIntersection->fBary1 * dtv1 + pIntersection->fBary2 * dtv2;
				m_pVB->Unlock();
			}
		}
		
		pVB->Unlock();
		pIB->Unlock();
		
		pVB->Release();
		pIB->Release();
		
	}
	
	if (numIntersect > 0)			
		// Call on intersect
		OnIntersect(&intersection);
	
	return S_OK;
}

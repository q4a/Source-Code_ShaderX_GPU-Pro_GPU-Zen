//-----------------------------------------------------------------------------
// File: ExampleBase.h
//
// Desc: Header file for example base stuff
//-----------------------------------------------------------------------------
#pragma once

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


//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
const int	C_HEIGHTMAP_SIZE	= 1025;
const int	C_STEP_SIZE			= 16;
const float	C_AUTO_CAM_TIME		= 999.0f;

//-----------------------------------------------------------------------------
// Vertex format for terrain vertex buffer
//-----------------------------------------------------------------------------
struct TERRAINVERTEX
{
    D3DXVECTOR3 pos;		// vertex position
	DWORD		color;		// vertex color
	float u,v;				// texture coords
};
#define D3DFVF_TERRAINVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2)




//-----------------------------------------------------------------------------
// Struct to store the current input state
//-----------------------------------------------------------------------------
struct UserInput
{
	BOOL bToggleScrPos;
	BOOL bToggleCam;
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
	BOOL bForward;
	BOOL bBackward;
};



//-----------------------------------------------------------------------------
// Name: class CBaseD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the
//       generic functionality needed in all Direct3D samples. CBaseD3DApplication
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CBaseD3DApplication : public CD3DApplication
{
    CD3DFont*               m_pFont;                // Font for drawing text
    UserInput               m_UserInput;            // Struct for storing user input

	int							m_nPolyCount;			// Terrain polys
    BOOL						m_bLoadingApp;          // TRUE, if the app is loading
	BYTE*						m_pHeightMap;			// Terrain heightmap
    LPDIRECT3DVERTEXBUFFER9		m_pVBTerrain;            // Vextex buffer terrain
	LPDIRECT3DTEXTURE9 			m_texTerrain;			// Terrain texture
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration;	// Vertex declaration


	BOOL					m_bAutoCam;				// Toggle flag for auto cam
	BOOL					m_bScrPos;
	BOOL					m_szKeys[256];

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT PreRender();
    virtual HRESULT Render();
    virtual HRESULT PostRender();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

	virtual HRESULT CreateShaders();
	virtual HRESULT InitializeFog();
	virtual HRESULT SetShaderConstants();

	
    HRESULT RenderText();
	HRESULT	CalcMovement();
	HRESULT CalcAutoCam();
	HRESULT SetScrShotPos();
	HRESULT	createTerrainPatch(LPDIRECT3DVERTEXBUFFER9 vb, int xOffset, int yOffset);
	BYTE	getHeight(int x, int y);
    void    UpdateInput( UserInput* pUserInput );

    D3DXMATRIX				m_matView;				// ViewMatrix
    D3DXMATRIX				m_matProj;				// ProjectionMatrix
    D3DXMATRIX				m_matWorld;				// WorldMatrix
    D3DXMATRIX				m_matWorldViewProj;
    D3DXMATRIX				m_matWorldView;
	D3DXMATRIX				m_matPosition;
	D3DXVECTOR3		        m_vVelocity;
	D3DXVECTOR3		        m_vAngularVelocity;
	D3DXVECTOR3				m_vecScale;
	float					m_fAutoTime;
	float					m_fAutoCamSpeed;			// Speed factor for auto cam

	BOOL					m_bCheckPSsupport;		// Enables/Disables pixelshader support check
	LPDIRECT3DVERTEXSHADER9	m_pVertexShader;		// VertexShader
	LPDIRECT3DPIXELSHADER9	m_pPixelShader;			// PixelShader
	D3DCOLOR				m_colFog;				// Fog color


public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CBaseD3DApplication();
    virtual ~CBaseD3DApplication();
};


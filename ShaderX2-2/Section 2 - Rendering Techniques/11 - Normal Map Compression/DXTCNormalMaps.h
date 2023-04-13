/////////////////////////////////////////////////////////////////////////////////////////
// Copyright (C) Jakub Klarowicz, 2003.
// ShaderX 2
// All rights reserved
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute.
//
// Author: Jakub Klarowicz
// Email: kuba@techland.pl
//
/////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// File: DXTCNormalMaps.h
//
// Desc: Header file DXTCNormalMaps sample app
//-----------------------------------------------------------------------------
#pragma once




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\DXTCNormalMaps")

// Custom D3D vertex format used by the vertex buffer
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;       // vertex position
    D3DXVECTOR2 texcoord;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)


// Struct to store the current input state
struct UserInput
{
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};


enum EMode
{
    emShowBumpRGB = 0,
    emShowBumpDXTC,
    emShowBumpGINA,

    emShowDiffDXTC,
    emShowDiffGINA,

    emShowLightRGB,
    emShowLightDXTC,
    emShowLightGINA,

    emNumModes
};

#define NUM_TEXTURES 3

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    LPDIRECT3DVERTEXBUFFER9 m_pVB;                  // Vextex buffer 
    CD3DFont*               m_pFont;                // Font for drawing text
    UserInput               m_UserInput;            // Struct for storing user input 

    float                   m_fWorldRotX;           // World rotation state X-axis
    float                   m_fWorldRotY;           // World rotation state Y-axis
    float                   m_fDistance;

    D3DXVECTOR3             m_vLightPos;            // Light position in world space

    ID3DXEffect*            m_pFX;
    D3DXHANDLE              m_hTechniques[emNumModes];

    IDirect3DTexture9*      m_pBaseTex[NUM_TEXTURES];      // Diffuse textures
    IDirect3DTexture9*      m_pBumpR8G8B8[NUM_TEXTURES];   // Uncompressed normal maps
    IDirect3DTexture9*      m_pBumpDXTC[NUM_TEXTURES];     // DXT5 normal maps
    IDirect3DTexture9*      m_pBumpGINA[NUM_TEXTURES];     // DXT5 with G in A normal maps

    EMode                   m_eMode;                // rendering mode
    int                     m_iTexSet;              // number of texture set

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );
public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};


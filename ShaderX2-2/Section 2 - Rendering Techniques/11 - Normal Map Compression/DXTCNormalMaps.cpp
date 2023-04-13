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
// File: DXTCNormalMaps.cpp
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
#include "D3DUtil.h"
#include "resource.h"
#include "DXTCNormalMaps.h"



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
    m_dwCreationWidth           = 800;
    m_dwCreationHeight          = 600;
    m_strWindowTitle            = TEXT( "DXTCNormalMaps" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Verdana"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pVB                       = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );

    m_fWorldRotX                =  0.5f;
    m_fWorldRotY                =  0;
    m_fDistance                 = -2.5f;

    m_vLightPos                 = D3DXVECTOR3(0,0,-4);

    m_pFX                       = NULL;
    memset(m_hTechniques, 0, sizeof(m_hTechniques));

    memset(m_pBaseTex, 0, sizeof(m_pBaseTex));
    memset(m_pBumpR8G8B8, 0, sizeof(m_pBumpR8G8B8));
    memset(m_pBumpDXTC, 0, sizeof(m_pBumpDXTC));
    memset(m_pBumpGINA, 0, sizeof(m_pBumpGINA));

    m_iTexSet = 0;

    m_eMode = emShowLightRGB;
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
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
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
    int i;
    HRESULT hr;

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    // Create the vertex buffer
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 3*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB, NULL ) ) )
        return DXTRACE_ERR( "CreateVertexBuffer", hr );

    // Fill the vertex buffer with 2 triangles
    CUSTOMVERTEX* pVertices;

    if( FAILED( hr = m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0 ) ) )
        return DXTRACE_ERR( "Lock", hr );

    pVertices[0].position = D3DXVECTOR3( -1.0f, -1.0f,  0.0f );
    pVertices[0].texcoord = D3DXVECTOR2(  0.0f,  1.0f );
    pVertices[1].position = D3DXVECTOR3( -1.0f,  1.0f,  0.0f );
    pVertices[1].texcoord = D3DXVECTOR2(  0.0f,  0.0f );
    pVertices[2].position = D3DXVECTOR3(  1.0f, -1.0f,  0.0f );
    pVertices[2].texcoord = D3DXVECTOR2(  1.0f,  1.0f );

    pVertices[3].position = D3DXVECTOR3( -1.0f,  1.0f,  0.0f );
    pVertices[3].texcoord = D3DXVECTOR2(  0.0f,  0.0f );
    pVertices[4].position = D3DXVECTOR3(  1.0f,  1.0f,  0.0f );
    pVertices[4].texcoord = D3DXVECTOR2(  1.0f,  0.0f );
    pVertices[5].position = D3DXVECTOR3(  1.0f, -1.0f,  0.0f );
    pVertices[5].texcoord = D3DXVECTOR2(  1.0f,  1.0f );

    m_pVB->Unlock();

    LPD3DXBUFFER err = NULL;
    if (FAILED(hr = D3DXCreateEffectFromFile(m_pd3dDevice, "bump.fx", NULL, NULL, 0, NULL, &m_pFX, &err)))
    {
        char buffer[1024];
        memcpy(buffer, err->GetBufferPointer(), err->GetBufferSize());
        buffer[err->GetBufferSize()] = 0;
        DXTRACE(buffer);

        err->Release();
        return hr;
    }

    for (i = 0; i < NUM_TEXTURES; ++i)
    {
        char file[MAX_PATH];

        sprintf(file, "%i_tmap.dds", i+1);
        if (FAILED(hr = D3DUtil_CreateTexture(m_pd3dDevice, file, &m_pBaseTex[i])))
            return DXTRACE_ERR("CreateTexture", hr);

        sprintf(file, "%i_bump.dds", i+1);
        if (FAILED(hr = D3DUtil_CreateTexture(m_pd3dDevice, file, &m_pBumpR8G8B8[i])))
            return DXTRACE_ERR("CreateTexture", hr);

        sprintf(file, "%i_bumpDXTC.dds", i+1);
        if (FAILED(hr = D3DUtil_CreateTexture(m_pd3dDevice, file, &m_pBumpDXTC[i])))
            return DXTRACE_ERR("CreateTexture", hr);

        sprintf(file, "%i_bumpGINA.dds", i+1);
        if (FAILED(hr = D3DUtil_CreateTexture(m_pd3dDevice, file, &m_pBumpGINA[i])))
            return DXTRACE_ERR("CreateTexture", hr);
    }

    m_pFX->SetTexture(m_pFX->GetParameterByName(NULL, "bump_rgb"), m_pBumpR8G8B8[0]);
    m_pFX->SetTexture(m_pFX->GetParameterByName(NULL, "bump_dxtc"), m_pBumpDXTC[0]);
    m_pFX->SetTexture(m_pFX->GetParameterByName(NULL, "bump_dxtc_gina"), m_pBumpGINA[0]);

    m_hTechniques[emShowBumpRGB]    = m_pFX->GetTechniqueByName("ShowBumpRGB");
    m_hTechniques[emShowBumpDXTC]   = m_pFX->GetTechniqueByName("ShowBumpDXTC");
    m_hTechniques[emShowBumpGINA]   = m_pFX->GetTechniqueByName("ShowBumpGINA");
    m_hTechniques[emShowDiffDXTC]   = m_pFX->GetTechniqueByName("ShowDiffDXTC");
    m_hTechniques[emShowDiffGINA]   = m_pFX->GetTechniqueByName("ShowDiffGINA");
    m_hTechniques[emShowLightRGB]   = m_pFX->GetTechniqueByName("ShowLightRGB");
    m_hTechniques[emShowLightDXTC]  = m_pFX->GetTechniqueByName("ShowLightDXTC");
    m_hTechniques[emShowLightGINA]  = m_pFX->GetTechniqueByName("ShowLightGINA");

    for (i = 0; i < emNumModes; ++i)
    {
        if (m_hTechniques[i])
            if (FAILED(hr = m_pFX->ValidateTechnique(m_hTechniques[i])))
                return DXTRACE_ERR("ValideateTechnique", hr);
    }

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
    m_pFX->OnResetDevice();

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Restore the font
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // TODO: update world

    // Update user input state
    UpdateInput( &m_UserInput );

    // Update the world state according to user input
    D3DXMATRIX matWorld;
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

    D3DXMatrixMultiply( &matWorld, &matRotX, &matRotY );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, m_fDistance );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    D3DXMATRIX matMVP, matMV;
    D3DXMatrixMultiply(&matMV, &matWorld, &matView);
    D3DXMatrixMultiply(&matMVP, &matMV, &matProj);

    m_pFX->SetMatrix(m_pFX->GetParameterByName(NULL, "combined"), &matMVP);

    D3DXMATRIX matInvWorld;
    float fDet;
    D3DXMatrixInverse(&matInvWorld, &fDet, &matWorld);

    // Animate light position
    float t = (float)timeGetTime();
    m_vLightPos.x = 3*sinf(t*0.001f);

    // Calculate light position in model space
    D3DXVECTOR4 vLightModelPos;
    D3DXVec3Transform(&vLightModelPos, &m_vLightPos, &matInvWorld);

    m_pFX->SetVector(m_pFX->GetParameterByName(NULL, "light_pos_model"), &vLightModelPos);

    // Calculate eye position in model space
    D3DXVECTOR3 vEyePos(0,0,m_fDistance);
    D3DXVECTOR4 vEyeModelPos;
    D3DXVec3Transform(&vEyeModelPos, &vEyePos, &matInvWorld);

    m_pFX->SetVector(m_pFX->GetParameterByName(NULL, "eye_pos_model"), &vEyeModelPos);

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
    
    if ( m_bActive && (GetAsyncKeyState( VK_PRIOR ) & 0x8000) == 0x8000 ) m_fDistance += 0.01f;
    if ( m_bActive && (GetAsyncKeyState( VK_NEXT ) & 0x8000) == 0x8000 )  m_fDistance -= 0.01f;

    if ( m_bActive && (GetAsyncKeyState( '1' ) & 0x8000) == 0x8000 ) m_eMode = emShowBumpRGB;
    if ( m_bActive && (GetAsyncKeyState( '2' ) & 0x8000) == 0x8000 ) m_eMode = emShowBumpDXTC;
    if ( m_bActive && (GetAsyncKeyState( '3' ) & 0x8000) == 0x8000 ) m_eMode = emShowBumpGINA;
    if ( m_bActive && (GetAsyncKeyState( '4' ) & 0x8000) == 0x8000 ) m_eMode = emShowDiffDXTC;
    if ( m_bActive && (GetAsyncKeyState( '5' ) & 0x8000) == 0x8000 ) m_eMode = emShowDiffGINA;
    if ( m_bActive && (GetAsyncKeyState( '6' ) & 0x8000) == 0x8000 ) m_eMode = emShowLightRGB;
    if ( m_bActive && (GetAsyncKeyState( '7' ) & 0x8000) == 0x8000 ) m_eMode = emShowLightDXTC;
    if ( m_bActive && (GetAsyncKeyState( '8' ) & 0x8000) == 0x8000 ) m_eMode = emShowLightGINA;

    if ( m_bActive && (GetAsyncKeyState( 'T' ) & 0x8001) == 0x8001 ) m_iTexSet = (m_iTexSet+1) % NUM_TEXTURES;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        m_pFX->SetTexture(m_pFX->GetParameterByName(NULL, "base_tex"), m_pBaseTex[m_iTexSet]);
        m_pFX->SetTexture(m_pFX->GetParameterByName(NULL, "bump_rgb"), m_pBumpR8G8B8[m_iTexSet]);
        m_pFX->SetTexture(m_pFX->GetParameterByName(NULL, "bump_dxtc"), m_pBumpDXTC[m_iTexSet]);
        m_pFX->SetTexture(m_pFX->GetParameterByName(NULL, "bump_dxtc_gina"), m_pBumpGINA[m_iTexSet]);

        // Render the vertex buffer contents
        m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
        m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

        m_pFX->SetTechnique(m_hTechniques[m_eMode]);
        UINT passes;
        m_pFX->Begin(&passes, 0);
        m_pFX->Pass(0);

        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );

        m_pFX->End();

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
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 60.0f; 

    m_pFont->DrawText( 2, fNextLine, fontColor, "0-7 - modes, T - next texture, arrows - rotate, pg up/pg down - zoom" );

    fNextLine -= 20.0f;
    switch (m_eMode)
    {
        case emShowBumpRGB:
            m_pFont->DrawText( 2, fNextLine, fontColor, "Normal map R8G8B8" );
            break;

        case emShowBumpDXTC:
            m_pFont->DrawText( 2, fNextLine, fontColor, "Normal map DXT5" );
            break;

        case emShowBumpGINA:
            m_pFont->DrawText( 2, fNextLine, fontColor, "Normal map DXT5 GinA" );
            break;

        case emShowDiffDXTC:
            m_pFont->DrawText( 2, fNextLine, fontColor, "Normal map error DXT5" );
            break;

        case emShowDiffGINA:
            m_pFont->DrawText( 2, fNextLine, fontColor, "Normal map error DXT5 GinA" );
            break;

        case emShowLightRGB:
            m_pFont->DrawText( 2, fNextLine, fontColor, "Lighting R8G8B8" );
            break;

        case emShowLightDXTC:
            m_pFont->DrawText( 2, fNextLine, fontColor, "Lighting DXT5" );
            break;

        case emShowLightGINA:
            m_pFont->DrawText( 2, fNextLine, fontColor, "Lighting DXT5 GinA" );
            break;
    }

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
    m_pFont->InvalidateDeviceObjects();

    m_pFX->OnLostDevice();

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
    m_pFont->DeleteDeviceObjects();
    SAFE_RELEASE( m_pFX );
    SAFE_RELEASE( m_pVB );

    for (int i = 0; i < NUM_TEXTURES; ++i)
    {
        SAFE_RELEASE(m_pBaseTex[i]);
        SAFE_RELEASE(m_pBumpR8G8B8[i]);
        SAFE_RELEASE(m_pBumpDXTC[i]);
        SAFE_RELEASE(m_pBumpGINA[i]);
    }

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
    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    return S_OK;
}





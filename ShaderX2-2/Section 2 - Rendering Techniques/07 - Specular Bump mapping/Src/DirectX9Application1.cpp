//-----------------------------------------------------------------------------
// File: DirectX9Application1.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <WindowsX.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <dinput.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "win32/resource.h"
#include "DirectX9Application1.h"
#include "Mesh.h"
#include "Scene.h"
#include "Light.h"
#include "TextureUtils.h"


#define LIGHT_BILLBOARDS


static const FLOAT pi = 3.14159f;
static const FLOAT halfPi = pi * 0.5f;
static const FLOAT twoPi = pi * 2.0f;


//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


struct VERTEX 
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu,  tv;
    
    static const DWORD FVF;
};
const DWORD VERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

FLOAT g_speed = 0.0f;

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


UserInput::UserInput()
: bIncPow(false), bDecPow(false), 
bIncSpeed(false), bDecSpeed(false), 
bIncSpecType(false), bIncSpecTypeOld(false)
{
}

CameraMouse::CameraMouse()
: init(false), drag(false),
mouseX(0), mouseY(0), mouseXO(0), mouseYO(0),
xAngle(0.0f), yAngle(0.0f)
{
}

void CameraMouse::GetCameraPos(D3DXVECTOR3& pos)
{
	D3DXMATRIX matRotX;
	D3DXMATRIX matRotY;

	D3DXMatrixRotationX(&matRotX, yAngle);
	D3DXMatrixRotationY(&matRotY, xAngle);

	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = -1.0f;

	D3DXVec3TransformCoord(&pos, &pos, &matRotX);
	D3DXVec3TransformCoord(&pos, &pos, &matRotY);
}

LRESULT CameraMouse::HandleMouseMessages( HWND hWnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );

	int mX = GET_X_LPARAM(lParam);
	int mY = GET_Y_LPARAM(lParam);

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
            // Start drag mode
            drag = true;
			break;

        case WM_LBUTTONUP:
            // End drag mode
            drag = false;
			break;

        case WM_MOUSEMOVE:
            //if( MK_LBUTTON&wParam )
            {
				mouseXO = mouseX;
				mouseYO = mouseY;
				mouseX = mX;
				mouseY = mY;
				if (!init)
				{
					mouseXO = mouseX;
					mouseYO = mouseY;
					init = true;
				}
			}
			break;
	}

	if (drag)
	{
		static const FLOAT mScale = 0.001f;
		xAngle += (FLOAT)(mouseX - mouseXO) * mScale;
		yAngle += (FLOAT)(mouseY - mouseYO) * mScale;

		// Wrap x angle
		if (xAngle >= twoPi) xAngle -= twoPi;
		if (xAngle < 0.0f) xAngle += twoPi;
		// Clamp y angle
		if (yAngle > halfPi) yAngle = halfPi;
		if (yAngle < -halfPi) yAngle = -halfPi;
	}

	return TRUE;
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
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "DirectX9Application1" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    //DXUtil_GetDXSDKMediaPathCb( m_strInitialDir, sizeof(m_strInitialDir) );
	GetCurrentDirectory(sizeof(m_strInitialDir), m_strInitialDir);

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

	m_pObject			        = new Mesh();

	m_Scene						= new Scene();

	FLOAT dist;
	
	dist = 100.0f;
	Light* light = new Light();
	light->m_Type = D3DLIGHT_POINT;
	light->m_Pos.x = -72.0f;
	light->m_Pos.y = 82.0f;
	light->m_Pos.z = 113.0f;
	light->m_Dir.x = 0.0f;
	light->m_Dir.y = 0.0f;
	light->m_Dir.z = 0.0f;
	light->m_Ambient.r = 0.1f;
	light->m_Ambient.g = 0.1f;
	light->m_Ambient.b = 0.1f;
	light->m_Ambient.a = 1.0f;
	light->m_Diffuse.r = 1.0f;
	light->m_Diffuse.g = 1.0f;
	light->m_Diffuse.b = 1.0f;
	light->m_Diffuse.a = 1.0f;
	light->m_Specular.r = 1.0f;
	light->m_Specular.g = 1.0f;
	light->m_Specular.b = 1.0f;
	light->m_Specular.a = 1.0f;
	m_Scene->m_Lights.push_back(light);

	dist = 300.0f;
	light = new Light();
	light->m_Type = D3DLIGHT_POINT;
	light->m_Pos.x = 140.0f;
	light->m_Pos.y = 39.0f;
	light->m_Pos.z = -60.0f;
	light->m_Dir.x = 0.0f;
	light->m_Dir.y = 0.0f;
	light->m_Dir.z = 0.0f;
	light->m_Ambient.r = 0.0f;
	light->m_Ambient.g = 0.0f;
	light->m_Ambient.b = 0.0f;
	light->m_Ambient.a = 1.0f;
	light->m_Diffuse.r = 0.61f;
	light->m_Diffuse.g = 0.71f;
	light->m_Diffuse.b = 0.82f;
	light->m_Diffuse.a = 1.0f;
	light->m_Specular.r = 0.61f;
	light->m_Specular.g = 0.71f;
	light->m_Specular.b = 0.82f;
	light->m_Specular.a = 1.0f;
	m_Scene->m_Lights.push_back(light);

    // Read settings from registry
    ReadSettings();
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

    // Initialize DirectInput
    InitInput( m_hWnd );

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReadSettings()
// Desc: Read the app settings from the registry
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::ReadSettings()
{
    HKEY hkey;
    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
        // TODO: change as needed

        // Read the stored window width/height.  This is just an example,
        // of how to use DXUtil_Read*() functions.
        DXUtil_ReadIntRegKey( hkey, TEXT("Width"), &m_dwCreationWidth, m_dwCreationWidth );
        DXUtil_ReadIntRegKey( hkey, TEXT("Height"), &m_dwCreationHeight, m_dwCreationHeight );

        RegCloseKey( hkey );
    }
}




//-----------------------------------------------------------------------------
// Name: WriteSettings()
// Desc: Write the app settings to the registry
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::WriteSettings()
{
    HKEY hkey;

    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
        // TODO: change as needed

        // Write the window width/height.  This is just an example,
        // of how to use DXUtil_Write*() functions.
        DXUtil_WriteIntRegKey( hkey, TEXT("Width"), m_rcWindowClient.right );
        DXUtil_WriteIntRegKey( hkey, TEXT("Height"), m_rcWindowClient.bottom );

        RegCloseKey( hkey );
    }
}





//-----------------------------------------------------------------------------
// Name: InitInput()
// Desc: Initialize DirectInput objects
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitInput( HWND hWnd )
{
    HRESULT hr;

    // Create a IDirectInput8*
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
        return DXTRACE_ERR( "DirectInput8Create", hr );
    
    // Create a IDirectInputDevice8* for the keyboard
    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
        return DXTRACE_ERR( "CreateDevice", hr );
    
    // Set the keyboard data format
    if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return DXTRACE_ERR( "SetDataFormat", hr );
    
    // Set the cooperative level on the keyboard
    if( FAILED( hr = m_pKeyboard->SetCooperativeLevel( hWnd, 
                                            DISCL_NONEXCLUSIVE | 
                                            DISCL_FOREGROUND | 
                                            DISCL_NOWINKEY ) ) )
        return DXTRACE_ERR( "SetCooperativeLevel", hr );

    // Acquire the keyboard
    m_pKeyboard->Acquire();

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
    // TODO: create device objects

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

#ifdef LIGHT_BILLBOARDS
	HRESULT hr;

	// Sprite object
	if( FAILED( hr = D3DXCreateSprite( m_pd3dDevice, &m_Sprite ) ) )
	{
		return hr;
	}

	// Sprite texture
	if( FAILED( hr = D3DUtil_CreateTexture( m_pd3dDevice, _T("flare.png"), 
									   &m_pLightTexture, D3DFMT_A8R8G8B8 ) ) )
	{
		return hr;
	}

	D3DSURFACE_DESC sDesc;
	m_pLightTexture->GetLevelDesc(0, &sDesc);
	m_LightTextureWidth = (FLOAT)(sDesc.Width);
	m_LightTextureHeight = (FLOAT)(sDesc.Height);
#endif

    // Load geometry
	MeshDesc desc;
	_tcscpy( desc.m_Mesh, _T("Demo.x") );
	_tcscpy( desc.m_Diffuse, _T("demo-diff.bmp") );
	_tcscpy( desc.m_Bump, _T("demo-bump.bmp") );
	_tcscpy( desc.m_SpecLevel, _T("demo-spec.bmp") );
	_tcscpy( desc.m_SpecPower, _T("demo-pow.bmp") );
	desc.m_Scale = 1.0f;
	desc.m_BumpScale = 10.0f;//1.0f;
	desc.m_DiffuseUVScale = 1.0f;
	desc.m_BumpUVScale = 1.0f;
	desc.m_SpecularType = ST_PER_PIXEL;
	desc.m_EnableSpecLevel = true;

    if( FAILED( m_pObject->Create( m_pd3dDevice, desc ) ) )
	{
        return D3DAPPERR_MEDIANOTFOUND;
	}

    return S_OK;
}


void CMyD3DApplication::InitMatrices()
{
    // Set the world matrix
    D3DXMatrixIdentity( &(m_Scene->m_matWorld) );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &(m_Scene->m_matWorld) );

    // Set the projection matrix
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
	D3DXMatrixPerspectiveFovLH( &(m_Scene->m_matProj), D3DX_PI/4, fAspect, 
					m_pObject->GetObjectRadius()/64.0f, 
					m_pObject->GetObjectRadius()*200.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &(m_Scene->m_matProj) );

    // Setup the arcball parameters
    m_ArcBall.SetWindow( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 0.85f );
    m_ArcBall.SetRadius( m_pObject->GetObjectRadius() );
	D3DXMatrixIdentity( m_ArcBall.GetTranslationMatrix() );
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
    // TODO: setup render states

    // Set up the textures
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );

    // Set miscellaneous render states
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );

	InitMatrices();

    // Restore the font
    m_pFont->RestoreDeviceObjects();

	m_pObject->RestoreDeviceObjects( m_pd3dDevice );

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
    D3DXMatrixTranslation( &(m_Scene->m_matWorld), 
								-m_pObject->GetObjectCenter().x,
                                -m_pObject->GetObjectCenter().y,
                                -m_pObject->GetObjectCenter().z );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &(m_Scene->m_matWorld) );

    // Set up view matrix
	static FLOAT dist = -2.5f;
	dist = m_ArcBall.GetTranslationMatrix()->_43;
	m_CameraMouse.GetCameraPos(m_Scene->m_camPos);
	m_Scene->m_camPos *= (2.5f * m_pObject->GetObjectRadius()) + dist;
    D3DXVECTOR3 vAt( 0, 0, 0 );
    D3DXVECTOR3 vUp( 0, 1.0f, 0 );
    D3DXMatrixLookAtLH( &(m_Scene->m_matView), &(m_Scene->m_camPos), &vAt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &(m_Scene->m_matView) );

	D3DXVECTOR3 startPos(-72.0f, 82.0f, 113.0f);
	D3DXVECTOR3 endPos(40.0f, 1.0f, 20.0f);
	Light* light = m_Scene->m_Lights[0];
	static FLOAT posAngle = 0.0f;
	posAngle += g_speed;
	if (posAngle < 0.0f) posAngle += twoPi;
	if (posAngle >= twoPi) posAngle -= twoPi;
	FLOAT interp = (cos(posAngle) + 1.0f) * 0.5f;
	light->m_Pos = (startPos * interp) + (endPos * (1.0f - interp));

	// Change specular power
	if (m_UserInput.bIncPow)
	{
		m_Scene->m_SpecularPower += 0.005f;
		if (m_Scene->m_SpecularPower > 1.0f)
		{
			m_Scene->m_SpecularPower = 1.0f;
		}
	}
	if (m_UserInput.bDecPow)
	{
		m_Scene->m_SpecularPower -= 0.005f;
		if (m_Scene->m_SpecularPower < 0.0f)
		{
			m_Scene->m_SpecularPower = 0.0f;
		}
	}

	// Change speed
	if (m_UserInput.bIncSpeed)
	{
		g_speed += 0.0005f;
	}
	if (m_UserInput.bDecSpeed)
	{
		g_speed -= 0.0005f;
		if (g_speed < 0.0f)
		{
			g_speed = 0.0f;
		}
	}

	// Change specular type
	if (m_UserInput.bIncSpecType && !(m_UserInput.bIncSpecTypeOld))
	{
		DWORD specularType = m_pObject->GetSpecularType();
		specularType++;
		if (specularType > ST_PER_PIXEL) specularType = ST_POW1;
		m_pObject->SetSpecularType(specularType, m_pd3dDevice);
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    HRESULT hr;

    // Get the input's device state, and put the state in dims
    ZeroMemory( &pUserInput->diks, sizeof(pUserInput->diks) );
    hr = m_pKeyboard->GetDeviceState( sizeof(pUserInput->diks), &pUserInput->diks );
    if( FAILED(hr) ) 
    {
        m_pKeyboard->Acquire();
        return; 
    }

    // TODO: Process user input as needed
	pUserInput->bIncSpecTypeOld = pUserInput->bIncSpecType;
    pUserInput->bIncPow		 = ( (pUserInput->diks[DIK_UP] & 0x80)		  == 0x80 );
    pUserInput->bDecPow		 = ( (pUserInput->diks[DIK_DOWN] & 0x80)	  == 0x80 );
    pUserInput->bIncSpeed	 = ( (pUserInput->diks[DIK_RIGHT] & 0x80)	  == 0x80 );
    pUserInput->bDecSpeed	 = ( (pUserInput->diks[DIK_LEFT] & 0x80)	  == 0x80 );
	pUserInput->bIncSpecType = ( (pUserInput->diks[DIK_S] & 0x80)		  == 0x80 );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	// Add all light ambient terms into the scene ambient term
	m_Scene->m_Ambient.r = 0.0f;
	m_Scene->m_Ambient.g = 0.0f;
	m_Scene->m_Ambient.b = 0.0f;
	m_Scene->m_Ambient.a = 0.0f;
	DWORD i;
	for (i=0;i<m_Scene->m_Lights.size();i++)
	{
		Light* light = m_Scene->m_Lights[i];
		m_Scene->m_Ambient.r += light->m_Ambient.r;
		m_Scene->m_Ambient.g += light->m_Ambient.g;
		m_Scene->m_Ambient.b += light->m_Ambient.b;
		m_Scene->m_Ambient.a += light->m_Ambient.a;
	}
	if (m_Scene->m_Ambient.r > 1.0f) m_Scene->m_Ambient.r = 1.0f;
	if (m_Scene->m_Ambient.g > 1.0f) m_Scene->m_Ambient.g = 1.0f;
	if (m_Scene->m_Ambient.b > 1.0f) m_Scene->m_Ambient.b = 1.0f;
	if (m_Scene->m_Ambient.a > 1.0f) m_Scene->m_Ambient.a = 1.0f;

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // Render the mesh
		m_pObject->Render( m_Scene, m_pd3dDevice );

#ifdef LIGHT_BILLBOARDS
		// Render light billboards
		m_Sprite->Begin();
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE );
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetPixelShader(NULL);
		for (i=0;i<m_Scene->m_Lights.size();i++)
		{
			Light* light = m_Scene->m_Lights[i];

			D3DXMATRIX matWorld;
			D3DXMatrixIdentity(&matWorld);

			D3DXMATRIX matProj(m_Scene->m_matProj);
			matProj._11 *= 0.5f;
			matProj._22 *= 0.5f;
			D3DXMATRIX matCamera;
			D3DXMATRIX mat;
			D3DXMatrixMultiply(&matCamera, &(matWorld), &(m_Scene->m_matView));
			D3DXMatrixMultiply(&mat, &matCamera, &(matProj));

			D3DXVECTOR3 lPos3(light->m_Pos.x, light->m_Pos.y, light->m_Pos.z);
			D3DXVECTOR4 lPos;
			D3DXVec3Transform(&lPos, &lPos3, &mat);

			lPos.x /= lPos.w;
			lPos.y /= lPos.w;
			lPos.x = lPos.x + 0.5f;
			lPos.y = 0.5f - lPos.y;
			lPos.x *= m_d3dsdBackBuffer.Width;
			lPos.y *= m_d3dsdBackBuffer.Height;

			FLOAT edgePixels = 0.2f * m_d3dsdBackBuffer.Width;

			lPos.x -= edgePixels * 0.5f;
			lPos.y -= edgePixels * 0.5f;
			D3DXVECTOR2 scale(edgePixels / m_LightTextureWidth, edgePixels / m_LightTextureHeight);
			D3DXVECTOR2 trans(lPos.x, lPos.y);
			D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
			m_Sprite->Draw(m_pLightTexture, NULL, &scale, NULL, 0.0f, &trans, col);
		}
		m_Sprite->End();
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
    FLOAT fNextLine = 60.0f; 

	if (m_pObject->GetSpecularType() == ST_FIXED_HIGH)
	{
		fNextLine += 20.0f;
		sprintf( szMsg, "Specular Power value: %.2f", m_Scene->m_SpecularPower);
		fNextLine -= 20.0f;
		m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
	}

	lstrcpy( szMsg, TEXT("Specular Power Type: ") );
	switch ( m_pObject->GetSpecularType() )
	{
	case ST_POW1:
		lstrcat( szMsg, TEXT("Power 1") );
		break;
	case ST_POW2:
		lstrcat( szMsg, TEXT("Power 2") );
		break;
	case ST_POW4:
		lstrcat( szMsg, TEXT("Power 4") );
		break;
	case ST_POW8:
		lstrcat( szMsg, TEXT("Power 8") );
		break;
	case ST_POW16:
		lstrcat( szMsg, TEXT("Power 16") );
		break;
	case ST_POW32:
		lstrcat( szMsg, TEXT("Power 32") );
		break;
	case ST_LINEAR_APPROX:
		lstrcat( szMsg, TEXT("Linear Approx") );
		break;
	case ST_FIXED_HIGH:
		lstrcat( szMsg, TEXT("Fixed High") );
		break;
	case ST_PER_PIXEL:
		lstrcat( szMsg, TEXT("Per Pixel") );
		break;
	default:
		lstrcat( szMsg, TEXT("Unknown") );
		break;
	}
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("'Left' and 'Right' array keys to vary light speed") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("'Up' and 'Down' array keys to vary specular power for 'Fixed high' type") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'S' to cycle through specular power types") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
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
    // Pass mouse messages to the ArcBall so it can build internal matrices
    m_ArcBall.HandleMouseMessages( hWnd, msg, wParam, lParam );
    m_CameraMouse.HandleMouseMessages( hWnd, msg, wParam, lParam );

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

		case WM_COMMAND:
		{
			if( m_bWindowed )
			{
				if ( LOWORD(wParam) == IDM_OPENFILE )
				{
					// Handle the open file command
					TCHAR g_strFilename[512]   = _T("");
					TCHAR strMeshFilename[512]   = _T("");

					// Display the OpenFileName dialog. Then, try to load the specified file
					OPENFILENAME ofn = { sizeof(OPENFILENAME), NULL, NULL,
										_T(".X Files (.x)\0*.x\0\0"), 
										NULL, 0, 1, strMeshFilename, 512, g_strFilename, 512, 
										m_strInitialDir, _T("Open Mesh File"), 
										OFN_FILEMUSTEXIST, 0, 1, NULL, 0, NULL, NULL };

					if( TRUE == GetOpenFileName( &ofn ) )
					{
						_tcscpy( m_strInitialDir, strMeshFilename );
						TCHAR* pLastSlash =  _tcsrchr( m_strInitialDir, _T('\\') );
						if( pLastSlash )
							*pLastSlash = 0;
						SetCurrentDirectory( m_strInitialDir );

						m_pObject->SetMesh(strMeshFilename, m_pd3dDevice);
						InitMatrices();
					}
				}
				else if (	LOWORD(wParam) == IDM_OPENDIFFUSE || 
							LOWORD(wParam) == IDM_OPENBUMP ||
							LOWORD(wParam) == IDM_OPENSPECLEVEL ||
							LOWORD(wParam) == IDM_OPENSPECPOWER)
				{
					// Handle an open texture command
					TCHAR g_strFilename[512]   = _T("");
					TCHAR strFilename[512]   = _T("");

					// Display the OpenFileName dialog. Then, try to load the specified file
					OPENFILENAME ofn = { sizeof(OPENFILENAME), NULL, NULL,
										_T("Texture Files (.bmp;.tga;.png)\0*.bmp;*.tga;*.png\0\0"), 
										NULL, 0, 1, strFilename, 512, g_strFilename, 512, 
										m_strInitialDir, _T("Open Mesh File"), 
										OFN_FILEMUSTEXIST, 0, 1, NULL, 0, NULL, NULL };

					if( TRUE == GetOpenFileName( &ofn ) )
					{
						_tcscpy( m_strInitialDir, strFilename );
						TCHAR* pLastSlash =  _tcsrchr( m_strInitialDir, _T('\\') );
						if( pLastSlash )
							*pLastSlash = 0;
						SetCurrentDirectory( m_strInitialDir );

						if ( LOWORD(wParam) == IDM_OPENDIFFUSE ) m_pObject->SetDiffuse(strFilename, m_pd3dDevice);
						else if ( LOWORD(wParam) == IDM_OPENBUMP ) m_pObject->SetBump(strFilename, m_pd3dDevice);
						else if ( LOWORD(wParam) == IDM_OPENSPECLEVEL ) m_pObject->SetSpecLevel(strFilename, m_pd3dDevice);
						else if ( LOWORD(wParam) == IDM_OPENSPECPOWER ) m_pObject->SetSpecPower(strFilename, m_pd3dDevice);
					}
				}
			}
			break;
		}
    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}


HRESULT CMyD3DApplication::DeviceLost()
{
#ifdef LIGHT_BILLBOARDS
	m_Sprite->OnLostDevice();
#endif
	return S_OK;
}

HRESULT CMyD3DApplication::DeviceReset()
{
#ifdef LIGHT_BILLBOARDS
	m_Sprite->OnResetDevice();
#endif
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();
	m_pObject->InvalidateDeviceObjects();

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

	m_pObject->Destroy();

	TextureUtils::Cleanup();

#ifdef LIGHT_BILLBOARDS
	SAFE_RELEASE( m_Sprite );
	SAFE_RELEASE( m_pLightTexture );
#endif

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
	SAFE_DELETE( m_pObject );

	SAFE_DELETE( m_Scene );

    // Cleanup DirectInput
    CleanupDirectInput();

    // Write the settings to the registry
    WriteSettings();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CleanupDirectInput()
// Desc: Cleanup DirectInput 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::CleanupDirectInput()
{
    // Cleanup DirectX input objects
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
	}
    SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );

}





//-----------------------------------------------------------------------------
// File: Particles.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
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
#include "resource.h"
#include "Explosion.h"

//number of particles in the system
#define NUM_PARTICLES 600



//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


//Explosion vertex shader.  
const static char particleVertexShader[] =
{ 
"vs_1_1 \n \
 #define VIEW_PROJECTION_TRANSPOSED c0 \n \
 #define VIEW_RIGHT_VECTOR c4 \n \
 #define VIEW_UP_VECTOR c5 \n \
 #define TIME c6 \n \
 dcl_position v0 \n \
 dcl_texcoord0 v1 \n \
 dcl_texcoord1 v2 ;up/right factor \n \
 dcl_texcoord2 v3 ;velocity \n \
 dcl_color0 v4 ;birth color \n \
 dcl_color1 v5 ;death color \n \
 \
 ; right \n \
 mul r0, VIEW_RIGHT_VECTOR, v2.x \n \
 \
 ; up \n \
 mul r1, VIEW_UP_VECTOR, v2.y \n \
 \
 ; v0 + up + right \n \
 add r2, r0, r1; \n \
 add r2, v0, r2; \n \
 ;pos + (velocity * time) + 0.5 * acceleration * time^2 \n \
 mul r3, v3, TIME.x \n \
 add r3.y, r3.y, TIME.z \n \
 add r2, r2, r3 \n \
 \
 ; mul by projection \n \
 m4x4 oPos, r2, VIEW_PROJECTION_TRANSPOSED \n \
 \
 mul r1, v5, TIME.x; \n \
 mul r2, v4, TIME.y; \n \
 add oD0, r1, r2; \n \
 mov oT0.xy, v1" 
};

//Simple pixel shader - just loads one texture
const static char particlePixelShader [] = {
"ps.1.1         \n \
 tex t0         \n \
 mul r0, v0, t0  // move to r0"
};

//declaration for the vertex type
const static D3DVERTEXELEMENT9 g_VertexDeclaration[] =
{
    { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 	
    { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
	{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1}, 
	{ 0, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 2}, 	
	{ 0, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,   0}, 
	{ 0, 44, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,   1},
    D3DDECL_END()
};       


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
    m_strWindowTitle            = TEXT( "Particles" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;    
    m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fCenterDist                = 25.0f;
    m_fWorldRotY                = 0.0f;

	m_pVertexBuffer = NULL;	
	m_pIndexBuffer = NULL;
	m_pVertexDeclaration = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;

	m_CurrentTime = m_PreviousTime = timeGetTime();
	m_DeltaTime = 0;
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

	// create the skybox
	m_pSkyBox = new CD3DMesh(); 
	m_pEmitter = new CD3DMesh();

    if( FAILED( m_pSkyBox->Create( m_pd3dDevice, _T("skybox.x") ) ) )
        return D3DAPPERR_MEDIANOTFOUND;

	if( FAILED( m_pEmitter->Create( m_pd3dDevice, _T("emitter.x") ) ) )
        return D3DAPPERR_MEDIANOTFOUND;	

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
    // TODO: setup render states

    // Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

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
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.    
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_MatView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_MatView );

    // Set the projection matrix
    
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_MatProj, D3DX_PI/4, fAspect, 1.0f, 10000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_MatProj );

    // Set up lighting states
    //D3DLIGHT9 light;
    //D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    //m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();
	
	//restore skybox
	m_pSkyBox->RestoreDeviceObjects( m_pd3dDevice );
	m_pEmitter->RestoreDeviceObjects( m_pd3dDevice );

	//recreate the shaders and texture
	createVertexData();
	createShaders();

	D3DXCreateTextureFromFile( m_pd3dDevice, "particle.tga", &m_pTexture );

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
        m_fWorldRotY += m_DeltaTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_DeltaTime;

    if( m_UserInput.bMoveForward && !m_UserInput.bMoveBackward )
	{
        m_fCenterDist -= m_DeltaTime * 35.0f;

		if(m_fCenterDist < 25.0f)
			m_fCenterDist = 25.0f;
	}
    else if( m_UserInput.bMoveBackward && !m_UserInput.bMoveForward )
	{
        m_fCenterDist += m_DeltaTime * 35.0f;

		if(m_fCenterDist > 155.0f)
			m_fCenterDist = 155.0f;
	}

	
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	vFromPt.x -= m_fCenterDist * cosf(m_fWorldRotY);
	vFromPt.z -= m_fCenterDist * sinf(m_fWorldRotY);

    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_MatView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_MatView );

	//time update
	m_PreviousTime = m_CurrentTime;
	m_CurrentTime = timeGetTime();
	
	m_DeltaTime = (m_CurrentTime - m_PreviousTime) * 0.001f; //convert from milliseconds to seconds

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
    pUserInput->bRotateLeft  = ( (pUserInput->diks[DIK_LEFT] & 0x80)  == 0x80 );
    pUserInput->bRotateRight = ( (pUserInput->diks[DIK_RIGHT] & 0x80) == 0x80 );
    pUserInput->bMoveForward   = ( (pUserInput->diks[DIK_UP] & 0x80)    == 0x80 );
    pUserInput->bMoveBackward  = ( (pUserInput->diks[DIK_DOWN] & 0x80)  == 0x80 );
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
                         0x000000ff, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // TODO: render world

		m_pd3dDevice->SetVertexShader( NULL );		
		m_pd3dDevice->SetPixelShader(NULL);

		m_pSkyBox->Render( m_pd3dDevice );
		m_pEmitter->Render( m_pd3dDevice );
	


		m_pd3dDevice->SetVertexDeclaration( m_pVertexDeclaration ); 
		m_pd3dDevice->SetVertexShader( m_pVertexShader );		
		m_pd3dDevice->SetPixelShader( m_pPixelShader );

		m_pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(ParticleVertex) );
		m_pd3dDevice->SetIndices(m_pIndexBuffer);
		
		//one:one blend
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );	
		

		//set the transformation matrix ( world(identity) * view * projection ), transposed

		D3DXMATRIX matTransposedTransformation;

		D3DXMatrixIdentity( &matTransposedTransformation );
		
		D3DXMatrixMultiply( &matTransposedTransformation, &m_MatView, &m_MatProj );

		D3DXMatrixTranspose(&matTransposedTransformation, &matTransposedTransformation);

		m_pd3dDevice->SetVertexShaderConstantF(  0, (float*)&matTransposedTransformation,  4 );	
		
		
		// extract right and up components of the view matrix and set them as constants
		D3DXVECTOR4 viewRight(m_MatView._11, m_MatView._21, m_MatView._31, 0.0f);
		D3DXVECTOR4 viewUp(m_MatView._12, m_MatView._22, m_MatView._32, 0.0f);	

		m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&viewRight, 1);
		m_pd3dDevice->SetVertexShaderConstantF(5, (float*)&viewUp, 1);


		float timeData[4];

		static float particleTime = 0.0f;

		particleTime += m_DeltaTime * 0.5f;

		if(particleTime > 1.0f)
		{
			particleTime = 0.0f;

			//starting over, play a sound using a simple win32 function
			sndPlaySound("boom.wav", SND_ASYNC);
		}
		
		timeData[0] = particleTime; //time
		timeData[1] = 1.0f - particleTime; //time complement for interpolation
		timeData[2] = (particleTime * particleTime) * (0.5f) * -450.81f; //acceleration
		timeData[3] = 0.0f; 

		m_pd3dDevice->SetVertexShaderConstantF(6, timeData, 1);

		//disable zwrite
		m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				
		m_pd3dDevice->SetTexture(0, m_pTexture);				

		//fog off
		m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		
		m_pd3dDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST ,0,0,NUM_PARTICLES * 4,
			0, NUM_PARTICLES * 2);		 
		
		//turn alpha blending off
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		//restore zwrite
		m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

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

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height;     

    lstrcpy( szMsg, TEXT("Use left and right arrows to rotate, up and down to zoom") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
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

	m_pSkyBox->InvalidateDeviceObjects();
	m_pEmitter->InvalidateDeviceObjects(); 


	//release stuff for reset
	SAFE_RELEASE(m_pVertexBuffer);	
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexDeclaration);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pTexture);


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

	m_pSkyBox->Destroy();
	m_pEmitter->Destroy();

	//release everything
	SAFE_RELEASE(m_pVertexBuffer);	
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexDeclaration);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pTexture);

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

    // Cleanup DirectInput
    CleanupDirectInput();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CleanupDirectInput()
// Desc: Cleanup DirectInput 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::CleanupDirectInput()
{
    // Cleanup DirectX input objects
    SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );

}

//Note that no error handling is done in this method
HRESULT CMyD3DApplication::createVertexData()
{
	//create the vertex buffer
	m_pd3dDevice->CreateVertexBuffer( NUM_PARTICLES * 4 * sizeof(ParticleVertex),
		D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL );
    
    // fill vb
    ParticleVertex *vertices;
    m_pVertexBuffer->Lock( 0, 0, (void**)&vertices, 0 );	

	//this loop could be different, passing along more customized data to particles
	for(unsigned short i = 0; i < NUM_PARTICLES; i++)
	{
		unsigned int color = D3DCOLOR_COLORVALUE(255.0f/255.0f, 236.0f/255.0f, 129/255.0f, 1.0f);//0xFFFFFFFF;
		D3DVECTOR center;

		center.x = center.z = center.y = 0.0f;

		color = D3DCOLOR_ARGB(255,244,211,122);

		//set up particles in vertex buffer

		setupParticle(vertices, i, center, 5.0f, 5.0f, color);
	}	
    
    m_pVertexBuffer->Unlock();		

	//create indices
	m_pd3dDevice->CreateIndexBuffer(NUM_PARTICLES * 6 * sizeof(unsigned short),
			D3DUSAGE_WRITEONLY , D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_pIndexBuffer, NULL );
	
    
    unsigned short *indices;
	unsigned short currentIndex = 0;	
    
	m_pIndexBuffer->Lock( 0, 0, (void**)&indices, 0 );	
	
	for(i = 0; i < NUM_PARTICLES; i++)
	{		
		indices[currentIndex++] = (unsigned short)(i*4);
		indices[currentIndex++] = (unsigned short)(i*4 + 1);		
		indices[currentIndex++] = (unsigned short)(i*4 + 2);
			
		indices[currentIndex++] = (unsigned short)(i*4);		
		indices[currentIndex++] = (unsigned short)(i*4 + 2);
		indices[currentIndex++] = (unsigned short)(i*4 + 3);				
	}
	
    m_pIndexBuffer->Unlock();	

	return S_OK;
}

void CMyD3DApplication::createShaders()
{
	HRESULT      hr;

	//vertex declaration
	hr = m_pd3dDevice->CreateVertexDeclaration( g_VertexDeclaration, &m_pVertexDeclaration );

	//create vertex shader	
    LPD3DXBUFFER errorBuf = NULL;
	LPD3DXBUFFER shaderBuf = NULL;    	

    // Build a DWORD array of opcodes from the text string
    hr = D3DXAssembleShader( particleVertexShader, lstrlen(particleVertexShader), 0, NULL, 0, //last 0 is flags, new for dx9
        &shaderBuf, &errorBuf );     
    
	if(SUCCEEDED(hr))
    {
        if(errorBuf)
			errorBuf->Release();

        // Create new pixel shader
        hr = m_pd3dDevice->CreateVertexShader(
			(DWORD*)shaderBuf->GetBufferPointer(), &m_pVertexShader);

        if( FAILED(hr) )
        {            
			if(shaderBuf)
				shaderBuf->Release();            
        }
        else
        {
			if(shaderBuf)
				shaderBuf->Release();						
        }
    }


	//create pixel shader	

    // Build a DWORD array of opcodes from the text string
    hr = D3DXAssembleShader( particlePixelShader, lstrlen(particlePixelShader), 0, NULL, 0, //last 0 is flags, new for dx9
        &shaderBuf, &errorBuf );    
    
	if(SUCCEEDED(hr))
    {
        if(errorBuf)
			errorBuf->Release();

        // Create new pixel shader
        hr = m_pd3dDevice->CreatePixelShader((DWORD*)shaderBuf->GetBufferPointer(),
            &m_pPixelShader );

        if( FAILED(hr) )
        {            
			if(shaderBuf)
				shaderBuf->Release();         
        }
        else
        {
			if(shaderBuf)
				shaderBuf->Release();            
        }
    }
}


void CMyD3DApplication::setupParticle(ParticleVertex* vertexData, int index, D3DVECTOR center,
									  float width, float height, unsigned int color)
{
	ParticleVertex* vertexOffset = &vertexData[(index*4)];

	unsigned int startColor = color;
	unsigned int endColor = 0x00000000 ;

	D3DVECTOR velocity;
	
	//create a random spherical velocity

	//random angle between 0 and 2PI
	float theta = (float)(rand()%360) * 0.017f;

	//random angle between 0 and PI
	float phi = (float)(rand()%180) * 0.017f;

	//random radius
	float radius = (float)(rand() % 150);
	
	velocity.x = radius * sinf(phi) * cosf(theta);
	velocity.y = radius * sinf(phi) * sinf(theta);
	velocity.z = radius * cosf(phi);
	
	
	//Upper left corner
	//R = -1, U = 1
	vertexOffset[0].position.x = center.x;
	vertexOffset[0].position.y = center.y;
	vertexOffset[0].position.z = center.z;	
	vertexOffset[0].tu = 0.0f;
	vertexOffset[0].tv = 0.0f;
	vertexOffset[0].rightFactor = -1.0f * width * 0.5f;
	vertexOffset[0].upFactor = 1.0f * height * 0.5f;	

	vertexOffset[0].velocity.x = velocity.x;
	vertexOffset[0].velocity.y = velocity.y;
	vertexOffset[0].velocity.z = velocity.z;
	vertexOffset[0].birthColor = startColor;
	vertexOffset[0].deathColor = endColor;

	//Upper right corner
	//R = 1, U = 1
	vertexOffset[1].position.x = center.x;
	vertexOffset[1].position.y = center.y;
	vertexOffset[1].position.z = center.z;	
	vertexOffset[1].tu = 1.0f;
	vertexOffset[1].tv = 0.0f;
	vertexOffset[1].rightFactor = 1.0f * width * 0.5f;
	vertexOffset[1].upFactor = 1.0f * height * 0.5f;

	vertexOffset[1].velocity.x = velocity.x;
	vertexOffset[1].velocity.y = velocity.y;
	vertexOffset[1].velocity.z = velocity.z;
	vertexOffset[1].birthColor = startColor;
	vertexOffset[1].deathColor = endColor;
	

	//Lower right corner
	//R = 1, U = -1
	vertexOffset[2].position.x = center.x;
	vertexOffset[2].position.y = center.y;
	vertexOffset[2].position.z = center.z;	
	vertexOffset[2].tu = 1.0f;
	vertexOffset[2].tv = 1.0f;
	vertexOffset[2].rightFactor = 1.0f * width * 0.5f;
	vertexOffset[2].upFactor = -1.0f * height * 0.5f;

	vertexOffset[2].velocity.x = velocity.x;
	vertexOffset[2].velocity.y = velocity.y;
	vertexOffset[2].velocity.z = velocity.z;
	vertexOffset[2].birthColor = startColor;
	vertexOffset[2].deathColor = endColor;
	

	//Lower left corner
	//R = -1, U = -1
	vertexOffset[3].position.x = center.x;
	vertexOffset[3].position.y = center.y;
	vertexOffset[3].position.z = center.z;	
	vertexOffset[3].tu = 0.0f;
	vertexOffset[3].tv = 1.0f;
	vertexOffset[3].rightFactor = -1.0f * width * 0.5f;
	vertexOffset[3].upFactor = -1.0f * height * 0.5f;	

	vertexOffset[3].velocity.x = velocity.x;
	vertexOffset[3].velocity.y = velocity.y;
	vertexOffset[3].velocity.z = velocity.z;
	vertexOffset[3].birthColor = startColor;
	vertexOffset[3].deathColor = endColor;
}
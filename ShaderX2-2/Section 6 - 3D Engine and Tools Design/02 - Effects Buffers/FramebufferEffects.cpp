//-----------------------------------------------------------------------------
// File: FramebufferEffects.cpp
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
#include "DIUtil.h"
#include "resource.h"
#include "FramebufferEffects.h"



//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// This GUID must be unique for every game, and the same for 
// every instance of this app.  // {69FE5D62-F96E-412D-9CF0-ADB4D63CB93D}
// The GUID allows DirectInput to remember input settings
GUID g_guidApp = { 0x69fe5d62, 0xf96e, 0x412d, { 0x9c, 0xf0, 0xad, 0xb4, 0xd6, 0x3c, 0xb9, 0x3d } };


// Input semantics used by this app
enum INPUT_SEMANTICS
{
    // Gameplay semantics
    // TODO: change as needed
    INPUT_ROTATE_AXIS_LR=1, INPUT_ROTATE_AXIS_UD,       
    INPUT_ROTATE_LEFT,      INPUT_ROTATE_RIGHT,    
    INPUT_ROTATE_UP,        INPUT_ROTATE_DOWN,
    INPUT_CONFIG_INPUT,     INPUT_CONFIG_DISPLAY,
	INPUT_RENDERING_DEBUG,	INPUT_RENDERING_SHADER,
	INPUT_RENDERING_SHADERTYPE,
};

// Actions used by this app
DIACTION g_rgGameAction[] =
{
    // TODO: change as needed.  Be sure to delete user map files 
    // (C:\Program Files\Common Files\DirectX\DirectInput\User Maps\*.ini)
    // after changing this, otherwise settings won't reset and will be read 
    // from the out of date ini files 

    // Device input (joystick, etc.) that is pre-defined by DInput, according
    // to genre type. The genre for this app is space simulators.
    { INPUT_ROTATE_AXIS_LR,  DIAXIS_3DCONTROL_LATERAL,      0, TEXT("Rotate left/right"), },
    { INPUT_ROTATE_AXIS_UD,  DIAXIS_3DCONTROL_MOVE,         0, TEXT("Rotate up/down"), },

    // Keyboard input mappings
    { INPUT_ROTATE_LEFT,     DIKEYBOARD_LEFT,               0, TEXT("Rotate left"), },
    { INPUT_ROTATE_RIGHT,    DIKEYBOARD_RIGHT,              0, TEXT("Rotate right"), },
    { INPUT_ROTATE_UP,       DIKEYBOARD_UP,                 0, TEXT("Rotate up"), },
    { INPUT_ROTATE_DOWN,     DIKEYBOARD_DOWN,               0, TEXT("Rotate down"), },
    { INPUT_CONFIG_DISPLAY,  DIKEYBOARD_F2,                 DIA_APPFIXED, TEXT("Configure Display"), },    
    { INPUT_CONFIG_INPUT,    DIKEYBOARD_F3,                 DIA_APPFIXED, TEXT("Configure Input"), },    
    { INPUT_RENDERING_DEBUG, DIKEYBOARD_F9,                 DIA_APPFIXED, TEXT("Cycle through rendering debug modes"), },
    { INPUT_RENDERING_SHADER, DIKEYBOARD_F8,                DIA_APPFIXED, TEXT("Cycle through pixel shader versions"), },
    { INPUT_RENDERING_SHADERTYPE, DIKEYBOARD_F7,            DIA_APPFIXED, TEXT("Cycle through visible effects"), },
	
	
};

#define NUMBER_OF_GAMEACTIONS    (sizeof(g_rgGameAction)/sizeof(DIACTION))




//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


// Using a global makes life so much easier.
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;


// Ditto some handy global textures.
LPDIRECT3DTEXTURE9 g_ptexDistortionBlob = NULL;
LPDIRECT3DTEXTURE9 g_ptexBlob = NULL;
LPDIRECT3DTEXTURE9 g_ptexBackdrop = NULL;


int g_iRenderingDebugMode = 0;
int g_iShaderVersion = 3;

// What sort of pixel shader do we want?
#define CPS_DISTORTION			(1<<0)
#define CPS_GREYSCALE			(1<<1)
#define CPS_HEAT				(1<<2)

// Start with all on.
DWORD g_dwRequiredShaderType = 0xffffffff;



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
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "FramebufferEffects" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pD3DXMesh                 = NULL;
    m_pInputDeviceManager       = NULL;
    m_pDIConfigSurface          = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    //m_fWorldRotX                = 0.0f;
    //m_fWorldRotY                = 0.0f;



	m_iNumRenderTargets			= 0;
	m_ppsurfRenderTargets		= NULL;
	m_pptexRenderTargets		= NULL;


	// Keep it powers of two to be hardware-friendly.
	m_iRenderTargetWidth		= 256;
	m_iRenderTargetHeight		= 256;



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



	// Create some render passes.
	// Note that these will be rendered in possibly random order, but they will be applied
	// to the final image in the given order, so order is important.

	RenderPass *prp;

	// First one must always be the main one.
	prp = new RenderPass ( "main", 3, 0x00004000 );
	lprenderpassMasterList.push_back ( prp );

	// And second one must always be the main alpha one.
	// There is a special hack to make sure "main" and "mainalpha" share the same rendertarget.
	prp = new RenderPass ( "mainalpha", 3 );
	lprenderpassMasterList.push_back ( prp );

	// Moves the pixels towards greyscale. Can also lerp away from greyscale to
	// over-saturate the colours.
	// This buffer is biased, so currently can't be put in an alpha channel,
	// cos we use alpha-blending to do diased renders.
	prp = new RenderPass ( "greyscale", 1, 0x80808080, false );
	lprenderpassMasterList.push_back ( prp );

	// A blur kernel. Note - the interactions with "distort" are... interesting.
	// They may need a special mode to render them at the same time.
	//prp = new RenderPass ( "blur", 1 );
	//lprenderpassMasterList.push_back ( prp );

	// An X+Y screenspace offset - distorts where source pixels are read from.
	// Used for heat shimmer, diffraction, and other silly effects.
	prp = new RenderPass ( "distort", 2, 0x80808080 );
	lprenderpassMasterList.push_back ( prp );

	// A buffer to render a cheesy "heat" value to.
	prp = new RenderPass ( "heat", 1, 0, false );
	lprenderpassMasterList.push_back ( prp );

	// And the buffer that says where to show this heat effect (some sort of "visor" or something)
	prp = new RenderPass ( "heatshow", 1 );
	lprenderpassMasterList.push_back ( prp );


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
// Name: StaticInputAddDeviceCB()
// Desc: Static callback helper to call into CMyD3DApplication class
//-----------------------------------------------------------------------------
HRESULT CALLBACK CMyD3DApplication::StaticInputAddDeviceCB( 
                                         CInputDeviceManager::DeviceInfo* pDeviceInfo, 
                                         const DIDEVICEINSTANCE* pdidi, 
                                         LPVOID pParam )
{
    CMyD3DApplication* pApp = (CMyD3DApplication*) pParam;
    return pApp->InputAddDeviceCB( pDeviceInfo, pdidi );
}




//-----------------------------------------------------------------------------
// Name: InputAddDeviceCB()
// Desc: Called from CInputDeviceManager whenever a device is added. 
//       Set the dead zone, and creates a new InputDeviceState for each device
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InputAddDeviceCB( CInputDeviceManager::DeviceInfo* pDeviceInfo, 
                                                   const DIDEVICEINSTANCE* pdidi )
{
    UNREFERENCED_PARAMETER( pdidi );
    
    // Setup the deadzone 
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = 500;
    pDeviceInfo->pdidDevice->SetProperty( DIPROP_DEADZONE, &dipdw.diph );

    // Create a new InputDeviceState for each device so the 
    // app can record its state 
    InputDeviceState* pNewInputDeviceState = new InputDeviceState;
    ZeroMemory( pNewInputDeviceState, sizeof(InputDeviceState) );
    pDeviceInfo->pParam = (LPVOID) pNewInputDeviceState;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitInput()
// Desc: Initialize DirectInput objects
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitInput( HWND hWnd )
{
    HRESULT hr;

    // Setup action format for the actual gameplay
    ZeroMemory( &m_diafGame, sizeof(DIACTIONFORMAT) );
    m_diafGame.dwSize          = sizeof(DIACTIONFORMAT);
    m_diafGame.dwActionSize    = sizeof(DIACTION);
    m_diafGame.dwDataSize      = NUMBER_OF_GAMEACTIONS * sizeof(DWORD);
    m_diafGame.guidActionMap   = g_guidApp;

    // TODO: change the genre as needed
    m_diafGame.dwGenre         = DIVIRTUAL_CAD_3DCONTROL; 

    m_diafGame.dwNumActions    = NUMBER_OF_GAMEACTIONS;
    m_diafGame.rgoAction       = g_rgGameAction;
    m_diafGame.lAxisMin        = -100;
    m_diafGame.lAxisMax        = 100;
    m_diafGame.dwBufferSize    = 16;
    _tcscpy( m_diafGame.tszActionMap, _T("FramebufferEffects Game") );

    // Create a new input device manager
    m_pInputDeviceManager = new CInputDeviceManager();

    if( FAILED( hr = m_pInputDeviceManager->Create( hWnd, NULL, m_diafGame, 
                                                    StaticInputAddDeviceCB, this ) ) )
        return DXTRACE_ERR( "m_pInputDeviceManager->Create", hr );

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

	// Can't use a PURE device because we use a variety of Get* functions.
	// A real game wouldn't need to, so it could use a PURE dev.
	if ( dwBehavior & D3DCREATE_PUREDEVICE )
	{
		bCapsAcceptable = FALSE;
	}

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




// Handy.
D3DXMATRIX MakePosOrnMatrix ( float fX, float fY, float fZ, float fYaw, float fPitch, float fRoll, float fScale = 1.0f )
{
	const float PI = 3.1415927f;

	D3DXMATRIX res, temp;
	D3DXMatrixRotationYawPitchRoll ( &res, fYaw * ( PI / 180.f ), fPitch * ( PI / 180.f ), fRoll * ( PI / 180.f ) );
	D3DXMatrixScaling ( &temp, fScale, fScale, fScale );
	D3DXMatrixMultiply ( &res, &res, &temp );
	D3DXMatrixTranslation ( &temp, fX, fY, fZ );
	D3DXMatrixMultiply ( &res, &res, &temp );
	return res;
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

    HRESULT hr;

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	g_pd3dDevice = m_pd3dDevice;

    // Create a teapot mesh using D3DX
    if( FAILED( hr = D3DXCreateTeapot( m_pd3dDevice, &m_pD3DXMesh, NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateTeapot", hr );



	// And make some more meshes and objects.
	// In a real game, I would create some meshes and these would be instances of those meshes,
	// rather than being completely new meshes of course. But this way is nice and simple.
	Object *pobj;

	// Just some normal objects first - some sort of environment to look around.
	pobj = new Object();
    if( FAILED( hr = D3DXCreateTeapot( m_pd3dDevice, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateTeapot", hr );
	pobj->mOrn = MakePosOrnMatrix ( 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00ff0000;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Normal );
	AddObject ( "heat", pobj, RenderType_Heat );


	pobj = new Object();
    if( FAILED( hr = D3DXCreateTorus( m_pd3dDevice, 0.5f, 1.0f, 8, 16, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateTorus", hr );
	pobj->mOrn = MakePosOrnMatrix ( -4.0f, 0.0f, 0.0f, 0.0f, 45.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00ffff00;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Normal );
	AddObject ( "heat", pobj, RenderType_Heat );


	pobj = new Object();
    if( FAILED( hr = D3DXCreateSphere( m_pd3dDevice, 1.0f, 16, 8, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateSphere", hr );
	pobj->mOrn = MakePosOrnMatrix ( 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00ff00ff;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Normal );
	AddObject ( "heat", pobj, RenderType_Heat );


	// A bogus object as a hack to draw the background.
	pobj = new Object();
	pobj->pD3DXMesh = NULL;
	pobj->mOrn = MakePosOrnMatrix ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00ff0000;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Background );


	// A couple of pillars.
	pobj = new Object();
    if( FAILED( hr = D3DXCreateCylinder( m_pd3dDevice, 0.5f, 0.5f, 8.0f, 16, 4, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateCylinder", hr );
	pobj->mOrn = MakePosOrnMatrix ( -10.0f, 0.0f, 10.0f, 0.0f, 90.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00ffffff;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Normal );

	pobj = new Object();
    if( FAILED( hr = D3DXCreateCylinder( m_pd3dDevice, 0.5f, 0.5f, 8.0f, 16, 4, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateCylinder", hr );
	pobj->mOrn = MakePosOrnMatrix ( -10.0f, 0.0f, -10.0f, 0.0f, 90.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00ffffff;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Normal );

	pobj = new Object();
    if( FAILED( hr = D3DXCreateCylinder( m_pd3dDevice, 0.5f, 0.5f, 8.0f, 16, 4, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateCylinder", hr );
	pobj->mOrn = MakePosOrnMatrix ( 10.0f, 0.0f, 10.0f, 0.0f, 90.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00ffffff;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Normal );

	pobj = new Object();
    if( FAILED( hr = D3DXCreateCylinder( m_pd3dDevice, 0.5f, 0.5f, 8.0f, 16, 4, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateCylinder", hr );
	pobj->mOrn = MakePosOrnMatrix ( 10.0f, 0.0f, -10.0f, 0.0f, 90.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00ffffff;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Normal );

	// And a floor.
	pobj = new Object();
    if( FAILED( hr = D3DXCreateBox ( m_pd3dDevice, 30.0f, 0.25f, 30.0f, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateBox", hr );
	pobj->mOrn = MakePosOrnMatrix ( 0.0f, -4.125f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00808080;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "main", pobj, RenderType_Normal );



	// And now an effect object in the middle - flaming teapot!
	LPD3DXMESH pD3DXMesh = NULL;
    if( FAILED( hr = D3DXCreateTeapot( m_pd3dDevice, &(pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateTeapot", hr );
	int iNumVerts = pD3DXMesh->GetNumVertices();
	ObjectParticleSystem *pobjps = new ObjectParticleSystem ( iNumVerts );
	pobjps->pD3DXMesh = pD3DXMesh;

	pobjps->mOrn = MakePosOrnMatrix ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	pobjps->dwColour = 0x80808080;
	pobjps->fGreyScale = 1.0f;

	// And add all the vertices as emitters.
	void *pvVerts = NULL;
	hr = pobjps->pD3DXMesh->LockVertexBuffer ( 0, &pvVerts );
	DWORD dwFVF = pobjps->pD3DXMesh->GetFVF();
	int iFVFSize = D3DXGetFVFVertexSize ( dwFVF );
	for ( int iVert = 0; iVert < iNumVerts; iVert++ )
	{
		float *pvPos = (float*)pvVerts;
		pobjps->AddEmitter ( D3DXVECTOR3 ( pvPos[0], pvPos[1], pvPos[2] ) );
		pvVerts = (void *)( ((char *)pvVerts) + iFVFSize );
	}
	hr = pobjps->pD3DXMesh->UnlockVertexBuffer();

	pobjps->iNumParticles = pobjps->iNumEmitters * 10;

	pobjps->rlLifeTime.Init ( 2.0f, 5.0f, 1234 );
	pobjps->rlSize.Init ( 0.01f, 0.2f, 4567 );
	pobjps->rlShade.Init ( 0.0f, 0.1f, 9876 );
	pobjps->rlMoveX.Init ( -0.2f, 0.2f, 37965 );
	pobjps->rlMoveY.Init (  0.2f, 1.0f, 93275 );
	pobjps->rlMoveZ.Init ( -0.2f, 0.2f, 3756 );

	lpobjectMasterList.push_back ( pobjps );
	AddObject ( "mainalpha", pobjps, RenderType_Alpha );
	AddObject ( "distort", pobjps, RenderType_Distort );
	AddObject ( "heat", pobjps, RenderType_Heat );





	// A colour-saturating sphere.
	pobj = new Object();
    if( FAILED( hr = D3DXCreateSphere( m_pd3dDevice, 1.0f, 16, 8, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateSphere", hr );
	pobj->mOrn = MakePosOrnMatrix ( 3.0f, 0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	pobj->dwColour = 0x00000000;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "greyscale", pobj, RenderType_Greyscale );




	// A greyscaling particle sphere
	pD3DXMesh = NULL;
    if( FAILED( hr = D3DXCreateSphere( m_pd3dDevice, 1.0f, 16, 8, &pD3DXMesh, NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateSphere", hr );
	iNumVerts = pD3DXMesh->GetNumVertices();
	pobjps = new ObjectParticleSystem ( iNumVerts );
	pobjps->pD3DXMesh = pD3DXMesh;
	pobjps->mOrn = MakePosOrnMatrix ( -3.0f, 0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	pobjps->dwColour = 0xffffffff;
	pobjps->fGreyScale = 1.0f;

	// And add all the vertices as emitters.
	pvVerts = NULL;
	hr = pobjps->pD3DXMesh->LockVertexBuffer ( 0, &pvVerts );
	dwFVF = pobjps->pD3DXMesh->GetFVF();
	iFVFSize = D3DXGetFVFVertexSize ( dwFVF );
	for ( int iVert = 0; iVert < iNumVerts; iVert++ )
	{
		float *pvPos = (float*)pvVerts;
		pobjps->AddEmitter ( D3DXVECTOR3 ( pvPos[0], pvPos[1], pvPos[2] ) );
		pvVerts = (void *)( ((char *)pvVerts) + iFVFSize );
	}
	hr = pobjps->pD3DXMesh->UnlockVertexBuffer();

	pobjps->iNumParticles = pobjps->iNumEmitters * 20;

	pobjps->rlLifeTime.Init ( 2.0f, 5.0f, 1234 );
	pobjps->rlSize.Init ( 0.0f, 0.5f, 4567 );
	pobjps->rlShade.Init ( 0.0f, 0.4f, 9876 );
	// General fuzziness.
	pobjps->rlMoveX.Init ( -0.2f, 0.2f, 37965 );
	pobjps->rlMoveY.Init ( -0.2f, 0.2f, 93275 );
	pobjps->rlMoveZ.Init ( -0.2f, 0.2f, 3756 );

	lpobjectMasterList.push_back ( pobjps );
	AddObject ( "greyscale", pobjps, RenderType_Greyscale );



#if 1
	// A block that lets you see heat through it.
	pobj = new Object();
    if( FAILED( hr = D3DXCreateBox ( m_pd3dDevice, 4.0f, 4.0f, 0.5f, &(pobj->pD3DXMesh), NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateBox", hr );
	pobj->mOrn = MakePosOrnMatrix ( 0.0f, 1.0f, -10.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	pobj->dwColour = 0xffffffff;
	lpobjectMasterList.push_back ( pobj );
	AddObject ( "heatshow", pobj, RenderType_HeatShow );
#endif



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
    HRESULT hr;

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
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
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
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    //m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// And that was the view orientation, whereas I want the orientation of the camera object.
	D3DXMatrixInverse ( &m_CameraOrn, NULL, &matView );

	

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->LightEnable( 1, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();

    if( !m_bWindowed )
    {
        // Create a surface for configuring DInput devices
        if( FAILED( hr = m_pd3dDevice->CreateOffscreenPlainSurface( 640, 480, 
                                        m_d3dsdBackBuffer.Format, D3DPOOL_DEFAULT, 
										&m_pDIConfigSurface, NULL ) ) ) 
            return DXTRACE_ERR( "CreateOffscreenPlainSurface", hr );
    }



	// Create a certain number of rendertargets.
	const int ciHowManyRenderTargets = 8;
	m_iNumRenderTargets = ciHowManyRenderTargets;
	m_ppsurfRenderTargets = new LPDIRECT3DSURFACE9[m_iNumRenderTargets];
	m_pptexRenderTargets = new LPDIRECT3DTEXTURE9[m_iNumRenderTargets];
	for ( int i = 0; i < m_iNumRenderTargets; i++ )
	{
		m_ppsurfRenderTargets[i] = NULL;

		hr = D3DXCreateTexture ( m_pd3dDevice,
			m_iRenderTargetWidth, m_iRenderTargetHeight,
			1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
			&(m_pptexRenderTargets[i]) );
		if ( FAILED ( hr ) )
		{
            return DXTRACE_ERR( "D3DXCreateTexture", hr );
		}
		// And get the surface as well.
		hr = m_pptexRenderTargets[i]->GetSurfaceLevel ( 0, &(m_ppsurfRenderTargets[i]) );
		if ( FAILED ( hr ) )
		{
            return DXTRACE_ERR( "GetSurfaceLevel", hr );
		}
	}

	// Create the helper class to go with them.
	hr = D3DXCreateRenderToSurface ( m_pd3dDevice,
		m_iRenderTargetWidth, m_iRenderTargetHeight,
		D3DFMT_A8R8G8B8, TRUE, D3DFMT_D24X8, 
		&(pd3dxRenderToSurface) );
	if ( FAILED ( hr ) )
	{
        return DXTRACE_ERR( "D3DXCreateRenderToSurface", hr );
	}

	m_pCombiningPixelShader = NULL;
	m_dwCombiningPixelShaderType = 0;
	m_pShader = NULL;



	hr = D3DXCreateTextureFromFileEx (
		m_pd3dDevice, "DistortionBlob.bmp", D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
		&g_ptexDistortionBlob );
	if ( FAILED ( hr ) )
	{
        return DXTRACE_ERR( "D3DXCreateTextureFromFileEx DistortionBlob.bmp", hr );
	}
	hr = D3DXCreateTextureFromFileEx (
		m_pd3dDevice, "Blob.bmp", D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
		&g_ptexBlob );
	if ( FAILED ( hr ) )
	{
        return DXTRACE_ERR( "D3DXCreateTextureFromFileEx Blob.bmp", hr );
	}

	// OK, and now do some image-mangling. Namely, copy the red channel of blob.bmp to the alpha channels of distortionblob and blob.
	// This is a horrible, horrible hack. Don't copy this code.
	D3DLOCKED_RECT rect1;
	D3DLOCKED_RECT rect2;
	for ( int iMip = 0; iMip < 7; iMip++ )
	{
		g_ptexDistortionBlob->LockRect ( iMip, &rect1, NULL, 0 );
		g_ptexBlob->LockRect ( iMip, &rect2, NULL, 0 );
		DWORD *src = (DWORD*)rect2.pBits;
		DWORD *dst1 = (DWORD*)rect1.pBits;
		DWORD *dst2 = (DWORD*)rect2.pBits;
		int iSize = 1 << ( 6 - iMip );
		ASSERT ( rect1.Pitch == iSize * 4 );
		ASSERT ( rect2.Pitch == iSize * 4 );
		iSize *= iSize;
		for ( int i = 0; i < iSize; i++ )
		{
			*dst1 = ( *dst1 & 0x00ffffff ) | ( ( *src & 0x00ff0000 ) << 8 );
			*dst2 = ( *dst2 & 0x00ffffff ) | ( ( *src & 0x00ff0000 ) << 8 );
			src++;
			dst1++;
			dst2++;
		}
		g_ptexDistortionBlob->UnlockRect ( iMip );
		g_ptexBlob->UnlockRect ( iMip );
	}


	hr = D3DXCreateTextureFromFileEx (
		m_pd3dDevice, "lake.bmp", D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
		&g_ptexBackdrop );
	if ( FAILED ( hr ) )
	{
        return DXTRACE_ERR( "D3DXCreateTextureFromFileEx lake.bmp", hr );
	}





    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: StaticConfigureInputDevicesCB()
// Desc: Static callback helper to call into CMyD3DApplication class
//-----------------------------------------------------------------------------
BOOL CALLBACK CMyD3DApplication::StaticConfigureInputDevicesCB( 
                                            IUnknown* pUnknown, VOID* pUserData )
{
    CMyD3DApplication* pApp = (CMyD3DApplication*) pUserData;
    return pApp->ConfigureInputDevicesCB( pUnknown );
}




//-----------------------------------------------------------------------------
// Name: ConfigureInputDevicesCB()
// Desc: Callback function for configuring input devices. This function is
//       called in fullscreen modes, so that the input device configuration
//       window can update the screen.
//-----------------------------------------------------------------------------
BOOL CMyD3DApplication::ConfigureInputDevicesCB( IUnknown* pUnknown )
{
    // Get access to the surface
    LPDIRECT3DSURFACE9 pConfigSurface = NULL;
    if( FAILED( pUnknown->QueryInterface( IID_IDirect3DSurface9,
                                          (VOID**)&pConfigSurface ) ) )
        return TRUE;

    // Render the scene, with the config surface blitted on top
    Render();

    RECT  rcSrc;
    SetRect( &rcSrc, 0, 0, 640, 480 );

    POINT ptDst;
    ptDst.x = (m_d3dsdBackBuffer.Width-640)/2;
    ptDst.y = (m_d3dsdBackBuffer.Height-480)/2;

    LPDIRECT3DSURFACE9 pBackBuffer;
    m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    m_pd3dDevice->UpdateSurface( pConfigSurface, &rcSrc, pBackBuffer, &ptDst );
    pBackBuffer->Release();

    // Present the backbuffer contents to the front buffer
    m_pd3dDevice->Present( 0, 0, 0, 0 );

    // Release the surface
    pConfigSurface->Release();

    return TRUE;
}


float g_fGameTime = 0.0f;
D3DXMATRIX g_CameraOrn;


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // TODO: update world

	// Let the rest of the world know.
	g_fGameTime = m_fTime;

    // Update user input state
    UpdateInput( &m_UserInput );

    // Respond to input
    if( m_UserInput.bDoConfigureInput && m_bWindowed )  // full-screen configure disabled for now
    {
        // One-shot per keypress
        m_UserInput.bDoConfigureInput = FALSE;

        Pause( true );

        // Get access to the list of semantically-mapped input devices
        // to delete all InputDeviceState structs before calling ConfigureDevices()
        CInputDeviceManager::DeviceInfo* pDeviceInfos;
        DWORD dwNumDevices;
        m_pInputDeviceManager->GetDevices( &pDeviceInfos, &dwNumDevices );

        for ( DWORD i=0; i<dwNumDevices; i++ )
        {
            InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;
            SAFE_DELETE( pInputDeviceState );
            pDeviceInfos[i].pParam = NULL;
        }

        // Configure the devices (with edit capability)
        if( m_bWindowed )
            m_pInputDeviceManager->ConfigureDevices( m_hWnd, NULL, NULL, DICD_EDIT, NULL );
        else
            m_pInputDeviceManager->ConfigureDevices( m_hWnd,
                                                     m_pDIConfigSurface,
                                                     (VOID*)StaticConfigureInputDevicesCB,
                                                     DICD_EDIT, (LPVOID) this );

        Pause( false );
    }

    if( m_UserInput.bDoConfigureDisplay )
    {
        // One-shot per keypress
        m_UserInput.bDoConfigureDisplay = FALSE;

        Pause(true);

        // Configure the display device
        UserSelectNewDevice();

        Pause(false);
    }


#if 0
    // Update the world state according to user input
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.fAxisRotateLR )
        m_fWorldRotY += m_fElapsedTime * m_UserInput.fAxisRotateLR;

    if( m_UserInput.fAxisRotateUD )
        m_fWorldRotX += m_fElapsedTime * m_UserInput.fAxisRotateUD;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixMultiply( &matWorld, &matRotX, &matRotY );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
#else
	// Doom controls.

    D3DXMATRIX matRotY;
    D3DXMATRIX matMove;

	float fRotateY = 0.0f;
    if( m_UserInput.fAxisRotateLR )
        fRotateY = m_fElapsedTime * -m_UserInput.fAxisRotateLR * 2.0f;

	float fMove = 0.0f;
    if( m_UserInput.fAxisRotateUD )
        fMove = m_fElapsedTime * m_UserInput.fAxisRotateUD * 10.0f;

    D3DXMatrixRotationY( &matRotY, fRotateY );
    D3DXMatrixTranslation( &matMove, 0.0f, 0.0f, fMove );

    D3DXMatrixMultiply( &m_CameraOrn, &matMove, &m_CameraOrn );
    D3DXMatrixMultiply( &m_CameraOrn, &matRotY, &m_CameraOrn );

	// Let the outside world know.
	g_CameraOrn = m_CameraOrn;

    D3DXMATRIX matTemp;
	D3DXMatrixInverse ( &matTemp, NULL, &m_CameraOrn );

    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matTemp );
#endif

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    if( NULL == m_pInputDeviceManager )
        return;

    // Get access to the list of semantically-mapped input devices
    CInputDeviceManager::DeviceInfo* pDeviceInfos;
    DWORD dwNumDevices;
    m_pInputDeviceManager->GetDevices( &pDeviceInfos, &dwNumDevices );

    // Loop through all devices and check game input
    for( DWORD i=0; i<dwNumDevices; i++ )
    {
        DIDEVICEOBJECTDATA rgdod[10];
        DWORD   dwItems = 10;
        HRESULT hr;
        LPDIRECTINPUTDEVICE8 pdidDevice = pDeviceInfos[i].pdidDevice;
        InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;

        hr = pdidDevice->Acquire();
        hr = pdidDevice->Poll();
        hr = pdidDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA),
                                        rgdod, &dwItems, 0 );
        if( FAILED(hr) )
            continue;

        // Get the sematics codes for the game menu
        for( DWORD j=0; j<dwItems; j++ )
        {
            BOOL  bButtonState = (rgdod[j].dwData==0x80) ? TRUE : FALSE;
            FLOAT fButtonState = (rgdod[j].dwData==0x80) ? 1.0f : 0.0f;
            FLOAT fAxisState   = (FLOAT)((int)rgdod[j].dwData)/100.0f;
            UNREFERENCED_PARAMETER( fButtonState );

            switch( rgdod[j].uAppData )
            {
                // TODO: Handle semantics for the game 

                // Handle relative axis data
                case INPUT_ROTATE_AXIS_LR: 
                    pInputDeviceState->fAxisRotateLR = -fAxisState;
                    break;
                case INPUT_ROTATE_AXIS_UD:
                    pInputDeviceState->fAxisRotateUD = -fAxisState;
                    break;

                // Handle buttons separately so the button state data
                // doesn't overwrite the axis state data, and handle
                // each button separately so they don't overwrite each other
                case INPUT_ROTATE_LEFT:  pInputDeviceState->bButtonRotateLeft  = bButtonState; break;
                case INPUT_ROTATE_RIGHT: pInputDeviceState->bButtonRotateRight = bButtonState; break;
                case INPUT_ROTATE_UP:    pInputDeviceState->bButtonRotateUp    = bButtonState; break;
                case INPUT_ROTATE_DOWN:  pInputDeviceState->bButtonRotateDown  = bButtonState; break;

                // Handle one-shot buttons
                case INPUT_CONFIG_INPUT:   if( bButtonState ) pUserInput->bDoConfigureInput = TRUE; break;
                case INPUT_CONFIG_DISPLAY: if( bButtonState ) pUserInput->bDoConfigureDisplay = TRUE; break;

				case INPUT_RENDERING_DEBUG:		if( bButtonState ) g_iRenderingDebugMode++; break;
				case INPUT_RENDERING_SHADER:	if( bButtonState ) {g_iShaderVersion++; m_dwCombiningPixelShaderType = ~g_dwRequiredShaderType;} break;
				case INPUT_RENDERING_SHADERTYPE:if( bButtonState ) g_dwRequiredShaderType++; break;
					
            }
        }
    }

    // TODO: change process code as needed

    // Process user input and store result into pUserInput struct
    pUserInput->fAxisRotateLR = 0.0f;
    pUserInput->fAxisRotateUD = 0.0f;

    // Concatinate the data from all the DirectInput devices
    for( DWORD i=0; i<dwNumDevices; i++ )
    {
        InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;

        // Use the axis data that is furthest from zero
        if( fabs(pInputDeviceState->fAxisRotateLR) > fabs(pUserInput->fAxisRotateLR) )
            pUserInput->fAxisRotateLR = pInputDeviceState->fAxisRotateLR;

        if( fabs(pInputDeviceState->fAxisRotateUD) > fabs(pUserInput->fAxisRotateUD) )
            pUserInput->fAxisRotateUD = pInputDeviceState->fAxisRotateUD;

        // Process the button data 
        if( pInputDeviceState->bButtonRotateLeft )
            pUserInput->fAxisRotateLR = 1.0f;
        else if( pInputDeviceState->bButtonRotateRight )
            pUserInput->fAxisRotateLR = -1.0f;

        if( pInputDeviceState->bButtonRotateUp )
            pUserInput->fAxisRotateUD = 1.0f;
        else if( pInputDeviceState->bButtonRotateDown )
            pUserInput->fAxisRotateUD = -1.0f;

    } 
}




const DWORD dwMyFVF = D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
struct MyVertex
{
	float fX, fY, fZ, fRHW;
	float fU, fV;
};

const DWORD dwMyFVF_T3 = D3DFVF_XYZRHW | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2);
struct MyVertex_T3
{
	float fX, fY, fZ, fRHW;
	float fU0, fV0;
	float fU1, fV1;
	float fU2, fV2;
};


const DWORD dwMyFVF4T2 = D3DFVF_XYZRHW | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3);
struct MyVertex4T2
{
	float fX, fY, fZ, fRHW;
	float fU0, fV0;
	float fU1, fV1;
	float fU2, fV2;
	float fU3, fV3;
};



inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }



DWORD MaskFlagsToMask ( DWORD dwColourFlags )
{
	DWORD dwMask = 0;
	if ( dwColourFlags & D3DCOLORWRITEENABLE_ALPHA )
	{
		dwMask |= 0xff000000;
	}
	if ( dwColourFlags & D3DCOLORWRITEENABLE_RED )
	{
		dwMask |= 0x00ff0000;
	}
	if ( dwColourFlags & D3DCOLORWRITEENABLE_GREEN )
	{
		dwMask |= 0x0000ff00;
	}
	if ( dwColourFlags & D3DCOLORWRITEENABLE_BLUE )
	{
		dwMask |= 0x000000ff;
	}
	return dwMask;
}



std::string MakeNameOfPassSource ( RenderPass *prp )
{
	if ( prp == NULL )
	{
		return "PassNotFound";
	}

	std::string sRes = "";
	if ( !prp->bEnabled )
	{
		sRes = "ShouldNotBeUsed";
	}
	else
	{
		ASSERT ( prp->iTargetIndex >= 0 );
		char pcTemp[100];
		sprintf ( pcTemp, "%i", prp->iTargetIndex );
		sRes = std::string("RTRead") + pcTemp;
		sRes += ".";

		if ( prp->dwColourChannelMask & D3DCOLORWRITEENABLE_RED )
		{
			sRes += "r";
		}
		if ( prp->dwColourChannelMask & D3DCOLORWRITEENABLE_GREEN )
		{
			sRes += "g";
		}
		if ( prp->dwColourChannelMask & D3DCOLORWRITEENABLE_BLUE )
		{
			sRes += "b";
		}
		if ( prp->dwColourChannelMask & D3DCOLORWRITEENABLE_ALPHA )
		{
			sRes += "a";
		}
	}

	return sRes;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // Clear the Z buffer.
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {




		// A bit clunky - means we can't use a PURE device.
		// But does mean the framework Just Works.
		LPDIRECT3DSURFACE9 m_psurfBackbuffer;
		LPDIRECT3DSURFACE9 m_psurfZbuffer;
		D3DVIEWPORT9 vpScreen;
		m_pd3dDevice->GetRenderTarget ( 0, &m_psurfBackbuffer );
		m_pd3dDevice->GetDepthStencilSurface ( &m_psurfZbuffer );
		m_pd3dDevice->GetViewport ( &vpScreen );


		
		D3DVIEWPORT9 vpTarget;
		vpTarget.X = 0;
		vpTarget.Y = 0;
		vpTarget.Width = m_iRenderTargetWidth;
		vpTarget.Height = m_iRenderTargetHeight;
		vpTarget.MinZ = 0.0f;
		vpTarget.MaxZ = 1.0f;




		m_pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
		m_pd3dDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		for ( int iStage = 0; iStage < 4; iStage++ )
		{
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
		}
		m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x12345678 );
		m_pd3dDevice->SetRenderState ( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );



		// Enable/disable each render pass.
		RenderPass *prp;
		prp = FindPass ( "heat" );
		ASSERT ( prp != NULL );
		if ( g_dwRequiredShaderType & CPS_HEAT ) { prp->bEnabled = true; } else { prp->bEnabled = false; }
		prp = FindPass ( "heatshow" );
		ASSERT ( prp != NULL );
		if ( g_dwRequiredShaderType & CPS_HEAT ) { prp->bEnabled = true; } else { prp->bEnabled = false; }
		prp = FindPass ( "distort" );
		ASSERT ( prp != NULL );
		if ( g_dwRequiredShaderType & CPS_DISTORTION ) { prp->bEnabled = true; } else { prp->bEnabled = false; }
		prp = FindPass ( "greyscale" );
		ASSERT ( prp != NULL );
		if ( g_dwRequiredShaderType & CPS_GREYSCALE ) { prp->bEnabled = true; } else { prp->bEnabled = false; }




		DWORD *pdwColourChannels = new DWORD[m_iNumRenderTargets];
		DWORD *pdwClearColour = new DWORD[m_iNumRenderTargets];
		// Assign each render pass a channel and rendertarget.
		for ( int i = 0; i < m_iNumRenderTargets; i++ )
		{
			pdwColourChannels[i] = 0;
			pdwClearColour[i] = 0;
		}




		{
			std::list<RenderPass*>::iterator it = lprenderpassMasterList.begin();

			// First pass must always be the main one and must always be enabled - there's a couple of assumptions that rely on this,
			// Mainly to do with the Z buffer.
			RenderPass *prp = *it;
			ASSERT ( prp->bEnabled );		// Must be enabled to set up the Z buffer.
			ASSERT ( 0 == stricmp ( prp->sName.c_str(), "main" ) );

			// Assign manually.
			ASSERT ( pdwColourChannels[0] == 0 );
			DWORD dwChannelMask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;
			prp->dwColourChannelMask = dwChannelMask;
			prp->iTargetIndex = 0;
			prp->psurfTarget = m_ppsurfRenderTargets[0];
			prp->ptexTarget = m_pptexRenderTargets[0];
			pdwColourChannels[0] = dwChannelMask;
			pdwClearColour[0] = MaskFlagsToMask ( dwChannelMask ) & prp->dwClearColour;

			it++;


			// Second one must be the mainalpha one, because it shares the same rendertarget.
			// Partitioning them into two different render passes makes life much simpler - I have
			// this mechanism to sort the order of rendering, so why not use it?
			prp = *it;
			ASSERT ( 0 == stricmp ( prp->sName.c_str(), "mainalpha" ) );
			prp->dwColourChannelMask = dwChannelMask;
			prp->iTargetIndex = 0;
			prp->psurfTarget = m_ppsurfRenderTargets[0];
			prp->ptexTarget = m_pptexRenderTargets[0];
			//pdwClearColour[0] = MaskFlagsToMask ( dwChannelMask ) & prp->dwClearColour;

			it++;


			// Rest done in two passes - first pass assigns passes with >1 channel.
			for ( ; it != lprenderpassMasterList.end(); it++ )
			{
				RenderPass &rp = **it;
				rp.iTargetIndex = -1;
				rp.psurfTarget = NULL;
				if ( !rp.bEnabled )
				{
					continue;
				}

				if ( rp.iNumChannels == 1 )
				{
					// Ignored - done next pass.
					continue;
				}
				else
				{
					ASSERT ( ( rp.iNumChannels >= 2 ) || ( rp.iNumChannels <= 4 ) );
					// For simplicity, must be assigned in order RGBA
					// Could theoretically pack two two-channel ones together RG/BA, but it gets more complex.
					for ( int iRT = 0; iRT < m_iNumRenderTargets; iRT++ )
					{
						if ( pdwColourChannels[iRT] == 0 )
						{
							// This'll do.
							DWORD dwChannelMask = 0;
							switch ( rp.iNumChannels )
							{
							case 2: dwChannelMask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN; break;
							case 3: dwChannelMask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE; break;
							case 4: dwChannelMask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA; break;
							default:
								ASSERT ( false );
								break;
							}
							rp.dwColourChannelMask = dwChannelMask;
							rp.iTargetIndex = iRT;
							rp.psurfTarget = m_ppsurfRenderTargets[iRT];
							rp.ptexTarget = m_pptexRenderTargets[iRT];
							pdwColourChannels[iRT] = dwChannelMask;
							pdwClearColour[iRT] |= MaskFlagsToMask ( dwChannelMask ) & rp.dwClearColour;
							break;
						}
					}
					// Make sure we found a target.
					ASSERT ( rp.psurfTarget != NULL );
				}
			}
		}

		// Second pass - assign single-channel passes.
		for ( std::list<RenderPass*>::iterator it = lprenderpassMasterList.begin(); it != lprenderpassMasterList.end(); it++ )
		{
			RenderPass &rp = **it;
			if ( !rp.bEnabled )
			{
				continue;
			}

			if ( rp.iNumChannels == 1 )
			{
				for ( int iRT = 0; iRT < m_iNumRenderTargets; iRT++ )
				{
					for ( int iChannel = 0; iChannel < 4; iChannel++ )
					{
						DWORD dwChannelMask;
						switch ( iChannel )
						{
							default: ASSERT ( false ); /*fallthrough*/
							case 0: dwChannelMask = D3DCOLORWRITEENABLE_RED; break;
							case 1: dwChannelMask = D3DCOLORWRITEENABLE_GREEN; break;
							case 2: dwChannelMask = D3DCOLORWRITEENABLE_BLUE; break;
							case 3: dwChannelMask = D3DCOLORWRITEENABLE_ALPHA; break;
						}

						if ( !rp.bCanBeAlphaChannel && ( iChannel == 3 ) )
						{
							// This pass can't live in an alpha channel
							continue;
						}

						if ( ( pdwColourChannels[iRT] & dwChannelMask ) == 0 )
						{
							// Channel is free.
							rp.dwColourChannelMask = dwChannelMask;
							rp.iTargetIndex = iRT;
							rp.psurfTarget = m_ppsurfRenderTargets[iRT];
							rp.ptexTarget = m_pptexRenderTargets[iRT];
							pdwColourChannels[iRT] |= dwChannelMask;
							pdwClearColour[iRT] |= MaskFlagsToMask ( dwChannelMask ) & rp.dwClearColour;
							goto found_a_channel;
						}
					}
				}
				// Out of channels!
				ASSERT ( false );
found_a_channel:
				// Make sure we found a target.
				ASSERT ( rp.psurfTarget != NULL );
			}
		}


        m_pd3dDevice->EndScene();


		// Render each pass to the respective rendertargets and channels.
		// Order by rendertargets to minimise SetRenderTarget calls.
		bool bFirstOne = true;
		for ( int iRT = 0; iRT < m_iNumRenderTargets; iRT++ )
		{
			pd3dxRenderToSurface->BeginScene ( m_ppsurfRenderTargets[iRT], &vpTarget );
			//m_pd3dDevice->SetViewport ( &vpTarget );

			if ( bFirstOne )
			{
				// First pass clears the Z buffer.
				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, pdwClearColour[iRT], 1.0f, 0L );
			}
			else
			{
				// Subsequent ones just use it.
				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, pdwClearColour[iRT], 1.0f, 0L );
			}


			for ( std::list<RenderPass*>::iterator it = lprenderpassMasterList.begin(); it != lprenderpassMasterList.end(); it++ )
			{
				RenderPass &rp = **it;
				if ( rp.psurfTarget == m_ppsurfRenderTargets[iRT] )
				{
					ASSERT ( rp.iTargetIndex == iRT );
					if ( bFirstOne )
					{
						// First pass must always be the main one and must always be enabled -
						// there's a couple of assumptions that rely on this, mainly the Z buffer behaviour.
						ASSERT ( rp.bEnabled );
						ASSERT ( 0 == stricmp ( rp.sName.c_str(), "main" ) );
						m_pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
						m_pd3dDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
						bFirstOne = false;
					}
					else
					{
						m_pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
						m_pd3dDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESS );
					}

					// This RP renders to this target. Do it.
					// Mask writes.
					m_pd3dDevice->SetRenderState ( D3DRS_COLORWRITEENABLE, rp.dwColourChannelMask );

					// Some bodgery to set up the "heat" effect light
					if ( 0 == stricmp ( rp.sName.c_str(), "heat" ) )
					{
						D3DLIGHT9 light;
						D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
						m_pd3dDevice->SetLight( 0, &light );
						m_pd3dDevice->LightEnable( 0, TRUE );
						m_pd3dDevice->LightEnable( 1, FALSE );
					}
					else
					{
						D3DLIGHT9 light;
						D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, g_CameraOrn._31, g_CameraOrn._32, g_CameraOrn._33 );
						m_pd3dDevice->SetLight( 1, &light );
						m_pd3dDevice->LightEnable( 0, FALSE );
						m_pd3dDevice->LightEnable( 1, TRUE );
					}

					for ( std::list<RenderPass::RenderPassObject>::iterator it = rp.lRenderPassObjects.begin(); it != rp.lRenderPassObjects.end(); it++ )
					{
						RenderPass::RenderPassObject &rpo = *(it);
						rpo.pObject->Render ( rpo.iPassID );
					}
				}
			}

			pd3dxRenderToSurface->EndScene ( D3DX_FILTER_NONE );
		}


		// And render the whole lot to the screen.
		m_pd3dDevice->SetRenderTarget ( 0, m_psurfBackbuffer );
		m_pd3dDevice->SetDepthStencilSurface ( m_psurfZbuffer );
		m_pd3dDevice->SetViewport ( &vpScreen );
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0L );

        m_pd3dDevice->BeginScene();

		m_pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
		m_pd3dDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_ALWAYS );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		for ( int iStage = 0; iStage < 4; iStage++ )
		{
			// No mipmapping, no wrapping.
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
			m_pd3dDevice->SetSamplerState( iStage, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );
		}
		m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x12345678 );
		m_pd3dDevice->SetRenderState ( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );


		if ( m_dwCombiningPixelShaderType != g_dwRequiredShaderType )
		{
			SAFE_RELEASE ( m_pCombiningPixelShader );
			m_dwCombiningPixelShaderType = g_dwRequiredShaderType;


			// Go through the render pass types, working out what textures have what bits.

			std::string sMain			= MakeNameOfPassSource ( FindPass ( "main" ) );
			std::string sDistort		= MakeNameOfPassSource ( FindPass ( "distort" ) );
			std::string sGreyscale		= MakeNameOfPassSource ( FindPass ( "greyscale" ) );
			std::string sHeat			= MakeNameOfPassSource ( FindPass ( "heat" ) );
			std::string sHeatShow		= MakeNameOfPassSource ( FindPass ( "heatshow" ) );


			std::string sShader = "";
		
			// Set up the samplers.
			for ( int iRT = 0; iRT < m_iNumRenderTargets; iRT++ )
			{
				if ( pdwColourChannels[iRT] != 0 )
				{
					// This one is being used.
					char pcTemp[100];
					sprintf ( pcTemp, "%i", iRT );
					// Sampler.
					sShader += "sampler RenderTarget";
					sShader += pcTemp;
					sShader += " = sampler_state { texture = NULL; minfilter = LINEAR; magfilter = LINEAR; mipfilter = NONE; };	\n";
				}
			}



			sShader +=	"float4 PS(													\n";
			// Annoyingly, PS1.1 and PS1.3 need separate texcoords for each texture.
			sShader +=	"		float2 ScreenUV0 : TEXCOORD0,						\n";
			sShader +=	"		float2 ScreenUV1 : TEXCOORD1,						\n";
			sShader +=	"		float2 ScreenUV2 : TEXCOORD2,						\n";
			sShader +=	"		float2 ScreenUV3 : TEXCOORD3						\n";
			sShader +=	"									) : COLOR				\n";
			sShader +=	"{															\n";


			// Read the required textures.
			for ( int iRT = 0; iRT < m_iNumRenderTargets; iRT++ )
			{
				if ( pdwColourChannels[iRT] != 0 )
				{
					// This one is being used.
					char pcTemp[100];
					sprintf ( pcTemp, "%i", iRT );
					sShader += "	float4 RTRead";
					sShader += pcTemp;
					sShader += " = tex2D(RenderTarget";
					sShader += pcTemp;
					sShader += ", ScreenUV";
					sShader += pcTemp;
					sShader += ");\n";
				}
			}


			// Everything "communicates" through the Result variable.
			sShader += "	float4 Result = {0, 0, 0, 0};					\n";

			// Actually do the computation.

			if ( g_dwRequiredShaderType & CPS_DISTORTION )
			{
				// Distortion requires a dependent read, so it's slightly special.
				RenderPass *prp = FindPass ( "main" );
				ASSERT ( prp != NULL );
				char pcTemp[100];
				sprintf ( pcTemp, "%i", prp->iTargetIndex );

				sShader += "float DistortionConstant = 0.1;\n";		// How much distortion there is.

				sShader +=
					"	float2 DistortCoords = ( "+sDistort+" * 2 - 1 ) * DistortionConstant + ScreenUV0;\n";
				sShader += "	Result = tex2D ( RenderTarget";
				sShader += pcTemp;
				sShader += ", DistortCoords );\n";


				//sShader += "	Result.rg = "+sDistort+";\n";
			}
			else
			{
				sShader += "	Result.rgb = "+sMain+";							\n";
			}

			// Calculate the intensity of the main buffer, because a couple of effects use that.
			// If it's not used, the compiler will bin it. HLSL is ace!
			sShader += "	float Desaturated = dot ( Result.rgb, float3 ( 0.3, 0.5, 0.2 ) );\n";

			if ( g_dwRequiredShaderType & CPS_GREYSCALE )
			{
				// Lerp between the existing colour and the greyscale value based on the greyscale channel.
				// This is slightly annoying because the interpolant for lerp() is clamped between 0 and 1
				// on some cards. Whereas I want to lerp from -1 to +1. So I need to do the lerp the non-standard way.
				sShader +=
					"	float LerpValue = "+sGreyscale+" * 2 - 1;				\n"
					"	float3 Delta = Desaturated - Result;					\n"
					"	Result.rgb += Delta * LerpValue;						\n";

				//sShader += "Result.rgb = "+sGreyscale+";\n";
			}

			if ( g_dwRequiredShaderType & CPS_HEAT )
			{
				// Make the heat colour by lerping from blue to red to yellow.
				sShader +=
					"	float3 Colour0 = { 0, 0, 1 };								\n"
					"	float3 Colour1 = { 1, 0, 0 };								\n"
					"	float3 Colour2 = { 1, 1, 0 };								\n";

				sShader +=
					"	float HeatLerp1 = " + sHeat + ";							\n"
					"	float HeatLerp2 = min(HeatLerp1, (1-HeatLerp1));			\n"
					"	float3 HeatColour = lerp ( Colour0, Colour2, clamp(HeatLerp1,0,1) );	\n"
					"	HeatColour = lerp ( HeatColour, Colour1, clamp(HeatLerp2*2,0,1) );		\n";

				// And then blend to it with the heatshow channel
				sShader +=
					"	Result.rgb = lerp ( Result.rgb, HeatColour, "+sHeatShow+" );\n";
			}


			// And finally.
			sShader +=
				"	return Result;									\n"
				"}													\n";


			LPD3DXBUFFER pErrors = NULL;
			SAFE_RELEASE ( m_pShader );

			OutputDebugString ( sShader.c_str() );

			char *pcShaderVersion;
			switch ( g_iShaderVersion )
			{
			default:
				g_iShaderVersion = 0;
				// Fallthrough.
			case 0:		pcShaderVersion = "ps_1_1";		break;
			case 1:		pcShaderVersion = "ps_1_3";		break;
			case 2:		pcShaderVersion = "ps_1_4";		break;
			case 3:		pcShaderVersion = "ps_2_0";		break;
			case 4:		pcShaderVersion = "ps_3_0";		break;
			}

			HRESULT hr = D3DXCompileShader (
				sShader.c_str(), strlen ( sShader.c_str() ),
				NULL, NULL, "PS", pcShaderVersion, 0, 
				&m_pShader, &pErrors, NULL );

			if ( pErrors != NULL )
			{
				// Output warnings as well.
				char *pcErrors = (char *)pErrors->GetBufferPointer();
				OutputDebugString ( pcErrors );
			}
			if ( SUCCEEDED ( hr ) )
			{
				// Now match the sampler numbers to the names.
				hr = D3DXGetShaderSamplers ( (DWORD*)m_pShader->GetBufferPointer(), m_pcSamplerNames, &m_iNumSamplers );
				if ( FAILED ( hr ) )
				{
					ASSERT ( false );
				}
				for ( int iSampler = 0; iSampler < m_iNumSamplers; iSampler++ )
				{
					ASSERT ( 0 == strncmp ( m_pcSamplerNames[iSampler], "RenderTarget", 12 ) );
					int iSamplerIndex;
					int iArgsRead = sscanf ( m_pcSamplerNames[iSampler] + 12, "%i", &iSamplerIndex );
					ASSERT ( iArgsRead == 1 );
					m_iSamplerIndex[iSampler] = iSamplerIndex;
				}

				// And compile the shader.
				hr = m_pd3dDevice->CreatePixelShader ( (DWORD*)m_pShader->GetBufferPointer(), &m_pCombiningPixelShader );
			}

			SAFE_RELEASE ( pErrors );
		}


		m_pd3dDevice->SetVertexShader ( NULL );
		m_pd3dDevice->SetFVF ( dwMyFVF4T2 );
		MyVertex4T2 vert[4];


		vert[0].fX = (float)( vpScreen.X );
		vert[0].fY = (float)( vpScreen.Y );
		vert[0].fZ = 0.5f;
		vert[0].fRHW = 0.5f;
		vert[0].fU0 = 0.0f;
		vert[0].fV0 = 0.0f;

		vert[1].fX = (float)( vpScreen.X + vpScreen.Width );
		vert[1].fY = (float)( vpScreen.Y );
		vert[1].fZ = 0.5f;
		vert[1].fRHW = 0.5f;
		vert[1].fU0 = 1.0f;
		vert[1].fV0 = 0.0f;

		vert[2].fX = (float)( vpScreen.X + vpScreen.Width );
		vert[2].fY = (float)( vpScreen.Y + vpScreen.Height );
		vert[2].fZ = 0.5f;
		vert[2].fRHW = 0.5f;
		vert[2].fU0 = 1.0f;
		vert[2].fV0 = 1.0f;

		vert[3].fX = (float)( vpScreen.X );
		vert[3].fY = (float)( vpScreen.Y + vpScreen.Height );
		vert[3].fZ = 0.5f;
		vert[3].fRHW = 0.5f;
		vert[3].fU0 = 0.0f;
		vert[3].fV0 = 1.0f;

		for ( int i = 0; i < 4; i++ )
		{
			vert[i].fU1 = vert[i].fU0;
			vert[i].fV1 = vert[i].fV0;
			vert[i].fU2 = vert[i].fU0;
			vert[i].fV2 = vert[i].fV0;
			vert[i].fU3 = vert[i].fU0;
			vert[i].fV3 = vert[i].fV0;
		}

		if ( ( m_pCombiningPixelShader != NULL ) && ( g_iRenderingDebugMode == 0 ) )
		{
			// Managed to make a shader. So use it.
			m_pd3dDevice->SetPixelShader ( m_pCombiningPixelShader );

			// Bind the textures to the right samplers.
			for ( int iSampler = 0; iSampler < m_iNumSamplers; iSampler++ )
			{
				int iRTIndex = m_iSamplerIndex[iSampler];
				ASSERT ( iRTIndex >= 0 );
				ASSERT ( iRTIndex < m_iNumRenderTargets );
				m_pd3dDevice->SetTexture ( iSampler, m_pptexRenderTargets[iRTIndex] );
			}
		}
		else
		{
			int iDebugMode = g_iRenderingDebugMode - 1;

			if ( m_pCombiningPixelShader == NULL )
			{
				// Failed to make a shader. Just display the main buffer.
				iDebugMode = 0;
			}

			int iRT = iDebugMode / 6;
			if ( iRT >= m_iNumRenderTargets )
			{
				g_iRenderingDebugMode = 0;
			}
			else if ( pdwColourChannels[iRT] == 0 )
			{
				// No more used rendertargets.
				g_iRenderingDebugMode = 0;
			}
			else
			{
				// Just draw the given rendertarget on the screen.
				m_pd3dDevice->SetPixelShader ( NULL );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				switch ( iDebugMode % 6 )
				{
				case 0:
					// RGB channels.
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0x00ffffff );
					break;
				case 1:
					// RG channels.
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0x00ffff00 );
					break;
				case 2:
					// Red channel.
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0x00ff0000 );
					break;
				case 3:
					// Green channel
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0x0000ff00 );
					break;
				case 4:
					// Green channel
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0x000000ff );
					break;
				case 5:
					// Alpha channel.
					m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE );
					m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xffffffff );
					break;
				}
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
				m_pd3dDevice->SetTexture ( 0, m_pptexRenderTargets[iRT] );
			}
		}



		m_pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLEFAN, 2, vert, sizeof(vert[0]) );
		m_pd3dDevice->SetPixelShader ( NULL );



        // Render stats and help text  
        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();



		// Coz we called "get" at the start of the scene.
		m_psurfBackbuffer->Release();
		m_psurfZbuffer->Release();

		SAFE_DELETE_ARRAY ( pdwClearColour );
		SAFE_DELETE_ARRAY ( pdwColourChannels );
    }

    return S_OK;
}



TWEAK_INT ( TWEAK_NumLines, "Number of lines", 20 );
TWEAK_INT ( TWEAK_Display, "Display tweaks on screen", 1 );



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

    sprintf( szMsg, TEXT("Left/Right Axis: %0.2f Up/Down Axis: %0.2f "), 
              m_UserInput.fAxisRotateLR, m_UserInput.fAxisRotateUD );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Use arrow keys or joystick to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Press 'F3' to configure input") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	char pcTemp[100];
	if ( g_iRenderingDebugMode == 0 )
	{
		sprintf ( pcTemp, "full scene" );
	}
	else
	{
		int iDebugMode = g_iRenderingDebugMode - 1;
		int iDebugBuffer = iDebugMode / 6;
		switch ( iDebugMode % 6 )
		{
		case 0:		sprintf ( pcTemp, "buffer %i, RGB", iDebugBuffer );			break;
		case 1:		sprintf ( pcTemp, "buffer %i, RG", iDebugBuffer );			break;
		case 2:		sprintf ( pcTemp, "buffer %i, R", iDebugBuffer );			break;
		case 3:		sprintf ( pcTemp, "buffer %i, G", iDebugBuffer );			break;
		case 4:		sprintf ( pcTemp, "buffer %i, B", iDebugBuffer );			break;
		case 5:		sprintf ( pcTemp, "buffer %i, A", iDebugBuffer );			break;
		}
	}
	lstrcpy( szMsg, TEXT("Press 'F9' to cycle debug draw modes: ") );
	strcat ( szMsg, pcTemp );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );


	switch ( g_iShaderVersion )
	{
	case 0:	strcpy ( pcTemp, "ps1.1" ); break;
	case 1:	strcpy ( pcTemp, "ps1.3" ); break;
	case 2:	strcpy ( pcTemp, "ps1.4" ); break;
	case 3:	strcpy ( pcTemp, "ps2.0" ); break;
	case 4:	strcpy ( pcTemp, "ps3.0" ); break;
	}
	if ( m_pCombiningPixelShader == NULL )
	{
		strcat ( pcTemp, " failed" );
	}
	else
	{
		strcat ( pcTemp, " OK" );
	}
	lstrcpy( szMsg, TEXT("Press 'F8' to cycle pixel shader version: ") );
	strcat ( szMsg, pcTemp );
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );


	strcpy ( pcTemp, "" );
	if ( g_dwRequiredShaderType & CPS_DISTORTION ) strcat ( pcTemp, "distortion " );
	if ( g_dwRequiredShaderType & CPS_GREYSCALE ) strcat ( pcTemp, "greyscale " );
	if ( g_dwRequiredShaderType & CPS_HEAT ) strcat ( pcTemp, "thermal " );
	lstrcpy( szMsg, TEXT("Press 'F7' to cycle enabled features: ") );
	strcat ( szMsg, pcTemp );
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );


	// Useful place to do this.
	Tweak::SortTweaksIntoAPrettyList();


	if ( TWEAK_Display )
	{
		Tweak *ptweakCur = Tweak::Current();
		// Forward a bit.
		for ( int iTweak = 0; iTweak < TWEAK_NumLines/2; iTweak++ )
		{
			Tweak *ptweak = ptweakCur->Next();
			if ( ptweak != NULL )
			{
				ptweakCur = ptweak;
			}
		}
		for ( int iTweak = 0; iTweak < TWEAK_NumLines; iTweak++ )
		{
			fNextLine -= 20.0f;

			sprintf( szMsg, TEXT("%s"), ptweakCur->pchName );
			m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
			switch ( ptweakCur->GetType() )
			{
			case Tweak::TweakType_Float:
				sprintf( szMsg, TEXT("%f"), ptweakCur->GetFloat() );
				break;
			case Tweak::TweakType_Int:
				sprintf( szMsg, TEXT("%i"), ptweakCur->GetInt() );
				break;
			default:
				ASSERT ( false );
				break;
			}
			m_pFont->DrawText( 200, fNextLine, fontColor, szMsg );
			sprintf( szMsg, TEXT("%s"), ptweakCur->pchDescription );
			m_pFont->DrawText( 250, fNextLine, fontColor, szMsg );

			ptweakCur = ptweakCur->Prev();
			if ( ptweakCur == NULL )
			{
				break;
			}
		}
	}



    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Called in to toggle the pause state of the app.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::Pause( bool bPause )
{
    // Get access to the list of semantically-mapped input devices
    // to zero the state of all InputDeviceState structs.  This is needed
    // because when using DISCL_FOREGROUND, the action mapper will not 
    // record actions when the focus switches, for example if a dialog appears.
    // This causes a problem when a button held down when loosing focus, and let
    // go when the focus is lost.  The app will not record that the button 
    // has been let go, so the state will be incorrect when focus returns.  
    // To fix this either use DISCL_BACKGROUND or zero the state when 
    // loosing focus.
    CInputDeviceManager::DeviceInfo* pDeviceInfos;
    DWORD dwNumDevices;
    m_pInputDeviceManager->GetDevices( &pDeviceInfos, &dwNumDevices );

    for( DWORD i=0; i<dwNumDevices; i++ )
    {
        InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;
        ZeroMemory( pInputDeviceState, sizeof(InputDeviceState) );
    }

    CD3DApplication::Pause( bPause );
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

        case WM_COMMAND:
        {
            switch( LOWORD(wParam) )
            {
                case IDM_CONFIGINPUT:
                    m_UserInput.bDoConfigureInput = TRUE;
                    break;

                case IDM_CHANGEDEVICE:
                    m_UserInput.bDoConfigureDisplay = TRUE;
                    return 0; // Don't hand off to parent
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

	for ( int i = 0; i < 8; i++ )
	{
		m_pd3dDevice->SetTexture ( i, NULL );
	}
	m_pd3dDevice->SetVertexShader ( NULL );


    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();
    SAFE_RELEASE( m_pDIConfigSurface );

	for ( int i = 0; i < m_iNumRenderTargets; i++ )
	{
		SAFE_RELEASE ( m_ppsurfRenderTargets[i] );
		SAFE_RELEASE ( m_pptexRenderTargets[i] );
	}
	SAFE_DELETE_ARRAY ( m_ppsurfRenderTargets );
	SAFE_DELETE_ARRAY ( m_pptexRenderTargets );
	m_iNumRenderTargets = 0;
	SAFE_RELEASE ( pd3dxRenderToSurface );

	SAFE_RELEASE ( g_ptexDistortionBlob );
	SAFE_RELEASE ( g_ptexBlob );
	SAFE_RELEASE ( g_ptexBackdrop );


	SAFE_RELEASE ( m_pCombiningPixelShader );
	m_dwCombiningPixelShaderType = 0;
	SAFE_RELEASE ( m_pShader );


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
    SAFE_RELEASE( m_pD3DXMesh );

	for ( std::list<Object*>::iterator it = lpobjectMasterList.begin(); it != lpobjectMasterList.end(); it++ )
	{
		Object *pobj = *(it);
		SAFE_RELEASE ( pobj->pD3DXMesh );
		SAFE_DELETE ( pobj );
	}
	lpobjectMasterList.clear();

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



	for ( std::list<RenderPass*>::iterator it = lprenderpassMasterList.begin(); it != lprenderpassMasterList.end(); it++ )
	{
		RenderPass *prp = *it;
		SAFE_DELETE ( prp );
	}
	lprenderpassMasterList.clear();


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
    if( NULL == m_pInputDeviceManager )
        return;

    // Get access to the list of semantically-mapped input devices
    // to delete all InputDeviceState structs
    CInputDeviceManager::DeviceInfo* pDeviceInfos;
    DWORD dwNumDevices;
    m_pInputDeviceManager->GetDevices( &pDeviceInfos, &dwNumDevices );

    for( DWORD i=0; i<dwNumDevices; i++ )
    {
        InputDeviceState* pInputDeviceState = (InputDeviceState*) pDeviceInfos[i].pParam;
        SAFE_DELETE( pInputDeviceState );
        pDeviceInfos[i].pParam = NULL;
    }

    // Cleanup DirectX input objects
    SAFE_DELETE( m_pInputDeviceManager );

}



RenderPass *CMyD3DApplication::FindPass ( std::string sName )
{
	for ( std::list<RenderPass*>::iterator it = lprenderpassMasterList.begin(); it != lprenderpassMasterList.end(); it++ )
	{
		if ( 0 == stricmp ( (*it)->sName.c_str(), sName.c_str() ) )
		{
			return (*it);
			break;
		}
	}
	return NULL;
}




bool CMyD3DApplication::AddObject ( std::string sName, Object *pObject, int iRenderTypeID )
{
	RenderPass *prp = FindPass ( sName );
	if ( prp != NULL )
	{
		prp->AddObject ( pObject, iRenderTypeID );
		return true;
	}
	else
	{
		return false;
	}
}





//-------------------------------------------------------------------------------



RenderPass::RenderPass ( std::string sNameNew, int iNumChannelsNew, DWORD dwClearColourNew, bool bCanBeAlphaChannelNew )
{
	sName = sNameNew;
	iNumChannels = iNumChannelsNew;
	dwClearColour = dwClearColourNew;
	bCanBeAlphaChannel = bCanBeAlphaChannelNew;

	psurfTarget = NULL;
	dwColourChannelMask = 0;

	lRenderPassObjects.clear();
}


RenderPass::~RenderPass ( void )
{
	lRenderPassObjects.clear();
}


void RenderPass::AddObject ( Object *pObject, int iRenderTypeID )
{
	RenderPassObject rpo;
	rpo.iPassID = iRenderTypeID;
	rpo.pObject = pObject;

	lRenderPassObjects.push_back ( rpo );
}


//-------------------------------------------------------------------------------


void Object::Render ( int iRenderTypeID )
{
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &mOrn );

	g_pd3dDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	g_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColour );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	D3DMATERIAL9 mat;
	mat.Diffuse  = D3DXCOLOR(dwColour);
	mat.Specular = D3DXCOLOR((unsigned)0);
	mat.Ambient  = D3DXCOLOR((unsigned)0);
	mat.Emissive = D3DXCOLOR((unsigned)0);
	mat.Power    = 1.0f;



	if ( iRenderTypeID == RenderType_Background )
	{
		// Ugh. Nasty hack to draw a background.

		g_pd3dDevice->SetVertexShader ( NULL );
		g_pd3dDevice->SetFVF ( dwMyFVF );

		MyVertex vert[4];

		D3DVIEWPORT9 vpScreen;
		g_pd3dDevice->GetViewport ( &vpScreen );

		vert[0].fX = (float)( vpScreen.X );
		vert[0].fY = (float)( vpScreen.Y );
		vert[0].fZ = 0.9999f;
		vert[0].fRHW = 0.5f;
		vert[0].fU = 0.0f;
		vert[0].fV = 0.0f;

		vert[1].fX = (float)( vpScreen.X + vpScreen.Width );
		vert[1].fY = (float)( vpScreen.Y );
		vert[1].fZ = 0.9999f;
		vert[1].fRHW = 0.5f;
		vert[1].fU = 1.0f;
		vert[1].fV = 0.0f;

		vert[2].fX = (float)( vpScreen.X + vpScreen.Width );
		vert[2].fY = (float)( vpScreen.Y + vpScreen.Height );
		vert[2].fZ = 0.9999f;
		vert[2].fRHW = 0.5f;
		vert[2].fU = 1.0f;
		vert[2].fV = 1.0f;

		vert[3].fX = (float)( vpScreen.X );
		vert[3].fY = (float)( vpScreen.Y + vpScreen.Height );
		vert[3].fZ = 0.9999f;
		vert[3].fRHW = 0.5f;
		vert[3].fU = 0.0f;
		vert[3].fV = 1.0f;

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTexture ( 0, g_ptexBackdrop );
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		g_pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLEFAN, 2, vert, sizeof ( vert[0] ) );
		return;
	}





	switch ( iRenderTypeID )
	{
	case RenderType_Normal:
		// Normal rendering.
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		break;
	case RenderType_Alpha:
		// Additive alpha rendering.
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		break;
	case RenderType_Greyscale:
		// More/less greyscale rendering.
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		break;

	case RenderType_Heat:
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		mat.Diffuse  = D3DXCOLOR(0xffffffff);

		break;
	}


	g_pd3dDevice->SetMaterial ( &mat );

	pD3DXMesh->DrawSubset(0);

}








ObjectParticleSystem::ObjectParticleSystem ( int iNewMaxNumEmitters )
{
	iMaxNumEmitters = iNewMaxNumEmitters;
	pvEmitterOrigin = new D3DXVECTOR3 [iMaxNumEmitters];
	iNumEmitters = 0;
	iNumParticles = 0;
}

ObjectParticleSystem::~ObjectParticleSystem()
{
	SAFE_DELETE_ARRAY ( pvEmitterOrigin );
}

void ObjectParticleSystem::AddEmitter ( const D3DXVECTOR3 &vPosition )
{
	ASSERT ( iNumEmitters < iMaxNumEmitters );
	pvEmitterOrigin[iNumEmitters] = vPosition;
	iNumEmitters++;
	rlEmitter.Init ( 0, iNumEmitters, 0x1234 );
}

const DWORD dwMyWorldFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
struct MyWorldVertex
{
	float fX, fY, fZ;
	DWORD dwDiffuse;
	float fU, fV;
};


void ObjectParticleSystem::Render ( int iRenderTypeID )
{
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &mOrn );

	g_pd3dDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	g_pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	g_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColour );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	switch ( iRenderTypeID )
	{
	case RenderType_Normal:
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTexture ( 0, NULL );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		break;

	case RenderType_Heat:
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTexture ( 0, g_ptexBlob );

		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		break;

	case RenderType_Alpha:
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTexture ( 0, g_ptexBlob );

		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		break;

	case RenderType_Greyscale:
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTexture ( 0, g_ptexBlob );

		// Alpha-blend rendering. These blobs are offset, so 128 = 0.0.
		// You can do this more correctly with a two-stage render, but we'll try the simple alpha-blend for now.
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;

	case RenderType_Distort:
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTexture ( 0, g_ptexDistortionBlob );

		// Alpha-blend rendering. These blobs are offset, so 128 = 0.0.
		// You can do this more correctly with a two-stage render, but we'll try the simple alpha-blend for now.
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;

	default:
		// Not sure what effect you're after here.
		ASSERT ( false );
		break;
	}

	g_pd3dDevice->SetVertexShader ( NULL );
	g_pd3dDevice->SetFVF ( dwMyWorldFVF );

	// The time interval between particles.
	float fMaxLifetime = rlLifeTime.Max();
	float fTimeBetweenParticles = fMaxLifetime / (float)iNumParticles;

	// Therefore find the fraction and whole number of each particle.
	DWORD dwParticleSeed = (DWORD)( g_fGameTime / fTimeBetweenParticles );
	float fFracTime = g_fGameTime - ( (float)dwParticleSeed * fTimeBetweenParticles );
	float fParticleTime = fFracTime;

	// Find the "up" and "right" unit vectors for the particles.
	D3DXVECTOR3 vUp		= D3DXVECTOR3 ( g_CameraOrn._21, g_CameraOrn._22, g_CameraOrn._23 );
	D3DXVECTOR3 vRight	= D3DXVECTOR3 ( g_CameraOrn._11, g_CameraOrn._12, g_CameraOrn._13 );

	fParticleTime -= fTimeBetweenParticles;
	dwParticleSeed++;

	for ( int iPart = 0; iPart < iNumParticles; iPart++ )
	{
		// Do these now to allow "continue" to work.
		fParticleTime += fTimeBetweenParticles;
		dwParticleSeed--;


		// Find relative lifetime 0-1
		float fLifeTime = rlLifeTime.Value ( dwParticleSeed );
		float fParticleAge = fParticleTime / fLifeTime;
		if ( fParticleAge > 1.0f )
		{
			// Too old.
			continue;
		}

		int iEmitter = rlEmitter.Value ( dwParticleSeed );
		ASSERT ( ( 0 <= iEmitter) && ( iEmitter < iNumEmitters ) );
		D3DXVECTOR3 pvPos = pvEmitterOrigin[iEmitter];
		pvPos.x += fParticleAge * rlMoveX.Value ( dwParticleSeed );
		if ( iRenderTypeID == RenderType_Alpha )
		{
			pvPos.y += fParticleAge * rlMoveY.Value ( dwParticleSeed );
		}
		else
		{
			pvPos.y += 3.0f * fParticleAge * rlMoveY.Value ( dwParticleSeed );
		}
		pvPos.z += fParticleAge * rlMoveZ.Value ( dwParticleSeed );

		// Particles simply grow then shrink according to a 4x*(1-x) curve.
		float fSize = fParticleAge * ( 1.0f - fParticleAge ) * 4.0f;
		float fPhysicalSize = fSize * rlSize.Value ( dwParticleSeed );
		float fShade = fSize * rlShade.Value ( dwParticleSeed );

		DWORD dwColour = 0;
		switch ( iRenderTypeID )
		{
		case RenderType_Heat:
			dwColour = ( (DWORD)( fShade * 255.0f ) );					// Brightness.
			dwColour |= ( dwColour << 8 ) | ( dwColour << 16 ) | ( dwColour << 24 );
			break;

		case RenderType_Alpha:
			// Flame blobs.
			dwColour  = ( (DWORD)( fShade * 255.0f ) ) << 24;			// Brightness.
			dwColour |= ( 255 ) << 16;
			dwColour |= ( (DWORD)( fParticleAge * 250.0f ) ) << 8;
			dwColour |= 0 << 0;

			fPhysicalSize *= 2.0f;
			break;
		case RenderType_Greyscale:
			dwColour = 0x00ffffff & this->dwColour;
			dwColour |= ( (DWORD)( fShade * 255.0f ) ) << 24;
			break;
		case RenderType_Distort:
			dwColour = (DWORD)( fShade * 80.0f );
			dwColour |= ( dwColour << 8 ) | ( dwColour << 16 ) | ( dwColour << 24 );
			break;
		default:
			ASSERT ( false );
			break;
		}

		D3DXVECTOR3 vThisUp		= vUp    * fPhysicalSize;
		D3DXVECTOR3 vThisRight	= vRight * fPhysicalSize;

		// Find the vertices.
		MyWorldVertex verts[4];

		verts[0].fX = pvPos.x - vThisRight.x + vThisUp.x;
		verts[0].fY = pvPos.y - vThisRight.y + vThisUp.y;
		verts[0].fZ = pvPos.z - vThisRight.z + vThisUp.z;
		verts[0].dwDiffuse = dwColour;
		verts[0].fU = 0.0f;
		verts[0].fV = 0.0f;

		verts[1].fX = pvPos.x + vThisRight.x + vThisUp.x;
		verts[1].fY = pvPos.y + vThisRight.y + vThisUp.y;
		verts[1].fZ = pvPos.z + vThisRight.z + vThisUp.z;
		verts[1].dwDiffuse = dwColour;
		verts[1].fU = 1.0f;
		verts[1].fV = 0.0f;

		verts[2].fX = pvPos.x + vThisRight.x - vThisUp.x;
		verts[2].fY = pvPos.y + vThisRight.y - vThisUp.y;
		verts[2].fZ = pvPos.z + vThisRight.z - vThisUp.z;
		verts[2].dwDiffuse = dwColour;
		verts[2].fU = 1.0f;
		verts[2].fV = 1.0f;

		verts[3].fX = pvPos.x - vThisRight.x - vThisUp.x;
		verts[3].fY = pvPos.y - vThisRight.y - vThisUp.y;
		verts[3].fZ = pvPos.z - vThisRight.z - vThisUp.z;
		verts[3].dwDiffuse = dwColour;
		verts[3].fU = 0.0f;
		verts[3].fV = 1.0f;

		g_pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLEFAN, 2, verts, sizeof ( verts[0] ) );
	}

	fParticleTime += fTimeBetweenParticles;

}




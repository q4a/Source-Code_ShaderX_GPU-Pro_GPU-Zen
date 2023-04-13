//-----------------------------------------------------------------------------
// File: SWShaders.cpp
//
// Desc: DirectX MFC dialog application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#include "stdafx.h"
#include "shader.h"
#include "Mandelbrot.h"
#include "SWShaders.h"




//-----------------------------------------------------------------------------
// Application globals
//-----------------------------------------------------------------------------
TCHAR*          g_strAppTitle       = _T( "SW Vertex Shaders" );
CApp            g_App;
HINSTANCE       g_hInst = NULL;
CAppForm*       g_AppFormView = NULL;


bool g_bDataChanging = false;


//-----------------------------------------------------------------------------
// The MFC macros are all listed here
//-----------------------------------------------------------------------------
IMPLEMENT_DYNCREATE( CAppDoc,      CDocument )
IMPLEMENT_DYNCREATE( CAppFrameWnd, CFrameWnd )
IMPLEMENT_DYNCREATE( CAppForm,     CFormView )




BEGIN_MESSAGE_MAP( CApp, CWinApp )
    //{{AFX_MSG_MAP(CApp)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




BEGIN_MESSAGE_MAP( CAppForm, CFormView )
    //{{AFX_MSG_MAP(CAppForm)
    ON_COMMAND(    IDC_VIEWFULLSCREEN, OnToggleFullScreen )
    ON_BN_CLICKED(IDC_CHANGEDEVICE, OnChangeDevice)
	ON_BN_CLICKED(IDC_WIREFRAME, OnWireframe)
	ON_EN_CHANGE(IDC_TESSELLATION, OnChangeTessellation)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TESS_SPIN, OnDeltaposTessSpin)
	ON_EN_CHANGE(IDC_MINX, OnChangeMinx)
	ON_EN_CHANGE(IDC_MINZ, OnChangeMinz)
	ON_EN_CHANGE(IDC_MAXX, OnChangeMaxx)
	ON_EN_CHANGE(IDC_MAXZ, OnChangeMaxz)
	ON_BN_CLICKED(IDC_VS_INITIAL, OnVsInitial)
	ON_BN_CLICKED(IDC_VS_OPT, OnVsOpt)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINX, OnDeltaposSpinMinx)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINZ, OnDeltaposSpinMinz)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXX, OnDeltaposSpinMaxx)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXZ, OnDeltaposSpinMaxz)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




BEGIN_MESSAGE_MAP(CAppDoc, CDocument)
    //{{AFX_MSG_MAP(CAppDoc)
            // NOTE - the ClassWizard will add and remove mapping macros here.
            //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




BEGIN_MESSAGE_MAP(CAppFrameWnd, CFrameWnd)
    //{{AFX_MSG_MAP(CAppFrameWnd)
    ON_COMMAND(IDM_CHANGEDEVICE, OnChangeDevice)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




//-----------------------------------------------------------------------------
// Name: CAppForm()
// Desc: Constructor for the dialog resource form.  Paired with ~CAppForm()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CAppForm::CAppForm()
         :CFormView( IDD_FORMVIEW )
{
    //{{AFX_DATA_INIT(CAppForm)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    g_AppFormView          = this;
    m_hwndRenderWindow     = NULL;
    m_hwndRenderFullScreen = NULL;
    m_hWndTopLevelParent   = NULL;

    // Override some CD3DApplication defaults:
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "SWShaders" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

	// Setup the shaders
	m_pMandelbrotShader = NULL;
	m_pCurrentShader = NULL;
	m_ShaderVersion = SHADER_INITIAL;

	m_bWireframe = false;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CAppForm::OneTimeSceneInit()
{
	// Default to SW vertex processing
	m_d3dSettings.SetVertexProcessingType(SOFTWARE_VP);

    // Setup the view matrix
    D3DXVECTOR3 vEye = D3DXVECTOR3( 2.0f, 3.0f, 3.0f );
    D3DXVECTOR3 vAt  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtRH( &m_matView, &vEye, &vAt, &vUp );

    // Set the position matrix
    D3DXMatrixInverse( &m_matPosition, NULL, &m_matView );

    m_bLoadingApp = FALSE;

	m_pMandelbrotShader = new MandelbrotShader( "Mandelbrot_Initial.vsh", "Mandelbrot_Opt.vsh" );

	m_pCurrentShader = m_pMandelbrotShader;
	m_pCurrentShader->SetActiveShader( SHADER_INITIAL );
	CheckRadioButton( IDC_VS_INITIAL, IDC_VS_OPT, IDC_VS_INITIAL );

	g_bDataChanging = true;
	SetDlgItemText( IDC_MINX, "-2.0" );
	SetDlgItemText( IDC_MINZ, "-2.0" );
	SetDlgItemText( IDC_MAXX, "2.0" );
	SetDlgItemText( IDC_MAXZ, "2.0" );

	m_pMandelbrotShader->SetMinx( -2.0f );
	m_pMandelbrotShader->SetMinz( -2.0f );
	m_pMandelbrotShader->SetMaxx( 2.0f );
	m_pMandelbrotShader->SetMaxz( 2.0f );

	SetDlgItemInt( IDC_TESSELLATION, 180, FALSE );
	g_bDataChanging = false;
    return S_OK;
}









//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CAppForm::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( pCaps );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( Format );
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CAppForm::InitDeviceObjects()
{

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	m_pMandelbrotShader->InitDeviceObjects( m_pd3dDevice, m_d3dSettings );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CAppForm::RestoreDeviceObjects()
{
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
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    FLOAT fAspectRatio = (FLOAT)m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovRH( &m_matProj, D3DXToRadian(60.0f), fAspectRatio, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();

	m_pMandelbrotShader->RestoreDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CAppForm::FrameMove()
{
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

    D3DXMatrixMultiply( &m_matWorld, &matRotX, &matRotY );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    D3DXMatrixInverse( &m_matView, NULL, &m_matPosition );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );


	m_pCurrentShader->FrameMove( m_fElapsedTime, m_matWorld, m_matView, m_matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CAppForm::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bRotateDown  = ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bRotateLeft  = ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bRotateRight = ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );
}




//-----------------------------------------------------------------------------
// Name: CAppForm::Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CAppForm::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x00457ECD, 1.0f, 0L );

	if( m_bWireframe )
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {

		if( m_pCurrentShader )
			m_pCurrentShader->Render();

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
HRESULT CAppForm::RenderText()
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

	CString cVal;
	float xmin, xmax, zmin, zmax;
	GetDlgItemText( IDC_MINX, cVal );
	xmin = (float)atof( (LPCTSTR)cVal );
	GetDlgItemText( IDC_MAXX, cVal );
	xmax = (float)atof( (LPCTSTR)cVal );
	GetDlgItemText( IDC_MINZ, cVal );
	zmin = (float)atof( (LPCTSTR)cVal );
	GetDlgItemText( IDC_MAXZ, cVal );
	zmax = (float)atof( (LPCTSTR)cVal );
    sprintf( szMsg, TEXT("Range: (%5.2f,%5.2f)-(%5.2f,%5.2f)"), xmin, zmin, xmax, zmax );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    wsprintf( szMsg, TEXT("Shader: %s"), (m_ShaderVersion == SHADER_INITIAL) ? "Initial" : "Optimized" );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DoDataExchange()
// Desc: DDX/DDV support
//-----------------------------------------------------------------------------
void CAppForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppForm)
	//}}AFX_DATA_MAP
}




//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CAppForm::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    return CFormView ::WindowProc(message, wParam, lParam);
}




//-----------------------------------------------------------------------------
// Name: OnChangeDevice()
// Desc: Needed to enable dlg menu item 
//-----------------------------------------------------------------------------
void CAppFrameWnd::OnChangeDevice() 
{
    g_AppFormView->OnChangeDevice();
}




//-----------------------------------------------------------------------------
// Name: CAppForm::InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CAppForm::InvalidateDeviceObjects()
{
    m_pFont->InvalidateDeviceObjects();

	m_pMandelbrotShader->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CAppForm::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();

	m_pMandelbrotShader->DeleteDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CAppForm::FinalCleanup()
{
    SAFE_DELETE( m_pFont );

	SAFE_DELETE( m_pMandelbrotShader );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitInstance()
// Desc: This is the main entry point for the application. The MFC window stuff
//       is initialized here. See also the main initialization routine for the
//       CAppForm class, which is called indirectly from here.
//-----------------------------------------------------------------------------
BOOL CApp::InitInstance()
{
    // Asscociate the MFC app with the frame window and doc/view classes
    AddDocTemplate( new CSingleDocTemplate( IDR_MAINFRAME,
                                            RUNTIME_CLASS(CAppDoc),
                                            RUNTIME_CLASS(CAppFrameWnd),
                                            RUNTIME_CLASS(CAppForm) ) );

    // Dispatch commands specified on the command line (req'd by MFC). This
    // also initializes the the CAppDoc, CAppFrameWnd, and CAppForm classes.
    CCommandLineInfo cmdInfo;
    ParseCommandLine( cmdInfo );
    if( !ProcessShellCommand( cmdInfo ) )
        return FALSE;

    if( !g_AppFormView->IsReady() )
        return FALSE;

    g_AppFormView->GetParentFrame()->RecalcLayout();
    g_AppFormView->ResizeParentToFit( FALSE ); 
    
    m_pMainWnd->SetWindowText( g_strAppTitle );
    m_pMainWnd->UpdateWindow();

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: LoadFrame()
// Desc: Uses idle time to render the 3D scene.
//-----------------------------------------------------------------------------
BOOL CAppFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
    BOOL bResult = CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
    
    LoadAccelTable( MAKEINTRESOURCE(IDR_MAIN_ACCEL) );

    return bResult;
}




//-----------------------------------------------------------------------------
// Name: OnIdle()
// Desc: Uses idle time to render the 3D scene.
//-----------------------------------------------------------------------------
BOOL CApp::OnIdle( LONG )
{
    // Do not render if the app is minimized
    if( m_pMainWnd->IsIconic() )
        return FALSE;

    TCHAR strStatsPrev[200];

    lstrcpy(strStatsPrev, g_AppFormView->PstrFrameStats());

    // Update and render a frame
    if( g_AppFormView->IsReady() )
    {
        g_AppFormView->CheckForLostFullscreen();
        g_AppFormView->RenderScene();
    }

    // Keep requesting more idle time
    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: PreCreateWindow()
// Desc: Change the window style (so it cannot maximize or be sized) before
//       the main frame window is created.
//-----------------------------------------------------------------------------
BOOL CAppFrameWnd::PreCreateWindow( CREATESTRUCT& cs )
{
    cs.style = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;

    return CFrameWnd::PreCreateWindow( cs );
}




//-----------------------------------------------------------------------------
// Name: ~CAppForm()
// Desc: Destructor for the dialog resource form. Shuts down the app
//-----------------------------------------------------------------------------
CAppForm::~CAppForm()
{
    Cleanup3DEnvironment();
    SAFE_RELEASE( m_pD3D );
    FinalCleanup();
}




//-----------------------------------------------------------------------------
// Name: OnToggleFullScreen()
// Desc: Called when user toggles the fullscreen mode
//-----------------------------------------------------------------------------
void CAppForm::OnToggleFullScreen()
{
    ToggleFullscreen();
}




//-----------------------------------------------------------------------------
// Name: OnChangeDevice()
// Desc: Use hit the "Change Device.." button. Display the dialog for the user
//       to select a new device/mode, and call Change3DEnvironment to
//       use the new device/mode.
//-----------------------------------------------------------------------------
VOID CAppForm::OnChangeDevice()
{
    Pause(true);

    UserSelectNewDevice();

    // Update UI
    UpdateUIForDeviceCapabilites();

    Pause(false);
}




//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Adjusts the window properties for windowed or fullscreen mode
//-----------------------------------------------------------------------------
HRESULT CAppForm::AdjustWindowForChange()
{
    if( m_bWindowed )
    {
        ::ShowWindow( m_hwndRenderFullScreen, SW_HIDE );
        CD3DApplication::m_hWnd = m_hwndRenderWindow;
    }
    else
    {
        if( ::IsIconic( m_hwndRenderFullScreen ) )
            ::ShowWindow( m_hwndRenderFullScreen, SW_RESTORE );
        ::ShowWindow( m_hwndRenderFullScreen, SW_SHOW );
        CD3DApplication::m_hWnd = m_hwndRenderFullScreen;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FullScreenWndProc()
// Desc: The WndProc funtion used when the app is in fullscreen mode. This is
//       needed simply to trap the ESC key.
//-----------------------------------------------------------------------------
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
	NM_UPDOWN nmhdr;
	LRESULT lr;

    if( msg == WM_CLOSE )
    {
        // User wants to exit, so go back to windowed mode and exit for real
        g_AppFormView->OnToggleFullScreen();
        g_App.GetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
    }
    else if( msg == WM_SETCURSOR )
    {
        SetCursor( NULL );
    }
	else if( msg == WM_CHAR )
	{
		switch( wParam  )
		{
			case ',':
				nmhdr.iDelta = 1;
				g_AppFormView->OnDeltaposSpinMinx( (NMHDR *)&nmhdr, &lr );
				break;
			case '.':
				nmhdr.iDelta = -1;
				g_AppFormView->OnDeltaposSpinMinx( (NMHDR *)&nmhdr, &lr );
				break;
			case '<':
				nmhdr.iDelta = 10;
				g_AppFormView->OnDeltaposSpinMinx( (NMHDR *)&nmhdr, &lr );
				break;
			case '>':
				nmhdr.iDelta = -10;
				g_AppFormView->OnDeltaposSpinMinx( (NMHDR *)&nmhdr, &lr );
				break;

			case ';':
				nmhdr.iDelta = 1;
				g_AppFormView->OnDeltaposSpinMinz( (NMHDR *)&nmhdr, &lr );
				break;
			case '\'':
				nmhdr.iDelta = -1;
				g_AppFormView->OnDeltaposSpinMinz( (NMHDR *)&nmhdr, &lr );
				break;
			case ':':
				nmhdr.iDelta = 10;
				g_AppFormView->OnDeltaposSpinMinz( (NMHDR *)&nmhdr, &lr );
				break;
			case '\"':
				nmhdr.iDelta = -10;
				g_AppFormView->OnDeltaposSpinMinz( (NMHDR *)&nmhdr, &lr );
				break;

			case '[':
				nmhdr.iDelta = 1;
				g_AppFormView->OnDeltaposSpinMaxx( (NMHDR *)&nmhdr, &lr );
				break;
			case ']':
				nmhdr.iDelta = -1;
				g_AppFormView->OnDeltaposSpinMaxx( (NMHDR *)&nmhdr, &lr );
				break;
			case '{':
				nmhdr.iDelta = 10;
				g_AppFormView->OnDeltaposSpinMaxx( (NMHDR *)&nmhdr, &lr );
				break;
			case '}':
				nmhdr.iDelta = -10;
				g_AppFormView->OnDeltaposSpinMaxx( (NMHDR *)&nmhdr, &lr );
				break;

			case '=':
				nmhdr.iDelta = 1;
				g_AppFormView->OnDeltaposSpinMaxz( (NMHDR *)&nmhdr, &lr );
				break;
			case '\\':
				nmhdr.iDelta = -1;
				g_AppFormView->OnDeltaposSpinMaxz( (NMHDR *)&nmhdr, &lr );
				break;
			case '+':
				nmhdr.iDelta = 10;
				g_AppFormView->OnDeltaposSpinMaxz( (NMHDR *)&nmhdr, &lr );
				break;
			case '|':
				nmhdr.iDelta = -10;
				g_AppFormView->OnDeltaposSpinMaxz( (NMHDR *)&nmhdr, &lr );
				break;
		}
	}
    else if( msg == WM_KEYDOWN ) 
    {
		switch( wParam  )
		{
			case VK_ESCAPE:
			case 'f': case 'F':
				// User wants to leave fullscreen mode
				g_AppFormView->OnToggleFullScreen();
				break;
			case 'w' : case 'W':
				g_AppFormView->m_bWireframe = !g_AppFormView->m_bWireframe;
				break;
			case 'i': case 'I':
				g_AppFormView->OnVsInitial();
				break;
			case 'o': case 'O':
				g_AppFormView->OnVsOpt();
				break;
		}
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: CheckForLostFullscreen()
// Desc: If fullscreen and device was lost (probably due to alt-tab), 
//       automatically switch to windowed mode
//-----------------------------------------------------------------------------
HRESULT CAppForm::CheckForLostFullscreen()
{
    HRESULT hr;

    if( m_bWindowed )
        return S_OK;

    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
        ForceWindowed();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateUIForDeviceCapabilites()
// Desc: Whenever we get a new device, call this function to enable/disable the
//       appropiate UI controls to match the device's capabilities.
//-----------------------------------------------------------------------------
VOID CAppForm::UpdateUIForDeviceCapabilites()
{
    // TODO: Check the capabilities of the device and update the UI as needed
    DWORD dwCaps = m_d3dCaps.RasterCaps;
    UNREFERENCED_PARAMETER( dwCaps );
}




//-----------------------------------------------------------------------------
// Name: OnInitialUpdate()
// Desc: When the AppForm object is created, this function is called to
//       initialize it. Here we getting access ptrs to some of the controls,
//       and setting the initial state of some of them as well.
//-----------------------------------------------------------------------------
VOID CAppForm::OnInitialUpdate()
{
    // Update the UI
    CFormView::OnInitialUpdate();

    // Get the top level parent hwnd
    m_hWndTopLevelParent = GetTopLevelParent()->GetSafeHwnd();

    // Save static reference to the render window
    m_hwndRenderWindow = GetDlgItem(IDC_RENDERVIEW)->GetSafeHwnd();

    // Register a class for a fullscreen window
    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
                          NULL, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
                          _T("Fullscreen Window") };
    RegisterClass( &wndClass );

    // We create the fullscreen window (not visible) at startup, so it can
    // be the focus window.  The focus window can only be set at CreateDevice
    // time, not in a Reset, so ToggleFullscreen wouldn't work unless we have
    // already set up the fullscreen focus window.
    m_hwndRenderFullScreen = CreateWindow( _T("Fullscreen Window"), NULL,
                                           WS_POPUP, CW_USEDEFAULT,
                                           CW_USEDEFAULT, 100, 100,
                                           m_hWndTopLevelParent, 0L, NULL, 0L );

    // Note that for the MFC samples, the device window and focus window
    // are not the same.
    CD3DApplication::m_hWnd = m_hwndRenderWindow;
    CD3DApplication::m_hWndFocus = m_hwndRenderFullScreen;
    CD3DApplication::Create( AfxGetInstanceHandle() );

    // TODO: Update the UI as needed
}


void CAppForm::OnWireframe() 
{
	m_bWireframe = IsDlgButtonChecked( IDC_WIREFRAME ) ? true : false;
}

void CAppForm::OnChangeTessellation() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView ::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if( g_bDataChanging ) return;
	g_bDataChanging = true;
	BOOL trans;
	int iTess = GetDlgItemInt( IDC_TESSELLATION, &trans, FALSE );

	if( iTess < 5 )
	{
		iTess = 5;
		SetDlgItemInt( IDC_TESSELLATION, 5, FALSE );
	}
	if( iTess > 181 )
	{
		iTess = 181;
		SetDlgItemInt( IDC_TESSELLATION, 181, FALSE );
	}
	g_bDataChanging = false;
	m_pMandelbrotShader->SetTessellation( iTess );

}

void CAppForm::OnDeltaposTessSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	BOOL trans;
	int iTess = GetDlgItemInt( IDC_TESSELLATION, &trans, FALSE );
	iTess = iTess - pNMUpDown->iDelta;
	SetDlgItemInt( IDC_TESSELLATION, iTess, FALSE );
		
	*pResult = 0;
}

void CAppForm::OnChangeMinx() 
{
	if( g_bDataChanging ) return;
	g_bDataChanging = true;
	CString cVal;
	float fVal;

	GetDlgItemText( IDC_MINX, cVal );
	fVal = (float)atof( (LPCTSTR)cVal );
	g_bDataChanging = false;
	if( (fVal < -2.0f ) || (fVal > 2.0f ) ) return;

	m_pMandelbrotShader->SetMinx( fVal );
}

void CAppForm::OnChangeMinz() 
{
	if( g_bDataChanging ) return;
	g_bDataChanging = true;
	CString cVal;
	float fVal;

	GetDlgItemText( IDC_MINZ, cVal );
	fVal = (float)atof( (LPCTSTR)cVal );
	g_bDataChanging = false;
	if( (fVal < -2.0f ) || (fVal > 2.0f ) ) return;

	m_pMandelbrotShader->SetMinz( fVal );
}

void CAppForm::OnChangeMaxx() 
{
	if( g_bDataChanging ) return;
	g_bDataChanging = true;
	CString cVal;
	float fVal;

	GetDlgItemText( IDC_MAXX, cVal );
	fVal = (float)atof( (LPCTSTR)cVal );
	g_bDataChanging = false;
	if( (fVal < -2.0f ) || (fVal > 2.0f ) ) return;

	m_pMandelbrotShader->SetMaxx( fVal );
}

void CAppForm::OnChangeMaxz() 
{
	if( g_bDataChanging ) return;
	g_bDataChanging = true;
	CString cVal;
	float fVal;

	GetDlgItemText( IDC_MAXZ, cVal );
	fVal = (float)atof( (LPCTSTR)cVal );
	g_bDataChanging = false;
	if( (fVal < -2.0f ) || (fVal > 2.0f ) )
		return;

	m_pMandelbrotShader->SetMaxz( fVal );
}

void CAppForm::OnVsInitial() 
{
	m_ShaderVersion = SHADER_INITIAL;
	if( m_pCurrentShader )	
		m_pCurrentShader->SetActiveShader( m_ShaderVersion );
	CheckRadioButton( IDC_VS_INITIAL, IDC_VS_OPT, IDC_VS_INITIAL );
}

void CAppForm::OnVsOpt() 
{
	m_ShaderVersion = SHADER_OPT;
	if( m_pCurrentShader )	
		m_pCurrentShader->SetActiveShader( m_ShaderVersion );
	CheckRadioButton( IDC_VS_INITIAL, IDC_VS_OPT, IDC_VS_OPT );
}

void CAppForm::OnDeltaposSpinMinx(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString cVal;
	float fVal;

	GetDlgItemText( IDC_MINX, cVal );
	fVal = (float)atof( (LPCTSTR)cVal );
	fVal -= pNMUpDown->iDelta * 0.01f;
	if( (fVal < -2.0f ) || (fVal > 2.0f ) ) return;

	cVal.Format( "%f", fVal );
	SetDlgItemText( IDC_MINX, cVal );

	m_pMandelbrotShader->SetMinx( fVal );

	*pResult = 0;
}

void CAppForm::OnDeltaposSpinMinz(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString cVal;
	float fVal;

	GetDlgItemText( IDC_MINZ, cVal );
	fVal = (float)atof( (LPCTSTR)cVal );
	fVal -= pNMUpDown->iDelta * 0.01f;
	if( (fVal < -2.0f ) || (fVal > 2.0f ) ) return;

	cVal.Format( "%f", fVal );
	SetDlgItemText( IDC_MINZ, cVal );

	m_pMandelbrotShader->SetMinz( fVal );

	*pResult = 0;
}

void CAppForm::OnDeltaposSpinMaxx(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString cVal;
	float fVal;

	GetDlgItemText( IDC_MAXX, cVal );
	fVal = (float)atof( (LPCTSTR)cVal );
	fVal -= pNMUpDown->iDelta * 0.01f;
	if( (fVal < -2.0f ) || (fVal > 2.0f ) ) return;

	cVal.Format( "%f", fVal );
	SetDlgItemText( IDC_MAXX, cVal );

	m_pMandelbrotShader->SetMaxx( fVal );
	
	*pResult = 0;
}

void CAppForm::OnDeltaposSpinMaxz(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString cVal;
	float fVal;

	GetDlgItemText( IDC_MAXZ, cVal );
	fVal = (float)atof( (LPCTSTR)cVal );
	fVal -= pNMUpDown->iDelta * 0.01f;
	if( (fVal < -2.0f ) || (fVal > 2.0f ) ) return;

	cVal.Format( "%f", fVal );
	SetDlgItemText( IDC_MAXZ, cVal );

	m_pMandelbrotShader->SetMaxz( fVal );
	
	*pResult = 0;
}

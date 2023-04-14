//-----------------------------------------------------------------------------
// File: LayeredFog.cpp
//
// Desc: 
//		Layer Fog - Code Sample for ShaderX 2nd Edition
//		Guillaume WERLE, April 2003
//		guille@free.fr
//		http://cocoon.planet-d.net/guille/
//-----------------------------------------------------------------------------


#define STRICT
#include "stdafx.h"
#include "LayeredFog.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// Application globals
//-----------------------------------------------------------------------------
TCHAR*          g_strAppTitle       = _T( "LayeredFog" );
CApp            g_App;
HINSTANCE       g_hInst = NULL;
CAppForm*       g_AppFormView = NULL;




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
    //}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FOG_COLOR_PICK_BUTTON, OnBnClickedFogColorPickButton)
//	ON_WM_SHOWWINDOW()
//	ON_WM_KEYDOWN()
//ON_WM_KEYDOWN()
//ON_WM_KEYUP()
//ON_WM_SYSCHAR()
//ON_WM_SYSKEYDOWN()
//ON_WM_SYSKEYUP()
//ON_WM_CHAR()
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
    m_strWindowTitle            = TEXT( "LayeredFog" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
	m_pScene = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = -1.589f;

	m_Position = D3DXVECTOR3(46.0f,26.6f,-1.94f);
	ZeroMemory(m_CustomColor, sizeof(m_CustomColor));

	m_FogColor = D3DXCOLOR(0.5f ,0.5f, 0.1f, 0.7f);


    // Read settings from registry
    ReadSettings();
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
    // TODO: perform one time initialization

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReadSettings()
// Desc: Read the app settings from the registry
//-----------------------------------------------------------------------------
VOID CAppForm::ReadSettings()
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
VOID CAppForm::WriteSettings()
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

	if (dwBehavior & D3DCREATE_PUREDEVICE)	return E_FAIL;

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
    // TODO: create device objects

    HRESULT hr;

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	
	{
		DWORD Options = D3DXMESH_MANAGED;
		DWORD NbMaterial = 0;

		hr = D3DXLoadMeshFromX("castle.x", Options, m_pd3dDevice, NULL, NULL, NULL, &NbMaterial, &m_pScene);
		assert(SUCCEEDED(hr));

	}

	hr = D3DXCreateTextureFromFile(m_pd3dDevice, "LightingMap.tga", &m_pTexture);
	assert(SUCCEEDED(hr));

	LPD3DXBUFFER pErrors = NULL;
	hr = D3DXCreateEffectFromFile(m_pd3dDevice, "Fog.fx", NULL, NULL, 0, NULL, &m_pEffect, &pErrors);
	if (FAILED(hr))
	{
		::MessageBox(0, (const char *)pErrors->GetBufferPointer(), "Effect error", MB_OK);
		assert(NULL);
	}
	SAFE_RELEASE(pErrors);

	

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
    // TODO: setup render states

    // Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 0.25f, 0.25f, 0.25f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE4X );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        D3DCOLOR_RGBA(255,255,255,255) );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f ) + m_Position;
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) + m_Position;
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );	

    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian(60), fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up lighting states
    D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));

	light.Type		= D3DLIGHT_POINT;
	light.Position	= D3DXVECTOR3(0,20,0);
	light.Direction	= D3DXVECTOR3(0,-1,0);
	light.Diffuse	= D3DXCOLOR(1.0f,0.25f,0.25f,1.0f) * 1.0f;
	light.Ambient	= D3DXCOLOR(0,0,0,0);
	light.Specular	= D3DXCOLOR(0,0,0,0);

	float Range = 10;
	light.Range	= Range;
	light.Attenuation0 = 0;
	light.Attenuation1 = Range * Range;


    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();

	m_pEffect->OnResetDevice();

	m_DensitySlider.SetRange(0, 255, false);
	m_DensitySlider.SetPos(128);

	m_LowRangeSlider.SetRange(-10, 60, false);
	m_LowRangeSlider.SetPos(10);

	m_HighRangeSlider.SetRange(-10, 60, false);
	m_HighRangeSlider.SetPos(30);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CAppForm::FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CAppForm::FrameMove()
{
    // TODO: update world

    // Update user input state
    UpdateInput( &m_UserInput );

    // Update the world state according to user input

	const float Speed		= 4.0f;
	const float RotSpeed	= 2.0f;

	if( m_UserInput.bRotRight)	m_fWorldRotY += m_fElapsedTime * RotSpeed;
	if( m_UserInput.bRotLeft)	m_fWorldRotY -= m_fElapsedTime * RotSpeed;

	D3DXMATRIX matRotY;
	D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

	if (m_UserInput.bMoveUp)	m_Position.y += m_fElapsedTime * Speed;
	if (m_UserInput.bMoveDown)	m_Position.y -= m_fElapsedTime * Speed;


	D3DXVECTOR4 Tmp;
	D3DXVECTOR3 DirFront(0,0,1), DirLeft(-1,0,0);

	DirFront	= (D3DXVECTOR3)(*D3DXVec3Transform(&Tmp, &DirFront, &matRotY));	
	DirLeft		= (D3DXVECTOR3)(*D3DXVec3Transform(&Tmp, &DirLeft, &matRotY));	

	if (m_UserInput.bMoveFront)	m_Position += DirFront * m_fElapsedTime * Speed;
	if (m_UserInput.bMoveBack)	m_Position -= DirFront * m_fElapsedTime * Speed;

	if (m_UserInput.bMoveLeft)	m_Position += DirLeft * m_fElapsedTime * Speed;
	if (m_UserInput.bMoveRight)	m_Position -= DirLeft * m_fElapsedTime * Speed;


	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the
	// origin, and define "up" to be in the y-direction.
	D3DXMATRIX matView;
	D3DXVECTOR3 vFromPt   = m_Position;
	D3DXVECTOR3 vLookatPt = m_Position + DirFront;
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );	

	D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CAppForm::UpdateInput( UserInput* pUserInput )
{
#define GET_ASYNC_KEY(_KeyCode)   (m_bActive&&(GetAsyncKeyState(_KeyCode)&0x8000)==0x8000)

	pUserInput->bMoveUp		= GET_ASYNC_KEY(VK_PRIOR);
	pUserInput->bMoveDown	= GET_ASYNC_KEY(VK_NEXT);
    pUserInput->bMoveFront	= GET_ASYNC_KEY(VK_UP);
	pUserInput->bMoveBack	= GET_ASYNC_KEY(VK_DOWN);
    pUserInput->bRotLeft	= GET_ASYNC_KEY(VK_LEFT);
    pUserInput->bRotRight	= GET_ASYNC_KEY(VK_RIGHT);
	pUserInput->bMoveLeft	= GET_ASYNC_KEY('O');
	pUserInput->bMoveRight	= GET_ASYNC_KEY('P');

}




//-----------------------------------------------------------------------------
// Name: CAppForm::Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CAppForm::Render()
{
	float Density = float(m_DensitySlider.GetPos()) / 255.0f;

	D3DXCOLOR FogColor = m_FogColor;
	FogColor.a = Density;				// copy the density in the alpha channel

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         (D3DCOLOR)FogColor, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        
        D3DXMATRIX	World;
		D3DXMatrixRotationX(&World, D3DXToRadian(-90));	// the model was exported from 3dsmax, we need swap the y/z axis
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &World);
		
		m_pd3dDevice->SetTexture(0, m_pTexture);		// lightmap texture in stage 0
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

		D3DXMATRIX WorldT, View, Proj, WVP, WVPT, ViewI;

		m_pd3dDevice->GetTransform(D3DTS_VIEW, &View);
		m_pd3dDevice->GetTransform(D3DTS_PROJECTION, &Proj);

		// World * View * Projection matrix constant
		WVP = World * View * Proj;
		D3DXMatrixTranspose(&WVPT, &WVP);
		m_pd3dDevice->SetVertexShaderConstantF(0, (float*)&WVPT, 4);

		// World matrix constant
		D3DXMatrixTranspose(&WorldT, &World);
		m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&WorldT, 4);


		/*
		 *	compute the fog range constant	
		 */
		float LowRange = (float)m_LowRangeSlider.GetPos();
		float HighRange = (float)m_HighRangeSlider.GetPos();
		float InvFogDelta = 1.0f / (HighRange - LowRange);

		float FogConst[] = { HighRange, InvFogDelta, 0.0f, 1.0f };
		m_pd3dDevice->SetVertexShaderConstantF(8, FogConst, 1);

		m_pd3dDevice->SetPixelShaderConstantF(0, (float*)&FogColor, 1);

		/*
		 *	compute the camera fog density
		 */
		float cd = (HighRange - m_Position.y) * InvFogDelta;
		cd = min(1,cd);
		cd = max(0,cd);
		D3DXCOLOR CameraDensity(cd,cd,cd,cd);
		m_pd3dDevice->SetPixelShaderConstantF(1, (float*)&CameraDensity, 1);

		D3DXCOLOR RedMask(1,0,0,0);
		m_pd3dDevice->SetPixelShaderConstantF(2, (float*)&RedMask, 1);


		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_ONE);
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_ZERO);

		m_pEffect->SetTechnique("tec0");

		UINT NbPass;


		/*
		 *	render the castle with a simple texture
		 */
		m_pEffect->Begin(&NbPass,D3DXFX_DONOTSAVESTATE);
		for (UINT iPass=0; iPass<NbPass; ++iPass)
		{
			m_pEffect->Pass(iPass);
			m_pScene->DrawSubset(0);		
		}
		m_pEffect->End();





		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA);

		m_pEffect->SetTechnique("tec1");
		
		/*
		 *	add the fog layer 
		 */
		m_pEffect->Begin(&NbPass,D3DXFX_DONOTSAVESTATE);
		for (iPass=0; iPass<NbPass; ++iPass)
		{
			m_pEffect->Pass(iPass);
			m_pScene->DrawSubset(0);		
		}
		m_pEffect->End();



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
	D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
    D3DCOLOR fontColorBg        = D3DCOLOR_ARGB(255,0,0,0);


#define nl "\n"
#define tab "   "

	TCHAR szTopMsg[] = 
	{
		"Guillaume WERLE for ShaderX 2 - April 2003" nl
		"http://cocoon.planet-d.net/guille/ " nl
	};

	TCHAR szMsg[] = 
	{
		"Controls : " nl
		tab "Arrow keys to move" nl
		tab "O - P to straf" nl
		tab "PageUp - PageDown to climb up or down" nl
	};


	m_pFont->DrawText(8+2, 5+2, fontColorBg, szTopMsg);
	m_pFont->DrawText(8, 5, fontColor, szTopMsg);

	float YPos = (FLOAT) m_d3dsdBackBuffer.Height - 20 * (4 + 1); 
	m_pFont->DrawText(8+2, YPos+2, fontColorBg, szMsg);
	m_pFont->DrawText(8, YPos, fontColor, szMsg);

/*
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
    wsprintf( szMsg, TEXT("Arrow keys: Up=%d Down=%d Left=%d Right=%d"), 
              m_UserInput.bMoveFront, m_UserInput.bMoveBack, m_UserInput.bMoveLeft, m_UserInput.bMoveRight );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
*/
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
	DDX_Control(pDX, IDC_DENSITY_SLIDER, m_DensitySlider);
	DDX_Control(pDX, IDC_LOW_RANGE_SLIDER, m_LowRangeSlider);
	DDX_Control(pDX, IDC_HIGH_RANGE_SLIDER, m_HighRangeSlider);
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
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();
	m_pEffect->OnLostDevice();

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
    // TODO: Cleanup any objects created in InitDeviceObjects()
    m_pFont->DeleteDeviceObjects();
	SAFE_RELEASE(m_pScene);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pEffect);

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
    // TODO: Perform any final cleanup needed
    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    // Write the settings to the registry
    WriteSettings();

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
    else if( msg == WM_KEYUP && wParam == VK_ESCAPE )
    {
        // User wants to leave fullscreen mode
        g_AppFormView->OnToggleFullScreen();
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





void CAppForm::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call __super::OnPaint() for painting messages

	
	CWnd * pPicture = GetDlgItem(IDC_FOG_COLOR_PICTURE);
	
	RECT rc;
	pPicture->GetClientRect(&rc);
	CDC * pDC = pPicture->GetDC();

	CBrush Brush;
	Brush.CreateSolidBrush( RGB(m_FogColor.r * 255, m_FogColor.g * 255, m_FogColor.b * 255) );

	pDC->FillRect(&rc, &Brush);


}

void CAppForm::OnBnClickedFogColorPickButton()
{
	// TODO: Add your control notification handler code here
	CHOOSECOLOR cc;
	ZeroMemory(&cc, sizeof(cc));

	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWndOwner;
	cc.hInstance = NULL;
	cc.Flags = CC_RGBINIT + CC_FULLOPEN;
	cc.rgbResult = RGB(m_FogColor.r * 255, m_FogColor.g * 255, m_FogColor.b * 255);
	cc.lpCustColors = m_CustomColor;

	ChooseColor(&cc);

	m_FogColor.r = float( GetRValue(cc.rgbResult) ) / 255.0f;
	m_FogColor.g = float( GetGValue(cc.rgbResult) ) / 255.0f;
	m_FogColor.b = float( GetBValue(cc.rgbResult) ) / 255.0f;


}

//void CAppForm::OnShowWindow(BOOL bShow, UINT nStatus)
//{
//	__super::OnShowWindow(bShow, nStatus);
//
//	// TODO: Add your message handler code here
//
//}

//void CAppForm::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	// __super::OnKeyDown(nChar, nRepCnt, nFlags);
//}

//void CAppForm::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	//__super::OnKeyDown(nChar, nRepCnt, nFlags);
//}

//void CAppForm::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	//__super::OnKeyUp(nChar, nRepCnt, nFlags);
//}

//void CAppForm::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	//__super::OnSysChar(nChar, nRepCnt, nFlags);
//}

//void CAppForm::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	//__super::OnSysKeyDown(nChar, nRepCnt, nFlags);
//}

//void CAppForm::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	//__super::OnSysKeyUp(nChar, nRepCnt, nFlags);
//}

//void CAppForm::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	//__super::OnChar(nChar, nRepCnt, nFlags);
//}

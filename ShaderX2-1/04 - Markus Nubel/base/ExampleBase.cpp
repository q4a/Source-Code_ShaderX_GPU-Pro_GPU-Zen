//-----------------------------------------------------------------------------
// File: ExampleBase.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#include "ExampleBase.h"



//-----------------------------------------------------------------------------
// Name: CBaseD3DApplication()
// Desc: Application constructor.   Paired with ~CBaseD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CBaseD3DApplication::CBaseD3DApplication()
{
    m_dwCreationWidth           = 640;
    m_dwCreationHeight          = 480;
    m_strWindowTitle            = TEXT("ExampleBase");
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
	m_pVBTerrain				= NULL;

    ZeroMemory(&m_UserInput, sizeof(m_UserInput) );
	ZeroMemory(&m_szKeys, sizeof(m_szKeys));
    m_vVelocity			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vAngularVelocity	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vecScale			= D3DXVECTOR3(3.0f, 4.0f, 3.0f);


	m_nPolyCount				= 0;

	m_fAutoTime					= 0.0f;
	m_fAutoCamSpeed				= 0.1f;
	m_bAutoCam					= TRUE;
	m_bScrPos					= FALSE;
	m_bCheckPSsupport			= TRUE;

	// Init shaders
	m_pVertexShader = NULL;
	m_pPixelShader	= NULL;

	// Fog settings
	m_colFog					= D3DCOLOR_XRGB(100,100,100);
}




//-----------------------------------------------------------------------------
// Name: ~CBaseD3DApplication()
// Desc: Application destructor.  Paired with CBaseD3DApplication()
//-----------------------------------------------------------------------------
CBaseD3DApplication::~CBaseD3DApplication()
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
HRESULT CBaseD3DApplication::OneTimeSceneInit()
{
	FILE*	f;
    // TODO: perform one time initialization

	// Load the heightmap
	m_pHeightMap	= new BYTE[1025*1025];
	f	= fopen(_T("..\\media\\height1025.raw"), _T("rb"));
	if(NULL == f)
		return S_FALSE;
	fread(m_pHeightMap, 1025*1025, 1, f);
	fclose(f);

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
HRESULT CBaseD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
	if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
	{
		if(!( pCaps->VertexShaderVersion >= D3DVS_VERSION(1,1) ))
			return E_FAIL;
	}

	if(m_bCheckPSsupport)
	{
		if(!( pCaps->PixelShaderVersion >= D3DPS_VERSION(1,1) ))
			return E_FAIL;
	}

    // Need to support post-pixel processing (for fog)
    if( FAILED( m_pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
        Format, D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
        D3DRTYPE_SURFACE, Format ) ) )
    {
        return E_FAIL;
    }

	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		return E_FAIL;
	}

    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}


BYTE	CBaseD3DApplication::getHeight(int x, int y)
{
	// First ensure, that the heightmap limits are not exceeded.
	x = x % C_HEIGHTMAP_SIZE;
	y = y % C_HEIGHTMAP_SIZE;

	// Access the heightmap
	return (BYTE)(((int)m_pHeightMap[x+y*C_HEIGHTMAP_SIZE])/2);
}

HRESULT CBaseD3DApplication::createTerrainPatch(LPDIRECT3DVERTEXBUFFER9 vb, int xOffset, int yOffset)
{
	HRESULT hr;

    // Fill the vertex buffer from the heightmap
    TERRAINVERTEX* pTerrain;

    if( FAILED( hr = vb->Lock( 0, 0, (VOID**)&pTerrain, 0 ) ) )
        return DXTRACE_ERR( "Lock Terrain VB", hr );

	int 	nIdx		= 0;
	float	fIncrement	= 1.0f/C_HEIGHTMAP_SIZE;
	for (int x = 0; x < C_HEIGHTMAP_SIZE; x += C_STEP_SIZE)
	{
		for (int y = 0;  y < C_HEIGHTMAP_SIZE; y += C_STEP_SIZE)
		{
			// Color each quad based on the height of the landscape.
			
			int nBaseColor	= 60;
			// Vertex 0
			int nColor = nBaseColor + getHeight(x, y);
			pTerrain[nIdx].color	= D3DCOLOR_ARGB(255, nColor, nColor, nColor);
			pTerrain[nIdx].pos		= D3DXVECTOR3((float)(x+xOffset), (float)getHeight(x,y),(float)(y+yOffset));
			pTerrain[nIdx].u		= x * fIncrement;
			pTerrain[nIdx].v		= y * fIncrement;
			// Vertex 1
			nColor = nBaseColor + getHeight(x+C_STEP_SIZE, y+C_STEP_SIZE );
			pTerrain[nIdx+1].color	= D3DCOLOR_ARGB(255, nColor, nColor, nColor);
			pTerrain[nIdx+1].pos	= D3DXVECTOR3((float)((x+xOffset)+C_STEP_SIZE), (float)getHeight(x+C_STEP_SIZE,y+C_STEP_SIZE),float((y+yOffset)+C_STEP_SIZE));
			pTerrain[nIdx+1].u		= (x+C_STEP_SIZE) * fIncrement;
			pTerrain[nIdx+1].v		= (y+C_STEP_SIZE) * fIncrement;
			//Vertex 2
			nColor = nBaseColor + getHeight(x+C_STEP_SIZE, y );
			pTerrain[nIdx+2].color	= D3DCOLOR_ARGB(255, nColor, nColor, nColor);
			pTerrain[nIdx+2].pos	= D3DXVECTOR3((float)((x+xOffset)+C_STEP_SIZE), (float)getHeight(x+C_STEP_SIZE,y),(float)(y+yOffset));
			pTerrain[nIdx+2].u		= (x+C_STEP_SIZE) * fIncrement;
			pTerrain[nIdx+2].v		= (y) * fIncrement;
			// Vertex 3
			nColor = nBaseColor + getHeight(x, y );
			pTerrain[nIdx+3].color	= D3DCOLOR_ARGB(255, nColor, nColor, nColor);
			pTerrain[nIdx+3].pos	= D3DXVECTOR3((float)(x+xOffset), (float)getHeight(x,y),(float)(y+yOffset));
			pTerrain[nIdx+3].u		= x * fIncrement;
			pTerrain[nIdx+3].v		= y * fIncrement;
			// Veretex 4
			nColor = nBaseColor + getHeight(x, y+C_STEP_SIZE );
			pTerrain[nIdx+4].color	= D3DCOLOR_ARGB(255, nColor, nColor, nColor);
			pTerrain[nIdx+4].pos	= D3DXVECTOR3((float)(x+xOffset), (float)getHeight(x,y+C_STEP_SIZE),(float)((y+yOffset)+C_STEP_SIZE));
			pTerrain[nIdx+4].u		= (x) * fIncrement;
			pTerrain[nIdx+4].v		= (y+C_STEP_SIZE) * fIncrement;
			// Vertex 5
			nColor = nBaseColor + getHeight(x+C_STEP_SIZE, y+C_STEP_SIZE );
			pTerrain[nIdx+5].color	= D3DCOLOR_ARGB(255, nColor, nColor, nColor);
			pTerrain[nIdx+5].pos	= D3DXVECTOR3((float)((x+xOffset)+C_STEP_SIZE), (float)getHeight(x+C_STEP_SIZE,y+C_STEP_SIZE),(float)((y+yOffset)+C_STEP_SIZE));
			pTerrain[nIdx+5].u		= (x+C_STEP_SIZE) * fIncrement;
			pTerrain[nIdx+5].v		= (y+C_STEP_SIZE) * fIncrement;
			
			// Advance index
			nIdx	+= 6;
		}
	}
    vb->Unlock();

	return S_OK;
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
HRESULT CBaseD3DApplication::InitDeviceObjects()
{
	HRESULT	hr;
    // TODO: create device objects



    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    // Create the vertex buffer for the terrain
	int nPolyCountOneDimension	= (C_HEIGHTMAP_SIZE/C_STEP_SIZE)+1;
	m_nPolyCount				= 2*nPolyCountOneDimension*nPolyCountOneDimension;

	// Patch1
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 3*m_nPolyCount*sizeof(TERRAINVERTEX),
                                                  0, D3DFVF_TERRAINVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVBTerrain, NULL ) ) )
        return DXTRACE_ERR( "CreateVertexBuffer Terrain", hr );

	createTerrainPatch(m_pVBTerrain, 0, 0);


	// Load the terrain texture

	hr = D3DXCreateTextureFromFile(m_pd3dDevice,"..\\media\\terrain.jpg",&m_texTerrain);
	if(FAILED(hr))
		return S_FALSE;


	// Vertex declaration
	D3DVERTEXELEMENT9 decl[] = 
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,    0 },
		D3DDECL_END()
	};

	m_pd3dDevice->CreateVertexDeclaration(decl, &m_pVertexDeclaration );

	// Call methods in derived class

	if(FAILED(CreateShaders()))
		return S_FALSE;

	return S_OK;
}

HRESULT CBaseD3DApplication::CreateShaders()
{
	return S_OK;
}


HRESULT CBaseD3DApplication::InitializeFog()
{
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
HRESULT CBaseD3DApplication::RestoreDeviceObjects()
{
    // TODO: setup render states

    // Set up the textures
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP ,D3DTOP_MODULATE2X );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.

    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 600.0f, -100.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 700.0f, 0.0f, 1000.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH(&m_matView, &vFromPt, &vLookatPt, &vUpVec);
    m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixTranslation(&m_matPosition, vFromPt.x, vFromPt.y, vFromPt.z);

    // Set the projection matrix
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI/3, fAspect, 1.0f, 10000.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

	// Init World
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixScaling(&m_matWorld, m_vecScale.x, m_vecScale.y, m_vecScale.z);

	// Setup fog 
	InitializeFog();

    // Restore the font
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}

// Called in AutoCamMode, to calculate the next view matrix
// for the next camera postion
HRESULT CBaseD3DApplication::CalcAutoCam()
{
	float		fAutoCamPos;
	float		fRadius;
	D3DXVECTOR3 vCamera;
	D3DXVECTOR3	vCenter;

	fRadius	= 600.0f;
	vCenter	= D3DXVECTOR3(512, 100, 512);
	vCenter.x	*= m_vecScale.x;
	vCenter.y	*= m_vecScale.y;
	vCenter.z	*= m_vecScale.z;


	if(m_fAutoTime > C_AUTO_CAM_TIME)
		m_fAutoTime = 0.0f;
	else
		m_fAutoTime += m_fElapsedTime*m_fAutoCamSpeed;

	float	fsin	= (float)sin(m_fAutoTime);
	float	fcos	= (float)cos(m_fAutoTime);
	// Current time point in autocam loop
	fAutoCamPos = (m_fAutoTime/C_AUTO_CAM_TIME)*2*D3DX_PI; // Results in a range between 0 and PI

	// Depending on the current rotation direction the next point is calculated
	vCamera = D3DXVECTOR3(	vCenter.x + fRadius*fcos , 
						/*vCenter.y + sin(fAutoCamPos)*fRadius*0.5f+*/600.0f, 
						vCenter.z + fRadius*fsin );
 
	// Setup of the view matrix
	D3DXVECTOR3 vAt		= D3DXVECTOR3(vCenter.x + fRadius/4.0f*fcos, vCenter.y + fRadius/8.0f*fsin, vCenter.z + fRadius/4.0f*fcos);
    D3DXVECTOR3 vRight  = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH(&m_matView, &vCamera, &vAt, &vUp );
	D3DXMatrixTranslation(&m_matPosition, vCamera.x, vCamera.y, vCamera.z);

	D3DXMATRIX     matR;
	D3DXQUATERNION qR;
	D3DXQuaternionRotationYawPitchRoll( &qR, -1.5f, .5f, 0.0f);
	D3DXMatrixRotationQuaternion( &matR, &qR );
	D3DXMatrixMultiply( &m_matPosition, &matR, &m_matPosition );

	return S_OK;
}

// Called in AutoCamMode, to calculate the next view matrix
// for the next camera postion
HRESULT CBaseD3DApplication::SetScrShotPos()
{
	float		fRadius;
	D3DXVECTOR3 vCamera;
	D3DXVECTOR3	vCenter;

	fRadius	= 600.0f;

	float	fTime	= 2.3f;
	float	fsin	= (float)sin(fTime);
	float	fcos	= (float)cos(fTime);

	// Camera position
 	vCamera	= D3DXVECTOR3(1761, 736, 732);

	// Setup of the view matrix
	D3DXVECTOR3 vAt = vCamera + D3DXVECTOR3(fRadius/4.0f*fcos, -vCamera.y/8.0f, fRadius/4.0f*fsin);
    D3DXVECTOR3 vRight  = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH(&m_matView, &vCamera, &vAt, &vUp );
	D3DXMatrixTranslation(&m_matPosition, vCamera.x, vCamera.y, vCamera.z);

	return S_OK;
}

// Processes keyboard input and updates the view matrix and the position 
HRESULT	CBaseD3DApplication::CalcMovement()
{
    FLOAT fSecsPerFrame = m_fElapsedTime;

	// Init rotation and translation
	D3DXVECTOR3 vT( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vR( 0.0f, 0.0f, 0.0f );

	// Check user inputs
	if(m_UserInput.bForward)				vT.z += 2.0f; // Move Forward
	if(m_UserInput.bBackward)				vT.z -= 2.0f; // Move Backward
	if(m_UserInput.bRotateRight)			vR.y += 1.0f; // Turn Right
	if(m_UserInput.bRotateLeft)				vR.y -= 1.0f; // Turn Left
	if(m_UserInput.bRotateDown)				vR.x += 1.0f; // Pitch Down
	if(m_UserInput.bRotateUp)               vR.x -= 1.0f; // Pitch Up

	// Scale the input, to get more responsiveness
	vT *= 200.0f;
	vR *= 1.2f;

	// Apply some friciton (dumping) to the current velocity
	// and add the new impulse
	m_vVelocity        = m_vVelocity * 0.9f			+ vT * 0.1f;
	m_vAngularVelocity = m_vAngularVelocity * 0.9f	+ vR * 0.1f;

	// Frame movement is calculated base on the frame duration
	// This is not totally accurate, since the calculation assume 
	// the velocity of the whole frame to be deterimned by the speed
	// at the end of the frame.
	D3DXVECTOR3  vTFrame = m_vVelocity			* fSecsPerFrame * 3.0f;
	D3DXVECTOR3  vRFrame = m_vAngularVelocity	* fSecsPerFrame * 3.0f;

	// Calculate the new position, based on this frames translation
	D3DXMATRIX     matT;
	D3DXMatrixTranslation( &matT, vTFrame.x, vTFrame.y, vTFrame.z);
	D3DXMatrixMultiply( &m_matPosition, &matT, &m_matPosition );

	// Extract view and right axix from view matrix
	D3DXVECTOR3	vecZAxis	= D3DXVECTOR3(m_matView._13, m_matView._23, m_matView._33);
	D3DXVECTOR3	vecXAxis	= D3DXVECTOR3(m_matView._11, m_matView._21, m_matView._31);
	D3DXVECTOR3 vecYAxis	= D3DXVECTOR3(0,1,0);

	// Rotation, is calculated by using quaternions
	D3DXMATRIX     matR;
	D3DXQUATERNION qR;
	D3DXQuaternionRotationYawPitchRoll( &qR, vRFrame.y, vRFrame.x, vRFrame.z );
	D3DXMatrixRotationQuaternion( &matR, &qR );

	D3DXMATRIX     matR1	= m_matPosition;

	D3DXMatrixMultiply( &m_matPosition, &matR, &m_matPosition );

	// The resulting view matrix is just the inverse of the camera transformation
	D3DXMatrixInverse( &m_matView, NULL, &m_matPosition );


	return S_OK;
}

HRESULT CBaseD3DApplication::SetShaderConstants()
{
	return S_OK;

}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::FrameMove()
{
    // TODO: update world

    // Update user input state
    UpdateInput( &m_UserInput );

	// Check for screenshot position
	if(m_UserInput.bToggleScrPos)
		m_bScrPos	= !m_bScrPos;

	// Check for auto cam movement
	if(m_UserInput.bToggleCam)
		m_bAutoCam = !m_bAutoCam;

	if(m_bScrPos)
		SetScrShotPos();
	else
	{
		if(m_bAutoCam)
			CalcAutoCam();
		else
			CalcMovement();
	}
	
	// Update the view matrix
    m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

	// Recalcualte some matrices
    m_matWorldViewProj	= m_matWorld * m_matView * m_matProj;
    m_matWorldView		= m_matWorld * m_matView;
	D3DXMatrixTranspose(&m_matWorldView, &m_matWorldView);
	D3DXMatrixTranspose(&m_matWorldViewProj, &m_matWorldViewProj);

	SetShaderConstants();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
#define KeyPressed(x) (GetAsyncKeyState((x)) & 0x8000)
#define KeyReleased(x) \
(m_szKeys[x] && !(GetAsyncKeyState((x)) & 0x8000))? TRUE : FALSE; m_szKeys[x] = (GetAsyncKeyState((x)) & 0x8000);
void CBaseD3DApplication::UpdateInput( UserInput* pUserInput )
{
	// If the application is not active, do not query inputs
	if(!m_bActive)
	{
		memset(pUserInput, 0, sizeof(UserInput));
		return;
	}

    pUserInput->bToggleScrPos	= KeyReleased('P');
    pUserInput->bToggleCam		= KeyReleased('T');
    pUserInput->bForward		= KeyPressed('W');
    pUserInput->bBackward		= KeyPressed('S');
    pUserInput->bRotateLeft		= KeyPressed('A') || KeyPressed(VK_LEFT) || KeyPressed(VK_NUMPAD1);
    pUserInput->bRotateRight	= KeyPressed('D') || KeyPressed(VK_RIGHT) || KeyPressed(VK_NUMPAD3);
    pUserInput->bRotateUp		= KeyPressed(VK_UP);
    pUserInput->bRotateDown		= KeyPressed(VK_DOWN);
}



HRESULT CBaseD3DApplication::PreRender()
{
    return S_OK;
}

HRESULT CBaseD3DApplication::PostRender()
{
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::Render()
{
	// Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         m_colFog, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // TODO: render world

		PreRender();

		if(NULL != m_pVertexShader)
			m_pd3dDevice->SetVertexShader(m_pVertexShader);
		if(NULL != m_pPixelShader)
			m_pd3dDevice->SetPixelShader(m_pPixelShader);

		// Terrain rendering
		m_pd3dDevice->SetTexture(0, m_texTerrain);
		m_pd3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
        m_pd3dDevice->SetStreamSource(0, m_pVBTerrain, 0, sizeof(TERRAINVERTEX));
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nPolyCount);
		m_pd3dDevice->SetTexture(0, NULL);

		PostRender();

        // Render stats and help text  
//        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(128,128,128,0);
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

    wsprintf( szMsg, TEXT("Arrow keys: Up=%d Down=%d Left=%d Right=%d"), 
              m_UserInput.bRotateUp, m_UserInput.bRotateDown, m_UserInput.bRotateLeft, m_UserInput.bRotateRight );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
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
LRESULT CBaseD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
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
HRESULT CBaseD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::DeleteDeviceObjects()
{
    // TODO: Cleanup any objects created in InitDeviceObjects()
	// Release the terrain texture
	SAFE_RELEASE(m_texTerrain);
	SAFE_RELEASE(m_pVertexDeclaration);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE( m_pVBTerrain );

    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CBaseD3DApplication::FinalCleanup()
{
    // TODO: Perform any final cleanup needed

	// Cleanup terrain
	if(NULL != m_pHeightMap)
		delete [] m_pHeightMap;

    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    return S_OK;
}





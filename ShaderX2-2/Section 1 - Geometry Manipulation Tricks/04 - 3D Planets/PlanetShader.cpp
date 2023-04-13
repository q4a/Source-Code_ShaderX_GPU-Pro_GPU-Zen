//-----------------------------------------------------------------------------
// File: PlanetShader.cpp
//
// Desc: DirectX MFC dialog application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#include "stdafx.h"
#include "resource.h"
#include "PlanetShader.h"




//-----------------------------------------------------------------------------
// Application globals
//-----------------------------------------------------------------------------
TCHAR*          g_strAppTitle       = _T( "PlanetShader" );
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
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnReleasedcaptureSlider1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
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
	m_time = 0;
	m_LightX = 0.0f;
	m_LightY = 0.0f;
	m_LightZ = 0.0f;
	//}}AFX_DATA_INIT

    g_AppFormView          = this;
    m_hwndRenderWindow     = NULL;
    m_hwndRenderFullScreen = NULL;
    m_hWndTopLevelParent   = NULL;

    // Override some CD3DApplication defaults:
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "PlanetShader" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    m_pD3DXFont                 = NULL;
    m_bLoadingApp               = TRUE;
    Planet                      = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;
	NumberOfPoints=90;
	m_LightX=5;
	m_LightY=15;
	m_LightZ=15;
	time=0;
	dtime=.1;
	


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
	Context=cgCreateContext();
    m_bLoadingApp = FALSE;

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



void CAppForm::SetupTable()
{
	int i,j,k;
	float v[3],s;
	for (i = 0 ; i < B ; i++) {
	
		do{
		for (int j = 0 ; j < 3 ; j++)
			v[j] = (float)((rand() % (B + B)) - B) / B;
		s=DOT(v,v);
		}while(s>1);
		s=sqrt(s);
		for(j=0;j<3;j++)
			pg[i][j]=v[j]/s;	
	}

	for(i=0;i<B;i++)
		pg[i][3]=i;
	for(i=B;i>0;i-=2){
	k=pg[i][3];
	pg[i][3]=pg[j=rand()%B][3];
	pg[j][3]=k;
	}
	for(i=0;i<B+2;i++){
		pg[B+i][3]=pg[i][3];
		for(j=0;j<3;j++)
			pg[B+i][j]=pg[i][j];
	}
}
#include<assert.h>
HRESULT CAppForm::InitDeviceObjects()
{
    
    // Create the vertex buffer
	cgD3D9SetDevice(m_pd3dDevice);
	PlanetShader=cgCreateProgramFromFile(Context,CG_SOURCE,"PlanetShader.cg",CG_PROFILE_VS_2_X,"main",0);
	SeaShader=cgCreateProgramFromFile(Context,CG_SOURCE,"SeaShader.cg",CG_PROFILE_VS_2_X,"main",0);
	CloudShader=cgCreateProgramFromFile(Context,CG_SOURCE,"CloudShader.cg",CG_PROFILE_VS_2_X,"main",0);
	   const D3DVERTEXELEMENT9 decl[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END(),
    };

	assert(cgD3D9ValidateVertexDeclaration(PlanetShader, decl));
	m_pd3dDevice->CreateVertexDeclaration(decl, &m_pDeclaration);

	SeaPixelShader=cgCreateProgramFromFile(Context,CG_SOURCE,"SeaPixelShader.cg",CG_PROFILE_PS_2_X,"main",0);
	CloudPixelShader=cgCreateProgramFromFile(Context,CG_SOURCE,"CloudPixelShader.cg",CG_PROFILE_PS_2_X,"main",0);
	PlanetPixelShader=cgCreateProgramFromFile(Context,CG_SOURCE,"PlanetPixelShader.cg",CG_PROFILE_PS_2_X,"main",0);
	Text=cgCreateProgramFromFile(Context,CG_SOURCE,"Text.cg",CG_PROFILE_PS_2_X,"main",0);
	cgD3D9LoadProgram(PlanetShader, true, 0);
	cgD3D9LoadProgram(PlanetPixelShader, true, 0);
	cgD3D9LoadProgram(SeaShader, true, 0);
	cgD3D9LoadProgram(SeaPixelShader, true, 0);
	cgD3D9LoadProgram(CloudShader, true, 0);
	cgD3D9LoadProgram(CloudPixelShader, true, 0);
	cgD3D9LoadProgram(Text, true, 0);
	WorldViewProj= cgGetNamedParameter(PlanetShader, "ModelViewProj");
	Light=cgGetNamedParameter(PlanetShader, "light");
	SLight=cgGetNamedParameter(SeaShader, "light");
	CLight=cgGetNamedParameter(CloudShader, "light");
	Layer=cgGetNamedParameter(CloudShader, "layer");
	WorldViewProj2= cgGetNamedParameter(SeaShader, "ModelViewProj");
	WorldViewProj3= cgGetNamedParameter(CloudShader, "ModelViewProj");
	NumberOfP= cgGetNamedParameter(PlanetShader, "NumberOfPoints");
	NumberOfPCloud= cgGetNamedParameter(CloudShader, "NumberOfPoints");
 	NumberOfPSea= cgGetNamedParameter(SeaShader, "NumberOfPoints");
	RandParam= cgGetNamedParameter(PlanetShader,"RandomArray");
	Texture = cgGetNamedParameter(PlanetPixelShader, "BaseTexture");
	Texture2 = cgGetNamedParameter(CloudPixelShader, "BaseTexture");
	Texture3 = cgGetNamedParameter(PlanetPixelShader, "BaseTexture2");
	CloudTime = cgGetNamedParameter(CloudPixelShader, "time");


	SetupTable();
	cgD3D9SetUniformArray(RandParam,0,B+B+2,&pg);
    return S_OK;
}

float number=0;
VOID WINAPI LandTexture (D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
const D3DXVECTOR2* pTexelSize, LPVOID pData)
{
  
	if((pTexCoord->x*16)<4)
	{
		pOut[0][0]=.5;
		pOut[0][1]=.42;
		pOut[0][2]=.28;
	}
	if((pTexCoord->x*16)<8&&(pTexCoord->x*16)>4)
	{
		pOut[0][0]=.15;
		pOut[0][1]=.40;
		pOut[0][2]=0;

	}
	if((pTexCoord->x*16)<9&&(pTexCoord->x*16)>8)
	{
	
		pOut[0][0]=.5;
		pOut[0][1]=.39;
		pOut[0][2]=.2;
	}
	if((pTexCoord->x*16)<16&&(pTexCoord->x*16)>9)
	{
		pOut[0][0]=1;
		pOut[0][1]=1;
		pOut[0][2]=1;
	
	}
	number++;
}

VOID WINAPI RandTexture (D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
const D3DXVECTOR2* pTexelSize, LPVOID pData)
{
  
		for(int j=0;j<4;j++)
		pOut[0][j]=(float) (rand() / (float) RAND_MAX);
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
 
    HRESULT hr;

    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer(2*(NumberOfPoints+1)*((NumberOfPoints/2)+1)*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &Planet, NULL ) ) )
        return DXTRACE_ERR( "CreateVertexBuffer", hr );

	
	    CUSTOMVERTEX* pVertices;

    if( FAILED( hr = Planet->Lock( 0, 0, (VOID**)&pVertices, 0 ) ) )
        return DXTRACE_ERR( "Lock", hr );

	int i=0;
	float nStep = 6.283185307179586476925286766559/NumberOfPoints;
	float fx=0;
	float fy=0;
	for(int y=0;y<=NumberOfPoints/2;y++)
	{
		for(int x=0;x<=NumberOfPoints;x++)
		{
			pVertices[i].x=fx;
			pVertices[i].y=fy;
			pVertices[i].u=x;
			pVertices[i].v=y;
			i++;
			fy-=nStep;
		
			pVertices[i].x=fx;
			pVertices[i].y=fy;
			pVertices[i].u=x;
			pVertices[i].v=y+1;
			fx-=nStep;
			fy+=nStep;
			i++;
		}
		fx=0;
		fy-=nStep;
	}
	Planet->Unlock();

  	D3DXCreateTexture(m_pd3dDevice,16,1,1,0,D3DFMT_A16B16G16R16F,D3DPOOL_DEFAULT ,&pTexture);
	D3DXFillTexture (pTexture, LandTexture, NULL);

	D3DXCreateTexture(m_pd3dDevice,512,1,1,0,D3DFMT_A16B16G16R16F,D3DPOOL_DEFAULT ,&pRandTexture);
	D3DXFillTexture (pRandTexture, RandTexture, NULL);

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
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	m_World=matIdentity;
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
	m_View=matView;
    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	m_Projection=matProj;
    // Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Create a D3D font using D3DX
    HFONT hFont = CreateFont( 20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                              ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, FF_DONTCARE, "Arial" );      
    if( FAILED( hr = D3DXCreateFont( m_pd3dDevice, hFont, &m_pD3DXFont ) ) )
        return DXTRACE_ERR( "D3DXCreateFont", hr );

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

    D3DXMatrixMultiply( &matWorld, &matRotX, &matRotY );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_World=matWorld;
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
	time+=(dtime+(m_time/50));
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x00000000, 1.0f, 0L );
	D3DXMATRIX mv;
	mv = m_World*m_View * m_Projection;
  	cgD3D9SetUniformMatrix(WorldViewProj, &mv);
  	cgD3D9SetUniformMatrix(WorldViewProj2, &mv);
	cgD3D9SetUniformMatrix(WorldViewProj3, &mv);
	cgD3D9SetUniform(NumberOfP,&NumberOfPoints);
	cgD3D9SetUniform(NumberOfPCloud,&NumberOfPoints);
	cgD3D9SetUniform(NumberOfPSea,&NumberOfPoints);
	LightPos[0]=m_LightX;
	LightPos[1]=m_LightY;
	LightPos[2]=m_LightZ;
	cgD3D9SetUniform(Light,&LightPos);
	cgD3D9SetUniform(SLight,&LightPos);
	cgD3D9SetUniform(CLight,&LightPos);
	cgD3D9SetUniform(CloudTime,&time);

	float layerinfo=.1;
	cgD3D9SetUniform(Layer,&layerinfo);
	cgD3D9SetTexture(Texture, pTexture);
    cgD3D9SetTexture(Texture2, pRandTexture);
	cgD3D9SetTexture(Texture3, pRandTexture);
	// Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		m_pd3dDevice->SetStreamSource( 0, Planet, 0, sizeof(CUSTOMVERTEX) );
		m_pd3dDevice->SetVertexDeclaration(m_pDeclaration);
		cgD3D9BindProgram(PlanetShader);
		cgD3D9BindProgram(PlanetPixelShader);
   
		m_pd3dDevice->SetTexture( 0, pTexture );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*(NumberOfPoints+1)*((NumberOfPoints/2)+1) );
		        
       	cgD3D9BindProgram(SeaShader);
	 	cgD3D9BindProgram(SeaPixelShader);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*(NumberOfPoints+1)*((NumberOfPoints/2)+1) );
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

		m_pd3dDevice->SetTexture( 0, pRandTexture );
		cgD3D9BindProgram(CloudShader);
	 	cgD3D9BindProgram(CloudPixelShader);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*(NumberOfPoints+1)*((NumberOfPoints/2)+1) );
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        // Render stats and help text  
		cgD3D9BindProgram(Text);
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
    RECT rct;
    ZeroMemory( &rct, sizeof(rct) );       

    m_pD3DXFont->Begin();
    rct.left   = 2;
    rct.right  = m_d3dsdBackBuffer.Width - 20;

    // Output display stats
    INT nNextLine = 40; 

    lstrcpy( szMsg, m_strDeviceStats );
    nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
    m_pD3DXFont->DrawText( szMsg, -1, &rct, 0, fontColor );

    lstrcpy( szMsg, m_strFrameStats );
    nNextLine -= 20; rct.top = nNextLine; rct.bottom = rct.top + 20;    
    m_pD3DXFont->DrawText( szMsg, -1, &rct, 0, fontColor );


    m_pD3DXFont->End();

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
	DDX_Slider(pDX, IDC_SLIDER1, m_time);
	DDX_Text(pDX, IDC_EDIT1, m_LightX);
	DDX_Text(pDX, IDC_EDIT2, m_LightY);
	DDX_Text(pDX, IDC_EDIT3, m_LightZ);
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
   
    SAFE_RELEASE( m_pD3DXFont );
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pRandTexture);
	SAFE_RELEASE(Planet);
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
    
    // Remove the menu bar.  Do it this way because CFrameWnd::LoadFrame() requires 
    // there be a menu called IDR_MAINFRAME.
	SetMenu( NULL );
	
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
   
    SAFE_RELEASE( Planet );
	SAFE_RELEASE(m_pDeclaration);
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pRandTexture)
	SAFE_RELEASE(m_pD3D );
	SAFE_RELEASE(m_pD3DXFont );
	SAFE_RELEASE(m_pd3dDevice);
	cgDestroyContext(Context);
	cgD3D9SetDevice(0);
	Cleanup3DEnvironment();
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

    
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pRandTexture)
	SAFE_RELEASE(Planet);
	SAFE_RELEASE(m_pDeclaration);
    SAFE_RELEASE( m_pD3DXFont );
	SAFE_RELEASE(m_pd3dDevice);
    // Update UI
	UserSelectNewDevice();
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





void CAppForm::OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	UpdateData(TRUE);
	*pResult = 0;
}



void CAppForm::OnButton1() 
{

	UpdateData(TRUE);
}

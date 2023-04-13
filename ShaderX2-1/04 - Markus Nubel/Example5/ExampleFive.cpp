//-----------------------------------------------------------------------------
// File: ExampleFive.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------


#include "ExampleFive.h"

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CBaseD3DApplication*	g_pApp  = NULL;
HINSTANCE				g_hInst = NULL;

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
// Desc: Constructor for derived application class.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_strWindowTitle            = TEXT("ExampleFive: Animated Fog");
	m_bCheckPSsupport			= FALSE;
	m_fAutoCamSpeed				= 0.03f;

}

//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Destructor for derived application class.
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{

}


//-----------------------------------------------------------------------------
// Name: CreateShaders()
// Desc: Loads and compiles the shaders, used by this application.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateShaders()
{
	HRESULT				hr;
	LPD3DXBUFFER        pCode;
	LPD3DXBUFFER        pErrorMsgs;
	TCHAR				szShaderPath[512];

	// Compile the vertex shader
	if(FAILED(DXUtil_FindMediaFileCb(szShaderPath, sizeof(szShaderPath), _T("AnimatedFog.vsh"))))
		return S_FALSE;
	hr = D3DXCompileShaderFromFile(szShaderPath, NULL, NULL, _T("main"), _T("vs_1_1"), 0, &pCode, &pErrorMsgs , NULL);
	if(FAILED(hr))
	{
		char* szMsg	= (char*)pErrorMsgs->GetBufferPointer();
		OutputDebugString(szMsg);
		return S_FALSE;
	}
	else
	{
		hr = m_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),  &m_pVertexShader);
		if(FAILED(hr))
		{
			m_pVertexShader = NULL;
			return S_FALSE;
		}
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(pErrorMsgs);
	}


	
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitializeFog()
// Desc: Initializes the FFP fog
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitializeFog()
{
	// Using FFP fog
	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,		TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR,		m_colFog );
	m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR );
	m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE,	D3DFOG_NONE );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SetShaderConstants()
// Desc: Is called every frame, to provide the shaders with the needed 
//       constants
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::SetShaderConstants()
{
	D3DXVECTOR4 vCamera(m_matPosition._41, m_matPosition._42, m_matPosition._43, m_matPosition._44);

									/* gray */
    D3DXVECTOR4 colFog    = D3DXVECTOR4(110.0f/255.0f, 110.0f/255.0f, 110.0f/255.0f, 0.0f/255.0f);
								   /* ( --  /  end / density/ -- )*/
    D3DXVECTOR4 fFog      = D3DXVECTOR4(0.0f, 1800.0f, 0.33f,  0.0f);

	// Fog animation factor
	static float fTime = 0.0f;
	fTime	+= 0.00035f * ((float)rand()/10.0f)*m_fElapsedTime;
	fFog.x	= fTime;

	// Set the shader constants
	m_pd3dDevice->SetVertexShaderConstantF(0, (float*)&m_matWorldViewProj,	4);
	m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&m_matWorldView,		4);
	m_pd3dDevice->SetVertexShaderConstantF(8, (float*)&m_matWorld,			4);
	m_pd3dDevice->SetVertexShaderConstantF(12, (float*)&fFog,				1);
	m_pd3dDevice->SetVertexShaderConstantF(13, (float*)&vCamera,			1);

	return S_OK;
}







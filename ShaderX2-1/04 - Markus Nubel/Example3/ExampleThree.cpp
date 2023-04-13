//-----------------------------------------------------------------------------
// File: ExampleThree.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------

#include "ExampleThree.h"

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
    m_strWindowTitle            = TEXT("ExampleThree: Exponential Squared Fog");
	m_bCheckPSsupport			= TRUE;

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
	if(FAILED(DXUtil_FindMediaFileCb(szShaderPath, sizeof(szShaderPath), _T("Exp2Fog.vsh"))))
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

	// Compile the pixel shader
	if(FAILED(DXUtil_FindMediaFileCb(szShaderPath, sizeof(szShaderPath), _T("Exp2Fog.psh"))))
		return S_FALSE;

	hr = D3DXCompileShaderFromFile(szShaderPath, NULL, NULL, _T("main"), _T("ps_1_1"), 0, &pCode, &pErrorMsgs , NULL);
	if(FAILED(hr))
	{
		char* szMsg	= (char*)pErrorMsgs->GetBufferPointer();
		OutputDebugString(szMsg);
		return S_FALSE;
	}
	else
	{
		hr = m_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(),  &m_pPixelShader );
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(pErrorMsgs);
		if(FAILED(hr))
		{
			m_pPixelShader = NULL;
			return S_FALSE;
		}
	}


	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitializeFog()
// Desc: Initializes the FFP fog
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitializeFog()
{
	// Not using FFP fog
	m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE,  D3DFOG_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

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

									/* (density/  end / --   / -- )*/
    D3DXVECTOR4 fFog      = D3DXVECTOR4(0.66f, 1800.0f, 0.0f, 0.0f);

									/* gray */
    D3DXVECTOR4 colFog    = D3DXVECTOR4(100.0f/255.0f, 100.0f/255.0f, 100.0f/255.0f, 0.0f/255.0f);

	// Set vertex shader constants
	m_pd3dDevice->SetVertexShaderConstantF(0, (float*)&m_matWorldViewProj,	4);
	m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&m_matWorldView,		4);
	m_pd3dDevice->SetVertexShaderConstantF(8, (float*)&m_matWorld,			4);
	m_pd3dDevice->SetVertexShaderConstantF(12, (float*)&fFog,				1);
	m_pd3dDevice->SetVertexShaderConstantF(13, (float*)&vCamera,			1);

	// Set pixel shader constants
	m_pd3dDevice->SetPixelShaderConstantF(5, (float*)&colFog,				1);

	return S_OK;
}







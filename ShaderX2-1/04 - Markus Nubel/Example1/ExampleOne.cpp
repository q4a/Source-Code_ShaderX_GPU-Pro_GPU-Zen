//-----------------------------------------------------------------------------
// File: ExampleOne.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------

#include "ExampleOne.h"

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
    m_strWindowTitle            = TEXT("ExampleOne: Linear Fog");
	m_bCheckPSsupport			= FALSE;

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
	if(FAILED(DXUtil_FindMediaFileCb(szShaderPath, sizeof(szShaderPath), _T("LinearFog.vsh")))) 
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
		m_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),  &m_pVertexShader );
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
    m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR,       m_colFog);
	m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);  
	m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE,  D3DFOG_NONE);  

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SetShaderConstants()
// Desc: Is called every frame, to provide the shaders with the needed 
//       constants
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::SetShaderConstants()
{

    // Update the position matrix (inverse of the camera transformation)
	D3DXVECTOR4 vCamera(m_matPosition._41, m_matPosition._42, m_matPosition._43, m_matPosition._44);

	// Submit the fog parameters
									/* ( end   / start / --   / -- )*/
    D3DXVECTOR4 fFog      = D3DXVECTOR4(1800.0f, 400.0f, 0.0f, 0.0f);

	// Set the shader constants
	m_pd3dDevice->SetVertexShaderConstantF(0, (float*)&m_matWorldViewProj,		4);
	m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&m_matWorldView,			4);
	m_pd3dDevice->SetVertexShaderConstantF(9, (float*)&vCamera,					1);
	m_pd3dDevice->SetVertexShaderConstantF(10, (float*)&fFog,					1);

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: PreRender()
// Desc: Called before the terrain is rendered
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::PreRender()
{
	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,TRUE); 

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: PostRender()
// Desc: Called after the terrain has been rendered
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::PostRender()
{
	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,FALSE); 

    return S_OK;
}


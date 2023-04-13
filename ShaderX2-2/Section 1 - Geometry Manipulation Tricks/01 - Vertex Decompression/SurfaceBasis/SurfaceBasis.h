//-----------------------------------------------------------------------------
// File: SurfaceBasis.h
//
// Desc: Header file SurfaceBasis sample app
//-----------------------------------------------------------------------------
#pragma once




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\SurfaceBasis")

// Struct to store the current input state
struct UserInput
{
    BYTE diks[256];   // DirectInput keyboard state buffer 

    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;

	BOOL b1,b2,b3,b4,b5,b6,b7,b8,b9;
	BOOL bf5,bf6,bf7,bf8,bf9;
};

enum DISPLAY_METHOD
{
	DM_CONTROL,			// the mesh the old fashioned way
	DM_CONTROL_NPATCH,	// uses D3DX to do the N-Patch on the CPU
	DM_BARYCENTRIC,		// used barycentic coordinate to render a 'normal' mesh
	DM_NPATCH,			// Vertex shader N-Patch
	DM_LINEAR,
};


//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text
    ID3DXMesh*              m_pD3DXMesh;            // D3DX mesh to store teapot
    ID3DXPatchMesh*         m_pD3DXPatchMesh;       // D3DX patch mesh to store teapot
    ID3DXMesh*				m_pD3DXPatchMeshDest;   // D3DX patch mesh to store teapot
    LPDIRECTINPUT8          m_pDI;                  // DirectInput object
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput keyboard device
    UserInput               m_UserInput;            // Struct for storing user input 

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis

	LPD3DXEFFECT			m_controlFX;			// normal transform effect
	LPD3DXEFFECT			m_baryFX;				// first barycentric effect
	LPD3DXEFFECT			m_npatchFX;				// npatch effect
	LPD3DXEFFECT			m_linearFX;				// npatch effect

	unsigned int			m_numVertices;
	unsigned int			m_numIndices;
	float*					m_posVertexData;
	float*					m_normVertexData;
	WORD*					m_indexData;

	LPDIRECT3DVERTEXBUFFER9	m_pVB;					// actual VB to render with
	LPDIRECT3DVERTEXBUFFER9	m_pIB;					// actual VB to render with (if used)

	D3DXMATRIX				m_matProj;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matWorld;
	DISPLAY_METHOD			m_displayMethod;

	HRESULT SetupDisplayMethod();
	void CleanDisplayMethod();
	HRESULT GetXMeshData( LPD3DXMESH in );


	// tesselator stuff
	unsigned int			m_lod;

	// first bary surface basis variables

	LPDIRECT3DVERTEXDECLARATION9	m_pBaryDecl;
	float*							m_baryPos;
	HRESULT							GenerateBaryConstants();

	// npatch surface basis
	LPDIRECT3DVERTEXDECLARATION9	m_pNPatchDecl;
	float*							m_npatchPos;
	float*							m_npatchNorm;
	HRESULT							GenerateNPatchConstants();
	HRESULT							GenerateNPatchBuffers( unsigned int LOD );
	HRESULT							GenerateD3DNPatchMesh( unsigned int LOD );

	// linear surface basis
	LPDIRECT3DVERTEXDECLARATION9	m_pLinearDecl;
	float*							m_linearPos;
	float*							m_linearNorm;
	HRESULT							GenerateLinearConstants();
	HRESULT							GenerateLinearBuffers( unsigned int LOD );

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();
    VOID    ReadSettings();
    VOID    WriteSettings();

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};


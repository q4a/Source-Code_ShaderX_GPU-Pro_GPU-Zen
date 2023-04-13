//-----------------------------------------------------------------------------
// File: Quantisation.h
//
// Desc: Header file Quantisation sample app
//-----------------------------------------------------------------------------
#ifndef AFX_QUANTISATION_H__C429C6DE_1BFE_4EAD_8122_272A65AAA8C3__INCLUDED_
#define AFX_QUANTISATION_H__C429C6DE_1BFE_4EAD_8122_272A65AAA8C3__INCLUDED_


#include "CompressedMesh.h"

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\Quantisation")

// Struct to store the current input state
struct UserInput
{
    BYTE diks[256];   // DirectInput keyboard state buffer 

    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
	BOOL bChange;

	BOOL b1,b2,b3,b4,b5,b6,b7,b8,b9;

	BOOL bf5,bf6,bf7,bf8,bf9;

};

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	enum COMPRESS_STATE
	{
		DISPLAY_QUANT_NORMAL = 0,
		DISPLAY_SO8BIT_POS,
		DISPLAY_CT8BIT_POS,
		DISPLAY_SO16BIT_POS,
		DISPLAY_CT16BIT_POS,
		DISPLAY_SCT16BIT_POS,
		DISPLAY_CT10BIT_POS,
		DISPLAY_CT101012BIT_POS,
	};

	enum MESH_NAME
	{
		DISPLAY_TEAPOT = 0,
		DISPLAY_CUBE,
		DISPLAY_TEAPOT_ROW,
		DISPLAY_CUBE_ROW,

		MAX_MESHES
	};

    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text
    ID3DXMesh*              m_pD3DXMesh[MAX_MESHES]; // D3DX mesh to store teapot
	CompressedMesh*			m_pQNMesh[MAX_MESHES];	// Quantised normal compressed mesh
	CompressedMesh*			m_pSO8Mesh[MAX_MESHES];	// Scale & offset 8 bit compressed mesh
	CompressedMesh*			m_pCT8Mesh[MAX_MESHES];	// compressed transform 8 bit compressed mesh
	CompressedMesh*			m_pSO16Mesh[MAX_MESHES];	// Scale & offset 8 bit compressed mesh
	CompressedMesh*			m_pCT16Mesh[MAX_MESHES];	// compressed transform 16 bit compressed mesh
	CompressedMesh*			m_pSCT16Mesh[MAX_MESHES];	// sliding compressed transform 16 bit compressed mesh
	CompressedMesh*			m_pCT10Mesh[MAX_MESHES];	// compressed transform UDEC3N bit compressed mesh
	CompressedMesh*			m_pCT101012Mesh[MAX_MESHES];	// compressed transform 10,10,12 bit compressed mesh

	IDirect3DVertexShader9*	m_dwControlShader[MAX_MESHES];	// D3DX mesh shader
	IDirect3DVertexShader9*	m_dwQNShader[MAX_MESHES];	// Quantised normal compressed mesh shader
	IDirect3DVertexShader9*	m_dwSO8Shader[MAX_MESHES];	// scale & offset 8bit compressed mesh shader
	IDirect3DVertexShader9*	m_dwCT8Shader[MAX_MESHES];	// compressed transform 8 bit compressed mesh shader
	IDirect3DVertexShader9* m_dwSO16Shader[MAX_MESHES];	// scale & offset 8bit compressed mesh shader
	IDirect3DVertexShader9* m_dwCT16Shader[MAX_MESHES];	// compressed transform 16 bit compressed mesh shader
	IDirect3DVertexShader9* m_dwSCT16Shader[MAX_MESHES]; // sliding compressed transform 16 bit compressed mesh shader
	IDirect3DVertexShader9* m_dwCT10Shader[MAX_MESHES];	// compressed transform DEC3N bit compressed mesh shader
	IDirect3DVertexShader9* m_dwCT101012Shader[MAX_MESHES];	// compressed transform 10,10,12 bit compressed mesh shader

	bool					m_original;				// which mesh is visible?
	D3DXMATRIX				m_matView;				// view matrix
	D3DXMATRIX				m_matProj;				// projection matrix
	D3DXMATRIX				m_matWorld;				// world matrix

    LPDIRECTINPUT8          m_pDI;                  // DirectInput object
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput keyboard device
    UserInput               m_UserInput;            // Struct for storing user input 

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis
	CompressedMesh*			m_pCompressMesh;		// currently visible compressed mesh (for stats)
	COMPRESS_STATE			m_compressState;		// which compression is currently visible
	MESH_NAME				m_meshState;			// which mesh is currently visible

protected:
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();
    HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();

    VOID    ReadSettings();
    VOID    WriteSettings();

	HRESULT GenerateTeapotRow( ID3DXMesh** mesh );
	HRESULT GenerateCubeRow( ID3DXMesh** mesh );
	HRESULT CreateVertexShaders( unsigned int i);

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
};


#endif // !defined(AFX_QUANTISATION_H__C429C6DE_1BFE_4EAD_8122_272A65AAA8C3__INCLUDED_)

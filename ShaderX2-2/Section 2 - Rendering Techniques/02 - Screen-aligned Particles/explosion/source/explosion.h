//-----------------------------------------------------------------------------
// File: Particles.h
//
// Desc: Header file Particles sample app
//-----------------------------------------------------------------------------
#pragma once




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\Particles")

// Struct to store the current input state
struct UserInput
{
    BYTE diks[256];   // DirectInput keyboard state buffer 

    // TODO: change as needed
    BOOL bMoveForward;
    BOOL bMoveBackward;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};

struct ParticleVertex
{
    D3DVECTOR position;			
	float tu, tv;    
	float rightFactor, upFactor;
	D3DVECTOR velocity;
	unsigned int birthColor, deathColor;
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
	CD3DMesh*     m_pSkyBox;				        // Skybox background object
	CD3DMesh*     m_pEmitter;							// Random object just for the scene

	D3DXMATRIX m_MatView;							// view matrix
	D3DXMATRIX m_MatProj;							// projection matrix

    LPDIRECTINPUT8          m_pDI;                  // DirectInput object
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput keyboard device
    UserInput               m_UserInput;            // Struct for storing user input 

    FLOAT                   m_fCenterDist;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis	


	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;	
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;		

	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration; 
	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;
	LPDIRECT3DPIXELSHADER9 m_pPixelShader;

	LPDIRECT3DTEXTURE9      m_pTexture;				// Texture for particles

	//timing
	unsigned int m_CurrentTime, m_PreviousTime;
	float m_DeltaTime;
    

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

	HRESULT createVertexData();

	void setupParticle(ParticleVertex* vertexData, int index, D3DVECTOR center, float width, float height
								 , unsigned int color);

	void createShaders();


    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};


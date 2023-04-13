//-----------------------------------------------------------------------------
// File: DirectX9Application1.h
//
// Desc: Header file DirectX9Application1 sample app
//-----------------------------------------------------------------------------
#pragma once


class Mesh;
class Scene;


//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\DirectX9Application1")

// Struct to store the current input state
struct UserInput
{
    BYTE diks[256];   // DirectInput keyboard state buffer 

	BOOL bIncPow;
	BOOL bDecPow;

	BOOL bIncSpeed;
	BOOL bDecSpeed;

	BOOL bIncSpecType;
	BOOL bIncSpecTypeOld;

	UserInput();
};


class CameraMouse
{
public:
	bool init;

	bool drag;

	int mouseX;
	int mouseY;
	int mouseXO;
	int mouseYO;

	FLOAT xAngle;
	FLOAT yAngle;

	CameraMouse();

	void GetCameraPos(D3DXVECTOR3& pos);

	LRESULT HandleMouseMessages( HWND hWnd, UINT uMsg, WPARAM wParam,
                                 LPARAM lParam );
};


//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    TCHAR		            m_strInitialDir[512];
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text
    LPDIRECTINPUT8          m_pDI;                  // DirectInput object
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput keyboard device
	ID3DXSprite*			m_Sprite;				// For rendering light billboards
	LPDIRECT3DTEXTURE9		m_pLightTexture;		// Light billboard texture
	FLOAT					m_LightTextureWidth;
	FLOAT					m_LightTextureHeight;

    UserInput               m_UserInput;            // Struct for storing user input 
    CD3DArcBall		        m_ArcBall;          // mouse rotation utility
	CameraMouse				m_CameraMouse;

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis
	Scene*					m_Scene;

	Mesh*					m_pObject;

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
	virtual void InitMatrices();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
	virtual HRESULT DeviceLost();
	virtual HRESULT DeviceReset();
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


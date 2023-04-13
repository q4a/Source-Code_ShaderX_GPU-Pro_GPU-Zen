//-----------------------------------------------------------------------------
// File: blas3D.h
//
// Desc: Header file blas3D sample app
//-----------------------------------------------------------------------------
#ifndef AFX_BLAS3D_H__4C44C43F_307E_4045_AEC9_8EE97972CC90__INCLUDED_
#define AFX_BLAS3D_H__4C44C43F_307E_4045_AEC9_8EE97972CC90__INCLUDED_




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\blas3D")

// Custom D3D vertex format used by the vertex buffer
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;       // vertex position
    D3DXVECTOR3 normal;         // vertex normal
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)


// Struct to store the current input state
struct UserInput
{
    // TODO: change as needed
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};

class Matrix;

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    LPDIRECT3DVERTEXBUFFER9 m_pVB;                  // Vextex buffer 
    CD3DFont*               m_pFont;                // Font for drawing text

    UserInput               m_UserInput;            // Struct for storing user input 

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis

	IDirect3DSurface9 * displaySurface;			//the surface that we see in the window
	ID3DXSprite * sprite;						//experimental sprite rendering
	Matrix * matrixList;
	LPSTR args;

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

    void    UpdateInput( UserInput* pUserInput );

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication(LPSTR);
    virtual ~CMyD3DApplication();
};


#endif // !defined(AFX_BLAS3D_H__4C44C43F_307E_4045_AEC9_8EE97972CC90__INCLUDED_)

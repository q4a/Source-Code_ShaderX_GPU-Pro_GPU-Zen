//-----------------------------------------------------------------------------
// File: TurbulentSun.h
//
// Desc: Header file TurbulentSun sample app
//-----------------------------------------------------------------------------
#pragma once
#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file
#endif




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\TurbulentSun")

// Struct to store the current input state
struct UserInput
{
    // TODO: change as needed
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};




//-----------------------------------------------------------------------------
// Name: class CAppForm
// Desc: CFormView-based class which allows the UI to be created with a form
//       (dialog) resource. This class manages all the controls on the form.
//-----------------------------------------------------------------------------
class CAppForm : public CFormView, public CD3DApplication
{
public:
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    ID3DXFont*              m_pD3DXFont;            // D3DX font    
    UserInput               m_UserInput;            // Struct for storing user input 
    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis

private:
    HWND    m_hwndRenderWindow;
    HWND    m_hwndRenderFullScreen;
    HWND    m_hWndTopLevelParent;
	//Dx stuff
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DVERTEXBUFFER9 SunVB;
	LPDIRECT3DVERTEXBUFFER9 SunFlare;
	IDirect3DVertexDeclaration9* m_pDeclaration;
	D3DXMATRIX m_World;
	D3DXMATRIX m_View;
	D3DXMATRIX m_Projection;
	//Time stuff
	float time;
	float dtime;
	float distort[2000];
	float steps;
	float updatetime;
	//Cg Stuff
	CGcontext Context;
	CGprogram SunPixel;
    CGprogram SunVertex;
	CGprogram Flare;
	CGprogram Blank;
    CGprogram Text;
	CGparameter WorldViewProj;
	CGparameter Texture;
	CGparameter Time;
	CGparameter AmountOfVer;
	CGparameter FlareWorldViewProj;
	//
	void GenerateSphere(int numbofpoints);
    virtual HRESULT ConfirmDevice( D3DCAPS9*,DWORD,D3DFORMAT );
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT FinalCleanup();
    virtual HRESULT AdjustWindowForChange();

    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );

    VOID    ReadSettings();
    VOID    WriteSettings();

    VOID    UpdateUIForDeviceCapabilites();

protected:
    DECLARE_DYNCREATE(CAppForm)

             CAppForm();
    virtual  ~CAppForm();

public:
    BOOL IsReady() { return m_bActive; }
    TCHAR* PstrFrameStats() { return m_strFrameStats; }
    VOID RenderScene() { Render3DEnvironment(); }
    HRESULT CheckForLostFullscreen();

    //{{AFX_DATA(CAppForm)
	enum { IDD = IDD_FORMVIEW };
	int		m_TimeChange;
	//}}AFX_DATA

    //{{AFX_VIRTUAL(CAppForm)
    virtual void OnInitialUpdate();
    protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

public:
    //{{AFX_MSG(CAppForm)
    afx_msg void OnToggleFullScreen();
    afx_msg void OnChangeDevice();
	afx_msg void OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CAppDoc
// Desc: Overridden CDocument class needed for the CFormView
//-----------------------------------------------------------------------------
class CAppDoc : public CDocument
{
protected:
    DECLARE_DYNCREATE(CAppDoc)

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAppDoc)
    public:
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CAppDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CAppFrameWnd
// Desc: CFrameWnd-based class needed to override the CFormView's window style
//-----------------------------------------------------------------------------
class CAppFrameWnd : public CFrameWnd
{
protected:
    DECLARE_DYNCREATE(CAppFrameWnd)
public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAppFrameWnd)
    public:
    virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CAppFrameWnd)
    afx_msg void OnChangeDevice();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CApp
// Desc: Main MFC application class derived from CWinApp.
//-----------------------------------------------------------------------------
class CApp : public CWinApp
{
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CApp)
    public:
    virtual BOOL InitInstance();
    virtual BOOL OnIdle( LONG );
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CApp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};





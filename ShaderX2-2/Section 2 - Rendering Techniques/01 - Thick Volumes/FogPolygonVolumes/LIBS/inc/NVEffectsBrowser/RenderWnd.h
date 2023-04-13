#if !defined(AFX_RENDERWND_H__0823D2E9_F78A_11D4_8D9C_000347486A4B__INCLUDED_)
#define AFX_RENDERWND_H__0823D2E9_F78A_11D4_8D9C_000347486A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd window

class CRenderWnd : public CWnd
{
// Construction
public:
	CRenderWnd();
	DECLARE_DYNCREATE(CRenderWnd)

// Attributes
public:
	CNVEffectsDoc* GetDocument();
	CRenderView*   GetRenderView();
    void           SetDefaultError(LPCTSTR);

    bool m_bFDown;
    bool m_bODown;
    bool m_bCtrlDown;


// Operations
public:
	nv_renderdevice::INVRenderDevice* m_pDevice;
	HINSTANCE m_hDeviceLib;
	HINSTANCE m_hDeviceLib9;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderWnd();

// Overrides
public:
	virtual HRESULT Init();
	virtual HRESULT Release();

	virtual HRESULT UpdateDevice();
	virtual HRESULT ConfirmDevice(IEBEffect*);

	virtual HRESULT SetupEffect(IEBEffect*);
	virtual HRESULT SetDefaultRenderstates();

	virtual HRESULT Render(bool bDrawLogo = false);
	virtual HRESULT Refresh(CDC*);
	virtual HRESULT Clear();
    virtual void SaveScreen(CString PathName);

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderWnd)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERWND_H__0823D2E9_F78A_11D4_8D9C_000347486A4B__INCLUDED_)

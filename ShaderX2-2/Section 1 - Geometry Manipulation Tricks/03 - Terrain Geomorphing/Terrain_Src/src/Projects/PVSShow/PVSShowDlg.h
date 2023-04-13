// PVSShowDlg.h : Header-Datei
//

#if !defined(AFX_PVSSHOWDLG_H__908E8112_2548_4DFB_B647_DDF9C49A5806__INCLUDED_)
#define AFX_PVSSHOWDLG_H__908E8112_2548_4DFB_B647_DDF9C49A5806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../pgLib/Render/Terrain/pgTerrainPVS.h"


/////////////////////////////////////////////////////////////////////////////
// CPVSShowDlg Dialogfeld

class CPVSShowDlg : public CDialog
{
// Konstruktion
public:
	CPVSShowDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPVSShowDlg)
	enum { IDD = IDD_PVSSHOW_DIALOG };
	CEdit	m_CheckYD;
	CEdit	m_CheckXD;
	CStatic	m_CheckV;
	CEdit	m_CheckZS;
	CEdit	m_CheckYS;
	CEdit	m_CheckXS;
	CStatic	m_Info2;
	CSliderCtrl	m_HeightSlider;
	CStatic	m_HeightTxt;
	CStatic	m_Info;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPVSShowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	bool Open(const CString& nFileName);
	void SetBitmapData(int nW, int nH, const unsigned char* nData);
	void CalcPVS(int minH, int maxH, int stepsH);
	void UpdateHeight();
	void UpdateInfo2();


	HICON m_hIcon;

	int				bitmapW, bitmapH;
	BITMAPINFO      bitmapInfo;
	RGBQUAD         *bitmapData;

	int				dataW,dataH;
	unsigned char*	data;

	int				patchW, patchH;
	unsigned char	*minData, *maxData;


	bool			pvs;
	int				pvsX,pvsY;
	unsigned char	*pvsData;


	int				pvsZ, curZ;
	bool			pvsZAbs;
	pgTerrainPVS*	terrainPVS;


	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CPVSShowDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpen();
	afx_msg void OnMin();
	afx_msg void OnMax();
	afx_msg void OnPvs();
	afx_msg void OnHeight();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCalcPVS();
	afx_msg void OnLoadPVS();
	afx_msg void OnSavePVS();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHeightTxt();
	afx_msg void OnShrinkPVS();
	afx_msg void OnChangeCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PVSSHOWDLG_H__908E8112_2548_4DFB_B647_DDF9C49A5806__INCLUDED_)

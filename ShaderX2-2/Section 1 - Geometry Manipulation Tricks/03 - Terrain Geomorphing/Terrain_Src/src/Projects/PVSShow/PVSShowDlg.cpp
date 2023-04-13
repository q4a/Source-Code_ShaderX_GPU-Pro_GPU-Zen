// PVSShowDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "PVSShow.h"
#include "PVSShowDlg.h"

#include <math.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPVSShowDlg Dialogfeld

CPVSShowDlg::CPVSShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPVSShowDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPVSShowDlg)
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDI_LOGOSMALL);

	bitmapW = bitmapH = 0;
	bitmapData = NULL;

    ZeroMemory(&bitmapInfo, sizeof(BITMAPINFO));
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth  = 0;
    bitmapInfo.bmiHeader.biHeight = 0;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage = 0;
    bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    bitmapInfo.bmiHeader.biClrImportant = 0;

	//terrainPVS = NULL;
	terrainPVS = new pgTerrainPVS();

	dataW = dataH = 0;
	data = NULL;

	patchW = patchH = 0;
	minData = maxData = pvsData = NULL;

	pvs = false;
	pvsX = pvsY = 0;
	pvsZAbs = true;

	curZ = 0;
}

void CPVSShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPVSShowDlg)
	DDX_Control(pDX, IDC_CHECKYD, m_CheckYD);
	DDX_Control(pDX, IDC_CHECKXD, m_CheckXD);
	DDX_Control(pDX, IDC_CHECKV, m_CheckV);
	DDX_Control(pDX, IDC_CHECKZ, m_CheckZS);
	DDX_Control(pDX, IDC_CHECKY, m_CheckYS);
	DDX_Control(pDX, IDC_CHECKX, m_CheckXS);
	DDX_Control(pDX, IDC_INFO2, m_Info2);
	DDX_Control(pDX, IDC_HEIGHT_SLIDER, m_HeightSlider);
	DDX_Control(pDX, IDC_HEIGHT_TXT, m_HeightTxt);
	DDX_Control(pDX, IDC_INFO, m_Info);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPVSShowDlg, CDialog)
	//{{AFX_MSG_MAP(CPVSShowDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_MIN, OnMin)
	ON_BN_CLICKED(IDC_MAX, OnMax)
	ON_BN_CLICKED(IDC_PVS, OnPvs)
	ON_BN_CLICKED(IDC_HEIGHT, OnHeight)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CALCPVS, OnCalcPVS)
	ON_BN_CLICKED(IDC_LOADPVS, OnLoadPVS)
	ON_BN_CLICKED(IDC_SAVEPVS, OnSavePVS)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_HEIGHT_TXT, OnHeightTxt)
	ON_BN_CLICKED(IDC_SHRINKPVS, OnShrinkPVS)
	ON_EN_CHANGE(IDC_CHECKX, OnChangeCheck)
	ON_EN_CHANGE(IDC_CHECKY, OnChangeCheck)
	ON_EN_CHANGE(IDC_CHECKZ, OnChangeCheck)
	ON_EN_CHANGE(IDC_CHECKXD, OnChangeCheck)
	ON_EN_CHANGE(IDC_CHECKYD, OnChangeCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPVSShowDlg Nachrichten-Handler

BOOL CPVSShowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	pvsZ = 150;
	m_HeightSlider.SetRange(0, 600);
	m_HeightSlider.SetPos(pvsZ);
	UpdateHeight();

	Open("terV5/heightmap.raw");

	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CPVSShowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CPVSShowDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

		if(bitmapData)
		{
			CRect rc;
			GetDlgItem(IDC_FRAME)->GetWindowRect(rc);
			ScreenToClient(rc);

			CDC* pDC = GetDC();

			//if(bitmapW>=256 || bitmapH>=256)
			//	SetDIBitsToDevice(pDC->m_hDC, rc.left+2,rc.top+2, bitmapW,bitmapH, 0,0, (UINT)0, bitmapH, bitmapData, &bitmapInfo, DIB_RGB_COLORS);
			//else
				StretchDIBits(pDC->m_hDC,
							  rc.left+2,rc.top+2,
							  513,513,
							  0,0,
							  bitmapW,bitmapH,
							  bitmapData, &bitmapInfo,
							  DIB_RGB_COLORS, SRCCOPY);

			if(pvs)
			{
				int x0,y0,
					s = 3;
				CBrush green(RGB(0,255,0)), red(RGB(255,0,0));

				for(y0=0; y0<patchH; y0++)
					for(x0=0; x0<patchW; x0++)
					{
						int x = (int)(rc.left + (x0+0.5) * rc.Width() / patchW),
							y = (int)(rc.top + (y0+0.5) * rc.Width() / patchH);

						CRect rc2(x-s,y-s, x+s,y+s);
						pDC->FillRect(rc2, pvsData[x0+y0*patchW] ? &green : &red);
					}

			}

			ReleaseDC(pDC);


			if(pvsX>=0 && pvsX<patchW && pvsY>=0 && pvsY<patchH)
				curZ = (maxData[pvsX + pvsY*patchW] + minData[pvsX + pvsY*patchW])/2;

			CString str;
			str.Format("%i %i : %i\n", pvsX, pvsY, curZ);
			m_Info.SetWindowText(str);
		}
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CPVSShowDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



bool
CPVSShowDlg::Open(const CString& nFileName) 
{
	CFileStatus status;
	CFile::GetStatus(nFileName, status);
	int			x0,y0, x1,y1;

	dataW = dataH = (int)sqrt(status.m_size);
	if(dataW*dataH != status.m_size)
	{
		MessageBox("Bitmap not of square size", "Error");
		return false;
	}

	FILE* fp = fopen(nFileName, "rb");
	if(!fp)
	{
		MessageBox("Bitmap file could not be opened", "Error");
		return false;
	}

	if(data)
		delete data;

	data = new unsigned char[dataW*dataH];


	for(y0=0; y0<dataH; y0++)
		for(x0=0; x0<dataW; x0++)
			data[x0 + y0*dataW] = fgetc(fp);

	patchW = (dataW-1) / 16;
	patchH = (dataH-1) / 16;

	if(minData)
		delete minData;
	minData = new unsigned char[patchW*patchH];

	if(maxData)
		delete maxData;
	maxData = new unsigned char[patchW*patchH];

	if(pvsData)
		delete pvsData;
	pvsData = new unsigned char[patchW*patchH];


	for(x0=0; x0<patchW; x0++)
		for(y0=0; y0<patchH; y0++)
		{
			int min = 255, max = 0;
			unsigned char* src = data + x0*16 + y0*16*dataW;

			for(x1=0; x1<=16; x1++)
				for(y1=0; y1<=16; y1++)
				{
					unsigned char c = src[x1 + y1*dataW];
					if(c<min)
						min = c;
					if(c>max)
						max = c;
				}

			minData[x0 + y0*patchW] = min;
			maxData[x0 + y0*patchW] = max;
		}

	fclose(fp);

	OnHeight();

	return true;
}

void CPVSShowDlg::OnOpen() 
{
	CFileDialog dlg(TRUE, "*.raw", NULL, 0, "Raw Files (*.raw)|*.raw|All Files (*.*)|*.*||");

	if(dlg.DoModal()==IDOK)
		Open(dlg.GetPathName());
	
}


void
CPVSShowDlg::SetBitmapData(int nW, int nH, const unsigned char* nData)
{
	bitmapW = nW;
	bitmapH = nH;
	bitmapInfo.bmiHeader.biWidth  = nW;
	bitmapInfo.bmiHeader.biHeight = nH;

	if(bitmapData)
		delete bitmapData;
	bitmapData = new RGBQUAD[nW*nH];

	for(int y0=0; y0<nH; y0++)
		for(int x0=0; x0<nW; x0++)
		{
			unsigned char c = nData[x0 + y0*nW];
			int idx = x0 + (nH-1-y0) * nW;

			bitmapData[idx].rgbRed = bitmapData[idx].rgbGreen = bitmapData[idx].rgbBlue = c;
		}
}


void CPVSShowDlg::OnHeight()
{
	pvs = false;
	SetBitmapData(dataW, dataH, data);
	RedrawWindow();
}


void CPVSShowDlg::OnMin() 
{
	SetBitmapData(patchW, patchH, minData);
	pvs = false;
	RedrawWindow();
}


void CPVSShowDlg::OnMax() 
{
	SetBitmapData(patchW, patchH, maxData);
	pvs = false;
	RedrawWindow();
}


void CPVSShowDlg::OnPvs() 
{
	pvs = true;
}


void
CPVSShowDlg::CalcPVS(int minH, int maxH, int stepsH)
{
	terrainPVS->calc(minData,maxData, patchW,patchH,stepsH, 20);
}


/*void
CPVSShowDlg::CalcPVSForCell(int nX, int nY, int nZ)
{
	int x0,y0;

	if(nZ==-1)
		nZ = (minData[nX + nY*patchW] + maxData[nX + nY*patchW])/2 + 0;

	for(y0=0; y0<patchH; y0++)
		for(x0=0; x0<patchW; x0++)
		{
			unsigned char	&vis = pvsData[x0 + y0*patchW],
							hMax = maxData[x0 + y0*patchW];

			float	p, step = 1.0f / (float)sqrt((x0-nX)*(x0-nX) + (y0-nY)*(y0-nY)),
					dx = (float)x0-nX,
					dy = (float)y0-nY,
					dz = (float)hMax-nZ;

			vis = 255;
			for(p=0.0f; p<=1.0f; p+=step)
			{
				int x = (int)(nX + dx*p),
					y = (int)(nY + dy*p),
					z = (int)(nZ + dz*p);

				if(z < minData[x + y*patchW])
				{
					vis = 0;
					break;
				}
			}
		}
}*/


void CPVSShowDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(pvs)
	{
		CRect rc;
		GetDlgItem(IDC_FRAME)->GetWindowRect(rc);
		ScreenToClient(rc);

		pvsX = (point.x - rc.left) * patchW / rc.Width(),
		pvsY = (point.y - rc.top) * patchH / rc.Height();

		int x,y;
		for(y=0; y<patchH; y++)
			for(x=0; x<patchW; x++)
				pvsData[x+y*patchW] = terrainPVS->isVisible(pvsX,pvsY,(pvsZAbs ? pvsZ : pvsZ+curZ)/terrainPVS->getDataDZ(), x,y) ? 255 : 0;

		InvalidateRect(&rc, FALSE);
	}

	CDialog::OnMouseMove(nFlags, point);
}


void CPVSShowDlg::OnCalcPVS()
{
	CalcPVS(pvsX,pvsY,32);
	
	GetDlgItem(IDC_SAVEPVS)->EnableWindow(TRUE);
	GetDlgItem(IDC_PVS)->EnableWindow(TRUE);

	UpdateInfo2();
}


void CPVSShowDlg::OnLoadPVS() 
{
	static char BASED_CODE szFilter[] = "PVS Files (*.pvs)|*.pvs|All Files (*.*)|*.*||";
 
	CFileDialog dlg(TRUE, ".pvs", NULL, 0, szFilter, this);

	if(dlg.DoModal()==IDOK)
	{
		terrainPVS->load(dlg.GetPathName());

		GetDlgItem(IDC_SAVEPVS)->EnableWindow(TRUE);
		GetDlgItem(IDC_PVS)->EnableWindow(TRUE);
	}

	UpdateInfo2();
}

void CPVSShowDlg::OnSavePVS() 
{
	static char BASED_CODE szFilter[] = "PVS Files (*.pvs)|*.pvs|All Files (*.*)|*.*||";
 
	CFileDialog dlg(FALSE, ".pvs", NULL, 0, szFilter, this);

	if(dlg.DoModal()==IDOK)
		terrainPVS->save(dlg.GetPathName());
}


void CPVSShowDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(pScrollBar->m_hWnd==m_HeightSlider.m_hWnd)
		UpdateHeight();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPVSShowDlg::OnHeightTxt() 
{
	pvsZAbs = !pvsZAbs;

	UpdateHeight();
}


void CPVSShowDlg::UpdateHeight()
{
	pvsZ = m_HeightSlider.GetPos();

	CString str;
	str.Format(pvsZAbs ? "abs. Höhe: %d" : "rel. Höhe: %d", pvsZ);
	m_HeightTxt.SetWindowText(str);
}

void CPVSShowDlg::OnShrinkPVS() 
{
	terrainPVS->shrink();

	UpdateInfo2();
}


void CPVSShowDlg::UpdateInfo2()
{
	CString str;
	str.Format("PVS: %d %d %d", terrainPVS->getDataX(), terrainPVS->getDataY(), terrainPVS->getDataZ());
	m_Info2.SetWindowText(str);
}

void CPVSShowDlg::OnChangeCheck() 
{
	CString str;
	int xs,ys,zs, xd,yd;

	m_CheckXS.GetWindowText(str);
	xs = atoi(str);
	m_CheckYS.GetWindowText(str);
	ys = atoi(str);
	m_CheckZS.GetWindowText(str);
	zs = atoi(str);

	m_CheckXD.GetWindowText(str);
	xd = atoi(str);
	m_CheckYD.GetWindowText(str);
	yd = atoi(str);

	m_CheckV.SetWindowText((terrainPVS && terrainPVS->isVisible(xs,ys,zs, xd,yd)) ? "1" : "0");
}

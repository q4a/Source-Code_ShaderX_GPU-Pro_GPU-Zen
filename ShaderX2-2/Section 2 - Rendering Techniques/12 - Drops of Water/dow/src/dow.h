// --------------------------------------------------------------
// --------------------------------------------------------------
//
// dow.h
//
//    Drops of Water source code - main application class header file
//
//                      ShaderX2 - www.shaderx2.com
//
//
//    Author:   Sylvain Lefebvre - 04/2003
//    ------
//
//    -= Check for updates at http://www.aracknea.net =-
//
//    File contents:
//    -------------
//
// --------------------------------------------------------------
// --------------------------------------------------------------
#ifndef __DOW__
#define __DOW__
// --------------------------------------------------------------
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <Cg/Cg.h>
#include <Cg/CgD3D9.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "D3DFile.h"
#include "MovieMaker.h"
#include "ScreenCapture.h"
// --------------------------------------------------------------
#include "CD3DQuad.h"
#include "CD3DCylinder.h"
// --------------------------------------------------------------
#include "drop.h"
// --------------------------------------------------------------
#include "libtexture/d3dtex.h"
// --------------------------------------------------------------
#define CHECK_ERROR(x) if ((x) != D3D_OK) MessageBox(NULL,#x,"Error",MB_OK | MB_ICONSTOP);
// --------------------------------------------------------------
struct DOWVERTEX
{
    FLOAT x, y, z;
    FLOAT nx, ny, nz;
	FLOAT u, v;
    FLOAT tx, ty, tz;
    FLOAT bx, by, bz;
};
//-----------------------------------------------------------------------------
#define D3DFVF_DOWVERTEX (D3DFVF_XYZ | \
	D3DFVF_NORMAL | \
	D3DFVF_TEX3 | \
	D3DFVF_TEXCOORDSIZE2(0) | \
	D3DFVF_TEXCOORDSIZE2(1) | \
D3DFVF_TEXCOORDSIZE2(2))
//-----------------------------------------------------------------------------
#define NB_OFFSET_MAP    4
//-----------------------------------------------------------------------------
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif
//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CDropAnimator;
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
protected:
	MovieMaker              m_VCR;
	ScreenCapture           m_ScrCap;
	bool                    m_bRecording;
	bool                    m_bHideText;
	CD3DArcBall             m_ArcBall;
	double                  m_dDist;
	CD3DFont               *m_pFont;
	int						m_iTech;
	double                  m_dLastSprite[4];
	double                  m_dZAngle;

	LPDIRECT3DVERTEXBUFFER9 m_PointVB;
	CD3DQuad               *m_Quad;
	CD3DCylinder           *m_Cyl;
	
	D3DXMATRIXA16			m_matProj;
	D3DXMATRIXA16			m_matView;
	D3DXMATRIXA16			m_matViewI;
	
	LPDIRECT3DSURFACE9      m_pWetAreasSurface;
	
	LPDIRECT3DTEXTURE9      m_pDropNormalTexture;
	LPDIRECT3DTEXTURE9      m_pColorMapTexture;
	LPDIRECT3DTEXTURE9      m_pNormalMapTexture;
	LPDIRECT3DTEXTURE9      m_pWetAreasTexture;
	LPDIRECT3DTEXTURE9      m_pSplashTexture;
	// Ask for wet floor clearing
	bool                    m_bClearWetAreas;
	
	// Menus
	bool                    m_bWireframe;
	bool                    m_bUniform;
	bool					m_bStop;
	bool                    m_bHideDrops;
	bool                    m_bStopDrops;
	bool					m_bShowWetAreas;
	bool					m_bMagEffectDemo;
	bool	 				m_bShowHelp;
	bool                    m_bCylinder;
	
	// Cg
	CGcontext				m_cgContext;
	CGprogram               m_cgVertexProg;
	CGprogram               m_cgFragmentProg;
	CGprogram               m_cgFragmentProgWetAreas;
	CGprogram               m_cgFragmentProgFloor;
	
	CGparameter             m_cgProj;
	CGparameter             m_cgView;
	CGparameter             m_cgViewI;
	CGparameter             m_cgViewerPos;
	CGparameter             m_cgTScale;
	CGparameter             m_cgLightPos;
	CGparameter             m_cgTexRepeat;
	
	CGparameter             m_cgOffsetMap;
	CGparameter             m_cgColorMap;
	CGparameter             m_cgDropNrms;
	CGparameter             m_cgMagCoeff;
	CGparameter             m_cgViewCoeff;
	
	CGparameter             m_cgAreasOffsetMap;
	CGparameter             m_cgAreasDropNrms;
	
	CGparameter             m_cgFloorWetAreas;
	CGparameter             m_cgFloorColorMap;
	CGparameter             m_cgFloorNrmsMap;
	
	// Texture Sprites
	int                     m_iSelectedOffsetMapRes;
	BYTE                   *m_OffsetMap[NB_OFFSET_MAP];
	LPDIRECT3DTEXTURE9      m_pOffsetMap[NB_OFFSET_MAP];
	UINT                    m_OffsetMapRes[NB_OFFSET_MAP];
	UINT                    m_dwRefTexNbTiles;
	bool                   *m_OffsetMapOverlap[NB_OFFSET_MAP];
	int                     m_NbSpritesInOffsetMap[NB_OFFSET_MAP];
	
	// Drops
	drops					m_Pos;
	float                   m_fRepeat;
	float					m_fView;
	float					m_fMag;

	// Impact
	double                  m_Impact[2];
	bool                    m_bDrawImpact;
	double                  m_dImpactSize;

	// Init
	void initDropsAndTexSprites();
	void loadUnderlyingTexture(const char *tex_name,const char *tex_nrm_name,double scale);
	
	// Animator
	CDropAnimator *m_Animator;
	
protected:
	
	// Direct3D
	HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );
	void    Draw();
	HRESULT Init();
	HRESULT Render();
	HRESULT RenderWetAreas();
	HRESULT FrameMove();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects(); 
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();
	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	VOID    SetMenuStates();
	
	HRESULT CreateWetAreas();
	
	
public:
	
	CMyD3DApplication();


	void    ClearWetAreas() {m_bClearWetAreas=true;}
	int     GetOffsetMapRes() const {return (m_iSelectedOffsetMapRes);}

	// Texture Sprites
	HRESULT CreateOffsetMap();
	VOID    ClearOffsetMap();
	HRESULT UpdateOffsetMap();
	void    IJFromUV(int map,double u,double v,int& i,int& j);
	bool    Overlap(double u,double v);
	bool    Overlap(int map,double u,double v);
	bool    PutSprite(int map,double u,double v,double angle,double scale,bool force=false);
	void    RefTexIJ2OffsetMapIJ(int map,
								 int ti,int tj,int gi,int gj,
								 double decalx,double decaly,
								 double angle,double scale,
								 bool clear=false);
	void    Impact(double x,double y,double sz) {m_bDrawImpact=true;m_Impact[0]=x;m_Impact[1]=y;m_dImpactSize=sz;}

};
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------

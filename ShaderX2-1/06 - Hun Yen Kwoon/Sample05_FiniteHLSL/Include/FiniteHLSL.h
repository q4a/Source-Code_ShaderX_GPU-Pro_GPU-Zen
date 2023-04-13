#ifndef __SHADOWVOLUME_FINITEHLSL_H__
#define __SHADOWVOLUME_FINITEHLSL_H__

// Toggle CRT memory tracing
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#define STRICT
#include <Windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <D3DX9.h>
#include <tchar.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "D3DFile.h"
#include "d3dx9effect.h"
#include "dxerr9.h"

#include "Shadow.h"


struct BIGSQUAREVERTEX
{
  D3DXVECTOR4 p;
  D3DCOLOR    color;
};

#define D3DFVF_BIGSQUAREVERTEX ( D3DFVF_XYZRHW|D3DFVF_DIFFUSE )
#define D3DFVF_MESHVERTEX      ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 )

enum FillModeEnum{
  POINT_MODE  = 0,
  SHADE_MODE  = 1,
  WIRE_MODE   = 2
};


//-----------------------------------------------------------------------------
// class CFiniteHLSL
//-----------------------------------------------------------------------------
class CFiniteHLSL : public CD3DApplication
{
  BOOL                    m_bKeyInfo;
  BOOL                    m_bShowObjects;
  BOOL                    m_bTwoSidedStencil;     // 2-sided stencil mode support
  BOOL                    m_bRunTwoSidedStencil;  // Running 2-sided stencil mode?
  BOOL                    m_bAnimateLight;
  BOOL                    m_bShowShadowVol;

  D3DXMATRIX              m_matProject;
  D3DXMATRIX              m_matView;
  D3DXMATRIX              m_matWorld;
  D3DXMATRIX              m_matPlatform;
  D3DXMATRIX              m_matLight;
  D3DXMATRIX              m_matObject;
  D3DXMATRIX              m_matObject2;

  CD3DFont*               m_pFont;
  CD3DMesh*               m_pWorldMesh;
  CD3DMesh*               m_pPlatformMesh;
  CD3DMesh*               m_pObjectMesh;
  CD3DMesh*               m_pObjectMesh2;
  CShadow*                m_pShadow;
  CShadow*                m_pShadow2;
  LPD3DXMESH              m_pSphereMesh;
  D3DMATERIAL9            m_LightMaterial;

  BYTE                    m_bKey[256];
  FillModeEnum            FillMode;

  D3DLIGHT9               m_Light;
  D3DXVECTOR3             m_vLightPos;
  D3DXVECTOR4             m_vOLP;             // Vector of light pos in obj space

  // For camera movements
  D3DXVECTOR3             m_vAngularSpeed;
  D3DXVECTOR3	            m_vPosition;
  D3DXVECTOR3             m_vDist;
  D3DXVECTOR3             m_vYPR;             // Yaw, Pitch, Roll accumulation
	D3DXVECTOR3	            m_vYPR_Dist;        // Yaw, Pitch, Roll speeds
  D3DXMATRIX              m_matOrientation;   // Camera orientation
  FLOAT                   m_fAngularSpeed;
  FLOAT                   m_fSpeed;

  LPD3DXEFFECT            m_pEffect;
  D3DXHANDLE              m_hmClip;           // Handle to mat World*View*Project
  D3DXHANDLE              m_hvOLP;            // Handle to light pos in obj space

  LPDIRECT3DVERTEXBUFFER9 m_pBigSquareVB;

protected:
  HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );
  HRESULT OneTimeSceneInit();
  HRESULT InitDeviceObjects();
  HRESULT RestoreDeviceObjects();
  HRESULT InvalidateDeviceObjects();
  HRESULT DeleteDeviceObjects();
  HRESULT Render();
  HRESULT RenderScene();
  HRESULT RenderShadowVolume();
  HRESULT DrawShadow();
  HRESULT FrameMove();
  HRESULT FinalCleanup();
  HRESULT LoadEffectsFile();

public:
  LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
  CFiniteHLSL();
};



#endif // __SHADOWVOLUME_FINITEHLSL_H__
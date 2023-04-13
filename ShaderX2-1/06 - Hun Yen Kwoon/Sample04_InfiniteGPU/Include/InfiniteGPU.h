#ifndef __SHADOWVOLUME_INFINITEGPU_H__
#define __SHADOWVOLUME_INFINITEGPU_H__

#define STRICT
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <D3DX8.h>
#include <D3DApp.h>
#include <D3DFile.h>
#include <D3DFont.h>
#include <D3DUtil.h>
#include <DXUtil.h>
#include <Shadow.h>

struct BIGSQUAREVERTEX
{
  D3DXVECTOR4 p;
  D3DCOLOR    color;
};

#define D3DFVF_BIGSQUAREVERTEX ( D3DFVF_XYZRHW|D3DFVF_DIFFUSE )
#define D3DFVF_MESHVERTEX       ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 )

enum FillModeEnum{
  POINT_MODE  = 0,
  SHADE_MODE  = 1,
  WIRE_MODE   = 2
};


//-----------------------------------------------------------------------------
// class CDepthFail
//-----------------------------------------------------------------------------
class CDepthFail : public CD3DApplication
{
private:
  BOOL m_bKeyInfo;
  D3DXMATRIX              m_matProject;
  D3DXMATRIX              m_matView;
  D3DXMATRIX              m_matWorld;
  D3DXMATRIX              m_matPlatform;
  D3DXMATRIX              m_matLight;
  D3DXMATRIX              m_matPosition;
  D3DXMATRIX              m_matObject;
  D3DXMATRIX              m_matObject2;

  CD3DFont*               m_pFont;
  CD3DMesh*               m_pWorldMesh;
  CD3DMesh*               m_pPlatformMesh;
  LPD3DXMESH              m_pSphereMesh;
  CD3DMesh*               m_pObjectMesh;
  CD3DMesh*               m_pObjectMesh2;

  CShadow*                m_pShadow;
  CShadow*                m_pShadow2;

  BYTE                    m_bKey[256];
  FillModeEnum            m_eFillMode;

  D3DLIGHT8               m_Light;
  D3DMATERIAL8            m_LightMaterial;

  D3DXVECTOR3             m_vLightPos;

  // For camera movements
  D3DXVECTOR3	            m_vPosition;
  D3DXVECTOR3             m_vDist;
  D3DXVECTOR3             m_vYPR;             // Yaw, Pitch, Roll accumulation
	D3DXVECTOR3	            m_vYPR_Dist;    // Yaw, Pitch, Roll speeds
  D3DXMATRIX              m_matOrientation;   // Camera orientation
  FLOAT                   m_fAngularSpeed;
  FLOAT                   m_fSpeed;

  BOOL                    m_bShowShadowVolFrontFace;
  BOOL                    m_bShowShadowVolBackFace;
  BOOL                    m_bShowObjects;
  BOOL                    m_bAnimateLight;
  
  LPDIRECT3DVERTEXBUFFER8 m_pBigSquareVB;

protected:
  HRESULT ConfirmDevice( D3DCAPS8*, DWORD, D3DFORMAT );
  HRESULT OneTimeSceneInit();
  HRESULT InitDeviceObjects();
  HRESULT RestoreDeviceObjects();
  HRESULT InvalidateDeviceObjects();
  HRESULT DeleteDeviceObjects();
  HRESULT Render();
  HRESULT RenderScene();
  HRESULT FrameMove();
  HRESULT FinalCleanup();
  HRESULT RenderShadowVolume();
  HRESULT DrawShadow();

public:
  LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
  CDepthFail();
};


inline DWORD F2DW(FLOAT f)
{
  return *((DWORD*)&f);
}

#endif // __SHADOWVOLUME_INFINITEGPU_H__
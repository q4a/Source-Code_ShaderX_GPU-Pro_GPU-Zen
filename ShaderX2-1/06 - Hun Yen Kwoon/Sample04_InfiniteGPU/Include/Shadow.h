#ifndef __SHADOWVOLUME_SHADOW_H__
#define __SHADOWVOLUME_SHADOW_H__

#include <D3dx8math.h>

typedef struct _BASEMESHVERTEX { 
  D3DXVECTOR3 p, n; 
  FLOAT tu, tv; 
}BASEMESHVERTEX;

typedef struct _SHADOWMESHVERTEX {
  D3DXVECTOR3 p, n;
}SHADOWMESHVERTEX;


//-----------------------------------------------------------------------------
// class CShadow
// The class that handles the maintainance of shadow volume geometries for
// use by the vertex shader
//-----------------------------------------------------------------------------
class CShadow
{
public:
  CShadow();
  ~CShadow();
  
public:
  void RenderShadow();
  void SetShaderConstants( const D3DLIGHT8* pLight, const D3DXMATRIX* matWorld, 
        const D3DXMATRIX* matView, const D3DXMATRIX* matProj );
  HRESULT InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
  HRESULT InvalidateDeviceObjects();
  HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice, 
        LPD3DXMESH pMesh );
  HRESULT Destroy();

private:
  HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH pMesh );
  HRESULT CreateVSFromCompiledFile ( IDirect3DDevice8* m_pd3dDevice, 
        DWORD* dwDeclaration, TCHAR* strVSPath, DWORD* m_dwVS );

private:
  LPDIRECT3DDEVICE8         m_pd3dDevice;

  DWORD                     m_dwIndexBufferSize;
  IDirect3DIndexBuffer8*    m_pIndexBuffer;

  DWORD                     m_dwVertexBufferSize;
  IDirect3DVertexBuffer8*   m_pVertexBuffer;

  DWORD                     m_dwVertexShader;
  DWORD                     m_dwNumOfVertices;  // No of vertices
};

#endif // __SHADOWVOLUME_SHADOW_H__

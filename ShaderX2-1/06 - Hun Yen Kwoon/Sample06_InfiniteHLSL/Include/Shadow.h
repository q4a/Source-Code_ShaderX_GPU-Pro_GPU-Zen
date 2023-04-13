#ifndef __SHADOWVOLUME_SHADOW_H__
#define __SHADOWVOLUME_SHADOW_H__

#include <D3dx9math.h>

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
  HRESULT InvalidateDeviceObjects();
  HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, 
        LPD3DXMESH pMesh );
  HRESULT Destroy();

private:
  HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXMESH pMesh );

private:
  LPDIRECT3DDEVICE9         m_pd3dDevice;

  DWORD                     m_dwIndexBufferSize;
  IDirect3DIndexBuffer9*    m_pIndexBuffer;

  DWORD                     m_dwVertexBufferSize;
  IDirect3DVertexBuffer9*   m_pVertexBuffer;

  DWORD                     m_dwNumOfVertices;  // No of vertices
};


#endif // __SHADOWVOLUME_SHADOW_H__
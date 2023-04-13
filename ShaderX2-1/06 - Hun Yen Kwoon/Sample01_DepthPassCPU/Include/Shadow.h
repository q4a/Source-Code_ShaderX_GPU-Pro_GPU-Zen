#ifndef __SHADOWVOLUME_SHADOW_H__
#define __SHADOWVOLUME_SHADOW_H__

#include <D3dx8math.h>

typedef struct _MESHVERTEX { 
  D3DXVECTOR3 p, n; 
  FLOAT tu, tv; 
}MESHVERTEX;


//-----------------------------------------------------------------------------
// class CShadow
//-----------------------------------------------------------------------------
class CShadow
{
public:
  CShadow( DWORD dwSize );
  ~CShadow();
  D3DXVECTOR3*  m_pVertices;
  DWORD         m_dwNumOfVertices;
  
public:
  VOID Reset();
  HRESULT BuildShadowVolume( LPD3DXMESH pMesh, D3DXVECTOR3 vLight );
  HRESULT RenderShadowVolume( LPDIRECT3DDEVICE8 pd3dDevice );

private:
  VOID InsertEdge( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 );
};

#endif // __SHADOWVOLUME_SHADOW_H__

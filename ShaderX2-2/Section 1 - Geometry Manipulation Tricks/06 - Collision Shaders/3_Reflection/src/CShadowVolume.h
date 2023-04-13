// -------------------------------------------------------------
// Shaodw Volume model
// 
// Copyright (c) 2002-2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------
#ifndef _CShadowVolume_h_
#define _CShadowVolume_h_

#include <d3d9.h>
#include <d3dx9.h>

class CShadowVolume {
public:
	struct SHADOW_VOLUME_VERTEX { D3DXVECTOR3 p, n; FLOAT t[2];FLOAT dt[2];};

    SHADOW_VOLUME_VERTEX*		m_pVertices;	// Vertex datas
	DWORD						m_dwNumFaces;	// num of faces
public:
	
	CShadowVolume();
	~CShadowVolume();

	HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXMESH pObject, FLOAT dt[2] );
	VOID Render( LPDIRECT3DDEVICE9 pd3dDevice );
	VOID Destroy();
};
#endif // !_CShadowVolume_h_

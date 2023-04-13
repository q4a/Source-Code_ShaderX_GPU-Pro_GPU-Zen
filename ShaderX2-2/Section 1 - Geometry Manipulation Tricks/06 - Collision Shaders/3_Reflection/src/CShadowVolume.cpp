
#include <assert.h>
#include <stdio.h>
#include "D3DFile.h"
#include "DXUtil.h"
#include "CShadowVolume.h"

// ---------------------------------------------------------------------------
// static object
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
CShadowVolume::CShadowVolume()
{
    m_pVertices = NULL;
	m_dwNumFaces = 0;
}
// ---------------------------------------------------------------------------
CShadowVolume::~CShadowVolume()
{
}
// ---------------------------------------------------------------------------
// rendering
// ---------------------------------------------------------------------------
VOID CShadowVolume::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_dwNumFaces,
								m_pVertices, sizeof(SHADOW_VOLUME_VERTEX) );
}
// ---------------------------------------------------------------------------
// creation
// ---------------------------------------------------------------------------
HRESULT CShadowVolume::Create( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXMESH pSrcMesh, FLOAT dt[2] )
{
	HRESULT ret = S_OK;
	struct MESHVERTEX { D3DXVECTOR3 p,n; };
	MESHVERTEX* pVertices;
    WORD*       pIndices;
	DWORD i,j,k,l, face;
	LPD3DXMESH pMesh;

    if( FAILED( pSrcMesh->CloneMeshFVF(	D3DXMESH_SYSTEMMEM,
										D3DFVF_XYZ|D3DFVF_NORMAL,
                                        pd3dDevice, &pMesh ) ) )
        return E_FAIL;
	DWORD dwNumFaces    = pMesh->GetNumFaces();

	// allocate memory
	m_dwNumFaces = 4*dwNumFaces;
	m_pVertices = new SHADOW_VOLUME_VERTEX[3*m_dwNumFaces];

	// lock buffer
    pMesh->LockVertexBuffer( 0L, (LPVOID*)&pVertices );
    pMesh->LockIndexBuffer ( 0L, (LPVOID*)&pIndices );

    for( i=0; i<3*m_dwNumFaces; i++ )
	{
		m_pVertices[i].dt[0] = dt[0];
		m_pVertices[i].dt[1] = dt[1];
	}

	// for save normal vector
    D3DXVECTOR3 *vNormal = new D3DXVECTOR3[dwNumFaces];
	if(NULL==vNormal){
		m_dwNumFaces = 0;
        ret = E_OUTOFMEMORY;
		goto end;
	}
	// make datas
    for( i=0; i<dwNumFaces; i++ )
	{
        D3DXVECTOR3 v0 = pVertices[pIndices[3*i+0]].p;
        D3DXVECTOR3 v1 = pVertices[pIndices[3*i+1]].p;
        D3DXVECTOR3 v2 = pVertices[pIndices[3*i+2]].p;
        D3DXVECTOR3 vCross1 = D3DXVECTOR3(v1-v0);
        D3DXVECTOR3 vCross2 = D3DXVECTOR3(v2-v1);
        D3DXVec3Cross( &vNormal[i], &vCross1, &vCross2 );
		
		m_pVertices[3*i+0].p = v0;
		m_pVertices[3*i+1].p = v1;
		m_pVertices[3*i+2].p = v2;
		m_pVertices[3*i+0].n = vNormal[i];
		m_pVertices[3*i+1].n = vNormal[i];
		m_pVertices[3*i+2].n = vNormal[i];
    }
	
	// make edge data
	face = dwNumFaces;
	for( i=0  ; i<dwNumFaces; i++ ){
	for( j=i+1; j<dwNumFaces; j++ ){
		DWORD id[9][2];
		DWORD cnt=0;
		for(k=0;k<3;k++){
		for(l=0;l<3;l++){
			D3DXVECTOR3 dv;
			D3DXVec3Subtract( &dv,	&pVertices[pIndices[3*i+k]].p,
									&pVertices[pIndices[3*j+l]].p);
			if( D3DXVec3LengthSq( &dv ) < 0.00001f ){
				// search data which index of vertes is same
				id[cnt][0] = 3*i+k;
				id[cnt][1] = 3*j+l;
				cnt++;
			}
		}
		}
		if(2==cnt){
			// have a share edge
			if(id[1][0]-id[0][0]!=1){
				// swap for culling
				DWORD tmp = id[0][0];
				id[0][0] = id[1][0];
				id[1][0] = tmp;
				tmp = id[0][1];
				id[0][1] = id[1][1];
				id[1][1] = tmp;
			}
			// make polygones on the Edge
			m_pVertices[3*face+0].p = pVertices[pIndices[id[1][0]]].p;
			m_pVertices[3*face+2].p = pVertices[pIndices[id[0][1]]].p;
			m_pVertices[3*face+1].p = pVertices[pIndices[id[0][0]]].p;
			m_pVertices[3*face+0].n = vNormal[i];
			m_pVertices[3*face+2].n = vNormal[j];
			m_pVertices[3*face+1].n = vNormal[i];
			face++;
			m_pVertices[3*face+0].p = pVertices[pIndices[id[1][0]]].p;
			m_pVertices[3*face+2].p = pVertices[pIndices[id[1][1]]].p;
			m_pVertices[3*face+1].p = pVertices[pIndices[id[0][1]]].p;
			m_pVertices[3*face+0].n = vNormal[i];
			m_pVertices[3*face+2].n = vNormal[j];
			m_pVertices[3*face+1].n = vNormal[j];
			face++;
		}
	}
	}
	assert(face==m_dwNumFaces);

	delete[] vNormal;
end:
	// unlock buffer
    pMesh->UnlockIndexBuffer();
    pMesh->UnlockVertexBuffer();
	
	pMesh->Release();

	return ret;
}
// ---------------------------------------------------------------------------
// destroy
// ---------------------------------------------------------------------------
VOID CShadowVolume::Destroy()
{
    SAFE_DELETE_ARRAY( m_pVertices );
	m_dwNumFaces = 0;
}
// ---------------------------------------------------------------------------

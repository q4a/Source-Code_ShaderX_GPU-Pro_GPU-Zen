#include <Shadow.h>

FLOAT g_fExt = 300.0f; // Absolute extrusion distance


//-----------------------------------------------------------------------------
// CShadow constructor
// dwSize specifies the amount of memory allocated to the temporary vertex
// buffer. If an insufficient amount of memory is allocated, illegal memory
// access errors will occur.
//-----------------------------------------------------------------------------
CShadow::CShadow( DWORD dwSize )
{
  m_pVertices = (D3DXVECTOR3*)malloc( sizeof( D3DXVECTOR3 )*dwSize );
  m_dwNumOfVertices = 0L;
}


//-----------------------------------------------------------------------------
// CShadow destructor
//-----------------------------------------------------------------------------
CShadow::~CShadow()
{
  if ( m_pVertices )
    free( m_pVertices );
}


//-----------------------------------------------------------------------------
// RenderShadowVolume()
// Renders the shadow volume geometries
//-----------------------------------------------------------------------------
HRESULT CShadow::RenderShadowVolume( LPDIRECT3DDEVICE8 pd3dDevice )
{
  pd3dDevice->SetVertexShader( D3DFVF_XYZ );
  return pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_dwNumOfVertices/3, 
                                      m_pVertices, sizeof( D3DXVECTOR3 ) );
}


//-----------------------------------------------------------------------------
// Reset()
//-----------------------------------------------------------------------------
VOID CShadow::Reset()
{
  m_dwNumOfVertices = 0L;
}


//-----------------------------------------------------------------------------
// BuildShadowVolume()
// Constucts the shadow volume according to the light's incident ray vector.
//-----------------------------------------------------------------------------
HRESULT CShadow::BuildShadowVolume(LPD3DXMESH pMesh, D3DXVECTOR3 vLight)
{
  MESHVERTEX* pVertices;
  WORD*       pIndices;

  // Lock the geometry buffers
  pMesh->LockVertexBuffer( 0L, (BYTE**)&pVertices );
  pMesh->LockIndexBuffer( 0L, (BYTE**)&pIndices );
  DWORD dwNumVertices = pMesh->GetNumVertices();
  DWORD dwNumFaces    = pMesh->GetNumFaces();

  // Allocate a temporary edge list
  WORD* pEdges = new WORD[dwNumFaces*6];
  DWORD dwNumEdges = 0;

  // For each face, check all 3 edges
  for( DWORD i=0; i<dwNumFaces; i++ )
  {
    WORD wIndex0 = pIndices[3*i+0];
    WORD wIndex1 = pIndices[3*i+1];
    WORD wIndex2 = pIndices[3*i+2];

    D3DXVECTOR3 v0 = pVertices[wIndex0].p;
    D3DXVECTOR3 v1 = pVertices[wIndex1].p;
    D3DXVECTOR3 v2 = pVertices[wIndex2].p;

    // Note that vLight had already been transformed to object space. This saves some computations work
    // Cosine value larger than 0.0 meant light facing since the angle between the light vector vLight
    // and the face normal is within -90 to 90 degrees
    // Face normal is computed in order to use welded models
    D3DXVECTOR3 vCrossValue1(v2-v1);
    D3DXVECTOR3 vCrossValue2(v1-v0);
    D3DXVECTOR3 vFaceNormal;
    D3DXVec3Cross( &vFaceNormal, &vCrossValue1, &vCrossValue2 );

    // Take note that we are doing a recalculation of vLightDir, or direction vector of incoming light ray
    // by using the first vertex of a face (3 vertices) to represent that face. The dot product test is also
    // only done once per face.
    D3DXVECTOR3 vLightDir = vLight - v0;  // Direction vector of incoming light rays
    if( D3DXVec3Dot( &vFaceNormal, &vLightDir ) >= 0.0f )
    {
      InsertEdge( pEdges, dwNumEdges, wIndex0, wIndex1 );
      InsertEdge( pEdges, dwNumEdges, wIndex1, wIndex2 );
      InsertEdge( pEdges, dwNumEdges, wIndex2, wIndex0 );
    }
  }

  // For each silhouette edge, duplicate it, 
  for( i=0; i<dwNumEdges; i++ )
  {
    D3DXVECTOR3 v1 = pVertices[pEdges[2*i+0]].p;
    D3DXVECTOR3 v2 = pVertices[pEdges[2*i+1]].p;

    D3DXVECTOR3 v3;
    D3DXVECTOR3 v4;
    D3DXVECTOR3 vExtrusionDir;    // Direction vector from light to vertex, or rather the direction to extrude

    // Extrusion can be tricky. It is not advisable to extrude vertices by a multiple of the magnitude of 
    // vExtrusionDir. This is because the magnitude may be so small that even a large multiple would not extrude
    // the vertices far enough. Results will be unpredictable if either light source of occluders are dynamic 
    // objects. Hence, we normalize the vExtrusionDir vector before multiply by the ABSOLUTE distance
    // that we want to extrude the vertex to.

    vExtrusionDir = v1 - vLight;  // Compute a new extrusion direction for new vertex
    D3DXVec3Normalize( &vExtrusionDir, &vExtrusionDir );
    v3.x = v1.x + vExtrusionDir.x * g_fExt;
    v3.y = v1.y + vExtrusionDir.y * g_fExt;
    v3.z = v1.z + vExtrusionDir.z * g_fExt;
  
    vExtrusionDir = v2 - vLight;  // Compute a new extrusion direction for new vertex
    D3DXVec3Normalize( &vExtrusionDir, &vExtrusionDir );
    v4.x = v2.x + vExtrusionDir.x * g_fExt;
    v4.y = v2.y + vExtrusionDir.y * g_fExt;
    v4.z = v2.z + vExtrusionDir.z * g_fExt;

    // Add a quad (two triangles) to the vertex list
    m_pVertices[m_dwNumOfVertices++] = v1;
    m_pVertices[m_dwNumOfVertices++] = v4;
    m_pVertices[m_dwNumOfVertices++] = v2;

    m_pVertices[m_dwNumOfVertices++] = v1;
    m_pVertices[m_dwNumOfVertices++] = v3;
    m_pVertices[m_dwNumOfVertices++] = v4;
  }
  // Delete the temporary edge list
  delete[] pEdges;

  // Unlock the geometry buffers
  pMesh->UnlockVertexBuffer();
  pMesh->UnlockIndexBuffer();

  return S_OK;
}


//-----------------------------------------------------------------------------
// InsertEdge()
// Insert edges and removes duplicated internal edges. We are only interested 
// in edges forming the silohuette
//-----------------------------------------------------------------------------
VOID CShadow::InsertEdge( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 )
{
  for (DWORD i=0; i < dwNumEdges; i++)
  {
    if( ( pEdges[2*i+0] == v0 && pEdges[2*i+1] == v1 ) || 
        ( pEdges[2*i+0] == v1 && pEdges[2*i+1] == v0 ) )
    {
      if( dwNumEdges > 1 )
      {
        pEdges[2*i+0] = pEdges[2*( dwNumEdges-1 )+0];
        pEdges[2*i+1] = pEdges[2*( dwNumEdges-1 )+1];
      }
      dwNumEdges--;
      return;
    }
  }

  pEdges[2*dwNumEdges+0] = v0;
  pEdges[2*dwNumEdges+1] = v1;
  dwNumEdges++;
}
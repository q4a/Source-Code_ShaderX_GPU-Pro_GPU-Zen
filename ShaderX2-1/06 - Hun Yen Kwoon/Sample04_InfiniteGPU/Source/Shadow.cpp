#include <Shadow.h>
#include <map>
#include <vector>
#include <dxutil.h>
#include <d3dapp.h>

#define MERGE_COS_ANGLE_PROXIMITY 0.95f
#define FLOAT_EQUALITY_FUDGE 0.001f


//-----------------------------------------------------------------------------
// DebugPrintf()
// Takes variable argument list and outputs text to debug window
//-----------------------------------------------------------------------------
void DebugPrintf( const char *szFmt, ... )
{
    char sz[4096], szMsg[4096];
    va_list va;
    va_start( va, szFmt );
    vsprintf( szMsg, szFmt, va );
    va_end( va );
    sprintf( sz, "%s", szMsg );
    sz[sizeof( sz )-1] = '\0';
#ifdef _DEBUG
    OutputDebugString( sz );
#endif
}


//-----------------------------------------------------------------------------
// operator <
//-----------------------------------------------------------------------------
inline bool operator < ( const D3DXVECTOR3& lhs, const D3DXVECTOR3& rhs )
{
   D3DXVECTOR3 d = lhs - rhs;
   const FLOAT f = FLOAT_EQUALITY_FUDGE;
   if ( d.x < -f ) return true;
   if ( d.x > f ) return false;
   if ( d.y < -f ) return true;
   if ( d.y > f ) return false;
   if ( d.z < -f ) return true;
   return false;
}


//-----------------------------------------------------------------------------
// operator >
//-----------------------------------------------------------------------------
inline bool operator > ( const D3DXVECTOR3& lhs, const D3DXVECTOR3& rhs )
{
   if ( lhs == rhs ) return false;
   return !( lhs < rhs );
}


//-----------------------------------------------------------------------------
// class EdgePositions
// Encapsulates the 2 points that define an edge's positions
//-----------------------------------------------------------------------------
class EdgePositions {
public:
   D3DXVECTOR3 p0, p1;

   bool operator == ( const EdgePositions& rhs ) const
   {
      return ( rhs.p0 == p1 && rhs.p1 == p0 );
   }
   bool operator < ( const EdgePositions& rhs ) const
   {
      if ( p0 < rhs.p0 ) return true;
      if ( p0 > rhs.p0 ) return false;
      if ( p1 < rhs.p1 ) return true;
      return false;
   }
};


//-----------------------------------------------------------------------------
// class EdgeData
// Encapsulates the normals and index information of an edge
//-----------------------------------------------------------------------------
class EdgeData {
public:
  D3DXVECTOR3 n0, n1;
  WORD i0, i1;
};


//-----------------------------------------------------------------------------
// Typedefs and namespace
//-----------------------------------------------------------------------------
using namespace std;
typedef map<EdgePositions, EdgeData> EdgeMap;
typedef vector<SHADOWMESHVERTEX> ShadowVertices;
typedef ShadowVertices::iterator VertexIter;
typedef vector<DWORD> ShadowIndices;
typedef ShadowIndices::iterator IndexIter;


//-----------------------------------------------------------------------------
// CShadow constructor
//-----------------------------------------------------------------------------
CShadow::CShadow()
{
  m_pd3dDevice          = NULL;
  m_dwIndexBufferSize   = 0L;
  m_pIndexBuffer        = NULL;
  m_dwVertexBufferSize  = 0L;
  m_pVertexBuffer       = NULL;
  m_dwVertexShader	    = 0xffffffff;
}


//-----------------------------------------------------------------------------
// CShadow destructor
//-----------------------------------------------------------------------------
CShadow::~CShadow()
{
  SAFE_RELEASE( m_pIndexBuffer );
  SAFE_RELEASE( m_pVertexBuffer );

  SAFE_DELETE( m_pIndexBuffer );
  SAFE_DELETE( m_pVertexBuffer );
}


//-----------------------------------------------------------------------------
// RenderShadow()
// Renders the shadow volume geometries
//-----------------------------------------------------------------------------
void CShadow::RenderShadow()
{
  // set the vertex shader
  m_pd3dDevice->SetVertexShader( m_dwVertexShader );	
  m_pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, 
        sizeof( SHADOWMESHVERTEX ) );
  m_pd3dDevice->SetIndices( m_pIndexBuffer, 0);
  m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 
        m_dwVertexBufferSize, 0, (UINT)(m_dwIndexBufferSize/3 ) );
}


//-----------------------------------------------------------------------------
// SetShaderConstants()
// Sets the shader constants for use within the vertex program for infinite
// shadow volume extrusion using homogeneous coordinates.
//-----------------------------------------------------------------------------
void CShadow::SetShaderConstants( const D3DLIGHT8* pLight, 
                                  const D3DXMATRIX* matWorld, 
                                  const D3DXMATRIX* matView, 
                                  const D3DXMATRIX* matProj )
{
// Take note that for the homogeneous coordinate (w=0) to work in D3D, we have 
// to work in worldlight space (World*InvLight). The light position becomes 
// the origin before final transformation to clip space (light clip).
// This will allow vectors (w=0) to be rendered correctly, semi-infinite into
// the direction they point towards.

  D3DXMATRIX matInvWorld;
  D3DXMATRIX matLight, matInvLight;
  D3DXMATRIX matWorldInvLight;
  D3DXMATRIX matLightClip;

  // Considering only point light source, hence orientation is not needed, 
  // light space transformation matrix thus contain only translation
  D3DXMatrixTranslation( &matLight, pLight->Position.x, pLight->Position.y, 
        pLight->Position.z );
  D3DXMatrixIdentity( &matLightClip );
  D3DXMatrixMultiply( &matLightClip, &matLight, matView );
  D3DXMatrixMultiply( &matLightClip, &matLightClip, matProj );

  D3DXMatrixInverse( &matInvWorld, NULL, matWorld );

  D3DXMatrixTranslation( &matInvLight, -pLight->Position.x, -pLight->Position.y, 
        -pLight->Position.z );
  D3DXMatrixMultiply( &matWorldInvLight, matWorld, &matInvLight );

  D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 0.0f );
  
  // Yellowish green hue for drawing shadow volume if needed
  D3DXVECTOR4 vColor( 0.3f, 0.4f, 0.2f, 0.0f );
  
  D3DXVECTOR4 vWorldLightPos, vObjectLightPos;
  vWorldLightPos = D3DXVECTOR4( pLight->Position.x, pLight->Position.y, 
      pLight->Position.z, 1.0f );

  // Transform light pos from world space to object space
  // Light ray vector is computed in object space to avoid transforming the 
  // face normal
  D3DXVec4Transform( &vObjectLightPos, &vWorldLightPos, &matInvWorld );

  // Set the shader constants
  m_pd3dDevice->SetVertexShaderConstant( 0, &vObjectLightPos, 1 );
  m_pd3dDevice->SetVertexShaderConstant( 1, &vConst, 1 );
  m_pd3dDevice->SetVertexShaderConstant( 2, &matLightClip, 4 );
  m_pd3dDevice->SetVertexShaderConstant( 6, matWorldInvLight, 4 );
  m_pd3dDevice->SetVertexShaderConstant( 10, &vColor, 1 );
}


//-----------------------------------------------------------------------------
// InitDeviceObjects()
// Declares the vertex shader and creates one from a .vso binary file
//-----------------------------------------------------------------------------
HRESULT CShadow::InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice )
{
  // vertex shader declaration
  DWORD dwDecl[] =
  {
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3 ),	// Vertex position in input reg 0
    D3DVSD_REG(1, D3DVSDT_FLOAT3 ),	// Face normal in input reg 1
    D3DVSD_END()
  };

  // loads a *.vso binary file, already compiled with NVASM and creates a 
  // vertex shader
  if ( FAILED( CreateVSFromCompiledFile( pd3dDevice, dwDecl, 
        "Shaders/VertexExtrusion.vso", &m_dwVertexShader ) ) )
    return E_FAIL;

  return S_OK;
}


//-----------------------------------------------------------------------------
// InvalidataDeviceObjects()
// Releases the internal index and vertex buffers
//-----------------------------------------------------------------------------
HRESULT CShadow::InvalidateDeviceObjects()
{
  SAFE_RELEASE( m_pIndexBuffer );
  SAFE_RELEASE( m_pVertexBuffer );
  return S_OK;
}


//-----------------------------------------------------------------------------
// RestoreDeviceObjects()
// Recreates the shadow volume geometries from the source mesh by calling
// the private Create() function
//-----------------------------------------------------------------------------
HRESULT CShadow::RestoreDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice, 
                                        LPD3DXMESH pMesh )
{
  return Create( pd3dDevice, pMesh );
}


//-----------------------------------------------------------------------------
// Destory()
// Destroys the vertex shader and release everything else
//-----------------------------------------------------------------------------
HRESULT CShadow::Destroy()
{
	if ( m_dwVertexShader != 0xffffffff )
	{
		m_pd3dDevice->DeleteVertexShader( m_dwVertexShader );
		m_dwVertexShader = 0xffffffff;
	}

  InvalidateDeviceObjects();
  return S_OK;
}


//-----------------------------------------------------------------------------
// Create()
// Creates the shadow volume geometries suitable for use in a vertex shader
// The source mesh is traversed fully and degenerate quads are inserted into
// shared edges.
//-----------------------------------------------------------------------------
HRESULT CShadow::Create( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH pMesh )
{
  BASEMESHVERTEX*   pVertices;
  WORD*             pIndices;
  EdgeMap           edgeMap;
  EdgeMap::iterator edgeMapIter;
  DWORD             dwNumSavedEdges = 0L; // No of coplanar edges optimized
  DWORD             dwI;

  ShadowVertices    shadowVerticesVector;
  ShadowIndices     shadowIndicesVector;

  // Copy pointer to d3d device
  if (pd3dDevice)
    m_pd3dDevice = pd3dDevice;
  else
    return E_FAIL;

  // Lock the geometry buffers
  pMesh->LockVertexBuffer( D3DLOCK_READONLY, (BYTE**)&pVertices );
  pMesh->LockIndexBuffer( D3DLOCK_READONLY, (BYTE**)&pIndices );
  DWORD dwNumSourceVertices     = pMesh->GetNumVertices();
  DWORD dwNumFaces              = pMesh->GetNumFaces();
  
  // Step thru all faces
  for ( DWORD faceIndex=0; faceIndex<dwNumFaces; faceIndex++ )
  {
    WORD wIndex0 = pIndices[3*faceIndex+0];
    WORD wIndex1 = pIndices[3*faceIndex+1];
    WORD wIndex2 = pIndices[3*faceIndex+2];

    D3DXVECTOR3 v0 = pVertices[wIndex0].p;
    D3DXVECTOR3 v1 = pVertices[wIndex1].p;
    D3DXVECTOR3 v2 = pVertices[wIndex2].p;

    // Compute face normal
    D3DXVECTOR3 faceNormal;
    D3DXVec3Cross( &faceNormal, &( v2 - v0 ), &( v1 - v0 ) );
    D3DXVec3Normalize( &faceNormal, &faceNormal );

    // Create shadow mesh vertices for current face
    SHADOWMESHVERTEX shadowVertices[3];
    for ( DWORD vertIndex = 0; vertIndex < 3; vertIndex++ )
    {
      shadowVertices[vertIndex].p = 
            pVertices[pIndices[3*faceIndex+vertIndex]].p;
      // There was a time when a typo (.p instead of .n) caused me 6 hours...
      shadowVertices[vertIndex].n = faceNormal;
    }

    // Search edgeMap for a possible partner edge to the current edge 
    // e.g. Equivalent positions
    EdgePositions edgePositions[3];
    EdgeData sharedEdgesData[3];
    EdgePositions edgeToFind;
    BOOL edgeFound[3];
    BOOL quadRequired[3];
    for ( DWORD edgeIndex = 0; edgeIndex < 3; edgeIndex++ )
    {
      edgePositions[edgeIndex].p0 = shadowVertices[edgeIndex].p;
      edgePositions[edgeIndex].p1 = shadowVertices[( edgeIndex + 1 ) % 3].p;

      // Find shared edge with reverse vertices
      edgeToFind.p0 = edgePositions[edgeIndex].p1;
      edgeToFind.p1 = edgePositions[edgeIndex].p0;
      edgeMapIter = edgeMap.find( edgeToFind );

      // Did we find it?
      if ( edgeMapIter != edgeMap.end() ) // Found!
      {
        edgeFound[edgeIndex] = TRUE;
        sharedEdgesData[edgeIndex] = edgeMapIter->second;
        edgeMap.erase( edgeMapIter );

        // We can optimize away coplanar shared edges. e.g. their normals are 
        // almost parallel
        if ( ( D3DXVec3Dot( &faceNormal, &sharedEdgesData[edgeIndex].n0 ) ) < 
              MERGE_COS_ANGLE_PROXIMITY )
        {
          quadRequired[edgeIndex] = TRUE;
        }
        else
        {
          quadRequired[edgeIndex] = FALSE;
          dwNumSavedEdges++;
        }
      }
      else  // New
        edgeFound[edgeIndex] = FALSE;
    }

    WORD index[3]; // Selected index of vertices
    for ( DWORD vertIndex=0; vertIndex<3; vertIndex++ )
    {
      // Vertex creation
      DWORD edgeIndex0  = vertIndex;              // 0, 1, 2
      DWORD edgeIndex1  = ( vertIndex + 2 ) % 3;  // 2, 0, 1
      // We check whether the current edge had been optimized away
      // If it is, we just need to copy the index from the shared edge since 
      // the current vertex is gone
      if ( ( edgeFound[edgeIndex0] ) && ( !quadRequired[edgeIndex0] ) )
      {
        index[vertIndex] = sharedEdgesData[edgeIndex0].i1;
      }
      else if ( ( edgeFound[edgeIndex1] ) && ( !quadRequired[edgeIndex1] ) )
      {
        index[vertIndex] = sharedEdgesData[edgeIndex1].i0;
      }
      else  // No optimization, new vertex OR degenerate quad required
      {
        // Current size = index
        index[vertIndex] = (WORD)shadowVerticesVector.size();
        // Store current vertex
        shadowVerticesVector.push_back( shadowVertices[vertIndex] );
      }

      // Index creation
      shadowIndicesVector.push_back( index[vertIndex] );
    }

    // Time to create the degenerate quads
    for ( DWORD edgeIndex = 0; edgeIndex < 3; edgeIndex++ )
    {
      EdgeData edgeData;
      edgeData.i0   = index[edgeIndex];             // 0, 1, 2
      edgeData.i1   = index[( edgeIndex + 1 ) % 3]; // 1, 2, 0
      edgeData.n0   = faceNormal;
      if ( edgeFound[edgeIndex] ) // Shared edge found
      {
        // Handle 3 cases: Insert full quad, insert left side tri or insert 
        // right side tri
        // If current edge had NOT been optimized away
        if ( ( sharedEdgesData[edgeIndex].i0 != edgeData.i1 ) && 
              ( sharedEdgesData[edgeIndex].i1 != edgeData.i0 ) )
        {
          shadowIndicesVector.push_back( edgeData.i1 );
          shadowIndicesVector.push_back( edgeData.i0 );
          shadowIndicesVector.push_back( sharedEdgesData[edgeIndex].i0 );
          shadowIndicesVector.push_back( sharedEdgesData[edgeIndex].i1 );
          shadowIndicesVector.push_back( sharedEdgesData[edgeIndex].i0 );
          shadowIndicesVector.push_back( edgeData.i0 );
        }
        else if ( sharedEdgesData[edgeIndex].i1 != edgeData.i0 )
        {
          shadowIndicesVector.push_back( edgeData.i1 );
          shadowIndicesVector.push_back( edgeData.i0 );
          shadowIndicesVector.push_back( sharedEdgesData[edgeIndex].i1 );
        }
        else if( sharedEdgesData[edgeIndex].i0 != edgeData.i1 )
        {
          shadowIndicesVector.push_back( edgeData.i1 );
          shadowIndicesVector.push_back( edgeData.i0 );
          shadowIndicesVector.push_back( sharedEdgesData[edgeIndex].i0 );
        }
        // All other cases meant that the current edge had been optimized away 
        // and only the shared edge is left. The shared edge is left alone with 
        // additional degenerate triangles added
      }
      else  // New edge
      {
        edgeMap[edgePositions[edgeIndex]] = edgeData;
      }
    }
  }

  // Gather some stats
  m_dwVertexBufferSize  = (DWORD)shadowVerticesVector.size();
  m_dwIndexBufferSize   = (DWORD)shadowIndicesVector.size();
  
  // ********** Create vertex and index buffers **********
  // Create index buffer
  if ( FAILED( m_pd3dDevice->CreateIndexBuffer( m_dwIndexBufferSize * 
        sizeof( WORD ), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, 
            &m_pIndexBuffer ) ) )
  {
    OutputDebugString( "Index buffer creation failed!\n" );
    return E_FAIL;
  }

  // Lock index buffer
  WORD* pLockedIndexBuffer;
  m_pIndexBuffer->Lock( 0, m_dwIndexBufferSize * sizeof( WORD ), 
        (BYTE**)&pLockedIndexBuffer, 0 );

  // Fill locked index buffer with data
  IndexIter indexIter;
  for ( indexIter = shadowIndicesVector.begin(), dwI = 0; 
        indexIter != shadowIndicesVector.end(); ++indexIter, dwI++ )
  {
    pLockedIndexBuffer[dwI] = (WORD)*indexIter;
  }
  m_pIndexBuffer->Unlock();

  // Create vertex buffer
  if ( FAILED( m_pd3dDevice->CreateVertexBuffer( m_dwVertexBufferSize * 
        sizeof( SHADOWMESHVERTEX ), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, 
            &m_pVertexBuffer ) ) )
  {
    OutputDebugString( "Vertex buffer creation failed!\n" );
    return E_FAIL;
  }

  // Lock vertex buffer
  SHADOWMESHVERTEX* pLockedVertexBuffer;
  VertexIter vertexIter;
  m_pVertexBuffer->Lock( 0, m_dwVertexBufferSize * sizeof( SHADOWMESHVERTEX ), 
        (BYTE**)&pLockedVertexBuffer, 0 );
  // Fill locked vertex buffer with data
  for ( vertexIter=shadowVerticesVector.begin(), dwI=0; 
        vertexIter != shadowVerticesVector.end(); ++vertexIter, dwI++ )
  {
    pLockedVertexBuffer[dwI] = *vertexIter;
  }
  m_pVertexBuffer->Unlock();


#ifdef _DEBUG
  // Print statistics to a text file
  FILE* fp;
  fp = fopen( "PreprocessLog.txt", "a+t" );
  fprintf( fp, "Source vertices count   : %d\n", dwNumSourceVertices );
  fprintf( fp, "Final vertices count    : %d\n", m_dwVertexBufferSize );
  fprintf( fp, "No. of collapsed edges  : %d\n", dwNumSavedEdges );
  DebugPrintf( "Collapsed %d edges because they won't often be silhouette edges\n", 
        dwNumSavedEdges );
  DebugPrintf( "%d source vertices expanded to %d shadow vertices\n", 
        dwNumSourceVertices, m_dwVertexBufferSize );

  if ( edgeMap.size() > 0 )
  {
    fprintf( fp, "Preprocessing warning: Mesh is not a closed volume\n" );
    fprintf( fp, "No. of unmatched edges : %d\n", edgeMap.size() );
    DebugPrintf( "Warning: geometry is not a closed volume. %d edges left!!!\n", 
          edgeMap.size() );
    for (edgeMapIter = edgeMap.begin(), dwI = 1; edgeMapIter != edgeMap.end(); 
          ++edgeMapIter, dwI++ )
    {
      EdgePositions edgePos;
      EdgeData edgeData;
      edgePos   = edgeMapIter->first;
      edgeData  = edgeMapIter->second;
      fprintf(fp, "  <%d>\t(%f, %f, %f) (%f, %f, %f) I(%d, %d) N(%f, %f, %f)\n", 
            dwI, edgePos.p0.x, edgePos.p0.y, edgePos.p0.z, 
                edgePos.p1.x, edgePos.p1.y, edgePos.p1.z, 
                    edgeData.i0, edgeData.i1, 
                        edgeData.n0.x, edgeData.n0.y, edgeData.n0.z );
    }
    fprintf( fp, "Preprocessing completed with errors.\n\n" );
  }
  else
    fprintf( fp, "Preprocessing completed with no errors.\n\n" );
  fclose( fp );
#endif // _DEBUG

  return S_OK;
}


//-----------------------------------------------------------------------------
// CreateVSFromBinFile()
// Loads a binary *.vso file that was compiled by NVASM and creates a vertex 
// shader
//-----------------------------------------------------------------------------
HRESULT CShadow::CreateVSFromCompiledFile( IDirect3DDevice8* m_pd3dDevice, 
                                          DWORD* dwDeclaration, 
                                          TCHAR* strVSPath, 
                                          DWORD* m_dwVS )
{
	char szBuffer[128];		    // debug output
	DWORD*	dwpVS;			      // pointer to address space of the calling process
	HANDLE hFile, hMap;		    // handle file and handle mapped file
    TCHAR tempVSPath[512];	// temporary file path
	HRESULT hr;				        // error 

  if( FAILED( hr = DXUtil_FindMediaFile( tempVSPath, strVSPath ) ) )
    return D3DAPPERR_MEDIANOTFOUND;
	
	hFile = CreateFile( tempVSPath, GENERIC_READ,0,0,OPEN_EXISTING,	
        FILE_ATTRIBUTE_NORMAL,0);

	if( hFile != INVALID_HANDLE_VALUE ) 
	{
		if( GetFileSize( hFile,0 ) > 0 ) 
			hMap = CreateFileMapping( hFile, 0, PAGE_READONLY, 0, 0, 0 );
		else
		{
			CloseHandle( hFile );
			return E_FAIL;		
		}
	}	
	else
		return E_FAIL;	
	
	// maps a view of a file into the address space of the calling process
	dwpVS = (DWORD *)MapViewOfFile( hMap, FILE_MAP_READ, 0, 0, 0 );
		
	// Create the vertex shader
	hr = m_pd3dDevice->CreateVertexShader( dwDeclaration, dwpVS, m_dwVS, 0 );
	if ( FAILED(hr) )
	{
		OutputDebugString( "Failed to create Vertex Shader, errors:\n" );
		D3DXGetErrorStringA( hr, szBuffer, sizeof( szBuffer ) );
		OutputDebugString( szBuffer );
		OutputDebugString( "\n" );
	  return hr;
	}
	
	UnmapViewOfFile( dwpVS );
	CloseHandle( hMap );
	CloseHandle( hFile );
	
  return S_OK;
}
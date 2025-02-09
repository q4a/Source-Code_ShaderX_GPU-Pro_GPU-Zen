/*********************************************************************NVMH4****
Path:  SDK\DEMOS\common\src\D3DCommon
File:  WingedEdgeData.cpp

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:

//gj
//@@@
  I'm not happy with how this class interacts with the SimpleObject.  Calling
Initialize(..) again will mess up the data.  Need a better way.


******************************************************************************/


#include <shared/NV_Common.h>
#include <shared/NV_Error.h>

#include <SimpleObject.h>
#include <WingedEdgeData.h>

#include <vector>





///////////////////////////////////////////////////////////

WingedEdgeData::WingedEdgeData()
{
	::SimpleObject();


	SetToConstructionValues();

}


WingedEdgeData::~WingedEdgeData()
{

	Free();

}


void WingedEdgeData::SetToConstructionValues()
{

	m_bWingedEdgeDataBuilt				= false;
	m_pCollapsedVertexSet				= NULL;
	m_wTriVertexIndicesToCollapsedSet	= NULL;
	m_pTriNeighbors						= NULL;
	m_pWingedEdgeMarkersObject			= NULL;

	m_wNumInCollapsedSet				= 0;
	m_pWingedEdgeMarkersObject			= NULL;

	m_fPositionSqrdThreshold = 1e-5f;
	m_fPositionSqrdThreshold = 1e-3f;

	m_bMarkWeldedVertices				= false;	// change vertex color when two are welded
	m_bOpenHull							= false;
	m_bSoloEdge							= false;

}


///////////////////////////////////////////////////////////

HRESULT WingedEdgeData::FreeTemporaryData()
{
	SAFE_ARRAY_DELETE( m_pCollapsedVertexSet );
	SAFE_ARRAY_DELETE( m_wTriVertexIndicesToCollapsedSet );

	return( S_OK );
}


HRESULT WingedEdgeData::Free()
{

	SimpleObject::UnAllocate();

	FreeTemporaryData();
	SAFE_ARRAY_DELETE( m_pTriNeighbors );

	SAFE_DELETE( m_pWingedEdgeMarkersObject );

	SetToConstructionValues();

	return( S_OK );
}




HRESULT WingedEdgeData::AllocateData( WORD numverts, WORD numtris )
{

	SAFE_ARRAY_DELETE( m_pCollapsedVertexSet );
	SAFE_ARRAY_DELETE( m_pTriNeighbors );
	SAFE_ARRAY_DELETE( m_wTriVertexIndicesToCollapsedSet );

	m_pCollapsedVertexSet = new CollapsedSetEntry[ numverts ];
	FAIL_IF_NULL( m_pCollapsedVertexSet );
//	memset( m_pCollapsedVertexSet, 0, sizeof( CollapsedSetEntry ) * numverts );

	m_pTriNeighbors = new NeighborTriIndices[ numtris ];
	FAIL_IF_NULL( m_pTriNeighbors );
//	memset( m_pTriNeighbors, 0, sizeof( NeighborTriIndices ) * numtris );

	int n;
	for(n=0; n < numtris; n++ )
	{
		m_pTriNeighbors[n].welded1 = NOT_WELDED;
		m_pTriNeighbors[n].welded2 = NOT_WELDED;
		m_pTriNeighbors[n].welded3 = NOT_WELDED;

		m_pTriNeighbors[n].ind1   = NO_NEIGHBOR;
		m_pTriNeighbors[n].ind2   = NO_NEIGHBOR;
		m_pTriNeighbors[n].ind3   = NO_NEIGHBOR;
	}

	m_wTriVertexIndicesToCollapsedSet = new WORD[ numtris * 3 ];
	FAIL_IF_NULL( m_wTriVertexIndicesToCollapsedSet );
//	memset( m_wTriVertexIndicesToCollapsedSet, 0, sizeof(WORD) * numtris * 3 );

	return( S_OK );
}


void WingedEdgeData::ReverseWinding()
{
	if( m_bWingedEdgeDataBuilt == true )
	{
		// Have to re-compute winged edge data or
		// re-order the neighbor triangle data

		assert( false );
	}
	else
	{
		SimpleObject::ReverseWinding();
	}	
}


bool	WingedEdgeData::IsWingedEdgeDataBuilt()
{
	return( m_bWingedEdgeDataBuilt );
}


void	WingedEdgeData::Warning_OpenHull()
{
	// warning message about having an open hull

	if( m_bOpenHull == false )
	{
		m_bOpenHull = true;
		FDebug("Warning - object is an open hull!\n");
	}	
}


void	WingedEdgeData::Warning_SoloEdge()
{
	// An edge has < 2 tris shared

	if( m_bSoloEdge == false )
	{
		m_bSoloEdge = true;
		Warning_SoloEdge();
	}	
}


SimpleObject  ** WingedEdgeData::GenerateWingedEdgeMarkers()
{
	// Generate geometry to represent winged edge connections
	// This is to visualize them to make sure they are computed correctly

	if( m_pTriNeighbors == NULL )
	{
		BuildWingedEdgeData();
	}

	assert( m_pTriNeighbors != NULL );

	int i;
	int n;
	WORD			base_tri;
	D3DXVECTOR3		base_tri_center, tri_center;

	SAFE_DELETE( m_pWingedEdgeMarkersObject );

	m_pWingedEdgeMarkersObject = new SimpleObject;
	assert( m_pWingedEdgeMarkersObject != NULL );

	assert( m_wNumInd * 3 > m_wNumInd );
	assert( m_wNumInd * 3 > m_wNumInd );

	m_pWingedEdgeMarkersObject->Allocate( m_wNumInd * 3, m_wNumInd * 3 );

	SimpleObject * pObj = m_pWingedEdgeMarkersObject;	// convenient shorter name

	pObj->m_bIsValid = true;
	pObj->m_PrimType = D3DPT_TRIANGLELIST;


	WORD num_vert, num_ind;
	num_vert = num_ind = 0;

	D3DXVECTOR3  vec, tri_nrm, perpendicular;
	float		 mag;

	V_WORD	neighbor;

	for( i=0; i < m_wNumInd / 3; i++ )
	{
		base_tri = i;

		neighbor.clear();

		if( m_pTriNeighbors[i].ind1 != NO_NEIGHBOR )
			neighbor.push_back( m_pTriNeighbors[i].ind1 );
		if( m_pTriNeighbors[i].ind2 != NO_NEIGHBOR )
			neighbor.push_back( m_pTriNeighbors[i].ind2 );
		if( m_pTriNeighbors[i].ind3 != NO_NEIGHBOR )
			neighbor.push_back( m_pTriNeighbors[i].ind3 );

		assert( neighbor.size() < 4 );


		for( n=0; n < neighbor.size(); n++ )
		{
			assert( neighbor[n] * 3 < m_wNumInd );

			ComputeTriCenter( & base_tri_center, base_tri );
			ComputeTriCenter( & tri_center, neighbor[n] );

			// make geometry along vector from center to center of
			//  winged-edge connected triangles

			vec = tri_center - base_tri_center;
			vec *= 0.25f;

			pObj->m_pVertices[ num_vert++ ].pos = base_tri_center + vec;		// 1/4 the way there
			pObj->m_pVertices[ num_vert++ ].pos = base_tri_center + vec;
			pObj->m_pVertices[ num_vert++ ].pos = base_tri_center + 3 * vec;	// 3/4 the way there
			assert( num_vert <= m_wNumInd * 3 );

			pObj->m_pIndices[ num_ind++ ] = num_vert - 3;
			pObj->m_pIndices[ num_ind++ ] = num_vert - 2;
			pObj->m_pIndices[ num_ind++ ] = num_vert - 1;
			assert( num_ind <= m_wNumInd * 3 );

			tri_nrm = m_pVertices[ m_pIndices[ base_tri*3] ].nrm;

			D3DXVec3Cross( & perpendicular, & vec, & tri_nrm  );
			D3DXVec3Normalize( &perpendicular, &perpendicular );

			mag = D3DXVec3Dot( & vec, & vec );
			mag = (float) sqrt( fabs( mag ) );

			// Offset the 2nd vertex above by the perpendicular from center to center and the 
			//   triangle 1st vertex's normal vector

			pObj->m_pVertices[ num_vert - 2 ].pos += 0.2f * mag * perpendicular;

		}
	}

	m_pWingedEdgeMarkersObject->m_wNumInd = num_ind;
	m_pWingedEdgeMarkersObject->m_wNumVerts = num_vert;


	FDebug("Winged edge markers has %d connections -- Two for pair of adjacent tris\n", m_pWingedEdgeMarkersObject->m_wNumInd / 3 );


	return( & m_pWingedEdgeMarkersObject );
}


void	WingedEdgeData::BuildNonDegeneratePositions()
{
	// Does a dumb search against all verts
	//gj ** Redo to use spatial partitioning so that it's faster

	// Creates m_pCollapsedVertexSet array 
	// This is used to create vertex data where no vertices overlap - 
	//  so that in creating winged edge data, the data can span holes in 
	//  the model where vertices are duplicated.

	assert( m_wNumVerts   > 0 );
	assert( m_pVertices != NULL );

	assert( m_pCollapsedVertexSet != NULL );

	WORD i, n;

	float		distsq;
	D3DXVECTOR3 delta;

	// Set indices to collapsed set to the num of vertices
	//  as a marker for which verts have been set.

	for( i=0; i < m_wNumVerts; i++ )
	{
		m_pCollapsedVertexSet[i].IndexToOriginalVerts = m_wNumVerts;
		m_pCollapsedVertexSet[i].CollapsedSetIndex    = m_wNumVerts;
	}


	m_wNumInCollapsedSet = 0;
	
	for( i=0; i < m_wNumVerts; i++ )
	{
		if( m_pCollapsedVertexSet[i].IndexToOriginalVerts == m_wNumVerts )
		{
			// We haven't replaced the index, so do the search

			m_pCollapsedVertexSet[i].IndexToOriginalVerts = i;

			m_pCollapsedVertexSet[i].CollapsedSetIndex = m_wNumInCollapsedSet;

			m_wNumInCollapsedSet++;

			for( n=i+1; n < m_wNumVerts; n++ )
			{
				delta = m_pVertices[i].pos - m_pVertices[n].pos;
				distsq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

				if( distsq < m_fPositionSqrdThreshold )
				{
					m_pCollapsedVertexSet[ n ].IndexToOriginalVerts = i;
					m_pCollapsedVertexSet[ n ].CollapsedSetIndex = m_pCollapsedVertexSet[i].CollapsedSetIndex;
				}
			}
		}
	}
}


void	WingedEdgeData::BuildNonEdgeDegenerateTris()
{
	assert( m_pCollapsedVertexSet != NULL );

	int i;
	WORD new_vert_index;

	for( i=0; i < m_wNumInd; i++ )
	{
		assert( m_pIndices[i] < m_wNumVerts );

		new_vert_index = m_pCollapsedVertexSet[ m_pIndices[i] ].IndexToOriginalVerts;

		assert( new_vert_index < m_wNumVerts );

		m_wTriVertexIndicesToCollapsedSet[i] = new_vert_index;
	}
}


//@@@ gj make the first two args const!

void WingedEdgeData::CompareTris(  std::vector< WORD > & set1,  std::vector< WORD > & set2, 
									  std::vector< WORD > & result )
{
	// Compares two sets of WORDs and returns a set which is the
	//  intersection

	int i,j;

	for( i=0; i < set1.size(); i++ )
	{
		for( j=0; j < set2.size(); j++ )
		{
			if( set1[i] == set2[j] )
			{
				result.push_back( set1[i] );
			}
		}
	}

	if( result.size() > 2 )
	{
		FDebug("*** An edge has more than 2 tris attached!\n");
		for( i=0; i < result.size(); i++ )
		{
			FDebug("\tTri[%d]", result.at( i ) );    
		}
		FDebug("\n\n");
	}

	if( result.size() < 2 )
	{
		Warning_SoloEdge();
	}
}




HRESULT  WingedEdgeData::BuildWingedEdgeData()
{

	// This is a bit of a complex calculation.

	// 1)  Make winged-edge model database
	//	  a) Collapse all degenerate vertices based
	//		 on position only.  Orig vertices not altered
	//		i) Each old vertex holds an index to a 
	//         new vertex array, nv[], with the collapsed
	//         vertices
	//    b) Create new triangle indices, ni[], which are 
	//         the old indices re-mapped to the use the new
	//          collapsed vertices, nv[], instead.
	//    c) Search the new indices, ni[] for adjacent edges
	//         and for each triangle, create three indices
	//         to it's edge neighbor triangles.

	HRESULT hr;

	assert( m_wNumInd     > 0 );
	assert( m_wNumVerts   > 0 );
	assert( m_pIndices != NULL );
	assert( m_pVertices != NULL );


	hr = AllocateData( m_wNumVerts, m_wNumInd / 3 );
	ASSERT_IF_FAILED( hr );


	// collapse degenerate positions
	BuildNonDegeneratePositions();
	
	// Create new tri indices that use the non-degenerate positions
	BuildNonEdgeDegenerateTris();

	// For each vertex in the collapsed set, allocate a vector of WORDS.  
	//  These WORDs will hold the triangle indices for all tris to which
	//  that vertex belongs.


	WORD i;
	WORD triangle;
	WORD vert_index;
	WORD collapsed_ind;


	// pVertexToTriangle is an array of STL arrays.  Each STL array
	//  contains all the indices of trianlges to which that vertex
	//  belongs,
	// ie  pVertexToTriangle[ 23 ] = { 2,3, 8, 10 } means vertex
	//  23 is used by triangles 2, 3, 8, and 10.  

	V_WORD * pVertexToTriangle = new V_WORD[ m_wNumInCollapsedSet ];
	assert( pVertexToTriangle != NULL );
		

	for( i=0; i < m_wNumInCollapsedSet; i++ )
	{
		// reserve 6 spots, reasoning that each vertex belongs to about
		//  6 tris on average.
		pVertexToTriangle[ i ].reserve( 6 );
	}

	/////////////////////////////////////////////////////////
	// Build list of which triangles each vertex belongs to

	for( i=0; i < m_wNumInd; i++ )
	{
		triangle = i / 3;		

		vert_index = m_wTriVertexIndicesToCollapsedSet[i];

		assert( vert_index < m_wNumVerts );

		collapsed_ind = m_pCollapsedVertexSet[ vert_index ].CollapsedSetIndex;

		assert( collapsed_ind < m_wNumInCollapsedSet );

		pVertexToTriangle[ collapsed_ind ].push_back( triangle );
	}


	// For each vertex of each triangle, retrieve the triangles
	//  that the vertex belongs to and compare them to find
	//  trianlges which share the same edge

	WORD v0, v1, v2;

	V_WORD  shared_tris;
	WORD	neighbor_tri;

	D3DXVECTOR3 d0, d1;
	D3DXVECTOR3 * pNrm;

	for( triangle = 0; triangle < m_wNumInd / 3; triangle ++ )
	{
		// indices to original verts as remapped after detecting 
		//  verts at same position

		v0 = m_wTriVertexIndicesToCollapsedSet[ triangle*3 + 0 ];
		v1 = m_wTriVertexIndicesToCollapsedSet[ triangle*3 + 1 ];
		v2 = m_wTriVertexIndicesToCollapsedSet[ triangle*3 + 2 ];

		// map those verts to their position in the collapsed set
		//  so we can get what triangles they belong to

		assert( m_pCollapsedVertexSet[v0].IndexToOriginalVerts == v0 );
		assert( m_pCollapsedVertexSet[v1].IndexToOriginalVerts == v1 );
		assert( m_pCollapsedVertexSet[v2].IndexToOriginalVerts == v2 );

		v0 = m_pCollapsedVertexSet[ v0 ].CollapsedSetIndex;
		v1 = m_pCollapsedVertexSet[ v1 ].CollapsedSetIndex;
		v2 = m_pCollapsedVertexSet[ v2 ].CollapsedSetIndex;


		//////////////////////////////////////////
		// Compare the tris that v0 and v1 belong to
		// On return from CompareTris(), shared_tris will contain
		//  indices to all triangles which use both of the two
		//	vertices.
		//
		// No early exit, as this will check for "radiator fan"
		//  structures where > 2 triangles share the same edge.
		// It will assert if it finds such a structure

		shared_tris.clear();
		CompareTris( pVertexToTriangle[v0], pVertexToTriangle[v1], shared_tris );


		// Each edge should have two tris to make a closed hull.
		// If not, mark the object as such
		if( shared_tris.size() != 2 )
		{
			m_bOpenHull = true;			
			
			if( shared_tris.size() > 2 )
				FDebug("Warning - object has > 2 tris per one edge!\n");	
			else
				Warning_OpenHull();

			neighbor_tri = NO_NEIGHBOR;
		}
		else
		{
			if( shared_tris[0] != triangle )
				neighbor_tri = shared_tris[0];
			else
				neighbor_tri = shared_tris[1];
	
			assert( neighbor_tri < m_wNumInd / 3 );
		}


		// Write the tri neighbor to the tri's neighbor information
		m_pTriNeighbors[ triangle ].ind1 = neighbor_tri;


		//////////////////////////////////////////
		// Compare tris belonging to v1 and v2
		shared_tris.clear();
		CompareTris( pVertexToTriangle[v1], pVertexToTriangle[v2], shared_tris );

		if( shared_tris.size() != 2 )
		{
			m_bOpenHull = true;			
			
			if( shared_tris.size() > 2 )
				FDebug("Warning - object has > 2 tris per one edge!\n");	
			else
				Warning_OpenHull();

			neighbor_tri = NO_NEIGHBOR;
		}
		else
		{
			if( shared_tris[0] != triangle )
				neighbor_tri = shared_tris[0];
			else
				neighbor_tri = shared_tris[1];
	
			assert( neighbor_tri < m_wNumInd / 3 );
		}


		// Write the tri neighbor to the tri's neighbor information
		m_pTriNeighbors[ triangle ].ind2 = neighbor_tri;

		//////////////////////////////////////////
		// Compare tris belonging to v2 and v0

		shared_tris.clear();
		CompareTris( pVertexToTriangle[v2], pVertexToTriangle[v0], shared_tris );

		if( shared_tris.size() != 2 )
		{
			m_bOpenHull = true;			
			
			if( shared_tris.size() > 2 )
				FDebug("Warning - object has > 2 tris per one edge!\n");	
			else
				Warning_OpenHull();

			neighbor_tri = NO_NEIGHBOR;
		}
		else
		{
			if( shared_tris[0] != triangle )
				neighbor_tri = shared_tris[0];
			else
				neighbor_tri = shared_tris[1];
	
			assert( neighbor_tri < m_wNumInd / 3 );
		}


		// Write the tri neighbor to the tri's neighbor information
		m_pTriNeighbors[ triangle ].ind3 = neighbor_tri;


		////////////////////////////////////////////////////
		// Calculate face normal

		d0 = m_pVertices[ m_pIndices[triangle*3+1]].pos - m_pVertices[ m_pIndices[triangle*3+0]].pos;
		d1 = m_pVertices[ m_pIndices[triangle*3+2]].pos - m_pVertices[ m_pIndices[triangle*3+0]].pos;

		pNrm = & ( m_pTriNeighbors[triangle].face_normal );

		D3DXVec3Cross( pNrm , &d1, &d0 );		// triangle normal
		D3DXVec3Normalize( pNrm, pNrm );

	}



	////////////////////////////////////////////////
	// Clean up

	for( i=0; i < m_wNumInCollapsedSet; i++ )
	{
		pVertexToTriangle[i].clear();
	}
	SAFE_ARRAY_DELETE( pVertexToTriangle );


	m_bWingedEdgeDataBuilt = true;


	return( S_OK );
}





void WingedEdgeData::WeldFacetedFaces( float threshold )
{
	// Weld faces (collapse degenerate vertices
	//  and edges) if the dot product of the
	//  face normals is greater than threshold.

	// Alter m_pVertices & m_pIndices to weld faces.
	// Winged-edge data will get built for you if not already.
	// For each triangle edge, test the face normals.
	//	Collapse degenerate vertices of the data where the face
	//  normals are similar.


#define WFFDEBUG	0
#define WFFDIAG		//

	WFFDIAG("WEO: num verts before Weld: %u\n", m_wNumVerts );



	
	if( m_bWingedEdgeDataBuilt == false )
	{
		BuildWingedEdgeData();
	}
	assert( m_bWingedEdgeDataBuilt == true );
	assert( m_pTriNeighbors != NULL );
	assert( m_pCollapsedVertexSet != NULL );


	WORD		tri, n;

	D3DXVECTOR3	base_nrm;
	float		dot;
	WORD		neighbor_tri;

	int			weld_count = 0;

	V_WORD		neighbor;
	V_WORD		edge;

	V_WORD		vert_indices;		// partialy collapsed set, so two tris
									//  using same vertex can both be welded
									//  into another tri.
									// (avoids cost of total search & replace
									//  for a vert index each time two verts
									//  are welded )

	// Begin with unity mapping:
	vert_indices.reserve( m_wNumVerts );
	for( n=0; n < m_wNumVerts; n++ )
	{
		vert_indices.push_back( n );
	}


	// for each triangle
	for( tri = 0; tri < m_wNumInd / 3; tri ++ )
	{
		base_nrm = m_pTriNeighbors[tri].face_normal;


		neighbor.clear();
		edge.clear();

		if( m_pTriNeighbors[tri].ind1 != NO_NEIGHBOR )
		{
			neighbor.push_back( m_pTriNeighbors[tri].ind1 );
			edge.push_back(0);
		}

		if( m_pTriNeighbors[tri].ind2 != NO_NEIGHBOR )
		{
			neighbor.push_back( m_pTriNeighbors[tri].ind2 );
			edge.push_back(1);
		}

		if( m_pTriNeighbors[tri].ind3 != NO_NEIGHBOR )
		{
			neighbor.push_back( m_pTriNeighbors[tri].ind3 );
			edge.push_back(2);
		}


		for( n=0; n < neighbor.size(); n++ )
		{
			dot = D3DXVec3Dot( & base_nrm, & m_pTriNeighbors[neighbor[n]].face_normal );

#if			WFFDEBUG 
				FDebug("\n");
#endif

			if( dot > threshold )
			{
				// Weld the triangles into a smooth pair by removing
				//  degenerate vertices on their shared edge
			
				// Vertices are not deleted here.  The 2nd tri is just
				//  re-indexed to refer to the other tri's vertices

				// Vertex normals are re-calculated when this is all done
				//  to reflect the new faceting/smoothing of the model

				// Does a brute force search from each triangle vertex to 
				//  each of the neighbor's vertices.  This makes it insensitive
				//  to the order in which vertices appear in the triangles.
				// This could be made faster by requiring that order to be fixed,
				//  so that the vertices to compare could be looked up directly 
				//  according to their indices.


				int				nv, basev;
				int				nvi, bvi;		// indices to vertices
												// nvi = neighbor vertex index
												// bvi = base triangle vertex index
				D3DXVECTOR3		delta;
				float			distsq;

				neighbor_tri	= neighbor[n];

				int				loc_weld_count;
				loc_weld_count = 0;
					

				for( nv = 0; nv < 3; nv++ )
				{
					nvi = m_pIndices[ neighbor_tri*3 + nv ];


					for( basev = 0; basev < 3; basev++ )
					{
						bvi = m_pIndices[ tri*3 + basev ];
						// Get true base vertex index incase this vert was previously
						//   welded
						bvi = vert_indices.at( bvi );


						delta = m_pVertices[nvi].pos - m_pVertices[bvi].pos;
						distsq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

						// Do any points in the base triangle have the same position
						//  as points in the neighbor triangle?



						if( distsq < m_fPositionSqrdThreshold )
						{
#if							WFFDEBUG 
								FDebug("vertices base: %d and neighbor %d match\n", bvi, nvi );
#endif
							
								
							// Update the vert we are replacing to point to the 
							//  vertex it was welded to:

							vert_indices.at( m_pIndices[ neighbor_tri * 3 + nv ] ) = bvi;

							// Change the tri to use the base vertex, id'd by bvi,
							//  thus effectively welding two verts together.

							m_pIndices[ neighbor_tri*3 + nv ] = bvi;
							
							// If you want to tag welded vertices by color
							if( m_bMarkWeldedVertices == true )
							{
								// Set color of welded vertex to something special
								m_pVertices[ bvi ].diffuse = ((DWORD)rand())*((DWORD)rand());
							}

							// Mark neighbor tri's edge as welded 
							switch( nv )
							{
							case 0:
								m_pTriNeighbors[neighbor_tri].welded1 = IS_WELDED;
								break;
							case 1:
								m_pTriNeighbors[neighbor_tri].welded2 = IS_WELDED;
								break;
							case 2:
								m_pTriNeighbors[neighbor_tri].welded3 = IS_WELDED;
								break;
							default:
								assert( false );
								break;
							}

							weld_count++;
							loc_weld_count++;

							// stop further looping through base vertices
							// and go back to searching for a counterpart for the 
							//  next neighboring triangle's vertex (inc nv)
							basev = 3;
						}
					}
				}

				if( loc_weld_count == 2 )
				{
					// a degenerate edge was collapsed (two vertices
					//  were collapsed) so mark the edge as welded
					
					//NVOPT
					// Update tri's welded info.  The neigh[n] structure's welded
					//  info will be updated when that tri is tested.  This duplicates
					//  some calcs unnecessarily.

					switch( edge[n] )
					{
					case 0:
						m_pTriNeighbors[tri].welded1 = IS_WELDED;
						break;
					case 1:
						m_pTriNeighbors[tri].welded2 = IS_WELDED;
						break;
					case 2:
						m_pTriNeighbors[tri].welded3 = IS_WELDED;
						break;
					default: 
						assert(false);
					}

				}
				else if( loc_weld_count < 2 )
				{
#if					WFFDEBUG 
					FDebug("\n");
					FDebug("T-Junction found while welding vertices!! weldcnt = %d\n", loc_weld_count);
					FDebug("  This should not exist in the winged-edge representation!\n");
#endif					
				}
				else
				{
					FDebug("\n");
					FDebug("Strange weld-count encountered -- weldcnt = %d\n", loc_weld_count);
				}
			}
		}
	}


	// Now we must make sure that all vertices which were collapsed are not
	//  refered to by the geometry.  This is simple, as we have the indices array	
	for( n=0; n < m_wNumInd; n++ )
	{
		m_pIndices[n] = vert_indices.at( m_pIndices[n] );
	}


	// At this point, no verts have been removed from the vertex array.
	//  Some vertices in the array may not be used anymore.
	//  We need to scan the indices and remove any verts which aren't 
	//  referenced by the indices.	
	RemoveUnusedVerts();


	if( weld_count > 0 )
	{
		// re-generate normals now that some verts have
		//  been welded into one.
		GenerateNormalsCCW();
	}


	WFFDIAG("WEO: num verts after Weld: %u\n", m_wNumVerts );

}



void WingedEdgeData::RemoveUnusedVerts()
{
	// scan indices array, find any vertices which are not used
	//  and remove them

	// The operation will invalidate m_pCollapsedVertexSet, which we could correct,
	// but better to just delete it.

	FreeTemporaryData();

	SimpleObject::RemoveUnusedVerts();	
}









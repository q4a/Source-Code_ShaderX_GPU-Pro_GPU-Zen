/*********************************************************************NVMH4****
Path:  SDK\DEMOS\common\src\D3DCommon
File:  WingedEdgeData.h

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
  Taking a different approach to winged-edge data than WingedEdgeData class.

  This class is not derived from a specific geometric object type with an implied
vertex format.  Instead, it accepts and processes the bare minimum topology 
information, which is vertex POSITION and triangle INDICES. 

zero-area triangle:
	Uses three unique vertices, but two or more vertices may occupy the same
	position in space, according to some threshold distance value.
degenerate triangle:
	Uses fewer than three unique vertices - ie vertex indices {0,0,1}

  It will ignore all zero-area and degenerate triangles.  Degenerate and zero-area
triangles are detected by applying a vertex-to-vertex distance test between the
three vertices in each triangle.  Those which fail are eliminated from the list
of triangles and are not part of the winged-edge topology.

  It's generaly better to do vertex-vertex distance tests for each triangle, and
not between every single vertex in the entire mesh.  Doing it per-triangle avoids
problems with mesh folding and 'hour-glass' pinching where two parts of a mesh
contact eachother but are not tesselated together.
	
  Removing a zero-area triangle (one which is not degenerate) means there will be
a 'gap' in the geometry.  The geometry will not be a closed hull, even though it
may appear closed and water-tight when rendered.  Such gaps need to be stitched
closed if using the model for an extruded shadow volume.  This stitching is 
performed by a separate class.


  This class may find problems with the source data and be unable to create the 
winged-edge topology.  These problems may include:

- A triangle edge with more than one neighbor triangle - ie. a 'fork' or
  'crossroads' intersection.  Each triangle should have only three adjacent 
  triangles.
 
- A triangle edge with no neighbor triangle.  This may be an open hole or T
  junction in the model.

** No attempt is made to correct these problems, though it is sometimes easy
to stitch open holes closed.  I hope to add a convenient visualization feature
to help identify and correct these problems.
  

//@@ NOTES:
if holes exist, then triangle-walking is not enough - when hole is found, have
to search for pos-degenerate vertices and consider their triangles

  so - if hole exists AND fold-over exists, things could get really messed up - 
  no way to resolve the fold-over nearest verts into which ones are the right ones




******************************************************************************/


#ifndef __NV_WINGEDEDGEDATA_H__
#define __NV_WINGEDEDGEDATA_H__



#include <NVD3DVers.h>

#pragma warning(disable: 4786)
#include <vector>

#include <SimpleObject.h>


//////////////////////////////////////////

typedef WORD		WELD_VALUE;

#define NOT_WELDED			0
#define IS_WELDED			1
#define NOT_WELDED_STITCHED	2

//////////////////////////////////////////
#define NO_NEIGHBOR		65535


/////////////////////////////////////////////////////////////////////





class WingedEdgeData : public SimpleObject
{
public:
	struct CollapsedSetEntry
	{
		// Data member for 1 vertex
		// Used to create alternate mapping into smaller set of vertices
		//  when finding winged-edge connections

		WORD	IndexToOriginalVerts;	// Index into m_pVertices for the data
										//  so two verts originaly degenerate in position
										//  can point to the same vertex in m_pVertices

		WORD	CollapsedSetIndex;		// index value (array position) of the vertex
										//  in the collapsed set of vertices.
										// This is used to build a list of which 
										//  triangles each vertex is a part of
	};

	struct NeighborTriIndices
	{
		WORD		ind1;		// Index to first neighbor triangle
								//  This neighbor tri should be on the side
								//  formed by the first and second vertices
								//  of the root triangle to which this struct
								//  belongs
								// Max value is m_wNumInd / 3 // 3 indices per tri
		WELD_VALUE	welded1;	// Is the edge between v0, v1 and ind1 triangle
								//  degenerate -- Are v0, v1 shared by the neighbor
								//  triangle?
		WORD		ind2;		// Neighbor adjacent to the v1, v2 points
		WELD_VALUE	welded2;

		WORD		ind3;		// Neighbor adjacent to v2, v0 points
		WELD_VALUE	welded3;

		D3DXVECTOR3	face_normal;
	};


private:

	float	m_fPositionSqrdThreshold;		// for determining which vertices
											//  are degenerate in position

	bool	m_bWingedEdgeDataBuilt;

	CollapsedSetEntry	* m_pCollapsedVertexSet;	// each entry maps 1 m_pVertices vertex
													// to a location in the collapsed set
	WORD	m_wNumInCollapsedSet;

	WORD	* m_wTriVertexIndicesToCollapsedSet;

	NeighborTriIndices * m_pTriNeighbors;		// one NeighborTriIndices struct for
												// each tri in m_pIndices

	SimpleObject	* m_pWingedEdgeMarkersObject;	// geometry which marks the winged-edge
													// connections.


	HRESULT AllocateData( WORD numverts, WORD numtris );

	void	BuildNonDegeneratePositions();
	void	BuildNonEdgeDegenerateTris();

	void	CompareTris( std::vector< WORD > & set1, std::vector< WORD > & set2, 
						 std::vector< WORD > & result );


	void	Warning_OpenHull();			// warning message about having an open hull
	void	Warning_SoloEdge();			// An edge has < 2 tris shared

	void	SetToConstructionValues();

	void	RemoveUnusedVerts();		// eliminates unused verts after exploding tris
										//  and processing


public:

	bool			m_bMarkWeldedVertices;	// use only for diagnostic/debug

	bool			m_bOpenHull;
	bool			m_bSoloEdge;


	HRESULT			BuildWingedEdgeData();


							// Generate additional geometry in the
							//  m_pWingedEdgeMarkersObject to mark the 
							//  winged-edge topology connections.
							// Currently a crude visualization.
	SimpleObject ** GenerateWingedEdgeMarkers();

							// weld faces (collapse degenerate vertices
							//  and edges) if the dot product of the
							//  face normals is greater than threshold.
	void			WeldFacetedFaces( float threshold );


	virtual void	ReverseWinding();  

	
	virtual HRESULT Free();
	HRESULT			FreeTemporaryData();

	bool			IsWingedEdgeDataBuilt();

	WingedEdgeData();
	virtual ~WingedEdgeData();

	friend class ShadowVolumeObject8;
};




#endif		// __NV_WingedEdgeData_H__
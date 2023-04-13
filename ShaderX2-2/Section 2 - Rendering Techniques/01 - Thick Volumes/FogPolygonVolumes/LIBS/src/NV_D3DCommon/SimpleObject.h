/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  SimpleObject.h

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

Trying new shared codebase between D3D8 and D3D9.

******************************************************************************/




#ifndef __SimpleObject_89_H__
#define __SimpleObject_89_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning(disable: 4786)
#include <vector>


#include <NVD3DVers.h>
#include <NV_D3DCommon/SimpleObjectVertex.h>


//////////////////////////////////////////////////////////

typedef std::vector< UINT >			V_UINT;
typedef std::vector< D3DXVECTOR4 >	V_V4;
typedef std::vector< D3DXVECTOR3 >	V_V3;


//////////////////////////////////////////////////////////


class SimpleObject  
{
public:
	bool		m_bIsValid;

	SimpleObjVertex *	m_pVertices;
	UINT				m_wNumVerts;
	UINT			*	m_pIndices;
	UINT				m_wNumInd;

	D3DPRIMITIVETYPE	m_PrimType;



	void UnAllocate();	// Free all allocated memory
	void Allocate( UINT nvert, UINT nind );

	void AllocateIndices( UINT nind );
	void AllocateMoreIndices( UINT nind );		// copies old indices into new array

	void AllocateMorePositions( UINT wNumTotalVertices );	// copies old verts into new array

	void AllocateReducedVerts( UINT n_vert_to_keep );


	bool IsValid()  { return( m_bIsValid ); };



	void InitTriangle( D3DXVECTOR3 & pt1, D3DXVECTOR3 & pt2, D3DXVECTOR3 & pt3 );

	void InitSphere( float radius, int nLat, int nLong );  // Sphere with n_ verts each direction
	void InitSphere( float radius, int nLat, int nLong,
						const D3DXVECTOR3 & axis1, 
						const D3DXVECTOR3 & axis2, 
						const D3DXVECTOR3 & axis3			);

	void InitSphereFromBox( float radius, int n_sub_x,
							int n_sub_y, int n_sub_z,
							const D3DXVECTOR3 & axis1,
							const D3DXVECTOR3 & axis2,
							const D3DXVECTOR3 & axis3  );


	void InitRectangularBlock_Smooth( const D3DXVECTOR3 center,
									  DWORD color,
									  const D3DXVECTOR3 & axis1,
									  const D3DXVECTOR3 & axis2,
									  const D3DXVECTOR3 & axis3 );

	void InitRectangularBlock_Faceted( const D3DXVECTOR3 center,
									   DWORD color,
									   const D3DXVECTOR3 & axis1,
									   const D3DXVECTOR3 & axis2,
									   const D3DXVECTOR3 & axis3  );


	void InitTesselatedPlane( const D3DXVECTOR3 & base_point, const D3DXVECTOR2 & base_uv,
							  const D3DXVECTOR3 & ccw_point,  const D3DXVECTOR2 & ccw_uv,
							  const D3DXVECTOR3 & cw_point,	  const D3DXVECTOR2 & cw_uv,
							  int n_sub_width, int n_sub_height );

	void InitTesselatedPlane( const D3DXVECTOR3 & width_direction, 
								 const D3DXVECTOR3 & height_direction,
								 float width, float height,
								 int n_sub_width, int n_sub_height );

	void InitTesselatedPlane_TriStrip(	const D3DXVECTOR3 & width_direction,								 const D3DXVECTOR3 & height_direction,
										 float width, float height,
										 int n_sub_width, int n_sub_height );


	void InitExtrusion( D3DXVECTOR3 * pPos, D3DXVECTOR3 * pNrm, D3DCOLOR * pCol,
						D3DXVECTOR2 * pTexCoord,
						int num_verts_in_cross_section,
						int num_cross_sections,
						bool stitch_section_end, bool stitch_extrusion_ends );

	void InitExtrusion_Subdivide( D3DXVECTOR3 * pPos, D3DXVECTOR3 * pNrm,
											D3DCOLOR * pCol,
											D3DXVECTOR2 * pTexCoord,
											int num_verts_in_cross_section,
											int num_cross_sections,
											bool stitch_section_end,
											bool stitch_extrusion_ends,
											int subdiv_cross_section,
											int add_cross_sections    );

	void InitCylinder(	D3DXVECTOR3 & end_cap1_center,
						D3DXVECTOR3 & end_cap2_center,
						float radius, int num_sides,
						int num_cap1_subdiv, int num_length_subdiv,
						int num_cap2_subdiv );

	void InitLathedObject( D3DXVECTOR3 * pPos, D3DXVECTOR3 * pNrm, D3DCOLOR * pCol,
							D3DXVECTOR2 * pTexCoord,
							int num_verts_in_cross_section,
							D3DXVECTOR3 & lathe_axis,
							float lathe_angle_start,	// in degrees
							float lathe_angle_end,		// in degrees
							int num_cross_sections,		// number in final object
							bool close_cross_section_ends,
							bool stitch_last_cross_section_to_first );

	void InitSpiral( float width, float height, int n_sub_width,
					int n_sub_height, float n_twists );

	void InitSpiral(	const D3DXVECTOR3 & width_direction, 
						const D3DXVECTOR3 & height_direction,
						float width, float height, int n_sub_width, 
						int n_sub_height, float n_twists );


	void InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings );
	void InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings,
					float TexCoordUMax, float TexCoordVMax );

	void InitTorusWrapped( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings );

	void InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings,
								const D3DXVECTOR3 & axis1,
								const D3DXVECTOR3 & axis2,
								const D3DXVECTOR3 & axis3	);


	void InitClone( SimpleObject * pObj );			// copy the input object
	void InitAddClone( SimpleObject * pObj );		// Add a copy of the input so a single complex object
													//  can be built from many simple objects.

	virtual void PostInit() { m_bIsValid = true; };				// If derived objs need to update things

	////////////////////////////////////////////
	void InitTest1_4QuadPlane(	const D3DXVECTOR3 & center,
								const D3DXVECTOR3 & axis1,
								const D3DXVECTOR3 & axis2  );
	
	////////////////////////////////////////////

	void AddPositionNoise( D3DXVECTOR3 vNoiseDir, float noise_magnitude );

	void AddPositionNoiseGrid( D3DXVECTOR3 vNoiseDir, float noise_magnitude,
								int grid_nx, int nsmooth );


	void AddDoubleTapsToFirstAndLast();	 // For strips, so we can combine several
										 // strips into one obj

	void MultiplyTexCoords( float ufactor, float vfactor );

	void TranslateXMinToZero();
	void Translate( float x, float y, float z );

	void Scale( float x, float y, float z );		// mult positions

	//@@ deprecate these two
	void GetPositionMinima( float * px, float * py, float * pz );
	void GetPositionMaxima( float * px, float * py, float * pz );

	void ComputePositionMinAndMax(	float * xmin, float * ymin, float * zmin,
									float * xmax, float * ymax, float * zmax ); // Finds object's AABB


	void ComputeBoundingSphere( D3DXVECTOR3 * out_center, float * out_radius );

	void ComputeAABBCenter( D3DXVECTOR3 * pCenter );	// center of object extents
	void ComputeObjectCenter( D3DXVECTOR3 * pCenter );	// avg of all points
	void ComputeTriCenter( D3DXVECTOR3 * pResult, UINT tri_index );


	void SetVertexColors( DWORD ARGB_color );
	void SetColorFromVertexNormal();


	virtual void ReverseWinding();		// change winding for all triangles
	void		 ReverseNormals();		// invert all normals



	////////////////////////////////////////////

	void	MakeFaceted();			// create degenerate verts for each triangle

	virtual void ExplodeFaces( float distance );	// move faces out along vector from
													//  center of object

	void	EliminateDegenerateTris();

	void	EliminateDuplicateTris();

	virtual	void RemoveUnusedVerts();

	void	RemoveDegenerateVerts( float pt_separation_threshold );

	///////////////////////

	void	TestEliminateTrisWithoutTheseVertices( V_UINT & verts );

	void	ListVertIndicesUsed();
	void	ListPositions( int start_vert, int last_vert );

	///////////////////////
	// You must keep the pointer returned, and pass
	//  it back to the Free() funcs to free it up 
	void FreeDuplicatePositions( D3DXVECTOR3 ** pPos );
	void DuplicatePositions( D3DXVECTOR3 ** pPos, UINT * num_verts );

	void FreeDuplicateIndices( UINT ** pInd );
	void DuplicateIndices( UINT ** pInd, UINT * old_n_ind );
	///////////////////////

	void MakeTris_RegularGrid( UINT nwidth_verts, UINT nheight_verts );

	void GenerateNormalsCW();		// from vertex pos and triangle data
	void GenerateNormalsCCW();		// Generate them facing the oposite direction

	void CalculateNormalsCCW();		// Calculate vertex normals from face normals

	void	CalculateFaceNormalCCW( const SimpleObjVertex & p0,
									const SimpleObjVertex & p1,
									const SimpleObjVertex & p2,
									D3DXVECTOR3 * pOutNormal );

	virtual void Draw( NV_D3DDEVICETYPE * pDev );


	UINT	GetNumVerts()   { return( m_wNumVerts );	};
	UINT	GetNumIndices() { return( m_wNumInd );		};
	UINT	GetNumTris();

	void	GetTriVerts( UINT n_tri, D3DXVECTOR3 * pt1,
								    D3DXVECTOR3 * pt2, 
								    D3DXVECTOR3 * pt3 );

	SimpleObject();
	virtual ~SimpleObject();

private:
	// Utility functions for the class
	void _AddToCrossSec( V_V4 * pOut, const D3DXVECTOR4 * pIn,
							int nvec_in_section, int n_sections,
							bool stitch_section_end,
							int subdiv_cross_section	);
	void _AddToCrossSec( V_V3 * pOut, const D3DXVECTOR3 * pIn,
							int nvec_in_section, int n_sections,
							bool stitch_section_end,
							int subdiv_cross_section	);

	void _Normalize( V_V3 * pArray );

	void _AddCrossSections( V_V3 * pOut, const D3DXVECTOR3 * pIn,
								int nvec_in_section, int n_sections,
								bool stitch_extrusion_ends, int add_cross_sections );
	void _AddCrossSections( V_V4 * pOut, const D3DXVECTOR4 * pIn,
								int nvec_in_section, int n_sections,
								bool stitch_extrusion_ends, int add_cross_sections );


};


#endif



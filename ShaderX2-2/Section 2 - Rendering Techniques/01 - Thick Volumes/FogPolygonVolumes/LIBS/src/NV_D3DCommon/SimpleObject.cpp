/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  SimpleObject.cpp

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



******************************************************************************/


#include <windows.h>
#include <math.h>

#include <shared/NV_Error.h>
#include <shared/NV_Common.h>

#include <NV_D3DCommon/SimpleObject.h>

#include <vector>
#include <algorithm>


using namespace std;



#ifndef UL_PI
#define UL_PI      3.1415926535897932384626433832795028841971693993751
#endif

#ifndef UL_SQRT_2
#define UL_SQRT_2  1.4142135623730950488016887242096980785696718753769
#endif

#ifndef UL_PI2
#define UL_PI2     (UL_PI/2.0)
#endif


#define BOUNDCHK(in, cmp, msg ) { if( in >= cmp ) { FDebug(msg); assert( false ); }}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SimpleObject::SimpleObject()
{
	m_pVertices = NULL;
	m_pIndices	= NULL;

	m_wNumVerts = 0;
	m_wNumInd	= 0;
	m_PrimType  = D3DPT_TRIANGLELIST;

	m_bIsValid = false;
}

SimpleObject::~SimpleObject()
{
	UnAllocate();
}

//////////////////////////////////////////////////////////////////////

void SimpleObject::UnAllocate()
{
	if( m_pVertices )
		free( m_pVertices	);
	if( m_pIndices	)
		free( m_pIndices	);

	m_pVertices = NULL;
	m_pIndices = NULL;

	m_wNumVerts = 0;
	m_wNumInd	= 0;
	m_PrimType  = D3DPT_TRIANGLELIST;

	m_bIsValid = false;
}



void SimpleObject::AllocateIndices( UINT nind )
{
	// Allocates indices.  Any existing index data will 
	//  be lost.

	m_wNumInd = 0;
	if( m_pIndices != NULL )
	{
		free( m_pIndices );
	}

	m_pIndices = (UINT *) calloc( nind, sizeof( UINT ) );
	NULLASSERT( m_pIndices, "SimpleObject:: Can't allocate indices!", true );

	m_wNumInd = nind;
}


void SimpleObject::AllocateMorePositions( UINT wNumTotalVertices )
{
	// copies old verts into new array

	if( wNumTotalVertices < m_wNumVerts )
	{
		FDebug("SimpleObject more vertices:  ask for more, not less!\n");
		assert( false );
		return;
	}
	if( wNumTotalVertices >= 65536 )
	{
		FDebug("Too many verts!\n");
		assert( false );
		return;
	}


	SimpleObjVertex * pnv;
	pnv = (SimpleObjVertex*) calloc( wNumTotalVertices, sizeof( SimpleObjVertex ) );
	assert( pnv != NULL );

	// copy old verts to new array
	memcpy( pnv, m_pVertices, sizeof(SimpleObjVertex) * m_wNumVerts );

	// Fill rest of verts with 0
	memset( & (pnv[m_wNumVerts]), 0, sizeof( SimpleObjVertex) * ( wNumTotalVertices - m_wNumVerts ));

	free( m_pVertices );

	m_pVertices = pnv;
	m_wNumVerts = wNumTotalVertices;

}



void SimpleObject::AllocateMoreIndices( UINT nind )
{
	// Allocate new Indices array with nind number of indices
	// Copies old indices into new array

	if( nind <= m_wNumInd )
	{
		FDebug("SimpleObject more indices: ask for more, not less!\n");
		assert( false );
		return;
	}


	UINT * pInd = (UINT *) calloc( nind, sizeof( UINT ) );
	NULLASSERT( pInd, "SimpleObject:: Can't allocate more indices!", true );

	// copy old indices to new array
	memcpy( pInd, m_pIndices, sizeof(UINT) * m_wNumInd );

	// fill remaining indices with 0
	memset( & (pInd[ m_wNumInd ]), 0, sizeof(UINT) * (nind - m_wNumInd ) );

	m_wNumInd = nind;

	if( m_pIndices != NULL )
	{
		free( m_pIndices );
	}

	m_pIndices = pInd;
}



void SimpleObject::Allocate( UINT nvert, UINT nind )
{
	UnAllocate();

	m_pVertices = (SimpleObjVertex*) calloc( nvert, sizeof( SimpleObjVertex ) );
	NULLASSERT( m_pVertices, "SimpleObject:: Can't allocate vertices!", true );
	m_wNumVerts = nvert;

	AllocateIndices( nind );

}

void SimpleObject::AllocateReducedVerts( UINT n_vert_to_keep )
{
	// reduces vertex array to hold only first n_vert_to_keep vertices

	if( n_vert_to_keep > m_wNumVerts || m_pVertices == NULL )
	{
		FDebug("Error - can't AllocateReducedVerts!\n");
		assert( false );
		return;
	}

	SimpleObjVertex* pV;
	pV = (SimpleObjVertex*) calloc( n_vert_to_keep, sizeof( SimpleObjVertex ));
	NULLASSERT( pV, "SimpleObject:: Can't reduce vertices!", true );

	int i;
	for( i=0; i < n_vert_to_keep; i++ )
	{
		pV[i] = m_pVertices[i];
	}

	free( m_pVertices );

	m_pVertices = pV;
	m_wNumVerts = n_vert_to_keep;

}


void SimpleObject::FreeDuplicatePositions( D3DXVECTOR3 ** pPos )
{
	if( *pPos != NULL )
	{
		free( *pPos );
		*pPos = NULL;
	}	
}


void SimpleObject::DuplicatePositions( D3DXVECTOR3 ** pPos, UINT * num_verts )
{
	// When done with the buffer, call the FreeDuplicatePositions() function
	//  above.

	*pPos = (D3DXVECTOR3*) calloc( m_wNumVerts, sizeof( D3DXVECTOR3 ));
	assert( *pPos != NULL );

	*num_verts = m_wNumVerts;

	int i;
	for( i=0; i < m_wNumVerts; i++ )
	{
		(*pPos)[i] = m_pVertices[i].pos;
	}
}


void SimpleObject::FreeDuplicateIndices( UINT ** pInd )
{
	if( *pInd != NULL )
	{
		free( *pInd );
		*pInd = NULL;
	}
}

void SimpleObject::DuplicateIndices( UINT ** pInd, UINT * old_n_ind )
{
	*pInd = (UINT*) calloc( m_wNumInd, sizeof( UINT ));
	assert( *pInd != NULL );

	*old_n_ind = m_wNumInd;

	memcpy( *pInd, m_pIndices, sizeof( UINT ) * m_wNumInd );
}



//-----------------------------------------------------------------------------
// Name: SimpleObject::InitSphere
// Desc: Make a sphere with arbitrary basis axes.
//   *** There are no checks or restrictions forcing the basis axes to be
//       orthogonal!
//  The sphere is made by sweeping an arc, so it will have a seam of overlapping
//    unconnected vertices along one edge.  The cap is tesselated separately
//    to avoid zero-area triangles and to minimize distortion of the texture
//    at the poles.
//  The resulting sphere will have many vertices sharing the same position
//    at the poles.  These vertices each have different texture coordinates, 
//    and this makes for a sphere relatively free of severe distortions of 
//    texture at the poles that you would see with other approaches.
//
// ****** This will not generate proper normals if the 
//  axis vectors introduce a scale factor or are non-orthogonal
//  This generates correct normals only for a true sphere!
//-----------------------------------------------------------------------------

void SimpleObject::InitSphere( float radius, int nLat, int nLong,
								const D3DXVECTOR3 & axis1, 
								const D3DXVECTOR3 & axis2, 
								const D3DXVECTOR3 & axis3			)
{
	int i,j;

	if( nLong < 1 )	nLong = 1;
	if( nLat  < 3 ) nLat  = 1;
	if( radius < 0 ) radius = -radius;

	assert( radius != 0.0f );


	//  If input nLong = 3, nLat = 2 (which are expanded for 
	//    vertex counts of nLong=4, nLat = 4, then the grid of
	//    vertices looks like this:
	//		
	//		  12   13    14   15
	//
	//		8    9    10   11
	//
	//		4    5    6    7
	//
	//		  0    1    2     3
	//
	//	** Vertices 15 and 3 are never used. **
	//  Vertices 0,1,2,3 are all at the same position at the pole.
	//  Same for vertices 12, 13, 14, 15
	//  The UV coords are offset by 1/2 the increment from 4 to 5
	//   to lessen distortion of textures at the poles.  If this
	//   were not done, the texture would spiral in at the poles
	//   in two rougly fan-like halves.
	//

	nLong++;		// repeat edge for texture coord happy
	nLat+=2;		// two rings on the ends

	UINT nvert = nLong * nLat;	

	////////////////////////////////////////////////
	// Explanation of ntri number:
	//
	// 2 * nLong * nLat => quads over grid nLong * nLat
	// - nLat * 2       => No quad for last row of verts which have
	//						same position as first row of verts
	// - nLong          => - 2 * nLong / 2 => Only 1 tri at end caps
	//					    instead of a quad

	UINT ntri  = 2 * nLong * nLat - nLong - nLat * 2;

	m_PrimType  = D3DPT_TRIANGLELIST;

	// Allocate memory:
	Allocate( nvert, ntri * 3 );

	//m_csDesc = "I'm a sphere!";
	//m_csName = "Sphere01";

	// make vertex positions
	int   index;
	float ringz;			// z cord of a latitude ring
	float ring_r;			// radius of x,y ring
	float theta, rho;		// theta = x,y plane angle, rho = z
	float x,y,u,v;				// x,y coord of pt.
	

	for( j=0; j < nLat; j++ )
	{
		rho = (float)( UL_PI * j / (float)(nLat-1) - UL_PI/2.0f );
		ringz  = (float) ( radius * sin(rho) );
		ring_r = (float) sqrt( fabs( radius * radius - ringz*ringz) );

		for( i=0; i < nLong; i++)
		{
			theta = (float)( 2* UL_PI * i / (float)( nLong - 1 ) );
			x = (float) ( ring_r * cos(theta) );
			y = (float) ( ring_r * sin(theta) );

			index = i + j*nLong;
			if( index > m_wNumVerts )
			{
				FDebug("InitSphere Error! %n index of only %n allocated!\n", index, m_wNumVerts );
				index = m_wNumVerts -1;
			}

			m_pVertices[ index ].pos.x = x * axis1.x + y * axis2.x + ringz * axis3.x;
			m_pVertices[ index ].pos.y = x * axis1.y + y * axis2.y + ringz * axis3.y;
			m_pVertices[ index ].pos.z = x * axis1.z + y * axis2.z + ringz * axis3.z;
				
			/*
			if( j==0 || j== 1 || j==nLat-1 )
			{
				FMsg("j=%4.4d     %f   %f   %f\n", j, m_pVertices[ index ].pos.x,
						m_pVertices[ index ].pos.y, m_pVertices[ index ].pos.z );

				FMsg("j=%4.4d     %f   %f   %f\n", j, x,y,ringz );
				FMsg("ringr   %f    theta %f\n", ring_r, theta );
				FMsg("radius  %f    \n", radius );
				FMsg("\n");
			}
			*/

			u = (float)( (float)i/(float)(nLong-1) );
			v = (float)( (float)j/(float)(nLat-1)  );

			// Correct for sphere pinch & "degenerate" triangle at caps
			if( j==0 )
			{
				// If at top/bottom of sphere
				u -= 0.5f / (float)nLong;
			}
			else if( j == nLat-1 )
			{
				// If at top/bottom of sphere
				u += 0.5f / (float)nLong;
			}

			m_pVertices[ index ].t0.x = u;
			m_pVertices[ index ].t0.y = v;

			//FDebug("U:  %f    V:  %f   ind: %d\n", u, v, index );

			//////////////////////////////////////////////////////////////////////
			// ****** This will not generate proper normals if the 
			//  axis vectors introduce a scale factor or are non-orthodonal
			//  This generates correct normals only for a true sphere!

			m_pVertices[index].nrm = m_pVertices[index].pos;

			D3DXVec3Normalize( & m_pVertices[index].nrm, &m_pVertices[index].nrm );
			//////////////////////////////////////////////////////////////////////


			m_pVertices[ index ].diffuse = D3DCOLOR_ARGB( 255, 255, 255, 255 );

		}
	}
	
	assert( index + 1 == m_wNumVerts );


	// stitch the tris
	// Culling is CCW
/*
//gj  This method builds zero-area tris and stitches the seam where
//    two vertices have the same position.  These tris are not needed.

	UINT  nind = 0;
	UINT  i0, i1, i2, i3;

	for( j=0; j < nLat-1; j++ )
	{
		for( i=0; i < nLong; i++)
		{
			i0 = i   + j*nLong;
			i1 = i   + (j+1)*nLong;
			i2 = (i+1)%nLong + j*nLong;		// % to wrap back to zero
			i3 = (i+1)%nLong + (j+1)*nLong;
		
			BOUNDCHK( nind, m_wNumInd-2, "InitSphere:: Index too high!", true );

			m_pIndices[nind++] = i2;
			m_pIndices[nind++] = i1;
			m_pIndices[nind++] = i0;

			BOUNDCHK( nind, m_wNumInd-2, "InitSphere:: Index too high 2!", true );
			m_pIndices[nind++] = i3;
			m_pIndices[nind++] = i1;
			m_pIndices[nind++] = i2;
		}
	}
*/
	/////////////////////////////////////////////////////////////////
	// This is a more efficient tesselation.  There are no zero-area
	//   tris

	// stitch the tris
	// Culling is CCW

	UINT  nind = 0;
	UINT  i0, i1, i2, i3;

	for( j=1; j < nLat-2; j++ )
	{
		for( i=0; i < nLong - 1; i++)
		{
			i0 = i   + j*nLong;
			i1 = i   + (j+1)*nLong;
			i2 = (i+1)%nLong + j*nLong;		// % to wrap back to zero
			i3 = (i+1)%nLong + (j+1)*nLong;
		
			BOUNDCHK( nind, m_wNumInd-2, "InitSphere:: Index too high!" );

			m_pIndices[nind++] = i2;
			m_pIndices[nind++] = i1;
			m_pIndices[nind++] = i0;

			BOUNDCHK( nind, m_wNumInd-2, "InitSphere:: Index too high 2!" );

			m_pIndices[nind++] = i3;
			m_pIndices[nind++] = i1;
			m_pIndices[nind++] = i2;
		}
	}

	// Stitch end caps
	// nLong-1 as there is no tri for last column of verts which is wrapped
	///  to lie at the same points as the first column.

	// Bottom cap:

	for( i=0; i < nLong-1; i++ )
	{
		// ^	nLong->
		// |

		// n
		// L
		// a
		// t
		//		  12   13    14   15
		//
		//		8    9    10   11
		//
		//		4    5    6    7
		//
		//		  0    1    2     3

		// First step = starts with vertex 0

		i0 = i          + 0 * nLong;

		// First step = vert 5
		// No need to modulo to wrap back - only 
		// stitching nLong-1 tris (3 for the above case)
		i1 = (i+1)		+ 1 * nLong;

		// First step:  end with vert 4

		i2 = i			+ 1 * nLong;


		BOUNDCHK( nind, m_wNumInd-2, "InitSphere:: Capping Index too high!" );
		m_pIndices[ nind++ ] = i0;
		m_pIndices[ nind++ ] = i1;
		m_pIndices[ nind++ ] = i2;
	}

	// Now for the top cap
	for( i=0; i < nLong-1; i++ )
	{
		//		  12   13    14   15
		//
		//		8    9    10   11
		//
		//		4    5    6    7
		//
		//		  0    1    2     3

		// First step = starts with vertex 12

		i0 = i          + (nLat-1) * nLong;

		// First step = vert 8

		i1 = i			+ (nLat-2) * nLong;

		// First step:  last vert = 9

		i2 = (i+1)		+ (nLat-2) * nLong;

		BOUNDCHK( nind, m_wNumInd-2, "InitSphere:: Capping Index too high!" );
		m_pIndices[ nind++ ] = i0;
		m_pIndices[ nind++ ] = i1;
		m_pIndices[ nind++ ] = i2;
	}

	m_wNumInd = nind;

	/////////////////////////////////////////////////////////////////

	m_bIsValid = true;
	PostInit();

	// all done
}						// InitSphere




//-----------------------------------------------------------------------------
// Name: SimpleObject::InitSphere( float radius, int nLat, int nLong )
// Desc: Make a sphere using a default set of axes.  
//-----------------------------------------------------------------------------

void SimpleObject::InitSphere( float radius, int nLat, int nLong )
{
	InitSphere( radius, nLat, nLong,
				D3DXVECTOR3( 1.0f, 0.0f, 0.0f ),
				D3DXVECTOR3( 0.0f, 1.0f, 0.0f ),
				D3DXVECTOR3( 0.0f, 0.0f, 1.0f )  );
}



void SimpleObject::InitSphereFromBox( float radius, int n_sub_x,
										int n_sub_y, int n_sub_z,
										const D3DXVECTOR3 & axis1,
										const D3DXVECTOR3 & axis2,
										const D3DXVECTOR3 & axis3  )
{
	//  Creates a sphere with better tesselation by turning a 
	//  tesselated box into a sphere.

	float sidesize = radius * 2.0f;

	InitTesselatedPlane( axis1, axis2, sidesize, sidesize, n_sub_x, n_sub_y );

	SimpleObject * pSO = new SimpleObject;
	if( pSO == NULL )
	{
		assert( false );
		return;
	}

	pSO->InitTesselatedPlane( axis1, axis2, sidesize, sidesize, n_sub_x, n_sub_y );
	pSO->Translate( -axis3.x * radius, -axis3.y * radius, -axis3.z * radius );

	InitClone( pSO );

	pSO->Translate( axis3.x * sidesize, axis3.y * sidesize, axis3.z * sidesize );
	pSO->ReverseWinding();

	InitAddClone( pSO );

	pSO->InitTesselatedPlane( axis1, axis3, sidesize, sidesize, n_sub_x, n_sub_z );
	pSO->Translate( axis2.x * radius, axis2.y * radius, axis2.z * radius );

	InitAddClone( pSO );

	pSO->Translate( -axis2.x * sidesize, -axis2.y * sidesize, -axis2.z * sidesize );
	pSO->ReverseWinding();

	InitAddClone( pSO );

	pSO->InitTesselatedPlane( axis3, axis2, sidesize, sidesize, n_sub_z, n_sub_y );
	pSO->Translate( axis1.x * radius, axis1.y * radius, axis1.z * radius );

	InitAddClone( pSO );

	pSO->Translate( -axis1.x * sidesize, -axis1.y * sidesize, -axis1.z * sidesize );
	pSO->ReverseWinding();
	pSO->SetVertexColors( 0x00FF0000 );

	InitAddClone( pSO );

	

	int i;
	for( i=0; i < m_wNumVerts; i++ )
	{
		D3DXVECTOR3 pos = m_pVertices[i].pos;

		D3DXVec3Normalize( &(m_pVertices[i].pos), &(m_pVertices[i].pos) );
		m_pVertices[i].pos = m_pVertices[i].pos * radius;

	}




/*
void SimpleObject::InitTesselatedPlane( const D3DXVECTOR3 & width_direction, 
										 const D3DXVECTOR3 & height_direction,
										 float width, float height, int n_sub_width, 
										 int n_sub_height )
*/


}




///////////////////////////////////////////////////////////////////////////////


void SimpleObject::InitTriangle( D3DXVECTOR3 & pt1, D3DXVECTOR3 & pt2, D3DXVECTOR3 & pt3 )
{
	Allocate( 3, 3 );
	m_PrimType = D3DPT_TRIANGLELIST;
	m_bIsValid = true;


	m_pVertices[0].pos = pt1;
	m_pVertices[1].pos = pt2;
	m_pVertices[2].pos = pt3;

	m_pVertices[0].t0 = D3DXVECTOR2( 0.0f, 0.0f );
	m_pVertices[1].t0 = D3DXVECTOR2( 1.0f, 0.0f );
	m_pVertices[2].t0 = D3DXVECTOR2( 0.0f, 1.0f );

	m_pIndices[0] = 0;
	m_pIndices[1] = 1;
	m_pIndices[2] = 2;

	GenerateNormalsCCW();
	PostInit();
}


///////////////////////////////////////////////////////////////////////////////

void SimpleObject::InitTest1_4QuadPlane(	const D3DXVECTOR3 & center,
											const D3DXVECTOR3 & axis1,
											const D3DXVECTOR3 & axis2  )
{

	Allocate( 16, 24 );
	m_PrimType = D3DPT_TRIANGLELIST;
	m_bIsValid = true;

	int i;
	i=0;

	D3DXVECTOR3  d1 = axis1 / 2.0f;
	D3DXVECTOR3  d2 = axis2 / 2.0f;

	m_pVertices[0].pos = center;
	m_pVertices[1].pos = center + d1;
	m_pVertices[2].pos = center + d1 + d2;
	m_pVertices[3].pos = center + d2;

	m_pIndices[i++] = 0;	m_pIndices[i++] = 1;	m_pIndices[i++] = 2;
	m_pIndices[i++] = 0;	m_pIndices[i++] = 2;	m_pIndices[i++] = 3;

	m_pVertices[4].pos = center;
	m_pVertices[5].pos = center + d2;
	m_pVertices[6].pos = center + d2 - d1;
	m_pVertices[7].pos = center - d1;

	m_pVertices[4 ].diffuse = 0x0000FF00;
	m_pVertices[5 ].diffuse = 0x0000FF00;
	m_pVertices[6 ].diffuse = 0x0000FF00;
	m_pVertices[7 ].diffuse = 0x0000FF00;

	int inc;
	inc = 4;
	m_pIndices[i++] = m_pIndices[0] + inc;	m_pIndices[i++] = m_pIndices[1] + inc;
	m_pIndices[i++] = m_pIndices[2] + inc;	m_pIndices[i++] = m_pIndices[3] + inc;
	m_pIndices[i++] = m_pIndices[4] + inc;	m_pIndices[i++] = m_pIndices[5] + inc;
	
	m_pVertices[8 ].pos = center;
	m_pVertices[9 ].pos = center - d1;
	m_pVertices[10].pos = center - d2 - d1;
	m_pVertices[11].pos = center - d2;

	m_pVertices[8 ].diffuse = 0x00FF0000;
	m_pVertices[9 ].diffuse = 0x00FF0000;
	m_pVertices[10].diffuse = 0x00FF0000;
	m_pVertices[11].diffuse = 0x00FF0000;

	inc = 8;
	m_pIndices[i++] = m_pIndices[0] + inc;	m_pIndices[i++] = m_pIndices[1] + inc;
	m_pIndices[i++] = m_pIndices[2] + inc;	m_pIndices[i++] = m_pIndices[3] + inc;
	m_pIndices[i++] = m_pIndices[4] + inc;	m_pIndices[i++] = m_pIndices[5] + inc;

	m_pVertices[12].pos = center;
	m_pVertices[13].pos = center - d2;
	m_pVertices[14].pos = center - d2 + d1;
	m_pVertices[15].pos = center + d1;

	m_pVertices[12].diffuse = 0x000000FF;
	m_pVertices[13].diffuse = 0x000000FF;
	m_pVertices[14].diffuse = 0x000000FF;
	m_pVertices[15].diffuse = 0x000000FF;

	inc = 12;
	m_pIndices[i++] = m_pIndices[0] + inc;	m_pIndices[i++] = m_pIndices[1] + inc;
	m_pIndices[i++] = m_pIndices[2] + inc;	m_pIndices[i++] = m_pIndices[3] + inc;
	m_pIndices[i++] = m_pIndices[4] + inc;	m_pIndices[i++] = m_pIndices[5] + inc;

	assert( i == 24 );

	GenerateNormalsCCW();
	PostInit();
}


// ============================================================ //
// Function name	: SimpleObject::InitRectangularBlock_Smooth
// Description	    : 
//		This function really makes a parallel piped with 8 vertices
//      Corners have only one normal so the object is "smooth"
//      Use the _Faceted(..) to create a block with degenerate 
//  vertices for different normals at the corners
//		axis1, 2, and 3 should specify a right 
//  handed coordinate system, where 1 CROSS 2 points in the
//  direction of 3 for a right-handed cross product.  The function
//	does not check for this.
//      Length of each side will be the lenght of each axis vector
//  Winding is CCW
//  Color is ARGB
//
// ============================================================ //

void SimpleObject::InitRectangularBlock_Smooth( const D3DXVECTOR3 center,
												 DWORD color,
												 const D3DXVECTOR3 & axis1,
												 const D3DXVECTOR3 & axis2,
												 const D3DXVECTOR3 & axis3  )
{
	int i;
	int nvert, nind;

	nvert = 8;
	nind  = 12 * 3;

	Allocate( nvert, nind );

	m_PrimType = D3DPT_TRIANGLELIST;

	D3DXVECTOR3		pVert[ 8 ];

	pVert[0] = center - axis1 / 2.0f - axis2 / 2.0f - axis3 / 2.0f;
	pVert[1] = pVert[0] + axis1;
	pVert[2] = pVert[1] + axis2;
	pVert[3] = pVert[0] + axis2;

	for( i=4; i < 8; i++ )
	{
		pVert[i] = pVert[ i - 4 ] + axis3;
	}

	for( i=0; i < 8; i++ )
	{
		m_pVertices[i].pos = pVert[i];
		m_pVertices[i].t0 = D3DXVECTOR2( 0.0f, 0.0f );
		m_pVertices[i].diffuse = color;
	}

	i=0;
	m_pIndices[i++] = 0;	
	m_pIndices[i++] = 1;	
	m_pIndices[i++] = 5;	
							
	m_pIndices[i++] = 0;	
	m_pIndices[i++] = 5;	
	m_pIndices[i++] = 4;	
							
	m_pIndices[i++] = 0;	
	m_pIndices[i++] = 4;	
	m_pIndices[i++] = 7;	
							
	m_pIndices[i++] = 0;	
	m_pIndices[i++] = 7;	
	m_pIndices[i++] = 3;	
							
	m_pIndices[i++] = 0;	
	m_pIndices[i++] = 2;	
	m_pIndices[i++] = 1;	
							
	m_pIndices[i++] = 0;	
	m_pIndices[i++] = 3;	
	m_pIndices[i++] = 2;	

	//////////////////////////////

	m_pIndices[i++] = 7;	
	m_pIndices[i++] = 4;	
	m_pIndices[i++] = 5;	
							
	m_pIndices[i++] = 7;	
	m_pIndices[i++] = 5;	
	m_pIndices[i++] = 6;	
							
	m_pIndices[i++] = 6;	
	m_pIndices[i++] = 5;	
	m_pIndices[i++] = 2;	
							
	m_pIndices[i++] = 2;	
	m_pIndices[i++] = 5;	
	m_pIndices[i++] = 1;	
							
	m_pIndices[i++] = 2;	
	m_pIndices[i++] = 7;	
	m_pIndices[i++] = 6;	
							
	m_pIndices[i++] = 2;	
	m_pIndices[i++] = 3;	
	m_pIndices[i++] = 7;	

	m_bIsValid = true;

	GenerateNormalsCCW();
	PostInit();
}



// ============================================================= //
// Function name	: SimpleObject::InitRectangularBlock_Faceted
// Description	    : Similar to the _Smooth but this one makes
//    a block with 24 vertices instead of only 8.  This is so 
//    each vertex can have a different normal and the cube can
//    be rendered with facets instead of smooth shading.
//
// ============================================================= //

void SimpleObject::InitRectangularBlock_Faceted( const D3DXVECTOR3 center,
												  DWORD color,
												  const D3DXVECTOR3 & axis1,
												  const D3DXVECTOR3 & axis2,
												  const D3DXVECTOR3 & axis3  )
{

	InitRectangularBlock_Smooth( center, color, axis1, axis2, axis3 );

	// Take the vertices / tris from the smooth block and expand them out into
	//  independent faces

	MakeFaceted();				// create degenerate vertices for each triangle

	GenerateNormalsCCW();

	m_bIsValid = true;

	PostInit();
}


// ============================================================= //
// Function name	: SimpleObject::MakeFaceted
// Description	    : Break up every triangle to use its own 
//    unique vertices, so every face will have a unique normal --
//    a flat face normal across the entire face.
//   This will increase the vertex count, to say the least.
// ============================================================= //


void SimpleObject::MakeFaceted()
{
	int i;

	UINT nvert = m_wNumInd;


	SimpleObject * pClone = new SimpleObject;
	assert( pClone != NULL );

	pClone->InitClone( this );


	// Allocate new vertices
	Allocate( nvert, m_wNumInd );


	// Fill in the unique vertices for each triangle from the
	//  cloned object

	for( i = 0; i < m_wNumVerts; i++ )
	{
		m_pVertices[i] = pClone->m_pVertices[ pClone->m_pIndices[i] ];
	}

	// re-map indices to use the new vertices
	for( i = 0; i < m_wNumInd; i++ )
	{
		m_pIndices[i] = i;
	}

	m_bIsValid = true;

	// Do not regenerate normals here
	// User can do that after MakeFaceted()

	// Free the cloned object
	delete pClone;
	pClone = NULL;

}


void	SimpleObject::ListPositions( int start_vert, int last_vert )
{

	assert( start_vert >=0 && start_vert < m_wNumVerts );
	assert( last_vert >=0 && last_vert  < m_wNumVerts );
	assert( last_vert >= start_vert );

	int i;

	FDebug("\n\n");
	FDebug("SimpleObject Vertex Positions:\n");

	for( i = start_vert; i < last_vert + 1; i++ )
	{
		FDebug("x: %10f   y: %10f   z: %10f\n", m_pVertices[i].pos.x, m_pVertices[i].pos.y, m_pVertices[i].pos.z );
	}

	FDebug("\n");
}



void	SimpleObject::ListVertIndicesUsed()
{
	// Text output to debug console

	V_UINT verts;
	V_UINT :: iterator v_end;

	int i;

	for(i=0; i < m_wNumInd; i++ )
	{
		verts.push_back( m_pIndices[ i ] );
	}

	sort( verts.begin(), verts.end() );
	v_end = unique( verts.begin(), verts.end() );

	verts.erase( v_end, verts.end() );

	FDebug("Vertex indices used for triangles:\n");
	for( i=0; i < verts.size(); i++ )
	{
		FDebug("%5.5d   ", verts.at(i) );

		if( (i+1)%10 == 0 )
			FDebug("\n");
	}

	FDebug("\n");
	FDebug("\n");
}




void SimpleObject::RemoveDegenerateVerts( float pt_separation_threshold )
{
	// Does a search between all verts.  This is order
	//  n^2, which is bad.  It could be made faster by
	//  using spatial partitioning.

	assert( m_wNumVerts   > 0 );
	assert( m_pVertices != NULL );

	UINT i, n;
	
	float		pt_sepsqrd = pt_separation_threshold * pt_separation_threshold;

	float		distsq;
	D3DXVECTOR3 delta;

	// make an array of vert indices for re-mapping vertices
	UINT * pVertInd = new UINT[ m_wNumVerts ];

	// fill re-mapping array with default mapping
	for( i=0; i < m_wNumVerts; i++ )
	{
		pVertInd[i] = i;
	}

	for( i=0; i < m_wNumVerts; i++ )
	{
		if( pVertInd[i] == i )
		{
			// We haven't replaced the vertex, so do the search
			//  for other verts in the same position

			for( n = i + 1; n < m_wNumVerts; n++ )
			{
				delta = m_pVertices[i].pos - m_pVertices[n].pos;
				distsq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

				if( distsq < pt_sepsqrd )
				{
					pVertInd[n] = pVertInd[i];
				}
			}
		}
	}

	// now modify triangles to use re-mapped vertices
	int tri;
	for( tri =0; tri < m_wNumInd; tri++ )
	{
		m_pIndices[tri] = pVertInd[ m_pIndices[tri] ];
	}

	// get rid of temporary vertex remapping array
	SAFE_ARRAY_DELETE( pVertInd );


	// eliminate unused vertices
	RemoveUnusedVerts();

}




void SimpleObject::RemoveUnusedVerts()
{


#define RUVDIAG 0

	// scan indices array, find any vertices which are not used
	//  and remove them

	// Now scan index list & remove vertices

	// Make an array used to remap vertex locations.
	//  This is for efficient copying of new vertex location
	//  back into the indices array.
	
	int * pVertMapping = new int[ m_wNumVerts ];
	if( pVertMapping == NULL )
	{
		assert( false );
		return;
	}

	bool * pVertUsed = new bool[ m_wNumVerts ];
	if( pVertUsed == NULL )
	{
		assert( false );
		return;
	}


	int i;

	// Start with everything marked unused with -1 value
	for( i=0; i < m_wNumVerts; i++ )
	{
		pVertUsed[i] = false;
		pVertMapping[i] = i;
	}

	// If vertex is used mark it's value
	UINT indx;

	for( i=0; i < m_wNumInd; i++ )
	{
		indx = m_pIndices[i];
		if( indx < 0 || indx >= m_wNumVerts )
		{
			FDebug("Error in WEO RemoveUnusedVerts");
			assert( false );
			return;
		}

		// if vert N is used, fill it's mapping with the value N
		pVertUsed[ indx ] = true;
	}

#if RUVDIAG
	int unused_count_before = 0;
	for( i=0; i < m_wNumVerts; i++ )
	{
		if( pVertUsed[i] == false )
			unused_count_before++;
	}
#endif


	// remove vertices for all pVertMapping values of -1
	int src;
	bool moved;
	int last_vertex = -1;

	for( i=0; i < m_wNumVerts; i++ )
	{
		if( pVertUsed[i] == false )
		{
			moved = false;

			for( src=i+1; src < m_wNumVerts; src++ )
			{
				if( pVertUsed[src] == true )
				{
					// remember where the removed vert was 
					// moved to.  Mark the vert as unused.
					pVertMapping[src] = i;
					pVertUsed[src] = false;

					// copy the vert
					m_pVertices[i] = m_pVertices[src];
					moved = true;
					last_vertex = i;
	
					// fill unused vertex with junk value
					// This may get overwritten later as a new vertex is
					//  moved into the unused slot.
					m_pVertices[src].pos = D3DXVECTOR3( 9e9f, 9e9f, 9e9f );	

					// break out of the for loop
					break;
				}
			}

 /*
			if( moved == false )
			{
				FDebug("vertex %d not filled with replacement - no further used verts after it!\n", i );
			}
// */
		}
	}


#if RUVDIAG

	if( last_vertex > 0 )
	{
		FDebug("\n");
		FDebug("Number orig verts:  %u\n", m_wNumVerts );
		FDebug("Number removed:     %d\n", unused_count_before );
		FDebug("Number remaining:   %d\n", last_vertex + 1 );

		assert( (int)m_wNumVerts == ( unused_count_before + last_vertex + 1 ) );
	}
	else
	{
		FDebug("No vertices removed!\n");
	}

#endif



	// re-allocate vertex array if we've removed vertices
	if( last_vertex >= 0 )
	{
		AllocateReducedVerts( (UINT) last_vertex + 1 );

		// re-map indices to use new vertex locations
		for( i=0; i < m_wNumInd; i++ )
		{
			m_pIndices[i] = pVertMapping[ m_pIndices[i] ];
		}
	}

	// free temporary data
	SAFE_ARRAY_DELETE( pVertMapping );
	SAFE_ARRAY_DELETE( pVertUsed );

}



void	SimpleObject::EliminateDuplicateTris()
{
	// not well tested

	V_UINT new_indices;
	V_UINT tri_hashes;

	UINT potential_cnt = 0;
	UINT actual_cnt = 0;
	UINT i;

	// ((a^b)^c) = (a^(b^c)) = ((a^c)^b)

	tri_hashes.reserve( m_wNumInd / 3 );
	new_indices.reserve( m_wNumInd );

	// Create hash value for each triangle based on it's index
	// values.  Order of indices must not change the hash value.

	UINT a1, a2, a3, b1, b2, b3, tmp;
	for( i=0; i < m_wNumInd / 3; i++ )
	{
		a1 = m_pIndices[i*3+0];
		a2 = m_pIndices[i*3+1];
		a3 = m_pIndices[i*3+2];

		a1 = a1 * 67 + a2 * 59 + a3 * 71;
		a1 = ( a1 << 13 ) ^ a1;
		a1 = ( a1*(a1*a1*15731 + 789221) + 1376312589);

		tri_hashes.push_back( a1 );
	}

	UINT lookfor;
	UINT j;

#define SOBJ_SORTTHREE( a, b, c, t )		\
	if( b < a ) { t = a; a = b; b = t; }	\
	if( c < b ) { t = b; b = c; c = t; }	\
	if( b < a ) { t = a; a = b; b = t; }

	D3DXVECTOR3 normal;
	D3DXVECTOR3 vertnormals;
	float dpi, dpj;

	V_UINT ind_to_eliminate;

	for( i=0; i < tri_hashes.size(); i++ )
	{
		lookfor = tri_hashes.at(i);

		for( j=i+1; j < tri_hashes.size(); j++ )
		{
			if( tri_hashes.at(j) == lookfor )
			{
				potential_cnt++;
				// check if vert indices actualy match
				a1 = m_pIndices[i*3+0];
				a2 = m_pIndices[i*3+1];
				a3 = m_pIndices[i*3+2];
				b1 = m_pIndices[j*3+0];
				b2 = m_pIndices[j*3+1];
				b3 = m_pIndices[j*3+2];
				
				SOBJ_SORTTHREE( a1, a2, a3, tmp );
				SOBJ_SORTTHREE( b1, b2, b3, tmp );

				if( a1 == b1 )
				{
					if( a2 == b2 )
					{
						if( a3 == b3 )
						{
							// tris match
							// eliminate the one facing the wrong way							
							//@@@ hard wired to CCW winding order

							a1 = m_pIndices[i*3+0];
							a2 = m_pIndices[i*3+1];
							a3 = m_pIndices[i*3+2];
							b1 = m_pIndices[j*3+0];
							b2 = m_pIndices[j*3+1];
							b3 = m_pIndices[j*3+2];

							CalculateFaceNormalCCW( m_pVertices[a1],
													m_pVertices[a2],
													m_pVertices[a3],
													& normal );
							vertnormals =	m_pVertices[a1].nrm +
											m_pVertices[a2].nrm +
											m_pVertices[a3].nrm;
							dpi = D3DXVec3Dot( & vertnormals, & normal );

							CalculateFaceNormalCCW( m_pVertices[b1],
													m_pVertices[b2],
													m_pVertices[b3],
													& normal );
							dpj = D3DXVec3Dot( & vertnormals, & normal );

							if( ( (dpi < 0.0f) && (dpj < 0.0f) ) ||
								( (dpi > 0.0f) && (dpj > 0.0f) )		)
							{
//								FMsg("Both tris face the same way! %u %u\n", i, j );
//								FMsg("  Eliminating tri %u\n", j );
								ind_to_eliminate.push_back( i*3 + 0 );
								ind_to_eliminate.push_back( i*3 + 1 );
								ind_to_eliminate.push_back( i*3 + 2 );
							}
							else if( dpi < 0.0f )
							{
//								FMsg("Tri i = %u faces wrong way\n", i );
								ind_to_eliminate.push_back( i*3 + 0 );
								ind_to_eliminate.push_back( i*3 + 1 );
								ind_to_eliminate.push_back( i*3 + 2 );
							}
							else if( dpj < 0.0f )
							{
//								FMsg("Tri j = %u faces wrong way\n", j );
								ind_to_eliminate.push_back( j*3 + 0 );
								ind_to_eliminate.push_back( j*3 + 1 );
								ind_to_eliminate.push_back( j*3 + 2 );
							}
							else
							{
								FMsg("EliminateDuplicateTris Problem with normals! %f %f\n", dpi, dpj );
							}

							actual_cnt++;
						}
					}
				}
				else
				{
					// tris do not match
					// do nothing
//					if( potential_cnt % 1000 == 0 )
//						FMsg("no match:  [%u %u %u]  [%u %u %u]\n", a1, a2, a3, b1, b2, b3 );
				}
			}
		}
	}

///*
	for( i=0; i < ind_to_eliminate.size(); i++ )
	{
		m_pIndices[ ind_to_eliminate.at(i) ] = 0;
	}
// */

	FMsg("Potential matches:  %u\n", potential_cnt );
	FMsg("Actual matches:     %u\n", actual_cnt );

	EliminateDegenerateTris();

	FMsg("num faces after cleanup: %u\n", m_wNumInd / 3 );


}



void	SimpleObject::CalculateFaceNormalCCW( const SimpleObjVertex & p0,
												const SimpleObjVertex & p1,
												const SimpleObjVertex & p2,
												D3DXVECTOR3 * pOutNormal )
{
	RET_IF_NULL( pOutNormal );

	D3DXVECTOR3 d0, d1;

	d0 = p1.pos - p0.pos;
	d1 = p2.pos - p0.pos;

	D3DXVec3Cross( pOutNormal, &d0, &d1 );		// triangle normal, CCW
	D3DXVec3Normalize( pOutNormal, pOutNormal );
}



void	SimpleObject::EliminateDegenerateTris()
{
	// Eliminates tris where two or more vertex indices are identical

	V_UINT new_tri;
	new_tri.reserve( m_wNumInd );

	UINT num_eliminated = 0;
	UINT i1, i2, i3;
	UINT tri;
	for( tri = 0; tri < m_wNumInd / 3; tri++ )
	{
		i1 = m_pIndices[tri*3 + 0];
		i2 = m_pIndices[tri*3 + 1];
		i3 = m_pIndices[tri*3 + 2];

		if( ( i1 == i2 ) ||
			( i2 == i3 ) || 
			( i1 == i3 )	)
		{
			// don't add the tri
			num_eliminated++;
		}
		else
		{
			new_tri.push_back( i1 );
			new_tri.push_back( i2 );
			new_tri.push_back( i3 );
		}
	}


	AllocateIndices( new_tri.size() );
	assert( m_wNumInd == new_tri.size() );

	for( tri = 0; tri < m_wNumInd; tri++ )
	{	
		m_pIndices[tri] = new_tri.at(tri);
	}

	FDebug("EliminateDegenerateTris() eliminated %u\n", num_eliminated );

}




// ============================================================= //
// Function name	: SimpleObject TestEliminateTrisWithoutTheseVertices
// Description	    : Eliminate all faces which do not use one of the vertex
//    indices in the verts array.
//    V_UINT = { 0 }  eliminates all faces which do not use vertex 0
// ============================================================= //

void SimpleObject::TestEliminateTrisWithoutTheseVertices( V_UINT & verts )
{
#define FUNC_DEBUG	0

	UINT i, nv;
	UINT verti;
	UINT tribase;

	V_UINT	tris_to_keep;
	tris_to_keep.reserve( verts.size() * 6 );

	// determine which tris to keep

	for( nv = 0; nv < verts.size(); nv++ )
	{
		verti = verts.at(nv);
		
		for( i=0; i < m_wNumInd; i += 3 )
		{
			if( m_pIndices[i]	== verti || 
				m_pIndices[i+1] == verti ||
				m_pIndices[i+2] == verti	)
			{
				//	FDebug("  Vertex %d found in tri %d\n", nv, i/3);
				tris_to_keep.push_back( i/3 );
			}
		}
	}

#if FUNC_DEBUG
	FDebug("\n");
	FDebug("Tris to keep before unique:  size:  %d\n", tris_to_keep.size() );
	for( i=0; i < tris_to_keep.size(); i++ )
	{
		FDebug("%4.4d   ", tris_to_keep.at(i));
		if( (i+1) % 10 == 0 )
			FDebug("\n");		
	}
	FDebug("\n\n");
#endif

	V_UINT::iterator t_end;

	sort( tris_to_keep.begin(), tris_to_keep.end() );

	// unique doesn't reduce size of the array, so need to 
	//  track the end thing it returns.
	t_end = unique( tris_to_keep.begin(), tris_to_keep.end() );

	tris_to_keep.erase( t_end, tris_to_keep.end() );

#if FUNC_DEBUG
	FDebug("\n");
	FDebug("Tris to keep after unique:  size:  %d\n", tris_to_keep.size() );
	for( i=0; i < tris_to_keep.size(); i++ )
	{
		FDebug("%4.4d   ",  tris_to_keep.at(i) );
		if( (i+1) % 10 == 0 )
			FDebug("\n");		
	}
	FDebug("\n\n");
#endif

	nv = 0;

	// Copy tris into smaller set of indices - do not reallocate
	//  the index array, change only the "last index" m_wNumInd value

	for( i=0; i < tris_to_keep.size(); i++ )
	{
		tribase = ( tris_to_keep.at(i) ) * 3;
		assert( tribase + 2 < m_wNumInd );
		assert( nv+2        < m_wNumInd );

		m_pIndices[nv++] = m_pIndices[ tribase     ];
		m_pIndices[nv++] = m_pIndices[ tribase + 1 ];
		m_pIndices[nv++] = m_pIndices[ tribase + 2 ];
	}

	m_wNumInd = nv;

}


//////////////////////////////////////////////////////////////////
// Create indices for a grid of triangles.  At least 
//  ( nwidth_verts * nheight_verts ) must exist in the vertex data,
//  or else the vertex indices generated will not be valid. 
// Re-allocates indices
//////////////////////////////////////////////////////////////////

void SimpleObject::MakeTris_RegularGrid( UINT nwidth_verts, UINT nheight_verts )
{

	assert( (DWORD)nwidth_verts * (DWORD)nheight_verts <= (DWORD)m_wNumVerts );


	UINT  nw, nh;	

	nw = nwidth_verts;
	nh = nheight_verts;


	AllocateIndices( (nw-1) * (nh-1) * 2 * 3 );

	m_PrimType = D3DPT_TRIANGLELIST;

	////////////////////////////////////////////////////////////////////
	// Set the indices
	int triind;				// triind = index of triangle -> 3 for each
							//   so 0,1,2 = 1st tri, 3,4,5 = 2nd tri
	triind = 0;

	int i,j, nvert, nind;

	nvert = m_wNumVerts;
	nind = m_wNumInd;

	for( j=0; j < nh-1; j++ )
	{
		for( i=0; i < nw-1; i++ )
		{

			if( i + j * ( nw-1 ) >= nvert )
				assert( false );
			if( i + 1 + j * ( nw ) >= nvert )
				assert( false );

			if( triind >= nind )
			{
				FDebug( "bad index calc: %d of %d", triind, nind );
				assert( false );
			}

			
			// stitching pattern is:
			//
			//			  v2 -- v1	      v2
			//				| /		    /  |
			//				|/		   /   |
			//            v0		v0 -- v1

			assert( triind < nind );
			m_pIndices[ triind++ ] = i + j * nw;
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + (j+1) * nw;
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + j * nw;
			assert( triind < nind );

			m_pIndices[ triind++ ] = i + j * nw;
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + (j+1) * nw;
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + (j+1) * nw;
		}
	}



}


//////////////////////////////////////////////////////////////////
//  Specification using a base point and it's two neighbors
//    along the edges of the plane.  
//  Specify base_point position and base_uv texture coordinate
//    Then the counter-clockwise neighbor point postion and UV
//    Then the clockwise neighbor point position and UV
//  
//  For example, base = (-1,-1,0 ), ccw_point = ( 1,-1,0), cw_point = (-1,1,0) 
//

void SimpleObject::InitTesselatedPlane( const D3DXVECTOR3 & base_point, const D3DXVECTOR2 & base_uv,
										 const D3DXVECTOR3 & ccw_point,  const D3DXVECTOR2 & ccw_uv,
										 const D3DXVECTOR3 & cw_point,	 const D3DXVECTOR2 & cw_uv,
										 int n_sub_width, int n_sub_height )
{
	
	int i,j;
	int nvert, nind;

	if( n_sub_height < 0 )
		n_sub_height = 0;
	if( n_sub_width < 0 )
		n_sub_width = 0;

	nvert = (2 + n_sub_width)*(n_sub_height + 2);
	nind = 0;

	Allocate( nvert, nind );

	D3DXVECTOR3 dccw, dcw;
	D3DXVECTOR2 dccwuv, dcwuv;

	dccw = ccw_point - base_point;
	dcw  = cw_point  - base_point;

	dccwuv = ccw_uv - base_uv;
	dcwuv  = cw_uv  - base_uv;


	D3DXVECTOR3 j_pos, i_pos;
	D3DXVECTOR2 j_uv,  i_uv;


	int ind;

	//@@@@@ InitTesselatedPlane -- Need better decision making based on direction of increments!!


	float j_interp, i_interp;

	for( j=0; j < n_sub_height + 2; j++ )
	{
		j_interp = ((float) j ) / ((float) (n_sub_height + 1));

		j_pos = base_point + j_interp * dcw;
		j_uv  = base_uv    + j_interp * dcwuv;

		for(i=0; i < n_sub_width + 2; i++)
		{
			i_interp = ((float) i ) / ((float) (n_sub_width + 1 ));

			i_pos	= i_interp * dccw;
			i_uv	= i_interp * dccwuv;


			ind = i + j * ( n_sub_width + 2 );

			assert( ind < nvert );

			m_pVertices[ ind ].pos = j_pos + i_pos;

			m_pVertices[ ind ].t0 =  j_uv  + i_uv;

		}
	}


	MakeTris_RegularGrid( n_sub_width + 2, n_sub_height + 2 );
 

	GenerateNormalsCCW();


	m_bIsValid = true;
	PostInit();

}



void SimpleObject::InitTesselatedPlane_TriStrip( const D3DXVECTOR3 & width_direction,
												  const D3DXVECTOR3 & height_direction,
												  float width, float height,
												  int n_sub_width, int n_sub_height )
{
	//**** No option for vertex cache priming with
	//  verts from first edge.  Would be not too hard
	//  to add this later...

	assert( n_sub_width >= 0 );
	assert( n_sub_height >= 0 );


	int long_ax, short_ax;


	if( n_sub_height > n_sub_width )
	{
		InitTesselatedPlane( height_direction, width_direction,
							 height, width, n_sub_height, n_sub_width );

		long_ax  = n_sub_height;
		short_ax = n_sub_width;
	}
	else
	{
		InitTesselatedPlane( width_direction, height_direction,
							 width, height, n_sub_width, n_sub_height );

		long_ax  = n_sub_width;
		short_ax = n_sub_height;
	}

	m_PrimType = D3DPT_TRIANGLESTRIP;


	long_ax  += 2;		// number verts
	short_ax += 2;		// number verts

					// + 2 for each subdivision in short axis for
					// degenerate verts to start a new strip
	UINT nind =  ( 2 * long_ax ) * ( short_ax - 1 ) + 2 * ( short_ax - 2 );

	AllocateIndices( nind );


	int i,j;
	int ind;

	ind = 0;

	for( j=0; j < short_ax - 1; j++ )
	{
		for( i=0; i < long_ax; i++ )
		{
			assert( ind < nind );
			m_pIndices[ ind++ ] = i + j * ( long_ax );

			assert( ind < nind );
			m_pIndices[ ind++ ] = i + (j+1) * ( long_ax );
		}

		i--;		// set i back to last vertex we just added
					// to the indices

		if( j < short_ax - 2 )
		{
			// If there's another row of tris on top
			//  then double tap verts to start a new strip

			// hit last vertex again
			assert( ind < nind );
			m_pIndices[ ind++ ] = i + (j+1) * ( long_ax );

			// hit new vertex of next strip
			assert( ind < nind );
			m_pIndices[ ind++ ] = 0 + (j+1) * ( long_ax );
		}
	}
}



void SimpleObject::InitTesselatedPlane( const D3DXVECTOR3 & width_direction, 
										 const D3DXVECTOR3 & height_direction,
										 float width, float height, int n_sub_width, 
										 int n_sub_height )
{

	int i,j;
	int nvert, nind;

	if( n_sub_height < 0 )
		n_sub_height = 0;
	if( n_sub_width < 0 )
		n_sub_width = 0;

	D3DXVECTOR3	width_dir;
	D3DXVec3Normalize( &width_dir, &width_direction );
	D3DXVECTOR3	height_dir;
	D3DXVec3Normalize( &height_dir, &height_direction );


	nvert = (2 + n_sub_width)*(n_sub_height + 2);
	nind = (n_sub_width+1)*(n_sub_height+1)*2 * 3;

	Allocate( nvert, nind );

	m_PrimType = D3DPT_TRIANGLELIST;

	float dx, dy;
	float x,y;

	
	dx =  width  / ( (float) n_sub_width  + 1.0f );
	dy =  height / ( (float) n_sub_height + 1.0f );


	int ind;

	//@@@@ InitTesselatedPlane -- Need better decision making based on direction of increments!!

	y = - height / 2.0f;


	for( j=0; j < n_sub_height + 2; j++ )
	{
		x = - width / 2.0f;

		for(i=0; i < n_sub_width + 2; i++)
		{
			ind = i + j * (n_sub_width+2);

			assert( ind < nvert );


			m_pVertices[ ind ].pos.x = x * width_dir.x + y * height_dir.x;
			m_pVertices[ ind ].pos.y = x * width_dir.y + y * height_dir.y;
			m_pVertices[ ind ].pos.z = x * width_dir.z + y * height_dir.z;

			m_pVertices[ ind ].t0.x = (float) i / (n_sub_width  + 1.0f);
			m_pVertices[ ind ].t0.y = (float) j / (n_sub_height + 1.0f);

			x += dx;
		}

		y += dy;
	}


	MakeTris_RegularGrid( n_sub_width + 2, n_sub_height + 2 );
 

	GenerateNormalsCCW();


	m_bIsValid = true;
	PostInit();


}



void SimpleObject::InitSpiral( const D3DXVECTOR3 & width_direction, 
								const D3DXVECTOR3 & height_direction,
								float width, float height, int n_sub_width, 
								int n_sub_height, float n_twists )
{

	int i,j;
	int nvert, nind;

	if( n_sub_height < 0 )
		n_sub_height = 0;
	if( n_sub_width < 0 )
		n_sub_width = 0;

	D3DXVECTOR3	width_dir;
	D3DXVec3Normalize( &width_dir, &width_direction );
	D3DXVECTOR3	height_dir;
	D3DXVec3Normalize( &height_dir, &height_direction );
	D3DXVECTOR3 length_dir;
	D3DXVec3Cross( &length_dir, &width_dir, &height_dir );



	nvert = (2 + n_sub_width)*(n_sub_height + 2);
	nind = (n_sub_width+1)*(n_sub_height+1)*2 * 3;

	Allocate( nvert, nind );

	m_PrimType = D3DPT_TRIANGLELIST;

	float z;
	float dz, dx;
	float angle;
	float da;
	float radius;

	z = -height / 2.0f;
	angle = 0;
	
	dz = height / (n_sub_height+1.0f);
	da = (float)( n_twists * UL_PI * 2.0f ) / (n_sub_height + 1.0f);

	dx =  width / ( (float)n_sub_width + 1.0f );


	int ind;

	float wt, ht, lt;		// params for each position axis

	for( j=0; j < n_sub_height + 2; j++ )
	{
		for(i=0; i < n_sub_width + 2; i++)
		{
			radius = width/2.0f - dx * i;

			ind = i + j * (n_sub_width+2);

			assert( ind < nvert );

			wt = radius * (float) sin( angle ); // amount of position in width_dir direction
			lt = radius * (float) cos( angle );	// amount of position in height_dir direction
			ht = z;

			m_pVertices[ ind ].pos.x = wt * width_dir.x + ht * height_dir.x + lt * length_dir.x;
			m_pVertices[ ind ].pos.y = wt * width_dir.y + ht * height_dir.y + lt * length_dir.y;
			m_pVertices[ ind ].pos.z = wt * width_dir.z + ht * height_dir.z + lt * length_dir.z;

			m_pVertices[ ind ].t0.x = (float)i/(n_sub_width  +1.0f);
			m_pVertices[ ind ].t0.y = (float)j/(n_sub_height +1.0f);

		}

		z += dz;
		angle += da;
	}

	// Set the indices
	int triind;				// triind = index of triangle -> 3 for each
							//   so 0,1,2 = 1st tri, 3,4,5 = 2nd tri
	triind = 0;

	for( j=0; j < n_sub_height + 1; j++ )
	{
		for( i=0; i < n_sub_width + 1; i++ )
		{

			if( i + j*(n_sub_width + 1) >= nvert )
				assert( false );
			if( i + 1 + j * ( n_sub_width + 2 ) >= nvert )
				assert( false );

			if( triind >= nind )
			{
				FDebug( "bad index calc: %d of %d", triind, nind );
				assert( false );
			}

			
			// stitching pattern is:
			//						           u,v = 1,1
			//			  v2 -- v1	      v2
			//				| /		    /  |
			//				|/		   /   |
			//            v0		v0 -- v1
			// u,v = 0,0

			assert( triind < nind );
			m_pIndices[ triind++ ] = i + j*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + (j+1)*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + j*(n_sub_width+2);
			assert( triind < nind );

			m_pIndices[ triind++ ] = i + j*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + (j+1)*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + (j+1)*(n_sub_width+2);		
		}
	}

	m_bIsValid = true;
	PostInit();
}


//@@@@@ fix to call other initspiral

void SimpleObject::InitSpiral( float width, float height, int n_sub_width,
					int n_sub_height, float n_twists )
{

	int i,j;
	int nvert, nind;

	if( n_sub_height < 0 )
		n_sub_height = 0;
	if( n_sub_width < 0 )
		n_sub_width = 0;


	nvert = (2 + n_sub_width)*(n_sub_height + 2);
	nind = (n_sub_width+1)*(n_sub_height+1)*2 * 3;

	Allocate( nvert, nind );

	m_PrimType = D3DPT_TRIANGLELIST;

	float z;
	float dz, dx;
	float angle;
	float da;
	float radius;

	z = -height / 2.0f;
	angle = 0;
	
	dz = height / (n_sub_height+1.0f);
	da = (float)( n_twists * UL_PI * 2.0f ) / (n_sub_height + 1.0f);

	dx =  width / ( (float)n_sub_width + 1.0f );


	int ind;

	for( j=0; j < n_sub_height + 2; j++ )
	{
		for(i=0; i < n_sub_width + 2; i++)
		{
			radius = width/2.0f - dx * i;

			ind = i + j * (n_sub_width+2);

			assert( ind < nvert );

			m_pVertices[ ind ].pos.x = radius * (float) sin( angle );
			m_pVertices[ ind ].pos.y = radius * (float) cos( angle );
			m_pVertices[ ind ].pos.z = z;

			m_pVertices[ ind ].t0.x = (float)i/(n_sub_width  +1.0f);
			m_pVertices[ ind ].t0.y = (float)j/(n_sub_height +1.0f);

		}

		z += dz;
		angle += da;
	}

	// Set the indices
	int triind;				// triind = index of triangle -> 3 for each
							//   so 0,1,2 = 1st tri, 3,4,5 = 2nd tri
	triind = 0;

	for( j=0; j < n_sub_height + 1; j++ )
	{
		for( i=0; i < n_sub_width + 1; i++ )
		{

			if( i + j*(n_sub_width + 1) >= nvert )
				assert( false );
			if( i + 1 + j * ( n_sub_width + 2 ) >= nvert )
				assert( false );

			if( triind >= nind )
			{
				FDebug( "bad index calc: %d of %d", triind, nind );
				assert( false );
			}

			
			// stitching pattern is:
			//						x,z = 1,1
			//			  v2 -- v1	      v2
			//				| /		    /  |
			//				|/		   /   |
			//            v0		v0 -- v1
			// x,z = 0,0

			assert( triind < nind );
			m_pIndices[ triind++ ] = i + j*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + (j+1)*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + j*(n_sub_width+2);
			assert( triind < nind );

			m_pIndices[ triind++ ] = i + j*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + (j+1)*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + (j+1)*(n_sub_width+2);		
		}
	}

	m_bIsValid = true;
	PostInit();
}





void SimpleObject::_Normalize( V_V3 * pArray )
{
	// Normalize all vec3s in the pArray
	// replacing the old un-normalized with the normalized vector.

	assert( pArray != NULL );

	int i;
	D3DXVECTOR3 pt;

	for( i=0; i < pArray->size(); i++ )
	{
		D3DXVec3Normalize( & (pArray->at(i)), & (pArray->at(i)) );
	}
}



void SimpleObject::_AddToCrossSec( V_V4 * pOut, const D3DXVECTOR4 * pIn,
								    int nvec_in_section, int n_sections,
									bool stitch_section_end,
									int subdiv_cross_section	)
{
	assert( pIn != NULL );
	assert( pOut != NULL );

		// number of original vertices
	int nov = n_sections * nvec_in_section; 

	int i, sec, n;
	int ind;
	ind = 0;

	D3DXVECTOR4 base, d, first, pt;

	pOut->clear();

	/////////////////////////////////////////////////////
	// Build the output array by interleaving new interpolated
	//   points with the original input points.

	for( sec = 0; sec < n_sections; sec++ )
	{
		for( i=0; i < nvec_in_section - 1; i++ )
		{
			assert( ind < nov );
			base = pIn[ind];

			if( i==0 )
				first = base;			// save 1st point for later

			// Difference vector between points
			// ( out, v1, v2 ) out = v1 - v2;

			d.x = pIn[ind+1].x - pIn[ind].x;
			d.y = pIn[ind+1].y - pIn[ind].y;
			d.z = pIn[ind+1].z - pIn[ind].z;
			d.w = pIn[ind+1].w - pIn[ind].w;

			// Dist increment between each newly added point
			d.x = d.x / (float) (subdiv_cross_section + 1.0f);
			d.y = d.y / (float) (subdiv_cross_section + 1.0f);
			d.z = d.z / (float) (subdiv_cross_section + 1.0f);
			d.w = d.w / (float) (subdiv_cross_section + 1.0f);

			pOut->push_back( pIn[ind++] );		// add orig point

			for( n=0; n < subdiv_cross_section; n++ )
			{
				pt.x = base.x + d.x * (float)(n+1.0f);
				pt.y = base.y + d.y * (float)(n+1.0f);
				pt.z = base.z + d.z * (float)(n+1.0f);
				pt.w = base.w + d.w * (float)(n+1.0f);

				pOut->push_back( pt );
			}
		}

		if( stitch_section_end )
		{
			// add the last point of the cross section
			base = pIn[ind];

			assert( ind < nov );
			pOut->push_back( pIn[ind++] );	// add orig point

			// interpolate points between last point of cross section
			//  and first point of cross section
		
			d.x = first.x - base.x;
			d.y = first.y - base.y;
			d.z = first.z - base.z;
			d.w = first.w - base.w;

			// Dist increment between each newly added point
			d.x = d.x / (float) (subdiv_cross_section + 1.0f);
			d.y = d.y / (float) (subdiv_cross_section + 1.0f);
			d.z = d.z / (float) (subdiv_cross_section + 1.0f);
			d.w = d.w / (float) (subdiv_cross_section + 1.0f);

			for( n=0; n < subdiv_cross_section; n++ )
			{
				pt.x = base.x + d.x * (float)(n+1.0f);
				pt.y = base.y + d.y * (float)(n+1.0f);
				pt.z = base.z + d.z * (float)(n+1.0f);
				pt.w = base.w + d.w * (float)(n+1.0f);

				pOut->push_back( pt );
			}
		}
		else
		{
			// add the last point of the cross section
			assert( ind < nov );
			pOut->push_back( pIn[ind++] );	// add orig point
		}
	}
}


void SimpleObject::_AddToCrossSec( V_V3 * pOut, const D3DXVECTOR3 * pIn,
								    int nvec_in_section, int n_sections,
									bool stitch_section_end,
									int subdiv_cross_section  )
{
	assert( pIn != NULL );
	assert( pOut != NULL );

		// number of original vertices
	int nov = n_sections * nvec_in_section; 

	int i, sec, n;
	int ind;
	ind = 0;

	D3DXVECTOR3 base3, d3, first3;

	pOut->clear();

	/////////////////////////////////////////////////////
	// Build the output array by interleaving new interpolated
	//   points with the original input points.

	for( sec = 0; sec < n_sections; sec++ )
	{
		for( i=0; i < nvec_in_section - 1; i++ )
		{
			assert( ind < nov );
			base3 = pIn[ind];

			if( i==0 )
				first3 = base3;			// save 1st point for later

			// Difference vector between points
			d3 = pIn[ind+1] - pIn[ind];

			// dist inc between points
			d3 = d3 / (float) (subdiv_cross_section+1.0f);

			pOut->push_back( pIn[ind++] );	// add orig point

			for( n=0; n < subdiv_cross_section; n++ )
			{
				pOut->push_back( base3 + d3 * (float)(n+1.0f) );
			}
		}

		if( stitch_section_end )
		{
			// add the last point of the cross section
			base3 = pIn[ind];

			assert( ind < nov );
			pOut->push_back( pIn[ind++] );	// add orig point

			// interpolate points between last point of cross section
			//  and first point of cross section
		
			d3 = first3 - base3;
			d3 = d3 / (float) (subdiv_cross_section+1.0f);  // dist inc between points
			for( n=0; n < subdiv_cross_section; n++ )
			{
				pOut->push_back( base3 + d3 * (float)(n+1.0f) );
			}
		}
		else
		{
			// add the last point of the cross section
			assert( ind < nov );
			pOut->push_back( pIn[ind++] );	// add orig point
		}
	}
}



void SimpleObject::_AddCrossSections( V_V3 * pOut, const D3DXVECTOR3 * pIn,
										int nvec_in_section, int n_sections,
										bool stitch_extrusion_ends, int add_cross_sections )
{
	// Interpolates and inserts new positions between cross sections
	//  held in pIn array.
	// nvec_in_section		=	number of verts in each cross section
	// n_sections			=	number of cross sections in pIn array
	// add_cross_sections	=	number of new cross sections to add between each
	//								existing cross section
	// stitch_extrusion_ends =	if true, then no cross sections are added between
	//							the last and first cross sections.

	assert( pIn != NULL );
	assert( pOut != NULL );


	int i, n, nadd;
	int nv = nvec_in_section * n_sections;

	vector < D3DXVECTOR3 >  new_sec;	// new cross section

	int sec0, sec1;		// base indices for cross section points we're interpolating
						//  between
	D3DXVECTOR3 pt;
	float interp;		// interpolation parameter

	pOut->clear();

	for( i=0; i < n_sections; i++ )
	{
		new_sec.clear();

		// Add interpolated vectors to a new cross section
		
		sec0 = i * nvec_in_section;
		sec1 = (i+1) * nvec_in_section;

		if( i+1 == n_sections )
		{
			sec1 = 0;		// wrap back to start, so that
							// interpolation happens between last
							// cross section and first cross section

			// verts between end and start will be deleted later if
			//   the stitch_extrusion_ends is false
		}

		for( nadd = 0; nadd < add_cross_sections; nadd++ )
		{
			interp = (nadd + 1.0f) / ((float) add_cross_sections + 1.0f );

			for( n=0; n < nvec_in_section; n++ )
			{
				// Interpolate and form new section
				// Sections are added one after the other

				// D3DXVec3Lerp( pOut, pV1, pV2, float s )
				// out = V1 + s(V2-V1)

				D3DXVec3Lerp( &pt, & pIn[sec0 + n], & pIn[sec1+n], interp );

				new_sec.push_back( pt );
			}
		}


		// Add verts from the existing section
		for( n=0; n < nvec_in_section; n++ )
		{
			pOut->push_back( pIn[ sec0 + n ] );
		}
		// Add verts from the new cross sections
		for( n=0; n < new_sec.size(); n++ )
		{
			pOut->push_back( new_sec[n] );
		}
	}


	// If not wrapping back to start, delete the last added cross sections
	
	if( ! stitch_extrusion_ends )
	{
		for( n=0; n < new_sec.size(); n++ )
		{
			pOut->pop_back();
		}
	}
}




void SimpleObject::_AddCrossSections( V_V4 * pOut, const D3DXVECTOR4 * pIn,
										int nvec_in_section, int n_sections,
										bool stitch_extrusion_ends, int add_cross_sections )
{
	assert( pIn != NULL );
	assert( pOut != NULL );


	int i, n, nadd;
	int nv = nvec_in_section * n_sections;

	vector < D3DXVECTOR4 >  new_sec;	// new cross section

	int sec0, sec1;		// base indices for cross section points we're interpolating
						//  between
	D3DXVECTOR4 pt;
	float interp;		// interpolation parameter

	pOut->clear();

	for( i=0; i < n_sections; i++ )
	{
		new_sec.clear();

		// Add interpolated vectors to a new cross section
		
		sec0 = i * nvec_in_section;
		sec1 = (i+1) * nvec_in_section;

		if( i+1 == n_sections )
		{
			sec1 = 0;		// wrap back to start

			// verts between end and start will be deleted later if
			//   the stitch_extrusion_ends is false
		}

		for( nadd = 0; nadd < add_cross_sections; nadd++ )
		{
			interp = (nadd + 1.0f) / ((float) add_cross_sections + 1.0f );

			for( n=0; n < nvec_in_section; n++ )
			{
				// Interpolate and form new section
				// Sections are added one after the other

				// D3DXVec3Lerp( pOut, pV1, pV2, float s )
				// out = V1 + s(V2-V1)

				D3DXVec4Lerp( &pt, & pIn[sec0 + n], & pIn[sec1+n], interp );

				new_sec.push_back( pt );
			}
		}


		// Add verts from the existing section
		for( n=0; n < nvec_in_section; n++ )
		{
			pOut->push_back( pIn[ sec0 + n ] );
		}
		// Add verts from the new cross sections
		for( n=0; n < new_sec.size(); n++ )
		{
			pOut->push_back( new_sec[n] );
		}
	}

	// If not wrapping back to start, delete the last added cross sections
	if( ! stitch_extrusion_ends )
	{
		for( n=0; n < new_sec.size(); n++ )
		{
			pOut->pop_back();
		}
	}
}




void SimpleObject::InitExtrusion_Subdivide( D3DXVECTOR3 * pPos, D3DXVECTOR3 * pNrm,
											D3DCOLOR * pCol,
											D3DXVECTOR2 * pTexCoord,
											int num_verts_in_cross_section,
											int num_cross_sections,
											bool stitch_section_end,
											bool stitch_extrusion_ends,
											int subdiv_cross_section,
											int add_cross_sections    )
{
	// Same as InitExtrusion, but adds extra interpolated vertices to the cross 
	//  sections or extra interpolated cross sections
	// subdiv_cross_section = N    will add N vertices between each vertex of a 
	//    cross section.  It does this for all cross sections
	// add_cross_sections = M      will add extra interpolated cross sections to
	//	  the extrusion.  It adds M cross sections after each original cross section.
	//    So use 1 to double the total number of cross sections.

	assert( pPos != NULL );
	assert( subdiv_cross_section >= 0 );
	assert( add_cross_sections >= 0 );


	vector < D3DXVECTOR4 >  v4tmp;		// values promoted to 4D vector for ease of 
										//  working with.  Also used again for final
										//  array of points.
	vector < D3DXVECTOR4 >  v4new;		// New array with interpolated points added

	vector < D3DXVECTOR3 >  v3tmp;
	

	// Final arrays from which the extrude object will be made:
	vector < D3DXVECTOR3 >  np;		// new positions
	vector < D3DXVECTOR3 >  nn;		// new normals
	vector < D3DCOLOR	 >  nc;		// new color, promoted to [0,1] float vec4 for calc
	vector < D3DXVECTOR2 >  nt;		// new tex coords


		// number of original vertices
	int nov = num_cross_sections * num_verts_in_cross_section; 

	int new_nvec_in_section;
	int new_n_sections;

	// Calcs to determine new number of verts in each cross section and in the 
	//  whole thing.  The work of adding is done in the _AddToCrossSec(..) calls
	//  but must be reflected accurately here.  
	new_nvec_in_section = num_verts_in_cross_section * ( subdiv_cross_section + 1 );
	if( ! stitch_section_end )
	{
		new_nvec_in_section -= subdiv_cross_section;
	}
	new_n_sections = num_cross_sections * ( add_cross_sections + 1 );
	if( ! stitch_extrusion_ends )
	{
		new_n_sections -= add_cross_sections;
	}


	////////////////////////////////////////////////////////////////
	// Position cross sections array:
	// Add interpolants to each cross section.
	// No new cross sections added

	_AddToCrossSec( & v3tmp, pPos, num_verts_in_cross_section, num_cross_sections,
								   stitch_section_end, subdiv_cross_section );

	int nnv = new_nvec_in_section * num_cross_sections;
	//FDebug("new vert cnt: %d    should be: %d\n", v3tmp.size(), nnv );


	// Add new cross sections:

	_AddCrossSections( & np, & v3tmp[0], new_nvec_in_section, num_cross_sections,
										stitch_extrusion_ends, add_cross_sections );

	nnv = new_nvec_in_section * new_n_sections;
	//FDebug("New vert cnt: %d   should be: %d\n", np.size(), nnv );


	////////////////////////////////////////////////////////////////
	// Normals cross sections array:
	// Add interpolants to each cross section.
	// No new cross sections added
	// Normalize vector after adding, as the interpolated additions can be
	//   de-normalized

	if( pNrm != NULL )
	{
		_AddToCrossSec( & v3tmp, pNrm, num_verts_in_cross_section, num_cross_sections,
									   stitch_section_end, subdiv_cross_section );
		_Normalize( & v3tmp );

		// Add new cross sections:

		_AddCrossSections( & nn, & v3tmp[0], new_nvec_in_section, num_cross_sections,
											stitch_extrusion_ends, add_cross_sections );
		_Normalize( & nn );

		pNrm = & nn[0];
	}


	////////////////////////////////////////////////////////////////
	// Colors
	// Promote each color to a vec4 for ease of processing

	int i;
	float r,g,b,a;
	DWORD col;

	if( pCol != NULL )
	{
		v4tmp.clear();
		for( i=0; i < nov; i++ )
		{
			a = ((( pCol[i] >> 24 ) & 0xFF ) / 255.0f );
			r = ((( pCol[i] >> 16 ) & 0xFF ) / 255.0f );
			g = ((( pCol[i] >>  8 ) & 0xFF ) / 255.0f );
			b = ((( pCol[i]       ) & 0xFF ) / 255.0f );

			v4tmp.push_back( D3DXVECTOR4(a,r,g,b) );
		}

		// Add interpolants to each cross section.
		_AddToCrossSec( & v4new, & v4tmp[0], num_verts_in_cross_section, num_cross_sections,
											stitch_section_end, subdiv_cross_section );
		// Add new cross sections:
		_AddCrossSections( & v4tmp, & v4new[0], new_nvec_in_section, num_cross_sections,
												stitch_extrusion_ends, add_cross_sections );

		// convert back to DWORD colors
		nc.clear();
		for( i=0; i < nnv; i++ )
		{
			a = v4tmp[i].x;
			r = v4tmp[i].y;
			g = v4tmp[i].z;
			b = v4tmp[i].w;

			col =	(((DWORD)(a * 255.0f)) << 24 ) +
					(((DWORD)(r * 255.0f)) << 16 ) +
					(((DWORD)(r * 255.0f)) <<  8 ) +
					(((DWORD)(b * 255.0f)));

			nc.push_back( col );
		}

		pCol = & nc[0];
	}

	////////////////////////////////////////////////////////////////
	// Texture coords
	// Promote each to a vec4 for ease of processing
	// Add interpolants to each cross section.
	// No new cross sections added

	D3DXVECTOR2 vec2;
	D3DXVECTOR4 vec4;


	if( pTexCoord != NULL )
	{
		v4tmp.clear();
		for( i=0; i < nov; i++ )
		{
			vec4.x = pTexCoord[i].x;
			vec4.y = pTexCoord[i].y;
			vec4.z = 0.0f;
			vec4.w = 0.0f;

			v4tmp.push_back( vec4 );
		}

		// Add interpolants to each cross section.
		_AddToCrossSec( & v4new, & v4tmp[0], num_verts_in_cross_section, num_cross_sections,
											stitch_section_end, subdiv_cross_section );
		// Add new cross sections:
		_AddCrossSections( & v4tmp, & v4new[0], new_nvec_in_section, num_cross_sections,
												stitch_extrusion_ends, add_cross_sections );

		// convert back to DWORD colors
		nt.clear();
		for( i=0; i < nnv; i++ )
		{
			vec2.x = v4tmp[i].x;
			vec2.y = v4tmp[i].y;
			nt.push_back( vec2 );
		}

		pTexCoord = & nt[0];
	}



	InitExtrusion( & np[0], pNrm, pCol, pTexCoord, new_nvec_in_section, new_n_sections,
													stitch_section_end, stitch_extrusion_ends );

}



void SimpleObject::InitLathedObject( D3DXVECTOR3 * pPos, D3DXVECTOR3 * pNrm, D3DCOLOR * pCol,
										D3DXVECTOR2 * pTexCoord,
										int num_verts_in_cross_section,
										D3DXVECTOR3 & lathe_axis,
										float lathe_angle_start,	// in degrees
										float lathe_angle_end,		// in degrees
										int num_cross_sections,		// number in final object
										bool close_cross_section_ends,
										bool stitch_last_cross_section_to_first )
{

	// pPos points to single cross section
	// Other vertices are created for the lathe process
	// lathe center is always zero.
	// num_cross_sections is total number of cross sections.  Note that if 
	//  angle is 360 all the way around, then last cross section will overlap
	//  the first.  To avoid this, set stitch_last_cross_section_to_first = true
	//  and set angle difference to 360 * ( 1 - 1 / num_cross_sections )

	vector < D3DXVECTOR3 >	pos;
	vector < D3DXVECTOR3 >	nrm;
	vector < D3DXVECTOR2 >	tex;
	vector < DWORD >		col;


	assert( pPos != NULL );
	//@@@ This may not handle normals correctly!
	assert( num_cross_sections >= 2 );
	assert( lathe_angle_end >= lathe_angle_start );


	D3DXVECTOR3 * pOutPos = NULL;
	D3DXVECTOR3 * pOutNrm = NULL;
	D3DXVECTOR2 * pOutTex = NULL;
	DWORD		* pOutCol = NULL;


	_AddCrossSections( & pos, pPos, num_verts_in_cross_section, 1,
						true, num_cross_sections - 1 );
	pOutPos = &(pos[0]);


	if( pNrm != NULL )
	{
		_AddCrossSections( & nrm, pNrm, num_verts_in_cross_section, 1,
							true, num_cross_sections - 1 );

		assert( nrm.size() == pos.size() );
		pOutNrm = &(nrm[0]);
	}


	// Rotate each cross section 
	D3DXMATRIX rot;

	int i, n;
	float ang_deg;
	float ang_radians;
	int vind;

	for( i = 0; i < num_cross_sections; i++ )
	{
		ang_deg = lathe_angle_start + ( lathe_angle_end - lathe_angle_start ) * (float) i / ((float)(num_cross_sections-1));
		ang_radians = ang_deg * UL_PI / 180.0f;

		D3DXMatrixRotationAxis( & rot, & lathe_axis, ang_radians );

		for( n=0; n < num_verts_in_cross_section; n++ )
		{
			vind = i * num_verts_in_cross_section + n;
			assert( vind < pos.size() );

			D3DXVec3TransformCoord( & (pos.at(vind)), & (pos.at(vind)), & rot );
		}

		if( pNrm != NULL )
		{
			for( n=0; n < num_verts_in_cross_section; n++ )
			{
				vind = i * num_verts_in_cross_section + n;
				assert( vind < nrm.size() );

				// Just doing rotation - so no need for D3DXVec3TransformNormal()
				D3DXVec3TransformCoord( & (nrm.at(vind)), & (nrm.at(vind)), & rot );
			}
		}
	}

	// duplicate color & tex coords if they exist
	if( pCol != NULL )
	{
		for( i=0; i < pos.size(); i ++ )
		{
			vind = i % num_verts_in_cross_section;

			col.push_back( pCol[ vind ] );
		}
		assert( col.size() == pos.size() );
		pOutCol = &(col[0]);
	}

	if( pTexCoord != NULL )
	{
		for( i=0; i < pos.size(); i ++ )
		{
			vind = i % num_verts_in_cross_section;

			tex.push_back( pTexCoord[vind] );
		}
		assert( tex.size() == pos.size() );
		pOutTex = &(tex[0]);
	}


	// Stitch verts together as an extrusion object to make the 
	//  lathed shape

	InitExtrusion( pOutPos, pOutNrm, pOutCol, pOutTex, 
					num_verts_in_cross_section, num_cross_sections,
					close_cross_section_ends, stitch_last_cross_section_to_first );


	D3DXVECTOR3 center;
	float		radius;

	ComputeBoundingSphere( & center, & radius );
	assert( radius > 0.0f );

	//@@ init lathed object - really want to eliminate degenerate verts?  probably NOT
	RemoveDegenerateVerts( radius / 10000.0f );

	EliminateDegenerateTris();

	pos.clear();
	nrm.clear();
	tex.clear();
	col.clear();
}






void SimpleObject::InitCylinder(	D3DXVECTOR3 & end_cap1_center,
									D3DXVECTOR3 & end_cap2_center,
									float radius, int num_sides,
									int num_cap1_subdiv, int num_length_subdiv,
									int num_cap2_subdiv )
{
	assert( num_cap1_subdiv >= 0 );
	assert( num_cap2_subdiv >= 0 );
	assert( num_length_subdiv >= 0 );

	vector< D3DXVECTOR3 > pos;

	D3DXVECTOR3	axis = end_cap2_center - end_cap1_center;

	int nvecs = 3;
	D3DXVECTOR3 vecs[3];
	vecs[0] = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	vecs[1] = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	vecs[2] = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 cv[3];
	float	f[3];
	float	max = 0.0f;
	int		nmax;

	int i;
	for( i=0; i < nvecs; i++ )
	{
		D3DXVec3Cross( &(cv[i]), &axis, &(vecs[i]) );
		f[i] = D3DXVec3Length( &(cv[i]) );
		if( f[i] > max )
		{
			max = f[i];
			nmax = i;
		}
	}

	// given which vector had max cross product, use it to 
	//  construct othogonal basis.

	D3DXVec3Normalize( &(cv[nmax]), &(cv[nmax]) );

	D3DXVECTOR3 orth1;
	D3DXVec3Cross( &orth1, &(cv[nmax]), &axis );
	D3DXVec3Normalize( &orth1, &orth1 );

	// orth1	gives direction for x coordinate parameter of cylinder
	// cv[nmax] gives direction for y axis
	// axis		gives direction for z axis.

	vector< D3DXVECTOR3 >  posparam;
	float px, pz;

	for( i=0; i < num_cap1_subdiv + 2; i++ )
	{
		px = ((float)i) / ((float) num_cap1_subdiv + 1.0f);
		pz = 0.0f;

		posparam.push_back( D3DXVECTOR3( px, 0.0f, pz ));
	}
	
	// i=1 to not duplicate corner point
	for( i=1; i < num_length_subdiv + 2; i++ )
	{
		pz = ((float)i) / ((float) num_length_subdiv + 1.0f);
		px = 1.0f;

		posparam.push_back( D3DXVECTOR3( px, 0.0f, pz ));
	}

	for( i = num_cap2_subdiv + 0; i >= 0; i-- )
	{
		px = ((float)i) / ((float) num_cap2_subdiv + 1.0f);
		pz = 1.0f;

		posparam.push_back( D3DXVECTOR3( px, 0.0f, pz ));
	}

	// Now convert position parameters to coordinates using
	//  the orthogonal basis vectors
	D3DXVECTOR3 position;

	for( i=0; i < posparam.size(); i++ )
	{
		position =  ( posparam.at(i).x ) * radius * orth1;
		position += posparam.at(i).z * axis;

		pos.push_back( position );
	}

	// Now create lathed object based on cross section
	D3DXVec3Normalize( &axis, &axis );

	InitLathedObject( &(pos[0]), NULL, NULL, NULL, pos.size(), axis,
						0.0f, 360.0f * ( 1.0f - 1.0f/((float)num_sides )),
						num_sides,
						false,		// don't close cross section ends
						true );		// stitch last to first
		

	Translate( end_cap1_center.x, end_cap1_center.y, end_cap1_center.z );

}



void SimpleObject::InitExtrusion( D3DXVECTOR3 * pPos, D3DXVECTOR3 * pNrm, D3DCOLOR * pCol,
								   D3DXVECTOR2 * pTexCoord,
									int num_verts_in_cross_section,
									int num_cross_sections,
									bool stitch_section_end, bool stitch_extrusion_ends )
{
	//
	// Would be better to create a tri strip instead of an indexed tri list =)
	//
	// stitch_section_end:		make tris to close each cross section into a tube
	// stitch_extrusion_ends:	stitch the first cross section to the last to close
	//							  a tube or wrap a ribbon back to its start
	//
	// pPos			Required
	// pNrm			Not required
	// pCol			Not required
	// pTexCoord	Not required
	//
	// For the vertex attributes, specify one cross section after another, so if
	//  num_verts_in_cross_section = 4, then pPos[0]..[3] are positions for first 
	//  cross section, pPos[4]..[7] are positions for 2nd cross section
	// All cross sections must have the same number of vertices!


	int i,j;
	int nvert, nind;

	assert( num_verts_in_cross_section > 1 );
	assert( num_cross_sections > 1 );
	assert( pPos != NULL );
	// Doesn't need to have pNrm, pCol, pTexCoord specified

	nvert = num_verts_in_cross_section * num_cross_sections;

	int		nquad_sec, nsecs_to_stitch;		// number of quads per section, 
											//  number of sections to stitch

	nquad_sec =  num_verts_in_cross_section - 1 ;	

	if( stitch_section_end )
	{
		// Then you get 2 more tris in each section
		nquad_sec += 1;
	}

	nsecs_to_stitch = num_cross_sections - 1;
	if( stitch_extrusion_ends )
	{
		// Then there is one more section to connect at the end
		nsecs_to_stitch += 1;
	}

	nind = nquad_sec * nsecs_to_stitch * 3 * 2;


	// allocate memory for vertices and indices
	Allocate( nvert, nind );

	m_PrimType = D3DPT_TRIANGLELIST;	// Could optimize to create tri
										//  strips, but not today!

	
	for( i=0; i < nvert; i++ )
	{
		m_pVertices[i].pos = pPos[i];
	}

	if( pNrm != NULL )
	{
		for( i=0; i < nvert; i++ )
		{
			m_pVertices[i].nrm = pNrm[i];
		}
	}
	if( pCol != NULL )
	{
		for( i=0; i < nvert; i++ )
		{
			m_pVertices[i].diffuse = pCol[i];
		}
	}
	if( pTexCoord != NULL )
	{
		for( i=0; i < nvert; i++ )
		{
			m_pVertices[i].t0 = pTexCoord[i];
		}
	}


	///////////////////////////////////////////////////////////
	// Stitch the tris 


	int ind;			// index location to fill with a vertex index
	int v1, v2, v3;		// vertex indices of a triangle

	UINT jplus, iplus;	// j+1, i+1 wrapped back to previous vertices if the
						//  various stitch_ bools are true

	
	ind = 0;

	for( j=0; j < nsecs_to_stitch; j++ )
	{
		for( i=0; i < nquad_sec; i++ )
		{

			// Find the coords of the next vertices to form the tri

			iplus = i + 1;
			if( iplus == num_verts_in_cross_section )
				iplus = 0;
			jplus = j + 1;
			if( jplus == num_cross_sections )
				jplus = 0;

			// CCW winding order
			v1 = j		* num_verts_in_cross_section		+ i;
			v2 = jplus	* num_verts_in_cross_section		+ i;
			v3 = j		* num_verts_in_cross_section		+ iplus;

			assert( v1 < nvert && v1 >= 0 );
			assert( v2 < nvert && v2 >= 0 );
			assert( v3 < nvert && v3 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;

			// second triangle of the pair
			v1 = v3;
			v2 = v2;
			v3 = jplus * num_verts_in_cross_section	+ iplus;

			assert( v3 < nvert && v3 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;
		}
	}

	//////////////////////////////////////////


	m_bIsValid = true;

	PostInit();

}





void SimpleObject::InitTorusWrapped( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings )
{
	// Creates a torus "spun" about the z axis.
	// This one has no overlapping vertices on the seams,
	//  so you must use texture coordinate wrapping when 
	//  drawing it.

	UINT i,j;
	int nvert, nind;

	if( Sides < 3 )
		Sides = 3;
	if( Rings < 3 )
		Rings = 3;
	if( InnerRadius > OuterRadius )
	{
		float tmp = InnerRadius;
		InnerRadius = OuterRadius;
		OuterRadius = InnerRadius;
	}

	nvert = Sides * Rings;
	nind = 3 * Sides * 2 * Rings;		// # of triangle vertex indices

	// allocate memory for vertices
	Allocate( nvert, nind );


	m_PrimType = D3DPT_TRIANGLELIST;	// Could optimize to create tri
										//  strips, but not today!


	// Fill in vertex data

	// phi is angle around z axis
	// theta is angle around each ring

	float d_phi = 2.0f * (float)UL_PI / ((float)Rings);
	float d_theta = 2.0f * (float)UL_PI / ((float)Sides);

	float cent_radius = (InnerRadius + OuterRadius) / 2.0f;
	float ring_radius = (OuterRadius - InnerRadius) / 2.0f;

	float angle;


	SimpleObjVertex * pRing = (SimpleObjVertex*) calloc( Sides, sizeof( SimpleObjVertex));
	NULLASSERT( pRing, "SimpleObject:: Can't allocate a ring of vertices!", true );


	// make the first ring - centered about cent_radius;
	for( i=0; i < Sides; i++ )
	{
		angle = 2.0f * (float)UL_PI * (float)i / (float)Sides;

		pRing[i].pos.x = ring_radius * ((float)cos( angle ));
		pRing[i].pos.z = ring_radius * ((float)sin( angle ));
		pRing[i].pos.y = 0.0f;

		pRing[i].pos.x += cent_radius;

		// Set normals
		pRing[i].nrm.x = (float) cos( angle );
		pRing[i].nrm.z = (float) sin( angle );
		pRing[i].nrm.y = 0.0f;

		pRing[i].t0.x = (float)i / (float)Sides;
	}

	int vind;
	float nx, ny, nz, length;
	vind = 0;

	for( j=0; j < Rings; j++ )
	{
		// rotate each ring into position
		//  around the sweep angle
		
		angle = 2.0f * (float)UL_PI * (float)j / (float) Rings;

		for( i=0; i < Sides; i++, vind++ )
		{
			assert( vind < nvert );
			m_pVertices[ vind ].pos.x = pRing[i].pos.x * ((float)cos( angle ));
			m_pVertices[ vind ].pos.y = pRing[i].pos.x * ((float)sin( angle ));
			m_pVertices[ vind ].pos.z = pRing[i].pos.z;

			// rotate normals into position
			nx = m_pVertices[ vind ].nrm.x = pRing[i].nrm.x * ((float)cos(angle));
			ny = m_pVertices[ vind ].nrm.y = pRing[i].nrm.x * ((float)sin(angle));
			nz = m_pVertices[ vind ].nrm.z = pRing[i].nrm.z;

			// normalize normals.  Shouldn't have to in this case, but
			//  just a safety measure.
			length = (float) sqrt(nx*nx + ny*ny + nz*nz);
			assert( length > 0.0f );

			m_pVertices[ vind ].nrm.x = nx / length;
			m_pVertices[ vind ].nrm.y = ny / length;
			m_pVertices[ vind ].nrm.z = nz / length;

			// Set texture coords:
			m_pVertices[ vind ].t0.x = pRing[i].t0.x;
			m_pVertices[ vind ].t0.y = (float) j / (float) Rings;
		}
	}

	///////////////////////////////////////////
	// Now make indices to form triangles

	int ind;			// index location to fill with a vertex index
	int v1, v2, v3;		// vertex indices of a triangle
	UINT jplus, iplus;	// j+1, i+1 wrapped back to 0 if > Rings or Sides, respectively


	ind = 0;

	for( j=0; j < Rings; j ++ )
	{
		for( i=0; i < Sides; i++ )
		{
			// first triangle of each "square" patch of surface
			//  on the torus

			iplus = i + 1;
			if( iplus == Sides )
				iplus = 0;
			jplus = j + 1;
			if( jplus == Rings )
				jplus = 0;


			// CCW winding order
			v1 = j		* Sides		+ i;
			v2 = j		* Sides		+ iplus;
			v3 = jplus	* Sides		+ i;


			assert( v1 < nvert && v1 >= 0 );
			assert( v2 < nvert && v2 >= 0 );
			assert( v3 < nvert && v3 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;

			// second triangle of the pair
			v1 = v3;
			v2 = v2;
			v3 = jplus * Sides	+ iplus;

			assert( v3 < nvert && v1 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;

		}
	}



	//////////////////////////////////////////

	if( pRing != NULL )
	{
		free( pRing );
		pRing = NULL;
	}

	m_bIsValid = true;
	PostInit();

}


/*
void SimpleObject::InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings,
								float TexCoordUMax, float TexCoordVMax )
{

	// Create a torus with user-adjustable scaling of texture coords so a 
	//  texture can repeat several times around the torus

	InitTorus( InnerRadius, OuterRadius, Sides, Rings );

	int i;

	for( i=0; i < m_wNumVerts; i ++ )
	{
		m_pVertices[i].t0.x *= TexCoordUMax;
		m_pVertices[i].t0.y *= TexCoordVMax;
	}

	m_bIsValid = true;
	PostInit();

}

  */

void SimpleObject::InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings,
								const D3DXVECTOR3 & axis1,
								const D3DXVECTOR3 & axis2,
								const D3DXVECTOR3 & axis3	)
{
	// Creates a torus "spun" about the 3rd axis.
	// With this non-wrapped model, vertices overlap at the seams.  
	// These overlapped vertices have appropriate texture coords so no
	//  change in tex coord wrapping mode is necessary.

	// There is no constraint saying tha the axes must be orthonormal
	// or consist of normalized vectors. 

	// *** This function will create incorrect normals if the three
	// axis vectors are not orthonormal or consist of non-normalized vectors


	UINT i,j;
	int nvert, nind;

	if( Sides < 3 )
		Sides = 3;
	if( Rings < 3 )
		Rings = 3;
	if( InnerRadius > OuterRadius )
	{
		float tmp = InnerRadius;
		InnerRadius = OuterRadius;
		OuterRadius = InnerRadius;
	}

	nvert = ( Sides + 1 ) * ( Rings + 1 );
	nind = 3 * Sides * 2 * Rings;		// # of triangle vertex indices

	// allocate memory for vertices
	Allocate( nvert, nind );


	m_PrimType = D3DPT_TRIANGLELIST;	// Could optimize to create tri
										//  strips, but not today!


	// Fill in vertex data

	// phi is angle around z axis
	// theta is angle around each ring

	UINT  nphi_verts;
	UINT  ntheta_verts;

	nphi_verts = Rings + 1;
	ntheta_verts = Sides + 1;

	float d_phi = 2.0f * (float)UL_PI / ((float)(nphi_verts - 1) );
	float d_theta = 2.0f * (float)UL_PI / ((float)(ntheta_verts - 1));

	float cent_radius = (InnerRadius + OuterRadius) / 2.0f;
	float ring_radius = (OuterRadius - InnerRadius) / 2.0f;

	float angle;


	SimpleObjVertex * pRing = (SimpleObjVertex*) calloc( ntheta_verts, sizeof( SimpleObjVertex));
	NULLASSERT( pRing, "SimpleObject:: Can't allocate a ring of vertices!", true );


	// make the first ring - centered about cent_radius;
	for( i=0; i < ntheta_verts; i++ )
	{
		angle = (float) i * d_theta;

		pRing[i].pos.x = ring_radius * ((float)cos( angle ));
		pRing[i].pos.z = ring_radius * ((float)sin( angle ));
		pRing[i].pos.y = 0.0f;

		pRing[i].pos.x += cent_radius;

		// Set normals
		pRing[i].nrm.x = (float) cos( angle );
		pRing[i].nrm.z = (float) sin( angle );
		pRing[i].nrm.y = 0.0f;

		pRing[i].t0.x = (float) i / (float)(ntheta_verts - 1);
	}

	int vind;
	float nx, ny, nz, length;
	vind = 0;

	for( j=0; j < nphi_verts; j++ )
	{
		// rotate each ring into position
		//  around the sweep angle
		
		angle = (float) j * d_phi;

		for( i=0; i < ntheta_verts; i++, vind++ )
		{
			assert( vind < nvert );
			m_pVertices[ vind ].pos.x = pRing[i].pos.x * ((float)cos( angle ));
			m_pVertices[ vind ].pos.y = pRing[i].pos.x * ((float)sin( angle ));
			m_pVertices[ vind ].pos.z = pRing[i].pos.z;

			// rotate normals into position
			nx = m_pVertices[ vind ].nrm.x = pRing[i].nrm.x * ((float)cos(angle));
			ny = m_pVertices[ vind ].nrm.y = pRing[i].nrm.x * ((float)sin(angle));
			nz = m_pVertices[ vind ].nrm.z = pRing[i].nrm.z;

			// normalize normals.  Shouldn't have to in this case, but
			//  just a safety measure.
			length = (float) sqrt(nx*nx + ny*ny + nz*nz);
			assert( length > 0.0f );

			m_pVertices[ vind ].nrm.x = nx / length;
			m_pVertices[ vind ].nrm.y = ny / length;
			m_pVertices[ vind ].nrm.z = nz / length;

			// Set texture coords:
			m_pVertices[ vind ].t0.x = pRing[i].t0.x;
			m_pVertices[ vind ].t0.y = (float) j / (float)( nphi_verts - 1);
		}
	}

	///////////////////////////////////////////
	// Now make indices to form triangles

	int ind;			// index location to fill with a vertex index
	int v1, v2, v3;		// vertex indices of a triangle
	UINT jplus, iplus;	// j+1, i+1 wrapped back to 0 if > Rings or Sides, respectively


	ind = 0;

	for( j=0; j < Rings; j ++ )
	{
		for( i=0; i < Sides; i++ )
		{
			// first triangle of each "square" patch of surface
			//  on the torus

			iplus = i + 1;
			jplus = j + 1;

			// CCW winding order
			v1 = j		* ntheta_verts		+ i;
			v2 = jplus	* ntheta_verts		+ i;
			v3 = j		* ntheta_verts		+ iplus;


			assert( v1 < nvert && v1 >= 0 );
			assert( v2 < nvert && v2 >= 0 );
			assert( v3 < nvert && v3 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;

			// second triangle of the pair
			v1 = v3;
			v2 = v2;
			v3 = jplus * ntheta_verts	+ iplus;

			assert( v3 < nvert && v1 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;
		}
	}


	D3DXVECTOR3 param;
	D3DXVECTOR3 d1, d2, d3;

	d1.x = axis1.x; d1.y = axis2.x; d1.z = axis3.x;
	d2.x = axis1.y; d2.y = axis2.y; d2.z = axis3.y;
	d3.x = axis1.z; d3.y = axis2.z; d3.z = axis3.z;


	for( i=0; i < m_wNumVerts; i++ )
	{
		param = m_pVertices[ i ].pos;

		// rotate positions into new axis
		m_pVertices[ i ].pos.x = D3DXVec3Dot( & param, & d1 );
		m_pVertices[ i ].pos.y = D3DXVec3Dot( & param, & d2 );
		m_pVertices[ i ].pos.z = D3DXVec3Dot( & param, & d3 );

		// rotate normals into position
		param = m_pVertices[ i ].nrm;

		m_pVertices[ i ].nrm.x = D3DXVec3Dot( & param, & d1 );
		m_pVertices[ i ].nrm.y = D3DXVec3Dot( & param, & d2 );
		m_pVertices[ i ].nrm.z = D3DXVec3Dot( & param, & d3 );
	}


	//////////////////////////////////////////

	if( pRing != NULL )
	{
		free( pRing );
		pRing = NULL;
	}

	m_bIsValid = true;
	PostInit();

}



void SimpleObject::InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings )
{
	// Creates a torus "spun" about the z axis.
	// With this non-wrapped model, vertices overlap at the seams.  
	// These overlapped vertices have appropriate texture coords so no
	//  change in tex coord wrapping mode is necessary.


	UINT i,j;
	int nvert, nind;

	if( Sides < 3 )
		Sides = 3;
	if( Rings < 3 )
		Rings = 3;
	if( InnerRadius > OuterRadius )
	{
		float tmp = InnerRadius;
		InnerRadius = OuterRadius;
		OuterRadius = InnerRadius;
	}

	nvert = ( Sides + 1 ) * ( Rings + 1 );
	nind = 3 * Sides * 2 * Rings;		// # of triangle vertex indices

	// allocate memory for vertices
	Allocate( nvert, nind );


	m_PrimType = D3DPT_TRIANGLELIST;	// Could optimize to create tri
										//  strips, but not today!


	// Fill in vertex data

	// phi is angle around z axis
	// theta is angle around each ring

	UINT  nphi_verts;
	UINT  ntheta_verts;

	nphi_verts = Rings + 1;
	ntheta_verts = Sides + 1;

	float d_phi = 2.0f * (float)UL_PI / ((float)(nphi_verts - 1) );
	float d_theta = 2.0f * (float)UL_PI / ((float)(ntheta_verts - 1));

	float cent_radius = (InnerRadius + OuterRadius) / 2.0f;
	float ring_radius = (OuterRadius - InnerRadius) / 2.0f;

	float angle;


	SimpleObjVertex * pRing = (SimpleObjVertex*) calloc( ntheta_verts, sizeof( SimpleObjVertex));
	NULLASSERT( pRing, "SimpleObject:: Can't allocate a ring of vertices!", true );


	// make the first ring - centered about cent_radius;
	for( i=0; i < ntheta_verts; i++ )
	{
		angle = (float) i * d_theta;

		pRing[i].pos.x = ring_radius * ((float)cos( angle ));
		pRing[i].pos.z = ring_radius * ((float)sin( angle ));
		pRing[i].pos.y = 0.0f;

		pRing[i].pos.x += cent_radius;

		// Set normals
		pRing[i].nrm.x = (float) cos( angle );
		pRing[i].nrm.z = (float) sin( angle );
		pRing[i].nrm.y = 0.0f;

		pRing[i].t0.x = (float) i / (float)(ntheta_verts - 1);
	}

	int vind;
	float nx, ny, nz, length;
	vind = 0;

	for( j=0; j < nphi_verts; j++ )
	{
		// rotate each ring into position
		//  around the sweep angle
		
		angle = (float) j * d_phi;

		for( i=0; i < ntheta_verts; i++, vind++ )
		{
			assert( vind < nvert );
			m_pVertices[ vind ].pos.x = pRing[i].pos.x * ((float)cos( angle ));
			m_pVertices[ vind ].pos.y = pRing[i].pos.x * ((float)sin( angle ));
			m_pVertices[ vind ].pos.z = pRing[i].pos.z;

			// rotate normals into position
			nx = m_pVertices[ vind ].nrm.x = pRing[i].nrm.x * ((float)cos(angle));
			ny = m_pVertices[ vind ].nrm.y = pRing[i].nrm.x * ((float)sin(angle));
			nz = m_pVertices[ vind ].nrm.z = pRing[i].nrm.z;

			// normalize normals.  Shouldn't have to in this case, but
			//  just a safety measure.
			length = (float) sqrt(nx*nx + ny*ny + nz*nz);
			assert( length > 0.0f );

			m_pVertices[ vind ].nrm.x = nx / length;
			m_pVertices[ vind ].nrm.y = ny / length;
			m_pVertices[ vind ].nrm.z = nz / length;

			// Set texture coords:
			m_pVertices[ vind ].t0.x = pRing[i].t0.x;
			m_pVertices[ vind ].t0.y = (float) j / (float)( nphi_verts - 1);
		}
	}

	///////////////////////////////////////////
	// Now make indices to form triangles

	int ind;			// index location to fill with a vertex index
	int v1, v2, v3;		// vertex indices of a triangle
	UINT jplus, iplus;	// j+1, i+1 wrapped back to 0 if > Rings or Sides, respectively


	ind = 0;

	for( j=0; j < Rings; j ++ )
	{
		for( i=0; i < Sides; i++ )
		{
			// first triangle of each "square" patch of surface
			//  on the torus

			iplus = i + 1;
			jplus = j + 1;

			// CCW winding order
			v1 = j		* ntheta_verts		+ i;
			v2 = jplus	* ntheta_verts		+ i;
			v3 = j		* ntheta_verts		+ iplus;


			assert( v1 < nvert && v1 >= 0 );
			assert( v2 < nvert && v2 >= 0 );
			assert( v3 < nvert && v3 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;

			// second triangle of the pair
			v1 = v3;
			v2 = v2;
			v3 = jplus * ntheta_verts	+ iplus;

			assert( v3 < nvert && v1 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;
		}
	}



	//////////////////////////////////////////

	if( pRing != NULL )
	{
		free( pRing );
		pRing = NULL;
	}

	m_bIsValid = true;
	PostInit();

}


void SimpleObject::InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings,
								float TexCoordUMax, float TexCoordVMax )
{

	// Create a torus with user-adjustable scaling of texture coords so a 
	//  texture can repeat several times around the torus

	InitTorus( InnerRadius, OuterRadius, Sides, Rings );

	int i;

	for( i=0; i < m_wNumVerts; i ++ )
	{
		m_pVertices[i].t0.x *= TexCoordUMax;
		m_pVertices[i].t0.y *= TexCoordVMax;
	}

	m_bIsValid = true;
	PostInit();

}



void SimpleObject::InitAddClone( SimpleObject * pObj )
{
	// Add a copy of the input so a single complex object
	//  can be built from many simple objects.
	// Translate pObj to the right object space position before
	//  calling this funtion.  
	// After calling this function, you will not be able to 
	//  distinguish the parts which came from pObj from the 
	//  rest of the model.

	int orig_nind = m_wNumInd;
	int orig_nvert = m_wNumVerts;

	ASSERT_AND_RET_IF_NULL( pObj );
	if( orig_nvert + pObj->m_wNumVerts >= 65536 )
	{
		assert( false );
		return;
	}

	AllocateMoreIndices( pObj->m_wNumInd + m_wNumInd );
	AllocateMorePositions( pObj->m_wNumVerts + m_wNumVerts );

	int i;

	// copy pObj's indices, accounting for the
	//  position where the new vertices will end up 
	for( i = orig_nind; i < m_wNumInd; i++ )
	{
		m_pIndices[i] = pObj->m_pIndices[ i - orig_nind ] + (UINT)orig_nvert;
	}

	// copy pObj's vertices
	for( i = orig_nvert; i < m_wNumVerts; i++ )
	{
		m_pVertices[i] = pObj->m_pVertices[ i - orig_nvert ];
	}
}


void SimpleObject::InitClone( SimpleObject * pObj )
{
	// copy of the input object

	assert( pObj != NULL );
	assert( pObj->m_bIsValid );

	Allocate( pObj->GetNumVerts(), pObj->GetNumIndices() );

	memcpy( m_pVertices, pObj->m_pVertices, sizeof( SimpleObjVertex ) * m_wNumVerts );
	memcpy( m_pIndices,  pObj->m_pIndices,  sizeof( UINT ) * m_wNumInd );

	m_PrimType = pObj->m_PrimType;
	m_bIsValid = pObj->m_bIsValid;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////////////
//  Adds noise only in positive direction of vNoiseDir
//  To get noise from, for example -1 to 1, you have to call this twice:
//    once with -1 and once with 1

void SimpleObject::AddPositionNoise( D3DXVECTOR3 vNoiseDir, float noise_magnitude )
{
	
	int numvert = m_wNumVerts;

	D3DXVECTOR3 noise;

	for( int ind = 0; ind < numvert; ind++ )
	{
		noise = vNoiseDir * noise_magnitude * ( ((float)rand())/(float)RAND_MAX );
		
		m_pVertices[ ind ].pos.x += noise.x;
		m_pVertices[ ind ].pos.y += noise.y;
		m_pVertices[ ind ].pos.z += noise.z;		
	}
}


void SimpleObject::AddPositionNoiseGrid( D3DXVECTOR3 vNoiseDir, float noise_magnitude,
										  int grid_nx, int nsmooth )
{
	// Add noise to the positions with possibility for averaging the noise
	//   values with their nearest neighbors as though they lie on a grid 
	//   of x dimension grid_nx.
	// Function does a crude compensation if the number of vertices doesn't fill
	//   out the last line of the imposed grid.
	// noise_magnitude is max magnitude that will be added - the noise field
	//   is scaled so the max value is that.

	int numvert = m_wNumVerts;

	int gridx = grid_nx;
	int gridy = m_wNumVerts/gridx;


	assert( gridx > 0 );
	assert( gridy >= 0 );

	if( gridy * gridx != m_wNumVerts )
	{
		FDebug("SimpleObject -- AddPositionNoiseGrid Vertices not a rectangular grid!\n");
		FDebug("    Compensating for remaining verts...\n");

		gridy++;
	}

	D3DXVECTOR3 noise;

	float * noisemag = (float*) calloc( numvert, sizeof( float ));
	assert( noisemag != NULL );

	int ind;

	for( ind = 0; ind < numvert; ind++ )
	{
		noisemag[ind] = ( ((float)rand())/(float)RAND_MAX );
	}

	// now smooth the noise according to the grid
	int i,j, n;
	int ind0, ind1;

	float avg;
	float maxmag;
	float minmag;

	for( n=0; n < nsmooth; n++ )
	{
		maxmag = 0.0f;
		minmag = FLT_MAX;

		for( j=0; j < gridy; j++ )
			for( i=0; i < gridx; i++ )
			{	
				ind0 = j*gridx + i;
				
				if( ind0 > numvert-1 )
				{
					FDebug("broke on:  %d    %d\n", i,j );
					break;
				}

				avg = noisemag[ind0];


				ind1 = ind0 + 1;

				if( ind1 > numvert-1 )
					ind1 = numvert-1;
				avg += noisemag[ind1];


				ind1 = ind0 + gridx;

				if( ind1 > numvert-1 )
				{
					ind1 = i;
				}
				avg += noisemag[ind1];


				ind1 = ind0 + gridx + 1;

				if( ind1 > numvert-1 )
				{
					ind1 = i + 1;
				}
				avg += noisemag[ind1];

				noisemag[ ind0 ] = avg / 4.0f;

				if( noisemag[ ind0 ] > maxmag )
				{
					maxmag = noisemag[ ind0 ];
				}
				if( noisemag[ ind0 ] < minmag )
				{
					minmag = noisemag[ ind0 ];
				}
			}
	}

	// scale the noise values so their max is noise_magnitude
	float scale;
	maxmag = maxmag - minmag;

	if( maxmag > 0.0f )
		scale = noise_magnitude / maxmag;
	else
		assert( false );		// something's gone wrong in the calcs

	for( i=0; i < numvert; i++ )
	{
		noisemag[i] = ( noisemag[i] - minmag ) * scale;
	}


	for( ind = 0; ind < numvert; ind++ )
	{
		noise = vNoiseDir * noisemag[ind];
		
		m_pVertices[ ind ].pos.x += noise.x;
		m_pVertices[ ind ].pos.y += noise.y;
		m_pVertices[ ind ].pos.z += noise.z;		
	}


	free( noisemag );
}


void SimpleObject::AddDoubleTapsToFirstAndLast()
{
	// For strips, so we can combine several
	// strips into one obj

	assert( m_PrimType == D3DPT_TRIANGLESTRIP );

	UINT * pInd;
	UINT   nInd;

	DuplicateIndices( &pInd, &nInd );

	AllocateIndices( m_wNumInd + 2 );	// one index at start, one at end

	int i, ind;
	
	ind = 0;
	m_pIndices[0]			= pInd[0];
	m_pIndices[m_wNumInd-1] = pInd[nInd-1];

	assert( m_wNumInd == nInd + 2 );

	for( i=0; i < nInd; i++ )
	{
		m_pIndices[i+1] = pInd[i];
	}

	FreeDuplicateIndices( &pInd );
}



void SimpleObject::MultiplyTexCoords( float ufactor, float vfactor )
{

	UINT i;

	for( i=0; i < m_wNumVerts; i++ )
	{
		m_pVertices[i].t0.x *= ufactor;
		m_pVertices[i].t0.y *= vfactor;
	}
}


void SimpleObject::ExplodeFaces( float distance )
{
	// Move faces out along vector from
	//  center of object
	// Does not re-compute face normals

	D3DXVECTOR3 center;

	ComputeObjectCenter( & center );

	int i;
	D3DXVECTOR3 face_center;
	int i1, i2, i3;
	D3DXVECTOR3 vec_from_center;

	// extrude each vertex of each face outward along vector from 
	//  center of object to center of face

	D3DXVECTOR3  * pDispl = new D3DXVECTOR3[ m_wNumVerts ];
	assert( pDispl != NULL );
	memset( pDispl, 0, sizeof( D3DXVECTOR3 ) * m_wNumVerts );

	for( i=0; i < m_wNumInd; i += 3 )
	{
		i1 = m_pIndices[i];
		i2 = m_pIndices[i+1];
		i3 = m_pIndices[i+2];
		
		face_center = ( m_pVertices[i1].pos + m_pVertices[i2].pos + m_pVertices[i3].pos ) / 3.0f;

		vec_from_center = face_center - center;
		D3DXVec3Normalize( &vec_from_center, &vec_from_center );

		pDispl[ i1 ] += vec_from_center;
		pDispl[ i2 ] += vec_from_center;
		pDispl[ i3 ] += vec_from_center;
	}

	for( i=0; i < m_wNumVerts; i ++ )
	{
		D3DXVec3Normalize( & pDispl[i], & pDispl[i] );
		pDispl[i] *= distance;

		m_pVertices[i].pos += pDispl[i];
	}

	SAFE_ARRAY_DELETE( pDispl );
}


void SimpleObject::ComputeTriCenter( D3DXVECTOR3 * pResult, UINT tri_index )
{
	assert( pResult != NULL );
	tri_index *= 3;
	assert( tri_index + 2  < m_wNumInd );

	*pResult  = m_pVertices[ m_pIndices[ tri_index + 0 ]].pos;
	*pResult += m_pVertices[ m_pIndices[ tri_index + 1 ]].pos;
	*pResult += m_pVertices[ m_pIndices[ tri_index + 2 ]].pos;

	*pResult /= 3.0f;
}


void SimpleObject::ComputeObjectCenter( D3DXVECTOR3 * pCenter )
{
	// average of all point positions.  This is not the
	//  center of the object's bounding volume

	assert( pCenter != NULL );

	int i;
	pCenter->x = pCenter->y = pCenter->z = 0.0f;

	for( i=0; i < m_wNumVerts; i++ )
	{
		*pCenter += m_pVertices[i].pos;
	}

	*pCenter = *pCenter / ((float)m_wNumVerts );

}

void SimpleObject::ComputeAABBCenter( D3DXVECTOR3 * pCenter )
{
	// compute Axis-Aligned Bounding Box Center
	assert( pCenter != NULL );

	D3DXVECTOR3 pmin, pmax, pavg;

	GetPositionMaxima( &pmax.x, &pmax.y, &pmax.z );
	GetPositionMinima( &pmin.x, &pmin.y, &pmin.z );

	pavg = ( pmin + pmax ) / 2.0f;

	*pCenter = pavg;
}


void SimpleObject::ComputeBoundingSphere( D3DXVECTOR3 * out_center, float * out_radius )
{
	assert( out_center != NULL );

	// center of object extents
	ComputeAABBCenter( out_center );

	// find max radius
	float maxr = -1.0f;
	float rsq;
	D3DXVECTOR3 vec;

	int i;
	for( i=0; i < m_wNumVerts; i++ )
	{
		vec = m_pVertices[i].pos - (*out_center);	
		rsq = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

		if( rsq > maxr )
			maxr = rsq;
	}

	*out_radius = (float) sqrt( maxr );
}



void SimpleObject::ReverseNormals()
{
	// invert all normals

	int i;

	for( i=0; i < m_wNumVerts; i++ )
	{
		m_pVertices[i].nrm = - m_pVertices[i].nrm;
	}
}



void SimpleObject::ReverseWinding()
{
	// change winding for all triangles

	// only knows about reversing trilists for now
	assert( m_PrimType == D3DPT_TRIANGLELIST );


	int i;
	int index;

	// Swap first and second indices for each triangle
	//  to change their windings.

	for( i=0; i < m_wNumInd; i += 3 )
	{
		// += 3 for skipping to next triangle

		index = m_pIndices[ i+1 ];
		m_pIndices[ i+1 ] = m_pIndices[i];
		m_pIndices[i]     = index;
	}
}


void SimpleObject::SetColorFromVertexNormal()
{
	RET_IF_NULL( m_pVertices );
	UINT ind;
	D3DXVECTOR3 nrm;
	float hlf = 0.5f;
	int r, g, b;

	for( ind = 0; ind < m_wNumVerts; ind ++ )
	{
		nrm.x = m_pVertices[ind].nrm.x * hlf + hlf;
		nrm.y = m_pVertices[ind].nrm.y * hlf + hlf;
		nrm.z = m_pVertices[ind].nrm.z * hlf + hlf;
		r = (int) (nrm.x * 255.0f);
		g = (int) (nrm.y * 255.0f);
		b = (int) (nrm.z * 255.0f);
		m_pVertices[ind].diffuse = D3DCOLOR_ARGB( 0, r, g, b );
	}
}


void SimpleObject::SetVertexColors( DWORD ARGB_color )
{
	RET_IF_NULL( m_pVertices );
	UINT ind;
	for( ind = 0; ind < m_wNumVerts; ind ++ )
	{
		m_pVertices[ind].diffuse = ARGB_color;
	}
}


void SimpleObject::Translate( float x, float y, float z )
{
	int numvert = m_wNumVerts;
	int ind;

	for( ind = 0; ind < numvert; ind ++ )
	{
		m_pVertices[ind].pos.x += x;		
		m_pVertices[ind].pos.y += y;		
		m_pVertices[ind].pos.z += z;		
	}
}

void SimpleObject::Scale( float x, float y, float z )
{
	int numvert = m_wNumVerts;
	int ind;

	for( ind = 0; ind < numvert; ind ++ )
	{
		m_pVertices[ind].pos.x *= x;		
		m_pVertices[ind].pos.y *= y;		
		m_pVertices[ind].pos.z *= z;		
	}	
}


void SimpleObject::TranslateXMinToZero()
{
	int numvert = m_wNumVerts;

	float minx = FLT_MAX;
	int ind;

	for( ind = 0; ind < numvert; ind++ )
	{
		if( m_pVertices[ ind ].pos.x < minx )
			minx = m_pVertices[ ind ].pos.x;
	}

	for( ind = 0; ind < numvert; ind++ )
	{
		m_pVertices[ ind ].pos.x -= minx;
	}
}


UINT	SimpleObject::GetNumTris()
{
	UINT nind = GetNumIndices();
	UINT ntri;

	switch( m_PrimType )
	{
	case D3DPT_TRIANGLELIST:
		
		ntri = nind / 3;

		break;

	case D3DPT_TRIANGLESTRIP:

		ntri = nind - 2;

		break;
	default:
		assert( false );
		break;
	}

	return( ntri );
}


/////////////////////////////////////////////////
// Fills vec3s with triangle point coords for 
//  triangle number n_tri in the mesh

void	SimpleObject::GetTriVerts( UINT n_tri, D3DXVECTOR3 * pt1,
								    D3DXVECTOR3 * pt2, 
								    D3DXVECTOR3 * pt3 )
{
	assert( n_tri < GetNumTris() );
	assert( pt1 != NULL );
	assert( pt2 != NULL );
	assert( pt3 != NULL );
	
	D3DXVECTOR3 * pp1;
	D3DXVECTOR3 * pp2;
	D3DXVECTOR3 * pp3;
	

	switch( m_PrimType )
	{
	case D3DPT_TRIANGLELIST:
		
		pp1 = & m_pVertices[ m_pIndices[ n_tri * 3     ]].pos;
		pp2 = & m_pVertices[ m_pIndices[ n_tri * 3 + 1 ]].pos;
		pp3 = & m_pVertices[ m_pIndices[ n_tri * 3 + 2 ]].pos;

		break;

	case D3DPT_TRIANGLESTRIP:

		pp1 = & m_pVertices[ m_pIndices[ n_tri     ]].pos;
		pp2 = & m_pVertices[ m_pIndices[ n_tri + 1 ]].pos;
		pp3 = & m_pVertices[ m_pIndices[ n_tri + 2 ]].pos;

		break;
	default:
		assert( false );
		break;
	}

	*pt1 = *pp1;
	*pt2 = *pp2;
	*pt3 = *pp3;
}



void SimpleObject::GetPositionMinima( float * px, float * py, float * pz )
{
	ComputePositionMinAndMax( px, py, pz, NULL, NULL, NULL );
}



void SimpleObject::ComputePositionMinAndMax( float * xmin, float * ymin, float * zmin,
											 float * xmax, float * ymax, float * zmax )
{
	float xmn, ymn, zmn;
	float xmx, ymx, zmx;

	int numvert = m_wNumVerts;
	int ind;

	xmn = xmx = m_pVertices[0].pos.x;
	ymn = ymx = m_pVertices[0].pos.y;
	zmn = zmx = m_pVertices[0].pos.z;

	for( ind = 1; ind < numvert; ind++ )
	{
		if( m_pVertices[ ind ].pos.x > xmx )
			xmx = m_pVertices[ ind ].pos.x;

		if( m_pVertices[ ind ].pos.y > ymx )
			ymx = m_pVertices[ ind ].pos.y;

		if( m_pVertices[ ind ].pos.z > zmx )
			zmx = m_pVertices[ ind ].pos.z;

		if( m_pVertices[ ind ].pos.x < xmn )
			xmn = m_pVertices[ ind ].pos.x;

		if( m_pVertices[ ind ].pos.y < ymn )
			ymn = m_pVertices[ ind ].pos.y;

		if( m_pVertices[ ind ].pos.z < zmn )
			zmn = m_pVertices[ ind ].pos.z;
	}

	if( xmin != NULL )
		*xmin = xmn;
	if( ymin != NULL )
		*ymin = ymn;
	if( zmin != NULL )
		*zmin = zmn;

	if( xmax != NULL )
		*xmax = xmx;
	if( ymax != NULL )
		*ymax = ymx;
	if( zmax != NULL )
		*zmax = zmx;
}


void SimpleObject::GetPositionMaxima( float * px, float * py, float * pz )
{
	ComputePositionMinAndMax( NULL, NULL, NULL, px, py, pz );
}



void SimpleObject::CalculateNormalsCCW()
{
	RET_IF_NULL( m_pIndices );
	RET_IF_NULL( m_pVertices );

	if( m_PrimType != D3DPT_TRIANGLELIST )
	{
		FDebug("Error: primitive type must be D3DPT_TRIANGLELIST\n");
		assert( false );
	}


	int i;
	int vi0, vi1, vi2;	    // vertex indices of triangle

	// zero normals for accumulation
	for( i=0; i < m_wNumVerts; i++ )
	{
		m_pVertices[i].nrm.x = 0.0f;
		m_pVertices[i].nrm.y = 0.0f;
		m_pVertices[i].nrm.z = 0.0f;
	}

	// find each triangle's normal and add into the 
	//   vertex normal

	D3DXVECTOR3 v0, v1, v2;
	D3DXVECTOR3 d0, d1, nrm;

	i=0;
	while( i < m_wNumInd )
	{
		vi0 = m_pIndices[i++];		
		vi1 = m_pIndices[i++];		
		vi2 = m_pIndices[i++];		

		CalculateFaceNormalCCW( m_pVertices[vi0],
								m_pVertices[vi1],
								m_pVertices[vi2],
								& nrm				);

		//  accumulate into vertex normals:

		m_pVertices[ vi0 ].nrm += nrm;
		m_pVertices[ vi1 ].nrm += nrm;
		m_pVertices[ vi2 ].nrm += nrm;
	}

	// normalize the normals
	for( i=0; i < m_wNumVerts; i++ )
	{
		D3DXVec3Normalize( &( m_pVertices[i].nrm ), &( m_pVertices[i].nrm ) );
	}
}


void SimpleObject::GenerateNormalsCCW()
{
	// Generate normals for counter-clockwise right-handed coordinate
	//   system models

	CalculateNormalsCCW();
}

void SimpleObject::GenerateNormalsCW()
{
	CalculateNormalsCCW();

	UINT i;
	for( i=0; i < m_wNumVerts; i++ )
	{
		m_pVertices[i].nrm = - m_pVertices[i].nrm;
	}
}







void SimpleObject::Draw( NV_D3DDEVICETYPE * pD3DDev )
{
	assert( pD3DDev != NULL );

	FDebug("SimpleObject::  Non-VertexBuffer rendering!  This will be slow!\n");

#ifdef NV_USING_D3D8

	switch( m_PrimType )
	{
	case D3DPT_TRIANGLELIST:

		pD3DDev->DrawIndexedPrimitiveUP(	m_PrimType,
											0,				// min index
											m_wNumVerts,
											m_wNumInd/3,	// prim count
											m_pIndices,
											D3DFMT_INDEX16,
											m_pVertices,
											sizeof( SimpleObjVertex ) );


	case D3DPT_TRIANGLESTRIP:

		pD3DDev->DrawIndexedPrimitiveUP(	m_PrimType,
											0,				// min index
											m_wNumVerts,
											m_wNumInd - 2,	// prim count
											m_pIndices,
											D3DFMT_INDEX16,
											m_pVertices,
											sizeof( SimpleObjVertex ) );

		break;
	default:
		assert( false );
		break;
	}

#else
	FMsg("No implementation of SimpleObject::Draw for other than D3D8!\n");
	assert( false );

#endif

}


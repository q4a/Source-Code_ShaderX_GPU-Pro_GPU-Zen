/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\FogPolygonVolumes
File:  PolygonalVolumeFogScene.cpp

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


#include <shared/NV_Common.h>
#include <shared/NV_Error.h>

#include "NVD3DVers.h"
#include <NV_D3DCommon/SimpleObject.h>
#include <NV_D3DCommon/ShadowVolumeObject.h>
#include <NV_D3DCommon/SimpleVBObject.h>
#include <NV_D3DCommon/ShadowVolumeVBObject.h>
#include <NV_D3DCommon/NoiseGrid3D.h>
#include <NV_D3DCommon/NVBModelLoader.h>

#include "PolygonalVolumeFogScene.h"
#include "PolygonalVolumeFog.h"

#include <shared/NVBScene9.h>


#define PLY_NAME		"HappyBuddha/happy_vrip_res4.ply"

#define NVBSCENE_FILE	"TEST/VolumeFogBeast.nvb"

#define TEX_BLOB		"textures/2D/NVIDIA_TestMaps/NVTestMap_00_Rainbow256DXT3.dds"
#define TEXTGEO_NAME	"lump001.txt"

#define TEX_FOGCOLORRAMP	"textures/2D/Gradients/VolumeFogRamp_01.tga"
#define TEX_DEFAULT_BASE	"textures/2D/d3d_set1/TerrainBase.tga"


using namespace std;


//#define PVFS_TRACE1		FMsg
#define PVFS_TRACE1		NullFunc


///////////////////////////////////////////////////////////////
// Forward declarations

void InitTerrain( SimpleObject * pObj );



///////////////////////////////////////////////////////////////

PolygonalVolumeFogScene::PolygonalVolumeFogScene()
{
	PVFS_TRACE1( "PolygonalVolumeFogScene::PolygonalVolumeFogScene()\n" );

	SetInstanceVarsToNull();
}

PolygonalVolumeFogScene::~PolygonalVolumeFogScene()
{
	PVFS_TRACE1( "PolygonalVolumeFogScene::~PolygonalVolumeFogScene()\n" );

	Free();
	SetInstanceVarsToNull();
}


void PolygonalVolumeFogScene::SetInstanceVarsToNull()
{
	m_ppMat_ViewProject			= NULL;
	m_ppAmbient					= NULL;
	m_ppAmbTerrain				= NULL;

	m_ppExtrusionOrigin1		= NULL;

	m_ppBlobGeo					= NULL;
	m_ppBlobMaster				= NULL;
	m_ppBlobNoise				= NULL;

	m_ppDitherControl			= NULL;
	m_ppDitherOff				= NULL;
	m_ppDitherOn				= NULL;

	m_ppTexBlob					= NULL;
	m_ppTexDefaultBase			= NULL;
	m_ppTexFogColorRamp			= NULL;

}


HRESULT PolygonalVolumeFogScene::Free()
{
	PVFS_TRACE1( "PolygonalVolumeFogScene::Free()\n" );

	HRESULT hr = S_OK;

	FreeMasterLists();

	SceneFactory::Free();

	return( hr );
}


HRESULT PolygonalVolumeFogScene::FreeInstanceLists()
{
	PVFS_TRACE1( "PolygonalVolumeFogScene::FreeInstanceLists()\n" );

	HRESULT hr = S_OK;

	m_ActiveObjects.Free();

	m_objStandInSolids.Free();
	m_objFogVolumesForOnOffControl.Free();
	m_objLightBeam.Free();
	m_objSimpleScene.Free();
	m_objOtherBlobs.Free();

	m_vppNegativeObjRotation.clear();
	m_vppNegativeObjTrans.clear();
	m_vpNegativeObj.clear();

	SceneFactory::FreeInstanceLists();

	SetInstanceVarsToNull();

	return( hr );
}


HRESULT	PolygonalVolumeFogScene::FreeMasterLists()
{
	PVFS_TRACE1( "PolygonalVolumeFogScene::FreeMasterLists()\n" );

	HRESULT hr = S_OK;

	hr = FreeInstanceLists();

	MASTERLIST_DELETE( m_vML_ObjFogVolumes );
	MASTERLIST_DELETE( m_vML_ObjExtrusionVolumes );
	MASTERLIST_DELETE( m_vML_ObjSolids );
	MASTERLIST_DELETE( m_vML_SVVBObjects );

	SceneFactory::FreeMasterLists();

	return( hr );	
}


////////////////////////////////////////////////////////////////////

HRESULT	PolygonalVolumeFogScene::Initialize( NV_D3DDEVICETYPE * pDev,
										PolygonalVolumeFogScene::ObjectCollectionType objtype,
										GetFilePathCallback file_path_callback )
{
	HRESULT hr = S_OK;
	
	Free();

	SceneFactory::Initialize( pDev, file_path_callback );

	// frequency and amplitude of blob animation
	m_fNoiseFreq	= 1.81f;
	m_fNoiseAmp		= 1.15f;


	float ra = 0.2f;
	m_ppAmbient = NewVector4( D3DXVECTOR4( ra, ra, ra, 0.0f ) );

	ra = 0.1f;
	m_ppAmbTerrain = NewVector4( D3DXVECTOR4( ra, ra, ra, 0.0f ));


	// Texture to use when rendering the blob as a solid
	FreeTexture( m_ppTexBlob );
	m_ppTexBlob = NewTexture( TEX_BLOB );


	// Texture that maps object thickness to color
	m_ppTexFogColorRamp = NewTexture( TEX_FOGCOLORRAMP );

	m_ppTexDefaultBase	= NewTexture( TEX_DEFAULT_BASE );



	// Specific values of m_ppDitherOn do not matter -- All that is required is that
	//   they DP4 with HCLIP position (oPos) to make some non-zero result for 
	//   turning dither ON.  Dither off is all zeros so the DP4 result is always
	//   zero for no dither.
	m_ppDitherOff	= NewVector4( D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ));
//	m_ppDitherOn	= NewVector4( D3DXVECTOR4( 507.0f, 303.7f, 707.3f, -3.0f ));
//	m_ppDitherOn	= NewVector4( D3DXVECTOR4( 2.0f, 5.0f, 1.0f, 0.0f ));
//	m_ppDitherOn	= NewVector4( D3DXVECTOR4( 67.0f, 111.7f, 187.3f, -3.0f )); // 69,59
	m_ppDitherOn	= NewVector4( D3DXVECTOR4( 9.0f, 23.7f, 33.3f, -3.0f ));

		// This value can be overwritten to control the dithering for
		// any objects which refer to the variable.
	m_ppDitherControl = NewVector4( **m_ppDitherOn );


	switch( objtype )
	{
	case PVFS_NO_OBJECTS:
		hr = S_OK;
		break;

	case PVFS_THRUSTPLUME_OBJECTS :
		hr = InitObjects01_ThrustObjects();
		break;

	case PVFS_HAZEBEAM_OBJECTS :

		hr = InitObjects02_HazeBeamTests();
		hr = InitObjects_Terrain1();

		break;

	case PVFS_OBJECTS_FROMSIMPLEFILE:
		InitObjects03_FromSimpleText();
		break;

	case PVFS_SPHERE_AND_BLOCK :
		hr = InitObjects04_SphereAndBlock();
		break;

	case PVFS_BUDDHA :
		hr = InitObjects05_Buddha();
		break;

	case PVFS_NVBSCENE :
		hr = InitObjects06_NVBScene( NVBSCENE_FILE );
		break;

	default:
		FDebug("unrecognized object collection type\n");
		assert( false );
		hr = E_FAIL;
	}

	return( hr );
}



Object_FogVolume		** PolygonalVolumeFogScene::NewObjectFogVolume()
{
	Object_FogVolume ** ppo = new Object_FogVolume *;
	RET_NULL_IF_NULL( ppo );
	*ppo = new Object_FogVolume;
	RET_NULL_IF_NULL( *ppo );
	m_vML_ObjFogVolumes.push_back( ppo );
	return( ppo );
}

Object_ExtrusionVolume	** PolygonalVolumeFogScene::NewObjectExtrusionVolume()
{
	Object_ExtrusionVolume ** ppo = new Object_ExtrusionVolume *;
	RET_NULL_IF_NULL( ppo );
	*ppo = new Object_ExtrusionVolume;
	RET_NULL_IF_NULL( *ppo );
	m_vML_ObjExtrusionVolumes.push_back( ppo );
	return( ppo );
}

Object_Solid	** PolygonalVolumeFogScene::NewObjectSolid()
{
	Object_Solid ** ppo = new Object_Solid *;
	RET_NULL_IF_NULL( ppo );
	*ppo = new Object_Solid;
	RET_NULL_IF_NULL( *ppo );
	(*ppo)->bRenderOn = true;
	m_vML_ObjSolids.push_back( ppo );
	return( ppo );
}


ShadowVolumeVBObject ** PolygonalVolumeFogScene::NewShadowVolumeVBObject( ShadowVolumeObject * pObj )
{
	// Allocate a pointer to a ShadowVolumeVBObject and a ShadowVolumeVBObject
	ShadowVolumeVBObject ** ppo = new ShadowVolumeVBObject *;
	RET_NULL_IF_NULL( ppo );
	*ppo = new ShadowVolumeVBObject;
	RET_NULL_IF_NULL( *ppo );
	m_vML_SVVBObjects.push_back( ppo );

	if( pObj != NULL )
	{
		RET_NULL_IF_NULL( m_pD3DDev );

		// assumes BuildShadowVolume() has already been called!
		(*ppo)->CreateFromShadowVolumeObject( pObj, m_pD3DDev );
	}
	return( ppo );
}


////////////////////////////////////

void InitSimpleThrust1( SimpleObject * pObj, float radius, int sides, int cap_subdiv, float z_low,
						float z_high, float z_noise_mag )
{

	if( pObj == NULL )
		return;


	assert( z_high > z_low );


	pObj->InitCylinder( D3DXVECTOR3( 0.0f, 0.0f, z_low ), 
						D3DXVECTOR3( 0.0f, 0.0f, z_high ),
						radius, sides,		// radius, sides
						cap_subdiv, 0,		// cap 1 subdivisions, length subdivisions
						0 );				// cap 2 subdivisions


	// Add noise to the end cap 
	float xy_disp = 0.5f;

	// Add noise
	int nv;
	for( nv = 0; nv < pObj->m_wNumVerts; nv++ )
	{
		if( pObj->m_pVertices[nv].pos.z == z_low )
		{
			// displace downward some random amount

			float ra;
			ra = ((float)rand()/ ((float)RAND_MAX));
			ra *= z_noise_mag;

			pObj->m_pVertices[nv].pos.z = pObj->m_pVertices[nv].pos.z + ra;

		}
	}
}


HRESULT	PolygonalVolumeFogScene::InitObjects04_SphereAndBlock()
{
	HRESULT hr = S_OK;	

	// Value of this matrix will be set later by the 
	//   application
	m_ppMat_ViewProject = NewMatrix();

	/////////////////////////////////////////////////////////////
	
	ShadowVolumeObject		* pSVO = NULL;
	SimpleObject			* pSO = NULL;

	ShadowVolumeVBObject	** ppSVVBO;
	SimpleVBObject			** ppSVBO;

	D3DXMATRIX		mat;

	MatrixV					** ppMatv;
	MatrixNode				** ppNode;
	Object_FogVolume		** ppOFogVol;
	Object_ExtrusionVolume	** ppOEV;
	Object_Solid			** ppOSolid;

	pSO = new SimpleObject;
	assert( pSO != NULL );

	pSVO = new ShadowVolumeObject;
	assert( pSVO != NULL );

	/////////////////////////////////////////////////////////////

	float	radius = 1.5f;
	int		sides = 12;
	int		cap_subdiv = 10;
	float	z_low = 4.0f;
	float	z_high = 4.2f;
	float	z_noise_mag = -3.5f;


	pSO->InitSphereFromBox( 1.5f, 10, 10, 10, 
							D3DXVECTOR3( 1.0f, 0.0f, 0.0f ),
							D3DXVECTOR3( 0.0f, 1.0f, 0.0f ),
							D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) );
	pSO->GenerateNormalsCCW();

	pSO->MultiplyTexCoords( 0.0f, 0.0f );

	ppSVBO	= NewSimpleVBObject( pSO );

	// Create Object_<> classes which associate SimpleObject
	//  geometry with matrices and other things for rendering
	
	D3DXMatrixTranslation( &mat, -1.5f, 0.0f, 0.7f );
	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
			// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

// /*
	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 1.0f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );
// */


	/////////////////////////////////////////////////////////
	//  block of fog

	pSO->InitRectangularBlock_Faceted( D3DXVECTOR3( 1.0f, 0.0f, 1.0f ),
										0xFFFFFFFF, 
										D3DXVECTOR3( 2.0f, 0.0f, 0.0f ),
										D3DXVECTOR3( 0.0f, 7.5f, 0.0f ),
										D3DXVECTOR3( 0.0f, 0.0f, 3.0f ) );
	pSO->MultiplyTexCoords( 0.0f, 0.0f );

	ppSVBO	= NewSimpleVBObject( pSO );
	
	D3DXMatrixTranslation( &mat, 0.6f, 0.0f, -1.0f );
	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
			// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );


	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 1.0f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	///////////////////////////////////////////////////
	// solid torus intersecting the volumes

	pSO->InitTorus( 1.3f, 2.0f, 20, 60, 
					D3DXVECTOR3( 1.0f, 0.0f, 0.0f ),
					D3DXVECTOR3( 0.0f, 1.0f, 0.0f ),
					D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) );

	pSO->MultiplyTexCoords( 0.0f, 0.0f );

	ppSVBO	= NewSimpleVBObject( pSO );
	
	D3DXMatrixTranslation( &mat, 0.3f, 0.0f, 0.6f );
	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
			// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

/*
	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 1.0f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );
// */

// /*
	ppOSolid	= NewObjectSolid();
	(*ppOSolid)->ppGeometry			= ppSVBO;
	(*ppOSolid)->ppMatrixNode		= ppNode;
	(*ppOSolid)->ppAmbient			= m_ppAmbient;

	// add to the instance list
	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );
// */

	/////////////////////////////////////////////

	InitObjects_Terrain1();


	/////////////////////////////////////////////////////////////

	SAFE_DELETE( pSVO );
	SAFE_DELETE( pSO );

	ppSVVBO	= NULL;
	pSVO	= NULL;
	ppOFogVol	= NULL;
	ppOEV		= NULL;
	ppMatv		= NULL;
	ppNode		= NULL;
	ppOSolid	= NULL;

	return( hr );


}



HRESULT	PolygonalVolumeFogScene::InitObjects01_ThrustObjects()
{
	// Creates objects that look roughly like rocket thrust
	// plumes.

	HRESULT hr = S_OK;	

	// Value of this matrix will be set later by the 
	//   application
	m_ppMat_ViewProject = NewMatrix();

	/////////////////////////////////////////////////////////////
	
	ShadowVolumeObject		* pSVO = NULL;
	SimpleObject			* pSO = NULL;

	ShadowVolumeVBObject	** ppSVVBO;
	SimpleVBObject			** ppSVBO;

	D3DXMATRIX		mat;

	MatrixV					** ppMatv;
	MatrixNode				** ppNode;
	Object_FogVolume		** ppOFogVol;
	Object_ExtrusionVolume	** ppOEV;
	Object_Solid			** ppOSolid;

	pSO = new SimpleObject;
	assert( pSO != NULL );

	pSVO = new ShadowVolumeObject;
	assert( pSVO != NULL );

	/////////////////////////////////////////////////////////////

	float	radius = 1.5f;
	int		sides = 12;
	int		cap_subdiv = 10;
	float	z_low = 4.0f;
	float	z_high = 4.2f;
	float	z_noise_mag = -3.5f;

	InitSimpleThrust1( pSO, radius, sides, cap_subdiv, z_low, z_high, z_noise_mag );

	ppSVBO	= NewSimpleVBObject( pSO );

	/////////////////////////////////////////////////////////////
	// Create Object_<> classes which associate SimpleObject
	//  geometry with matrices and other things for rendering
	

	D3DXMatrixTranslation( &mat, -2.5, 0.0f, 0.0f );
	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
			// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();

	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 1.0f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	//////////////////////////////////////////////////////////
	// another instance of same geometry with different matrix

	D3DXMatrixTranslation( &mat, 2.5, 0.0f, 0.0f );
	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
			// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();

	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 1.0f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );


	//////////////////////////////////////////////////////////
	// Solid: 
	// Make a solid white end cap for the last volume created
	//  above

	// 0.0 noise, so it makes a flat cylinder
	InitSimpleThrust1( pSO, radius, sides, 0, z_high - 0.05f, z_high + 0.2f, 0.0f );
	pSO->SetVertexColors( 0xFFFFFFFF );		// white

	ppSVBO		= NewSimpleVBObject( pSO );
	ppOSolid	= NewObjectSolid();

	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppMatrixNode	= ppNode;
	(*ppOSolid)->ppAmbient		= NewVector4( D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 0.0f ));	// RGBA

	// add to the instance list
	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );

	//////////////////////////////////////////////////////////
	// Solid
	// Make some simple objects which can occlude the volumes

	float z_up = 1.0f;
	float post_radius = radius * 0.1f;
	// A thin post
	pSO->InitCylinder( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),	// end cap 1 center
						D3DXVECTOR3( 0.0f, 0.0f, z_high), 
						post_radius, 5, 0, 0, 0 );


	// Create another object which will be made of several simple
	//  objects in various positions.

	SimpleObject * pSO2 = new SimpleObject;
	assert( pSO2 != NULL );

	float trans = radius + 0.7f;

	pSO->Translate( 0.0f, trans, z_up );
	pSO2->InitClone( pSO );

	pSO->Translate( 0.0f, -2.0f * trans, 0.0f );						
	pSO2->InitAddClone( pSO );

	pSO->Translate( trans, trans, 0.0f );						
	pSO2->InitAddClone( pSO );

	pSO->Translate( -2.0f * trans, 0.0f, 0.0f );						
	pSO2->InitAddClone( pSO );

	// ring at the top of the posts
	pSO->InitTorus( trans - post_radius, trans + post_radius, 
					4, 4, 	// sides, rings
					D3DXVECTOR3( 1.0f, 0.0f, 0.0f ),
					D3DXVECTOR3( 0.0f, 1.0f, 0.0f ),
					D3DXVECTOR3( 0.0f, 0.0f, 1.0f )  );
	pSO->MultiplyTexCoords( 0.0f, 0.0f );
	pSO->Translate( 0.0f, 0.0f, z_high + z_up );

	pSO2->InitAddClone( pSO );

	ppSVBO		= NewSimpleVBObject( pSO2 );
	ppOSolid	= NewObjectSolid();

	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppMatrixNode	= ppNode;
	(*ppOSolid)->ppAmbient		= m_ppAmbient;

	// add to the instance list
	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );

	SAFE_DELETE( pSO2 );


	//////////////////////////////////////////////////////////
	// some thrust volumes farther away

	InitSimpleThrust1( pSO, radius, sides, 5, 3.2f, z_high, - 2.5f );
	ppSVBO	= NewSimpleVBObject( pSO );

	D3DXMatrixTranslation( &mat, 4.0f, 50.0f, 0.0f );	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 0.1f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	//////////////////////////////////////////////////////
	D3DXMatrixTranslation( &mat, -4.0f, 70.0f, 0.0f );	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 0.3f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	//////////////////////////////////////////////////////
	D3DXMatrixTranslation( &mat, 0.0f, 70.0f, 10.0f );	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 0.5f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	//////////////////////////////////////////////////////
	D3DXMatrixTranslation( &mat, 1.0f, 70.0f, -10.0f );	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 1.0f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	//////////////////////////////////////////////////////
	D3DXMatrixTranslation( &mat, 7.0f, 20.0f, 0.0f );	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 1.0f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	//////////////////////////////////////////////////////
	D3DXMatrixTranslation( &mat, -7.0f, 20.0f, 0.0f );	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 0.5f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	//////////////////////////////////////
	// make an array of them

	int i,j, ni, nj;
	ni = 4;
	nj = 4;
	float xc, yc;
	float sz = 35.0f;
	for( i=0; i < ni; i++ )
	{
		for( j=0; j < ni; j++ )
		{
			xc = sz/2.0f - sz * ((float)i)/(ni-1.0f);
			yc = sz/2.0f - sz * ((float)j)/(nj-1.0f);

			D3DXMatrixTranslation( &mat, xc, 27.0f, yc );

			ppMatv	= NewMatrix( mat );
			ppNode	= NewMatrixNode( ppMatv );
			ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

			ppOFogVol	= NewObjectFogVolume();
			(*ppOFogVol)->ppGeometry		= ppSVBO;
			(*ppOFogVol)->ppMatrixNode	= ppNode;
			(*ppOFogVol)->fDepthScale	= 0.5f - 0.35f * i/(float)ni;
			(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

			// add fog volume object to instance list
			m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );
		}
	}


	/////////////////////////////////////////////////////////////
	// Make a long smooth dim cone with negative objects inside
	// Remember the matrices for the dim objects so they can be
	//  animated

	pSO->InitCylinder( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
						D3DXVECTOR3( 6.0f, 0.0f, 0.0f ),
						2.5f,  14,		// radius, num_sides
						0, 7, 0 );
	// Modify verts by shrinking them as X increases
	float dist;
	for( i=0; i < pSO->m_wNumVerts; i++ )
	{
		dist = pSO->m_pVertices[i].pos.x;
		dist = dist / 6.0f;
		pSO->m_pVertices[i].pos = pSO->m_pVertices[i].pos / ( 1.0f + 1.5f * dist * dist );
		pSO->m_pVertices[i].pos.x = dist * 6.0f;
	}

	ppSVBO	= NewSimpleVBObject( pSO );

	// left 2, back 5, down 1
	MatrixV ** ppMatBase;
	D3DXMatrixTranslation( &mat, -2.0f, 5.0f, -1.0f );	
	ppMatv	= NewMatrix( mat );
	ppMatBase = ppMatv;
	ppNode	= NewMatrixNode( ppMatv );
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppMatrixNode		= ppNode;
	(*ppOFogVol)->fDepthScale		= 0.3f;
	(*ppOFogVol)->ppDitherControl	= m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );

	// make and attach negative objects as extrusion volumes
	// Thin cylinders

	pSVO->InitCylinder( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), D3DXVECTOR3( 0.1f, 0.0f, 0.0f ),
						0.3f, 5, 0, 0, 0 );
	pSVO->BuildShadowVolume( 0.1f );
	ppSVVBO	= NewShadowVolumeVBObject( pSVO );


	//////////
	MatrixV ** ppMatR;
	D3DXMatrixIdentity( &mat );
	ppMatR	= NewMatrix( mat );
	m_vppNegativeObjRotation.push_back( ppMatR );		// remember it so we can animate
	D3DXMatrixTranslation( &mat, 1.0f, 0.5f, 0.5f );
	ppMatv	= NewMatrix( mat );
	m_vppNegativeObjTrans.push_back( ppMatv );		// remember it so we can animate


	ppNode	= NewMatrixNode( ppMatR );				// rotation first
	ppNode	= NewMatrixNode( ppMatv, ppNode );		// translation after rotation
	ppNode	= NewMatrixNode( ppMatBase, ppNode );	// whole object collection translation
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOEV		= NewObjectExtrusionVolume();
	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode		= ppNode;
	(*ppOEV)->fDepthScale		= 0.5f;
	(*ppOEV)->fExDist			= 6.0f;
	(*ppOEV)->fShadowInset		= 0.0f;
	(*ppOEV)->bRenderOn			= true;
	(*ppOEV)->ppExtrusionOriginO = NewVector4( D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f) );
	(*ppOEV)->ppDitherControl = m_ppDitherControl;

	m_vpNegativeObj.push_back( ppOEV );
	m_ActiveObjects.m_vFogExtrusionHoles.push_back( ppOEV );


	//////////
	D3DXMatrixIdentity( &mat );
	ppMatR	= NewMatrix( mat );
	m_vppNegativeObjRotation.push_back( ppMatR );		// remember it so we can animate
	D3DXMatrixTranslation( &mat, 2.0f, 0.0f, 0.0f );
	ppMatv	= NewMatrix( mat );
	m_vppNegativeObjTrans.push_back( ppMatv );		// remember it so we can animate

	ppNode	= NewMatrixNode( ppMatR );				// rotation first
	ppNode	= NewMatrixNode( ppMatv, ppNode );		// translation after rotation
	ppNode	= NewMatrixNode( ppMatBase, ppNode );	// whole object collection translation
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOEV		= NewObjectExtrusionVolume();
	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode		= ppNode;
	(*ppOEV)->fDepthScale		= 0.5f;
	(*ppOEV)->fExDist			= 6.0f;
	(*ppOEV)->fShadowInset		= 0.0f;
	(*ppOEV)->ppExtrusionOriginO = NewVector4( D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f) );
	(*ppOEV)->ppDitherControl = m_ppDitherControl;

	m_vpNegativeObj.push_back( ppOEV );
	m_ActiveObjects.m_vFogExtrusionHoles.push_back( ppOEV );


	/////////////////////////////////////////////////////////////

	SAFE_DELETE( pSVO );
	SAFE_DELETE( pSO );

	ppSVVBO	= NULL;
	pSVO	= NULL;
	ppOFogVol	= NULL;
	ppOEV		= NULL;
	ppMatv		= NULL;
	ppNode		= NULL;
	ppOSolid	= NULL;

	return( hr );
}




void InitTerrain( SimpleObject * pObj )
{
	// Create the ground object

	ASSERT_AND_RET_IF_NULL( pObj );

	float	width, height;
	int		hdiv, vdiv;

	width	= 30.0f;
	height	= 30.0f;
	hdiv	= 100;
	vdiv	= 100;

	width	= 20.0f;
	height	= 20.0f;
	hdiv	= 70;
	vdiv	= 70;


	pObj->InitTesselatedPlane( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ),
								 D3DXVECTOR3( 0.0f, 1.0f, 0.0f ),
								 width, width, hdiv, vdiv );

	pObj->Translate( 5.0f, 5.0f, 0.0f );


	D3DXVECTOR3  vNoiseDir;
	
	vNoiseDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );		// direction of noise perturbation

	// Make a fractalish landscape by adding noise of varying roughness
	// This is what generates the terrain-like geometry 
	//   1st param = direction to offset positions
	//   2nd param = magnitude of max offset
	//   3rd       = sets grid width for smoothing a 2D array of noise
	//				 This is just number of vertices in 1st dimension = m_nHDiv+2
	//   4th       = number of smoothing passes - the more, the smoother
	//
	
	float lsc = 1.0f;
	lsc = 1.5f;

	pObj->AddPositionNoiseGrid( vNoiseDir, lsc* 0.4f,  hdiv+2, 30 );   // smooth big lumps
	pObj->AddPositionNoiseGrid( vNoiseDir, lsc* 0.65f, hdiv+2, 15 );   // less smooth smaller lumps
	pObj->AddPositionNoiseGrid( vNoiseDir, lsc* 0.45f, hdiv+2, 2  );    // short rough lumps


	// Translate so the minimum Z point is at zero
	float px, py, pz;
	float pmx, pmy, pmz;
	pObj->GetPositionMinima( &px, &py, &pz );
	pObj->GetPositionMaxima( &pmx, &pmy, &pmz );
	pObj->Translate( 0.0f, 0.0f, -(pz + pmz)/2.0f );


	pObj->ReverseWinding();
	pObj->GenerateNormalsCCW();

	pObj->SetVertexColors( 0x00FFFFFF );
}



void	CreateMasterBlob( SimpleObject * pSO )
{
	// Unperturbed blob in system memory that serves as 
	//  the starting point for the vertex animation.

	float or = 2.0f;
	int sides = 20;

	pSO->InitSphereFromBox( or, sides, sides, sides,
								D3DXVECTOR3( 1.0f, 0.0f, 0.0f ),
								D3DXVECTOR3( 0.0f, 1.0f, 0.0f ),
								D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) );

	pSO->RemoveDegenerateVerts( 0.00001f );
	pSO->Translate( 0.0f, 0.0f, 0.6f );

}


void PolygonalVolumeFogScene::AnimateBlob01( float time_in_seconds )
{
	RET_IF_NULL( m_ppBlobMaster );
	RET_IF_NULL( m_ppBlobNoise );
	RET_IF_NULL( m_ppBlobGeo );
	RET_IF_NULL( *m_ppBlobMaster );
	RET_IF_NULL( *m_ppBlobNoise );
	RET_IF_NULL( *m_ppBlobGeo );


	// translate spatial noise according to total time
	float tx = time_in_seconds * 0.5f;


	// Initialize noise generator with amplitudes and frequencies

	GridNoiseComponent::FilterType filt;
	filt = GridNoiseComponent::FilterType::GNFT_TRILINEAR;

	vector< GridNoiseComponent > vNoiseComps;

	float or = 2.0f;

	// freq, amp, seed, filter
	vNoiseComps.push_back( GridNoiseComponent( m_fNoiseFreq,  m_fNoiseAmp, 1, filt ) );


	// add up the amplitudes of all noise components
	float mag = 0.0f;
	int i;
	for( i=0; i < vNoiseComps.size(); i++ )
	{
		mag += vNoiseComps.at(i).amplitude;
	}

	NoiseGrid3D noisegrid;
	float off = 0.25f;
	D3DXVECTOR3 noi;
	D3DXVECTOR3 mag_off( mag/2.0f, mag/2.0f, mag/2.0f );

	// calculate noise values based on vertex position and
	//  and add them to each vertex position
	for( i=0; i < (*m_ppBlobMaster)->m_wNumVerts; i++ )
	{
		D3DXVECTOR3 pos = (*m_ppBlobMaster)->m_pVertices[i].pos;

		noisegrid.NoiseVec3D( noi.x, noi.y, noi.z,
								pos.x + tx,
								pos.y,
								pos.z - tx * 0.3f,
								vNoiseComps );

		pos = pos + noi - mag_off;

		(*m_ppBlobNoise)->m_pVertices[i].pos = pos;
	}

	// This does keeps a system memory copy of the animated vertex data.
	// Could improve it by copying the data directly to the vertex buffer object
	// Performance on 2.6 GHz machine is not limited by this animation.

	if( m_ppBlobGeo != NULL )
	{
		// Update the dynamic vertex buffer blob object from
		//  the system memory copy.
		(*m_ppBlobGeo)->UpdateVerticesFromSimpleObject( *m_ppBlobNoise );
	}

}



HRESULT PolygonalVolumeFogScene::InitObjects_Terrain1()
{
	// Call this after InitObjects02_HazeBeamTests()

	HRESULT hr = S_OK;

	ShadowVolumeObject		* pSVO		= NULL;
	SimpleObject			* pSO		= NULL;

	ShadowVolumeVBObject	** ppSVVBO	= NULL;
	SimpleVBObject			** ppSVBO	= NULL;

	D3DXMATRIX		mat;

	MatrixNode				** ppNode;
	Object_Solid			** ppOSolid;

	pSO = new SimpleObject;
	assert( pSO != NULL );

	////////////////////////////////////////////////////

	// quick way to easily change negative volumes to positive
	//  if you want to.
	vector< Object_ExtrusionVolume ** > * pNegativeExtrudeDest;
	pNegativeExtrudeDest = & m_ActiveObjects.m_vFogExtrusionHoles;
//	pNegativeExtrudeDest = & m_ActiveObjects.m_vFogExtrusionVolumes;

	////////////////////////////////////////////////////


	InitTerrain( pSO );

	ppSVBO	= NewSimpleVBObject( pSO );

	ppNode	= NewMatrixNode( m_ppMat_ViewProject );

	ppOSolid = NewObjectSolid();

	(*ppOSolid)->ppAmbient		= m_ppAmbTerrain;
	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppMatrixNode	= ppNode;


	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );


	////////////////////////////////////////////////////
	SAFE_DELETE( pSO );
	ppOSolid	= NULL;

	return( hr );
}




HRESULT PolygonalVolumeFogScene::LoadPLY( const char * filename,
											SimpleObject * pToThisObject )
{
	HRESULT hr = S_OK;

	FAIL_IF_NULL( filename );
	FAIL_IF_NULL( pToThisObject );

	FMsg("Loading [%s]\n", filename );

	FILE * fp = NULL;
	fp = fopen( filename, "rt" );
	FAIL_IF_NULL( fp );

	bool readingheader = true;
	char buf[4096];
	UINT num_verts;
	UINT num_inds;

	while( readingheader )
	{
		fscanf( fp, "%s", buf );

		if( strcmp( buf, "element" ) == 0 )
		{
			fscanf( fp, "%s", buf );

			if( strcmp( buf, "vertex" ) == 0 )
			{
				fscanf( fp, "%u", & num_verts );
				FMsg("num verts: %u\n", num_verts );
			}
			if( strcmp( buf, "face" ) == 0 )
			{
				fscanf( fp, "%u", & num_inds );
				num_inds = num_inds * 3;
				FMsg("num faces: %u\n", num_inds / 3 );
			}
		}

		if( strcmp( buf, "end_header" ) == 0 )
		{
			readingheader = false;
			FMsg("Done reading header\n");
		}
	}


	pToThisObject->Allocate( num_verts, num_inds );


	UINT i;
	FMsg("Reading vertices: ");
	for( i=0; i < num_verts; i++ )
	{
		fscanf( fp, "%f", & ( pToThisObject->m_pVertices[i].pos.x ) );
		fscanf( fp, "%f", & ( pToThisObject->m_pVertices[i].pos.y ) );
		fscanf( fp, "%f", & ( pToThisObject->m_pVertices[i].pos.z ) );

		if( i % (num_verts/10) == 0 )
		{
			FMsg("%u  ", i );
		}
	}
	FMsg("\n");

	FMsg("Reading indices: ");
	int numind;
	UINT uitmp;

	for( i=0; i < num_inds / 3; i++ )
	{
		fscanf( fp, "%d", & numind );
		
		if( numind != 3 )
		{
			FMsg("Bad num indices in a triangle!!\n");

			fscanf( fp, "%u", & uitmp );
			FMsg("%u ", uitmp );
			fscanf( fp, "%u", & uitmp );
			FMsg("%u ", uitmp );
			fscanf( fp, "%u", & uitmp );
			FMsg("%u ", uitmp );
			fscanf( fp, "%u", & uitmp );
			FMsg("%u ", uitmp );
			FMsg("\n");
			assert( false );
			return( E_FAIL );
		}

		fscanf( fp, "%u", & (pToThisObject->m_pIndices[i*3]) );
		fscanf( fp, "%u", & (pToThisObject->m_pIndices[i*3+1]) );
		fscanf( fp, "%u", & (pToThisObject->m_pIndices[i*3+2]) );

		if( i % (num_inds/10) == 0 )
		{
			FMsg("%u  ", i );
		}
	}
	FMsg("\n");


	fclose( fp );
	fp = NULL;

	return( hr );
}


HRESULT PolygonalVolumeFogScene::InitObjects06_NVBScene( const char * filepath )
{
	FAIL_IF_NULL( filepath );
	HRESULT hr = S_OK;

	// Value of this matrix will be set later by the 
	//   application
	m_ppMat_ViewProject = NewMatrix();

	m_ppExtrusionOrigin1 = NewVector4( D3DXVECTOR4( 5.0f, 2.0f, 20.0f, 1.0f ));	// value also set later
	m_ppExtrusionOrigin1 = NewVector4( D3DXVECTOR4( 4.0f, 20.0f, 0.0f, 1.0f ));	// value also set later


	ShadowVolumeObject		* pSVO;
	ShadowVolumeVBObject	** ppSVVBO;
	SimpleVBObject			** ppSVBO;

	MatrixV					** ppMatv;
	MatrixNode				** ppNode;
	Object_Solid			** ppOSolid;
	Object_ExtrusionVolume	** ppOEV;

	pSVO = new ShadowVolumeObject;
	assert( pSVO != NULL );

	/////////////////////////////////////////////////////////////

	// quick way to easily change negative volumes to positive
	//  if you want to.
	vector< Object_ExtrusionVolume ** > * pNegativeExtrudeDest;
	pNegativeExtrudeDest = &m_ActiveObjects.m_vFogExtrusionHoles;
//	pNegativeExtrudeDest = &m_ActiveObjects.m_vFogExtrusionVolumes;

	//////////////////////////////////////////////////////

	NVBModelLoader loader;

	loader.LoadNVBFile( filepath, m_pGetFilePathCallback );

	FMsg("NVBModelLoader reports %d meshes\n", loader.GetNumMeshes() );

	SimpleObject so;

	loader.LoadMeshToSimpleObject( 0, so );

	so.ReverseNormals();
	so.ReverseWinding();
	so.Scale( 0.01f, 0.01f, 0.01f );

	float minx, miny, minz;
	float maxx, maxy, maxz;
	so.GetPositionMinima( &minx, &miny, &minz );
	so.GetPositionMaxima( &maxx, &maxy, &maxz );
	so.Translate( -(minx+maxx)/2.0f, -miny, -(minz+maxz)/2.0f );

	ppSVBO	= NewSimpleVBObject( & so );

	ppOSolid = NewObjectSolid();

	D3DXMATRIX mat, rot;
	D3DXMatrixIdentity( &mat );
	// negative y reverses the handed-ness and facing of the tris
	D3DXMatrixScaling( &mat, 1.00f, 1.000f, 1.00f );
	D3DXMatrixRotationAxis( &rot, & D3DXVECTOR3( 1.0f, 0.0f, 0.0f ), 3.14159f / 2.0f );
	D3DXMatrixMultiply( &mat, &rot, &mat );

	ppMatv	= NewMatrix( mat );					// identity matrix
	ppNode	= NewMatrixNode( ppMatv );
	MatrixNode ** ppWorld = ppNode;
			// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );


	(*ppOSolid)->ppAmbient		= NewVector4( D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ));
	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppTexture		= NULL;
	(*ppOSolid)->bRenderOn		= true;
	(*ppOSolid)->ppMatrixNode	= ppNode;

	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );

	//////////////

	ShadowVolumeObject svo;
	svo.InitClone( & so );

	svo.BuildShadowVolume( 0.9f );

	ppSVVBO	= NewShadowVolumeVBObject( &svo );
	ppOEV	= NewObjectExtrusionVolume();

	(*ppOEV)->ppWorldMatrixNode = ppWorld;
	
	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode	= ppNode;

	(*ppOEV)->fDepthScale	= 1.0f;
	(*ppOEV)->fExDist		= 10.0f;
	(*ppOEV)->fShadowInset	= 0.0f;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->ppAmbient			= m_ppAmbient;

	(*pNegativeExtrudeDest).push_back( ppOEV );

	
	////////////////////
	// positive volume

	pSVO->InitRectangularBlock_Faceted( D3DXVECTOR3(  2.0f, 10.0f, 0.0f ), 0x0033FF66, 
										 D3DXVECTOR3( 3.0f, 0.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.1f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.0f, 3.1f )  );
	pSVO->BuildShadowVolume( 0.9f );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();

	float exd1 = 0.0f;
	float inset = 0.0f;

	(*ppOEV)->ppGeometry			= ppSVVBO;
	(*ppOEV)->ppMatrixNode		= ppNode;
	(*ppOEV)->fDepthScale		= 1.0f;
	(*ppOEV)->fExDist			= 10.0f;
	(*ppOEV)->fShadowInset		= 0.0f;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->bRenderOn			= true;

	m_ActiveObjects.m_vFogExtrusionVolumes.push_back( ppOEV );
	m_objLightBeam.m_vFogExtrusionVolumes.push_back( ppOEV );


	return( hr );
}




HRESULT PolygonalVolumeFogScene::InitObjects05_Buddha()
{
	HRESULT hr = S_OK;	

	// Value of this matrix will be set later by the 
	//   application
	m_ppMat_ViewProject	= NewMatrix();
	m_ppExtrusionOrigin1 = NewVector4( D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ));	// value also set later



	// Subtract object color from white background
	m_ActiveObjects.m_dwFogToScreenDestBlend = D3DBLEND_ONE;
	m_ActiveObjects.m_dwFogToScreenSrcBlend = D3DBLEND_ONE;	
	m_ActiveObjects.m_dwFogToScreenBlendOp = D3DBLENDOP_REVSUBTRACT;

	m_ActiveObjects.m_dwFBClearColor = 0x00FFFFFF;	// ARGB = white

	m_ActiveObjects.m_fThicknessToColorScale = 84.5f;

	/////////////////////////////////////////////////////////////
	
	SimpleVBObject			** ppSVBO;

	D3DXMATRIX		mat;

	MatrixV					** ppMatv;
	MatrixNode				** ppNode;
	Object_FogVolume		** ppOFogVol;
	Object_Solid			** ppOSolid;

	/////////////////////////////////////////////////////////////

		// Weld adjacent faces if the dotproduct of their
		//  face normals is greater than threshold
	float fWeldEdgeThreshold = 0.8f;

	float exd1 = 0.3f * 15.0f;

	float inset = 0.0f;

	/////////////////////////////////////////////////////////////

	// rotate the object to a good orientation
	D3DXMatrixRotationAxis( &mat, & D3DXVECTOR3( 1.0f, 0.0f, 0.0f ), 3.14159f / 2.0f );
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv );

	D3DXMatrixRotationAxis( &mat, & D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), -3.14159f / 4.0f );
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv, ppNode );

	D3DXMatrixTranslation( &mat, 0.0f, 0.0f, -0.7f );
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv, ppNode );

				// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );


	// Quick way to easily change negative volumes to positive
	//  if you want to.
	vector< Object_ExtrusionVolume ** > * pNegativeExtrudeDest;
	pNegativeExtrudeDest = &m_ActiveObjects.m_vFogExtrusionHoles;
//	pNegativeExtrudeDest = &m_ActiveObjects.m_vFogExtrusionVolumes;

	//////////////////////////////////////////////////////////////

	SimpleObject obj;

	LoadPLY( GetFilePath( PLY_NAME ).c_str(),
				& obj );

	obj.CalculateNormalsCCW();
	obj.SetColorFromVertexNormal();		// a diagnostic option to visualize normals

//	obj.EliminateDuplicateTris();

	float xmin, ymin, zmin, xmax, ymax, zmax;
	obj.ComputePositionMinAndMax( &xmin, &ymin, &zmin, &xmax, &ymax, &zmax );

	float cx, cy, cz;
	cx = (xmin + xmax )/2.0f;
	cy = (ymin + ymax )/2.0f;
	cz = (zmin + zmax )/2.0f;

	obj.Translate( -cx / 2.0f, -ymin, -cz / 2.0f );

	float sc;
	if( xmax - xmin == 0.0f )
		xmax = xmin + 2.5f;
	sc = 2.5f / ( xmax - xmin );
	obj.Scale( sc, sc, sc );


	// Assign texture coords based on vertex position
	// This is only to display the object with some sort of 
	//  solid texture when not rendered as a volume object
	UINT vc;
	for( vc = 0; vc < obj.m_wNumVerts; vc++ )
	{
		obj.m_pVertices[vc].t0.x = obj.m_pVertices[vc].pos.x;
		obj.m_pVertices[vc].t0.y = obj.m_pVertices[vc].pos.y;
	}



	ppSVBO		= NewSimpleVBObject( &obj );
	ppOFogVol	= NewObjectFogVolume();
	
	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppDitherControl	= m_ppDitherControl;
	(*ppOFogVol)->ppMatrixNode		= ppNode;
	(*ppOFogVol)->fDepthScale		= 1.0f;
	(*ppOFogVol)->bRenderOn			= true;

	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );
	// Add to the list for switching between solid texture and fog rendering
	m_objFogVolumesForOnOffControl.m_vFogVolumes.push_back( ppOFogVol );


	// Make a stand-in object to illustrate the geometry
	ppOSolid = NewObjectSolid();
	(*ppOSolid)->ppAmbient		= NewVector4( D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ));
	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppMatrixNode	= ppNode;
	(*ppOSolid)->ppTexture		= m_ppTexBlob;
	(*ppOSolid)->bRenderOn		= false;
	// Objects always rendered from m_ActiveObjects.  m_objStandInSolids is just
	//  a list for control purposes
	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );
	m_objStandInSolids.m_vSolidsAffectingFog.push_back( ppOSolid );


	/////////////////////////////////////////////////////////////

	ppSVBO		= NULL;
	ppOFogVol	= NULL;
	ppOSolid	= NULL;
	ppMatv		= NULL;
	ppNode		= NULL;

	return( hr );
}					// buddha



HRESULT	PolygonalVolumeFogScene::InitObjects02_HazeBeamTests()
{
	HRESULT hr = S_OK;

	// Value of this matrix will be set later by the 
	//   application
	m_ppMat_ViewProject = NewMatrix();

	m_ppExtrusionOrigin1 = NewVector4( D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ));	// value also set later


	m_ActiveObjects.m_dwFogToScreenDestBlend = D3DBLEND_INVSRCCOLOR;
	m_ActiveObjects.m_dwFogToScreenSrcBlend = D3DBLEND_ONE;
	m_ActiveObjects.m_dwFogToScreenBlendOp	= D3DBLENDOP_ADD;
	m_ActiveObjects.m_dwFBClearColor		= 0x00000000;	// ARGB = black
	m_ActiveObjects.m_fThicknessToColorScale = 34.0f;


	/////////////////////////////////////////////////////////////
	
	ShadowVolumeObject		* pSVO = NULL;
	SimpleObject			* pSO = NULL;

	ShadowVolumeVBObject	** ppSVVBO;
	SimpleVBObject			** ppSVBO;

	D3DXMATRIX		mat;

	MatrixV					** ppMatv;
	MatrixNode				** ppNode;
	Object_Solid			** ppOSolid;
	Object_FogVolume		** ppOFogVol;
	Object_ExtrusionVolume	** ppOEV;

	pSO = new SimpleObject;
	assert( pSO != NULL );

	pSVO = new ShadowVolumeObject;
	assert( pSVO != NULL );

	/////////////////////////////////////////////////////////////

		// Weld adjacent faces if the dotproduct of their
		//  face normals is greater than threshold
	float fWeldEdgeThreshold = 0.8f;

	float exd1 = 0.3f * 15.0f;

	float inset = 0.0f;

	/////////////////////////////////////////////////////////////

	ppMatv	= NewMatrix();					// identity matrix
	ppNode	= NewMatrixNode( ppMatv, "node 0", NULL );
			// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, "0 vp", ppNode );


	// quick way to easily change negative volumes to positive
	//  if you want to.
	vector< Object_ExtrusionVolume ** > * pNegativeExtrudeDest;
	pNegativeExtrudeDest = & m_ActiveObjects.m_vFogExtrusionHoles;
//	pNegativeExtrudeDest = & m_ActiveObjects.m_vFogExtrusionVolumes;


	//////////////////////////////////////////////////////////
	// Create Object_<> classes which associate SimpleObject
	//  geometry with matrices and other things for rendering

	//////////////
	// Big overhead positive volume
	pSVO->InitRectangularBlock_Faceted( D3DXVECTOR3(  0.0f, 0.0f, 2.5), 0x0033FF66, 
										 D3DXVECTOR3( 2.2f, 0.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 4.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.0f, 0.1f )  );
	pSVO->BuildShadowVolume( fWeldEdgeThreshold );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();

	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode	= ppNode;
	(*ppOEV)->fDepthScale	= 1.0f;
	(*ppOEV)->fExDist		= exd1;
	(*ppOEV)->fShadowInset	= inset;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;

	m_ActiveObjects.m_vFogExtrusionVolumes.push_back( ppOEV );
	m_objLightBeam.m_vFogExtrusionVolumes.push_back( ppOEV );


	//////////////////////////////////////////////
	// positive non-extruded blob

	CreateMasterBlob( pSO );

	ppSVBO		= NewSimpleVBObject( pSO, true );	// dynamic = true
	m_ppBlobGeo	= ppSVBO;

	// Create copy of the blob to use as the starting point
	//  for animation each frame.
	m_ppBlobMaster = NewSimpleObject();
	(*m_ppBlobMaster)->InitClone( pSO );

	// Create a copy which will have its vertices displaced
	//  and animated.
	m_ppBlobNoise = NewSimpleObject();
	(*m_ppBlobNoise)->InitClone( pSO );



	ppOFogVol	= NewObjectFogVolume();

	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppDitherControl	= m_ppDitherControl;
	(*ppOFogVol)->ppMatrixNode		= ppNode;
	(*ppOFogVol)->fDepthScale		= 1.0f;
	(*ppOFogVol)->bRenderOn			= false;

	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );
	m_objFogVolumesForOnOffControl.m_vFogVolumes.push_back( ppOFogVol );


	// Make a stand-in object to illustrate the blob geometry
	ppOSolid = NewObjectSolid();
	(*ppOSolid)->ppAmbient		= NewVector4( D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ));
	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppMatrixNode	= ppNode;
	(*ppOSolid)->ppTexture		= m_ppTexBlob;
	(*ppOSolid)->bRenderOn		= false;
	// Objects always rendered from m_ActiveObjects.  m_objStandInSolids is just
	//  a list for control purposes
	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );
	m_objStandInSolids.m_vSolidsAffectingFog.push_back( ppOSolid );


	// more blobs using same blob geometry
	float dlt = 6.0f;

	ppOFogVol	= NewObjectFogVolume();
	D3DXMATRIX  matT;
	D3DXMatrixTranslation( & matT, dlt, 0.0f, 0.0f );
	ppMatv	= NewMatrix( matT );
	MatrixNode ** ppBN	= NewMatrixNode( ppMatv, "blob x trans", NULL );
	ppBN				= NewMatrixNode( m_ppMat_ViewProject, "blob x trans vp", ppBN );

	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppDitherControl	= m_ppDitherControl;
	(*ppOFogVol)->ppMatrixNode		= ppBN;
	(*ppOFogVol)->fDepthScale		= 1.0f;
	(*ppOFogVol)->bRenderOn			= false;

	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );
	m_objOtherBlobs.m_vFogVolumes.push_back( ppOFogVol );

	// Make a stand-in object to illustrate the blob geometry
	ppOSolid = NewObjectSolid();
	(*ppOSolid)->ppAmbient		= NewVector4( D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ));
	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppMatrixNode	= ppBN;
	(*ppOSolid)->ppTexture		= m_ppTexBlob;
	(*ppOSolid)->bRenderOn		= false;
	// Objects always rendered from m_ActiveObjects.  m_objStandInSolids is just
	//  a list for control purposes
	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );
	m_objOtherBlobs.m_vSolidsAffectingFog.push_back( ppOSolid );


	ppOFogVol	= NewObjectFogVolume();
	D3DXMatrixRotationAxis( & matT, & D3DXVECTOR3( 1.0f, 0.0f, 1.0f ), 3.14159f );
	ppMatv		= NewMatrix( matT );
	ppBN		= NewMatrixNode( ppMatv, "blob +y rot", NULL );
	D3DXMatrixTranslation( & matT, 0.0f, dlt, 0.0f );
	ppMatv		= NewMatrix( matT );
	ppBN		= NewMatrixNode( ppMatv, "blob +y trans", ppBN );
	ppBN		= NewMatrixNode( m_ppMat_ViewProject, "blob +y vp", ppBN );

	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppDitherControl	= m_ppDitherControl;
	(*ppOFogVol)->ppMatrixNode		= ppBN;
	(*ppOFogVol)->fDepthScale		= 1.0f;
	(*ppOFogVol)->bRenderOn			= false;

	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );
	m_objOtherBlobs.m_vFogVolumes.push_back( ppOFogVol );

	// Make a stand-in object to illustrate the blob geometry
	ppOSolid = NewObjectSolid();
	(*ppOSolid)->ppAmbient		= NewVector4( D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ));
	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppMatrixNode	= ppBN;
	(*ppOSolid)->ppTexture		= m_ppTexBlob;
	(*ppOSolid)->bRenderOn		= false;
	// Objects always rendered from m_ActiveObjects.  m_objStandInSolids is just
	//  a list for control purposes
	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );
	m_objOtherBlobs.m_vSolidsAffectingFog.push_back( ppOSolid );


	// Bigger blob scaled in X and Y using same VB geometry
	ppOFogVol	= NewObjectFogVolume();
	D3DXMatrixRotationAxis( & matT, & D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 3.14159f );
	ppMatv		= NewMatrix( matT );
	ppBN		= NewMatrixNode( ppMatv, "blobXY rot", NULL );

	D3DXMatrixScaling( &matT, 2.0f, 2.0f, 1.0f );	// wider in x and y
	ppMatv		= NewMatrix( matT );
	ppBN		= NewMatrixNode( ppMatv, "blobXY scale", ppBN );		// append scale to prevous rotation

	D3DXMatrixTranslation( & matT, dlt + 2.0f, dlt + 2.0f, 0.0f );
	ppMatv		= NewMatrix( matT );
	ppBN		= NewMatrixNode( ppMatv, "blobXY trans", ppBN );

	// Append view project matrix
	ppBN		= NewMatrixNode( m_ppMat_ViewProject, "blob XY VP", ppBN );

	(*ppOFogVol)->ppGeometry		= ppSVBO;
	(*ppOFogVol)->ppDitherControl	= m_ppDitherControl;
	(*ppOFogVol)->ppMatrixNode		= ppBN;
	(*ppOFogVol)->fDepthScale		= 1.0f;
	(*ppOFogVol)->bRenderOn			= false;

	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );
	m_objOtherBlobs.m_vFogVolumes.push_back( ppOFogVol );

	// Make a stand-in object to illustrate the blob geometry
	ppOSolid = NewObjectSolid();
	(*ppOSolid)->ppAmbient		= NewVector4( D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ));
	(*ppOSolid)->ppGeometry		= ppSVBO;
	(*ppOSolid)->ppMatrixNode	= ppBN;
	(*ppOSolid)->ppTexture		= m_ppTexBlob;
	(*ppOSolid)->bRenderOn		= false;
	// Objects always rendered from m_ActiveObjects.  m_objStandInSolids is just
	//  a list for control purposes
	m_ActiveObjects.m_vSolidsAffectingFog.push_back( ppOSolid );
	m_objOtherBlobs.m_vSolidsAffectingFog.push_back( ppOSolid );

	/////////////////////////////////////////////////////
	// Occluders
	// Mid-sized vertical column

	pSVO->InitRectangularBlock_Faceted( D3DXVECTOR3(  1.0f, 0.0f, 1.0f), 0x0033FF66, 
										 D3DXVECTOR3( 0.5f, 0.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.5f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.0f, 2.0f )  );
	pSVO->BuildShadowVolume( fWeldEdgeThreshold );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();

	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode	= ppNode;
	(*ppOEV)->fDepthScale	= 1.0f;
	(*ppOEV)->fExDist		= exd1;
	(*ppOEV)->fShadowInset	= inset;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->ppAmbient			= m_ppAmbient;

	(*pNegativeExtrudeDest).push_back( ppOEV );

	/////////////////////////////////////////////////////
	// Mid-sized vertical column

	pSVO->InitRectangularBlock_Faceted( D3DXVECTOR3( -1.0f, 0.0f, 1.0f), 0x0033FF66, 
										 D3DXVECTOR3( 0.5f, 0.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.5f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.0f, 2.0f )  );
	pSVO->BuildShadowVolume( fWeldEdgeThreshold );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();

	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode	= ppNode;
	(*ppOEV)->fDepthScale	= 1.0f;
	(*ppOEV)->fExDist		= exd1;
	(*ppOEV)->fShadowInset	= inset;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->ppAmbient			= m_ppAmbient;

	(*pNegativeExtrudeDest).push_back( ppOEV );

	/////////////////////////////////////////////////////
	// 0.25 thick horizontal beam

	pSVO->InitRectangularBlock_Faceted( D3DXVECTOR3(  0.0f, 1.0f, 2.1f), 0x0033FF66, 
										 D3DXVECTOR3( 2.5f, 0.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.25f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.0f, 0.25f )  );
	pSVO->BuildShadowVolume( fWeldEdgeThreshold );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();

	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode	= ppNode;
	(*ppOEV)->fDepthScale	= 1.0f;
	(*ppOEV)->fExDist		= exd1;
	(*ppOEV)->fShadowInset	= inset;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->ppAmbient			= m_ppAmbient;

	(*pNegativeExtrudeDest).push_back( ppOEV );

	/////////////////////////////////////////////////////
	// 0.1 thick horizontal beam at negative y coordinate

	pSVO->InitRectangularBlock_Faceted( D3DXVECTOR3(  0.0f, -1.0f, 2.1f), 0x0033FF66, 
										 D3DXVECTOR3( 2.0f, 0.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.1f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.0f, 0.1f )  );
	pSVO->BuildShadowVolume( fWeldEdgeThreshold );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();

	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode	= ppNode;
	(*ppOEV)->fDepthScale	= 1.0f;
	(*ppOEV)->fExDist		= exd1;
	(*ppOEV)->fShadowInset	= inset;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->ppAmbient			= m_ppAmbient;

	(*pNegativeExtrudeDest).push_back( ppOEV );


	/////////////////////////////////////////////////////
	// 0.1 thick horizontal beam at negative y coordinate
	//  lower in Z than the other one

	pSVO->InitRectangularBlock_Faceted( D3DXVECTOR3(  0.0f, -1.0f, 1.5f), 0x0033FF66, 
										 D3DXVECTOR3( 2.0f, 0.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.3f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.0f, 0.1f )  );
	pSVO->BuildShadowVolume( fWeldEdgeThreshold );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();

	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode	= ppNode;
	(*ppOEV)->fDepthScale	= 1.0f;
	(*ppOEV)->fExDist		= exd1;
	(*ppOEV)->fShadowInset	= inset;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->ppAmbient			= m_ppAmbient;

//	(*pNegativeExtrudeDest).push_back( ppOEV );

	/////////////////////////////////////////////////////
	// 0.1 thick horizontal beam in Y axis instead of X
	
	pSVO->InitRectangularBlock_Faceted( D3DXVECTOR3(  0.0f, -1.0f, 1.8f), 0x0033FF66, 
										 D3DXVECTOR3( 0.1f, 0.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 2.0f, 0.0f ),
										 D3DXVECTOR3( 0.0f, 0.0f, 0.1f )  );
	pSVO->BuildShadowVolume( fWeldEdgeThreshold );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();

	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode	= ppNode;
	(*ppOEV)->fDepthScale	= 1.0f;
	(*ppOEV)->fExDist		= exd1;
	(*ppOEV)->fShadowInset	= inset;
	(*ppOEV)->ppExtrusionOriginO = m_ppExtrusionOrigin1;
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->ppAmbient			= m_ppAmbient;

	(*pNegativeExtrudeDest).push_back( ppOEV );
	
	/////////////////////////////////////////////////////////////
	// fan blade

	float z_low, z_high, radius;
	int sides;

	z_low = 0.0f;
	z_high = 0.03f;
	radius = 0.75f;
	sides = 45;

	pSVO->InitCylinder( D3DXVECTOR3( 0.0f, 0.0f, z_low ), 
						D3DXVECTOR3( 0.0f, 0.0f, z_high ),
						radius, sides,		// radius, sides
						0, 0,				// cap 1 subdivisions, length subdivisions
						0 );				// cap 2 subdivisions
	pSVO->RemoveDegenerateVerts( 0.0001f );

	int i;
	float ang;
	float pi2 = 3.14159f * 2.0f;
	float inc = pi2 / 5.0f;
	float sc = 3.5f;

	for( i = 0; i < pSVO->m_wNumVerts; i++ )
	{
		// -pi to pi radians
		ang = atan2( pSVO->m_pVertices[i].pos.y, pSVO->m_pVertices[i].pos.x );
		ang = ang + pi2;

		pSVO->m_pVertices[i].pos.z = pSVO->m_pVertices[i].pos.z * sc;

		if( fmod( ang, inc ) < inc * 2.0f / 3.0f )			
		{
			pSVO->m_pVertices[i].pos.x = pSVO->m_pVertices[i].pos.x * sc;
			pSVO->m_pVertices[i].pos.y = pSVO->m_pVertices[i].pos.y * sc;
		}
	}
	pSVO->GenerateNormalsCCW();


	pSVO->BuildShadowVolume( 0.9f );
	sc = 1.0f / sc;
	pSVO->Scale( sc, sc, sc );

	ppSVVBO	= NewShadowVolumeVBObject( pSVO );		// create vertex buffer object
	ppOEV	= NewObjectExtrusionVolume();


	MatrixV ** ppMatR;
	D3DXMatrixIdentity( &mat );
	ppMatR	= NewMatrix( mat );						// initialize to dummy value

	// haze beam objects
	m_vppNegativeObjRotation.push_back( ppMatR );	// remember it so we can animate

	D3DXMatrixTranslation( &mat, 0.0f, -1.3f, 1.6f );
	ppMatv	= NewMatrix( mat );

	ppNode	= NewMatrixNode( ppMatR, "10 rot", NULL );			// rotation first
	ppNode	= NewMatrixNode( ppMatv, "10 trans", ppNode );	// translation after rotation
	(*ppOEV)->ppWorldMatrixNode = ppNode;

	ppNode	= NewMatrixNode( m_ppMat_ViewProject, "10 VP", ppNode );

	(*ppOEV)->ppGeometry		= ppSVVBO;
	(*ppOEV)->ppMatrixNode		= ppNode;
	(*ppOEV)->fDepthScale		= 1.0f;
	(*ppOEV)->fExDist			= exd1;
	(*ppOEV)->fShadowInset		= inset;
	(*ppOEV)->ppExtrusionOriginO = NewVector4( D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ));
	(*ppOEV)->ppDitherControl	= m_ppDitherControl;
	(*ppOEV)->ppAmbient			= m_ppAmbient;

	(*pNegativeExtrudeDest).push_back( ppOEV );
	

	/////////////////////////////////////////////////////////////

	SAFE_DELETE( pSVO );
	SAFE_DELETE( pSO );

	ppSVVBO		= NULL;
	pSVO		= NULL;
	ppOFogVol	= NULL;
	ppOEV		= NULL;
	ppMatv		= NULL;
	ppNode		= NULL;
	ppOSolid	= NULL;

	return( hr );
}



HRESULT	PolygonalVolumeFogScene::InitObjects03_FromSimpleText()
{
	// Loads object from simple text descritpion file.

	HRESULT hr = S_OK;

	FILE * fp;
	fp = fopen( GetFilePath( TEXTGEO_NAME ).c_str(), "rt" );

	if( fp == NULL )
	{
		assert( false );
		return( E_FAIL );
	}
	
	int		rint;
	float	flts[6];

	fscanf( fp, "%d", &rint );

	FDebug("Read file version: %d\n", rint );

	if( rint != 1200 )
	{
		assert( false );
		return( E_FAIL );
	}

	int		nvert;
	int		ntri;

	fscanf( fp, "%d %d", &nvert, &ntri );

	FDebug("nvert:  %d   ntri:  %d\n", nvert, ntri );

	SimpleObject * pSO;
	pSO = new SimpleObject;
	pSO->Allocate( nvert, ntri * 3 );


	int i;
	for( i=0; i < nvert; i++ )
	{
		fscanf( fp, "%f %f %f %f %f %f", &flts[0], &flts[1], &flts[2],
										 &flts[3], &flts[4], &flts[5]  );

		pSO->m_pVertices[i].pos = D3DXVECTOR3( flts[0], flts[1], flts[2] );
		pSO->m_pVertices[i].nrm = D3DXVECTOR3( flts[3], flts[4], flts[5] );

		if( i==0 || i==1 || i==nvert-1 )
			FDebug("i: %d  %f %f %f\n", i, flts[0], flts[1], flts[2] );
	}

	int	tri[3];

	for( i=0; i < ntri; i++ )
	{
		fscanf( fp, "%d %d %d", &tri[0], &tri[1], &tri[2] );

		if( i==0 || i == 1 || i == ntri-1 )
		{
			FDebug("i:  %d   %d %d %d\n", i, tri[0], tri[1], tri[2] );
		}

		if( tri[0] >= nvert || tri[0] < 0 )
			assert( false );
		if( tri[1] >= nvert || tri[1] < 0 )
			assert( false );
		if( tri[2] >= nvert || tri[2] < 0 )
			assert( false );
		

		pSO->m_pIndices[i*3 + 0] = (WORD)(tri[0]);
		pSO->m_pIndices[i*3 + 1] = (WORD)(tri[1]);
		pSO->m_pIndices[i*3 + 2] = (WORD)(tri[2]);
	}

	fclose( fp );
	fp = NULL;


	pSO->GenerateNormalsCCW();
	pSO->SetVertexColors( 0x00FFFFFF );

	float radius;
	D3DXVECTOR3 center;
	pSO->ComputeBoundingSphere( & center, & radius );

	float sc = 4.0f / radius;
	pSO->Scale( sc, sc, sc );


	// Value of this matrix will be set later by the 
	//   application
	m_ppMat_ViewProject = NewMatrix();

	/////////////////////////////////////////////////////////////

	SimpleVBObject		** ppSVBO;

	D3DXMATRIX			mat;

	MatrixV				** ppMatv;
	MatrixNode			** ppNode;
	Object_FogVolume	** ppOFogVol;

	/////////////////////////////////////////////////////////////

	ppSVBO	= NewSimpleVBObject( pSO );

	/////////////////////////////////////////////////////////////
	// Create Object_<> classes which associate SimpleObject
	//  geometry with matrices and other things for rendering
	

	D3DXMatrixTranslation( &mat, 0.0f, 0.0f, 0.0f );
	
	ppMatv	= NewMatrix( mat );
	ppNode	= NewMatrixNode( ppMatv, "simple text trans", NULL );
			// Append view-projection matrix 
	ppNode	= NewMatrixNode( m_ppMat_ViewProject, ppNode );

	ppOFogVol	= NewObjectFogVolume();

	(*ppOFogVol)->ppGeometry	= ppSVBO;
	(*ppOFogVol)->ppMatrixNode	= ppNode;
	(*ppOFogVol)->fDepthScale	= 1.0f;
	(*ppOFogVol)->ppDitherControl = m_ppDitherControl;

	// add fog volume object to instance list
	m_ActiveObjects.m_vFogVolumes.push_back( ppOFogVol );


	//////////////////////////////////////////////////////

	SAFE_DELETE( pSO );

	return( hr );
}




void PolygonalVolumeFogScene::IncrementAllExtrusionDistances( float increment )
{
	int i=0;
	float * pf;
	Object_ExtrusionVolume	**	ppo;
	Object_ExtrusionVolume	*	po;

	for( i=0; i < m_vML_ObjExtrusionVolumes.size(); i++ )
	{
		ppo = m_vML_ObjExtrusionVolumes.at(i);
		if( ppo == NULL )
			continue;
		po = *ppo;
		if( po == NULL )
			continue;

		pf = & ( po->fExDist );
		*pf = (*pf) + increment;
	}
}

void PolygonalVolumeFogScene::SetAllExtrusionDistances( float dist )
{
	int i=0;
	Object_ExtrusionVolume	**	ppo;
	Object_ExtrusionVolume	*	po;

	for( i=0; i < m_vML_ObjExtrusionVolumes.size(); i++ )
	{
		ppo = m_vML_ObjExtrusionVolumes.at(i);
		if( ppo == NULL )
			continue;
		po = *ppo;
		if( po == NULL )
			continue;

		po->fExDist = dist;
	}
}


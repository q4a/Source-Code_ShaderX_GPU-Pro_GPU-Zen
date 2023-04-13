/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\FogPolygonVolumes
File:  PolygonalVolumeFogScene.h

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

#ifndef __GJ_POLYFOGSCENE_H__
#define __GJ_POLYFOGSCENE_H__


#include "NVD3DVers.h"		// local file copy determines which D3D to include
#include <NV_D3DCommon/SceneFactory.h>

#include "PolygonalVolumeFogObjects.h"

#include <vector>

using namespace std;

class	SimpleObject;
class	SimpleVBObject;
class	ShadowVolumeObject;
class	ShadowVolumeVBObject;


//////////////////////////////////////////////////////////////////

// h  - Handle variable
// s  - additional string displayed after variable name
// Example:
//   float ** ppFloat;
//   FMSG_HANDLE( ppFloat, "1st float" );
#ifndef FMSG_HANDLE
#define FMSG_HANDLE( h, s )		\
FMsg("FMSG_HANDLE:   %s  %s = %8.8x  *handle = %8.8x\n", #h, s, (h), ((h)!=NULL) ? (*(h)) : 0 );
#endif

// p  - pointer variable
// s  - additional string displayed after variable name
// Example:
//   float * pFloat;
//   FMSG_POINTER( pFloat, "1st float" );
#ifndef FMSG_POINTER
#define FMSG_POINTER( p, s )		\
FMsg("FMSG_POINTER:  %s  %s = %8.8x\n", #p, s, (p) );
#endif

//////////////////////////////////////////////////////////////////


class PolygonalVolumeFogScene : public SceneFactory
{
public:
	enum ObjectCollectionType
	{
		PVFS_NO_OBJECTS,		// initialize to be empty
		PVFS_THRUSTPLUME_OBJECTS,
		PVFS_HAZEBEAM_OBJECTS,
		PVFS_OBJECTS_FROMSIMPLEFILE,
		PVFS_SPHERE_AND_BLOCK,
		PVFS_BUDDHA,
		PVFS_NVBSCENE,
	};

public:

	// Handles so other classes can tweak the scene easily
	// You can remove some of these if you do not
	//   want to use them.

	MatrixV		**	m_ppMat_ViewProject;	// view-project matrix 
											// Applies to all objects in the scene
	D3DXVECTOR4 **	m_ppAmbient;
	D3DXVECTOR4 **	m_ppAmbTerrain;

	D3DXVECTOR4	**	m_ppExtrusionOrigin1;	// in object space -- ie. objects should all be
											//  at the same MatrixNode.
	SimpleVBObject	** m_ppBlobGeo;
	SimpleObject	** m_ppBlobMaster;		// system memory copy, vertices not perturbed
	SimpleObject	** m_ppBlobNoise;		// system memory copy with verts perturbed

	float	m_fNoiseFreq;
	float	m_fNoiseAmp;


	D3DXVECTOR4 **	m_ppDitherControl;
	D3DXVECTOR4 **	m_ppDitherOff;
	D3DXVECTOR4 **	m_ppDitherOn;


	IDirect3DTexture9 ** m_ppTexBlob;		// for rendering blob geometry as solid
	IDirect3DTexture9 ** m_ppTexFogColorRamp;
	IDirect3DTexture9 ** m_ppTexDefaultBase;


		// Instance lists to remember a few objects so we can
		//  animate them
	vector< MatrixV ** >				m_vppNegativeObjTrans;
	vector< MatrixV ** >				m_vppNegativeObjRotation;
	vector< Object_ExtrusionVolume ** >	m_vpNegativeObj;


	// Instance lists of objects using fog volume geometry
	//  so you can easily turn on/off visualization of 
	//  fog volumes by rendering them as solid objects

	// Solids rendered with a texture to show their shape
	VolumeFogObjectCollection	m_objStandInSolids;
	// Fog volumes to turn off when solids representing them are turned on
	VolumeFogObjectCollection	m_objFogVolumesForOnOffControl;

	VolumeFogObjectCollection	m_objLightBeam;
	VolumeFogObjectCollection	m_objOtherBlobs;	// other blobs to turn on or off

	VolumeFogObjectCollection	m_objSimpleScene;


	void	AnimateBlob01( float time_in_seconds );

	void	IncrementAllExtrusionDistances( float increment );
	void	SetAllExtrusionDistances( float dist );

public:

	PolygonalVolumeFogScene();
	~PolygonalVolumeFogScene();

	HRESULT		Free();
	HRESULT		FreeInstanceLists();
	HRESULT		FreeMasterLists();		// will also FreeInstanceLists()
	void		SetInstanceVarsToNull();

	HRESULT		Initialize( NV_D3DDEVICETYPE * pDev,
							PolygonalVolumeFogScene::ObjectCollectionType objtype,
							GetFilePathCallback file_path_callback = NULL );


	Object_FogVolume		** NewObjectFogVolume();
	Object_ExtrusionVolume	** NewObjectExtrusionVolume();
	Object_Solid			** NewObjectSolid();

	ShadowVolumeVBObject	** NewShadowVolumeVBObject( ShadowVolumeObject * pObj );


	///////////////////////////////////////////////
	// Object instance lists.
	// Pointers here are copies and should not be freed.
	// Set these to NULL and free the version in the master list

	VolumeFogObjectCollection	m_ActiveObjects;


	// Should call Initialize() to trigger each of these
	HRESULT		InitObjects01_ThrustObjects();
	HRESULT		InitObjects02_HazeBeamTests();
	HRESULT		InitObjects03_FromSimpleText();
	HRESULT		InitObjects04_SphereAndBlock();
	HRESULT		InitObjects05_Buddha();
	HRESULT		InitObjects06_NVBScene( const char * filepath );

	HRESULT		InitObjects_Terrain1();

				// load a PLY file format geometry
	HRESULT		LoadPLY( const char * filename,
						SimpleObject * pToThisObject );


protected:

	// Master lists for tracking allocation & free
	vector< Object_FogVolume ** >		m_vML_ObjFogVolumes;
	vector< Object_ExtrusionVolume ** >	m_vML_ObjExtrusionVolumes;
	vector< Object_Solid ** >			m_vML_ObjSolids;
	vector< ShadowVolumeVBObject ** >	m_vML_SVVBObjects;

};








#endif

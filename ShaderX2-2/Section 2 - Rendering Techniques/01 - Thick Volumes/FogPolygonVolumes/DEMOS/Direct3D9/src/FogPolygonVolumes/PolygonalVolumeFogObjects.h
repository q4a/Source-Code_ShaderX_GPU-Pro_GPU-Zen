/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\FogPolygonVolumes
File:  PolygonalVolumeFogObjects.h

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

Lightweight classes which tie geometry and matrix information to other values
which are needed to render them with the PolygonalVolumeFog class.


******************************************************************************/


#ifndef _H_PolygonalVolumeFogObjects_H
#define _H_PolygonalVolumeFogObjects_H


#include <vector>

#include "NVD3DVers.h"
#include <NV_D3DCommon/MatrixNode.h>


#include <shared/NVBScene9.h>


using namespace std;


class SimpleVBObject;
class ShadowVolumeVBObject;
class NVBScene;


//////////////////////////////////////////////////////////////////////////////


// A solid object
class Object_Solid
{
public:
	SimpleVBObject		** ppGeometry;
	MatrixNode			** ppMatrixNode;

	D3DXVECTOR4			** ppAmbient;

	IDirect3DTexture9	** ppTexture;

	bool		bRenderOn;

	void SetAllNull()
	{
		ppGeometry = NULL;
		ppMatrixNode = NULL;
		ppAmbient = NULL;
		ppTexture = NULL;
	}

	Object_Solid()
	{
		SetAllNull();
	}
	~Object_Solid()
	{
		SetAllNull();
	}
};



// A polygonal fog volume object
struct Object_FogVolume
{
	SimpleVBObject		** ppGeometry;
	MatrixNode			** ppMatrixNode;
	float	fDepthScale;	// Values other than 1.0 may cause artifacts
							//  in some cases.  See the effect documentation
							//  for more details.
	D3DXVECTOR4	** ppDitherControl;

	bool		bRenderOn;

};


// A polygonal fog volume that can be extruded 
//  by a vertex shader
class Object_ExtrusionVolume
{	
public:
	ShadowVolumeVBObject	** ppGeometry;

	MatrixNode				** ppMatrixNode;
	MatrixNode				** ppWorldMatrixNode;

	float	fDepthScale;	// Values other than 1.0 may cause artifacts
							//  in some cases.  See the effect documentation
							//  for more details.
	float	fShadowInset;
	float	fExDist;		// extrusion distance

	D3DXVECTOR4	** ppExtrusionOriginO;	// Point away from which vertices are
										//  extruded -- ie. the light position
										//  if extruding a shadow volume.
										// Object space -- For now the point is
										//  in object space, but idealy it would
										//  be in world space and transformed by
										//  the inverse object-to-world space transform
	D3DXVECTOR4	** ppDitherControl;

	D3DXVECTOR4 ** ppAmbient;

	bool		bRenderOn;

	Object_ExtrusionVolume()
	{
		SetAllNull();
	}
	~Object_ExtrusionVolume()
	{
		SetAllNull();
	}
	void SetAllNull()
	{
		ppGeometry			= NULL;
		ppAmbient			= NULL;
		ppDitherControl		= NULL;
		ppExtrusionOriginO	= NULL;
		ppWorldMatrixNode	= NULL;
		ppMatrixNode		= NULL;
		bRenderOn			= true;
	};
};



/*
class MyInst : public SceneFactory::InstanceListBase
{
public:
	void SetAllNull()
	{
		FMsg("MyInst::SetAllNull()\n");
	}


};
*/


class VolumeFogObjectCollection
{
public:

	// A collection of fog scene objects.
	// You can create several of these to keep track of groups of objects
	//  or to change the properties of groups of objects.
	//
	//	m_vFogVolumes;			// objects which add their volume to the fog
	//	m_vFogVolumeHoles;		// objects which subtract their volume from the fog
	//
	//	m_vFogExtrusionVolumes;	// objects extruded to form fog volume
	//	m_vFogExtrusionHoles;	// objects extruded to subtract from the volume
	//
	//	m_vSolidsAffectingFog;	// Solid scene objects which may intersect or occlude
								//  the fog volumes.  Objects which do not intersect
								//  or occlude the fog volumes do not need to be included
								//  in this vector.

	vector< Object_FogVolume ** >		m_vFogVolumes;
	vector< Object_FogVolume ** >		m_vFogVolumeHoles;
	vector< Object_ExtrusionVolume ** >	m_vFogExtrusionVolumes;
	vector< Object_ExtrusionVolume ** >	m_vFogExtrusionHoles;

	vector< Object_Solid ** >			m_vSolidsAffectingFog;

	DWORD	m_dwFogToScreenDestBlend;
	DWORD	m_dwFogToScreenSrcBlend;
	DWORD	m_dwFogToScreenBlendOp;
	DWORD	m_dwFBClearColor;

	float	m_fThicknessToColorScale;	// scale for converting thickness to texture
										//  coordinate for fog color.

	VolumeFogObjectCollection()
	{
		m_dwFogToScreenDestBlend = D3DBLEND_INVSRCCOLOR;
		m_dwFogToScreenSrcBlend = D3DBLEND_ONE;
		m_dwFogToScreenBlendOp	= D3DBLENDOP_ADD;
		m_dwFBClearColor		= 0x00000000;	// ARGB = black
		m_fThicknessToColorScale = 34.0f;
	}
	~VolumeFogObjectCollection()
	{
		Free();
	}

	HRESULT Free()
	{
		HRESULT hr = S_OK;
		
		m_vFogVolumes.clear();
		m_vFogVolumeHoles.clear();
		m_vFogExtrusionVolumes.clear();
		m_vFogExtrusionHoles.clear();
		m_vSolidsAffectingFog.clear();

		return( hr );
	}

	void EnableSolids( bool val )
	{
		UINT i;
		Object_Solid ** ppo;
		Object_Solid * po;
		for( i=0; i < m_vSolidsAffectingFog.size(); i++ )
		{
			ppo = m_vSolidsAffectingFog.at(i);
			if( ppo == NULL )
				continue;
			po = *ppo;
			if( po == NULL )
				continue;

			po->bRenderOn = val;
		}
	}

	void EnableFogs( bool val )
	{
		UINT i;
		Object_ExtrusionVolume	**	ppo;
		Object_ExtrusionVolume	*	po;
		Object_FogVolume		**	ppfv;
		Object_FogVolume		*	pfv;

		for( i=0; i < m_vFogVolumes.size(); i++ )
		{
			ppfv = m_vFogVolumes.at(i);
			if( ppfv == NULL )
				continue;
			pfv = *ppfv;
			if( pfv == NULL )
				continue;
			pfv->bRenderOn = val;
		}
		for( i=0; i < m_vFogVolumeHoles.size(); i++ )
		{
			ppfv = m_vFogVolumeHoles.at(i);
			if( ppfv == NULL )
				continue;
			pfv = *ppfv;
			if( pfv == NULL )
				continue;
			pfv->bRenderOn = val;
		}
		for( i=0; i < m_vFogExtrusionVolumes.size(); i++ )
		{
			ppo = m_vFogExtrusionVolumes.at(i);
			if( ppo == NULL )
				continue;
			po = *ppo;
			if( po == NULL )
				continue;
			po->bRenderOn = val;
		}
		for( i=0; i < m_vFogExtrusionHoles.size(); i++ )
		{
			ppo = m_vFogExtrusionHoles.at(i);
			if( ppo == NULL )
				continue;
			po = *ppo;
			if( po == NULL )
				continue;
			po->bRenderOn = val;
		}
	}


	void EnableRendering( bool val )
	{
		// val true or false to enable or disable rendering
		//  all objects in the collection
		EnableSolids( val );
		EnableFogs( val );
	}
};


#endif



///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgSegment
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSegment.h,v 1.6 2002/12/18 22:00:38 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __SEGMENT_HEADERFILE__
#define __SEGMENT_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Math/pgVec4.h>
#include <pgLib/Render/pgBaseMesh.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>
#include <pgLib/Render/pgTransformable.h>

#include <D3DX8.h>

class pgMaterial;


/// A pgSegment object can render a 3d object having exactly one material
/**
 *  For optimal rendering speed it is necessary to store triangles sorted by
 *  material. A pgSegment object has exactly one material and can therefore
 *  be rendered by one single DirectX call. In order to create/use objects
 *  consisting of more than one material use pgMesh which stores an arbitrary
 *  number of pgSegments.
 */
class PGLIB_API pgSegment : public pgD3DObject, public pgTransformable
{
friend class pgMeshUtil;
friend class pgXUtil;
public:

	/// Properties of a pgSegment
	enum SETTINGS	{
						SET_LIGHT		= 1,		/**<  Mesh will render with light  */
						SET_ZTEST		= 2,		/**<  Mesh will do z-tests  */
						SET_ZWRITE		= 4,		/**<  Mesh will do z-testing  */
						SET_ALPHATEST	= 8,		/**<  Mesh will do alpha testing  */
						SET_FILL		= 16,		/**<  Mesh will fill its triangles  */
						SET_TEXTURED	= 32		/**<  Mesh will draw textures  */
	};

	pgSegment();

	void init();

	/// Sets the base mesh, which is rendered during render()
	void setBaseMesh(pgBaseMesh* nBaseMesh)  {  baseMesh = nBaseMesh;  }


	pgBaseMesh* getBaseMesh()  {  return baseMesh;  }


	/// Sets the current render settings. all previous render settings are lost
	/**
	 *  See pgSegment::SETTING for a list of all possible settings.
	 *  (all settings can be combined freely)
	 */
	void setRenderSettings(int nSettings)  {  settings = nSettings;  }


	/// Enables specific render settings
	void enableRenderSettings(int nSettings)  {  settings |= nSettings;  }


	/// Disables specific render settings
	void disableRenderSettings(int nSettings)  {  settings &= ~nSettings;  }

	
	/// Sets the material to use during rendering
	void setMaterial(pgMaterial* nMaterial)  {  material = nMaterial;  updateRenderSettings();  }

	pgMaterial* getMaterial()  {  return material;  }

	/// Updates the render settings()
	/**
	 *  If any render setting of property of the set material is changed,
	 *  this method has to be called in order to update the render settings
	 */
	void updateRenderSettings();


	/// Returns the number of frames which are stored in the base mesh
	int getNumFrames() const  {  return baseMesh!=NULL ? baseMesh->getNumFrames() : 0;  }


	/// Renders the mesh by blending each segement between two frames.
	/**
	 *  this methods expects the mesh to have at least nMaxFrame frames
	 *  nFrame must be between 0.0 and 1.0. the method automatically
	 *  determines between which two frames it has to tween
	 *  nTexture0 must always be !=NULL. if nTexture1 is !=NULL and
	 *  nTexBlend != 0.0 then a blending between the two textures is performed
	 */
	int renderTweenedWithBlendTextures(float nFrame, int nMinFrame, int nMaxFrame,
									   pgTexturePtr nTexture0, pgTexturePtr nTexture1, float nTexBlend);


	/// Renders the mesh by blending each segement between two frames.
	/**
	 *  Works exactly the as renderTweenedWithBlendTextures, except that
	 *  the texture settings are used from the segments' stage settings
	 */
	int renderTweened(float nFrame, int nMinFrame, int nMaxFrame);


	/// Renders the set frame
	void render(int nFrame);

	/*void setPosition(const pgVec3& nPos) {  if (transform) transform->setPosition(nPos);  }
	void setRotation(const pgVec3& nRot) {  if (transform) transform->setRotation(nRot);  }
	void setScale(const pgVec3& nScale)  {  if (transform) transform->setScale (nScale);  }*/


	/// Returns the basemesh's bounding box
	pgAABBox* getBBox(int nIndex)  {  return baseMesh!=NULL ? baseMesh->getBBox(nIndex) : 0;  }

	static pgString getStringFromSettings(int nSet);
	static int getSettingsFromString(const pgString& nString);

	void deleteDeviceObjects();
	bool restoreDeviceObjects();

protected:
	void deleteStateBlocks();
	void createStageBlock();
	void applyFillAndCull();

	pgBaseMesh*			baseMesh;

	int					settings;
	pgMaterial*			material;

	DWORD				stateBlock;
};

#endif //__SEGMENT_HEADERFILE__

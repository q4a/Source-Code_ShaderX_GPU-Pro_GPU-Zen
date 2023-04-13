///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIResourceManager
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIResourceManager.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __RESOURCEMANAGER_HEADERFILE__
#define __RESOURCEMANAGER_HEADERFILE__


#include <pgLib/pgLib.h>

#include <D3D8.h>

#include <pgLib/Base/pgImage.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Base/pgString.h>

class pgAnimated;
class pgBaseMesh;
class pgCharacter;
class pgMesh;
class pgParticleSystem;
class pgPath;
class pgPathLinear;
class pgTexture;


/// pgIResourceManager is the one and only instance for loading graphics stuff from HD.
/**
 *  Resource Types are:
 *   Raw Images
 *   Textures in format: dds, tga, jpg, bmp
 */
namespace pgIResourceManager
{
	/// Source Paths
	enum SOURCE {
					SOURCE_STD,		/**< Standard Texture Path */
					SOURCE_BSP,		/**< BSP Object and Texture Path */
					SOURCE_MD2,		/**< MD2 Object Texture Path */
					SOURCE_OBJ,		/**< Wavefront OBJ and Texture Path */
	};

	enum {	MAX_TEXTURES = 2048  };


	/// Init has to be called before any resource can be loaded.
	PGLIB_API void init();


	/// Returns the image loaded from the given filename.
	/**
	 *  Since it is a raw image, the size, height and format are needed too.
	 *  CAUTION: the raw file has to have extension ".raw".
	 */
	PGLIB_API pgImage* getRawImage(const pgString& nFileName, int nWidth, int nHeight, pgImage::FORMAT nFormat, SOURCE nSource);


	/// Returns the texture with the given name.
	/**
	 *  If that texture is not in memory yet, it is loaded from the given source.
	 *  If nFormat is pgImage::UNKNOWN the format is taken from the file.
	 */
	PGLIB_API pgTexture* getTexture(const pgString& nFileName, SOURCE nSource, pgImage::FORMAT nFormat=pgImage::UNKNOWN);


	/// Returns the image loaded from the given filename.
	/**
	 *  Since it is a raw image, the size, height and format are needed too.
	 *  CAUTION: the raw file has to have extension ".raw".
	 */
	PGLIB_API pgImage*  getRawImage(const pgString& nFileName, int nWidth, int nHeight, pgImage::FORMAT nFormat);


	/// Returns the texture with the given name.
	/**
	 *  If that texture is not in memory yet, it is loaded from the given source.
	 *  If nFormat is pgImage::UNKNOWN the format is taken from the file.
	 */
	PGLIB_API pgTexture* getTexture(const pgString& nFileName, pgImage::FORMAT nFormat=pgImage::UNKNOWN);


	/// Returns the MD2 with the given name as a basemesh object.
	/**
	 *  If that MD2 is not in memory yet, it is loaded.
	 */
	PGLIB_API pgBaseMesh* getBaseMeshMD2(const pgString& nFileName, bool nLighting=false);


	/// Returns the OBJ with the given name as a basemesh object.
	/**
	 *  If that OBJ is not in memory yet, it is loaded.
	 */
	PGLIB_API pgMesh* getMeshOBJ(const pgString& nFileName, bool nLighting=false, bool nCreateNew=false);


	/// Returns the pgAnimated object with the given name.
	/**
	 *  If that object is not in memory yet, it is loaded.
	 *  nFileName must refer to a .ani script in the ani directory.
	 */
	PGLIB_API pgAnimated* getAnimated(const pgString& nFileName);


	/// Returns the ParticleSystem with the given name.
	/**
	 *  If that object is not in memory yet, it is loaded.
	 *  nFileName must refer to a .ps script in the ani directory.
	 */
	PGLIB_API pgParticleSystem* getParticleSystem(const pgString& nFileName, bool nCreateNew=false);


	/// Returns the LinearPath with the given name as a general path object.
	/**
	 *  If that object is not in memory yet, it is loaded.
	 *  nFileName must refer to a .path file in the path directory.
	 */
	PGLIB_API pgPathLinear* getLinearPath(const pgString& nFileName);


	/// Returns the standard texture, which is used, when another texture could not be loaded
	PGLIB_API pgTexture* getTexNotFound();
};



#endif //__RESOURCEMANAGER_HEADERFILE__


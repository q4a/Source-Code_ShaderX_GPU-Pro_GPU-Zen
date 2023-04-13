///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrain
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrain.h,v 1.10 2003/03/09 18:15:24 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TERRAIN_HEADERFILE__
#define __TERRAIN_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgImage.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgProfiler.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Math/pgVec4.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>
#include <pgLib/Render/pgTextureStage.h>

#include <pgLib/Render/Terrain/pgTerrainPatchBase.h>
#include <pgLib/Render/Terrain/pgTerrainPVS.h>

#include <D3DX8.h>

class pgBlendTree;

struct PGLIB_API D3DXMATRIX;

/// pgTerrain can render terrain by using geo-mipmapping
/**
 *  pgTerrain uses geo-mipmapping in order to display large
 *  landscapes at a high framerate. An arbitrary number of
 *  texture passes can be applied. An alpha map defines,
 *  where each texture is visible on the landscape and where not.
 */
class PGLIB_API pgTerrain : public pgD3DObject
{
//friend class pgTerrainPatch;
friend class pgTerrainPatchBase;

public:
	/// Settings how the terrain is rendered
	enum RENDERMODE {
			UNIFIED,					/**<  Renders the terrain with one DrawIndexedPrimitive() call  */
			SPLIT,						/**<  Renders the each patch seperated  */
			MORPH_SW,					/**<  Does Vertex Morphing between two tesselation stages to reduce pops  */
			MORPH_HW					/**<  Does Vertex Morphing using Hardware Tweeming between two tesselation stages to reduce pops  */
	};

	pgTerrain();


	/// Sets a new render mode
	/**
	 *  Default mode is SPLIT_MORPH.
	 *  The render mode can not be changed after build() has been called.
	 */
	void setRenderMode(RENDERMODE nMode)  {  renderMode = nMode;  }


	/// Sets a mipmap filter
	/**
	 *  Default filter is LINEAR.
	 */
	void setMipmapFilter(pgTextureStage::FILTER nFilter)  {  mipFilter = nFilter;  }


	/// Sets the base passes texture repeat rate and lightmap
	/**
	 *  Sets a texture for the base pass. nRepeatX and nRepeatY define
	 *  how often the texture is repeated (wrapped) throughout the whole terrain.
	 *  (1.0 means to stretch the texture to cover the whole terrain
	 *  without repeating). The lightmap is not repeated.
	 */
	//void setBasePass(pgTexture* nColorTexture, float nRepeatX, float nRepeatY, pgTexture* nLightmap);

	/// Adds a pass to the terrain.
    /**
	 *  This method works same as setBasePass, except that the lightmap has
	 *  to have an alpha channel, which defines, where the colortexture is
	 *  visible and where not.
	 */
	void addPass(pgTexture* nColorTexture, float nRepeatX, float nRepeatY, pgTexture* nLightmap);


	/// Sets the height map
	/**
	 *  A height value of 0 will create a vertex at pos.y.
	 *  A height value of 255 will create a vertex at pos.y + sizeY
	 */
	void setHeightMap(pgImage* nImage);


	/// Defines the name of the pvs map file
	//
	void setPVSName(const pgString& nName)  {  pvsName = nName;  }


	/// Sets a map which defines where trees are inserted
	void setTreeMap(pgImage* nImage)  {  treeMap = nImage;  }


	void addTreeType(pgBlendTree* nTree, int nIdx)  {  treeTypes.addTail(TreeType(nTree, nIdx));  }


	/// Sets a texture which will be painted on the terrain to show cloud shadows.
	/**
	 *  nRepeatX and nRepeatY define of often the texture is repeated (wrapped).
	 *  nSpeedX and nSpeedY define how much (in texture coordinates) the texture
	 *  moves in one second.
	 */
	void setSkyMap(pgTexture* nSkyMap, float nRepeatX, float nRepeatY, float nSpeedX, float nSpeedY);


	/// Sets the size of the landscape in number of patches.
	/**
	 *  nX and nY must correlate to the image which was set as heightmap.
	 *  The heightmaps size must be (nX*16)+1 x (nY*16)+1.
	 */
	void setNumPatches(int nX, int nY)  {  patchesX = nX;  patchesY = nY;  }


	/// Sets how large one patch is.
	/**
	 *  The size of the complete landscape can be calculated by:
	 *  patchesX*nSizeX x nSizeY x patchesY*nSizeZ
	 */
	void setPatchSize(float nSizeX, float nSizeY, float nSizeZ);


	/// Sets the maximum error which is allowed for patch rendering
	/**
	 *  If the projected error of a patch on the render window exceeds
	 *  maxError, the next more detailed tesselation is used.
	 */
	void setMaxError(float nErr)  {  maxError = nErr;  }


	/// Returns the maximum error, which was set by setMaxError()
	float getMaxError() const  {  return maxError;  }


	/// Sets the fog's range
	/**
	 *  If this method is not called no fog is used
	 */
	void setFogRange(float nNear, float nFar)  {  fogNear = nNear;  fogFar = nFar;  }


	/// Sets the fog's color (default: (1.0, 1.0, 1.0, 1.0)
	void setFogColor(const pgVec4& nCol)  {  fogColor = nCol;  }


	/// Sets the fps which shall be achieved
	/**
	 *  If the value is not zero, the terrain will automatically
	 *  been drawn coarser or finer in order to achieve the
	 *  desired fps. Pass 0 in order to turn of fps achievment.
	 */
	void setDesiredFPS(int nFPS)  {  desiredFPS = nFPS;  }


	/// Returns the desired fps set with setDesiredFPS()
	/**
	 *  By default the automatic fps feature is disabled and
	 *  getDesiredFPS() will return 0.
	 */
	int getDesiredFPS() const  {  return desiredFPS;  }


	void setMergePatches(bool nSet)  {  mergeSmallPatches = nSet;  }


	bool getMergePatches() const  {  return mergeSmallPatches;  }


	/// Build the landscape after all properties have been set.
	void build();


	/// Renders the landscape.
	void render();


	/// Updates the landscapes tesselation.
	/**
	 *  It's important that the camera's final position for
	 *  the current frame has already been set, since it is
	 *  used to calculate the tesselation depth for each
	 *  patch.
	 */
	void update();


	/// Does not yet work correctly
	bool intersectLine(const pgVec3& nPos0, const pgVec3& nPos1, float& nScalarPos) const;


	/// Calculates the position on landscape's surface below nPos
	/**
	 *  If nPos is not above the landscape false is returned.
	 */
	bool projectDown(const pgVec3& nPos, pgVec3& nProjected) const;


	/// Moves nPos to stay above the surface.
	/**
	 *  (nGlideFactor is not used yet)
	 */
	bool moveAboveSurface(pgVec3& nPos, float nHeight, float nGlideFactor) const;


	/// Returns the number of patches which were rendered in the last frame
	int getNumPatchesRendered()  {  return numPatchesRendered;  }


	pgTerrainPatchBasePtr getPatch(int nX, int nY)  {  return patches[nY][nX];  }


	/// Fills a string with information about the last rendered terrain
	void fillInfoString(pgString& nStr);
	void fillInfoString2(pgString& nStr);


	virtual void setPosition(const pgVec3& nPos)  {  pos = nPos;  }
	virtual void setRotation(const pgVec3& nRot)  {  rot = nRot;  }

	virtual void deleteDeviceObjects();
	virtual bool restoreDeviceObjects();

	void addBufferCreateCount()  {  bufferCreateCount++;  }

	static RENDERMODE getRenderModeFromString(const pgString& nString);

protected:
	struct MultiPassInfo
	{
		MultiPassInfo()  {  texColor = texMod = NULL;  repeatX = repeatY = 1.0f;  }

		pgTexturePtr	texColor, texMod;
		float			repeatX, repeatY;
		D3DXMATRIX		matrix;

		// these members are only used for searching invisible patches...
		//
		unsigned char*	visData;
		int				visPitch;
		bool			inVis,fullVis;
	};


	struct TreeType
	{
		TreeType()  {  tree = NULL;  idx = 0;  }
		TreeType(pgBlendTree* nTree, int nIdx)  {  tree = nTree;  idx = nIdx;  }

		friend bool																	// operator== needed for pgList
			operator==(const TreeType&, const TreeType&)  { return false; }

		pgBlendTree*	tree;
		int				idx;
	};


	void updateSkyMatrix();

	void renderUnified();
	void renderSplit();

	void fillPassInfo(MultiPassInfo* nPass, pgTexture* nTexture, float nX, float nY, pgTexture* nLightmap);
	void calcPassVisibility(MultiPassInfo* nInfo, int nX, int nY);

	void setupTextureStages(MultiPassInfo* nPass, bool nBasePass, bool nForceNoHW=false);
	void setupTextureStagesForSky(bool nForceNoHW=false);
	void updateWorldMatrix();
	void checkBuffers();

	pgTerrainPatchBasePtr& getActiveList()  {  return activePatches;  }

	inline float getHeight(int nX, int nY) const;
	inline void getNormal(int nX, int nY, pgVec3& nNormal) const;
	inline void getTexCoord(int nX, int nY, pgVec2& nTCoord) const;

	float getHeightDif(int nX0, int nY0, int nX1, int nY1);
	void createNormals();


	pgTerrainPatchBase* createPatch(int nX, int nY);


	RENDERMODE					renderMode;

	pgString					pvsName;
	pgTerrainPVS*				pvs;

	pgVec3						pos, rot;

	float						sizeX, sizeY, sizeZ;
	float						patchDX, patchDY, patchDZ;
	int							minX, minY, maxX, maxY;

	float						maxError;
	int							desiredFPS;

	pgTerrainPatchBasePtr		**patches;
	pgTerrainPatchBasePtr		activePatches;

	int							patchesX, patchesY;
	int							numPatchesRendered, numPatchesPassesRendered, numPatchesProcessed;

	D3DXMATRIX					worldMatrix;

	pgPtrList<MultiPassInfo>	multiPassInfos;


	MultiPassInfo				skyPass;
	float						skySpeedX,skySpeedY,
								skyPosX,skyPosY;

	float						fogNear, fogFar;
	pgVec4						fogColor;

	pgImagePtr					heightmap;
	unsigned char				*hmData;
	int							hmWidth, hmHeight;

	pgImagePtr					treeMap;
	pgList<TreeType>			treeTypes;

	pgVec3Ptr					normals;

	int							numNewVertices, numNewIndices,
								numVertices, numIndices;

	bool						forceBufferCreate;
	PDIRECT3DINDEXBUFFER8		iBuffer;
	PDIRECT3DVERTEXBUFFER8		vBuffer;

	pgTextureStage::FILTER		mipFilter;

	bool						useUnitDistance, mergeSmallPatches;
	int							patchCount[pgTerrainPatchBase::MAX_SUBDIV+1], bufferCreateCount;

protected:
	pgProfiler					profileUpdate, profileRender;
};

#define FLIP_Y

inline float
pgTerrain::getHeight(int nX, int nY) const
{
	assert(nX>=0 && nX<hmWidth);
	assert(nY>=0 && nY<hmHeight);

#ifdef FLIP_Y
	return ((float)hmData[nX + (hmHeight-1-nY)*hmWidth])/255.0f;
#else
	return ((float)hmData[nX + nY*hmWidth])/255.0f;
#endif // FLIP_Y
}


inline void
pgTerrain::getNormal(int nX, int nY, pgVec3& nNormal) const
{
	assert(nX>=0 && nX<hmWidth);
	assert(nY>=0 && nY<hmHeight);

//#ifdef FLIP_Y
//	nNormal = normals[nX + (hmHeight-1-nY)*hmWidth];
//#else
	nNormal = normals[nX + nY*hmWidth];
//#endif // FLIP_Y
}


inline void
pgTerrain::getTexCoord(int nX, int nY, pgVec2& nTCoord) const
{
	nTCoord.x = (float)nX/(float)(hmWidth-1);

#ifdef FLIP_Y
	nTCoord.y = (float)(hmHeight-1-nY)/(float)(hmHeight-1);
#else
	nTCoord.y = (float)nY/(float)(hmHeight-1);
#endif // FLIP_Y
}


#endif //__TERRAIN_HEADERFILE__


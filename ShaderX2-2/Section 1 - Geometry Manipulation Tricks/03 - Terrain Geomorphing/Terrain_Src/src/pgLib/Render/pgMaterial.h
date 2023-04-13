///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgMaterial
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMaterial.h,v 1.3 2002/12/15 18:48:18 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __MATERIAL_HEADERFILE__
#define __MATERIAL_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgString.h>
#include <pgLib/Render/pgTextureStage.h>

#include <D3DX8.h>

class pgVec4;


/// pgMaterial defines the properties of a surface
/**
 *  A pgMaterial object defines lighting properties
 *  (how the surface reacts to lights) and texture
 *  stages. Each pgSegment object must have a pgMaterial
 *  in order to be rendered.
 */
class PGLIB_API pgMaterial
{
friend class pgSegment;
friend class pgXUtil;
public:

	/// Type of culling performed for each triangle
	enum CULLING {	CULL_NONE = D3DCULL_NONE,		/**<  No culling performed  */
					CULL_CW = D3DCULL_CW,			/**<  Clockwise culling  */
					CULL_CCW = D3DCULL_CCW			/**<  Counterclockwise culling  */
	};


	/// Type of calculation how stage sources are combined.
	enum BLEND  {	BLEND_NONE				= 0,							/**<  Undefined  */
					BLEND_ZERO				= D3DBLEND_ZERO,				/**<  Value is replaced (multiplied) by zero  */
					BLEND_ONE				= D3DBLEND_ONE,					/**<  Value is multiplied by one (unchanged)  */
					BLEND_SRCCOLOR          = D3DBLEND_SRCCOLOR,			/**<  Value is multiplied by source color  */
					BLEND_INVSRCCOLOR       = D3DBLEND_INVSRCCOLOR,			/**<  Value is multiplied by one minus source color  */
					BLEND_SRCALPHA          = D3DBLEND_SRCALPHA,			/**<  Value is multiplied by source alpha  */
					BLEND_INVSRCALPHA       = D3DBLEND_INVSRCALPHA,			/**<  Value is multiplied by one minus source alpha  */
					BLEND_DESTALPHA         = D3DBLEND_DESTALPHA,			/**<  Value is multiplied by destination alpha  */
					BLEND_INVDESTALPHA      = D3DBLEND_INVDESTALPHA,		/**<  Value is multiplied by one minus destination alpha  */
					BLEND_DESTCOLOR         = D3DBLEND_DESTCOLOR,			/**<  Value is multiplied by destination color  */
					BLEND_INVDESTCOLOR      = D3DBLEND_INVDESTCOLOR,		/**<  Value is multiplied by one minus destination color  */
					BLEND_SRCALPHASAT       = D3DBLEND_SRCALPHASAT,
					BLEND_BOTHSRCALPHA      = D3DBLEND_BOTHSRCALPHA,
					BLEND_BOTHINVSRCALPHA   = D3DBLEND_BOTHINVSRCALPHA
	};

	pgMaterial();

	void setName(const char* nName)  {  name = nName;  }

	const pgString& getName() const  {  return name;  }

	void setBaseColors(const pgVec4& nAmbient, const pgVec4& nDiffuse, const pgVec4& nSpecular, const pgVec4& nEmissive, float nPower);

	void setAmbient(const pgVec4& nColor);

	void setDiffuse(const pgVec4& nColor);

	void setSpecular(const pgVec4& nColor);

	void setEmissive(const pgVec4& nColor);

	void setPower(float nPower);

	void setStage(int nIndex, const pgTextureStage& nStage)  {  stages[nIndex] = nStage;  }


	/// Adds a texture stage to the material
	/**
	 *  Up to 8 texture stages are allowed
	 *  Keep care that the render device can not render more
	 *  stages than pgIDirectX::getNumTextureBlendStages() returns
	 */
	void addStage(const pgTextureStage& nStage)  {  stages.addTail(nStage);  }

	const pgTextureStage& getStage(int nIndex) const  {  return stages[nIndex];  }

	pgTextureStage& getStage(int nIndex)  {  return stages[nIndex];  }

	int getNumStages() const  {  return stages.getSize();  }

	void setBlending(BLEND nSource, BLEND nDest)  {  blendSrc = nSource;  blendDst = nDest;  }


	/// Sets a culling method (clockwise, counterclockwise or none)
	void setCulling(CULLING nCull)  {  culling = nCull;  }


	/// Returns the underlying Direct3D material which is applied to the render device
	D3DMATERIAL8* getDDMaterial()  {  return &ddMaterial;  }

	pgList<pgTextureStage>& getStages()  {  return stages;  };

	static const char* getCullingString(CULLING nCulling);

	static CULLING getCullingFromString(const pgString& nString);

	static const char* getBlendFromString(BLEND nBlend);

	static BLEND getBlendFromString(const pgString& nString);

protected:
	BLEND									blendSrc, blendDst;
	CULLING									culling;
	D3DMATERIAL8							ddMaterial;
	pgList<pgTextureStage>					stages;

	pgString								name;
};

typedef pgMaterial* pgMaterialPtr;


#endif //__MATERIAL_HEADERFILE__

///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTextureStage
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTextureStage.h,v 1.5 2002/12/18 22:00:38 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TEXTURESTAGE_HEADEFILE__
#define __TEXTURESTAGE_HEADEFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Render/pgTexture.h>

#include <D3DX8.h>


/// pgTextureStage is part of a pgMaterial
/**
 *  pgTextureStage defines the setup of a specific
 *  texture stage for a segment.
 */
class PGLIB_API pgTextureStage
{
friend class pgSegment;
friend class pgXUtil;
public:
	/// Filder method
	enum FILTER  {
					FILTER_NONE = D3DTEXF_NONE,						/**<  No filtering (only valid for mipmap, means: no mipmapping)  */
					FILTER_POINT = D3DTEXF_POINT,					/**<  Nearest neighbour  */
					FILTER_LINEAR = D3DTEXF_LINEAR,					/**<  Linear interpolation  */
					FILTER_ANISOTROPIC = D3DTEXF_ANISOTROPIC		/**<  Anisotropic filtering  */
	};

	enum TEXTUREOP  {
					TEXOP_DISABLE						= D3DTOP_DISABLE,
					TEXOP_SELECT_ARG1					= D3DTOP_SELECTARG1,
					TEXOP_SELECT_ARG2					= D3DTOP_SELECTARG2,
					TEXOP_MODULATE						= D3DTOP_MODULATE,
					TEXOP_MODULATE2X					= D3DTOP_MODULATE2X,
					TEXOP_MODULATE4X					= D3DTOP_MODULATE4X,
					TEXOP_ADD							= D3DTOP_ADD,
					TEXOP_ADDSIGNED						= D3DTOP_ADDSIGNED,
					TEXOP_ADDSIGNED2X					= D3DTOP_ADDSIGNED2X,
					TEXOP_SUBTRACT						= D3DTOP_SUBTRACT,
					TEXOP_ADDSMOOTH						= D3DTOP_ADDSMOOTH,
					TEXOP_BLENDDIFFUSEALPHA				= D3DTOP_BLENDDIFFUSEALPHA,
					TEXOP_BLENDTEXTUREALPHA				= D3DTOP_BLENDTEXTUREALPHA,
					TEXOP_BLENDFACTORALPHA				= D3DTOP_BLENDFACTORALPHA,
					TEXOP_BLENDTEXTUREALPHAPM			= D3DTOP_BLENDTEXTUREALPHAPM,
					TEXOP_BLENDCURRENTALPHA				= D3DTOP_BLENDCURRENTALPHA,
					TEXOP_PREMODULATE					= D3DTOP_PREMODULATE,
					TEXOP_MODULATEALPHA_ADDCOLOR		= D3DTOP_MODULATEALPHA_ADDCOLOR,
					TEXOP_MODULATECOLOR_ADDALPHA		= D3DTOP_MODULATECOLOR_ADDALPHA,
					TEXOP_MODULATEINVALPHA_ADDCOLOR		= D3DTOP_MODULATEINVALPHA_ADDCOLOR,
					TEXOP_MODULATEINVCOLOR_ADDALPHA		= D3DTOP_MODULATEINVCOLOR_ADDALPHA,
					TEXOP_BUMPENVMAP					= D3DTOP_BUMPENVMAP,
					TEXOP_BUMPENVMAPLUMINANCE			= D3DTOP_BUMPENVMAPLUMINANCE,
					TEXOP_DOTPRODUCT3					= D3DTOP_DOTPRODUCT3,
					TEXOP_MULTIPLYADD					= D3DTOP_MULTIPLYADD,
					TEXOP_LERP							= D3DTOP_LERP  };

	enum TEXTUREARG  {
					TEXARG_DIFFUSE						= D3DTA_DIFFUSE,
					TEXARG_CURRENT						= D3DTA_CURRENT,
					TEXARG_TEXTURE						= D3DTA_TEXTURE,
					TEXARG_TFACTOR						= D3DTA_TFACTOR,
					TEXARG_SPECULAR						= D3DTA_SPECULAR,
					TEXARG_TEMP							= D3DTA_TEMP,
					TEXARG_COMPLEMENT					= D3DTA_COMPLEMENT,
					TEXARG_ALPHAREPLICATE				= D3DTA_ALPHAREPLICATE  };


	pgTextureStage(pgTexture* nTexture=NULL, FILTER nMinFilter=FILTER_LINEAR, FILTER nMagFilter=FILTER_LINEAR, FILTER nMipFilter=FILTER_POINT);


	void setTexture(pgTexture* nTexture)  {  texture = nTexture;  }

	void setCoordIndex(int nIdx)  {  coordIdx = nIdx;  }

	void setMinFilter(FILTER nMinFilter)  {  minFilter = nMinFilter;  }

	void setMagFilter(FILTER nMagFilter)  {  magFilter = nMagFilter;  }

	void setMipFilter(FILTER nMipFilter)  {  mipFilter = nMipFilter;  }


	void setColorOp(TEXTUREOP nOp)  {  colorOp = nOp;  }

	void setColorArg1(TEXTUREARG nArg)  {  colorArg1 = nArg;  }

	void setColorArg2(TEXTUREARG nArg)  {  colorArg2 = nArg;  }


	void setAlphaOp(TEXTUREOP nOp)  {  alphaOp = nOp;  }

	void setAlphaArg1(TEXTUREARG nArg)  {  alphaArg1 = nArg;  }

	void setAlphaArg2(TEXTUREARG nArg)  {  alphaArg2 = nArg;  }


	const pgTexture* getTexture() const  {  return texture;  }

	friend bool																	// operator== needed for pgList
		operator==(const pgTextureStage&, const pgTextureStage&)  {  return false;  }


	static FILTER getFilterFromString(const pgString& nString);

	static const char* getStringFromFilter(FILTER nFilter);

	static TEXTUREOP getTextureOpFromString(const pgString& nString);

	static const char* getStringFromTextureOp(pgTextureStage::TEXTUREOP nOp);

	static TEXTUREARG getTextureArgFromString(const pgString& nString);

	static const char* getStringFromTextureArg(pgTextureStage::TEXTUREARG nArg);

protected:
	pgTexture*			texture;

	FILTER				minFilter,
						magFilter,
						mipFilter;

	TEXTUREOP			colorOp;
	TEXTUREARG			colorArg1, colorArg2;

	TEXTUREOP			alphaOp;
	TEXTUREARG			alphaArg1, alphaArg2;

	int					coordIdx;
};


#endif //__TEXTURESTAGE_HEADEFILE__

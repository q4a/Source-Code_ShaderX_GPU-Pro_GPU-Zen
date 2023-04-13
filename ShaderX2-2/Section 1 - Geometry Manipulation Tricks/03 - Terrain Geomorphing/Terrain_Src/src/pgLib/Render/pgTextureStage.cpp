///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTextureStage
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTextureStage.cpp,v 1.4 2002/12/18 22:00:38 daniel Exp $
///////////////////////////////////////////////////////////////////


#include <pgLib/pgLib.h>
#include "pgTextureStage.h"


pgTextureStage::pgTextureStage(pgTexture* nTexture, FILTER nMinFilter, FILTER nMagFilter, FILTER nMipFilter)
{
	texture = nTexture;
	minFilter = nMinFilter;
	magFilter = nMagFilter;
	mipFilter = nMipFilter;

	colorOp = TEXOP_SELECT_ARG1;
	colorArg1 = TEXARG_TEXTURE;
	colorArg2 = TEXARG_DIFFUSE;

	alphaOp = TEXOP_DISABLE;
	alphaArg1 = alphaArg2 = TEXARG_DIFFUSE;

	coordIdx = 0;
}


pgTextureStage::FILTER
pgTextureStage::getFilterFromString(const pgString& nString)
{
	pgString str = nString;

	str.toLower();

	if(str=="none")
		return FILTER_NONE;
	if(str=="point")
		return FILTER_POINT;
	if(str=="linear")
		return FILTER_LINEAR;
	if(str=="anisotropic")
		return FILTER_ANISOTROPIC;

	return FILTER_LINEAR;
}


const char*
pgTextureStage::getStringFromFilter(pgTextureStage::FILTER nFilter)
{
	switch(nFilter)
	{
	default:
	case FILTER_NONE:
		return "none";

	case FILTER_POINT:
		return "point";

	case FILTER_LINEAR:
		return "linear";

	case FILTER_ANISOTROPIC:
		return "anisotropic";
	}
}


pgTextureStage::TEXTUREOP
pgTextureStage::getTextureOpFromString(const pgString& nString)
{
	if(nString=="disable")
		return TEXOP_DISABLE;
	if(nString=="select_arg1")
		return TEXOP_SELECT_ARG1;
	if(nString=="select_arg2")
		return TEXOP_SELECT_ARG2;
	if(nString=="modulate")
		return TEXOP_MODULATE;
	if(nString=="modulate2x")
		return TEXOP_MODULATE2X;
	if(nString=="modulate4x")
		return TEXOP_MODULATE4X;
	if(nString=="add")
		return TEXOP_ADD;
	if(nString=="addsigned")
		return TEXOP_ADDSIGNED;
	if(nString=="addsigned2x")
		return TEXOP_ADDSIGNED2X;
	if(nString=="subtract")
		return TEXOP_SUBTRACT;
	if(nString=="addsmooth")
		return TEXOP_ADDSMOOTH;
	if(nString=="blenddiffusealpha")
		return TEXOP_BLENDDIFFUSEALPHA;
	if(nString=="blendtexturealpha")
		return TEXOP_BLENDTEXTUREALPHA;
	if(nString=="blendfactoralpha")
		return TEXOP_BLENDFACTORALPHA;
	if(nString=="blendtexturealphapm")
		return TEXOP_BLENDTEXTUREALPHAPM;
	if(nString=="blendcurrentalpha")
		return TEXOP_BLENDCURRENTALPHA;
	if(nString=="premodulate")
		return TEXOP_PREMODULATE;
	if(nString=="modulatealpha_addcolor")
		return TEXOP_MODULATEALPHA_ADDCOLOR;
	if(nString=="modulatecolor_addalpha")
		return TEXOP_MODULATECOLOR_ADDALPHA;
	if(nString=="modulateinvalpha_addcolor")
		return TEXOP_MODULATEINVALPHA_ADDCOLOR;
	if(nString=="modulateinvcolor_addalpha")
		return TEXOP_MODULATEINVCOLOR_ADDALPHA;
	if(nString=="bumpenvmap")
		return TEXOP_BUMPENVMAP;
	if(nString=="bumpenvmapluminance")
		return TEXOP_BUMPENVMAPLUMINANCE;
	if(nString=="dotproduct3")
		return TEXOP_DOTPRODUCT3;
	if(nString=="multiplyadd")
		return TEXOP_MULTIPLYADD;
	if(nString=="lerp")
		return TEXOP_LERP;

	return TEXOP_DISABLE;
}


const char*
pgTextureStage::getStringFromTextureOp(pgTextureStage::TEXTUREOP nOp)
{
	switch(nOp)
	{
	default:
	case TEXOP_DISABLE:
		return "disable";
	case TEXOP_SELECT_ARG1:
		return "select_arg1";
	case TEXOP_SELECT_ARG2:
		return "select_arg2";
	case TEXOP_MODULATE:
		return "modulate";
	case TEXOP_MODULATE2X:
		return "modulate2x";
	case TEXOP_MODULATE4X:
		return "modulate4x";
	case TEXOP_ADD:
		return "add";
	case TEXOP_ADDSIGNED:
		return "addsigned";
	case TEXOP_ADDSIGNED2X:
		return "addsigned2x";
	case TEXOP_SUBTRACT:
		return "subtract";
	case TEXOP_ADDSMOOTH:
		return "addsmooth";
	case TEXOP_BLENDDIFFUSEALPHA:
		return "blenddiffusealpha";
	case TEXOP_BLENDTEXTUREALPHA:
		return "blendtexturealpha";
	case TEXOP_BLENDFACTORALPHA:
		return "blendfactoralpha";
	case TEXOP_BLENDTEXTUREALPHAPM:
		return "blendtexturealphapm";
	case TEXOP_BLENDCURRENTALPHA:
		return "blendcurrentalpha";
	case TEXOP_PREMODULATE:
		return "premodulate";
	case TEXOP_MODULATEALPHA_ADDCOLOR:
		return "modulatealpha_addcolor";
	case TEXOP_MODULATECOLOR_ADDALPHA:
		return "modulatecolor_addalpha";
	case TEXOP_MODULATEINVALPHA_ADDCOLOR:
		return "modulateinvalpha_addcolor";
	case TEXOP_MODULATEINVCOLOR_ADDALPHA:
		return "modulateinvcolor_addalpha";
	case TEXOP_BUMPENVMAP:
		return "bumpenvmap";
	case TEXOP_BUMPENVMAPLUMINANCE:
		return "bumpenvmapluminance";
	case TEXOP_DOTPRODUCT3:
		return "dotproduct3";
	case TEXOP_MULTIPLYADD:
		return "multiplyadd";
	case TEXOP_LERP:
		return "lerp";
	}
}


pgTextureStage::TEXTUREARG
pgTextureStage::getTextureArgFromString(const pgString& nString)
{
	if(nString=="diffuse")
		return TEXARG_DIFFUSE;

	if(nString=="current")
		return TEXARG_CURRENT;

	if(nString=="texture")
		return TEXARG_TEXTURE;

	if(nString=="tfactor")
		return TEXARG_TFACTOR;

	if(nString=="specular")
		return TEXARG_SPECULAR;

	if(nString=="temp")
		return TEXARG_TEMP;

	if(nString=="complement")
		return TEXARG_COMPLEMENT;

	if(nString=="alphareplicate")
		return TEXARG_ALPHAREPLICATE;

	return TEXARG_DIFFUSE;
}

const char*
pgTextureStage::getStringFromTextureArg(pgTextureStage::TEXTUREARG nArg)
{
	switch(nArg)
	{
	default:
	case TEXARG_DIFFUSE:
		return "diffuse";
	case TEXARG_CURRENT:
		return "current";
	case TEXARG_TEXTURE:
		return "texture";
	case TEXARG_TFACTOR:
		return "tfactor";
	case TEXARG_SPECULAR:
		return "specular";
	case TEXARG_TEMP:
		return "temp";
	case TEXARG_COMPLEMENT:
		return "complement";
	case TEXARG_ALPHAREPLICATE:
		return "alphareplicate";
	}

	return "diffuse";
}


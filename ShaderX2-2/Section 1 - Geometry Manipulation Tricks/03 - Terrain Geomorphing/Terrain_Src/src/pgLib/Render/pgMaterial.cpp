///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgMaterial
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMaterial.cpp,v 1.3 2002/12/15 18:48:18 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgMaterial.h"

#include <pgLib/Base/pgIDirectX.h>


pgMaterial::pgMaterial()
{
	culling = CULL_CW;
	blendSrc = BLEND_SRCALPHA;
	blendDst = BLEND_INVSRCALPHA;

	pgIDirectX::setColor(ddMaterial.Ambient, pgVec4(1.0f,1.0f,1.0f,1.0f));
	pgIDirectX::setColor(ddMaterial.Diffuse, pgVec4(1.0f,1.0f,1.0f,1.0f));
	pgIDirectX::setColor(ddMaterial.Specular, pgVec4(0.0f,0.0f,0.0f,0.0f));
	pgIDirectX::setColor(ddMaterial.Emissive, pgVec4(0.0f,0.0f,0.0f,0.0f));
	ddMaterial.Power = 0.0f;
}


void
pgMaterial::setBaseColors(const pgVec4& nAmbient, const pgVec4& nDiffuse, const pgVec4& nSpecular, const pgVec4& nEmissive, float nPower)
{
	pgIDirectX::setColor(ddMaterial.Ambient, nAmbient);
	pgIDirectX::setColor(ddMaterial.Diffuse, nDiffuse);
	pgIDirectX::setColor(ddMaterial.Specular, nSpecular);
	pgIDirectX::setColor(ddMaterial.Emissive, nEmissive);
	ddMaterial.Power = nPower;
}


void
pgMaterial::setAmbient(const pgVec4& nColor)
{
	pgIDirectX::setColor(ddMaterial.Ambient, nColor);
}


void
pgMaterial::setDiffuse(const pgVec4& nColor)
{
	pgIDirectX::setColor(ddMaterial.Diffuse, nColor);
}


void
pgMaterial::setSpecular(const pgVec4& nColor)
{
	pgIDirectX::setColor(ddMaterial.Specular, nColor);
}


void
pgMaterial::setEmissive(const pgVec4& nColor)
{
	pgIDirectX::setColor(ddMaterial.Emissive, nColor);
}


void
pgMaterial::setPower(float nPower)
{
	ddMaterial.Power = nPower;
}


const char*
pgMaterial::getCullingString(pgMaterial::CULLING nCulling)
{
	switch(nCulling)
	{
	case CULL_CW:
		return "cw";

	case CULL_CCW:
		return "ccw";

	case CULL_NONE:
	default:
		return "none";
	}
}


pgMaterial::CULLING
pgMaterial::getCullingFromString(const pgString& nString)
{
	if(nString=="cw")
		return CULL_CW;

	if(nString=="ccw")
		return CULL_CCW;

	return CULL_NONE;
}


const char*
pgMaterial::getBlendFromString(pgMaterial::BLEND nBlend)
{
	switch(nBlend)
	{
	case BLEND_ZERO:
		return "zero";

	case BLEND_ONE:
		return "one";

	case BLEND_SRCCOLOR:
		return "srccolor";

	case BLEND_INVSRCCOLOR:
		return "invsrccolor";

	case BLEND_SRCALPHA:
		return "srcalpha";

	case BLEND_INVSRCALPHA:
		return "invsrcalpha";

	case BLEND_DESTALPHA:
		return "destalpha";

	case BLEND_INVDESTALPHA:
		return "invdestalpha";

	case BLEND_DESTCOLOR:
		return "destcolor";

	case BLEND_INVDESTCOLOR:
		return "invdestcolor";

	case BLEND_SRCALPHASAT:
		return "srcalphasat";

	case BLEND_BOTHSRCALPHA:
		return "bothsrcalpha";

	case BLEND_BOTHINVSRCALPHA:
		return "bothinvsrcalpha";

	case BLEND_NONE:
	default:
		return "none";
	}
}


pgMaterial::BLEND
pgMaterial::getBlendFromString(const pgString& nString)
{
	if(nString=="zero")
		return BLEND_ZERO;

	if(nString=="one")
		return BLEND_ONE;

	if(nString=="srccolor")
		return BLEND_SRCCOLOR;

	if(nString=="invsrccolor")
		return BLEND_INVSRCCOLOR;

	if(nString=="srcalpha")
		return BLEND_SRCALPHA;

	if(nString=="invsrcalpha")
		return BLEND_INVSRCALPHA;

	if(nString=="destalpha")
		return BLEND_DESTALPHA;

	if(nString=="invdestalpha")
		return BLEND_INVDESTALPHA;

	if(nString=="destcolor")
		return BLEND_DESTCOLOR;

	if(nString=="invdestcolor")
		return BLEND_INVDESTCOLOR;

	if(nString=="srcalphasat")
		return BLEND_SRCALPHASAT;

	if(nString=="bothsrcalpha")
		return BLEND_BOTHSRCALPHA;

	if(nString=="bothinvsrcalpha")
		return BLEND_BOTHINVSRCALPHA;

	return BLEND_NONE;
}


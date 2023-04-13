///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgLight
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgLight.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgLight.h"
#include <pgLib/Base/pgIDirectX.h>


pgLight::pgLight() : type(TYPE_DIRECTIONAL), dir(1.0f, 0.0f, 0.0f), diffuse(1.0f, 1.0f, 1.0f, 1.0f)
{
	updateAll();
}


void
pgLight::setColors(const pgVec4& nAmbient, const pgVec4& nDiffuse, const pgVec4& nSpecular)
{
	ambient = nAmbient;  diffuse = nDiffuse;  specular = nSpecular;
	pgIDirectX::setColor(ddLight.Ambient, ambient);
	pgIDirectX::setColor(ddLight.Diffuse, diffuse);
	pgIDirectX::setColor(ddLight.Specular, specular);
}


void
pgLight::setDirection(const pgVec3n& nDir)
{
	dir = nDir;
	pgIDirectX::setD3DVecFromVec3(ddLight.Direction, dir);
}


void
pgLight::setPosition(const pgVec3& nPos)
{
	pos = nPos;
	pgIDirectX::setD3DVecFromVec3(ddLight.Position, pos);
}


void
pgLight::setAmbient(const pgVec4& nAmbient)
{
	ambient = nAmbient;
	pgIDirectX::setColor(ddLight.Ambient, ambient);
}


void
pgLight::setDiffuse(const pgVec4& nDiffuse)
{
	diffuse = nDiffuse;
	pgIDirectX::setColor(ddLight.Diffuse, diffuse);
}


void
pgLight::setSpecular(const pgVec4& nSpecular)
{
	specular = nSpecular;
	pgIDirectX::setColor(ddLight.Specular, specular);
}


void
pgLight::setRange(float nRange)
{
	range = nRange;
	ddLight.Range = nRange;
}


void
pgLight::updateAll()
{
	ZeroMemory(&ddLight, sizeof(D3DLIGHT8));

	ddLight.Type = static_cast<D3DLIGHTTYPE>(type);
	pgIDirectX::setD3DVecFromVec3(ddLight.Direction, dir);
	pgIDirectX::setD3DVecFromVec3(ddLight.Position, pos);
	pgIDirectX::setColor(ddLight.Ambient, ambient);
	pgIDirectX::setColor(ddLight.Diffuse, diffuse);
	pgIDirectX::setColor(ddLight.Specular, specular);
}

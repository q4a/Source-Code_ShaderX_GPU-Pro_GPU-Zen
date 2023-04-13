///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTexture
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTexture.cpp,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgTexture.h"


pgTexture::pgTexture() : d3dTexture(NULL)
{
	width = height = 0;
	format = pgImage::UNKNOWN;
	name = "noname";

	lockLevel = -1;
}


pgTexture::pgTexture(const pgString& nName, LPDIRECT3DTEXTURE8 nD3DTex,
					 int nWidth, int nHeight, pgImage::FORMAT nFormat) : d3dTexture(nD3DTex)
{
	name = nName;
	width = nWidth;
	height = nHeight;
	format = nFormat;

	lockLevel = -1;
}


bool
pgTexture::isCompressed() const
{
	return pgImage::isCompressed(format);
}


bool
pgTexture::getData(unsigned char*& nData, int& nPitch, int nLevel, bool nReadOnly)
{
	if(!d3dTexture || lockLevel>=0)
		return false;

	D3DLOCKED_RECT lockedRect;

	if(d3dTexture->LockRect(nLevel, &lockedRect, NULL, nReadOnly ? D3DLOCK_READONLY : 0) != D3D_OK)
		return false;

	nData = (unsigned char*)lockedRect.pBits;
	nPitch = lockedRect.Pitch;

	lockLevel = nLevel;
	return true;
}


void
pgTexture::releaseData()
{
	if(lockLevel<0)
		return;

	d3dTexture->UnlockRect(lockLevel);
	lockLevel = -1;
}


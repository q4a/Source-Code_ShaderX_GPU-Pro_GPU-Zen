///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgImage
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgImage.cpp,v 1.2 2002/12/11 21:09:30 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgImage.h"

	
pgImage::pgImage() : data(NULL)
{
	width = height = 0;
	format = UNKNOWN;
}


pgImage::pgImage(int nWidth, int nHeight, FORMAT nFormat, const pgString& nName, unsigned char* nData, bool nCopy)
{
	width = nWidth;
	height = nHeight;
	format = nFormat;
	name = nName;

	int size = width * height * getPixelSize();

	if(nData)
	{
		if(nCopy)
		{
			data = new unsigned char[size];
			memcpy(data, nData, size);
		}
		else
			data = nData;
	}
	else
		data = new unsigned char[size];
}


int
pgImage::getPixelSize() const
{
	return getPixelSize(format);
}


int
pgImage::getPixelSize(FORMAT nFormat)
{
	switch(nFormat)
	{
	default:
	case UNKNOWN:
		return 0;

	case A8:
	case P8:
		return 1;

	case RGB565:
	case XRGB1555:
	case ARGB1555:
	case ARGB4444:
		return 2;

	case RGB888:
		return 3;

	case ARGB8888:
	case XRGB8888:
		return 4;
	}
}


pgString
pgImage::getFormatString(D3DFORMAT nFormat)
{
	return getFormatString((FORMAT)nFormat);
}


pgString
pgImage::getFormatString(FORMAT nFormat)
{
	switch(nFormat)
	{
	default:
	case UNKNOWN:
		return "FORMAT-UNKNOWN";

	case A8:
		return "ALPHA-8";

	case P8:
		return "INDEXED-8";

	case RGB565:
		return "RGB-565";

	case XRGB1555:
		return "XRGB-1555";

	case ARGB1555:
		return "ARGB-1555";

	case ARGB4444:
		return "ARGB-4444";

	case RGB888:
		return "RGB-888";

	case ARGB8888:
		return "ARGB-8888";

	case XRGB8888:
		return "XRGB-8888";

	case DXT1:
		return "DXT1";

	case DXT2:
		return "DXT2";

	case DXT3:
		return "DXT3";

	case DXT4:
		return "DXT4";

	case DXT5:
		return "DXT5";

	case L8:
		return "LUMINANCE-8";
	}
}


bool
pgImage::isCompressed(FORMAT nFormat)
{
	switch(nFormat)
	{
	case DXT1:
	case DXT2:
	case DXT3:
	case DXT4:
	case DXT5:
		return true;

	default:
		return false;
	}
}


bool
pgImage::hasAlpha(FORMAT nFormat)
{
	switch(nFormat)
	{
	case ARGB8888:
	case ARGB1555:
	case ARGB4444:
	case A8:
	case DXT1:
	case DXT2:
	case DXT3:
	case DXT4:
	case DXT5:
		return true;

	default:
		return false;
	}
}

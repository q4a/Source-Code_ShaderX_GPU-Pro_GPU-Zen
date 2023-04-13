///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTexture
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTexture.h,v 1.2 2002/12/11 21:09:32 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TEXTURE_HEADERFILE__
#define __TEXTURE_HEADERFILE__


#include <pgLib/pgLib.h>

#include <pgLib/Base/pgImage.h>
#include <pgLib/Base/pgString.h>

#include <D3D8.h>


/// pgTextur class
class PGLIB_API pgTexture
{
public:
	pgTexture();

	pgTexture(const pgString& nName, LPDIRECT3DTEXTURE8 nD3DTex, int nWidth, int nHeight, pgImage::FORMAT nFormat);


	/// Returns a pointer to the underlying Direct3D texture
	/**
	 *  The special case the texture object is invalid (NULL)
	 *  is caught and NULL is returned
	 */
	LPDIRECT3DTEXTURE8 getD3DTexture()  {  return (this==NULL) ? NULL : d3dTexture;  }


	/// Cast operator to cast the texture to a Direct3D texture
	/**
	 *  The special case the texture object is invalid (NULL)
	 *  is caught and NULL is returned
	 */
	operator LPDIRECT3DTEXTURE8()  {  return (this==NULL) ? NULL : d3dTexture;  }


	/// Returns true if the texture in a compressed format
	bool isCompressed() const;


	/// Returns the width of the texture
	int getWidth() const  {  return width;  }


	/// Returns the height of the texture
	int getHeight() const  {  return height;  }


	/// Returns the name of the texture
	const pgString& getName() const  {  return name;  }


	/// Retrieves a pointer to the texture data
	/**
	 *  In order to get a pointer to the texture's data, the texture must be locked.
	 *  Before the texture can be used for further rendering it must be unlocked
	 *  by calling releaseData()
	 */
	bool getData(unsigned char*& nData, int& nPitch, int nLevel=0, bool nReadOnly=true);


	/// returns the texture's image format
	pgImage::FORMAT getFormat() const  {  return format;  }


	/// Releases a texture which was by getData()
	void releaseData();


	LPDIRECT3DTEXTURE8	d3dTexture;
	int					width, height;
	pgImage::FORMAT		format;
	pgString			name;

	int					lockLevel;
};

typedef pgTexture *pgTexturePtr;


#endif //__TEXTURE_HEADERFILE__

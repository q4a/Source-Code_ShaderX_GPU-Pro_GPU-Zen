///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgImage
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgImage.h,v 1.2 2002/12/11 21:09:30 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __IMAGE_HEADERFILE__
#define __IMAGE_HEADERFILE__


#include <pgLib/pgLib.h>

#include <pgLib/Base/pgString.h>

#include <D3D8.h>


/// pgImage objects store images
/**
 *  Each image is defined by its resolution and format
 */
class PGLIB_API pgImage
{
public:
	/// Image format specification
	enum FORMAT {
					UNKNOWN = D3DFMT_UNKNOWN,		/**<  Undefined Format  */
					RGB888 = D3DFMT_R8G8B8,			/**<  24-bit RGB  */
					ARGB8888 = D3DFMT_A8R8G8B8,		/**<  32-bit ARGB  */
					XRGB8888 = D3DFMT_X8R8G8B8,		/**<  32-bit RGB  */
					RGB565 = D3DFMT_R5G6B5,			/**<  16-bit RGB  */
					XRGB1555 = D3DFMT_X1R5G5B5,		/**<  16-bit RGB  */
					ARGB1555 = D3DFMT_A1R5G5B5,		/**<  16-bit ARGB  */
					ARGB4444 = D3DFMT_A4R4G4B4,		/**<  16-bit ARGB  */
					A8 = D3DFMT_A8,					/**<  8-bit Alpha  */
					P8 = D3DFMT_P8,					/**<  8-bit Indexed  */
					L8 = D3DFMT_L8,					/**<  8-bit Gray  */
					DXT1 = D3DFMT_DXT1,				/**<  Opaque / one-bit alpha  */
					DXT2 = D3DFMT_DXT2,				/**<  Explicit alpha (premultiplied)  */
					DXT3 = D3DFMT_DXT3,				/**<  Explicit alpha  */
					DXT4 = D3DFMT_DXT4,				/**<  Interpolated alpha (premultiplied)  */
					DXT5 = D3DFMT_DXT5				/**<  Interpolated alpha  */
	};


	/// Creates an undefinded image (size 0*0)
	pgImage();


	/// Creates an image with size nWidth * nHeight and format nFormat
	/**
	 *  If nData is unequal to NULL the data is taken to initialize the image object.
	 *  If nCopy is true the data is copied and not just referenced.
	 */
	pgImage(int nWidth, int nHeight, FORMAT nFormat, const pgString& nName, unsigned char* nData=NULL, bool nCopy=true);


	/// Returns the width of the image in pixels.
	int getWidth() const  {  return width;  }

	/// Returns the height of the image in pixels.
	int getHeight() const  {  return height;  }

	/// Returns a pointer to the image's pixels
	const unsigned char* getData() const  {  return data;  }

	/// Returns a pointer to the image's pixels
	unsigned char* getData()  {  return data;  }

	/// Returns the number of bytes a single pixel takes in the image
	int getPixelSize() const;

	/// Returns true if the current image format is a compressed format
	bool isCompressed() const  {  return isCompressed(format);  }

	/// Returns the name of the image which was set during construction
	const pgString& getName() const  {  return name;  }

	/// Toolmethod to calculate the pixelsize from a format.
	/**
	 *  The pixelsize is returned as number of bytes each pixel takes using the passed format
	 */
	static int getPixelSize(FORMAT nFormat);

	/// Returns the name of the format as a string
	static pgString getFormatString(D3DFORMAT nFormat);

	/// Returns the name of the format as a string
	static pgString getFormatString(FORMAT nFormat);

	/// Converts a D3D pixel format into a pgLib pixel format
	static D3DFORMAT getD3DFormat(FORMAT nFormat)  {  return D3DFORMAT(nFormat);  }

	/// Returns true if the passed image format is a compressed format
	static bool isCompressed(FORMAT nFormat);

	/// Returns true if the passed image format has an alpha component
	static bool hasAlpha(FORMAT nFormat);

protected:
	int				width, height;
	FORMAT			format;
	unsigned char*	data;
	pgString		name;
};

typedef pgImage* pgImagePtr;

#endif //__IMAGE_HEADERFILE__

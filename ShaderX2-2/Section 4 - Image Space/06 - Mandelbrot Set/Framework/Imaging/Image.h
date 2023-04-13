                  /*--.          .-"-. 
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#ifndef _IMAGE_H_
#define _IMAGE_H_

enum FORMAT {
	FORMAT_NONE    = 0,
	// Plain formats
	FORMAT_I8      = 1,
	FORMAT_RGB8    = 2,
	FORMAT_RGBA8   = 3,
	FORMAT_I32F    = 4,
	FORMAT_RGB32F  = 5,
	FORMAT_RGBA32F = 6,
	// Packed formats
	FORMAT_RGB565  = 7,
	// Compressed formats
	FORMAT_DXT1    = 8,
	FORMAT_DXT3    = 9,
	FORMAT_DXT5    = 10,
};


inline bool isPlainFormat(const FORMAT format){
	return (format <= FORMAT_RGBA32F);
}

inline bool isPackedFormat(const FORMAT format){
	return (format == FORMAT_RGB565);
}

inline bool isCompressedFormat(const FORMAT format){
	return (format >= FORMAT_DXT1);
}

inline bool isFloatFormat(const FORMAT format){
	return (format >= FORMAT_I32F && format <= FORMAT_RGBA32F);
}

inline int getChannels(const FORMAT format){
	switch (format){
	case FORMAT_I8:
	case FORMAT_I32F:
		return 1;
	case FORMAT_RGB8:
	case FORMAT_RGB32F:
	case FORMAT_RGB565:
	case FORMAT_DXT1:
		return 3;
	case FORMAT_RGBA8:
	case FORMAT_RGBA32F:
	case FORMAT_DXT3:
	case FORMAT_DXT5:
		return 4;
	default:
		return 0;
	}
}

// Accepts only plain formats
inline int getBytesPerChannel(const FORMAT format){
	return (format <= FORMAT_RGBA8)? 1 : 4;
}
// Accepts only plain and packed formats
inline int getBytesPerPixel(const FORMAT format){
	switch(format){
	case FORMAT_I8:
		return 1;
	case FORMAT_RGB565:
		return 2;
	case FORMAT_RGB8:
		return 3;
	case FORMAT_RGBA8:
	case FORMAT_I32F:
		return 4;
	case FORMAT_RGB32F:
		return 12;
	case FORMAT_RGBA32F:
		return 16;
	default:
		return 0;
	}
}

// Accepts only compressed formats
inline int getBytesPerBlock(const FORMAT format){
	return (format == FORMAT_DXT1)? 8 : 16;
}

inline float clamp(const float x){
	if (x > 1) return 1;
	if (x < 0) return 0;
	return x;
}

/* ------------------------------------------ */

enum MIPMAP_ACTION {
	MIPMAP_NONE     = 0,
	MIPMAP_ALLOCATE = 1,
	MIPMAP_CREATE   = 2
};


enum ResizeMethod {
	IMAGE_NEAREST,
	IMAGE_BILINEAR,
	IMAGE_BICUBIC,
};

enum WrapMethod {
	REPEAT = 0,
	CLAMP  = 1,
};

#define LOREPEAT(x, s) ((x <  0)? x + s : x)
#define HIREPEAT(x, s) ((x >= s)? x - s : x)

#define LOCLAMP(x)     ((x <  0)? 0 : x)
#define HICLAMP(x, s)  ((x >= s)? s : x)

#define LOWRAP(x, s, m) ((m == REPEAT)? LOREPEAT(x, s) : LOCLAMP(x))
#define HIWRAP(x, s, m) ((m == REPEAT)? HIREPEAT(x, s) : HICLAMP(x, s))

/* ------------------------------------------ */


class Image {
protected:
	unsigned char *pixels;
	int width, height;
	int nMipMaps;
	FORMAT format;
	bool ownsMemory;

public:
	Image();
	Image(const Image &img);
	~Image();

	void free();
	void clear();

	unsigned char *getImage(const int mipMapLevel = 0) const;
	int getNumberOfMipMaps() const { return nMipMaps; }
	int getNumberOfMipMapsFromDimesions() const;
	int getMipMappedSize (const int firstMipMapLevel = 0, const int nMipMapLevels = 0x7fffffff, FORMAT srcFormat = FORMAT_NONE) const;
	int getNumberOfPixels(const int firstMipMapLevel = 0, const int nMipMapLevels = 0x7fffffff) const;

	int getWidth (const int mipMapLevel = 0) const;
	int getHeight(const int mipMapLevel = 0) const;
	FORMAT getFormat() const { return format; }

protected:
	bool loadDds (const char *fileName, const bool useRGBA, const bool wantMipMaps);
#ifndef NO_JPEG
	bool loadJpeg(const char *fileName, const bool useRGBA, const bool wantMipMaps);
#endif
#ifndef NO_PNG
	bool loadPng (const char *fileName, const bool useRGBA, const bool wantMipMaps);
#endif
	bool loadTga (const char *fileName, const bool useRGBA, const bool wantMipMaps);
	bool loadBmp (const char *fileName, const bool useRGBA, const bool wantMipMaps);
	bool loadPcx (const char *fileName, const bool useRGBA, const bool wantMipMaps);

	bool saveDds (const char *fileName, const bool isRGBA);
#ifndef NO_JPEG
	bool saveJpeg(const char *fileName, const bool isRGBA, const int quality);
#endif
#ifndef NO_PNG
	bool savePng (const char *fileName, const bool isRGBA);
#endif
	bool saveTga (const char *fileName, const bool isRGBA);
	bool saveBmp (const char *fileName, const bool isRGBA);
	bool savePcx (const char *fileName, const bool isRGBA);

public:
	bool loadImage(const char *fileName, const bool useRGBA = true, const MIPMAP_ACTION mipmapAction = MIPMAP_NONE);
	bool saveImage(const char *fileName, const bool isRGBA = true, const int quality = 75);

	void loadFromMemory(unsigned char *mem, const int w, const int h, const FORMAT frmt, const bool ownsmemory, const bool mipMapped = false);

public:
	bool createMipMaps();
	bool colorMipMaps();
	bool uncompressImage();
	bool unpackImage();

	bool convert(const FORMAT newFormat);

	bool resize(const int newWidth, const int newHeight, const ResizeMethod method = IMAGE_BILINEAR);
	bool toNormalMap(const bool useRGBA = true);
	bool toGrayScale(const bool isRGBA = true, const bool reallocate = true);
	bool alphaFade();

	bool laplace(const WrapMethod wrap);
};

#endif // _IMAGE_H_

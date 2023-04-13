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

#ifndef _IMAGE3D_H_
#define _IMAGE3D_H_

class Image3D {
protected:
	unsigned char *pixels;
	int width, height, depth;
	int nChannels;
	int nMipMaps;
	bool ownsMemory;

public:
	Image3D();
	Image3D(const Image3D &img);
	~Image3D();

	void free();
	void clear();

	unsigned char *getImage(int mipMapLevel = 0) const;
	int getNumberOfMipMaps() const;
	int getNumberOfMipMapsFromDimesions() const;
	int getMipMappedSize(int firstMipMapLevel = 0, int nMipMapLevels = 0x7fffffff) const;

	int getWidth (int mipMapLevel = 0) const;
	int getHeight(int mipMapLevel = 0) const;
	int getDepth (int mipMapLevel = 0) const;
	int getChannels() const;

protected:
	void buildMipMap(unsigned char *dest, unsigned char *src, int width, int height, int depth, int channels);
public:
	bool loadFromFile(const char *fileName);
	void loadFromMemory(unsigned char *mem, const int w, const int h, const int d, const int channels, bool ownsmemory, bool mipMapped = false);
	bool createMipMaps();
};

#endif // _IMAGE3D_H_

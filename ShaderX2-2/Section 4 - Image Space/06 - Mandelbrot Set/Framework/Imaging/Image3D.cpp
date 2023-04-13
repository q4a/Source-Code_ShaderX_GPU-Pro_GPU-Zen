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

#include "Image3D.h"
#include <stdlib.h>
#include <string.h>

Image3D::Image3D(){
	pixels = NULL;
	width  = 0;
	height = 0;
	depth  = 0;
	nChannels = 0;
	nMipMaps  = 0;
	ownsMemory = true;
}

Image3D::Image3D(const Image3D &img){
	width  = img.width;
	height = img.height;
	depth  = img.depth;
	nChannels = img.nChannels;
	nMipMaps  = img.nMipMaps;
	ownsMemory = true;
	
	int size = getMipMappedSize(0, nMipMaps);
	pixels = new unsigned char[size];
	memcpy(pixels, img.pixels, size);
}

Image3D::~Image3D(){
	if (ownsMemory && (pixels != NULL)) delete pixels;
}

void Image3D::free(){
	if (pixels != NULL){
		if (ownsMemory) delete pixels;
		pixels = NULL;
	}
	ownsMemory = true;
}

void Image3D::clear(){
	free();
	width  = 0;
	height = 0;
	depth  = 0;
	nChannels = 0;
	nMipMaps  = 0;
}

unsigned char *Image3D::getImage(int mipMapLevel) const{
	return (mipMapLevel < nMipMaps)? pixels + getMipMappedSize(0, mipMapLevel) : NULL;
}

int Image3D::getNumberOfMipMaps() const { 
	return nMipMaps;
}

int Image3D::getNumberOfMipMapsFromDimesions() const {
	int max = (width > height)? ((width > depth)? width : depth) : (height > depth)? height : depth;
	int i = 0;

	while (max > 0){
		max >>= 1;
		i++;
	}

	return i;
}

int Image3D::getMipMappedSize(int firstMipMapLevel, int nMipMapLevels) const {
	int w = getWidth (firstMipMapLevel) << 1,
		h = getHeight(firstMipMapLevel) << 1,
		d = getDepth (firstMipMapLevel) << 1;

	int level = 0, size = 0;

	while (level < nMipMapLevels && (w != 1 || h != 1 || d != 1)){
		if (w > 1) w >>= 1;
		if (h > 1) h >>= 1;
		if (d > 1) d >>= 1;

		size += w * h * d;
		level++;
	}

	size *= nChannels;

	return size;
}

int Image3D::getWidth (int mipMapLevel) const {
	int a = width >> mipMapLevel;
	return (a == 0)? 1 : a;
}

int Image3D::getHeight(int mipMapLevel) const {
	int a = height >> mipMapLevel;
	return (a == 0)? 1 : a;
}

int Image3D::getDepth (int mipMapLevel) const {
	int a = depth >> mipMapLevel;
	return (a == 0)? 1 : a;
}

int Image3D::getChannels() const {
	return nChannels;
}

void Image3D::loadFromMemory(unsigned char *mem, const int w, const int h, const int d, const int channels, bool ownsmemory, bool mipMapped){
	free();

	pixels = mem;
	width  = w;
	height = h;
	depth  = d;
	nChannels = channels;
	ownsMemory = ownsmemory;

	nMipMaps = mipMapped? getNumberOfMipMapsFromDimesions() : 1;
}

void Image3D::buildMipMap(unsigned char *dest, unsigned char *src, int width, int height, int depth, int channels){
	int xOff = (width  < 2)? 0 : channels;
	int yOff = (height < 2)? 0 : width * channels;
	int zOff = (depth  < 2)? 0 : width * height * channels;

	for (int z = 0; z < depth; z += 2){
		for (int y = 0; y < height; y += 2){
			for (int x = 0; x < width; x += 2){
				for (int i = 0; i < channels; i++){
					*dest++ = ((src[0]    + src[xOff]        + src[yOff]        + src[yOff + xOff] + 
							    src[zOff] + src[zOff + xOff] + src[zOff + yOff] + src[zOff + yOff + xOff]) + 4) >> 3;
					src++;
				}
				src += xOff;
			}
			src += yOff;
		}
		src += zOff;
	}
}

bool Image3D::createMipMaps(){
	int w = width, h = height, d = depth;
	unsigned char *src, *dest;

	if (nMipMaps <= 1){
		pixels = (unsigned char *) realloc(pixels, getMipMappedSize());
		nMipMaps = getNumberOfMipMapsFromDimesions();
	}
	dest = pixels;

	while (w > 1 || h > 1 || d > 1){
		src = dest;
		dest += w * h * d * nChannels;
		buildMipMap(dest, src, w, h, d, nChannels);

		if (w > 1) w >>= 1;
		if (h > 1) h >>= 1;
		if (d > 1) d >>= 1;
	}

	return true;
}

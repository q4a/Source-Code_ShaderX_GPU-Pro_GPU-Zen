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

#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

enum RenderPixelFormat {
	RPF_RGBA8   = 0,
	RPF_RGBA16  = 1,
	RPF_R16f    = 2,
	RPF_RG16f   = 3,
	RPF_RGBA16f = 4,
	RPF_R32f    = 5,
	RPF_RG32f   = 6,
	RPF_RGBA32f = 7,
};

inline int getChannelBits(const RenderPixelFormat rpf){
	switch(rpf){
	case RPF_RGBA8:
		return 8;
	case RPF_R16f:
	case RPF_RG16f:
	case RPF_RGBA16:
	case RPF_RGBA16f:
		return 16;
	case RPF_R32f:
	case RPF_RG32f:
	case RPF_RGBA32f:
		return 32;
	default:
		return 0;
	}
}

inline bool isFloatPixelFormat(const RenderPixelFormat rpf){
	return (rpf >= RPF_R16f);
}

class RenderTarget {
protected:
	unsigned int width, height;
	unsigned int texID;
public:
	unsigned int getTexID() const { return texID; }

	virtual bool bind() = 0;
	virtual bool unbind() = 0;

	virtual bool create(const int w, const int h, const bool mipmapped, const bool cubemap, RenderPixelFormat rPixelFormat) = 0;
	virtual bool destroy() = 0;

	virtual bool setCurrent() = 0;
	virtual bool unsetCurrent() = 0;
	
	virtual bool isLost() = 0;
	virtual bool setRenderFace(const int face) = 0;
};

#endif // _RENDERTARGET_H_

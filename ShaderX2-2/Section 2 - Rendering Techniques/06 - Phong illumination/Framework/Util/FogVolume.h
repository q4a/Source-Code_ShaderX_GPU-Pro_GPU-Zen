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

#ifndef _FOGVOLUME_H_
#define _FOGVOLUME_H_

#include "../OpenGL/OpenGLRenderer.h"
#include "../Math/Vertex.h"
#include "../Math/Color.h"

class FogVolume {
protected:
	Color color;
	float worldSize;
public:
	Color getColor() const { return color; }
	float getWorldSize() const { return worldSize; }
	virtual void draw(const Vertex &camPos) = 0;
	virtual bool isInVolume(const Vertex &v) = 0;
};


class SphereFogVolume : public FogVolume {
protected:
	Vertex position;
	float radius;
public:
	SphereFogVolume(const Vertex &pos, const float size, const Color &col, const float worldsize){
		position = pos;
		radius = size;
		color = col;
		worldSize = worldsize;
	}

	Vertex getPosition() const { return position; }
	void setPosition(const Vertex &pos){ position = pos; }
	void setSize(const float size){ radius = size; }

	void draw(const Vertex &camPos);
	bool isInVolume(const Vertex &v);
};

class CubeFogVolume : public FogVolume {
protected:
	Vertex min, max;
	Vertex v[8];

	void indexQuad(const Vertex &camPos, const int i0, const int i1, const int i2, const int i3);
public:
	CubeFogVolume(const Vertex &low, const Vertex &high, const Color &col, const float worldsize){
		min = low;
		max = high;

		v[0] = low;
		v[1] = Vertex(low.x,  low.y,  high.z);
		v[2] = Vertex(low.x,  high.y, low.z);
		v[3] = Vertex(low.x,  high.y, high.z);
		v[4] = Vertex(high.x, low.y,  low.z);
		v[5] = Vertex(high.x, low.y,  high.z);
		v[6] = Vertex(high.x, high.y, low.z);
		v[7] = high;

		color = col;
		worldSize = worldsize;
	}
	
	void draw(const Vertex &camPos);
	bool isInVolume(const Vertex &v);
};

#endif // _FOGVOLUME_H_

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


#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "MyMath.h"

#define Vector Vertex

// The Vertex structure defines a single point in space or a vector
struct Vertex {
	float x,y,z;
	Vertex(){}
	Vertex(const float ix, const float iy, const float iz){
		x = ix;
		y = iy;
		z = iz;
	}
	operator const float *() const {
		return &x;
	}
	float &operator [] (const int index);
	void operator += (const Vertex &v);
	void operator -= (const Vertex &v);
	void operator *= (const float scalar);
	void operator /= (const float dividend);
	void normalize();
	void fastNormalize();
};

bool operator == (const Vertex &v0, const Vertex &v1);
bool operator != (const Vertex &v0, const Vertex &v1);

Vertex operator + (const Vertex &u, const Vertex &v);
Vertex operator - (const Vertex &u, const Vertex &v);
Vertex operator - (const Vertex &v);
float  operator * (const Vertex &u, const Vertex &v);
Vertex operator * (const float scalar, const Vertex &v);
Vertex operator * (const Vertex &v, const float scalar);
Vertex operator / (const Vertex &v, const float dividend);

Vertex cross(const Vertex &u, const Vertex &v);
float dist(const Vertex &u, const Vertex &v);
float length(const Vertex &v);
float fastLength(const Vertex &v);
float lengthSqr(const Vertex &v);
Vertex normalize(const Vertex &v);
Vertex fastNormalize(const Vertex &v);

unsigned int getRGBANormal(Vertex v);
unsigned int getBGRANormal(Vertex v);

#endif

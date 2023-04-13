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


#include "Vertex.h"

float &Vertex::operator [] (const int index){
	return *(&x + index);
}

void Vertex::operator += (const Vertex &v){
	x += v.x;
	y += v.y;
	z += v.z;
}

void Vertex::operator -= (const Vertex &v){
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void Vertex::operator *= (const float scalar){
	x *= scalar;
	y *= scalar;
	z *= scalar;
}
void Vertex::operator /= (const float dividend){
    x /= dividend;
    y /= dividend;
    z /= dividend;
}

bool operator == (const Vertex &v0, const Vertex &v1){
	return (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z);
}


void Vertex::normalize(){
	float invLen = 1.0f / sqrtf(x * x + y * y + z * z);
	x *= invLen;
	y *= invLen;
	z *= invLen;
}

void Vertex::fastNormalize(){
	float invLen = rsqrtf(x * x + y * y + z * z);
	x *= invLen;
	y *= invLen;
	z *= invLen;
}

/* ------------------------------------------------------------ */

Vertex operator + (const Vertex &u, const Vertex &v){
	return Vertex(u.x + v.x,u.y + v.y,u.z + v.z);
}

Vertex operator - (const Vertex &u, const Vertex &v){
	return Vertex(u.x - v.x,u.y - v.y,u.z - v.z);
}

Vertex operator - (const Vertex &v){
	return Vertex(-v.x,-v.y,-v.z);
}

float operator * (const Vertex &u, const Vertex &v){
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vertex operator * (const float scalar, const Vertex &v){
	return Vertex(scalar * v.x,scalar * v.y,scalar * v.z);
}

Vertex operator * (const Vertex &v, const float scalar){
	return Vertex(scalar * v.x,scalar * v.y,scalar * v.z);
}


Vertex operator / (const Vertex &v, const float dividend){
	return Vertex(v.x / dividend,v.y / dividend,v.z / dividend);
}



Vertex cross(const Vertex &u, const Vertex &v){
	return Vertex(u.y * v.z - v.y * u.z, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

float dist(const Vertex &u, const Vertex &v){
	return sqrtf(sqrf(u.x - v.x) + sqrf(u.y - v.y) + sqrf(u.z - v.z));
}

float length(const Vertex &v){
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float fastLength(const Vertex &v){
	float lsqr = v.x * v.x + v.y * v.y + v.z * v.z;
	return lsqr * rsqrtf(lsqr);
}

float lengthSqr(const Vertex &v){
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

Vertex normalize(const Vertex &v){
	float invLen = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return v * invLen;
}

Vertex fastNormalize(const Vertex &v){
	float invLen = rsqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return v * invLen;
}

unsigned int getRGBANormal(Vertex v){
	unsigned int norm;
	int tmp;
	float lenSqr = lengthSqr(v);
	if (lenSqr > 1) v *= rsqrtf(lenSqr);
	
	tmp = int((v.x+1) * 127.5f);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	norm = tmp;
	tmp = int((v.y+1) * 127.5f);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	norm |= tmp << 8;
	tmp = int((v.z+1) * 127.5f);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	norm |= tmp << 16;
	return norm;
}

unsigned int getBGRANormal(Vertex v){
	unsigned int norm;
	int tmp;
	float lenSqr = lengthSqr(v);
	if (lenSqr > 1) v *= rsqrtf(lenSqr);
	
	tmp = int((v.z+1) * 127.5f);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	norm = tmp;
	tmp = int((v.y+1) * 127.5f);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	norm |= tmp << 8;
	tmp = int((v.x+1) * 127.5f);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	norm |= tmp << 16;
	return norm;
}

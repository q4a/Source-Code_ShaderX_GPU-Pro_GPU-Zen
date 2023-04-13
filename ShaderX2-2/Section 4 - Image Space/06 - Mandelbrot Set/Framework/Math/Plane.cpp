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


#include "Plane.h"

Plane::Plane(const Vertex &Normal, const float Offset){
	normal = Normal;
	offset = Offset;
}

Plane::Plane(const float a, const float b, const float c, const float d){
	normal.x = a;
	normal.y = b;
	normal.z = c;
	offset   = d;
}

Plane::Plane(const Vertex &v0, const Vertex &v1, const Vertex &v2){
	normal = cross(v1 - v0, v2 - v0);
	normal.normalize();
	offset = -(normal * v0);
}

void Plane::normalize(){
	float invLen = 1.0f / length(normal);
	normal *= invLen;
	offset *= invLen;
}

float Plane::distance(const Vertex &v) const {
	return (v * normal + offset);
}

float Plane::getPlaneHitInterpolationConstant(const Vertex &v0, const Vertex &v1) const {
	return (distance(v0)) / (normal * (v0 - v1));
}

RELATION Plane::getVertexRelation(const Vertex &v, float planarDiff) const {
	float rel = distance(v);

	if (fabsf(rel) < planarDiff) return PLANAR;
	if (rel > 0) return FRONT; else return BACK;
}

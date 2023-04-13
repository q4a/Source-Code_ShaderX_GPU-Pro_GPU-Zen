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

#include "FogVolume.h"

bool SphereFogVolume::isInVolume(const Vertex &v){
	return lengthSqr(v - position) < radius * radius;
}

void SphereFogVolume::draw(const Vertex &camPos){
	int i, j, n = 17;
	float x,y,z,r,y2,r2;
	Vertex p;

	y = radius;
	r = 0;
	for (i = 1; i < n; i++){
		y2 = y;
		y  = radius * cosf(i * PI / (n - 1));
		r2 = r;
		r  = radius * radius - y * y;
		r  = ((r < 0)? 0 : sqrtf(r));
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0; j <= n; j++){
			x = cosf(j * 2 * PI / n);
			z = sinf(j * 2 * PI / n);
			
			p = position + Vertex(r  * x, y,  r  * z);
			glTexCoord3fv((camPos - p) / worldSize + Vertex(0.5f, 0.5f, 0.5f));
			glVertex3fv(p);
			
			p = position + Vertex(r2 * x, y2, r2 * z);
			glTexCoord3fv((camPos - p) / worldSize + Vertex(0.5f, 0.5f, 0.5f));
			glVertex3fv(p);
		}
		glEnd();
	}
}




bool CubeFogVolume::isInVolume(const Vertex &v){
	return (v.x > min.x && v.x < max.x &&
			v.y > min.y && v.y < max.y && 
			v.z > min.z && v.z < max.z);
}

void CubeFogVolume::indexQuad(const Vertex &camPos, const int i0, const int i1, const int i2, const int i3){
	glTexCoord3fv((camPos - v[i0]) / worldSize + Vertex(0.5f, 0.5f, 0.5f));
	glVertex3fv(v[i0]);
	glTexCoord3fv((camPos - v[i1]) / worldSize + Vertex(0.5f, 0.5f, 0.5f));
	glVertex3fv(v[i1]);
	glTexCoord3fv((camPos - v[i2]) / worldSize + Vertex(0.5f, 0.5f, 0.5f));
	glVertex3fv(v[i2]);
	glTexCoord3fv((camPos - v[i3]) / worldSize + Vertex(0.5f, 0.5f, 0.5f));
	glVertex3fv(v[i3]);
}

void CubeFogVolume::draw(const Vertex &camPos){
	glBegin(GL_QUADS);
	indexQuad(camPos, 3,2,0,1);
	indexQuad(camPos, 2,6,4,0);
	indexQuad(camPos, 6,7,5,4);
	indexQuad(camPos, 7,3,1,5);
	indexQuad(camPos, 3,7,6,2);
	indexQuad(camPos, 0,4,5,1);
	glEnd();
}

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

#include "Frustum.h"

void Frustum::loadFrustum(const Matrix &modelView, const Matrix &projection){
	Matrix mvp = projection * modelView;

	planes[FRUSTUM_LEFT  ] = Plane(mvp[3] - mvp[0], mvp[7] - mvp[4], mvp[11] - mvp[8],  mvp[15] - mvp[12]);
	planes[FRUSTUM_RIGHT ] = Plane(mvp[3] + mvp[0], mvp[7] + mvp[4], mvp[11] + mvp[8],  mvp[15] + mvp[12]);

	planes[FRUSTUM_TOP   ] = Plane(mvp[3] - mvp[1], mvp[7] - mvp[5], mvp[11] - mvp[9],  mvp[15] - mvp[13]);
	planes[FRUSTUM_BOTTOM] = Plane(mvp[3] + mvp[1], mvp[7] + mvp[5], mvp[11] + mvp[9],  mvp[15] + mvp[13]);

	planes[FRUSTUM_FAR   ] = Plane(mvp[3] - mvp[2], mvp[7] - mvp[6], mvp[11] - mvp[10], mvp[15] - mvp[14]);
	planes[FRUSTUM_NEAR  ] = Plane(mvp[3] + mvp[2], mvp[7] + mvp[6], mvp[11] + mvp[10], mvp[15] + mvp[14]);

	for (int i = 0; i < 6; i++) planes[i].normalize();
}

bool Frustum::pointInFrustum(const Vertex &v) const {
    for (int i = 0; i < 6; i++){
        if (planes[i].distance(v) <= 0) return false;
    }
    return true;
}

bool Frustum::objectInFrustum(const Vertex *vertices, const int nVertices) const {
	int i, j;

    for (i = 0; i < 6; i++){
		for (j = 0; j < nVertices; j++){
			if (planes[i].distance(vertices[j]) > 0) break;
		}		
		if (j == nVertices) return false;
    }
    return true;
}

bool Frustum::sphereInFrustum(const Vertex &v, const float radius) const {
    for (int i = 0; i < 6; i++){
        if (planes[i].distance(v) <= -radius) return false;
    }
    return true;
}

bool Frustum::cubeInFrustum(const float minX, const float maxX, const float minY, const float maxY, const float minZ, const float maxZ) const {
    for (int i = 0; i < 6; i++){
		if (planes[i].distance(Vertex(minX, minY, minZ)) > 0) continue;
		if (planes[i].distance(Vertex(minX, minY, maxZ)) > 0) continue;
		if (planes[i].distance(Vertex(minX, maxY, minZ)) > 0) continue;
		if (planes[i].distance(Vertex(minX, maxY, maxZ)) > 0) continue;
		if (planes[i].distance(Vertex(maxX, minY, minZ)) > 0) continue;
		if (planes[i].distance(Vertex(maxX, minY, maxZ)) > 0) continue;
		if (planes[i].distance(Vertex(maxX, maxY, minZ)) > 0) continue;
		if (planes[i].distance(Vertex(maxX, maxY, maxZ)) > 0) continue;
        return false;
    }
    return true;
}

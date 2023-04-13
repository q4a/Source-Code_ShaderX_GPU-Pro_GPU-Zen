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

#include "DemoLooper.h"
#include <stdio.h>
#include "../Math/MyMath.h"

DemoLooper::DemoLooper(){
	clampIndices = false;
}

DemoLooper::~DemoLooper(){
	clear();
}

bool DemoLooper::loadFromFile(char *fileName){
	unsigned int i, len;
	LoopNode node;
	
	clear();
	FILE *file = fopen(fileName, "rb");
	if (file == NULL) return false;

	fread(&len, sizeof(len), 1, file);
	for (i = 0; i < len; i++){
		fread(&node, sizeof(LoopNode), 1, file);
		nodes.add(node);
	}
	fclose(file);

	return true;
}

bool DemoLooper::writeToFile(char *fileName){
	unsigned int i, len = nodes.getSize();
	
	FILE *file = fopen(fileName, "wb");
	if (file == NULL) return false;

	fwrite(&len, sizeof(len), 1, file);
	for (i = 0; i < len; i++){
		fwrite(&nodes[i], sizeof(LoopNode), 1, file);
	}
	fclose(file);

	return true;
}

void DemoLooper::addNode(const Vertex &pos, const float Wx, const float Wy, const float Wz){
	nodes.add(LoopNode(pos.x, pos.y, pos.z, Wx,Wy,Wz));
}

void DemoLooper::removeNode(const int index){
	nodes.orderedRemove(index);
}

void DemoLooper::removeLastNode(){
	removeNode(nodes.getSize() - 1);
}


void DemoLooper::clear(){
	nodes.clear();
}

/*
	We need this fucntion to allow angles to wrap around, 360 => 0 etc.
	otherwise we'll get a very quick rotation back at the end of the loop
	unless we've rotated equally in both directions.
*/
void fixAngles(float *s){
	if (s[1] > s[0]){
		while (s[1]-s[0] > PI) s[1] -= 2 * PI;
	} else {
		while (s[0]-s[1] > PI) s[1] += 2 * PI;
	}
	
	if (s[2] > s[1]){
		while (s[2]-s[1] > PI) s[2] -= 2 * PI;
	} else {
		while (s[1]-s[2] > PI) s[2] += 2 * PI;
	}
	
	if (s[3] > s[2]){
		while (s[3]-s[2] > PI) s[3] -= 2 * PI;
	} else {
		while (s[2]-s[3] > PI) s[3] += 2 * PI;
	}
}

int clamp(int x, const int len){
	if (x < 0) return 0;
	if (x >= len) return len - 1; else return x;
}

int wrap(int x, const int len){
	x %= len;
	if (x < 0) x += len;
	return x;
}

LoopNode DemoLooper::getNode(const float t){
	int len, i, i0, i1, i2, i3;
	float tFrac, ang0, ang1, ang2, s[4];
	
	len = nodes.getSize();
	i = (int) floorf(t);
	tFrac = t - i;

	if (clampIndices){
		i0 = clamp(i - 1, len);
		i1 = clamp(i,     len);
		i2 = clamp(i + 1, len);
		i3 = clamp(i + 2, len);
	} else {
		i0 = wrap(i - 1, len);
		i1 = wrap(i,     len);
		i2 = wrap(i + 1, len);
		i3 = wrap(i + 2, len);
	}

	
	s[0] = nodes[i0].Wx;
	s[1] = nodes[i1].Wx;
	s[2] = nodes[i2].Wx;
	s[3] = nodes[i3].Wx;
	fixAngles(s);
	ang0 = cerpf(s[0], s[1], s[2], s[3], tFrac);
	
	s[0] = nodes[i0].Wy;
	s[1] = nodes[i1].Wy;
	s[2] = nodes[i2].Wy;
	s[3] = nodes[i3].Wy;
	fixAngles(s);
	ang1 = cerpf(s[0], s[1], s[2], s[3], tFrac);
	
	s[0] = nodes[i0].Wz;
	s[1] = nodes[i1].Wz;
	s[2] = nodes[i2].Wz;
	s[3] = nodes[i3].Wz;
	fixAngles(s);
	ang2 = cerpf(s[0], s[1], s[2], s[3], tFrac);
	
	return LoopNode(
		cerpf(nodes[i0].x, nodes[i1].x, nodes[i2].x, nodes[i3].x, tFrac), 
		cerpf(nodes[i0].y, nodes[i1].y, nodes[i2].y, nodes[i3].y, tFrac), 
		cerpf(nodes[i0].z, nodes[i1].z, nodes[i2].z, nodes[i3].z, tFrac), 
		ang0, ang1, ang2);
}

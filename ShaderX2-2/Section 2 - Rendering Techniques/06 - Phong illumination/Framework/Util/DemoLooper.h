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

#ifndef _DEMOLOOPER_H_
#define _DEMOLOOPER_H_

#include "../Util/Set.h"
#include "../Math/Vertex.h"

class LoopNode {
public:
	float x,y,z;
	float Wx, Wy, Wz;

    LoopNode(){}
	LoopNode(const float X, const float Y, const float Z, const float WX, const float WY, const float WZ){
		x = X;
		y = Y;
		z = Z;
		Wx = WX;
		Wy = WY;
		Wz = WZ;
	}
};


// ##############################################################################################################################


class DemoLooper {
private:
	Set <LoopNode> nodes;
	bool clampIndices;
public:
	DemoLooper();
	~DemoLooper();

	void setClampMode(const bool clamp){ clampIndices = clamp; }

    bool loadFromFile(char *fileName);
    bool writeToFile(char *fileName);

	void addNode(const Vertex &pos, const float Wx, const float Wy, const float Wz);
	void removeNode(const int index);
	void removeLastNode();
    void clear();

    int getNumberOfNodes() const { return nodes.getSize(); }

	LoopNode getNode(const float t);
};


#endif // _DEMOLOOPER_H_

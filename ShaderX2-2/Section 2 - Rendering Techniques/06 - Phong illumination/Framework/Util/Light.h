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


#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "../Math/Vertex.h"
#include "../Math/Color.h"

class Light {
protected:
	Vertex position;
	float size;
	Color color;

	Vertex coronaDir;
	float coronaSizeFactor;
	float lastVisibleTime;
	float lastHiddenTime;
	bool corona;
	bool doubleSided;
public:
	Light(){}
	Light(const Vertex &pos, const Color &col, const float Size);


    void setPosition(const Vertex &pos){ position = pos; }
    void setColor(const Color &col){ color = col; }
    void setSize(const float Size){ size = Size; }

	const Vertex &getPosition() const {	return position; }
	const Color &getColor() const { return color; }
	const float getSize() const { return size; }
	const bool hasCorona() const { return corona; }

	void setCorona(const Vertex &CoronaDir, const float CoronaSizeFactor, const bool DoubleSided);
	void setLastTime(const bool visible, const float time);

	const float getCoronaSize(const Vertex &pos, const float currTime);
};

#endif // _LIGHT_H_

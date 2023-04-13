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


#include "Light.h"

Light::Light(const Vertex &pos, const Color &col, const float Size){
	position = pos;
	color = col;
	size = Size;
	corona = false;
}

void Light::setCorona(const Vertex &CoronaDir, const float CoronaSizeFactor, const bool DoubleSided){
	coronaDir = CoronaDir;
	coronaDir.normalize();
	coronaSizeFactor = CoronaSizeFactor;
	corona = true;
	doubleSided = DoubleSided;
}

void Light::setLastTime(const bool visible, const float time){
	if (visible){
		lastVisibleTime = time;
	} else {
		lastHiddenTime = time;
	}
}

const float Light::getCoronaSize(const Vertex &pos, const float currTime){
	Vertex lightVector = pos - position;
	float factor, angular, len = length(lightVector);
	lightVector /= len;
	
	if (lastVisibleTime >= lastHiddenTime){
		factor = 0.00005f * (currTime - lastHiddenTime);
		if (factor > 1) factor = 1;
	} else {
		factor = 1 + 0.00005f * (lastVisibleTime - currTime);
		if (factor < 0) factor = 0;
	}
	
	angular = lightVector * coronaDir;
	if (doubleSided) angular = fabsf(angular);
	return factor * size * coronaSizeFactor * sinf(PI - PI * len / (len + 700.0f)) * angular;
}

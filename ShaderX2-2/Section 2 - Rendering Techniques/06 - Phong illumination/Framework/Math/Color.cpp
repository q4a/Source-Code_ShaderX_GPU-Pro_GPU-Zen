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

#include "Color.h"

Color::Color(const float red, const float green, const float blue){
	r = red;
	g = green;
	b = blue;
	a = 1.0f;
}

Color::Color(const float red, const float green, const float blue, const float alpha){
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

unsigned int Color::getRGBA() const {
	unsigned int col;
	int tmp;
	
	tmp = int(r * 255);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	col = tmp;
	tmp = int(g * 255);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	col |= tmp << 8;
	tmp = int(b * 255);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	col |= tmp << 16;
	tmp = int(a * 255);
	if (tmp < 0  ) tmp = 0;
	if (tmp > 255) tmp = 255;
	col |= tmp << 24;
	return col;
}

void Color::operator += (const Color &v){
	r += v.r;
	g += v.g;
	b += v.b;
	a += v.a;
}

void Color::operator -= (const Color &v){
	r -= v.r;
	g -= v.g;
	b -= v.b;
	a -= v.a;
}


Color operator + (const Color &u, const Color &v){
	return Color(u.r + v.r,u.g + v.g,u.b + v.b,u.a + v.a);
}

Color operator - (const Color &u, const Color &v){
	return Color(u.r - v.r,u.g - v.g,u.b - v.b,u.a - v.a);
}

Color operator * (const Color &u, const Color &v){
	return Color(u.r * v.r, u.g * v.g, u.b * v.b,u.a * v.a);
}

Color operator * (const float scalar, const Color &u){
	return Color(scalar * u.r, scalar * u.g, scalar * u.b, scalar * u.a);
}

Color operator * (const Color &u, const float scalar){
	return Color(scalar * u.r, scalar * u.g, scalar * u.b, scalar * u.a);
}

Color operator / (const Color &u, const float diuidend){
	return Color(u.r / diuidend,u.g / diuidend,u.b / diuidend, u.a / diuidend);
}

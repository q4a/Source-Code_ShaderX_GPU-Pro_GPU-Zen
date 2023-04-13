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

#ifndef _QUATERNION_H_
#define _QUATERNION_H_

//#include "Matrix.h"

struct Quaternion {
	float s,x,y,z;

	Quaternion(){}
	Quaternion(const float Wx, const float Wy);
	Quaternion(const float Wx, const float Wy, const float Wz);
	Quaternion(const float is, const float ix, const float iy, const float iz){
		s = is;
		x = ix;
		y = iy;
		z = iz;
	}

	operator const float *() const {
		return &s;
	}

	void operator += (const Quaternion &v);
	void operator -= (const Quaternion &v);
	void operator *= (const float scalar);
	void operator /= (const float dividend);
	void normalize();
	void fastNormalize();
};

Quaternion operator + (const Quaternion &u, const Quaternion &v);
Quaternion operator - (const Quaternion &u, const Quaternion &v);
Quaternion operator - (const Quaternion &v);
Quaternion operator * (const Quaternion &u, const Quaternion &v);
Quaternion operator * (const float scalar, const Quaternion &v);
Quaternion operator * (const Quaternion &v, const float scalar);
Quaternion operator / (const Quaternion &v, const float dividend);

Quaternion slerp(const Quaternion &q0, const Quaternion &q1, const float t);
Quaternion scerp(const Quaternion &q0, const Quaternion &q1, const Quaternion &q2, const Quaternion &q3, const float t);


#endif // _QUATERNION_H_

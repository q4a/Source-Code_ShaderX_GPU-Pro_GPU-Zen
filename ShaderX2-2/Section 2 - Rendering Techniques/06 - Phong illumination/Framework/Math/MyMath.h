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


#ifndef _MYMATH_H_
#define _MYMATH_H_

#include <math.h>

#define PI 3.14159265358979323846f

// This code supposedly originates from Id-software
// It makes a fast 1 / sqrtf(v) approximation
inline float rsqrtf(float v){
    float v_half = v * 0.5f;
    long i = *(long *) &v;
    i = 0x5f3759df - (i >> 1);
    v = *(float *) &i;
    return v * (1.5f - v_half * v * v);
}

inline float sqrf(const float x){
    return x * x;
}

inline float sincf(const float x){
	return (x == 0)? 1 : sinf(x) / x;
}

#define roundf(x) floorf((x) + 0.5f)

inline float intAdjustf(const float x, const float diff = 0.01f){
	float f = roundf(x);

	return (fabsf(f - x) < diff)? f : x;
}

inline float lerpf(const float a, const float b, const float x){
	return a + x * (b - a);
}

inline float cerpf(const float a, const float b, const float c, const float d, const float x){
	float p = (d - c) - (a - b);
	float q = (a - b) - p;
	float r = c - a;
	return x * (x * (x * p + q) + r) + b;
}

inline unsigned int getClosestPowerOfTwo(const unsigned int x){
	int i,k;
	
	k = x;
	i = -1;
	while (k != 0){
		k >>= 1;
		i++;
	}
	return 1 << (i + ((x >> (i-1)) & 1));
}

#endif // _MYMATH_H_

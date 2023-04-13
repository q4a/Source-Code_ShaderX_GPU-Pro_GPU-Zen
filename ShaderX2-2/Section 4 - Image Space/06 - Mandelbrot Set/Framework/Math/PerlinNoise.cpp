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


/*
	Credits:

	This file is based on the great work done by Ken Perlin.
	http://mrl.nyu.edu/~perlin/doc/oscar.html
*/


#include <stdlib.h>
#include "PerlinNoise.h"
#include <math.h>


float bias(const float a, const float b){
	return powf(a, logf(b) / logf(0.5f));
}

float gain(const float a, const float b){
	if (a < 0.001f)	return 0; else if (a > 0.999f) return 1.0f;

	float p = logf(1.0f - b) / logf(0.5f);

	if (a < 0.5f) return float(powf(2 * a, p) / 2);
			 else return 1 - float(powf(2 * (1.0f - a), p) / 2);
}

/* noise functions over 1, 2, and 3 dimensions */

#define B 0x1000
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static float g2[B + B + 2][2];
static float g1[B + B + 2];

#define s_curve(t) (t * t * (3 - 2 * t))


#define lerp(t, a, b) (a + t * (b - a))

#define setup(i,b0,b1,r0,r1)\
	t = i + N;\
	b0 = ((int) t) & BM;\
	b1 = (b0 + 1) & BM;\
	r0 = t - (int) t;\
	r1 = r0 - 1;

float noise1(const float x){
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v;

	setup(x, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[p[bx0]];
	v = rx1 * g1[p[bx1]];

	return lerp(sx, u, v);
}

#define at2(rx,ry) (rx * q[0] + ry * q[1])

float noise2(const float x, const float y){
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	setup(x, bx0,bx1, rx0,rx1);
	setup(y, by0,by1, ry0,ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);


	q = g2[b00]; u = at2(rx0,ry0);
	q = g2[b10]; v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[b01]; u = at2(rx0,ry1);
	q = g2[b11]; v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

#define at3(rx,ry,rz) (rx * q[0] + ry * q[1] + rz * q[2])

float noise3(const float x, const float y, const float z){
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	setup(x, bx0,bx1, rx0,rx1);
	setup(y, by0,by1, ry0,ry1);
	setup(z, bz0,bz1, rz0,rz1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

	q = g3[b00 + bz0]; u = at3(rx0,ry0,rz0);
	q = g3[b10 + bz0]; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[b01 + bz0]; u = at3(rx0,ry1,rz0);
	q = g3[b11 + bz0]; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[b00 + bz1]; u = at3(rx0,ry0,rz1);
	q = g3[b10 + bz1]; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[b01 + bz1]; u = at3(rx0,ry1,rz1);
	q = g3[b11 + bz1]; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

static void normalize2(float v[2]){
	float s = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1]);
	v[0] *= s;
	v[1] *= s;
}

static void normalize3(float v[3]){
	float s = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
}

float turbulence(const float x, const float y, const float z, float freq){
	float t = 0.0f;

	do {
		t += fabsf(noise3(freq * x, freq * y, freq * z)) / freq;
		freq *= 0.5f;
	} while (freq >= 1.0f);
	
	return t;
}

void initPerlin(){
	int i, j, k;

	for (i = 0; i < B; i++) {
		p[i] = i;

		g1[i] = (float) ((rand() % (B + B)) - B) / B;

		for (j = 0; j < 2; j++)
			g2[i][j] = (float) ((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0; j < 3; j++)
			g3[i][j] = (float) ((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0; i < B + 2; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0; j < 2; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0; j < 3; j++)
			g3[B + i][j] = g3[i][j];
	}
}

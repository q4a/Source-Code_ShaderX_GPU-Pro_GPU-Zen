///
///		Copyright (c) 2003 F. Brebion (f.brebion@vrcontext.com)
///
///		Licence/disclaimer: This demo is provided 'as-is', without any express or implied
///		warranty. In no event can i be held liable for any damages arising from the use
///		of that code or this demo.
///
///		Permission is granted to anyone to use the code for any purpose, including
///		commercial usage, and to alter it and redistribute it freely, subject to the
///		following restrictions:
///
///		1. Proper credits must be given to the author for his contribution to the code
///		if the code is totally or partially reused. If you implement the described technic
///		yourself, credits are appreciated but not required. I'd also appreciate it
///		if you send me an email saying how you're going to use the code for.
///
///		2. The 3D models and textures in this demo belong to Daniel Cornibert
///		(danielcornibert@hotmail.com) and are included in it as pure curtesy from his
///		part. PERMISSION TO USE THIS ART IS NOT GRANTED by this license.
///

#ifndef __UTILS
#define __UTILS

void warning(char *txt);

#define copy3f(a, b) memcpy(a, b, sizeof(float) * 3)
#define copy4f(a, b) memcpy(a, b, sizeof(float) * 4)
#define set3f(a, v) { (a)[0] = (v); (a)[1] = (v); (a)[2] = (v); }
#define set4f(a, v) { (a)[0] = (v); (a)[1] = (v); (a)[2] = (v); (a)[3] = (v); }
#define add3f(a, b) { (a)[0] += (b)[0]; (a)[1] += (b)[1]; (a)[2] += (b)[2]; }
#define sub3f(a, b) { (a)[0] -= (b)[0]; (a)[1] -= (b)[1]; (a)[2] -= (b)[2]; }
#define scale3f(a, v) { (a)[0] *= (v); (a)[1] *= (v); (a)[2] *= (v); }
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define FFAIL(x) if (FAILED(x)) return false;
#define TFAIL(x) if (FAILED(x)) return true;
#define RFAIL(x) if (FAILED(x)) return;
#define EFAIL(x) if (FAILED(x)) exit(1);

float dot(float x1, float y1, float z1, float x2, float y2, float z2);
float dot(const float *v0, const float *v1);
void cross(float x1, float y1, float z1, float x2, float y2, float z2,
    float& x, float& y, float& z);
float clamp(const float v);
void normalize(float& x, float& y, float& z);

bool rayTriIntersect(const float *start, const float *end, const float *v1, const float *v2,
    const float *v3, float *t, float *u, float *v);
void calculatePlane(float *plane, const float *v0, const float *v1, const float *v2);
void calculatePlane(float *plane, const float *p, const float *n);
bool vecEq(const float *v0, const float *v1);
float vecLength(const float *v);
float vecDist(const float *v0, const float *v1);
float vecDist2(const float *v0, const float *v1);
bool raySphereIntersect(const float *start, const float *dirNorm, const float *center,
    const float radius, float *t1, float *t2);
void closestPointOnLine(const float *a, const float *b, const float *p, float *res);
void closestPointOnTri(const float *a, const float *b, const float *c, const float *p,
    float *res);
bool rayPlaneIntersect(const float *rayO, const float *rayDir, const float *planeO,
    const float *planeN, float *t);
bool pointIsInTriangle(const float *p, const float *v0, const float *v1, const float *v2);
void projectPointOnPlane(const float *plane, float *p);
void normalizePlane(float *plane);
float randfs();
bool pointIsInEllipsoid(const float *p, const float *o, const float *r);
float rayEllipsoidIntersect(const float *start, const float *dirNorm, const float *center,
    const float *radius);
float distanceToPlane(const float *plane, const float *p);
void initTime();
float getTime();
void vecRotateX(float *v, const float cosa, const float sina);
void vecRotateY(float *v, const float cosa, const float sina);
void vecRotateZ(float *v, const float cosa, const float sina);

#endif

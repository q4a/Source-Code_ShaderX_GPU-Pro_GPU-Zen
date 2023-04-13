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

#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "CUtils.h"

void warning(char *txt)
{
    OutputDebugString(txt);
    FILE *f = fopen("log.txt", "a");
    fwrite(txt, strlen(txt), 1, f);
    fclose(f);
}

float dot(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return(x1 * x2 + y1 * y2 + z1 * z2);
}

float dot(const float *v0, const float *v1)
{
    return(v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2]);
}

void cross(float x1, float y1, float z1, float x2, float y2, float z2,
    float& x, float& y, float& z)
{
    x = y1 * z2 - z1 * y2;
    y = z1 * x2 - x1 * z2;
    z = x1 * y2 - y1 * x2;
}

float clamp(const float v)
{
    if (v < 0)
        return(0);
    if (v > 1)
        return(1);
    return(v);
}

void normalize(float& x, float& y, float& z)
{
    float l = sqrtf(x * x + y * y + z * z);
    if (l > 0.0001f)
    {
        x = x / l;
        y = y / l;
        z = z / l;
    }
}

bool rayTriIntersect(const float *start, const float *end, const float *v1, const float *v2,
    const float *v3, float *t, float *u, float *v)
{
    float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
    float det, inv_det, dir[3];

    /* find vectors for two edges sharing vert0 */
    edge1[0] = v2[0] - v1[0];
    edge1[1] = v2[1] - v1[1];
    edge1[2] = v2[2] - v1[2];
    edge2[0] = v3[0] - v1[0];
    edge2[1] = v3[1] - v1[1];
    edge2[2] = v3[2] - v1[2];
    dir[0] = end[0] - start[0];
    dir[1] = end[1] - start[1];
    dir[2] = end[2] - start[2];

    /* begin calculating determinant - also used to calculate U parameter */
    cross(dir[0], dir[1], dir[2], edge2[0], edge2[1], edge2[2], pvec[0], pvec[1], pvec[2]);

    /* if determinant is near zero, ray lies in plane of triangle */
    det = dot(edge1[0], edge1[1], edge1[2], pvec[0], pvec[1], pvec[2]);

    if ((det > -0.0001f) && (det < 0.0001f))
        return false;

    inv_det = 1.0f / det;

    /* calculate distance from vert0 to ray origin */
    tvec[0] = start[0] - v1[0];
    tvec[1] = start[1] - v1[1];
    tvec[2] = start[2] - v1[2];

    /* calculate U parameter and test bounds */
    *u = dot(tvec[0], tvec[1], tvec[2], pvec[0], pvec[1], pvec[2]) * inv_det;
    if ((*u < 0.0f) || (*u > 1.0f))
        return false;

    /* prepare to test V parameter */
    cross(tvec[0], tvec[1], tvec[2], edge1[0], edge1[1], edge1[2], qvec[0], qvec[1], qvec[2]);

    /* calculate V parameter and test bounds */
    *v = dot(dir[0], dir[1], dir[2], qvec[0], qvec[1], qvec[2]) * inv_det;
    if ((*v < 0.0f) || (*u + *v > 1.0f))
        return false;

    /* calculate t, ray intersects triangle */
    *t = dot(edge2[0], edge2[1], edge2[2], qvec[0], qvec[1], qvec[2]) * inv_det;

    return true;
}

void calculatePlane(float *plane, const float *v0, const float *v1, const float *v2)
{
    plane[0] = v0[1]*(v1[2]-v2[2]) + v1[1]*(v2[2]-v0[2]) + v2[1]*(v0[2]-v1[2]);
	plane[1] = v0[2]*(v1[0]-v2[0]) + v1[2]*(v2[0]-v0[0]) + v2[2]*(v0[0]-v1[0]);
	plane[2] = v0[0]*(v1[1]-v2[1]) + v1[0]*(v2[1]-v0[1]) + v2[0]*(v0[1]-v1[1]);
	plane[3] = -( v0[0]*( v1[1]*v2[2] - v2[1]*v1[2] ) +
				v1[0]*(v2[1]*v0[2] - v0[1]*v2[2]) +
				v2[0]*(v0[1]*v1[2] - v1[1]*v0[2]) );
}

bool vecEq(const float *v0, const float *v1)
{
    return(fabsf(v0[0] - v1[0]) <= 0.01f && fabsf(v0[1] - v1[1]) <= 0.01f
        && fabsf(v0[2] - v1[2]) <= 0.01f);
}

float vecLength(const float *v)
{
    return(sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
}

float vecDist(const float *v0, const float *v1)
{
    float v[3];
    v[0] = v1[0] - v0[0];
    v[1] = v1[1] - v0[1];
    v[2] = v1[2] - v0[2];
    return(vecLength(v));
}

float vecDist2(const float *v0, const float *v1)
{
    float v[3];
    v[0] = v1[0] - v0[0];
    v[1] = v1[1] - v0[1];
    v[2] = v1[2] - v0[2];
    return(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

bool raySphereIntersect(const float *start, const float *dirNorm, const float *center,
    const float radius, float *t1, float *t2)
{
    float b = 2.0f * (dirNorm[0] * (start[0] - center[0]) + dirNorm[1] *
        (start[1] - center[1]) + dirNorm[2] * (start[2] - center[2]));
    float c = (start[0] - center[0]) * (start[0] - center[0]) +
        (start[1] - center[1]) * (start[1] - center[1]) +
        (start[2] - center[2]) * (start[2] - center[2]) - radius * radius;

    float coef = b * b - 4 * c;
    if (coef < 0)
    {
        *t1 = -1;
        *t2 = -1;
        return(false);
    }

    *t1 = ((-b) - sqrtf(coef)) / 2.0f;
    *t2 = ((-b) + sqrtf(coef)) / 2.0f;
    return(true);
}

void closestPointOnLine(const float *a, const float *b, const float *p, float *res)
{
    float c[3];
    c[0] = p[0] - a[0];
    c[1] = p[1] - a[1];
    c[2] = p[2] - a[2];
    float v[3];
    v[0] = b[0] - a[0];
    v[1] = b[1] - a[1];
    v[2] = b[2] - a[2];
    normalize(v[0], v[1], v[2]);
    float d = vecDist(a, b);
    float t = dot(v, c);

    if (t < 0)
    {
        res[0] = a[0];
        res[1] = a[1];
        res[2] = a[2];
        return;
    }

    if (t > d)
    {
        res[0] = b[0];
        res[1] = b[1];
        res[2] = b[2];
        return;
    }

    res[0] = a[0] + v[0] * t;
    res[1] = a[1] + v[1] * t;
    res[2] = a[2] + v[2] * t;
}

void closestPointOnTri(const float *a, const float *b, const float *c, const float *p,
    float *res)
{
    float rab[3];
    closestPointOnLine(a, b, p, rab);
    float rbc[3];
    closestPointOnLine(b, c, p, rbc);
    float rca[3];
    closestPointOnLine(c, a, p, rca);

    float dab = vecDist(rab, p);
    float dbc = vecDist(rbc, p);
    float dca = vecDist(rca, p);

    if (dab < dbc && dab < dca)
    {
        res[0] = rab[0];
        res[1] = rab[1];
        res[2] = rab[2];
    }
    else if (dbc < dab && dbc < dca)
    {
        res[0] = rbc[0];
        res[1] = rbc[1];
        res[2] = rbc[2];
    }
    else
    {
        res[0] = rca[0];
        res[1] = rca[1];
        res[2] = rca[2];
    }
}

bool rayPlaneIntersect(const float *rayO, const float *rayDir, const float *planeO,
    const float *planeN, float *t)
{
    float d = -(dot(planeN, planeO));
    float numer = dot(planeN, rayO) + d;
    float denom = dot(planeN, rayDir);
    if (denom == 0)
        return(false);
    
    *t = -(numer / denom);
    return(true);
}

bool pointIsInTriangle(const float *p, const float *v0, const float *v1, const float *v2)
{
    /// angle1 = angle v0 P v1
    float d0[3], d1[3];

    copy3f(d0, v0);
    sub3f(d0, p);
    copy3f(d1, v1);
    sub3f(d1, p);
    normalize(d0[0], d0[1], d0[2]);
    normalize(d1[0], d1[1], d1[2]);
    float angle01 = acosf(dot(d0, d1));

    copy3f(d0, v1);
    sub3f(d0, p);
    copy3f(d1, v2);
    sub3f(d1, p);
    normalize(d0[0], d0[1], d0[2]);
    normalize(d1[0], d1[1], d1[2]);
    float angle12 = acosf(dot(d0, d1));

    copy3f(d0, v2);
    sub3f(d0, p);
    copy3f(d1, v0);
    sub3f(d1, p);
    normalize(d0[0], d0[1], d0[2]);
    normalize(d1[0], d1[1], d1[2]);
    float angle20 = acosf(dot(d0, d1));

    float sum = angle01 + angle12 + angle20;
    return(fabsf(sum - 3.14159279f * 2.0f) <= 0.001f);
}

void calculatePlane(float *plane, const float *p, const float *n)
{
    copy3f(plane, n);
    plane[3] = -(plane[0] * p[0] + plane[1] * p[1] + plane[2] * p[2]);
}

void normalizePlane(float *plane)
{
    float l = vecLength(plane);
    if (l > 0)
    {
        l = 1.0f / l;
        plane[0] *= l;
        plane[1] *= l;
        plane[2] *= l;
        plane[3] *= l; // yeah THAT IS NEEDED!!
    }
}

void projectPointOnPlane(const float *plane, float *p)
{
    float d = (plane[0] * p[0] + plane[1] * p[1] + plane[2] * p[2] + plane[3]);

    p[0] = p[0] - d * plane[0];
    p[1] = p[1] - d * plane[1];
    p[2] = p[2] - d * plane[2];
}

/// Returns a random float in the [-1 - 1] range
float randfs()
{
    float v = (float)((rand() % 16383) / 16383.0f);
    return(-1.0f + v * 2.0f);
}

bool pointIsInEllipsoid(const float *p, const float *o, const float *r)
{
    float dx = (p[0] - o[0]) / r[0];
    float dy = (p[1] - o[1]) / r[1];
    float dz = (p[2] - o[2]) / r[2];

    return(dx * dx + dy * dy + dz * dz <= 1.00001f);
}

float rayEllipsoidIntersect(const float *start, const float *dirNorm, const float *center,
    const float *radius)
{
    static float nullCenter[3] = {0, 0, 0};
    float newStart[3];
    float newDir[3];

    copy3f(newStart, start);
    copy3f(newDir, dirNorm);
    sub3f(newStart, center);
    
    newStart[0] /= radius[0];
    newStart[1] /= radius[1];
    newStart[2] /= radius[2];
    newDir[0] /= radius[0];
    newDir[1] /= radius[1];
    newDir[2] /= radius[2];
    normalize(newDir[0], newDir[1], newDir[2]);

    float t1, t2;
    if (!raySphereIntersect(newStart, newDir, nullCenter, 1.0f, &t1, &t2))
        return(-1.0f);

    float t = t1;
    if (t2 >= 0 && t1 < 0)
        t = t2;
    if (t1 >= 0 && t2 < 0)
        t = t1;
    if (t1 >= 0 && t2 >= 0)
        t = MIN(t1, t2);

    if (t < 0)
        /// not found..:(
        return(-1.0f);

    float intersect[3];
    intersect[0] = newStart[0] + t * newDir[0];
    intersect[1] = newStart[1] + t * newDir[1];
    intersect[2] = newStart[2] + t * newDir[2];
    intersect[0] *= radius[0];
    intersect[1] *= radius[1];
    intersect[2] *= radius[2];
    add3f(intersect, center);

    return(vecDist(start, intersect));
}

float distanceToPlane(const float *plane, const float *p)
{
    return(plane[0] * p[0] + plane[1] * p[1] + plane[2] * p[2] + plane[3]);
}

/// Time stuff 

__int64 timeFreq = 0;
__int64 timeStart = 0;

void initTime()
{
	LARGE_INTEGER li = { 0 };
	if (QueryPerformanceFrequency(&li) == 0)
	{
		timeFreq = 0;
	}
    else
    {
        timeFreq = li.QuadPart;

        LARGE_INTEGER t;
	    QueryPerformanceCounter(&t);
        timeStart = t.QuadPart;
    }
}

float getTime()
{
    if (timeFreq > 0)
    {
    	LARGE_INTEGER t;
	    QueryPerformanceCounter(&t);
        return((float)((double)(t.QuadPart - timeStart) / (double)timeFreq));
    }
    else
    {
        return((float)timeGetTime() / 1000.0f);
    }
}

void vecRotateX(float *v, const float cosa, const float sina)
{
    float temp[3];
    temp[0] = v[0];
    temp[1] = (v[1] * cosa) + (v[2] * -sina);
    temp[2] = (v[1] * sina) + (v[2] * cosa);
    copy3f(v, temp);
}

void vecRotateY(float *v, const float cosa, const float sina)
{
    float temp[3];
    temp[0] = (v[0] * cosa) + (v[2] * sina);
    temp[1] = v[1];
    temp[2] = (v[0] * -sina) + (v[2] * cosa);
    copy3f(v, temp);
}

void vecRotateZ(float *v, const float cosa, const float sina)
{
    float temp[3];
    temp[0] = (v[0] * cosa) + (v[1] * -sina);
    temp[1] = (v[0] * sina) + (v[1] * cosa);
    temp[2] = v[2];
    copy3f(v, temp);
}

///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgVec3
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgVec3.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgVec3.h"
#include <pgLib/Math/pgPlane.h>

#include <math.h>

#pragma warning(disable: 4514)

#define MAX3(x,y,z) ( ((x>y)&&(x>z)) ? x :( ((y>x)&&(y>z))? y : z) )
#define MID3(x,y,z) ( ((x>y)&&(x<z)) ? x :( ((y>x)&&(y<z))? y : z) )
#define MIN3(x,y,z) ( ((x<y)&&(x<z)) ? x :( ((y<x)&&(y<z))? y : z) )


float 
pgVec3::length() const
{
	return (float)sqrt( x*x + y*y + z*z );
}


float
pgVec3::distance(const pgVec3& left, const pgVec3& right)
{
	return (float)sqrt(pow(left.x-right.x,2) + pow(left.y-right.y,2) + pow(left.z-right.z,2));
}


float
pgVec3::normalize()
{
    float l = length(), il;
	if (l!=0.0f)
	{
		il = 1.0f / l;
		x *= il;
		y *= il;
		z *= il;
	}
    return ( l );
}


pgVec3& 
pgVec3::cross( const pgVec3& v1, const pgVec3& v2 )
{
    x = v1.y*v2.z - v1.z*v2.y;
    y = v1.z*v2.x - v1.x*v2.z;
    z = v1.x*v2.y - v1.y*v2.x;
    return *this;
}


float
pgVec3::cosine(const pgVec3& left,const pgVec3& right)
{
	float dif = (right-left).length() * 0.5f;
	if (dif<0.00001f)
		return 0.0f;
	return 2.0f*(float)asin(dif);
}


pgVec3
pgVec3::normal()
{
	float rx,ry;
	rx = MAX3(x,y,z);
	ry = MID3(x,y,z);
	return pgVec3(-ry,rx,0);
}


float 
pgVec3::dot(const pgVec3& nV) const
{
	return x*nV.x + y*nV.y + z*nV.z;
}


bool
pgVec3::isInfrontOf(const pgPlane& nPlane) const
{
	return nPlane.x*x + nPlane.y*y + nPlane.z*z > nPlane.d;
}

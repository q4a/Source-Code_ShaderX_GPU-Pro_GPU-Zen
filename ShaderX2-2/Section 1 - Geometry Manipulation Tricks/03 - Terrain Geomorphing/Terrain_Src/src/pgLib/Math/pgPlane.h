///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgPlane
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgPlane.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __PLANE_HEADERFILE__
#define __PLANE_HEADERFILE__


#include <pgLib//pgLib.h>
#include "pgVec3.h"
#include "pgVec3n.h"

//struct D3DXPLANE;
#include <d3dx8.h>

/// This class implements a plane
class PGLIB_API pgPlane
{
public:
    float x, y, z, d;

    pgPlane()  { x=y=z=d=0.0f; }
    pgPlane(float nX, float nY, float nZ, float nD)  { x=nX; y=nY; z=nZ; d=nD;  normalize(); }
	pgPlane(const pgPlane& tocopy)  { *this =  tocopy;}
	pgPlane(const float *nVec)  { set(nVec); }
	pgPlane(const pgVec3& nVec1, const pgVec3& nVec2, const pgVec3& nVec3)  {  set(nVec1, nVec2, nVec3);  }
	pgPlane(const pgVec3& nPos, const pgVec3& nNormal)  {  x = nNormal.x;  y = nNormal.y;  z = nNormal.z;
														   d = x*nPos.x + y*nPos.y + z*nPos.z;  normalize();  }
	pgPlane(const pgVec3& nPos, const pgVec3n& nNormal)  {  x = nNormal.x;  y = nNormal.y;  z = nNormal.z;
															d = x*nPos.x + y*nPos.y + z*nPos.z;  }


	D3DXPLANE*
		getD3DPlane()  {  return reinterpret_cast<D3DXPLANE*>(this);  }

	const D3DXPLANE*
		getD3DPlane() const  {  return reinterpret_cast<const D3DXPLANE*>(this);  }

	void
		set(D3DXPLANE* nPlane)  {  *reinterpret_cast<D3DXPLANE*>(this) = *nPlane;  }

	void
		set(const float *nVec)  { x = nVec[0];  y = nVec[1];  z = nVec[2];  d = nVec[3]; }

	void
		set(float nX, float nY, float nZ, float nD)  { x = nX;  y = nY;  z = nZ;  d = nD;  normalize(); }

	inline void
		set(const pgVec3& nVec1, const pgVec3& nVec2, const pgVec3& nVec3);

    const float& 
		getX() const { return x; }
    const float& 
		getY() const { return y; }
    const float& 
		getZ() const { return z; }
    const float& 
		getD() const { return d; }


	inline void
		normalize();
	

	inline pgPlane& 
		operator=(const pgPlane&);
	friend inline bool 
		operator==(const pgPlane& left, const pgPlane& right);
    const float& 
		operator[](unsigned index) const  { return (&x)[index]; }
    float& 
		operator[](unsigned index)  { return (&x)[index]; }
};


inline pgPlane&
pgPlane::operator=(const pgPlane& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	d = other.d;
	return *this;
}


inline void
pgPlane::normalize()
{
	float l = x*x + y*y + z*z;

	if(l==1.0f || l==0.0f)
		return;

	l = (float)sqrt(l);

	float il = 1.0f/l;

	x *= il;							// normalize without
	y *= il;							// moving the plane
	z *= il;
	d *= l;
}


inline bool 
operator==(const pgPlane& left, const pgPlane& right)
{
    return (left.x==right.x && left.y==right.y && left.z==right.z && left.d==right.d);
}


// pass in the vertices clockwise order
//
inline void
pgPlane::set(const pgVec3& nVec1, const pgVec3& nVec2, const pgVec3& nVec3)
{
	pgVec3	v12 = nVec2-nVec1,
			v13 = nVec3-nVec1,
			normal;

	normal.cross(v13, v12);
	normal.normalize();

	x = normal.x;
	y = normal.y;
	z = normal.z;
	d = normal * nVec1;
}


#endif //__PLANE_HEADERFILE__

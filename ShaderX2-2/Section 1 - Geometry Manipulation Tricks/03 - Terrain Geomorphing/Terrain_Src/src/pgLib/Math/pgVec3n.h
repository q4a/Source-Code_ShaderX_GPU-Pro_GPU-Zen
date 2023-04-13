///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgVec3n
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgVec3n.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef	__VEC3N_HEADERFILE__
#define __VEC3N_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Math/pgVec3.h>


/// 3D normalized vector class
class PGLIB_API pgVec3n : public pgVec3
{
public:
	bool valid;

	//constructors
	pgVec3n()  { x=y=0.0f;z=1.0f; valid = true; }
    pgVec3n(float nX, float nY, float nZ)  { set(nX,nY,nZ); }
	pgVec3n(const pgVec3n& tocopy)  { *this = tocopy;}
	pgVec3n(const pgVec3& tocopy, bool nNormaize=true)  { if(nNormaize) set(tocopy.x,tocopy.y,tocopy.z); else setNoNorm(tocopy.x,tocopy.y,tocopy.z); }

	bool
		isValid() const  { return valid; }

	void 
		set(float nX, float nY, float nZ);

	// setNoNorm lets you specify a unit vector
	// without normalization
	void
		setNoNorm(float nX, float nY, float nZ)  {  x = nX;  y = nY;  z = nZ;  valid = true;  }

	inline pgVec3n& 
		operator=(const pgVec3n&);

	const pgVec3n&
		getDifferentVector() const;

	//standard vectors
	static pgVec3n ORIGIN;
	static pgVec3n FRONT;
	static pgVec3n BACK;
	static pgVec3n LEFT;
	static pgVec3n RIGHT;
	static pgVec3n UP;
	static pgVec3n DOWN;
};


typedef pgVec3n *pgVec3nPtr;


inline pgVec3n&
pgVec3n::operator=(const pgVec3n& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	valid = other.valid;
	return *this;
}


#endif //__VEC3N_HEADERFILE__

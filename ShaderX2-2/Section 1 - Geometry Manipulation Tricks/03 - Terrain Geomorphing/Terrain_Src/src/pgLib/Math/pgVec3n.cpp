///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgVec3n
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgVec3n.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgVec3n.h"
#include <math.h>

#pragma warning(disable: 4514)

pgVec3n pgVec3n::ORIGIN(0.0f,0.0f,0.0f);
pgVec3n pgVec3n::FRONT(0.0f,0.0f,1.0f);
pgVec3n pgVec3n::BACK(0.0f,0.0f,-1.0f);
pgVec3n pgVec3n::LEFT(-1.0f,0.0f,0.0f);
pgVec3n pgVec3n::RIGHT(1.0f,0.0f,0.0f);
pgVec3n pgVec3n::UP(0.0f,1.0f,0.0f);
pgVec3n pgVec3n::DOWN(0.0f,-1.0f,0.0f);


void 
pgVec3n::set(float nX, float nY, float nZ)
{
	x=nX;
	y=nY;
	z=nZ;
	
	float l = length();

	if(l!=0.0f)
	{
		float il = 1.0f / l;
		x *= il;
		y *= il;
		z *= il;
		valid = true;
	}
	else
		valid = false;
}


const pgVec3n&
pgVec3n::getDifferentVector() const
{
	if(fabs(*this * pgVec3n::LEFT)<0.99)
		return pgVec3n::LEFT;
	else
		return pgVec3n::DOWN;
}

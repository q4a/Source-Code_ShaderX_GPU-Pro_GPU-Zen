///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgVec4
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgVec4.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////

#pragma warning(disable: 4514)

#include "pgVec4.h"
#include <math.h>


float
pgVec4::length()
{
	return (float)sqrt( x*x + y*y + z*z + w*w );
}


void
pgVec4::makeHomogen()
{
    if(w!=1.0)
    {
        float wa=(float)fabs(w);
        x/=wa;
        y/=wa;
        z/=wa;
        w=1.0;
    }
}

float
pgVec4::distance(const pgVec4& left, const pgVec4& right)
{
	return (float)sqrt(pow(left.x-right.x,2) + pow(left.y-right.y,2) + pow(left.z-right.z,2));
}

///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgVec2
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgVec2.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#pragma warning(disable: 4514)

#include "pgVec2.h"

#include <pgLib/Math/pgIMathTool.h>

#include <assert.h>
#include <math.h>


float
pgVec2::cosine(const pgVec2& left, Axis nAxis)
{
	float alpha;

	switch(nAxis)
	{
	default:
	case X_AXIS:
		alpha = (float)acos(left[0]);
		if (left[1]<0.0f)
			alpha = pgFTWOPI - alpha;
		return alpha;
	case Y_AXIS:
		alpha = (float)acos(left[1]);
		if (left[0]>0.0f)
			alpha = pgFTWOPI - alpha;
		return alpha;
	}
	return -1.0f;
}


float
pgVec2::cosine(const pgVec2& left, const pgVec2& right)
{
	float alpha,beta,dif;

	alpha = cosine(left);
	beta = cosine(right);
	dif = beta-alpha;
	if (dif > pgFPI)
		dif = -pgFTWOPI + dif;
	else
	if (dif < -pgFPI)
		dif = pgFTWOPI + dif;
	assert(dif<=pgFPI && dif>=-pgFPI);
	return dif;
}


float
pgVec2::distance(const pgVec2& vec) const
{
    return (float)sqrt((x-vec.x)*(x-vec.x)+(y-vec.y)*(y-vec.y));
}


float pgVec2::length() const
{
    return (float)sqrt(x*x+y*y);
}


float
pgVec2::normalize()
{
    float len = x*x+y*y;
    if (len!=1.0)
    {
        if (len>0.00001f)
        {
            len = (float)sqrt(len);
            x /= len;
            y /= len;
        }
        else
            len = 0.0;
    }
    return len;
}

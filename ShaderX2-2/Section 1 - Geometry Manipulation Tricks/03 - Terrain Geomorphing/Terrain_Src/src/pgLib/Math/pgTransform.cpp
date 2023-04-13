///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTransform
// creator:     Markus Hötzinger
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTransform.cpp,v 1.6 2003/01/05 15:10:35 markus Exp $
///////////////////////////////////////////////////////////////////


#include "pgTransform.h"


pgTransform& pgTransform::operator=(const pgTransform& nT)
{
	pos	  = nT.pos;
	rot	  = nT.rot;	
	scale = nT.scale;

	dirty=true;
	return *this;	
}


pgTransform& pgTransform::operator*=(const pgTransform& nT)
{
	pos	  += nT.pos;
	rot	  *= nT.rot;
	scale.x *= nT.scale.x;
	scale.y *= nT.scale.y;
	scale.z *= nT.scale.z;

	dirty=true;
	return *this;	
}


pgTransform& pgTransform::operator>=(const pgTransform& nT)
{
	pos	  += nT.pos;
	rot	  >= nT.rot;	//rot = t.rot * rot

	scale.x *= nT.scale.x;
	scale.y *= nT.scale.y;
	scale.z *= nT.scale.z;

	dirty=true;
	return *this;	
}


const pgMatrix&
pgTransform::getMatrix()
{
	updateMatrix();
	return matrix;
}


void
pgTransform::updateMatrix()
{
	if(!dirty)
		return;

	
	pgMatrix mrot;
	rot.setMatrix(mrot);

	matrix.m00 = mrot.m00*scale.x;
	matrix.m10 = mrot.m10*scale.y;
	matrix.m20 = mrot.m20*scale.z;
	matrix.m30 = pos.x;

	matrix.m01 = mrot.m01*scale.x;
	matrix.m11 = mrot.m11*scale.y;
	matrix.m21 = mrot.m21*scale.z;
	matrix.m31 = pos.y;

	matrix.m02 = mrot.m02*scale.x;
	matrix.m12 = mrot.m12*scale.y;
	matrix.m22 = mrot.m22*scale.z;
	matrix.m32 = pos.z;

	matrix.m03 = 0.0f;
	matrix.m13 = 0.0f;
	matrix.m23 = 0.0f;
	matrix.m33 = 1.0f;
}








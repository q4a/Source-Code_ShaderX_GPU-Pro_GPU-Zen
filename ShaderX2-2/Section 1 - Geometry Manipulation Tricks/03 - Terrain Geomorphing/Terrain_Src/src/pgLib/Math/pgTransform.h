///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTransform
// creator:     Markus Hötzinger
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTransform.h,v 1.3 2002/12/18 13:27:23 markus Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TRANSFORM_HEADERFILE__
#define __TRANSFORM_HEADERFILE__


#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgQuat.h>
#include <D3d8types.h>


class PGLIB_API pgTransform
{
public:
	pgTransform() { pos.x=pos.y=pos.z=0.0f; scale.x=scale.y=scale.z=1.0f; dirty=true; }

	/***** combine Transforms ... Caution!!! Quaternion-Combination not commutative ******/
	pgTransform& operator*=(const pgTransform& t); //result= Objekt * t
	pgTransform& operator>=(const pgTransform& t); //result= t * Objekt
	pgTransform& operator= (const pgTransform& t);



	void setPosition (const pgVec3& nPos)	{pos = nPos;		dirty=true;}
	void setScale	 (const pgVec3& nScale)	{scale = nScale;	dirty=true;}
	void setRotation (const pgQuat& nRot)	{rot = nRot;		dirty=true;}

	void move   (const pgVec3& move)			{pos+=move;				dirty=true;}
	void scalef (const pgVec3& scalefactor)		{scale*=scalefactor;	dirty=true;}
	void rotate (const pgQuat& drot)			{rot*=drot;				dirty=true;}
	void rotate (const pgVec3& drot)			{pgQuat quat(drot); rotate(quat); dirty=true;}


	const pgVec3& getPosition() const  { return pos;	}
	const pgVec3& getScale	  () const  { return scale;}
	const pgQuat& getRotation() const  { return rot;	}

	void updateMatrix();

	const pgMatrix& getMatrix();


	//bool isDirty()  { return dirty; }

protected:	
	pgVec3		pos, scale;
	pgQuat		rot;

	pgMatrix	matrix;

	bool		dirty;
};


#endif	// __TRANSFORM_HEADERFILE__

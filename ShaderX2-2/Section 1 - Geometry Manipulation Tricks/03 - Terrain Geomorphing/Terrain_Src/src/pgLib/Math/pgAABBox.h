///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgAABBox
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgAABBox.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BOX_HEADERFILE__
#define __BOX_HEADERFILE__


#include <pgLib/pgLib.h>
#include "pgVec3.h"
#include "pgIMathTool.h"
#include "pgPlane.h"

#include <math.h>


/// Axis aligned bounding box
/**
 *  An axis aligned bounding box can be calculated
 *  very effectivly. A AABBox is defined by only two
 *  vertices (corners).
 */
class PGLIB_API pgAABBox		// axis aligned bounding box
{
public:

	// constructors
	pgAABBox()  {  reset();  }
	pgAABBox(const pgVec3& nMin, const pgVec3& nMax) : minVertex(nMin), maxVertex(nMax)  {}
	pgAABBox(const pgVec3& nMid, float dx, float dy, float dz) : minVertex(nMid[0]-dx/2.0f, nMid[1]-dy/2.0f, nMid[2]-dz/2.0f),
																 maxVertex(nMid[0]+dx/2.0f, nMid[1]+dy/2.0f, nMid[2]+dz/2.0f)
		{}

	/// Sets one corner of the bounding box
	void	
		setMin(const pgVec3& nVertex)  {  minVertex = nVertex;  }

	/// Sets another corner of the bounding box
	void		
		setMax(const pgVec3& nVertex)  {  maxVertex = nVertex;  }

	/// Extends the box to contain the given vertex
	inline void 
		addVertex(const pgVec3& nVertex);


	/// Extends the box to contain an other box
	inline void 
		addBox(const pgAABBox& nOther);


	// Uncreases the box, so that it would fit, if it was rotated by nAngle
	inline void
		exentedByRotationY(float nAngle);


	/// Calculates and returns the center of the box
	pgVec3
		getCenter() const  {  return (maxVertex+minVertex)*0.5f;  }


	/// Moves the box by nMove
	void move(const pgVec3& nMove)  {  minVertex += nMove;  maxVertex += nMove;  }


	/// Resets the box to contain everything
	void 
		reset()
		{
			maxVertex = pgVec3(pgFLOAT_MIN,pgFLOAT_MIN,pgFLOAT_MIN);
			minVertex = pgVec3(pgFLOAT_MAX,pgFLOAT_MAX,pgFLOAT_MAX);
		};


	/// Resets the box to be centered at nMid and to have each side of length d
	void
		setMid(const pgVec3& nMid, float d)
		{
			setMid(nMid, d,d,d);
		}

	/// Resets the box to be centered at nMid and to have size dx,dy,dz
	void
		setMid(const pgVec3& nMid, float dx, float dy, float dz)
		{
			minVertex = pgVec3(nMid[0]-dx/2.0f, nMid[1]-dy/2.0f, nMid[2]-dz/2.0f);
			maxVertex = pgVec3(nMid[0]+dx/2.0f, nMid[1]+dy/2.0f, nMid[2]+dz/2.0f);
		}


	/// returns one point of the box
	const pgVec3&		
		getMin() const  {  return minVertex;  }


	/// returns the other point of the box
	const pgVec3&		
		getMax() const  {  return maxVertex;  }


	/// returns the center of the box
	pgVec3		
		getMid() const  {  return pgVec3(minVertex[0] + (maxVertex[0] - minVertex[0]) / 2.0f,
										 minVertex[1] + (maxVertex[1] - minVertex[1]) / 2.0f,
										 minVertex[2] + (maxVertex[2] - minVertex[2]) / 2.0f);  }

	// testing functions
	//
	inline bool
		doesOverlap(const pgAABBox& nOther) const;

protected:
	pgVec3	minVertex,
			maxVertex;
};


inline 
void pgAABBox::addVertex(const pgVec3& nVertex)
{
	// is there a new max
	if (maxVertex[0] == pgFLOAT_MIN)
		maxVertex = nVertex;
	else
	{
		if (nVertex[0] > maxVertex[0])
			maxVertex[0] = nVertex[0];
		if (nVertex[1] > maxVertex[1])
			maxVertex[1] = nVertex[1];
		if (nVertex[2] > maxVertex[2])
			maxVertex[2] = nVertex[2];
	}

	// is there a new min
	if (minVertex[0] == pgFLOAT_MAX)
		minVertex = nVertex;
	else
	{
		if (nVertex[0] < minVertex[0])
			minVertex[0] = nVertex[0];
		if (nVertex[1] < minVertex[1])
			minVertex[1] = nVertex[1];
		if (nVertex[2] < minVertex[2])
			minVertex[2] = nVertex[2];
	}
}


inline void 
pgAABBox::addBox(const pgAABBox& nOther)
{
	addVertex(nOther.maxVertex);
	addVertex(nOther.minVertex);
}


inline bool
pgAABBox::doesOverlap(const pgAABBox& nOther) const
{
	return !(minVertex.x>nOther.maxVertex.x || nOther.minVertex.x>maxVertex.x ||
			 minVertex.y>nOther.maxVertex.y || nOther.minVertex.y>maxVertex.y ||
			 minVertex.z>nOther.maxVertex.z || nOther.minVertex.z>maxVertex.z);
}


inline void
pgAABBox::exentedByRotationY(float nAngle)
{
	float	cosA = (float)cos(nAngle),
			sinA = (float)sin(nAngle);
	int		i;
	pgVec3	corners[8], dest;

	corners[0].set(minVertex.x, minVertex.y, minVertex.z);
	corners[1].set(maxVertex.x, minVertex.y, minVertex.z);
	corners[2].set(minVertex.x, maxVertex.y, minVertex.z);
	corners[3].set(maxVertex.x, maxVertex.y, minVertex.z);
	corners[4].set(minVertex.x, minVertex.y, maxVertex.z);
	corners[5].set(maxVertex.x, minVertex.y, maxVertex.z);
	corners[6].set(minVertex.x, maxVertex.y, maxVertex.z);
	corners[7].set(maxVertex.x, maxVertex.y, maxVertex.z);

	for(i=0; i<8; i++)
	{
		dest.x =  corners[i].x * cosA + corners[i].z * sinA;
		dest.y =  corners[i].y;
		dest.z = -corners[i].x * sinA + corners[i].z * cosA;

		addVertex(dest);
	}
}


#endif //__BOX_HEADERFILE__

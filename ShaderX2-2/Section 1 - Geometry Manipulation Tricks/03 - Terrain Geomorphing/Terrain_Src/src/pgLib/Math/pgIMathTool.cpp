///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIMathTool
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIMathTool.cpp,v 1.4 2003/01/05 15:52:30 daniel Exp $
///////////////////////////////////////////////////////////////////


#pragma warning(disable: 4514)

#include "pgIMathTool.h"

#include <pgLib/Math/pgPlane.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgVec3n.h>

#include <string.h>
#include <math.h>


float *pgIMathTool::sinTable = NULL;
float *pgIMathTool::cosTable = NULL;
unsigned int *pgIMathTool::sqrtTable = NULL;


void
pgIMathTool::initSinCos()
{
	int i;

	if(!sinTable)
	{
		sinTable = new float[3600];
		for(i=0; i<3600; i++)
			sinTable[i] = (float)::sin(i*pgFTWOPI/360.0f/10.0f);
	}

	if(!cosTable)
	{
		cosTable = new float[3600];
		for(i=0; i<3600; i++)
			cosTable[i] = (float)::cos(i*pgFTWOPI/360.0f/10.0f);
	}
}




void 
pgIMathTool::initSqrt()
{	

	/*typedef*/ union //FastSqrtUnion
	{
		float f;
		unsigned int i;
	}s;

	unsigned int i;
//	FastSqrtUnion s;

	
	if(!sqrtTable)
	{
		sqrtTable = new unsigned int[0x10000];
		
		for (i = 0; i <= 0x7FFF; i++)
		{		
			s.i = (i << 8) | (0x7F << 23);
			s.f = (float)sqrt(s.f);
		
			sqrtTable[i + 0x8000] = (s.i & 0x7FFFFF);
			
			s.i = (i << 8) | (0x80 << 23);
			s.f = (float)sqrt(s.f);
			
			sqrtTable[i] = (s.i & 0x7FFFFF);
		}
	}
}




float
pgIMathTool::sin(float nVal)
{
	int idx = (int)(10.0f*nVal) % 3600;

	if(idx<0)
		idx += 3600;


	return sinTable[idx];
}


float
pgIMathTool::cos(float nVal)
{
	int idx = (int)(10.0f*nVal) % 3600;

	if(idx<0)
		idx += 3600;

	return cosTable[idx];
}


float 
pgIMathTool::sqrt(float nVal)
{
	#define FP_BITS(fp) (*(DWORD *)&(fp))

	if (FP_BITS(nVal) == 0) return 0.0f;                 
	
	FP_BITS(nVal) = sqrtTable[(FP_BITS(nVal) >> 8) & 0xFFFF] | ((((FP_BITS(nVal) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
	
	return nVal;
}



float
pgIMathTool::random(float nMin, float nMax)
{
	float d = nMax-nMin;
	float r = (rand() * d) / RAND_MAX;

	return nMin + r;
}


bool
pgIMathTool::isPointInSphere(const pgVec3& nPoint, const pgVec3& nSpherePos, float nRadius) {
	
	float d = (nPoint-nSpherePos).sqrLength();

	return d<=nRadius*nRadius;
}


bool
pgIMathTool::isPointInTriangle(const pgVec3& nPoint, const pgVec3& nA, const pgVec3& nB, const pgVec3& nC)
{
	double total_angles = 0.0f;
   
	pgVec3	v1 = nPoint-nA,
			v2 = nPoint-nB,
			v3 = nPoint-nC;

	v1.normalize();
	v2.normalize();
	v3.normalize();

	total_angles += acos(v1*v2);
	total_angles += acos(v2*v3);
	total_angles += acos(v3*v1);
 
	if(fabs(total_angles-pgFTWOPI) <= 0.005)
		return true;
     
  return false;
}


pgVec3
pgIMathTool::getClosestPointOnLine(const pgVec3& nPoint, const pgVec3& nA, const pgVec3& nB)
{
	// determine t (the length of the vector from ‘a’ to ‘p’)
	//
	pgVec3	c = nPoint-nA,
			V = nB-nA; 
  
	float d = V.length();
  
	V /= d;
	float t = V * c;


	// Check to see if ‘t’ is beyond the extents of the line segment
	//
	if(t<0.0f) return nA;
	if(t>d) return nB;


	// Return the point between ‘a’ and ‘b’
	//set length of V to t. V is normalized so this is easy
	V.x = V.x * t;
	V.y = V.y * t;
	V.z = V.z * t;

	return nA+V;
}


pgVec3
pgIMathTool::getClosestPointOnTriangle(const pgVec3& nPoint, const pgVec3& nA, const pgVec3& nB, const pgVec3& nC)
{
	pgVec3 Rab = getClosestPointOnLine(nPoint, nA, nB);
	pgVec3 Rbc = getClosestPointOnLine(nPoint, nB, nC);
	pgVec3 Rca = getClosestPointOnLine(nPoint, nC, nA);

	double dAB = (nPoint-Rab).length();
	double dBC = (nPoint-Rbc).length();
	double dCA = (nPoint-Rca).length();
	double min = dAB;
	pgVec3 result = Rab;

	if (dBC < min)
	{
		min = dBC;
		result = Rbc;
	}

	if (dCA < min)
		result = Rca;

	return (result);	
}


float
pgIMathTool::getDistanceToPlane(const pgVec3& nPos, const pgPlane& nPlane)
{
	return nPos.x*nPlane.x + nPos.y*nPlane.y + nPos.z*nPlane.z - nPlane.d;
}

bool
pgIMathTool::isPointBehindPlane(const pgVec3& nPos, const pgPlane& nPlane)
{
	return (nPos.x*nPlane.x + nPos.y*nPlane.y + nPos.z*nPlane.z) <= nPlane.d+pgEPS;
}


bool
pgIMathTool::isPointBehindPlanes(const pgVec3& nPos, int nNumPlanes, const pgPlane* nPlanes)
{
	for(int i=0; i<nNumPlanes; i++)
		if((nPos.x*nPlanes[i].x + nPos.y*nPlanes[i].y + nPos.z*nPlanes[i].z) > nPlanes[i].d+pgEPS)
			return false;

	return true;
}


bool
pgIMathTool::isPointOnPlane(const pgVec3& nPos, const pgPlane& nPlane)
{
	float d = nPos.x*nPlane.x + nPos.y*nPlane.y + nPos.z*nPlane.z;

	return (d>nPlane.d-pgEPS && d<nPlane.d+pgEPS);
}


bool
pgIMathTool::arePointsBehindPlane(int numPos, const pgVec3* nPos, const pgPlane& nPlane)
{
	for(int i=0; i<numPos; i++)
		if((nPos[i].x*nPlane.x + nPos[i].y*nPlane.y + nPos[i].z*nPlane.z) <= nPlane.d+pgEPS)
			return true;

	return false;
}


bool
pgIMathTool::findIntersectionRaySphere(const pgVec3& nPos0, const pgVec3n& nDir0, const pgVec3& nSpherePos, float nRadius, float& nT)
{
   pgVec3 Q = nSpherePos-nPos0;
   
   double c = Q.length();
   double v = Q * nDir0;
   double d = nRadius*nRadius - (c*c - v*v);

   if(d < 0.0)				// no intersection ?
   {
	   nT = -1.0f;
	   return false;
   }

   // Return the distance to the [first] intersecting point
   //
   nT = (float)(v - sqrt((float)d));
   return true;
}


bool
pgIMathTool::findIntersectionRayPlane(const pgPlane& nPlane, const pgVec3& nPos0, const pgVec3n& nDir, pgVec3& nPos, float& nT)
{
	pgVec3	pNorm(nPlane.x, nPlane.y, nPlane.z);

	pNorm.normalize();

	float	scalar_pnorm_dir = pNorm*nDir,
			scalar_pnorm_pos = pNorm*nPos0;

	if(scalar_pnorm_dir<pgEPS && scalar_pnorm_dir>-pgEPS)
		return false;

	nT = -(scalar_pnorm_pos - nPlane.d) / scalar_pnorm_dir;
	nPos.addScaled(nPos0, nT, nDir);

	return true;
}


bool
pgIMathTool::findIntersectionPlanes(const pgPlane& nPlane1, const pgPlane& nPlane2, const pgPlane& nPlane3, pgVec3& nPoint)
{
	pgVec3	n1(nPlane1.x, nPlane1.y, nPlane1.z),
			n2(nPlane2.x, nPlane2.y, nPlane2.z),
			n3(nPlane3.x, nPlane3.y, nPlane3.z);
	float	d1 = nPlane1.d,
			d2 = nPlane2.d,
			d3 = nPlane3.d,
			denom;

	pgVec3	c12, c23, c31;

	c12.cross(n1, n2);
	c23.cross(n2, n3);
	c31.cross(n3, n1);

	denom = n1 * c23;							//	N1 . ( N2 * N3 )

	if(denom<pgEPS && denom>-pgEPS)
		return false;

	nPoint = c23 * d1;							// d1 ( N2 * N3 ) + d2 ( N3 * N1 ) + d3 ( N1 * N2 )
	nPoint.addScaled(nPoint, d2, c31);
	nPoint.addScaled(nPoint, d3, c12);

	nPoint /= denom;
	
	return true;
}


// 
// if nPlane is NULL, the plane is calculated from the triangle corners
//
bool
pgIMathTool::findIntersectionSphereTriangle(const pgVec3& nSpherePos, float nRadius, const pgVec3& nDir,
										   const pgVec3& nCorner1, const pgVec3& nCorner2, const pgVec3& nCorner3, const pgPlane* nPlane, 
										   float& nDist, pgVec3& nColSpherePos, pgVec3& nColTrianglePos, bool& nStuck)
{
	pgVec3			dir = nDir;
	pgVec3n			planeNormal;
	float			dirLen = dir.length(), planeIDist;
	const pgPlane*	plane;
	pgPlane			tPlane;
	pgVec3			planeIPoint, sphereIPoint;

	// if this triangle is invalid (area size is zero)
	// we just skip it
	if(nCorner1==nCorner2 || nCorner1==nCorner3 || nCorner2==nCorner3)
		return false;

	dir /= dirLen;
	pgVec3n dir0(dir, false);

	if(nPlane==NULL)
	{
		tPlane.set(nCorner1, nCorner2, nCorner3);
		plane = &tPlane;
	}
	else
		plane = nPlane;

	// make a first and quick classification if the
	// sphere won't get near enough to the triangle's plane
	if(getDistanceToPlane(nSpherePos, *plane)>nRadius+dirLen)
		return false;

	planeNormal.setNoNorm(plane->getX(), plane->getY(), plane->getZ());

	if((planeNormal * dir0) > 0.0f)															// we move away from the plane (or are on the backside)
		return false;

	sphereIPoint = nSpherePos - planeNormal * nRadius;										// temporal sphere intersection point

	if(isPointBehindPlane(sphereIPoint, *plane))
	{
		if(!findIntersectionRayPlane(*plane, sphereIPoint, planeNormal, planeIPoint, planeIDist))
			return false;
	}
	else
	{
		if(!findIntersectionRayPlane(*plane, sphereIPoint, dir0, planeIPoint, planeIDist))	// no intersection at all ?
			return false;
	}


	//if(planeIDist>dirLen || planeIDist<0.0f)										// to long to go until hitting the plane (or wrong direction)
	//	return false;

	// assume the we hit the triangle (not only the plane)
	//
	pgVec3 polyIPoint = planeIPoint;
	float sphereIDist = planeIDist;

	if(!isPointInTriangle(planeIPoint, nCorner1,nCorner2,nCorner3))					// dir we hit the plane outside the triangles edges ?
	{
		polyIPoint = getClosestPointOnTriangle(planeIPoint, nCorner1,nCorner2,nCorner3);

		findIntersectionRaySphere(polyIPoint, -dir0, nSpherePos, nRadius, sphereIDist);

		if(sphereIDist>0)
			sphereIPoint = polyIPoint - dir0 * sphereIDist;
	}

	// do a short error checking to see
	// if we got ourself stuck last frame
	if(isPointInSphere(polyIPoint, nSpherePos, nRadius))
		nStuck = true;

	nColSpherePos = sphereIPoint;
	nColTrianglePos = polyIPoint;
	nDist = sphereIDist;

	return (sphereIDist>0.0 && sphereIDist<=dirLen);
}

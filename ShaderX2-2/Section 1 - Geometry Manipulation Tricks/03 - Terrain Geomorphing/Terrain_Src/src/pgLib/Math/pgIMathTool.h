///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIMathTool
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIMathTool.h,v 1.3 2003/01/05 15:10:34 markus Exp $
///////////////////////////////////////////////////////////////////


#ifndef __MATHTOOL_HEADERFILE__
#define __MATHTOOL_HEADERFILE__


#include <pgLib/pgLib.h>

#define pgFLOAT_MIN 1.175494351e-38f
#define pgFLOAT_MAX 3.402823466e+38f

#define pgINT_MIN -(2147483647+1)
#define pgINT_MAX 2147483647

#define pgEPS 0.00001f
#define pgEPSL 0.05f
#define pgFPI 3.14159265f
#define pgFTWOPI 6.28318531f
#define pgFTWOPIRAND pgFTWOPI / RAND_MAX
#define pgDECTORAD pgFPI/180.0f

#define pg360RAND 360.0f / RAND_MAX


class pgPlane;
class pgVec3;
class pgVec3n;


/// pgIMathTool provides mathematical routines for 3d calculations
class PGLIB_API pgIMathTool
{
public:
	/// Initializes the tables used by sin() and cos()
	/**
	 *  This method must be called before sin() or cos() can be called.
	 */
	static void
		initSinCos();

	/// Initializes the tables used by sqrt()
	/**
	 *  This method must be called before sqrt() can be called.
	 */
	static void
		initSqrt();


	/// Uses a sine table for fast sinus calculation
	/**
	 *  Returns the sine of nVal
	 *  CAUTION: nVal has to be passed in degrees
	 */
	static float
		sin(float nVal);


	/// Uses a cosine table for fast calculation
	/**
	 *  Returns the cosine of nVal
	 *  CAUTION: nVal has to be passed in degrees
	 */
	static float
		cos(float nVal);

	
	/// Uses a sqrt table for fast sqrt calculation
	static float
		sqrt(float n);


	/// Calculates a random number that lies between nMin and nMax
	static float
		random(float nMin, float nMax);

	/// Returns true if nPoint lies in the sphere
	static bool
		isPointInSphere(const pgVec3& nPoint, const pgVec3& nSpherePos, float nRadius);


	/// Returns true if the point lies inside the triangle
	/**
	 *  It's assumed that the point always lies in the triangles plane
	 */
	static bool
		isPointInTriangle(const pgVec3& nPoint, const pgVec3& nA, const pgVec3& nB, const pgVec3& nC);


	/// Returns the closest point from nPoint on the given line
	static pgVec3
		getClosestPointOnLine(const pgVec3& nPoint, const pgVec3& nA, const pgVec3& nB);


	/// Returns the closest point from nPoint which lies on the triangles edges.
	/**
	 *  It's assumed that nPoint lies NOT inside the triangle
	 */
	static pgVec3
		getClosestPointOnTriangle(const pgVec3& nPoint, const pgVec3& nA, const pgVec3& nB, const pgVec3& nC);


	/// Returns the signed distance of the given position to the plane
	static float
		getDistanceToPlane(const pgVec3& nPos, const pgPlane& nPlane);


	/// Returns true if a point lies behind a plane (planes are always directed)
	static bool
		isPointBehindPlane(const pgVec3& nPos, const pgPlane& nPlane);


	/// Returns true if a point lies behind all planes
	static bool
		isPointBehindPlanes(const pgVec3& nPos, int nNumPlanes, const pgPlane* nPlanes);


	/// Returns true if a point lies on the plane
	static bool
		isPointOnPlane(const pgVec3& nPos, const pgPlane& nPlane);


	/// Returns true if at least one of the points lies behind the plane
	static bool
		arePointsBehindPlane(int numPos, const pgVec3* nPos, const pgPlane& nPlane);


	/// Finds the intersection of a ray and a sphere
	/**
	 *  Returns the length of the ray in nT
	 *  Returns true if the ray really hit the sphere
	 */
	static bool
		findIntersectionRaySphere(const pgVec3& nPos0, const pgVec3n& nDir0, const pgVec3& nSpherePos, float nRadius, float& nT);


	/// Finds the intersection of a ray and a plane.
	/**
	 *  Returns the position in parameter pos and signed distance of nPos0 and pos in nT
	 *  Returns true as return value if there was an intersection
	 */
	static bool
		findIntersectionRayPlane(const pgPlane& nPlane, const pgVec3& nPos0, const pgVec3n& nDir, pgVec3& nPos, float& nT);


	/// Finds the intersection of three planes
	/** Returns the point in nPoint
	 *  Returns true if nPoint is valid
	 */
	static bool
		findIntersectionPlanes(const pgPlane& nPlane1, const pgPlane& nPlane2, const pgPlane& nPlane3, pgVec3& nPoint);


	/// Finds the intersection of the given sphere moving into direction nDir with the given triangle.
	/**
	 *  If the sphere did already collide with the triangle nStuck is set to true
	 *  If nPlane is not NULL it must lie inside the triangle
	 *  Returns: the distance the sphere moves until it collides
	 *           the position on the sphere, that will collide with the triangle
	 *           the position on the triangle that will collide with the sphere
	 *          sets nStuck to true if there was already a collision on the inital position
	 */
	static bool
		findIntersectionSphereTriangle(const pgVec3& nSpherePos, float nRadius, const pgVec3& nDir,
									   const pgVec3& nCorner1, const pgVec3& nCorner2, const pgVec3& nCorner3, const pgPlane* nPlane, 
									   float& nDist, pgVec3& nColSpherePos, pgVec3& nColTrianglePos, bool& nStuck);
protected:
	static float *sinTable;
	static float *cosTable;
	static unsigned int *sqrtTable;  // declare table of square roots 
};


#endif //__MATHTOOL_HEADERFILE__

///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgPathLinear
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgPathLinear.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __PATHLINEAR_HEADERFILE__
#define __PATHLINEAR_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Math/pgPath.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgString.h>

/// This class implements the basic pgPath interface.
/**
 *  It provides a path contructed of linear subsection
 *  (polyline). the rotation can be interpolated if requested.
 */
class PGLIB_API pgPathLinear : public pgPath
{
public:
	pgPathLinear();

	/// Loads a path from the file
	/**
	 *  This method load a path from a file
	 *  Each line must contain three float values.
	 *  In order to support 3dsMAX the y & z values
	 *  are exchanged.
	 */
	bool load(const char* nFileName);


	/// Enables interpolation of the rotation
	/**
	 *  Not implemented yet.
	 */
	void setInterpolateRotation(bool nEnable)  {  interpolRot = nEnable;  }


	/// Returns the number of corners, which build this linear path
	int getNumCorners() const  {  return posList.getSize();  }


	/// Returns a specific corner
	const pgVec3& getCorner(int nIndex) const  {  return posList[nIndex];  }


	// implementation of pgPath interface
	virtual pgVec3 getPosition(float nPos);
	virtual pgVec3n getDirection(float nPos);
	virtual int getPosDir(float nPos, pgVec3& nPosition, pgVec3n& nDirection);
	virtual float getLength() const  {  return lenList.getTail();  }

private:
	pgString						name;
	bool							interpolRot;
	pgList<pgVec3>	posList;
	pgList<float>	lenList;
};

#endif //__PATHLINEAR_HEADERFILE__

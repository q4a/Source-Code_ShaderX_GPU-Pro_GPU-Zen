///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgPath
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgPath.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __PATH_HEADERFILE__
#define __PATH_HEADERFILE__


#include <pgLib/pgLib.h>
#include "pgVec3n.h"


/// This class defines the basic path interface which all path classes have to implement.
/**
 *  Some of the get-methods are not constant, since
 *  a path might want to cache request data
 *  (section of last request, etc...)
 */
class PGLIB_API pgPath
{
public:
	/// Types of paths
	enum TYPE  {
					TYPE_NONE,		/**<  Undefined Path Type  */
					TYPE_LINEAR		/**<  Linear Path  */
	};


	/// Returns the interpolated position on the path
	/**
	 *  nPos must be in range [0.0-1.0]
	 */
	virtual pgVec3 getPosition(float nPos) = 0;


	/// Returns the interpolated rotation on the path
	/**
	 *  nPos must be in range [0.0-1.0]
	 */
	virtual pgVec3n getDirection(float nPos) = 0;


	/// Returns the interpolated position and rotation
	/**
	 *  This method is far more efficient than calling
	 *  getPosition() and getRotation() seperately
	 *  nPos must be in range [0.0-1.0]
	 *  Returns the index of the base vertex used
	 */
	virtual int getPosDir(float nPos, pgVec3& nPosition, pgVec3n& nDirection) = 0;


	/// Returns the type of the path
	virtual TYPE getType() const  {  return type;  }


	/// Returns the length of the path
	virtual float getLength() const = 0;

protected:
	TYPE	type;
};

typedef pgPath* pgPathPtr;

#endif //__PATH_HEADERFILE__

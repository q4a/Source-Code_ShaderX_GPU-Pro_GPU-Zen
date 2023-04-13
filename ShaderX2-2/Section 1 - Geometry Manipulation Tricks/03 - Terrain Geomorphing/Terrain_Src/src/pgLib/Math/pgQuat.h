///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgQuat
// creator:     Markus Hötzinger
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgQuat.h,v 1.4 2002/12/15 20:34:32 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __QUAT_HEADERFILE__
#define __QUAT_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgMatrix.h>


#pragma warning (disable: 4201)		// disable warning about nameless struct/union


class PGLIB_API pgQuat
{
public:
	pgQuat () { x=y=z=w=0.0f; }
	pgQuat (float xval, float yval, float zval, float wval );
	pgQuat (const pgQuat& other);
	pgQuat (const pgVec3& dir);


    ///////////// operators ////////////////////////////////////////////////////////////////
	pgQuat& operator= ( const pgQuat& quat	);
	pgQuat  operator* ( const pgQuat& quat	);

	pgQuat& operator*=( const pgQuat& quat	);	//result= object * quat
	pgQuat& operator>=( const pgQuat& quat	);	//result= quat * object
    	
	bool	operator==( const pgQuat& quat	);
	bool	operator!=( const pgQuat& quat	);


	///////////// Member Functions //////////////////////////////////////////////////////////
	void setMatrix (pgMatrix& mat);

	void set (const pgMatrix& mat);
	void set (const pgVec3& dir); 

	void makeRotX (float nX);
	void makeRotY (float nY);
	void makeRotZ (float nZ);

	float dot(const pgQuat& left,const pgQuat& right);

	float getX() const  {  return x;  }

	float getY() const  {  return y;  }

	float getZ() const  {  return z;  }

	float getW() const  {  return w;  }


	union
	{
		struct
		{
			float x,y,z,w;
		};
		float i[4];
    };
};




#endif // __QUAT_HEADERFILE__



///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgVec3
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgVec3.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __VEC3_HEADERFILE__
#define __VEC3_HEADERFILE__


#include <pgLib/pgLib.h>
#include <assert.h>


class pgPlane;


/// 3D vector class
class PGLIB_API pgVec3
{
public:
    float x, y, z;

	//constructors
    pgVec3()  {  x=y=z=0.0f;  }
    pgVec3(float nX, float nY, float nZ )  {  x=nX; y=nY; z=nZ;  }
	pgVec3(const pgVec3& tocopy)  {  *this =  tocopy;  }
	pgVec3(const float *nVec)  {  set(nVec);  }

	void
		set(const float *nVec) { x = nVec[0];y = nVec[1];z = nVec[2]; }

	void
		set(float nX, float nY, float nZ)  { x = nX;  y = nY;  z = nZ; }

	void
		setLength(float nLength)  {  (*this)*=(nLength/length());  }

    const float& 
		getX() const { return x; }
    const float& 
		getY() const { return y; }
    const float& 
		getZ() const { return z; }

    float 
		length() const;
    float 
		sqrLength() const  { return x*x + y*y + z*z; }
	float
		normalize();

    pgVec3& 
		cross( const pgVec3& v1, const pgVec3& v2 );
	static float
		cosine(const pgVec3& left,const pgVec3& right);

	/// Returns the dot product between this vec3 and nV
    inline float 
		dot(const pgVec3& nV) const;

	inline bool
		isInfrontOf(const pgPlane& nPlane) const;

	//returns one normal vector 
    inline pgVec3
		normal();

	inline void
		negate()  {  x = -x;  y = -y;  z = -z;  }

    inline pgVec3& 
		add(const pgVec3& v1, const pgVec3& v2);

    inline pgVec3& 
		addScaled(const pgVec3& v1, float s, const pgVec3& v2);

	inline pgVec3& 
		operator=( const pgVec3& );

    inline pgVec3& 
		operator+=( const pgVec3& );
    inline pgVec3& 
		operator-=( const pgVec3& );
    inline pgVec3& 
		operator*=( const pgVec3&  );
    inline pgVec3& 
		operator*=( float mult );
    inline pgVec3& 
		operator/=( float div );

    inline pgVec3 
		operator+( float value );
    inline pgVec3 
		operator-( float value );
    inline pgVec3 
		operator*( float value );

	friend inline bool 
		operator==(const pgVec3& left, const pgVec3& right);
	friend inline bool 
		operator<(const pgVec3& left, const pgVec3& right);
	friend inline bool 
		operator>(const pgVec3& left, const pgVec3& right);
	friend inline bool 
		operator<=(const pgVec3& left, const pgVec3& right);
	friend inline bool 
		operator>=(const pgVec3& left, const pgVec3& right);

	static float
		distance(const pgVec3& left, const pgVec3& right);

    friend inline const pgVec3
		operator+(const pgVec3& left, const pgVec3& right);
    friend inline const pgVec3 
		operator-(const pgVec3& left, const pgVec3& right);
    friend inline const pgVec3 
		operator+(const pgVec3& right);
    friend inline const pgVec3 
		operator-(const pgVec3& right);
    friend inline float
		operator*(const pgVec3& left, const pgVec3& right);
    friend inline float
		operator/(const pgVec3& left, const pgVec3& right);
    friend inline pgVec3 
		operator*(const pgVec3& left, float right);
    friend inline pgVec3 
		operator/(const pgVec3& left, float right );

    const float& 
		operator[](unsigned index) const { return (&x)[index]; }
    float& 
		operator[](unsigned index) { return (&x)[index]; }
};


/// Direction in euler values
typedef pgVec3 pgEuler;

typedef pgVec3 *pgVec3Ptr;


inline
pgVec3& pgVec3::add(const pgVec3& vec1, const pgVec3& vec2)
{
    x = vec1.x+vec2.x;
    y = vec1.y+vec2.y;
	z = vec1.z+vec2.z;
    return *this;
}


inline
pgVec3& pgVec3::addScaled(const pgVec3& vec1, float s, const pgVec3& vec2)
{
    x = vec1.x+s*vec2.x;
    y = vec1.y+s*vec2.y;
	z = vec1.z+s*vec2.z;
    return *this;
}


inline pgVec3&
pgVec3::operator=( const pgVec3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

inline pgVec3&
pgVec3::operator+=(const pgVec3& vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

inline
pgVec3& pgVec3::operator-=(const pgVec3& vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

inline
pgVec3& pgVec3::operator*=(float scale)
{
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
}

inline pgVec3& 
pgVec3::operator*=( const pgVec3& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
	return *this;
}

inline pgVec3& 
pgVec3::operator/=(float div)
{
    x /= div;
    y /= div;
    z /= div;
    return *this;
}

inline bool 
operator==(const pgVec3& left, const pgVec3& right)
{
    return ( left.x == right.x
          && left.y == right.y
          && left.z == right.z );
}

inline bool 
operator<(const pgVec3& left, const pgVec3& right)
{
    return ( left.x < right.x
          && left.y < right.y
          && left.z < right.z );
}

inline bool 
operator>(const pgVec3& left, const pgVec3& right)
{
    return ( left.x > right.x
          && left.y > right.y
          && left.z > right.z );
}

inline bool 
operator<=(const pgVec3& left, const pgVec3& right)
{
    return ( left.x <= right.x
          && left.y <= right.y
          && left.z <= right.z );
}

inline bool 
operator>=(const pgVec3& left, const pgVec3& right)
{
    return ( left.x >= right.x
          && left.y >= right.y
          && left.z >= right.z );
}

inline const pgVec3
operator+(const pgVec3& left, const pgVec3& right)
{
	return pgVec3(left.x+right.x,left.y+right.y,left.z+right.z);
}

inline const pgVec3
operator-(const pgVec3& left, const pgVec3& right)
{
	return pgVec3(left.x-right.x,left.y-right.y,left.z-right.z);
}

inline const pgVec3
operator+(const pgVec3& right)
{
	return pgVec3(right.x,right.y,right.z);
}

inline const pgVec3
operator-(const pgVec3& right)
{
	return pgVec3(-right.x,-right.y,-right.z);
}

inline float
operator*(const pgVec3& left, const pgVec3& right)
{
    return left.x*right.x + left.y*right.y + left.z*right.z;
}

inline float
operator/(const pgVec3& left, const pgVec3& right)
{
    return left.x/right.x + left.y/right.y + left.z/right.z;
}

inline pgVec3 
pgVec3::operator+( float value )
{
	return pgVec3(x+value,y+value,z+value);
}

inline pgVec3 
pgVec3::operator-( float value )
{
	return pgVec3(x-value,y-value,z-value);
}

inline pgVec3 
pgVec3::operator*( float value )
{
	return pgVec3(x*value,y*value,z*value);
}

inline pgVec3 
operator*( const pgVec3& left, float value )
{
	return pgVec3(left.x*value,left.y*value,left.z*value);
}

inline pgVec3 
operator/( const pgVec3& left, float value )
{
	assert(value!=0.0f);
	return pgVec3(left.x/value,left.y/value,left.z/value);
}


#endif //__VEC3_HEADERFILE__

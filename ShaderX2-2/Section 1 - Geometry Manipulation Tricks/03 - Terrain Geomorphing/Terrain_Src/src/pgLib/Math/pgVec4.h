///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgVec4
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgVec4.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __VEC4_HEADERFILE__
#define __VEC4_HEADERFILE__


#include <pgLib/pgLib.h>


/// 4D vector class
class PGLIB_API pgVec4
{
public:
    float x, y, z, w;
    typedef float float4[4];

    pgVec4() { x=y=z=w=0.0f;  }
    pgVec4( float nX, float nY, float nZ , float nW ) { x=nX; y=nY; z=nZ;w = nW; }              
    pgVec4(const float4& xyzw) { x = xyzw[0]; y = xyzw[1]; z = xyzw[2]; w = xyzw[3]; }

    pgVec4& operator/=(float div)      { x /= div; y /= div; z /= div; w /= div; return *this; }
    pgVec4& operator*=(float scale)    { x *= scale; y *= scale; z *= scale; w *= scale; return *this; }

    const float& getX() const { return x; }
    const float& getR() const { return x; }
    const float& getY() const { return y; }
	const float& getG() const { return y; }
    const float& getZ() const { return z; }
	const float& getB() const { return z; }
    const float& getW() const { return w; }
	const float& getA() const { return w; }

    float length();
    float sqrLength()  {  return x*x + y*y + z*z + w*w;  }

    friend pgVec4 operator*(const pgVec4& vec, float scale);
    friend pgVec4 operator*(float scale, const pgVec4& vec);

	static float pgVec4::distance(const pgVec4& left, const pgVec4& right);

    void makeHomogen();

    const float& 
		operator[](unsigned index) const { return (&x)[index]; }
    float& 
		operator[](unsigned index) { return (&x)[index]; }

	inline pgVec4&
		operator=( const pgVec4& );
    inline pgVec4& 
		operator+=( const pgVec4& );
    inline pgVec4& 
		operator-=( const pgVec4& );
    inline pgVec4& 
		operator*=( const pgVec4&  );

	friend inline bool 
		operator==(const pgVec4& left, const pgVec4& right);

    friend inline const pgVec4
		operator+(const pgVec4& left, const pgVec4& right);
    friend inline const pgVec4 
		operator-(const pgVec4& left, const pgVec4& right);
    friend inline const pgVec4
		operator*(const pgVec4& left, const pgVec4& right);
    friend inline const pgVec4 
		operator/(const pgVec4& left, const pgVec4& right);

    friend inline const pgVec4 
		operator+(const pgVec4& right);
    friend inline const pgVec4 
		operator-(const pgVec4& right);
};


/// Color defined by quadruple of [0.0-1.0] float values
typedef pgVec4 pgColor;

typedef pgVec4 *pgVec4Ptr;


inline
pgVec4 operator*(const pgVec4& vec, float scale)
{
    return pgVec4(vec.x*scale, vec.y*scale, vec.z*scale, vec.w*scale);
}


inline
pgVec4 operator*(float scale, const pgVec4& vec)
{
    return pgVec4(scale*vec.x, scale*vec.y, scale*vec.z, scale*vec.w);
}


inline pgVec4& 
pgVec4::operator+=( const pgVec4& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}


inline pgVec4& 
pgVec4::operator-=( const pgVec4& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}


inline pgVec4& 
pgVec4::operator*=( const pgVec4&  other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;
	return *this;
}


inline bool 
operator==(const pgVec4& left, const pgVec4& right)
{
    return ( left.x == right.x
          && left.y == right.y
		  && left.z == right.z
          && left.w == right.w );
}


inline const pgVec4
operator+(const pgVec4& left, const pgVec4& right)
{
	return pgVec4(left.x+right.x,left.y+right.y,left.z+right.z,left.w+right.w);
}


inline const pgVec4
operator-(const pgVec4& left, const pgVec4& right)
{
	return pgVec4(left.x-right.x,left.y-right.y,left.z-right.z,left.w-right.w);
}


inline const pgVec4
operator*(const pgVec4& left, const pgVec4& right)
{
	return pgVec4(left.x*right.x,left.y*right.y,left.z*right.z,left.w*right.w);
}


inline const pgVec4
operator/(const pgVec4& left, const pgVec4& right)
{
	return pgVec4(left.x/right.x,left.y/right.y,left.z/right.z,left.w/right.w);
}


inline const pgVec4
operator+(const pgVec4& right)
{
	return pgVec4(right.x,right.y,right.z,right.w);
}


inline const pgVec4
operator-(const pgVec4& right)
{
	return pgVec4(-right.x,-right.y,-right.z,-right.w);
}


inline pgVec4&
pgVec4::operator=(const pgVec4& vec)
{
    x = vec.x;
    y = vec.y;
	z = vec.z;
	w = vec.w;
    return *this;
}


#endif //__VEC4_HEADERFILE__

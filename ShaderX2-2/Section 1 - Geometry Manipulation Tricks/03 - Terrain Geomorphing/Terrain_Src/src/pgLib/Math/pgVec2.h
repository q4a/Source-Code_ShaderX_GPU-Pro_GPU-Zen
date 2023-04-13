///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgVec2
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgVec2.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __VEC2_HEADERFILE__
#define __VEC2_HEADERFILE__


#include <pgLib/pgLib.h>


/// 2D vector class
class PGLIB_API pgVec2
{
public:
	enum Axis { X_AXIS, Y_AXIS };

    float x, y;

	/// Creates a default vec2 (0,0)
	pgVec2();


	/// Creates a vec2 from to floats
    pgVec2(float x,float y);


	/// Creates a vec2 from a pointer to two floats
    pgVec2(float *nValues)  {  set(nValues);  }


	/// Creates a vec2 as a copy from an other one
	pgVec2(const pgVec2& other);


	/// access to members (x,y) by indices
    float& operator[]( unsigned index ) { return (&x)[index]; }


	/// access to members (x,y) by indices
    const float& operator[]( unsigned index ) const { return (&x)[index]; }


	/// copy a vec2 from an other
    inline pgVec2& operator=(const pgVec2&);


	/// addition same as for normal value type
    inline pgVec2 operator+() const;


	/// add an other vec2 to this one
    inline pgVec2& operator+=(const pgVec2&);


	/// substraction same as for normal value type
    inline pgVec2 operator-() const;


	/// substract an other vec2 to this one
    inline pgVec2& operator-=(const pgVec2&);


	/// multiply this vec2 with a float
    inline pgVec2& operator*=(float mult);


	/// divide this vec2 by a float
    inline pgVec2& operator/=(float div);

    friend inline pgVec2
		operator+(const pgVec2& left, const pgVec2& right);
    friend inline pgVec2 
		operator-(const pgVec2& left, const pgVec2& right);
    friend inline pgVec2 
		operator/(const pgVec2& left, const pgVec2& right);
	friend inline bool 
		operator==(const pgVec2& left, const pgVec2& right);

    friend inline float
		operator*(const pgVec2& left, const pgVec2& right);

	
	/// Adds two vec2 and saves the result in this vec2
    inline pgVec2& 
		add(const pgVec2& v1, const pgVec2& v2);


	/// Adds two vec2 scaling the second one and saves the result in this vec2
    inline pgVec2& 
		addScaled(const pgVec2& v1, float s, const pgVec2& v2);


	/// Returns true if this vec2 and nV are at maximum nTol different
    inline bool 
		almostEqual(const pgVec2& nV, float nTol) const;


	/// Adds two vec2 scaling both and saves the result in this vec2
    inline pgVec2& 
		combine(float s1, const pgVec2& v1, float s2, const pgVec2& v2);


	/// Same a operator=()
    inline pgVec2& 
		copy(const pgVec2& v);


	/// Returns the arithmetic distance between this vec2 and nV
    float 
		distance(const pgVec2& nV) const;


	/// Returns the squared arithmetic distance between this vec2 and nV
    inline float 
		sqrDistance(const pgVec2& nV) const;


	/// Returns the dot product between this vec2 and nV
    inline float 
		dot(const pgVec2& nV) const;


	/// Same as operator==()
    inline bool 
		equal(const pgVec2& v) const;


	/// Returns the length of this vec2
    float 
		length() const;


	/// Negates this vec2
    inline void
		negate();


	/// Normalizes this vec2
    float      
		normalize();


	/// Multiplies this vec2 by nS
    inline pgVec2& 
		scale(float nS);


	/// Multiplies nV by nS and stores the result in this vec2
    inline pgVec2& 
		scale(float nS, const pgVec2& nV);


	/// Multiplies this vector component-wise by tensor
    inline pgVec2& 
		scaleBy( const pgVec2& tensor );


	/// Sets new x and y values
    inline void 
		set(float x, float y);


	/// Sets new x and y values
    inline void 
		set(const float xy[2]);


	/// Subtracts two vec2 and stores the result in this vec2
    inline pgVec2& 
		sub(const pgVec2& v1, const pgVec2& v2);


	/// Returns the angle between the vector and one axis
	static float
		cosine(const pgVec2& left, Axis nAxis = X_AXIS);

	/// Return the angle between the two vectors
	static float
		cosine(const pgVec2& left,const pgVec2& right);
};

typedef pgVec2 *pgVec2Ptr;


inline
pgVec2::pgVec2()
{
    x = 0.0f;
    y = 0.0f;
}

inline
pgVec2::pgVec2(float ix, float iy)
{
    x = ix;
    y = iy;
}

inline
pgVec2::pgVec2(const pgVec2& other)
{
    x = other.x;
    y = other.y;
}

inline
pgVec2& pgVec2::operator=(const pgVec2& vec)
{
    x = vec.x;
    y = vec.y;
    return *this;
}


inline
pgVec2 pgVec2::operator+() const
{
    return *this;
}


inline
pgVec2& pgVec2::operator+=(const pgVec2& vec)
{
    x += vec.x;
    y += vec.y;
    return *this;
}


inline
pgVec2 pgVec2::operator-() const
{
    return pgVec2(-x, -y);
}


inline
pgVec2& pgVec2::operator-=(const pgVec2& sub)
{
    x -= sub.x;
    y -= sub.y;
    return *this;
}


inline
pgVec2 operator*(const pgVec2& vec, float s)
{
    return pgVec2(vec.x*s, vec.y*s);
}


inline
pgVec2 operator*(float s, const pgVec2& vec)
{
    return pgVec2(s*vec.x, s*vec.y);
}


inline
pgVec2 operator/(const pgVec2& vec, float div)
{
    return pgVec2(vec.x/div, vec.y/div);
}


inline
pgVec2& pgVec2::operator*=(float mult)
{
    x *= mult;
    y *= mult;
    return *this;
}


inline
pgVec2& pgVec2::operator/=(float div)
{
    x /= div;
    y /= div;
    return *this;
}

inline pgVec2
operator+(const pgVec2& left, const pgVec2& right)
{
	return pgVec2(left.x+right.x,left.y+right.y);
}

inline pgVec2 
operator-(const pgVec2& left, const pgVec2& right)
{
	return pgVec2(left.x-right.x,left.y-right.y);
}

inline pgVec2 
operator/(const pgVec2& left, const pgVec2& right)
{
	return pgVec2(left.x/right.x,left.y/right.y);
}

inline bool 
operator==(const pgVec2& left, const pgVec2& right)
{
    return ( left.x == right.x && left.y == right.y);
}

inline bool 
operator!=(const pgVec2& left, const pgVec2& right)
{
    return ( left.x!=right.x
          || left.y!=right.y );
}

inline float
operator*(const pgVec2& left, const pgVec2& right)
{
	return (left[0]*right[0]+left[1]*right[1]);
}

inline
pgVec2& pgVec2::add(const pgVec2& vec1, const pgVec2& vec2)
{
    x = vec1.x+vec2.x;
    y = vec1.y+vec2.y;
    return *this;
}


inline
pgVec2& pgVec2::addScaled(const pgVec2& vec1, float s, const pgVec2& vec2)
{
    x = vec1.x+s*vec2.x;
    y = vec1.y+s*vec2.y;
    return *this;
}


inline
bool pgVec2::almostEqual(const pgVec2& vec, float tol) const
{
    float dx = x-vec.x;
    float dy = y-vec.y;
    return (dx>-tol && dx<tol && dy>-tol && dy<tol);
}


inline
bool almostEqual(const pgVec2 vec1, const pgVec2 vec2, float epsilon)
{
    return vec1.almostEqual(vec2, epsilon);
}


inline
pgVec2& pgVec2::combine(float s1, const pgVec2& vec1, float s2, const pgVec2& vec2)
{
    x = s1*vec1.x+s2*vec2.x;
    y = s1*vec1.y+s2*vec2.y;
    return *this;
}


inline
pgVec2& pgVec2::copy(const pgVec2& vec)
{
    x = vec.x;
    y = vec.y;
    return *this;
}


inline
float pgVec2::sqrDistance(const pgVec2& vec) const
{
    return (x-vec.x)*(x-vec.x)
          +(y-vec.y)*(y-vec.y);
}


inline
float pgVec2::dot(const pgVec2& vec) const
{
    return (x*vec.x+y*vec.y);
}


inline
float Dot(const pgVec2& vec1, const pgVec2& vec2)
{
    return (vec1.x*vec2.x+vec1.y*vec2.y);
}


inline
bool pgVec2::equal(const pgVec2& vec) const
{
    return ( x==vec.x && y==vec.y );
}


inline
void pgVec2::negate()
{
    x = -x;
	y = -y;
}


inline
pgVec2& pgVec2::scale(float s)
{
    x *= s;
    y *= s;
    return *this;
}


inline
pgVec2& pgVec2::scale(float s, const pgVec2& vec)
{
    x = s*vec.x;
    y = s*vec.y;
    return *this;
}


inline
pgVec2& pgVec2::scaleBy( const pgVec2& tensor )
{
    x *= tensor.x;
    y *= tensor.y;
    return *this;
}


inline
void pgVec2::set(float ix, float iy)
{
    x = ix;
    y = iy;
}


inline
void pgVec2::set(const float xy[])
{
    x = xy[0];
    y = xy[1];
}


inline
pgVec2& pgVec2::sub(const pgVec2& vec1, const pgVec2& vec2)
{
    x = vec1.x-vec2.x;
    y = vec1.y-vec2.y;
    return *this;
}


#endif //__VEC2_HEADERFILE__

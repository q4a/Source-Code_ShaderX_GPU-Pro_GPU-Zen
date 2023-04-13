//////////////////////////////////////////////////////////////////
//
// Vector
//
//	Copyright GP
// Created 3.6.2000
//
//////////////////////////////////////////////////////////////////

#ifndef __VECTOR__
#define __VECTOR__

class TStream;

class XDLL TVector {
public:
   union {
      float                            f[4];
      //
      struct {
         float                         x,y,z,w;
      };
      struct {
         float                         R,G,B,A;
      };
   };
   //
public:
                                       TVector(){};
                                       TVector(float a)
                                       {
                                          x=a;
                                          y=a;
                                          z=a;
                                          w=1;
                                       }
                                       TVector(float ax,float ay,float az=0,float aw=1)
                                       {
                                          x=ax;
                                          y=ay;
                                          z=az;
                                          w=aw;
                                       }
                                       TVector(const TVector &v)
                                       {
                                          x=v.x;
                                          y=v.y;
                                          z=v.z;
                                          w=v.w;
                                       }
public:
	void                                Set(float x1,float y1=0,float z1=0,float w1=1);
   // indexed access
   float                               &operator [] (int i);
   float                               operator () (int i) const;
public:
   // assignment
	// =
	TVector                             &operator = ( float val );
	TVector                             &operator = ( const TVector& v);
   // 
	TVector                             &operator += ( const  TVector& v );
	TVector                             &operator -= ( const  TVector& v );
   //
	TVector                             &operator *= ( float d );
	TVector                             &operator /= ( float d );
   //
	TVector                             operator + ( const TVector &v ) const;
	TVector                             operator - ( const TVector &v ) const;
	TVector                             operator - () const
   {
      return TVector( -x , -y , -z );
   }
public:
	// *
	TVector                             operator * ( float d) const;
	TVector                             operator / ( float d) const;
public:
   // dot and cross product
	float                               operator * ( const TVector &v ) const;
	TVector                             operator % ( const TVector &v ) const;
public:
	// qualification
	int                                 operator == ( const TVector &v ) const;
	int                                 operator == ( const float d ) const;
   //
	int                                 operator != ( const TVector &v ) const;
	int                                 operator != ( const float d ) const;
   //
	int                                 operator > ( const TVector &v ) const;
	int                                 operator > ( float d ) const;
   //
	int                                 operator < ( const TVector &v ) const;
	int                                 operator < ( float d ) const;
   //
	int                                 operator >= ( const TVector &v ) const;
	int                                 operator >= ( float d ) const;
   //
	int                                 operator <= ( const TVector &v ) const;
	int                                 operator <= ( float d ) const;
   //
   int                                 IsSameAs( float f,float error=1E-5) const;
   int                                 IsSameAs( const TVector &v,float error=1E-5) const;
public:
	// Square length
	float					                  SqrLength() const;
	float					                  Length() const;
	TVector                             &Normalize(float f=1);
	TVector                             &Neg();
   // converts vector to int (floorf)
   TVector                             &Int();
   // part multiply (x*x,y*y,z*z)
   TVector                             &MulPart(const TVector &a);
   //
	int					                  MinIdx() const  { return( x<y ? (x<z ? 0:2) : (y<z ? 1:2) ); }
	int					                  MaxIdx() const  { return( x>y ? (x>z ? 0:2) : (y>z ? 1:2) ); }
public:
   int                                 asColor() const;
   TVector                             &fromColor(int color);
};

//////////////////////////////////////////////////////////////////////
// Vector operators
//////////////////////////////////////////////////////////////////////

inline TVector operator * ( float d , const TVector &v )
{
   return v*d;
}

//////////////////////////////////////////////////////////////////////
// Inline for all
//////////////////////////////////////////////////////////////////////

inline void TVector::Set(float x1,float y1,float z1,float w1)
{
	x=x1;  
   y=y1;  
   z=z1; 
   w=w1;
}
//
inline float &TVector::operator [] (int i) 
{ 
   #if _DEBUG
      if( i<0 || i>3 ) MAbort("TVector::operator[] error: index out of range.");
   #endif
   //
   return f[i]; 
}
//
inline float TVector::operator () (int i) const
{ 
   #if _DEBUG
      if( i<0 || i>3 ) MAbort("TVector::operator() error: index out of range.");
   #endif
   //
   return f[i]; 
}

inline int TVector::asColor() const
{
   return (((int)(A*0xff) << 24) + ((int)(R*0xff) << 16) + ((int)(G*0xff) << 8) + ((int)(B*0xff)));
}
inline TVector &TVector::fromColor(int color)
{
   R=((color >> 16 )&255) / 255.f;
   G=((color >>  8 )&255) / 255.f;
   B= (color&255) / 255.f;
   A=((color >> 24 )&255) / 255.f;
   //
   return *this;
}

//////////////////////////////////////////////////////////////////////
// Inline
//////////////////////////////////////////////////////////////////////

#ifdef   _MATH_SSE
   #include "vectorsse.inl"
#elif    _MATH_3DNOW
   #include "vector3dnow.inl"
#else    
   #include "vectornormal.inl"
#endif

/////////////////////////////////////////////////////////////
//
// Misc functionality routines
//
/////////////////////////////////////////////////////////////

inline TVector		Normalize(const TVector &v)
{	
   TVector r=v;
   r.Normalize();
   return r;
}	

inline TVector		Neg(const TVector &v)
{	
   TVector r=v;
   r.Neg();
   return r;
}	

inline TVector    MulPart(const TVector &a,const TVector &b)
{
   TVector r=a;
   r.MulPart(b);
   return r;
}

inline TVector    Int(const TVector &v)
{
   TVector r=v;
   r.Int();
   return r;
}

inline TVector		Max( const TVector &a, const TVector &b )
{
	return TVector(   a.x > b.x ? a.x:b.x,
	                  a.y > b.y ? a.y:b.y,
	                  a.z > b.z ? a.z:b.z );
}

inline TVector		Min( const TVector &a, const TVector &b )
{
	return TVector(   a.x < b.x ? a.x:b.x,
	                  a.y < b.y ? a.y:b.y,
	                  a.z < b.z ? a.z:b.z );
}

//////////////////////////////////////////////////////////////////////
// functions
//////////////////////////////////////////////////////////////////////

inline TVector toVector(int i)
{
   TVector v;
   v.fromColor( i );
   //
   return v;
}



#endif


//////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////


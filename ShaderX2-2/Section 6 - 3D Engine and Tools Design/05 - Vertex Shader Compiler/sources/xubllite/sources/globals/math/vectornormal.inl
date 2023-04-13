/////////////////////////////////////////////////////////////////////////////////
//
// VectorNormal software
// Created 3.6.2000
//
/////////////////////////////////////////////////////////////////////////////////

// =
inline TVector &TVector::operator = ( float val )
{
   x=y=z=w=val;
   //
   return *this;
}

inline TVector &TVector::operator = ( const TVector& v)
{
   x=v.x;
   y=v.y;
   z=v.z;  
   w=v.w;
   //
   return *this;
}

// the following don't change/use w
// +=
inline TVector &TVector::operator += ( const  TVector& v )
{
   x+=v.x;
   y+=v.y;
   z+=v.z;
   //
   return *this;
}


inline TVector &TVector::operator -= ( const  TVector& v )
{
   x-=v.x;
   y-=v.y;
   z-=v.z;
   //
   return *this;
}

inline TVector &TVector::operator *= ( float d )
{
   x*=d;
   y*=d;
   z*=d;
   //
   return *this;
}

inline TVector &TVector::operator /= ( float d )
{
   float dz1=1.0f/d;
   //
   x*=dz1;
   y*=dz1;
   z*=dz1;
   //
   return *this;
}

//
inline TVector TVector::operator + ( const TVector &v ) const
{
   return TVector( x + v.x , y + v.y , z + v.z );
}

inline TVector TVector::operator - ( const TVector &v ) const
{
   return TVector( x - v.x , y - v.y , z - v.z );
}

// *
inline TVector TVector::operator * ( float d) const
{
   TVector vec(*this);
   vec*=d;
   return vec;
}

inline float TVector::operator * ( const TVector &v ) const
{
   return x*v.x + y*v.y + z*v.z;;
}

//
inline TVector TVector::operator / ( float d) const
{
   TVector vec(*this);
   vec/=d;
   return vec;
}

inline TVector TVector::operator % ( const TVector &v ) const
{
   return TVector(   y * v.z - z * v.y,
						   z * v.x - x * v.z,
						   x * v.y - y * v.x );
}

//
inline int TVector::operator == ( const TVector &v ) const
{
   return x==v.x && y==v.y && z==v.z;
}

inline int TVector::operator == ( const float d ) const
{
   return x==d && y==d && z==d;
}

inline int TVector::operator != ( const TVector &v ) const
{
   return x!=v.x || y!=v.y || z!=v.z;
}

inline int TVector::operator != ( const float d ) const
{
   return x!=d || y!=d || z!=d;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline int TVector::operator > ( const TVector &v ) const
{
   return x > v.x &&  
          y > v.y && 
          z > v.z;
}

inline int TVector::operator > ( float d ) const
{
   return x > d &&  
          y > d && 
          z > d;
}
//
inline int TVector::operator < ( const TVector &v ) const
{
   return x < v.x &&  
          y < v.y && 
          z < v.z;
}

inline int TVector::operator < ( float d ) const
{
   return x < d &&  
          y < d && 
          z < d;
}
//
inline int TVector::operator >= ( const TVector &v ) const
{
   return x >= v.x &&  
          y >= v.y && 
          z >= v.z;
}

inline int TVector::operator >= ( float d ) const
{
   return x >= d &&  
          y >= d && 
          z >= d;
}
//
inline int TVector::operator <= ( const TVector &v ) const
{
   return x <= v.x &&  
          y <= v.y && 
          z <= v.z;
}

inline int TVector::operator <= ( float d ) const
{
   return x <= d &&  
          y <= d && 
          z <= d;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//
inline int TVector::IsSameAs( float f,float error) const
{
   return abs( x-f ) < error && abs( y-f ) < error && abs( z-f ) < error;
}

inline int TVector::IsSameAs( const TVector &v,float error) const
{
   return abs( x-v.x ) < error && abs( y-v.y ) < error && abs( z-v.z ) < error;
}

// partial multiplication into *this
inline TVector &TVector::MulPart(const TVector &a)
{
   x*=a.x;
   y*=a.y;
   z*=a.z;
   return *this;
}

inline TVector &TVector::Int()
{
   x=floorf(x);
   y=floorf(y);
   z=floorf(z);
   //
   return *this;
}

// Square length
inline float	TVector::SqrLength() const
{
   return x * x + y * y + z * z;
}

inline float	TVector::Length() const
{
   return sqrtf( x * x + y * y + z * z );
}

inline TVector &TVector::Normalize(float f)
{
   //
   float l=Length();
   if( abs(l)<1E-5 ) 
   {
      //#ifdef _DEBUG
      //   MAbort( "TVector::Normalize warning: vector length to small." );
      //#endif
      //
      return *this;
   }
   //
   *this*=f/l;
   return *this;
}

inline TVector &TVector::Neg()
{
   x=-x;
   y=-y;
   z=-z;
   //
   return *this;
}

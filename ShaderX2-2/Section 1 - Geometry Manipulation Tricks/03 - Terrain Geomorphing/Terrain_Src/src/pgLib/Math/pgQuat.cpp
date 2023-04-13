///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgQuat
// creator:     Markus Hötzinger
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgQuat.cpp,v 1.4 2003/01/05 15:10:35 markus Exp $
///////////////////////////////////////////////////////////////////


#include "pgQuat.h"
#include <pgLib/Math/pgIMathTool.h>


/*************** CONSTRUCTORS *****************************************************/

pgQuat::pgQuat( float xval, float yval, float zval, float wval ) 
{
	x=xval; 
	y=yval; 
	z=zval; 
	w=wval;
}


pgQuat::pgQuat( const pgQuat& other )
{
    x = other.x;
    y = other.y;
	z = other.z;
	w = other.w;
}


pgQuat::pgQuat (const pgVec3& dir)
{
	set(dir);
}




/*************** OPERATORS *****************************************************/

pgQuat& pgQuat::operator= (const pgQuat& quat)
{
	x = quat.x;
	y = quat.y;
	z = quat.z;
	w = quat.w;

	return *this;
}



pgQuat pgQuat::operator* (const pgQuat& quat)
{
	
	return pgQuat (	w*quat.x + x*quat.w + y*quat.z - z*quat.y, 
					w*quat.y + y*quat.w + z*quat.x - x*quat.z,
					w*quat.z + z*quat.w + x*quat.y - y*quat.x, 
					w*quat.w - x*quat.x - y*quat.y - z*quat.z	);
}



pgQuat& pgQuat::operator*= (const pgQuat& quat)
{

	float tempx =	x*quat.x - y*quat.y - z*quat.z - w*quat.w;
	float tempy =	x*quat.y + y*quat.x + z*quat.w - w*quat.z;
	float tempz =	x*quat.z - y*quat.w + z*quat.x + w*quat.y;
	float tempw =	x*quat.w + y*quat.z - z*quat.y + w*quat.x;

	x=tempx;
	y=tempy;
	z=tempz;
	w=tempw;

	return *this;
}


pgQuat& pgQuat::operator>= (const pgQuat& quat)
{
	
	float tempx =	quat.x*x - quat.y*y - quat.z*z - quat.w*w;
	float tempy =	quat.y*x + quat.x*y + quat.w*z - quat.z*w;
	float tempz =	quat.z*x - quat.w*y + quat.x*z + quat.y*w;
	float tempw =	quat.w*x + quat.z*y - quat.y*z + quat.x*w;
	
	x=tempx;
	y=tempy;
	z=tempz;
	w=tempw;

	return *this;
}




bool pgQuat::operator== (const pgQuat& quat)
{
	return ( (x == quat.x) && (y == quat.y) && (z == quat.z) && (w == quat.w)  );
}



bool pgQuat::operator!= (const pgQuat& quat)
{
	return ( (x!=quat.x) || (y!=quat.y) || (z!=quat.z) || (w!=quat.w) );
}




/***************** MEMBER FUNCTIONS ************************************************/

void pgQuat::setMatrix(pgMatrix& mat)
{

	float xx = x*x;
    float xy = x*y;
    float xz = x*z;
    float xw = x*w;

    float yy = y*y;
    float yz = y*z;
    float yw = y*w;

    float zz = z*z;
    float zw = z*w;

    mat.m00 = 1-2*( yy+zz );
    mat.m10 =   2*( xy+zw );
    mat.m20 =   2*( xz-yw );

    mat.m01 =   2*( xy-zw );
    mat.m11 = 1-2*( xx+zz );
    mat.m21 =   2*( yz+xw );

    mat.m02 =   2*( xz+yw );
    mat.m12 =   2*( yz-xw );
    mat.m22 = 1-2*( xx+yy );




    mat.m30 = mat.m31 = mat.m32 = mat.m03 = mat.m13 = mat.m23 = 0;
    mat.m33 = 1;
}





void pgQuat::set(const pgVec3& dir) 
{    
		float c1 = pgIMathTool::cos(dir.x);    
		float s1 = pgIMathTool::sin(dir.x);    
		float c2 = pgIMathTool::cos(dir.y);    
		float s2 = pgIMathTool::sin(dir.y);    
		float c3 = pgIMathTool::cos(dir.z);    
		float s3 = pgIMathTool::sin(dir.z);
		
		x = c1*c2*c3 + s1*s2*s3;
		y =	c1*c2*s3 - s1*s2*c3;
		z = c1*s2*c3 + s1*c2*s3;
		w = s1*c2*c3 - c1*s2*s3;
}







void pgQuat::set(const pgMatrix& mat)
{
	float  tr, s, q[4];
	int    i, j, k;
	int nxt[3] = {1, 2, 0};

	tr = mat[0][0] + mat[1][1] + mat[2][2];

	// check the diagonal
	if (tr > 0.0) 
	{
		s = pgIMathTool::sqrt (tr + 1.0f);
		w = s / 2.0f;
		s = 0.5f / s;
		x = (mat.m12 - mat.m21) * s;
		y = (mat.m20 - mat.m02) * s;
		z = (mat.m01 - mat.m10) * s;
	} 
	else 
	{		
		// diagonal is negative
		i = 0;
		if (mat.m11 > mat.m00) i = 1;
		if (mat.m22 > mat[i][i]) i = 2;
		j = nxt[i];
		k = nxt[j];

		s = pgIMathTool::sqrt ((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0f);

		q[i] = s * 0.5f;

		if (s != 0.0f) s = 0.5f / s;

		q[3] = (mat[j][k] - mat[k][j]) * s;
		q[j] = (mat[i][j] + mat[j][i]) * s;
		q[k] = (mat[i][k] + mat[k][i]) * s;

		x = q[0];
		y = q[1];
		z = q[2];
		w = q[3];
	}
}



void pgQuat::makeRotX (float nX)
{
	x = pgIMathTool::sin(nX*0.5f);
	y = 0.0f;
	z = 0.0f;
	w = pgIMathTool::cos(nX*0.5f);
}


void pgQuat::makeRotY (float nY)
{
	x = 0.0f;
	y = pgIMathTool::sin(nY*0.5f);
	z = 0.0f;
	w = pgIMathTool::cos(nY*0.5f);
}


void pgQuat::makeRotZ (float nZ)
{
	x = 0.0f;
	y = 0.0f;
	z = pgIMathTool::sin(nZ*0.5f);
	w = pgIMathTool::cos(nZ*0.5f);
}



float pgQuat::dot(const pgQuat& left,const pgQuat& right)
{
    return left.x*right.x+left.y*right.y+left.z*right.z+left.w*right.w;
}

/*
void
pgQuat::Slerp(const pgQuat& from,const pgQuat& to,float slice)
{
	float	to1[4];
	float	omega, cosom, sinom, scale0, scale1;

	// calc cosine
	cosom = from.x * to.x + from.y * to.y + from.z * to.z+ from.w * to.w;

	// adjust signs (if necessary)
	if ( cosom <0.0 )
	{ 
		cosom = -cosom; to1[0] = - to.x;
		to1[1] = - to.y;
		to1[2] = - to.z;
		to1[3] = - to.w;
	} 
	else  
	{
		to1[0] = to.x;
		to1[1] = to.y;
		to1[2] = to.z;
		to1[3] = to.w;
	}
	// calculate coefficients
	if ( (1.0 - cosom) > FLT_EPSILON ) 
	{
		// standard case (slerp)
		omega = pgAcos(cosom);
		sinom = pgSin(omega);
		scale0 = pgSin((1.0f - slice) * omega) / sinom;
		scale1 = pgSin(slice * omega) / sinom;
	} 
	else 
	{        
		// "from" and "to" quaternions are very close 
		//  ... so we can do a linear interpolation
		scale0 = 1.0f - slice;
		scale1 = slice;
	}
	// calculate final values
	x = scale0 * from.x + scale1 * to1[0];
	y = scale0 * from.y + scale1 * to1[1];
	z = scale0 * from.z + scale1 * to1[2];
	w = scale0 * from.w + scale1 * to1[3];
}


pgQuat
pgQuat::conjugate()
{
	return pgQuat(x,-y,-z,-w);
}

float
pgQuat::norm2()
{
	return (x*x+y*y+z*z+w*w);
}

pgQuat
pgQuat::inverse()
{
	float  n = norm2();
	pgQuat q = conjugate();
	if (n>FLT_EPSILON)
        return pgQuat(q.x/n,q.y/n,q.z/n,q.w/n);
	else
        return pgQuat(q.x,q.y,q.z,q.w);
}

float
pgQuat::normalize()
{
	float  n = pgSqrt(norm2());
	if (n>FLT_EPSILON)
	{
		x /= n;
		y /= n;
		z /= n;
		w /= n;
	}
	return n;
}*/
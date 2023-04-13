#include "../stdafx.h"

#include "MathUtil.h"


void math::buildMatrix( D3DXMATRIX& out, const D3DXVECTOR3& x, const D3DXVECTOR3& y, const D3DXVECTOR3& z, const D3DXVECTOR3& origin )
{
	out._11 = x.x;	out._12 = x.y;	out._13 = x.z;	out._14 = 0.0f;
	out._21 = y.x;	out._22 = y.y;	out._23 = y.z;	out._24 = 0.0f;
	out._31 = z.x;	out._32 = z.y;	out._33 = z.z;	out._34 = 0.0f;
	out._41 = origin.x; out._42 = origin.y; out._43 = origin.z; out._44 = 1.0f;
}


void math::buildObjectMatrixXZ( D3DXMATRIX& out, const D3DXVECTOR3& front, const D3DXVECTOR3& right, const D3DXVECTOR3& origin )
{
	// normalize front
	D3DXVECTOR3 f;
	D3DXVec3Normalize( &f, &front );
	
	// make front ant right at right angle
	D3DXVECTOR3 r = right;
	r -= f * D3DXVec3Dot( &r, &f );
	// right is zero case
	if( 1e-6f > D3DXVec3LengthSq( &r ) ) {
		r = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) - f * f.x;
		// if still zero
		if( 1e-6f > D3DXVec3LengthSq( &r ) ) {
			r = D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) - f * f.z;
			if( 1e-6f > D3DXVec3LengthSq( &r ) )
				return; // absurd abstract case ;)
		}
	}
	// normalize right
	D3DXVec3Normalize( &r, &r );
	
	// up == cross( front, right ).
	D3DXVECTOR3 u;
	D3DXVec3Cross( &u, &f, &r );

	math::buildMatrix( out, r, u, f, origin );
}


void math::buildObjectMatrixXY( D3DXMATRIX& out, const D3DXVECTOR3& front, const D3DXVECTOR3& up, const D3DXVECTOR3& origin )
{
	// normalize front
	D3DXVECTOR3 f;
	D3DXVec3Normalize( &f, &front );
	
	// make front ant up at right angle
	D3DXVECTOR3 u = up;
	u -= f * D3DXVec3Dot( &u, &f );
	// up is zero case
	if( 1e-6f > D3DXVec3LengthSq( &u ) ) {
		u = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) - f * f.y;
		// if still zero
		if( 1e-6f > D3DXVec3LengthSq( &u ) ) {
			u = D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) - f * f.z;
			if( 1e-6f > D3DXVec3LengthSq( &u ) )
				return; // absurd abstract case ;)
		}
	}
	// normalize up
	D3DXVec3Normalize( &u, &u );
	
	// right == cross( up, front ).
	D3DXVECTOR3 r;
	D3DXVec3Cross( &r, &u, &f );
	
	math::buildMatrix( out, r, u, f, origin );
}

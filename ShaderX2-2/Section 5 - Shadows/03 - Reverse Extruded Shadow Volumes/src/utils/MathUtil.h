
#ifndef __MATHUTIL_H__
#define __MATHUTIL_H__


namespace math {

/** Builds matrix, given 3 unit axis vectors and origin point. */
void buildMatrix( D3DXMATRIX& out, const D3DXVECTOR3& x, const D3DXVECTOR3& y, const D3DXVECTOR3& z, const D3DXVECTOR3& origin );

/**
 *  Builds object (NOT camera) transform matrix.
 *  Preserves given "front", makes "right" orthogonal to it.
 */
void buildObjectMatrixXZ( D3DXMATRIX& out, const D3DXVECTOR3& front, const D3DXVECTOR3& right, const D3DXVECTOR3& origin );

/**
 *  Builds object (NOT camera) transform matrix.
 *  Preserves given "front", makes "right" orthogonal to it.
 */
void buildObjectMatrixXY( D3DXMATRIX& out, const D3DXVECTOR3& front, const D3DXVECTOR3& up, const D3DXVECTOR3& origin );


}; // namespace math


#endif

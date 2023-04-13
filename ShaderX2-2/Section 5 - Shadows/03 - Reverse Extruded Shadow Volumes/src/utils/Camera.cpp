#include "../stdafx.h"

#include "Camera.h"
#include "MathUtil.h"


CCamera::CCamera()
:	mPosition( 0, 0, 0 ),
	mTarget( 0, 1, 0 ),
	mRoll( 0.0f )
{
	setProjectionParams( 3.1415927f * 0.5f, 1.3f, 10.0f, 1000.0f );
	updateViewParams();
}


void CCamera::updateViewParams()
{
	math::buildObjectMatrixXY( mCameraMatrix, mTarget-mPosition, D3DXVECTOR3(0,1,0), mPosition );
	D3DXMATRIX rollm;
	D3DXMatrixRotationZ( &rollm, mRoll );
	mCameraMatrix = rollm * mCameraMatrix;
	D3DXMatrixInverse( &mViewMatrix, NULL, &mCameraMatrix );
}


void CCamera::setProjectionParams( float fov, float aspect, float znear, float zfar )
{
	D3DXMatrixPerspectiveFovLH( &mProjectionMatrix, fov, aspect, znear, zfar );
}

void CCamera::setDeviceTransforms( IDirect3DDevice8& device, bool view, bool proj )
{
	if( view )
		device.SetTransform( D3DTS_VIEW, &mViewMatrix );
	if( proj )
		device.SetTransform( D3DTS_PROJECTION, &mProjectionMatrix );
}

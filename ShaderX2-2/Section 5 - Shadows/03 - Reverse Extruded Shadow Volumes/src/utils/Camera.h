
#ifndef __CAMERA_H
#define __CAMERA_H


class CCamera {
public:
	CCamera();

	void	setProjectionParams( float fov, float aspect, float znear, float zfar );
	void	updateViewParams(); // from position, target, roll

	void	setDeviceTransforms( IDirect3DDevice8& device, bool view = true, bool proj = false );

	const D3DXVECTOR3& getPosition() const { return mPosition; }
	D3DXVECTOR3& getPosition() { return mPosition; }
	const D3DXVECTOR3& getTarget() const { return mTarget; }
	D3DXVECTOR3& getTarget() { return mTarget; }
	const float& getRoll() const { return mRoll; }
	float& getRoll() { return mRoll; }

	const D3DXMATRIX& getViewMatrix() const { return mViewMatrix; }
	const D3DXMATRIX& getCameraMatrix() const { return mCameraMatrix; }
	const D3DXMATRIX& getProjMatrix() const { return mProjectionMatrix; }
	
private:
	D3DXVECTOR3	mPosition;
	D3DXVECTOR3	mTarget;
	float		mRoll;

	D3DXMATRIX	mProjectionMatrix;
	D3DXMATRIX	mCameraMatrix;
	D3DXMATRIX	mViewMatrix;
};


#endif

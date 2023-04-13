
#ifndef __SHADOWS_H
#define __SHADOWS_H

#include "DXObject.h"


class CCamera;


class CShadows : public CDXObject {
public:
	CShadows( const CCamera& camera );
	
	virtual HRESULT onCreateDevice( IDirect3DDevice8* device );
	virtual HRESULT onLostDevice();
	virtual HRESULT onResetDevice();
	virtual HRESULT onDestroyDevice();

	virtual void	render( float lastFrameTime, float time );

private:
	void renderMeshVS( ID3DXMesh& mesh );
	
public:
	const CCamera*	mCamera;

	//IDirect3DTexture8*		mTexObject;
	//IDirect3DTexture8*		mTexGround;
	
	ID3DXEffect*			mEfxAmbient;
	ID3DXEffect*			mEfxCasterReverse;
	ID3DXEffect*			mEfxCasterNormal;
	ID3DXEffect*			mEfxReceiver;
	//ID3DXEffect*			mEfxLight;
	
	ID3DXMesh*				mMeshObject;
	ID3DXMesh*				mMeshObjectSimple;
	ID3DXMesh*				mMeshGround;
	//ID3DXMesh*				mMeshLight;

	D3DXVECTOR4			mLightPos;
	D3DXMATRIX			mObjectMatrix;
	D3DXMATRIX			mGroundMatrix;

	bool				mAnimateLight;
	bool				mAnimateObject;
	bool				mLowPolyCaster;
	bool				mReverseExtrusion;
};

#endif

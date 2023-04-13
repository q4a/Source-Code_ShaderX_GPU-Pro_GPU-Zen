
#ifndef __DEMO_APP_H
#define __DEMO_APP_H

#include "utils/Camera.h"
#include "DXObject.h"


class CShadows;


class CDemoApp : public CD3DApplication {
public:
    CDemoApp();
	~CDemoApp();
	
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
	virtual HRESULT ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format );
	
    virtual LRESULT MsgProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	
private:
	void	resetCamera();
	void	updateCamera();
	
private:
	CDXObjectContainer	mObjects;
    CD3DFont*		mFontSmall;
	CShadows*		mShadows;
	
	bool			mKeys[256];

	bool			mShowingHelp;

	CCamera			mCamera;
	D3DXVECTOR3		mCameraSphericalCoords;
	D3DXVECTOR3		mCameraDelta;
};


#endif

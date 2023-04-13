#include "stdafx.h"

#include "DemoApp.h"
#include "Shadows.h"


#define CAMERA_MIN_DISTANCE 20.0f
#define CAMERA_MAX_DISTANCE 50.0f
#define CAMERA_MIN_LATITUDE 0.1f
#define CAMERA_MAX_LATITUDE 3.1415927f * 0.48f

#define SCENE_HEIGHT 25


CDemoApp::CDemoApp()
{
    m_strWindowTitle    = _T("reversely extruded shadows");
    m_bUseDepthBuffer   = TRUE;
	m_bWindowed			= TRUE;
	m_dwMinDepthBits	= 16;
	m_dwMinStencilBits	= 4;
	m_dwCreationWidth	= 400;
	m_dwCreationHeight	= 300;

	mShowingHelp = false;
	
    mFontSmall		= new CD3DFont( _T("Arial"),  9, D3DFONT_BOLD );

	mShadows = new CShadows( mCamera );
	mObjects.addObject( *mShadows );
}

CDemoApp::~CDemoApp()
{
}

HRESULT CDemoApp::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	if( Format != D3DFMT_X8R8G8B8 && Format != D3DFMT_A8R8G8B8 )
		return E_FAIL;

    // needs at least vertex shader 1_1
    if( pCaps->VertexShaderVersion >= D3DVS_VERSION(1,1) ||
        dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
		return S_OK;
	
    return E_FAIL;
}


HRESULT CDemoApp::OneTimeSceneInit()
{
	ZeroMemory( mKeys, sizeof(mKeys) );

	// resources
	
    return S_OK;
}


HRESULT CDemoApp::InitDeviceObjects()
{
	resetCamera();
	
	mObjects.onCreateDevice( m_pd3dDevice );
	
    mFontSmall->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}


HRESULT CDemoApp::RestoreDeviceObjects()
{
	// camera
	mCamera.setProjectionParams(
		3.1415927f * 0.4f,
		(float)m_d3dsdBackBuffer.Width / m_d3dsdBackBuffer.Height,
		1.0f,
		200.0f
	);
	mCamera.updateViewParams();
	mCamera.setDeviceTransforms( *m_pd3dDevice, false, true );
	
	mObjects.onResetDevice();
	
    mFontSmall->RestoreDeviceObjects();

	return S_OK;
}


HRESULT CDemoApp::InvalidateDeviceObjects()
{
	mObjects.onLostDevice();

    mFontSmall->InvalidateDeviceObjects();
	
    return S_OK;
}


HRESULT CDemoApp::DeleteDeviceObjects()
{
	mObjects.onResetDevice();

    mFontSmall->DeleteDeviceObjects();
	
    return S_OK;
}


// --------------------------------------------------------------------------

void CDemoApp::resetCamera()
{
	mCamera.setProjectionParams(
		3.1415927f * 0.4f,
		(float)m_d3dsdBackBuffer.Width / m_d3dsdBackBuffer.Height,
		1.0f,
		200.0f
		);
	mCamera.setDeviceTransforms( *m_pd3dDevice, false, true );

	mCameraSphericalCoords = D3DXVECTOR3( (CAMERA_MIN_DISTANCE+CAMERA_MAX_DISTANCE)/2, 0.0f, 1.0f );
	mCameraDelta = D3DXVECTOR3(0,0,0);
	mCamera.getTarget() = D3DXVECTOR3( 0.0f, SCENE_HEIGHT*0.5f, 0.0f );
	updateCamera();
}

void CDemoApp::updateCamera()
{
	mCameraDelta *= 0.9f;
	if( fabsf( mCameraDelta.x ) < 0.001f ) mCameraDelta.x = 0.0f;
	if( fabsf( mCameraDelta.y ) < 0.001f ) mCameraDelta.y = 0.0f;
	if( fabsf( mCameraDelta.z ) < 0.001f ) mCameraDelta.z = 0.0f;

	mCameraSphericalCoords += mCameraDelta * m_fElapsedTime;

	if( mCameraSphericalCoords.x < CAMERA_MIN_DISTANCE )
		mCameraSphericalCoords.x = CAMERA_MIN_DISTANCE;
	else if( mCameraSphericalCoords.x > CAMERA_MAX_DISTANCE )
		mCameraSphericalCoords.x = CAMERA_MAX_DISTANCE;

	if( mCameraSphericalCoords.z < CAMERA_MIN_LATITUDE )
		mCameraSphericalCoords.z = CAMERA_MIN_LATITUDE;
	else if( mCameraSphericalCoords.z > CAMERA_MAX_LATITUDE )
		mCameraSphericalCoords.z = CAMERA_MAX_LATITUDE;
	
	D3DXVECTOR3 v( 0.0f, 0.0f, mCameraSphericalCoords.x );
	D3DXMATRIX m;
	D3DXMatrixRotationX( &m, -mCameraSphericalCoords.z );
	D3DXVec3TransformCoord( &v, &v, &m );
	D3DXMatrixRotationY( &m, mCameraSphericalCoords.y );
	D3DXVec3TransformCoord( &v, &v, &m );

	mCamera.getPosition() = v + mCamera.getTarget();
	mCamera.updateViewParams();
}


HRESULT CDemoApp::FrameMove()
{
	if( mKeys['R'] ) {
		resetCamera();
	}
	// update rotation from keys
	if( mKeys[VK_UP] )		mCameraDelta.z += 0.2f;
	if( mKeys[VK_DOWN] )	mCameraDelta.z -= 0.2f;
	if( mKeys[VK_LEFT] )	mCameraDelta.y += 0.2f;
	if( mKeys[VK_RIGHT] )	mCameraDelta.y -= 0.2f;
	if( mKeys['A'] )	mCameraDelta.x -= 30.0f;
	if( mKeys['Z'] )	mCameraDelta.x += 30.0f;
	
    return S_OK;
}


HRESULT CDemoApp::Render()
{   
    HRESULT hr;
	
    if( FAILED( hr = m_pd3dDevice->BeginScene() ) )
        return hr;
	
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0 );

	// camera
	updateCamera();
	mCamera.setDeviceTransforms( *m_pd3dDevice );

	mObjects.render( m_fElapsedTime, m_fTime );

	// Show info and help
	mFontSmall->DrawText( 4, 2, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
	//mFontSmall->DrawText( 4, 16, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

	char buf[200];

	sprintf( buf, "Anim light: %s, object: %s.",
		mShadows->mAnimateLight?"yes":"no",
		mShadows->mAnimateObject?"yes":"no"
	);
	mFontSmall->DrawText( 4, 16, D3DCOLOR_ARGB(192,255,255,255), buf );

	sprintf( buf, "Caster: %s poly (%i verts, %i faces).",
		mShadows->mLowPolyCaster?"low":"high",
		(mShadows->mLowPolyCaster?mShadows->mMeshObjectSimple:mShadows->mMeshObject)->GetNumVertices(),
		(mShadows->mLowPolyCaster?mShadows->mMeshObjectSimple:mShadows->mMeshObject)->GetNumFaces()
	);
	mFontSmall->DrawText( 4, 30, D3DCOLOR_ARGB(192,255,255,255), buf );

	sprintf( buf, "Extrusion: %s.",
		mShadows->mReverseExtrusion?"reverse (frontfaces)":"normal (backfaces)"
	);
	mFontSmall->DrawText( 4, 44, D3DCOLOR_ARGB(192,255,255,255), buf );

	if( mShowingHelp ) {
		mFontSmall->DrawText( 4, 60, D3DCOLOR_ARGB(192,255,64,64),
			"Key\n"
			"F1\n"
			"F2\n"
			"F3\n"
			"F4\n"
			"F5\n"
			"Up/Down\n"
			"Left/Right\n"
			"A/Z"
			);
		mFontSmall->DrawText( 84, 60, D3DCOLOR_ARGB(192,255,255,255),
			"action\n"
			"- toggle help\n"
			"- animate light\n"
			"- animate object\n"
			"- low/high poly caster\n"
			"- normal/reverse shadow extrusion\n"
			"- orbit camera vertically\n"
			"- orbit camera horizontally\n"
			"- move camera forward/backward\n"
			);
	} else {
		mFontSmall->DrawText( 4, 60, D3DCOLOR_ARGB(192,255,255,255),
			"Press F1 for help"
		);
	}
	
	if( FAILED( hr = m_pd3dDevice->EndScene() ) )
		return hr;
		
	return S_OK;
}


LRESULT CDemoApp::MsgProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg ) {
	case WM_KEYDOWN:
		mKeys[ wParam ] = true;
		break;
	case WM_KEYUP:
		mKeys[ wParam ] = false;
		switch( wParam ) {
		case VK_F1:
			mShowingHelp = !mShowingHelp;
			break;
		case VK_F2:
			mShadows->mAnimateLight = !mShadows->mAnimateLight;
			break;
		case VK_F3:
			mShadows->mAnimateObject = !mShadows->mAnimateObject;
			break;
		case VK_F4:
			mShadows->mLowPolyCaster = !mShadows->mLowPolyCaster;
			break;
		case VK_F5:
			mShadows->mReverseExtrusion = !mShadows->mReverseExtrusion;
			break;
		}

		break;
	};
	
    return CD3DApplication::MsgProc( wnd, msg, wParam, lParam );
}

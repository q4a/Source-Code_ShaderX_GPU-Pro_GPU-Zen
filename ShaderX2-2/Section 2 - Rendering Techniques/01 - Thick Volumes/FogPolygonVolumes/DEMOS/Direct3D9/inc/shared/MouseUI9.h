/*********************************************************************NVMH2****
File:  MouseUI.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:

This is a modified version of Sim's original MouseUI.h.
This one requires you call LButtonDown() and LButtonUp(), but you no longer
 have to pass in the flags parameter.

******************************************************************************/

#ifndef _MOUSEUI_H_
#define _MOUSEUI_H_

#include <d3dx9.h>

/////////////
// MouseUI.h
//
// By Sim Dietrich & Cem Cebenoyan
//
// What it does :
//
// This UI object supports simple rotation through dragging with the left mouse button held down.
// It also has virtual methods for left button down & up, which could be overriden by a derived
// GUI object that handled buttons or something.
// Cyril Zeller: Added a mode (m_bYawPitchOnly) where rotation is constrained to yaw and pitch (no roll) (useful for camera motion)
// 
// Usage:
//
// Create an instance of the MouseUI object, and pass in the Rect of the window you want it to refer to.
// I originally had it use the HWND of the window, but it occurred to me that we really want to have
// the ability to do sub-areas, for instance if you had a 3D gui, and you only wanted the smaller viewport
// to respond to mouse messages.
//
// If the window resizes, you should call SetRECT() with the new rect size.
//
// One other nice thing about this class is that you don't have to check if the mouse x & y is in the rect.
// The class will always check for you on all button/mouse move messages.
//
// Just call MouseMove when appropriate and the MouseUI will update its internal RotationMatrix.  It doesn't
// orthonormalize the matrix, so it may go wacky eventually.
//
// Then call GetRotationMatrix().  You can use the matrix to rotate around the object, the view or the world as appropriate.
// Then call GetTranslationMatrix().  You can use the matrix to translate the object, the view or the world as appropriate.
//
// To get the rotation & translation matrices back to identity, call Reset()
//
// You can also get/set the sensitivity of the rotations through the Get/SetSensitivityFactor() calls.
// The SensitivityFactor defaults to 1.0f
//
static const float MOUSEUI_TRANSLATION_SCALE = 0.05f;
static const float MOUSEUI_ROTATION_SCALE = 1.0f;

class MouseUI
{
protected :
	
    bool m_bLButtonDown;
	bool m_bYawPitchOnly;
    bool m_bUseCameraControl;
	
	unsigned int mStartX;
	unsigned int mStartY;
	
	unsigned int mWidth;
	unsigned int mHeight;
	
	D3DXMATRIX mRotationMatrix;
	D3DXMATRIX mTranslationMatrix;
	float mYaw, mPitch;
	
	float mRotationalSensitivityFactor;
	float mTranslationalSensitivityFactor;
	

	D3DXMATRIX		m_mViewOrientationMatrix;

	RECT mRect;
	
	void SetLocation( const unsigned int& x, const unsigned int& y )
	{
		mStartX = max( (unsigned int)mRect.left, x );
		mStartX = min( (unsigned int)mRect.right, mStartX );
		
		mStartY = max( (unsigned int)mRect.top, y );
		mStartY = min( (unsigned int)mRect.bottom, mStartY );
	}
	
	void SetSize()
	{
		mWidth =  ( mRect.right - mRect.left );
		mHeight = ( mRect.bottom - mRect.top );
	}
	
public :
	
	enum ViewMode
	{
		VM_STANDARD_ZAXIS,
		VM_CAMERA_AWARE,
		VM_FORCEDWORD = 0xffffffff	
	};

	ViewMode	m_eViewMode;


	MouseUI::MouseUI( const RECT& theRect, bool bCameraControl = false, bool bYawPitchOnly = false) 
            : mRect( theRect )
            , m_bUseCameraControl( bCameraControl )
			, m_bYawPitchOnly(bYawPitchOnly)
			, mYaw(0.0f), mPitch(0.0f)
	{
		m_eViewMode = VM_STANDARD_ZAXIS;
		Reset();
	}
	

	void SetControlOrientationMatrix( D3DXMATRIX mMat )
	{
		// Send in your view matrix so the controls respond with
		//  the correct translation & rotation relative to it
		// Used with VM_CAMERA_AWARE view mode.

		// zero translation and invert matrix so the vieworientation
		//  takes things from view space to world space
		mMat._41 = 0.0f;
		mMat._42 = 0.0f;
		mMat._43 = 0.0f;

		mMat._14 = 0.0f;
		mMat._24 = 0.0f;
		mMat._34 = 0.0f;

		mMat._44 = 1.0f;

		D3DXMatrixInverse( &m_mViewOrientationMatrix, NULL, &mMat );
	}

	void SetViewMode( ViewMode mode ) { m_eViewMode = mode; }


	RECT GetRECT() const { return mRect; }
	
	void SetRECT( const RECT& rhs ) 
	{
		mRect = rhs; 
		SetSize();
	}
	
	void GetRotationMatrix( D3DXMATRIX* pMatrix ) const { (*pMatrix) = mRotationMatrix; }
	
	D3DXMATRIX GetRotationMatrix() const { return mRotationMatrix; }
	D3DXMATRIX * GetRotationMatrixPtr() { return( & mRotationMatrix ); }
	
	void GetTranslationMatrix( D3DXMATRIX* pMatrix ) const { (*pMatrix) = mTranslationMatrix; }
	
	D3DXMATRIX GetTranslationMatrix() const { return mTranslationMatrix; }
	D3DXMATRIX * GetTranslationMatrixPtr() { return( & mTranslationMatrix ); }
	
	void SetTranslationMatrix( const D3DXMATRIX & mat )
	{
		mTranslationMatrix = mat;
	}

	void SetRotationMatrix( const D3DXMATRIX & mat )
	{
		mRotationMatrix = mat;
		mYaw = atan2f(mat._13, mat._11);
		mPitch = asinf(mat._32);
	}

	void SetMatrix( const D3DXMATRIX & mat )
	{
		D3DXMATRIX rot = mat;
		rot._41 = rot._42 = rot._43 = 0.0f;
		SetRotationMatrix(rot);
		D3DXMATRIX trans;
		D3DXMatrixTranslation(&trans, mat._41, mat._42, mat._43);
		SetTranslationMatrix(trans);
	}

	bool IsMouseDown() const { return ( m_bLButtonDown ); }
	
	void Reset()
	{
		SetSize();
		mRotationalSensitivityFactor = 1.0f * MOUSEUI_ROTATION_SCALE;
		mTranslationalSensitivityFactor = 1.0f * MOUSEUI_TRANSLATION_SCALE;
		m_bLButtonDown = false;
		SetLocation( 0, 0 );
		D3DXMatrixIdentity( &mRotationMatrix );
		D3DXMatrixIdentity( &mTranslationMatrix );
	}
	
	bool IsInWindow( const unsigned int& x, const unsigned int& y )
	{
		SetSize();


		return ( ( x >= (unsigned int)mRect.left ) && ( x < (unsigned int)mRect.right ) &&
			( y >= (unsigned int)mRect.top  ) && ( y < (unsigned int)mRect.bottom ) );
	}
	
	float GetTranslationalSensitivityFactor() const { return mTranslationalSensitivityFactor / MOUSEUI_TRANSLATION_SCALE; }
	
	void  SetTranslationalSensitivityFactor( const float& rhs ) { mTranslationalSensitivityFactor = rhs * MOUSEUI_TRANSLATION_SCALE; }

	float GetRotationalSensitivityFactor() const { return mRotationalSensitivityFactor / MOUSEUI_ROTATION_SCALE; }
	
	void  SetRotationalSensitivityFactor( const float& rhs ) { mRotationalSensitivityFactor = rhs * MOUSEUI_ROTATION_SCALE; }
	
	// accepts window coordinates
	virtual void OnLButtonDown( const unsigned int& x, const unsigned int& y )
	{
		if ( IsInWindow( x, y ) )
		{
			SetLocation( x, y );
			m_bLButtonDown = true;
		}
		else
		{
			OnLButtonUp( x, y );
		}
	}
	
	virtual void OnLButtonUp( const unsigned int& x, const unsigned int& y )
	{
		SetLocation( x, y );
		m_bLButtonDown = false;
	}
	
	virtual void Translate( const float& X, const float& Y, const float& Z )
	{
		D3DXMATRIX mov;
		D3DXMatrixTranslation( &mov, X, Y, Z );
		D3DXMatrixMultiply( &mTranslationMatrix, &mTranslationMatrix, &mov );
	}
	
	virtual void Translate( D3DXVECTOR3 const &t)
	{
		D3DXMATRIX mov;
		D3DXMatrixTranslation( &mov, t.x, t.y, t.z);
		D3DXMatrixMultiply( &mTranslationMatrix, &mTranslationMatrix, &mov );
	}

	virtual void Rotate(float fYaw, float fPitch, float fRoll)
	{
		D3DXMATRIX rot;
		D3DXMatrixRotationYawPitchRoll(&rot, fYaw, fPitch, fRoll);
		D3DXMatrixMultiply(&mRotationMatrix, &mRotationMatrix, &rot);
	}

    virtual void OnMouseMove( const unsigned int& x, const unsigned int& y )
	{
		D3DXVECTOR3		vTrans;
		D3DXVECTOR4		vT4;

		if ( !IsInWindow( x, y ) )
		{
			OnLButtonUp( x, y );
		}
		else if ( IsMouseDown() )
		{ 
			float deltax = ( (float)x - (float)mStartX );
			float deltay = ( (float)y - (float)mStartY );
			if ( !( ( deltax == 0.0f ) && ( deltay == 0.0f ) ) )
			{
				bool bShift = ( ( GetAsyncKeyState( VK_LSHIFT )   < 0 ) || ( GetAsyncKeyState( VK_SHIFT    ) < 0 ) || ( GetAsyncKeyState( VK_RSHIFT )  < 0 ) );
				//bool bAlt =   ( ( GetAsyncKeyState( VK_LMENU )    < 0 ) || ( GetAsyncKeyState( VK_RMENU    ) < 0 ) || ( GetAsyncKeyState( VK_MENU )    < 0 ) );
				bool bCtl =   ( ( GetAsyncKeyState( VK_LCONTROL ) < 0 ) || ( GetAsyncKeyState( VK_RCONTROL ) < 0 ) || ( GetAsyncKeyState( VK_CONTROL ) < 0 ) );
				if (bShift || bCtl) { //translation
					if (m_bUseCameraControl)
					{
						D3DXMATRIX matLookRotations;
						D3DXMatrixTranspose(&matLookRotations, &mRotationMatrix);
						D3DXVECTOR3 camLookDirection( 0.0f, 0.0f, 1.0f );
						D3DXVec3TransformNormal(&camLookDirection, &camLookDirection, &matLookRotations);

						D3DXVECTOR3 const   yVector(0.0f, 1.0f, 0.0f);
						float       const   h = D3DXVec3Dot(&yVector, &camLookDirection);
						D3DXVECTOR3         xzProjectedLookDirection(camLookDirection - h*yVector);

						D3DXVec3Normalize(&xzProjectedLookDirection, &xzProjectedLookDirection);

						D3DXVECTOR3         rightVector;         
						D3DXVec3Cross(&rightVector, &yVector, &xzProjectedLookDirection);

						D3DXVECTOR3         transVector;         

						if ( bShift )
						{
							transVector  =  deltax * mTranslationalSensitivityFactor * rightVector;
							transVector += -deltay * mTranslationalSensitivityFactor * yVector;

							Translate( transVector );
						}
						else
						{
							transVector  =  deltay * mTranslationalSensitivityFactor * xzProjectedLookDirection;
							Translate( transVector );
						}
					}
					else
					{

						if ( bShift )
						{
							vTrans = D3DXVECTOR3(	 deltax * mTranslationalSensitivityFactor,
													-deltay * mTranslationalSensitivityFactor,
													 0.0f );
						}
						else 
						{
							vTrans = D3DXVECTOR3(	 0.0f,
													 0.0f,
													 deltay * mTranslationalSensitivityFactor );
						}
						switch( m_eViewMode )
						{
						case VM_CAMERA_AWARE:

							// Transform the desired view space movement into world space
							//  so the objects moves appropriately given the camera view
							D3DXVec3TransformCoord( &vTrans, &vTrans, &m_mViewOrientationMatrix );
							
						case VM_STANDARD_ZAXIS:
						default:
							// do nothing - use the translation vector set above

							break;
						}

						Translate( vTrans );
					}
				}
				else { //rotation
					if (m_bYawPitchOnly) {
						mYaw -= D3DXToRadian(- mRotationalSensitivityFactor * deltax);
						if (mYaw < - 180.0f)
							mYaw += 360.0f;
						else if (mYaw > 180.0f)
							mYaw -= 360.0f;
						mPitch -= D3DXToRadian(- mRotationalSensitivityFactor * deltay);
						if (mPitch < - 90.0f)
							mPitch += 360.0f;
						else if (mPitch > 90.0f)
							mPitch -= 360.0f;
						D3DXMATRIX oldRot;
						D3DXMatrixTranspose(&oldRot, &mRotationMatrix);
						D3DXMatrixRotationYawPitchRoll(&mRotationMatrix, mYaw, mPitch, 0.0f);
						D3DXMatrixTranspose(&mRotationMatrix, &mRotationMatrix);
						D3DXVECTOR3 oldTrans(mTranslationMatrix._41, mTranslationMatrix._42, mTranslationMatrix._43);
						D3DXVECTOR3 newTrans;
						D3DXVec3TransformCoord(&newTrans, &oldTrans, &oldRot);
						D3DXVec3TransformCoord(&newTrans, &newTrans, &mRotationMatrix);
						D3DXMatrixTranslation(&mTranslationMatrix, newTrans.x, newTrans.y, newTrans.z);
					}
					else {
						float mag = (float)sqrt( deltax * deltax + deltay * deltay );
						float dx = ( deltax / mag );
						float dy = ( deltay / mag );
						
						//find a vector in the plane perpendicular to delta vector
						float perpx = dy;
						float perpy = dx;
						
						//rotate around this vector
						D3DXVECTOR3 axis( perpx, perpy, 0.0f );

						if( m_eViewMode == VM_CAMERA_AWARE )
						{
							// transform the rotation axis from view to world space also:
							D3DXVec3TransformCoord( &axis, &axis, &m_mViewOrientationMatrix );
						}
						D3DXMATRIX deltaRot;
						D3DXMatrixRotationAxis( &deltaRot, &axis, D3DXToRadian( -mRotationalSensitivityFactor * mag ) );
						D3DXMatrixMultiply( &mRotationMatrix, &mRotationMatrix, &deltaRot );
					}
				}
			}
			SetLocation( x, y );
		}
	}
};



#endif  _MOUSEUI_H_



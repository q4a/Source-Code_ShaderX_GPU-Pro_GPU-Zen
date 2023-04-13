/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\FogPolygonVolumes
File:  Effect_FogPolygonVolumes.h

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:

CgBrowser effect class.  ::Initialize() is the main entry point for the effect,
and the class creates the fog rendering helper class and the scene class.  It
also handles mouse and keyboard UI.



******************************************************************************/

#ifndef __Effect_FogPolygonVolumes_H
#define __Effect_FogPolygonVolumes_H

#pragma warning(disable: 4786)
#include <vector>

#include <NVD3DVers.h>
#include <NV_D3DCommon/ShaderManager.h>			// for easy load/cleanup of shaders
#include <NV_D3DCommon/D3DDeviceAndHWInfo.h>

#include <NVEffectsBrowser/eb_effect9.h>

using namespace std;


class MouseUI;

class SimpleObject;
class SimpleVBObject;
class ShadowVolumeVBObject;

class TextureDisplay;

class	PolygonalVolumeFog;
struct	Object_FogVolume;
class	Object_ExtrusionVolume;
class	Object_Solid;

class PolygonalVolumeFogScene;
class MatrixV;
class MatrixNode;


////////////////////////////////////////////////


class Effect_FogPolygonVolumes : public EBEffect
{
public:
	struct SimpleTime
	{
		int		m_nFrameCount;

		DWORD	m_dwStart;
		DWORD	m_dwCurrent;

		float	m_fIncrement;
	};

public:
	PolygonalVolumeFog * m_pPVF;

	PolygonalVolumeFogScene * m_pScene;

	D3DDeviceAndHWInfo	m_DeviceInfo;

	MouseUI	* m_pUI;

	void	Animate( float frame_interval_in_seconds );

	void	ToggleRendering( int nObjs, bool bStandInOn );	// turn various objects on or off


public:  
	int		m_nSceneSelect;
	int		m_nNumScenes;

	// functions to Set Render State
	HRESULT	Tick_SetStuffBeforeRendering();

	void	SRS_ForDiffuseDirectional( NV_IDIRECT3DTEXTURE *  pTex );

	// Functions to Set RenderState Constants
	void	SRSC_Ambient( D3DXVECTOR4 * pAmbient );
	void	SRSC_General( DWORD const_index, D3DXVECTOR4 * pValue );



	void	RenderObjects( vector< Object_Solid ** > * pvObj );
	void	RenderObjects( vector< Object_ExtrusionVolume ** > * pvObj,
							float extrusion_scale );


public:


	Effect_FogPolygonVolumes();
	~Effect_FogPolygonVolumes();
	virtual HRESULT Initialize( NV_D3DDEVICETYPE * pDev );
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick( EBTimer* pTimer );
	virtual HRESULT ConfirmDevice( NV_D3DCAPS * pCaps, DWORD dwBehavior, D3DFORMAT Format);

	virtual void UpdateProperties();
	virtual void PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten );
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

	void	MouseMove(HWND hWnd, int x, int y);
	void	MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);

	HRESULT		InitScene( int scene );

	HRESULT		LoadShaders();


	void SetDefaultView();
	void SetView();


	ShaderManager	* m_pShaderManager;

	bool	m_bAnimateBlob;
	bool	m_bRotateLight;

	float	m_fNearClip;
	float	m_fFarClip;

	bool	m_bDither;

protected:

	float	m_fAngle;
	int		m_nWireframe;

	SM_SHADER_INDEX		m_VSHI_DiffuseDirectional;


	D3DXVECTOR3  m_LightPos;		// in world space
	D3DXVECTOR3  m_EyePos;
	D3DXVECTOR3  m_vLookAt, m_vUp;

	D3DXMATRIX	m_matProj;
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matViewInv;
	D3DXMATRIX  m_matWorld;

	SimpleTime	* m_pTime;
};


#endif					// __Effect_FogPolygonVolumes_H
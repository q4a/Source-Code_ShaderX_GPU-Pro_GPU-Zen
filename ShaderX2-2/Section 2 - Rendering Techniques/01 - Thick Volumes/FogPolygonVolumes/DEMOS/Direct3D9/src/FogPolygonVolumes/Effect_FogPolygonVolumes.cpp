/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\FogPolygonVolumes
File:  Effect_FogPolygonVolumes.cpp

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


  
******************************************************************************/


#include <NVEffectsBrowser/eb_effect9.h>
#include <shared/MouseUI9.h>

#include "Effect_FogPolygonVolumes.h" 
#include "PolygonalVolumeFog.h"
#include "PolygonalVolumeFogScene.h"

#include "..\MEDIA\programs\D3D9_FogPolygonVolumes\Constants.h"

#include <shared/NV_Error.h>
#include <shared/NV_Common.h>

#include <NVD3DVers.h>
#include <NV_D3DCommon/SimpleObject.h>
#include <NV_D3DCommon/SimpleVBObject.h>

#include <NV_D3DCommon/WingedEdgeObject.h>
#include <NV_D3DCommon/ShadowVolumeObject.h>
#include <NV_D3DCommon/ShadowVolumeVBObject.h>

#include <NV_D3DCommon/TextureDisplay.h>
#include <NV_D3DCommon/ShaderManager.h>

#include <NV_D3DCommon/NoiseGrid3D.h>


using namespace std;

/////////////////////////////////////////////////////////////////

#ifndef CHECK_BOUNDS
#define CHECK_BOUNDS( var, min, max )			\
	if( (var < min) || (var > max) )			\
	{	FDebug("Variable out of bounds!\n");	\
		assert( false ); return;				\
	}
#endif

/////////////////////////////////////////////////////////////////

DECLARE_EFFECT_MAIN()
DECLARE_EFFECT_COUNT(1)
DECLARE_EFFECT_CREATE_BEG()
DECLARE_EFFECT_CREATE(0, Effect_FogPolygonVolumes())
DECLARE_EFFECT_CREATE_END()


//////////////////////////////////////////////////////////

Effect_FogPolygonVolumes::Effect_FogPolygonVolumes()
{

	m_fAngle = 0.0f;

	m_strEffectName		= "Fog Polygon Volumes";
	m_strEffectLocation = "Effects\\Atmospheric";
	m_strEffectVersion	= "1.0";

	m_pD3DDev		= NULL;
	m_pScene		= NULL;

	m_pPVF = NULL;
	m_pTime = NULL;

	m_pUI				= NULL;
	m_pShaderManager	= NULL;
	
}


Effect_FogPolygonVolumes::~Effect_FogPolygonVolumes()
{
	Free();
}

HRESULT Effect_FogPolygonVolumes::Free()
{

	// Set backbuffers to their initial state
	if( m_pPVF != NULL )
	{
		m_pPVF->SetRenderTargetsToDefault( true, true );
	}

	SAFE_DELETE( m_pScene );
	SAFE_DELETE( m_pPVF );
	SAFE_DELETE( m_pTime );
	SAFE_DELETE( m_pUI );
	SAFE_DELETE( m_pShaderManager );

	SAFE_RELEASE( m_pD3DDev );

	return S_OK;
}


void Effect_FogPolygonVolumes::UpdateProperties()
{
	EBEffect::UpdateProperties();

	// Add menu item to show keyboard commands
	AddProperty(new EBTriggerProperty("F1 or H for keyboard commands..."));


	/////////////////////////////////////////////////////
    string aboutText = GetFilePath("NVEffectsExplained.htm");

	// Vertex shader files:
	AddShaderCodeFile( SHADERTYPE_VERTEX, "Depth to TexCoord",
						"DepthToTexcrdForRGB.nvv" );
	AddShaderCodeFile( SHADERTYPE_VERTEX, "Depth to TexCoord + extrusion",
						"DepthToTexcrd_ObjectExtruder.nvv" );
	AddShaderCodeFile( SHADERTYPE_VERTEX, "RGB to depth setup",
						"RGBDifferenceToFogColor_11.nvv");


	AddShaderCodeFile( SHADERTYPE_PIXEL, "Depth to RGB Encoding", "DepthToRGBEncode.nvp" );
	AddShaderCodeFile( SHADERTYPE_PIXEL, "RGB difference to fog color", "RGBDifferenceToFogColor_13.nvp" );
	AddShaderCodeFile( SHADERTYPE_PIXEL, "Subtract depths", "SubtractRGBEncodedDepth_13.nvp" );

	

	SetAboutInfo( NULL, _T("Fog Polygon Volumes"), _T(aboutText.c_str()));
	SetAboutInfo( NULL, _T("Developer Relations"), _T("http://www.nvidia.com/developer"));
	SetAboutInfo( NULL, _T("NVIDIA Corporation"), _T("http://www.nvidia.com"));
	SetAboutInfo( _T("Date"), _T("May 2001"));
}


void Effect_FogPolygonVolumes::PropertyUpdateCallback( const EBProperty* pProperty,
													   bool bWritten)
{
	if (!bWritten)
		return;

	if (pProperty->IsKindOf( EBTYPE_TRIGGER_PROP ))
	{
		// only one trigger property which displays key commands
		Keyboard( 'H', 0, true );

	}
}



HRESULT Effect_FogPolygonVolumes::InitScene( int scene )
{
	HRESULT hr = S_OK;


	m_nNumScenes = 2;

	if( scene > m_nNumScenes )
		scene = 0;
	

	switch( scene )
	{

	case 0 :
		m_pScene->Initialize( m_pD3DDev, PolygonalVolumeFogScene::PVFS_HAZEBEAM_OBJECTS,
								effect_api::GetFilePath );
		m_bAnimateBlob = true;
		break;

	case 1 :
		m_pScene->Initialize( m_pD3DDev, PolygonalVolumeFogScene::PVFS_BUDDHA,
								effect_api::GetFilePath );
		m_bAnimateBlob = false;
		break;

	case 2 : 
		m_pScene->Initialize( m_pD3DDev, PolygonalVolumeFogScene::PVFS_OBJECTS_FROMSIMPLEFILE,
								effect_api::GetFilePath );
		m_bAnimateBlob = false;
		break;

	}


	return( hr );
}



HRESULT Effect_FogPolygonVolumes::Initialize( NV_D3DDEVICETYPE* pDev )
{
	FDebug("\n\n");
//	FDebug("Begin Effect_FogPolygonVolumes::Initialize...\n");

	HRESULT hr;

	FAIL_IF_NULL( pDev );
 	m_pD3DDev = pDev;
	pDev->AddRef();


	// get info about HW and surfaces
	m_DeviceInfo.Initialize( m_pD3DDev, true );		// device, verbose


	///////////////////////////////////////////////////////////

	m_nSceneSelect		= 0;

	m_bDither	= true;

	m_nWireframe	= false;
	m_bAnimateBlob	= true;
	m_bRotateLight	= true;


	///////////////////////////////////////////////////////////

	m_pScene	= new PolygonalVolumeFogScene;
	FAIL_IF_NULL( m_pScene );

	InitScene( m_nSceneSelect );


	///////////////

	m_pTime = new SimpleTime;
	FAIL_IF_NULL( m_pTime );

	m_pTime->m_nFrameCount = 0;
	m_pTime->m_dwStart = 0;


	///////////////////////////////////////////////////////////

	m_pShaderManager = new ShaderManager;
	assert( m_pShaderManager != NULL );
	m_pShaderManager->Initialize( m_pD3DDev, effect_api::GetFilePath );


	// Load vertex & pixel shaders
	hr = LoadShaders();
	ASSERT_AND_RET_IF_FAILED(hr);


	/////////////////////////////////////////////////////////////

	RECT rect;
	rect.left = rect.top = 0;
	NV_D3DVIEWPORT viewport;

	m_pD3DDev->GetViewport( &viewport );
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;

	FMsg("Viewport:    %d x %d\n", viewport.Width, viewport.Height );

	D3DSURFACE_DESC * pDesc;
	pDesc = m_DeviceInfo.GetDepthBufferDesc();
	if( pDesc != NULL )
		FMsg("Backbuffer:  %u x %u\n", pDesc->Width, pDesc->Height );

	/////////////////////////////////////////////////

	m_pUI = new MouseUI( (const RECT) rect );
	assert( m_pUI != NULL );

	m_pUI->SetTranslationalSensitivityFactor(0.01f);
    m_pUI->Reset();

	//////////////////////////////////////////////////
	// Set camera matrices -- 
	// Also sets m_fFarClip;

	m_fNearClip = 0.5f;
	m_fFarClip	= 100.0f;

	SetDefaultView();


	////////////////////////////////////////////////////////////////////////

	m_pPVF = new PolygonalVolumeFog;
	FAIL_IF_NULL( m_pPVF );

	float fThicknessToColorScale;
	fThicknessToColorScale = 34.0f;

	int width, height;
	width = rect.right;
	height = rect.bottom;


	m_pPVF->Initialize( m_pD3DDev,
						width,
						height,		// resolution of render targets
									// max depth complexity and resulting depth precision
						PolygonalVolumeFog::PVF_16_SURFACES_12_BIT_DEPTH,
									// scale for color ramp tex coord from object thickness
						fThicknessToColorScale,
						m_fNearClip,
						m_fFarClip,
						& m_pShaderManager );

	// Other good choices for depth & precision:
	//	PolygonalVolumeFog::PVF_32_SURFACES_9_BIT_DEPTH,
	//	PolygonalVolumeFog::PVF_16_SURFACES_12_BIT_DEPTH,
	//	PolygonalVolumeFog::PVF_8_SURFACES_15_BIT_DEPTH,
	//	PolygonalVolumeFog::PVF_4_SURFACES_18_BIT_DEPTH,


//	m_pPVF->SetParameters( PolygonalVolumeFog::PVF_16_SURFACES_12_BIT_DEPTH,
//							fThicknessToColorScale,
//							m_fNearClip, m_fFarClip );

	return S_OK;
}





void Effect_FogPolygonVolumes::SetView()
{	

	// Create view matrix
	D3DXMatrixLookAtLH( &m_matView, &m_EyePos, &m_vLookAt, &m_vUp );
	
	D3DXMatrixInverse( &m_matViewInv, NULL, &m_matView );

	if( m_pUI != NULL )
	{
		m_pUI->SetViewMode( MouseUI::VM_CAMERA_AWARE );
		m_pUI->SetControlOrientationMatrix( m_matView );
	}
}


void Effect_FogPolygonVolumes::SetDefaultView()
{
	// far distant
	m_EyePos.x	=	0.0f;
	m_EyePos.y	=	-12.0f;
	m_EyePos.z	=	1.687f;

	// close above, looking down on objects
	m_EyePos.x	=	-1.0f;
	m_EyePos.y	=	-1.0f;
	m_EyePos.z	=	3.5f;

	m_EyePos.x	=	-6.0f;
	m_EyePos.y	=	-3.0f;
	m_EyePos.z	=	1.5f;


	m_vLookAt.x = 0.0f;
	m_vLookAt.y = 0.0f;
	m_vLookAt.z = 2.0f;

	m_vUp.x = 0.0f;
	m_vUp.y = 0.0f;
	m_vUp.z = 1.0f;


	// Set world matrix to identity
	D3DXMatrixIdentity(&m_matWorld);

	// Set field of view
	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(55.0f),
								1.0f,
								m_fNearClip, m_fFarClip );		// znear,far


	SetView();


}



HRESULT Effect_FogPolygonVolumes::ConfirmDevice( NV_D3DCAPS * pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		m_strLastError = "Device does not support 3 element texture coordinates!";
		return E_FAIL;
	}

	if (!(pCaps->MaxTextureBlendStages >= 2))
	{
		m_strLastError = "Not enough texture blend stages!";
		return E_FAIL;
	}


	if(D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1 )
	{
		m_strLastError = "Device does not support pixel shaders 2.0!";
		return E_FAIL;
	}


	// Checking the backbuffer for stencil bits is not possible at this point.
	// That must be checked during intialization and accounted for

	return S_OK;
}




HRESULT Effect_FogPolygonVolumes::Start()
{
	m_fAngle = 0.0f;
	return S_OK;
}


void Effect_FogPolygonVolumes::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if( m_pUI == NULL )
		return;


	if( button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
		{
			m_pUI->OnLButtonDown(x, y);
		}
		else
		{
			m_pUI->OnLButtonUp(x, y);
		}
	}
}


void Effect_FogPolygonVolumes::MouseMove(HWND hWnd, int x, int y)
{
	if( m_pUI != NULL )
	{
		m_pUI->OnMouseMove(x, y);

//		FDebug("MouseMove %d %d\n", x, y );

	}

	return;
}


void Effect_FogPolygonVolumes::ToggleRendering( int nObjs, bool bStandInOn )
{
	// ugly bit of code to turn various objects on or off depending on 
	//  a counter, nObjs, and control

	RET_IF_NULL( m_pScene );

	bool bBlobOn;

	if( m_nSceneSelect == 0 )
	{
		bBlobOn = nObjs > 0;

		// When m_nSceneSelect == 0
		// nObjs = 0  for shafts of light
		// nObjs = 1  for 1 blob
		// nObjs = 2  for multiple blobs

		FMsg("bBlobOn = %d  nObjs = %d\n", bBlobOn, nObjs );

		// Turn on or off the rendering of the blob geometry vs.
		//  the shafts of light.

		m_pScene->m_objFogVolumesForOnOffControl.EnableRendering( bBlobOn && (!bStandInOn) );

		m_pScene->m_objLightBeam.EnableRendering( !bBlobOn );


		// nObjs == 0 is shafts of light which have no stand-in solids
		m_pScene->m_objStandInSolids.EnableRendering( bStandInOn && (nObjs !=0 ) );


		if( nObjs == 2 )
		{
			// enable other fog blobs if stand in solids are not on
			m_pScene->m_objOtherBlobs.EnableFogs( !bStandInOn );
			m_pScene->m_objOtherBlobs.EnableSolids( bStandInOn );
		}
		else
		{
			// if not 2, they're always off
			m_pScene->m_objOtherBlobs.EnableRendering( false );
		}

		// If blobs are on, set beam extrusion to zero.  This can be
		// adjusted with '[' or ']' keys
		if( bBlobOn )
		{
			m_pScene->SetAllExtrusionDistances( 0.0f );
		}
		else
		{
			m_pScene->SetAllExtrusionDistances( 15.0f * 0.3f );
		}
	}
	else
	{
		m_pScene->m_objStandInSolids.EnableSolids( bStandInOn );
		m_pScene->m_objStandInSolids.EnableFogs( !bStandInOn );
	}

}


void Effect_FogPolygonVolumes::Keyboard( DWORD dwKey, UINT nFlags, bool bDown)
{

	ASSERT_AND_RET_IF_NULL( m_pPVF );
	RET_IF_NULL( m_pScene );

	UINT charcode;


	float strafe_factor = 0.1f;
	float strafe_min    = 0.1f;
	float inset_inc		= 0.01f;


	float vdfac = 0.3f;		// scale factor for 1 step of changing shadow vol
							//  extrusion distance
	
	float weld_inc = 0.01f;

	float ASf	= 1.02f;	// A,S keys factor for changing thickness to tex coord scale
							// multiplicative scale
	float noisf = 1.02f;

	static int  nObjs = 0;
	static bool bStandInOn = false;

	if( bDown )
	{

		switch( dwKey )
		{

		// blob animation freq & amplitude
		case 'U':
			m_pScene->m_fNoiseFreq /= noisf; 
			FMsg("m_fNoiseFreq = %f\n", m_pScene->m_fNoiseFreq );
			break;
		case 'I':
			m_pScene->m_fNoiseFreq *= noisf; 
			FMsg("m_fNoiseFreq = %f\n", m_pScene->m_fNoiseFreq );
			break;

		case 'J':
			m_pScene->m_fNoiseAmp /= noisf;
			FMsg("m_fNoiseAmp = %f\n", m_pScene->m_fNoiseAmp );
			break;
		case 'K':
			m_pScene->m_fNoiseAmp *= noisf;
			FMsg("m_fNoiseAmp = %f\n", m_pScene->m_fNoiseAmp );
			break;

		case 'P' :
			// toggle between fog objects and stand-in solid objects
			if( nObjs >= 1 || m_nSceneSelect > 0 )
			{
				bStandInOn = !bStandInOn;
				ToggleRendering( nObjs, bStandInOn );
			}
			break;

		case VK_SPACE:
			if( m_pPVF->m_bDisplayFogCalcTextures )
			{
				m_pPVF->m_nPermute++;
			}
			else
			{
				// control which objects and scenes are displayed
				nObjs++;
				if( nObjs >= 3 || m_nSceneSelect > 0 )
				{
					nObjs = 0;
					m_nSceneSelect++;

					// turn off stand-in solids
					bStandInOn = false;

					m_nSceneSelect = m_nSceneSelect % m_nNumScenes;

					InitScene( m_nSceneSelect );

					FMsg("m_nSceneSelect = %d\n", m_nSceneSelect );
				}

				ToggleRendering( nObjs, bStandInOn );
			}

			break;

		case VK_F5:
			LoadShaders();
			m_pPVF->LoadShaders();
			break;

		case VK_F8:
			m_pPVF->m_bDisplayFogCalcTextures = ! m_pPVF->m_bDisplayFogCalcTextures;
			FDebug("m_bDisplayFogCalcTextures = %d\n", m_pPVF->m_bDisplayFogCalcTextures );
			break;

		case 'D':
			// Dither on / off
			m_bDither = ! m_bDither;

			if( m_bDither )
			{
				**(m_pScene->m_ppDitherControl) = **(m_pScene->m_ppDitherOn);
			}
			else
			{
				**(m_pScene->m_ppDitherControl) = **(m_pScene->m_ppDitherOff);
			}

			FDebug("m_bDither = %d\n", m_bDither );
			break;


		case 'A' :
			m_pScene->m_ActiveObjects.m_fThicknessToColorScale /= ASf;
			FDebug("m_fThicknessToColorScale = %f\n", m_pScene->m_ActiveObjects.m_fThicknessToColorScale );

			break;
		case 'S' :
			m_pScene->m_ActiveObjects.m_fThicknessToColorScale *= ASf;
			FDebug("m_fThicknessToColorScale = %f\n", m_pScene->m_ActiveObjects.m_fThicknessToColorScale );

			break;

		case 'C':
			m_pPVF->m_fScale *= 0.98f;
			FDebug("m_fScale = %f\n", m_pPVF->m_fScale );
			break;
		case 'V':
			m_pPVF->m_fScale /= 0.98f;
			FDebug("m_fScale = %f\n", m_pPVF->m_fScale );
			break;

		case 'L':
			m_bRotateLight = !m_bRotateLight;
			break;

		case 'B':
			m_bAnimateBlob = !m_bAnimateBlob;
			break;

		case 'W':
			// m_nWireframe 0 = off, 1 = on, 2 = on backface culled
			//  3 = on not occluded by solid

			m_nWireframe ++;
			m_nWireframe = m_nWireframe % 4;

			if( m_pPVF != NULL )
				m_pPVF->m_nWireframe = m_nWireframe;
	
			FDebug("m_nWireframe = %d\n", m_nWireframe );

			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			break;

		default:

			charcode = MapVirtualKey( dwKey, 2 ); // VK to char code
			switch( charcode )
			{

			case '.':
			case '>':
				// Used only when F8 m_bDisplayFogCalcTextures is on
				m_pPVF->m_nNumAdditivePasses++;
				FDebug("m_nNumAdditivePasses = %d\n", m_pPVF->m_nNumAdditivePasses );

				break;

			case ',':
			case '<':
				// Used only when F8 m_bDisplayFogCalcTextures is on
				m_pPVF->m_nNumAdditivePasses--;
				if( m_pPVF->m_nNumAdditivePasses < 0 )
					m_pPVF->m_nNumAdditivePasses = 0;

				FDebug("m_nNumAdditivePasses = %d\n", m_pPVF->m_nNumAdditivePasses );

				break;

			case '[':
			case '{':
				FDebug("Modifying object's fExDist - \n");

				// modify all extrusion distances of objects in the master list
				if( m_pScene != NULL )
				{
					m_pScene->IncrementAllExtrusionDistances( -vdfac );
				}
				break;

			case ']':
			case '}':
				FDebug("Modifying object's fExDist + \n");

				// modify all extrusion distances of objects in the master list
				if( m_pScene != NULL )
				{
					m_pScene->IncrementAllExtrusionDistances( vdfac );
				}
				break;
			};

			break;
		}
	}


	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
	EBString str;
 

	switch ( Action )
    {
		case EB_HELP:

			str =  " Help : F1 - Help\n\n";
			str += " W    - Wireframe toggle\n";

			str += "\n";
			str += "Shift + drag  - Pan camera\n";
			str += "Ctrl + drag   - Pan camera in/out\n";

			str += "\n";
			str += "SPACE - Cycle through scenes when not in F8 display\n"; 
			str += "P     - Display fog geometry as solid\n";

			str += "\n";
			str += "L     - Rotate shadow volume extrusion origin    \n";
			str += "B     - Animate blob\n";
			str += "U,I,J,K - blob animation freq and amp\n";

			str += "[     - Decrease shadow volume extrusion\n";
			str += "]     - Increase shadow volume extrusion\n";

			str += "\n";
			str += "F8    - Display fog calculation textures\n";
			str += "  <,>   - darken/brighten F8 texture display\n";

			str += "\n";
			str += "D     - Dither On / Off toggle\n";
			str += "A,S   - dec/inc fog thickness to texcoord scale\n";
			str += "C,V   - dec/inc depth-to-RGB scale\n";

			str += "\n";
			str += "F5    - reload shaders\n";

			str += "\n\n";
			str += "\n\n";

			MessageBoxEx( NULL, str.c_str(),
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
			break;
		default:
			break;
	}
}



void Effect_FogPolygonVolumes::SRS_ForDiffuseDirectional( NV_IDIRECT3DTEXTURE * pTex )
{
	// Texture assigned to stage 0

	MSG_AND_RET_IF_NULL( pTex, "Must supply texture pointer!" );

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,		D3DZB_TRUE );
	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,	true );
	m_pD3DDev->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );

	m_pD3DDev->SetRenderState( D3DRS_FILLMODE,		D3DFILL_SOLID );
	if( m_nWireframe == 3 )
	{
		m_pD3DDev->SetRenderState( D3DRS_FILLMODE,		D3DFILL_WIREFRAME );
	}


	m_pD3DDev->SetRenderState( D3DRS_STENCILENABLE,		false );
	m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE,	false );
	m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,			false );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,	false );
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );

	m_pD3DDev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA |
		                                               D3DCOLORWRITEENABLE_RED |
													   D3DCOLORWRITEENABLE_GREEN | 
													   D3DCOLORWRITEENABLE_BLUE );

	// Use vshader to calculate diffuse light and output to oD0, read here
	//   as D3DTA_DIFFUSE

	m_pD3DDev->SetPixelShader( 0 );

	m_pD3DDev->SetTexture( 0, pTex );
	m_pD3DDev->SetTexture( 1, NULL );
	m_pD3DDev->SetTexture( 2, NULL );
	m_pD3DDev->SetTexture( 3, NULL );

	// ( diffuse + tfactor * ) base texture
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,	  D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	
}



void Effect_FogPolygonVolumes::RenderObjects( vector< Object_Solid ** > * pvObj )
{
	// No batching of primitives is performed

	const D3DXMATRIX * pWVPMat;
	D3DXMATRIX transpose;

	assert( m_pPVF != NULL );		// polygonal fog volume class


	if( pvObj != NULL )
	{
		Object_Solid ** ppObj;
		Object_Solid * pObj;

		int i;
		for( i=0; i < (*pvObj).size(); i++ )
		{
			ppObj = (*pvObj).at(i);
			if( ppObj == NULL )
				continue;

			pObj = *ppObj;
			if( pObj == NULL )
				continue;
			if( pObj->ppGeometry == NULL )
				continue;
			if( pObj->bRenderOn == false )
				continue;
			if( pObj->ppMatrixNode == NULL )
				continue;

			pWVPMat = (*pObj->ppMatrixNode)->GetTotalMatrixPtr();
			D3DXMatrixTranspose( &transpose, pWVPMat );

			// Set vertex shader constants for world-view-projection matrix
			m_pPVF->SRSC_MatWorldViewProjectTrans( & transpose );

			//@@@@ light is in world space - not accounting for object to world space transform

			SRSC_Ambient( *pObj->ppAmbient );

			if( pObj->ppTexture != NULL )
			{
				if( *(pObj->ppTexture) != NULL )
				{
					m_pD3DDev->SetTexture( 0, *(pObj->ppTexture) );
				}
			}
			else
			{
				m_pD3DDev->SetTexture( 0, *(m_pScene->m_ppTexDefaultBase) );
			}

			(*pObj->ppGeometry)->Draw();
		}
	}
}



void Effect_FogPolygonVolumes::RenderObjects( vector< Object_ExtrusionVolume ** > * pvObj,
											float extrusion_scale )
{
	// No batching of primitives is performed

	const D3DXMATRIX * pWVPMat;
	D3DXMATRIX transpose;


	if( pvObj != NULL )
	{
		Object_ExtrusionVolume ** ppObj;
		Object_ExtrusionVolume * pObj;

		int i;
		for( i=0; i < (*pvObj).size(); i++ )
		{
			ppObj = (*pvObj).at(i);
			if( ppObj == NULL )
				continue;
			pObj = *ppObj;
			if( pObj == NULL )
				continue;
			if( pObj->ppGeometry == NULL )
				continue;
			if( pObj->bRenderOn == false )
				continue;
			if( pObj->ppMatrixNode == NULL )
				continue;

			pWVPMat = (*pObj->ppMatrixNode)->GetTotalMatrixPtr();
			D3DXMatrixTranspose( &transpose, pWVPMat );
			// Set vertex shader constants for world-view-projection matrix
			m_pPVF->SRSC_MatWorldViewProjectTrans( & transpose );


			// Set extrusion origin in object space
			m_pPVF->SRSC_ExtrusionOriginObjSpc( *pObj->ppExtrusionOriginO );
			// Set extrusion distance
			m_pPVF->SRSC_ExtrusionDistance( pObj->fExDist * extrusion_scale );
			// Set amount to inset vertices along vertex normal
			m_pPVF->SRSC_ExtrusionInset( 0.0f );

			SRSC_Ambient( *pObj->ppAmbient );

			// Render the geometry
			(*pObj->ppGeometry)->Draw();
		}
	}
}


void	Effect_FogPolygonVolumes::SRSC_General( DWORD const_index, D3DXVECTOR4 * pValue )
{
	// Set extrusion origin in object space
	if( pValue != NULL )
	{
		m_pD3DDev->SetVertexShaderConstantF( const_index, (float*)pValue, 1 );
	}
	else
	{
		assert( false );
	}
}


void	Effect_FogPolygonVolumes::SRSC_Ambient( D3DXVECTOR4 * pAmbient )
{
	SRSC_General( CV_OBJ_AMBIENT_COL, pAmbient );
}




HRESULT Effect_FogPolygonVolumes::LoadShaders()
{
	FDebug("Loading shaders from disk!\n");

	HRESULT hr = S_OK;


	// Load vertex & pixel shaders

	hr = m_pShaderManager->LoadAndAssembleShader(
			GetFilePath("D3D9_FogPolygonVolumes\\DiffuseDirectional.nvv"),
			SM_SHADERTYPE_VERTEX,
			& m_VSHI_DiffuseDirectional	 );
	ASSERT_AND_RET_IF_FAILED( hr );


	return( hr );
}




void	Effect_FogPolygonVolumes::Animate( float frame_interval_in_seconds )
{
	static int cnt = 0;
	static float total_time = 0.0f;

	// total_time will grow until increments are too small
	//  compared to its value.
	total_time += frame_interval_in_seconds;

	// Animate the geometry every third frame
	if( m_bAnimateBlob &&
		((cnt%3) == 0)			)
	{
		if( m_pScene != NULL )
			m_pScene->AnimateBlob01( total_time );
	}



	if( 0 )
	{	
		// crude animation of the dither pattern each frame
		D3DXVECTOR4 control = D3DXVECTOR4( rand(),
											rand(),
											rand(),
											1.0f );
		D3DXVec4Normalize( &control, &control );
		control = control * 5.0f;

		**(m_pScene->m_ppDitherControl) = control;
	}

	cnt++;
}




HRESULT Effect_FogPolygonVolumes::Tick( EBTimer * pTimer )
{
	FAIL_IF_NULL( m_pPVF );
	FAIL_IF_NULL( m_pD3DDev );

	HRESULT hr = S_OK;


	if( pTimer != NULL )
	{
		Animate( pTimer->GetInterval() );
	}

	Tick_SetStuffBeforeRendering();


	// Give the PolygonFogVolume class a list of objects to render

	m_pPVF->m_pFogSceneObjects	= & (m_pScene->m_ActiveObjects);

	m_pPVF->SetThicknessToColorTexCoordScale( m_pScene->m_ActiveObjects.m_fThicknessToColorScale );



	if( m_pPVF->m_bDisplayFogCalcTextures )
	{
		// If displaying fog calculation textures, just render the 
		// fog and return.  This skips rendering solid objects
		// to the screen.

		hr = m_pPVF->RenderFogToScene( m_pScene->m_ppTexFogColorRamp );

		ASSERT_AND_RET_IF_FAILED(hr);

		return( hr );
	}


	////////////////////////////////////////////////////
	// Render scene objects to the ordinary backbuffer

	// Render object's ambient + faint directional light values
	// No texturing
	// This gives some faint illumination throughout the scene.

	// Set render targets to the ordinary backbuffer
	m_pPVF->SetRenderTargetsToDefault( true, true );


	// Clear backbuffer
	hr = m_pD3DDev->Clear( 0, NULL,
							D3DCLEAR_TARGET | m_DeviceInfo.GetDepthClearFlags(),
							m_pScene->m_ActiveObjects.m_dwFBClearColor,
							1.0, 0);
	ASSERT_AND_RET_IF_FAILED(hr);
	

	if( m_nWireframe )
	{
		m_pPVF->Render_FogObjectsWireframe( 0x00FF0000, true );
	}


	SRS_ForDiffuseDirectional( *(m_pScene->m_ppTexDefaultBase) );
	m_pShaderManager->SetShader( m_VSHI_DiffuseDirectional );


	RenderObjects( & (m_pScene->m_ActiveObjects.m_vSolidsAffectingFog ) );


	// Render hole objects as solid with 0 extrusion
	RenderObjects( & (m_pScene->m_ActiveObjects.m_vFogExtrusionHoles), 0.0f );


	/////////////////////////////////////////////////////
	// Render objects to fog thickness calculation textures
	//  and blend fog into the scene.

	hr = m_pPVF->RenderFogToScene( m_pScene->m_ppTexFogColorRamp );

	ASSERT_AND_RET_IF_FAILED(hr);


	return( hr );
}




HRESULT	Effect_FogPolygonVolumes::Tick_SetStuffBeforeRendering()
{
	HRESULT hr = S_OK;

	FAIL_IF_NULL( m_pPVF );


	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,	true );
	m_pD3DDev->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,		D3DZB_TRUE );


	D3DXMATRIX matWorld, matInvWorld;
	D3DXMATRIX matTemp;

	D3DXVECTOR4  eyeObjSpc, lightObjSpc;	   // eye and light positions in object space


	// Demo used to base object animation on timeGetTime() -- this is NOT good
	// Instead, time the first few frames and then decide a fixed increment 
	// for updating animation

	const num_frames_to_time = 50;

	if( m_pTime->m_nFrameCount == 0 )
	{
		m_pTime->m_dwStart = timeGetTime();
		
		m_pTime->m_fIncrement = 0.02f;	// pick a general starting increment
	}


	float land_rot_scale = 300.8f;
	land_rot_scale = 1.0f;


	if( m_pTime->m_nFrameCount == num_frames_to_time )
	{
		m_pTime->m_dwCurrent = timeGetTime();

		// compute interval in milliseconds
		float interval = (float)( m_pTime->m_dwCurrent - m_pTime->m_dwStart );

		// compute time per frame, in seconds
		interval = interval / ( 1000.0f * num_frames_to_time );

		// compute angle increment value from the time per frame
		m_pTime->m_fIncrement = interval * 3.14159 * land_rot_scale;


		FDebug("Computed new increment:  %f  radians/frame\n", m_pTime->m_fIncrement );

		FDebug("\n");

	}

	m_pTime->m_nFrameCount ++;

	///////////////////////////////////////////////

	if( m_bRotateLight )
	{
		m_fAngle += m_pTime->m_fIncrement;

		m_fAngle = (float) fmod( m_fAngle, 360.0f );
	}

	// Set light position:
	float lightr = 2.0f;
	float lang  =  m_fAngle * 8.0f;

	m_LightPos.x  = lightr * (float)sin( D3DXToRadian( lang ) );
	m_LightPos.y  = lightr * (float)cos( D3DXToRadian( lang ) );
	m_LightPos.z  = 5.0f;
	m_LightPos.z  = 7.0f;



	// No automatic rotation of the world
	D3DXMatrixIdentity( &matWorld );

	///////////////////////////////////////////////////////////
	//  apply MouseUI control matrix

	D3DXMATRIX matMouseUI;
	D3DXMatrixMultiply( &matMouseUI, m_pUI->GetRotationMatrixPtr(), m_pUI->GetTranslationMatrixPtr() );

	// Apply mouse UI rotation after initial rotation in matWorld

	D3DXMatrixMultiply( &matWorld, &matWorld, &matMouseUI );

	// calculate matTemp =  world * view * projection matrices
	D3DXMatrixMultiply( &matTemp, &matWorld, &m_matView );
	D3DXMatrixMultiply( &matTemp, &matTemp, &m_matProj );

	// Update the scene with new positions, matrices, etc.
	// Update the view-projection matrix used by the various nodes
	//   that contain geometry.

	if( m_pScene != NULL )
	{
		if( m_pScene->m_ppMat_ViewProject != NULL )
		{
			MatrixV * pMat = *(m_pScene->m_ppMat_ViewProject);

			if( pMat != NULL )
			{
				pMat->m_Matrix = matTemp;
				pMat->MarkAsUpdated();
			}
		}

		if( m_pScene->m_ppExtrusionOrigin1 != NULL )
		{
			D3DXVECTOR4 * pvec = *(m_pScene->m_ppExtrusionOrigin1);
			if( pvec != NULL )
			{
				*pvec = D3DXVECTOR4( m_LightPos.x, m_LightPos.y, m_LightPos.z, 1.0f );
			}
		}


		////////////////////////////////////////////////
		// Animate matrices if the pointers are not NULL
		// Very simple animation to illustrate the point

		int i;
		float ang = lang * 2.3f;

		for( i=0; i < m_pScene->m_vppNegativeObjRotation.size(); i++ )
		{
			MatrixV ** ppR;
			MatrixV * pR;
			ppR = m_pScene->m_vppNegativeObjRotation.at(i);

			if( ppR != NULL )
			{
				if( *ppR != NULL )
				{
					pR = *ppR;
					pR->MarkAsUpdated();

					D3DXVECTOR3 axis( 0.0f, 0.0f, 1.0f );

					D3DXVec3Normalize( &axis, &axis );

					D3DXMatrixRotationAxis( & pR->m_Matrix, & axis, D3DXToRadian(ang) );

					int j;
					Object_ExtrusionVolume ** ppOEV;
					Object_ExtrusionVolume * pOEV;

					for( j =0; j < m_pScene->m_ActiveObjects.m_vFogExtrusionHoles.size(); j++ )
					{
						ppOEV = m_pScene->m_ActiveObjects.m_vFogExtrusionHoles.at(j);
						if( ppOEV == NULL )
							continue;
						pOEV = *ppOEV;
						if( pOEV == NULL )
							continue;
						if( pOEV->ppWorldMatrixNode == NULL )
							continue;
						if( (*(pOEV->ppWorldMatrixNode)) == NULL )
							continue;

						if( (*pOEV->ppWorldMatrixNode)->DependsOn( pR ) )
						{
							// transform extrusion origin by inverse of the world matrix
							D3DXMATRIX matInv;
							D3DXMatrixInverse( &matInv, NULL, (*pOEV->ppWorldMatrixNode)->GetTotalMatrixPtr() );

							D3DXVECTOR4 extrude_orig;					
							D3DXVECTOR4 orig_world;

							if( m_pScene->m_ppExtrusionOrigin1 != NULL )
								if( *(m_pScene->m_ppExtrusionOrigin1) != NULL )
								{
									orig_world = **(m_pScene->m_ppExtrusionOrigin1);

									D3DXVec4Transform( &extrude_orig, &orig_world, &matInv );
								}

							if( pOEV->ppExtrusionOriginO != NULL )
								if( *(pOEV->ppExtrusionOriginO) != NULL )
								{
									// 6.0 is size of postive volume being subtracted from!
									**(pOEV->ppExtrusionOriginO) = extrude_orig;
								}
						}
					}
				}
			}
		}
	}

	////////////////////////////////////////////////////////////

	m_pD3DDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE);


	float lb = 0.5f;		// color of directional light
	m_pD3DDev->SetVertexShaderConstantF( CV_DIRLIGHT1_COLOR,
					(float*)(& D3DXVECTOR4( lb, lb, lb, 0.0f )), 1 );


	return( hr );
}







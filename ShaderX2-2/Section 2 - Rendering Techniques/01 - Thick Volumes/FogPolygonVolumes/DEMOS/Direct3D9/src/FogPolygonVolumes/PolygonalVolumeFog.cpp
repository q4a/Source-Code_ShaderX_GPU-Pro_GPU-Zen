/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\FogPolygonVolumes
File:  PolygonalVolumeFog.cpp

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

See header file for comments

  
******************************************************************************/

//#define NVMSG_SAFE_ARRAY_DELETE
//#define NVMSG_SAFE_DELETE
//#define NVMSG_SAFE_RELEASE


#include <shared/NV_Error.h>
#include <shared/NV_Common.h>

#include "NVD3DVers.h"
#include <NV_D3DCommon/SimpleVBObject.h>
#include <NV_D3DCommon/ShadowVolumeObject.h>
#include <NV_D3DCommon/ShadowVolumeVBObject.h>
#include <NV_D3DCommon/TextureDisplay.h>
#include <NV_D3DCommon/ShaderManager.h>
#include <NV_D3DCommon/MatrixNode.h>
#include <NV_D3DCommon/D3DDeviceAndHWInfo.h>

#include "PolygonalVolumeFog.h" 
#include "..\MEDIA\programs\D3D9_FogPolygonVolumes\Constants.h"

using namespace std;


#define PVFTRACE0	NullFunc
//#define PVFTRACE0	FMsg


//////////////////////////////////////////////////////////

PolygonalVolumeFog::PolygonalVolumeFog()
{
	SetAllNull();
	m_nWireframe		= 0;
	

	m_nSolidDepths		= 0;
	m_nFrontfaceDepths	= 1;
	m_nBackfaceDepths	= 2;
	m_nDepthDifference	= 3;	// used only for ps.1.3 path

	// Clear color for RGB depth acumulation is black because surface
	//  depth values are additively blended.
	m_dwRGBDepthClearColor = 0x00000000;

	// Color for clearing the solid object depth texture is white
	// because that is the greatest depth value and solid depths are
	// not alpha blended into the texture render target.
	m_dwRGBSolidsClearColor = 0x00FFFFFF;

}

PolygonalVolumeFog::~PolygonalVolumeFog()
{
	Free();	
}


void PolygonalVolumeFog::SetAllNull()
{

	m_pFogSceneObjects			= NULL;

	m_pTexBlueDepthRamp			= NULL;
	m_pTexRedGreenDepthRamp		= NULL;

	m_pTextureDisplay			= NULL;

	m_DepthValuesPerColorChannel = PVF_UNSET;


	m_VSHI_DepthToTexcrd_ObjectExtruder		= -1;
	m_PSHI_RGBDifferenceToFogColor_13		= -1;
	m_VSHI_RGBDifferencetoFogColor_11		= -1;
	m_PSHI_SubtractRGBAndGetFogColor_20		= -1;

	m_PSHI_DepthToRGBEncode					= -1;
	m_PSHI_DepthToRGBAndCompare_13			= -1;
	m_PSHI_DepthToRGBAndCompare_20			= -1;
	m_PSHI_SubtractRGBEncodedDepth_13		= -1;

	m_VSHI_DepthToTexcrdForRGB				= -1;
	m_VSHI_DepthToTexcrdForRGB_TC4Proj		= -1;

}	


HRESULT PolygonalVolumeFog::Free()
{
	RenderToTextureBase::Free();

	SAFE_DELETE( m_pTextureDisplay );

	// Free textures
	int i;
	for( i=0; i < m_vppFVTexToFree.size(); i++ )
	{
		NV_IDIRECT3DTEXTURE ** ppFVTex;
		ppFVTex = m_vppFVTexToFree.at(i);		
		if( ppFVTex != NULL )
		{
			SAFE_RELEASE( *ppFVTex );
		}
		m_vppFVTexToFree.at(i) = NULL;
	}
	m_vppFVTexToFree.clear();

	SetAllNull();

	return S_OK;
}



HRESULT PolygonalVolumeFog::LoadShaders()
{
	ShaderManager * pManager = GetShaderManager();
	FAIL_IF_NULL( pManager );

	FDebug("Loading shaders from disk!\n");

	HRESULT hr = S_OK;



	// Vertex shader to generate texture coordinates for encoding
	//  pixel depth as an RGB color.

	hr = pManager->LoadAndAssembleShader(
			"D3D9_FogPolygonVolumes\\DepthToTexcrdForRGB.nvv",
			SM_SHADERTYPE_VERTEX,
			& m_VSHI_DepthToTexcrdForRGB );
	ASSERT_AND_RET_IF_FAILED( hr );

	hr = pManager->LoadAndAssembleShader(
			"D3D9_FogPolygonVolumes\\DepthToTexcrd_ObjectExtruder.nvv",
			SM_SHADERTYPE_VERTEX,
			& m_VSHI_DepthToTexcrd_ObjectExtruder );
	ASSERT_AND_RET_IF_FAILED( hr );


	hr = pManager->LoadAndAssembleShader(
			"D3D9_FogPolygonVolumes\\DepthToTexcrdForRGB_TC4Proj.nvv",
			SM_SHADERTYPE_VERTEX,
			& m_VSHI_DepthToTexcrdForRGB_TC4Proj );
	ASSERT_AND_RET_IF_FAILED( hr );

	hr = pManager->LoadAndAssembleShader(
			"D3D9_FogPolygonVolumes\\DepthToRGBEncode.nvp",
			SM_SHADERTYPE_PIXEL,
			& m_PSHI_DepthToRGBEncode );
	ASSERT_AND_RET_IF_FAILED( hr );


	// A vertex shader to override TextureDisplay's vertex shader
	// so we can add our own texture coordinates

	hr = pManager->LoadAndAssembleShader(
			"D3D9_FogPolygonVolumes\\RGBDifferenceToFogColor_11.nvv",
			SM_SHADERTYPE_VERTEX,
			& m_VSHI_RGBDifferencetoFogColor_11 );
	ASSERT_AND_RET_IF_FAILED( hr );


	// Now load shaders specific to ps_2_0 or ps.1.3 hardware

	if( m_bUsePS20 == true )
	{
		// Shaders for ps_2_0 capable HW

		hr = pManager->LoadAndAssembleShader(
				"D3D9_FogPolygonVolumes\\DepthToRGBAndCompare_20.psh",
				SM_SHADERTYPE_PIXEL,
				& m_PSHI_DepthToRGBAndCompare_20 );
		ASSERT_AND_RET_IF_FAILED( hr );

		hr = pManager->LoadAndAssembleShader(
				"D3D9_FogPolygonVolumes\\SubtractRGBAndGetFogColor_20.psh",
				SM_SHADERTYPE_PIXEL,
				& m_PSHI_SubtractRGBAndGetFogColor_20 );
		ASSERT_AND_RET_IF_FAILED( hr );
	}
	else
	{
		// Shaders for ps.1.3 capable hardware

		hr = pManager->LoadAndAssembleShader(
				"D3D9_FogPolygonVolumes\\DepthToRGBAndCompare_13.nvp",
				SM_SHADERTYPE_PIXEL,
				& m_PSHI_DepthToRGBAndCompare_13 );
		ASSERT_AND_RET_IF_FAILED( hr );

		hr = pManager->LoadAndAssembleShader(
				"D3D9_FogPolygonVolumes\\SubtractRGBEncodedDepth_13.nvp",
				SM_SHADERTYPE_PIXEL,
				& m_PSHI_SubtractRGBEncodedDepth_13 );
		ASSERT_AND_RET_IF_FAILED( hr );

		hr = pManager->LoadAndAssembleShader(
				"D3D9_FogPolygonVolumes\\RGBDifferenceToFogColor_13.nvp",
				SM_SHADERTYPE_PIXEL,
				& m_PSHI_RGBDifferenceToFogColor_13 );
		ASSERT_AND_RET_IF_FAILED( hr );
	}

	return( hr );
}



HRESULT PolygonalVolumeFog::Initialize( NV_D3DDEVICETYPE * pDev,
										int resolution_x,
										int resolution_y,
										PolygonalVolumeFog::MaxFogDepthComplexity max_depth_complexity,
										float fThicknessToColorScale,
										float fNearClip,
										float fFarClip,
										ShaderManager ** ppShaderManager )
{
	FAIL_IF_NULL( pDev );
	HRESULT hr;


	m_dwFogToScreenSrcBlend = D3DBLEND_ONE;
	m_dwFogToScreenDestBlend = D3DBLEND_INVSRCCOLOR;
	m_dwFogToScreenBlendOp	= D3DBLENDOP_ADD;

	m_nNumAdditivePasses	= 0;
	m_nPermute				= 0;

	m_fNDotLExtrudeThresh = 0.0f;		// Extrude shadow volume objects
										//  if N Dot L < this value

	m_bDisplayFogCalcTextures = false;


	// Use pixel shaders 1.3 or 2.0 depending on device capabilities
	D3DDeviceAndHWInfo	devinfo;
	devinfo.Initialize( pDev );
	float fvshader, fpshader;
	devinfo.GetHWShaderInfo( & fvshader, & fpshader );

	int num_RTTextures;

	if( fpshader >= 2.0f )
	{
		m_bUsePS20		= true;
		num_RTTextures	= 3;		// solid, front, and back

		FMsg("PolygonalVolumeFog using ps_2_0\n");
	}
	else if( fpshader >= 1.3f )
	{
		m_bUsePS20		= false;
		num_RTTextures	= 4;		// solid, front, back, and difference

		FMsg("PolygonalVolumeFog using ps.1.3\n");
	}
	else
	{
		FMsg("PolygonalVolumeFog: Device must support pixel shaders 1.3!  psh version = %f\n", fpshader );
		assert( false );
		return( E_FAIL );
	}


	RenderToTextureBase::Initialize(	pDev,
										ppShaderManager,
										resolution_x,
										resolution_y,
										num_RTTextures,		// render targets
										true );				// allocate depth buffer


	///////////////////////////////////////////////////
	// A class to render quad over entire render target
	//  and to display intermediate render-to-texture
	//  results

	m_pTextureDisplay = new TextureDisplay;
	assert( m_pTextureDisplay != NULL );

	// NULL shader manager causes it to allocate it's own shader manager
	// CV_PTS_BASE indicates the vshader constants the texture display's shaders
	//  should use.
	m_pTextureDisplay->Initialize( m_pD3DDev, NULL, CV_PTS_BASE );


	////////////////////////////////////////////////////////
	// Load vertex & pixel shaders

	hr = LoadShaders();
	ASSERT_AND_RET_IF_FAILED(hr);


	// Set parameters for depth scale, tradeoff between 
	//  the depth precision and the max number of overlapping
	//  surfaces, etc.

	SetParameters( max_depth_complexity, fThicknessToColorScale, fNearClip, fFarClip );



	// Make a rect to use with TextureDisplay to cover the full screen
	// Offset position coords by 1/2 texel width and height to sample
	//  from the exact texel centers

	float ox, oy;
	ox = 0.5f / m_nRTTWidth;
	oy = 0.5f / m_nRTTHeight;
	m_RectFullCoverage.bottom	= 0.0f - oy;
	m_RectFullCoverage.top		= 1.0f - oy;
	m_RectFullCoverage.left		= 0.0f - ox;
	m_RectFullCoverage.right	= 1.0f - ox;


	PVFTRACE0("End PolygonalVolumeFog::Initialize...\n");
	return S_OK;
}



void	PolygonalVolumeFog::CreateGradient2D( NV_IDIRECT3DTEXTURE ** ppTex2D,
				int width, int height, byte lower, byte upper,
				D3DXVECTOR3 col_mask_u, D3DXVECTOR3 col_mask_v )
{

	// Creates two gradients in a 2D texture.
	// col_mask_u is applied to 1st gradient in U axis
	// col_mask_v is applied to 2nd gradient in V axis
	//
	// each color mask should have values in the range [0,1]

	// Color mask is from [0,1] and gets multiplied into
	//  the color components

	ASSERT_AND_RET_IF_NULL( ppTex2D );
	ASSERT_AND_RET_IF_NULL( m_pD3DDev );

	FreeTexture( *ppTex2D );


	HRESULT hr;
#ifdef NV_USING_D3D8
	hr = m_pD3DDev->CreateTexture( width, height,
									1,				// mip levels
									0,				// usage
									D3DFMT_A8R8G8B8,
									D3DPOOL_MANAGED,
									ppTex2D );
#endif
#ifdef NV_USING_D3D9
	hr = m_pD3DDev->CreateTexture( width, height,
									1,				// mip levels
									0,				// usage
									D3DFMT_A8R8G8B8,
									D3DPOOL_MANAGED,
									ppTex2D,
									NULL );
#endif

	if( FAILED(hr))
	{
		FDebug("Can't create texture!\n");
		assert( false );
		return;
	}
	ASSERT_AND_RET_IF_NULL( *ppTex2D );

	m_vppFVTexToFree.push_back( ppTex2D );	// add texture to list of textures to free


	// Now lock and fill the texture with the gradient
	NV_IDIRECT3DTEXTURE * pTex = *ppTex2D;
	D3DLOCKED_RECT lr;
	hr = pTex->LockRect( 0, &lr,
						NULL,		// region to lock - NULL locks whole thing
						0 );		// No special lock flags
									// Can't use LOCK_DISCARD unless texture is DYNAMIC
	ASSERT_IF_FAILED(hr);

	byte * pixel;
	byte * pBase = (BYTE*) lr.pBits;
	int bytes_per_pixel = 4;
	assert( bytes_per_pixel * width == lr.Pitch );

	byte colu;
	byte colv;
	int i,j;
	D3DXVECTOR3 sum;

	for( j=0; j < height; j++ )
	{
		for( i=0; i < width; i++ )
		{
			pixel = pBase + j * lr.Pitch + i * bytes_per_pixel;

			// calculate gradient values
			colu = (byte)( lower + (upper-lower) * (float)i / ((float)width - 1.0f));

			colv = (byte)( lower + (upper-lower) * (float)j / ((float)height - 1.0f));

			sum = colu * col_mask_u + colv * col_mask_v;

			// alpha should be 255 because it is used later in alpha test
			*((D3DCOLOR*)pixel) = D3DCOLOR_ARGB( 0,
												(BYTE)( sum.x ),
												(BYTE)( sum.y ),
												(BYTE)( sum.z ) );
		}	
	}

	hr = pTex->UnlockRect( 0 );
	ASSERT_IF_FAILED( hr );
	FDebug("Created 2D gradient texture!\n");


	if( 0 )
	{
		string filename;
		filename = "C:\\RG_0_15.bmp";

		FMsg("Texture filename [%s]\n", filename.c_str() );
		
		// D3DXIFF_BMP can't be parsed by Photoshop
		//  but if you re-save it in another app it will be ok for Photoshop
		hr = D3DXSaveTextureToFile( filename.c_str(),
									D3DXIFF_BMP,
									*ppTex2D,
									NULL	);		// palette 
		ASSERT_IF_FAILED(hr);
	}
}



void	PolygonalVolumeFog::CreateGradientDithered( NV_IDIRECT3DTEXTURE ** ppTex,
													int width, int height,
													byte lower, byte upper,
													D3DXVECTOR3 color_mask,
													const D3DCOLOR * pDither_upper )
{
	// width, height are resolution of texture
	// lower, upper are values for the limits of the gradient
	// color_mask is applied to byte values to generate the color
	// pDither_upper can point to a color value.  If not NULL, then the 
	//    upper texels (rightmost) are dithered to the color pointed to.

	// Set true to save the resulting texture to a file for inspection
	bool bSaveTexture = false;


	// Texture dither should continue at the top and bottom to the 
	//  next highest and lowest values, respectively, but this would
	//  require storing and blending a negative green value at the
	//  lowest dither points.  That's impossible, but we can store a
	//  positive value in the alpha channel and treat it as a negative
	//  green value in pixel shaders 2.0.
	// We can't treat the alpha channel as negative green value with
	//  pixel shaders 1.3 in the depth comparison shader because we
	//  run out of instructions.
	// Dither the lower bits only if using ps_2_0.

	bool bDitherLowerToBlueAndAlpha = false;
	if( m_bUsePS20 )
	{
		bDitherLowerToBlueAndAlpha = true;
	}


	HRESULT hr = S_OK;

	// Creates a gradient in X axis of the texture
	// Color mask is from [0,1] and gets multiplied into
	//  the color components

	ASSERT_AND_RET_IF_NULL( ppTex );
	ASSERT_AND_RET_IF_NULL( m_pD3DDev );
	
	FreeTexture( *ppTex );

	// Generating mip maps for this low bits texture does not
	// improve performance, but increasing or decreasing the
	// texture coordinate scale does affect performance.  As the
	// texture coordinate dither scale increases, performance 
	// decreases.  This is due to the degree of coherence of
	// samples taken from the texture and the texture cache
	// utilization.

#ifdef NV_USING_D3D8
	hr = m_pD3DDev->CreateTexture( width, height,
									1,				// mip levels
									0,				// usage
									D3DFMT_A8R8G8B8,
									D3DPOOL_MANAGED,
									ppTex );
#endif
#ifdef NV_USING_D3D9
	hr = m_pD3DDev->CreateTexture( width, height,
									1,				// mip levels, 0 = all
									0,				// usage
									D3DFMT_A8R8G8B8,
									D3DPOOL_MANAGED,
									ppTex,
									NULL );
#endif

	if( FAILED(hr))
	{
		FDebug("CreateGradientDithered() Can't create texture!\n");
		assert( false );
		return;
	}
	ASSERT_AND_RET_IF_NULL( *ppTex );

	m_vppFVTexToFree.push_back( ppTex );		// add texture to list of textures to free


	// Now lock and fill the texture with the gradient
	NV_IDIRECT3DTEXTURE * pTex = *ppTex;

	D3DLOCKED_RECT lr;

	hr = pTex->LockRect( 0, &lr,
						NULL,		// region to lock - NULL locks whole thing
						0 );		// No special lock flags
									// Can't use LOCK_DISCARD unless texture is DYNAMIC
	ASSERT_IF_FAILED(hr);

	byte * pixel;
	byte * pBase = (BYTE*) lr.pBits;

	int bytes_per_pixel = 4;

	assert( bytes_per_pixel * width == lr.Pitch );

	int val;
	int dithered_val;
	DWORD color;
	int noiseinc;
	float fcol;
	float frand;
	int i,j;


	for( j=0; j < height; j++ )
	{
		for( i=0; i < width; i++ )
		{
			pixel = pBase + j * lr.Pitch + i * bytes_per_pixel;

			// calculate gradient values
			val = (int)( lower + (upper+1-lower) * ((float)i / ((float)width)));


			fcol = lower + (upper+1-lower) * ((float)i / ((float)width));

			fcol = (float) fmod( fcol, 1.0f );
			frand = ((float)rand()) / ((float)RAND_MAX);
			if( fcol < 0.5f )
			{
				fcol = 0.5f - fcol;
				if( frand < fcol )
					noiseinc = -1;
				else
					noiseinc = 0;
			}
			else
			{
				fcol = fcol - 0.5f;			// 0 at middle of color band
											// 0.5 at right edge of color band
				if( frand > fcol )
					noiseinc = 0;
				else
					noiseinc = 1;
			}

			dithered_val = val + noiseinc;
			if( dithered_val < 0 )
			{
				if( bDitherLowerToBlueAndAlpha )
				{
					// Dither bottom into blue and alpha
					// Alpha will become negative green increment

					color = D3DCOLOR_ARGB( 1, 
											(BYTE)( upper * color_mask.x ),
											(BYTE)( upper * color_mask.y ),
											(BYTE)( upper * color_mask.z ) );
				}
				else
				{
					color = 0x00;
				}
			}
			else if( dithered_val > upper )
			{
				if( pDither_upper != NULL )
				{
					color = *pDither_upper;
				}
				else
				{
					// color = maximum value
					dithered_val = dithered_val - 1;
					color = D3DCOLOR_ARGB( 0, 
											(BYTE)( dithered_val * color_mask.x),
											(BYTE)( dithered_val * color_mask.y),
											(BYTE)( dithered_val * color_mask.z) );
				}
			}
			else
			{
				// standard dithered value
				color = D3DCOLOR_ARGB( 0, 
										(BYTE)( dithered_val * color_mask.x),
										(BYTE)( dithered_val * color_mask.y),
										(BYTE)( dithered_val * color_mask.z) );
			}

			// If at the top line of the texture, do not dither, because
			//  dither control of (0,0,0,0) accesses the top line
			// This is only so the demo can illustrate the difference between
			//  dither and no dither.  If you always dither, you can remove this
			//  j==0 case.
			if( j == 0 )
			{
				color = D3DCOLOR_ARGB( 0,
										(BYTE)( val * color_mask.x),
										(BYTE)( val * color_mask.y),
										(BYTE)( val * color_mask.z) );
			}


			*((D3DCOLOR*)pixel) = color;
		}	
	}


	hr = pTex->UnlockRect( 0 );
	ASSERT_IF_FAILED( hr );


	// mip mapping the blue bits texture doesn't improve perf
//	pTex->SetAutoGenFilterType( D3DTEXF_POINT );
//	pTex->GenerateMipSubLevels();


	FDebug("Created dithered gradient texture.  Resolution: %d, %d\n", width, height );


	if( bSaveTexture == true )
	{
		char filename[200];
		sprintf( filename, "C:\\PVFDither_%d_%d%s.bmp", width, height, pDither_upper ? "_WrapDither" : "" );

		FMsg("Texture filename [%s]\n", filename );
		
		// D3DXIFF_TGA is not yet supported
		// D3DXIFF_BMP can't be parsed by Photoshop
		//   but if you open in another app and re-save then it is ok.
		// D3DXIFF_PPM is not supported
		// PNG is not supported
		hr = D3DXSaveTextureToFile( filename,
									D3DXIFF_BMP,
									*ppTex,
									NULL	);		// palette 
		ASSERT_IF_FAILED(hr);
	}
}


void	PolygonalVolumeFog::CreateGradient( NV_IDIRECT3DTEXTURE ** ppTex,
										  int width, int height,
										  byte lower, byte upper, 
										  D3DXVECTOR3 color_mask )
{
	// Creates a gradient in X axis of the texture
	// Color mask is from [0,1] and gets multiplied into
	//  the color components

	ASSERT_AND_RET_IF_NULL( ppTex );
	ASSERT_AND_RET_IF_NULL( m_pD3DDev );
	
	FreeTexture( *ppTex );

	HRESULT hr;

#ifdef NV_USING_D3D8
	hr = m_pD3DDev->CreateTexture( width, height,
									1,				// mip levels
									0,				// usage
									D3DFMT_A8R8G8B8,
									D3DPOOL_MANAGED,
									ppTex );
#endif
#ifdef NV_USING_D3D9
	hr = m_pD3DDev->CreateTexture( width, height,
									1,				// mip levels
									0,				// usage
									D3DFMT_A8R8G8B8,
									D3DPOOL_MANAGED,
									ppTex,
									NULL );
#endif

	if( FAILED(hr))
	{
		FDebug("Can't create texture!\n");
		assert( false );
		return;
	}
	ASSERT_AND_RET_IF_NULL( *ppTex );

	m_vppFVTexToFree.push_back( ppTex );		// add texture to list of textures to free


	// Now lock and fill the texture with the gradient
	NV_IDIRECT3DTEXTURE * pTex = *ppTex;

	D3DLOCKED_RECT lr;

	hr = pTex->LockRect( 0, &lr,
						NULL,		// region to lock - NULL locks whole thing
						0 );		// No special lock flags
									// Can't use LOCK_DISCARD unless texture is DYNAMIC
	ASSERT_IF_FAILED(hr);

//	FDebug("Surface pitch (bytes):  %d        pointer: %u\n", lr.Pitch, lr.pBits );

	byte * pixel;
	byte * pBase = (BYTE*) lr.pBits;

	int bytes_per_pixel = 4;

	assert( bytes_per_pixel * width == lr.Pitch );

	byte col;
	int i,j;
	for( j=0; j < height; j++ )
	{
		for( i=0; i < width; i++ )
		{
			pixel = pBase + j * lr.Pitch + i * bytes_per_pixel;

			// calculate gradient values
			col = (byte)( lower + (upper-lower) * (float)i / ((float)width - 1.0f));

			// alpha should be 255 because it is used later in alpha test
			*((D3DCOLOR*)pixel) = D3DCOLOR_ARGB( 255,
												(BYTE)(col * color_mask.x),
												(BYTE)(col * color_mask.y),
												(BYTE)(col * color_mask.z) );
		}	
	}


	hr = pTex->UnlockRect( 0 );
	ASSERT_IF_FAILED( hr );

	FDebug("Created gradient texture!\n");
}


void	PolygonalVolumeFog::SRS_SolidsToZAndColor()
{
	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,			D3DZB_TRUE );
	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,		true );
	m_pD3DDev->SetRenderState( D3DRS_ZFUNC,				D3DCMP_LESSEQUAL );
	m_pD3DDev->SetRenderState( D3DRS_STENCILENABLE,		false );

	m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE,	false );
	m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,			false );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,	false );

	// Front facing tris only
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );


	m_pD3DDev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA |
		                                               D3DCOLORWRITEENABLE_RED |
													   D3DCOLORWRITEENABLE_GREEN | 
													   D3DCOLORWRITEENABLE_BLUE );

	int tcc;
	for( tcc=0; tcc < 3; tcc++ )
	{
		m_pD3DDev->SETTEXTURESAMPLERSTATE( tcc, ADDRESSU,  D3DTADDRESS_WRAP );
		m_pD3DDev->SETTEXTURESAMPLERSTATE( tcc, ADDRESSV,  D3DTADDRESS_WRAP );

		m_pD3DDev->SETTEXTURESAMPLERSTATE( tcc, MAGFILTER, D3DTEXF_POINT );
		m_pD3DDev->SETTEXTURESAMPLERSTATE( tcc, MINFILTER, D3DTEXF_POINT );
		m_pD3DDev->SETTEXTURESAMPLERSTATE( tcc, MIPFILTER, D3DTEXF_NONE  );
	}

}



void PolygonalVolumeFog::RenderObjects( vector< Object_FogVolume ** > * pvObj )
{
	// No batching of primitives is performed

	const D3DXMATRIX * pWVPMat;
	D3DXMATRIX transpose;

	if( pvObj != NULL )
	{
		Object_FogVolume ** ppObj;
		Object_FogVolume *	pObj;

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
			SRSC_MatWorldViewProjectTrans( & transpose );

			if( pObj->ppDitherControl != NULL )
			{
				m_pD3DDev->SetVertexShaderConstantF( CV_DITHER_CONTROL,
													(float*)(*pObj->ppDitherControl), 1 );

//				FMsg("%d dither = %f %f %f\n", i, (*pObj->ppDitherControl)->x,
//													(*pObj->ppDitherControl)->y,
//													(*pObj->ppDitherControl)->z );

			}

			float fDS = pObj->fDepthScale;
			D3DXVECTOR4 vec;
			vec = D3DXVECTOR4(	fDS * m_fScale,					// for red ramps
								fDS * m_fGrnRamp * m_fScale,	// for green ramps
								fDS * m_fBluRamp * m_fScale,
								0.0f );
			m_pD3DDev->SetVertexShaderConstantF( CV_RAMPSCALE, vec, 1 );


			(*pObj->ppGeometry)->Draw();
		}
	}
}



void PolygonalVolumeFog::RenderObjects( vector< Object_ExtrusionVolume ** > * pvObj )
{
	RenderObjects( pvObj, 1.0f );
}



void PolygonalVolumeFog::RenderObjects( vector< Object_ExtrusionVolume ** > * pvObj,
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
			SRSC_MatWorldViewProjectTrans( & transpose );

			// Set extrusion origin in object space
			SRSC_ExtrusionOriginObjSpc( *pObj->ppExtrusionOriginO );
			// Set extrusion distance
			SRSC_ExtrusionDistance( pObj->fExDist * extrusion_scale );
			// Set amount to inset vertices along vertex normal
			SRSC_ExtrusionInset( 0.0f );

			if( pObj->ppDitherControl != NULL )
			{
				m_pD3DDev->SetVertexShaderConstantF( CV_DITHER_CONTROL,
													(float*)(*pObj->ppDitherControl), 1 );
			}

			float fDS = pObj->fDepthScale;
			D3DXVECTOR4 vec;
			vec = D3DXVECTOR4(	fDS * m_fScale,					// for red ramps
								fDS * m_fGrnRamp * m_fScale,	// for green ramps
								fDS * m_fBluRamp * m_fScale,
								0.0f );
			m_pD3DDev->SetVertexShaderConstantF( CV_RAMPSCALE, vec, 1 );


			// Render the geometry
			(*pObj->ppGeometry)->Draw();

		}
	}
}


void PolygonalVolumeFog::RenderObjects( vector< Object_Solid ** > * pvObj )
{
	// No batching of primitives is performed

	const D3DXMATRIX * pWVPMat;
	D3DXMATRIX transpose;

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
			SRSC_MatWorldViewProjectTrans( & transpose );

			// Set other stuff here if you want

			(*pObj->ppGeometry)->Draw();
		}
	}
}

void PolygonalVolumeFog::SRSC_ExtrusionOriginObjSpc( D3DXVECTOR4 * pOrigin )
{
	// Set extrusion origin in object space
	assert( pOrigin != NULL );
	m_pD3DDev->SetVertexShaderConstantF( CV_LIGHT_POS_OSPACE, (float*)pOrigin, 1 );
}

void PolygonalVolumeFog::SRSC_ExtrusionDistance( float ex_dist )
{
	// Set extrusion distance
	m_pD3DDev->SetVertexShaderConstantF( CV_EXTRUSION_DIST,
					D3DXVECTOR4( ex_dist, ex_dist, ex_dist, 0.0f ), 1 );
}

void PolygonalVolumeFog::SRSC_ExtrusionInset( float ex_inset )
{
	// Set amount to inset vertices along vertex normal
	m_pD3DDev->SetVertexShaderConstantF( CV_FATNESS_SCALE,
					D3DXVECTOR4( ex_inset, ex_inset, ex_inset, 1.0f ), 1 );
}




void PolygonalVolumeFog::RenderP13_ConvertDifferenceToFogColor( NV_IDIRECT3DTEXTURE * pFogColorRampTex )
{

	// pFogColorRampTex	=	pointer to ramp texture which maps object thickness
	//						to a fog color

	RET_IF_NULL( pFogColorRampTex );

	// Convert RGB color encoded depth value to a single depth/fog
	//   thickness value.

	SRS_DepthDifferenceAndBlend();


	GetShaderManager()->SetShader( m_PSHI_RGBDifferenceToFogColor_13 );

	// Render to ordinary backbuffer, blending fog on top of the scene
	// No depth buffer is used.
	SetRenderTargetsToDefault( true, NULL );		// NULL for no depth buffer


//@@@ uses texm3x2 for now on ps.1.3 hardware
	m_pD3DDev->SetTexture( 0, GetTexture( m_nDepthDifference ) );	// RGB bias-scaled depth
	m_pD3DDev->SetTexture( 1, NULL );
	m_pD3DDev->SetTexture( 2, pFogColorRampTex );


	// Set constant that will be copied into texture coordinates at stage
	// 1 to decode RGB-encoded thickness to a floating point texture coordinate.
	// w component of m_vTexAddrWeights doesn't matter with these shaders

	m_pD3DDev->SetVertexShaderConstantF( CV_RGB_TEXADDR_WEIGHTS, m_vTexAddrWeights, 1 );

	
	// m_VSHI_RGBDifferencetoFogColor_11 is a variant on TextureDisplay vertex shader
	GetShaderManager()->SetShader( m_VSHI_RGBDifferencetoFogColor_11 );


	// Use TextureDisplay class to render a quad over the full 
	//  screen, executing the pixel shader at each pixel.

	// false to use our own vshader
	m_pTextureDisplay->RenderAlignedQuad( &m_RectFullCoverage, false );

}


void PolygonalVolumeFog::RenderP13_SubtractFrontDepthsFromBackDepths()
{

	// Subtract front facing depth sum from back facing depth sum, 
	//  writing the result to m_nDepthDifference texture render target.
	//  This texture will then be converted to the fog color by another 
	//  function
	// Disable Z buffer


	SetRenderTargets( m_nDepthDifference, -1 );		// -1 sets depth to NULL

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,	D3DZB_FALSE );
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );

	m_pD3DDev->SetRenderState( D3DRS_STENCILENABLE, false );
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );


	m_pD3DDev->SetTexture( 0, GetTexture( m_nFrontfaceDepths ) );	// front faces depth sum
	m_pD3DDev->SetTexture( 1, GetTexture( m_nBackfaceDepths ) );	// back faces depth sum


	GetShaderManager()->SetShader( m_PSHI_SubtractRGBEncodedDepth_13 );


	// Use TextureDisplay class to render a quad over the full 
	//  screen, executing the pixel shader at each pixel.
	// Render a quad over the entire render target, computing the 
	//  depth difference in the pixel shader at each pixel.
	// This function sets its own vshader state
	// Texture coordinates are the default (0,0) to (1,1) values

	m_pTextureDisplay->RenderAlignedQuad( & m_RectFullCoverage );

}


void PolygonalVolumeFog::SRS_DepthDifferenceAndBlend()
{

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,			D3DZB_FALSE );
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
	m_pD3DDev->SetRenderState( D3DRS_STENCILENABLE,		false );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,	true );

	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,	m_dwFogToScreenSrcBlend );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, m_dwFogToScreenDestBlend );
	m_pD3DDev->SetRenderState( D3DRS_BLENDOP,   m_dwFogToScreenBlendOp );

	// Point sample back face texture
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 0, MINFILTER, D3DTEXF_POINT );
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 0, MAGFILTER, D3DTEXF_POINT );
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 0, MIPFILTER, D3DTEXF_NONE );

	// Point saple front face texture
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 1, MINFILTER, D3DTEXF_POINT );
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 1, MAGFILTER, D3DTEXF_POINT );
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 1, MIPFILTER, D3DTEXF_NONE );

	// bilinear sample fog color ramp texture
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 2, MINFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 2, MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 2, MIPFILTER, D3DTEXF_NONE );

	// Clamp so brightest fog doesn't wrap back to zero
    m_pD3DDev->SETTEXTURESAMPLERSTATE( 2, ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pD3DDev->SETTEXTURESAMPLERSTATE( 2, ADDRESSV,  D3DTADDRESS_CLAMP );


}


void PolygonalVolumeFog::RenderP20_FogSubtractConvertAndBlend( int nBack,
																int nFront,
													NV_IDIRECT3DTEXTURE * pRampTex )
{

	// Use this single pass difference and decode step with Pixel Shaders 2.0
	//  in place of the two pass approach using RenderP13_SubtractFrontDepthsFromBackDepths
	//  and RenderP13_ConvertDifferenceToFogColor
	// Also blends fog color to scene rather than rendering it to texture

	//  nBack			=	index into RTTTexture array for source of back depth info
	//  nFront			=	index into RTTTexture array for source of front depth info
	//	pRampTex		=	pointer to ramp texture which encodes fog color with respect
	//						to thickness.

//@@@@ PUT IN OPTION to render only to lower right corner of screen if fog display is on.

	RET_IF_NULL( pRampTex );

	////////////////////////////////////////////////////////////////////
	// Subtract front facing values from back facing values in pixel shader
	// Disable Z

	SRS_DepthDifferenceAndBlend();

	// Render to the ordinary backbuffer, no depth buffer
	SetRenderTargetsToDefault( true, NULL );

	m_pD3DDev->SetTexture( 0, GetTexture( nBack ) );	// back faces; positive depths
	m_pD3DDev->SetTexture( 1, GetTexture( nFront ) );	// front faces; negative depths
	m_pD3DDev->SetTexture( 2, pRampTex );


	m_pD3DDev->SetPixelShaderConstantF( CP_RGB_TEXADDR_WEIGHTS, m_vTexAddrWeights, 1 );


	// Does not require custom vertex shader for TextureDisplay like the 
	//  vs/ps 1.1 - 1.3 versions do.  Texture address is calculated from
	//  a pixel shader constant rather than a constant texture coordinate.
	
	GetShaderManager()->SetShader( m_PSHI_SubtractRGBAndGetFogColor_20 );


	// Use TextureDisplay class to render a quad over the full 
	//  screen, executing the pixel shader at each pixel.

	// This function sets its own vshader state
	m_pTextureDisplay->RenderAlignedQuad( & m_RectFullCoverage );

}





HRESULT PolygonalVolumeFog::RenderFogToScene( NV_IDIRECT3DTEXTURE ** ppTexColorRamp )
{
	// Renders fog objects to thickness calculation textures
	//  and blends fog color to the default color back buffer

	HRESULT hr = S_OK;

	m_ppTexFogThicknessToColor = ppTexColorRamp;
	FAIL_IF_NULL( m_ppTexFogThicknessToColor );
	FAIL_IF_NULL( *m_ppTexFogThicknessToColor );
	FAIL_IF_NULL( m_pD3DDev );

	////////////////////////////////////////////////

	Tick_SetStuffBeforeRendering();

	// Render solid object depth texture
	// This encodes depth to nearest solid object at each pixel
	//  as an RGB color

	RenderP__SolidObjectsRGBDepth();


	// Render front and back face depth acumulation textures
	// This sums the front face depths to one texture, and sums the
	//  backface depths to another texture.  The previously created
	//  solid object depth texture is sampled to handle solid objects
	//  intersecting the fog volumes.

	RenderP__FogVolumeDepthsWithDepthCompare();


	// Rendering to framebuffer
	// Ordinary solid objects in the scene should already be rendered
	// Subtract front from back and convert the difference to fog color

	if( m_bUsePS20 )
	{
		RenderP20_FogSubtractConvertAndBlend( m_nBackfaceDepths,
												m_nFrontfaceDepths,
												* m_ppTexFogThicknessToColor );
	}
	else
	{
		// ps.1.3 path requires one more pass than the ps_2_0 path

		RenderP13_SubtractFrontDepthsFromBackDepths();

		RenderP13_ConvertDifferenceToFogColor( * m_ppTexFogThicknessToColor );

	}



	// RenderPostFogDisplay() renders wireframe view of fog objects
	//  or displays intermediate textures to screen.

	RenderPostFogDisplay();


	return( hr );
}




HRESULT PolygonalVolumeFog::RenderFogToSceneNoIntersectors( NV_IDIRECT3DTEXTURE ** ppTexColorRamp )
{
	// Simple version of fog rendering which does not handle solid objects
	//  intersecting the fog volume.
	// This reduces the number of passes required, but is very restrictive

	FMsg("RenderFogToSceneNoIntersectors not yet updated to new structure\n");
	assert( false );
	return( S_OK );

	// function not yet in place

	PVFTRACE0("RenderFogToSceneNoIntersectors():\n");
	HRESULT hr = S_OK;	
	m_ppTexFogThicknessToColor = ppTexColorRamp;
	FAIL_IF_NULL( m_ppTexFogThicknessToColor );
	FAIL_IF_NULL( *m_ppTexFogThicknessToColor );
	FAIL_IF_NULL( m_pD3DDev );


	return( hr );
}



void PolygonalVolumeFog::RenderPostFogDisplay()
{

	// Render the fog calculation textures to screen

	if( m_bDisplayFogCalcTextures )
	{
		int i0, i1, i2, i3;
		i0 = ( m_nPermute + 0 ) % ( GetNumSurfaces() );
		i1 = ( m_nPermute + 1 ) % ( GetNumSurfaces() );
		i2 = ( m_nPermute + 2 ) % ( GetNumSurfaces() );
		i3 = ( m_nPermute + 3 ) % ( GetNumSurfaces() );

		Display4Textures( i0, i1, i2, i3,		// tex to display
			              m_nFrontfaceDepths,	// remaining args are the 
												// textures to optionally brighten
						  m_nSolidDepths,
						  m_nBackfaceDepths,
						  -1	);
	}
}


void PolygonalVolumeFog::RenderP__SolidObjectsRGBDepth()
{
	// Render depth of solid objects to a render target texture
	// This texture is used to handle objects intersecting the 
	// fog volumes.
	// Depth is encoded as RGB color.

	// The resulting texture will contain
	//  the closest front faces of the solid objects
	// Z buffer depth is not used later, but Z buffering is
	//  needed to render front-most faces.


	// Even if user display is wireframe, render things as solid
	m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );


	// Disables stencil
	// Disables alpha blend
	// Sets point filtering for RGB depth encoding
	SRS_SolidsToZAndColor();

	m_pD3DDev->SetTexture( 0, m_pTexRedGreenDepthRamp );
	m_pD3DDev->SetTexture( 1, m_pTexBlueDepthRamp );
	m_pD3DDev->SetTexture( 2, NULL );
	m_pD3DDev->SetTexture( 3, NULL );

	SetRenderTargets( m_nSolidDepths, 0 );		// 0 for the matched RTT depth buffer


	// Clear front solid object depth color texture and shared depth buffer
	m_pD3DDev->Clear( 0, NULL,
						D3DCLEAR_TARGET | GetDepthClearFlags(),
						m_dwRGBSolidsClearColor, 1.0, 0 );


	GetShaderManager()->SetShader( m_VSHI_DepthToTexcrdForRGB );
	GetShaderManager()->SetShader( m_PSHI_DepthToRGBEncode );

	m_pD3DDev->SetPixelShaderConstantF( CP_RGB_TEXADDR_WEIGHTS, m_vTexAddrWeights, 1 );


	RenderObjects( & (m_pFogSceneObjects->m_vSolidsAffectingFog ) );

	// Render shadow volume hole source objects as solids 
	// This renders them without extruding them into a shadow volume,
	//   so they are rendered as the solid objects from which the 
	//   shadow volumes originate.

	GetShaderManager()->SetShader( m_VSHI_DepthToTexcrd_ObjectExtruder );
	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionHoles ), 0.0f );

}



void PolygonalVolumeFog::RenderP__FogVolumeDepthsWithDepthCompare()
{
	// Uses RGB-encoded depth comparison to handle solid objects
	//  intersecting the fog volumes.
	// Before calling this, make sure RenderP__SolidsRGBDepth()
	//  has rendered the nearest solid object depths.

	RET_IF_NULL( m_pFogSceneObjects );


	// Render front faces of fog volumes to front face depth acumulation texture
	// Disable Z buffering

	const int proj_tex_stage = 3;

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,		false );
	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,	false );
	m_pD3DDev->SetRenderState( D3DRS_ZFUNC,			D3DCMP_ALWAYS );

	// Additive blending
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_BLENDOP,		D3DBLENDOP_ADD );


	SetRenderTargets( m_nFrontfaceDepths, -1 );		// -1 for no depth buffer

	m_pD3DDev->Clear( 0, NULL,
						D3DCLEAR_TARGET,
						m_dwRGBDepthClearColor, 1.0, 0 );


	if( m_bUsePS20 )
	{
		// No need to set D3DTTFF_PROJECTED for stage 3 as the texldp instruction is
		//  used in the ps_2_0 pixel shader

		// Effect of the depth comparison is to always choose the closer
		//  depth value between the fog volume face depth and the solid
		//  object rendered depth.

		GetShaderManager()->SetShader( m_PSHI_DepthToRGBAndCompare_20 );
	}
	else
	{
		GetShaderManager()->SetShader( m_PSHI_DepthToRGBAndCompare_13 );

		// Use projective texture coord in stage three to defeat perspective-
		//  correct texture interpolation and map the screen-aligned solid object
		//  depth texture to exactly cover the full screen regardless of the shape
		//  of the geometry being rendered.
		// This way, as pixel of each object is rendered, the pixel shader can compare
		//  it's depth to the depth of the nearest solid object and always output the
		//  lesser depth value.

		// Set to divide by w
		m_pD3DDev->SetTextureStageState( proj_tex_stage,
											D3DTSS_TEXTURETRANSFORMFLAGS,
											D3DTTFF_PROJECTED | D3DTTFF_COUNT4 );
	}


	m_pD3DDev->SetTexture( 0, m_pTexRedGreenDepthRamp );
	m_pD3DDev->SetTexture( 1, m_pTexBlueDepthRamp );
	// Set texture 3 to previously rendered solid object depth texture
	m_pD3DDev->SetTexture( proj_tex_stage, GetTexture( m_nSolidDepths ) );


	// Sets up tex coord t3 to project texture 1:1 to screen
	GetShaderManager()->SetShader( m_VSHI_DepthToTexcrd_ObjectExtruder );



	// Cull mode to CCW to render front faces only
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE,	D3DCULL_CCW );
	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionVolumes) );

	// Render back faces of negative fog volume objects (holes)
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionHoles ) );

	// Render back faces of negative fog volume objects, non-extruded
	GetShaderManager()->SetShader( m_VSHI_DepthToTexcrdForRGB_TC4Proj );
	RenderObjects( & (m_pFogSceneObjects->m_vFogVolumeHoles ) );

	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	RenderObjects( & (m_pFogSceneObjects->m_vFogVolumes) );

	/////////////////////////////////////////////////////////
	// Render back faces of fog volumes to back-face depth acumulation texture
	// 
	// Fog volume holes are rendered with opposite culling from the
	//  back faces.  The front faces of fog volume holes should be
	//  rendered to this texture, so that the backfaces of the holes,
	//  (rendered to the other texture) can be subtracted from this
	//  to decrease the fog volume thickness.


	SetRenderTargets( m_nBackfaceDepths, -1 );		// -1 for no depth buffer

	m_pD3DDev->Clear( 0, NULL,
						D3DCLEAR_TARGET,
						m_dwRGBDepthClearColor, 1.0, 0 );


	// Render front faces of negative fog volume objects (holes).

	// The following are already set from the previous rendering:
	//	m_pShaderManager->SetShader( m_VSHI_DepthToTexcrdForRGB );
	//	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	RenderObjects( & (m_pFogSceneObjects->m_vFogVolumeHoles ) );

	// Render back faces of positive fog volume objects
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	RenderObjects( & (m_pFogSceneObjects->m_vFogVolumes ) );

	GetShaderManager()->SetShader( m_VSHI_DepthToTexcrd_ObjectExtruder );

	// Render back faces of positive objects
	//	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionVolumes) );

	// Render front faces of negative fog volume objects (holes)
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionHoles ) );

}




void PolygonalVolumeFog::Render_FogObjectsWireframe( DWORD color, bool z_enable )
{
	// Selects TFACTOR as wireframe color

	RET_IF_NULL( m_pFogSceneObjects );

	m_pD3DDev->SetPixelShader( 0 );
	GetShaderManager()->SetShader( m_VSHI_DepthToTexcrdForRGB );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, color );	// ARGB
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );

	if( m_nWireframe == 2 )
	{
		m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	}
	else
	{
		m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	}

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,  z_enable ? D3DZB_TRUE : D3DZB_FALSE );
	m_pD3DDev->SetRenderState( D3DRS_ZFUNC,  z_enable ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS );

	m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	RenderObjects( & (m_pFogSceneObjects->m_vFogVolumes ));

	// WIREFRAME display
	// Draw extrusion objects once without extrusion shader
	// Then draw them again with the extrusion shader

	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionVolumes) );

	// Negative volume objects in green
	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0x0000FF00 );	// ARGB


	RenderObjects( & (m_pFogSceneObjects->m_vFogVolumeHoles ) );
	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionHoles ) );

	GetShaderManager()->SetShader( m_VSHI_DepthToTexcrd_ObjectExtruder );

	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionHoles ) );

	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, color );

	RenderObjects( & (m_pFogSceneObjects->m_vFogExtrusionVolumes) );
}



void PolygonalVolumeFog::SRS_RenderFogToScene( DWORD color_ARGB )
{
	SetRenderTargetsToDefault();


	m_pTextureDisplay->SetRenderState();

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	// alpha blend mode is up to specific Render_ function

	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, color_ARGB );	// ARGB
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );

	m_pD3DDev->SETTEXTURESAMPLERSTATE( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SETTEXTURESAMPLERSTATE( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE  );


	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pD3DDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );	
}





HRESULT	PolygonalVolumeFog::Tick_SetStuffBeforeRendering()
{
	FAIL_IF_NULL( m_pFogSceneObjects );

	HRESULT hr = S_OK;

	m_dwFogToScreenDestBlend = m_pFogSceneObjects->m_dwFogToScreenDestBlend;
	m_dwFogToScreenSrcBlend = m_pFogSceneObjects->m_dwFogToScreenSrcBlend;
	m_dwFogToScreenBlendOp	= m_pFogSceneObjects->m_dwFogToScreenBlendOp;



	m_pD3DDev->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,	true );
	m_pD3DDev->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,		D3DZB_TRUE );


		// note how D3DXVECTOR4 casts itself to a *
	m_pD3DDev->SetVertexShaderConstantF( CV_CONSTS_1,	D3DXVECTOR4( 0.0f, 0.5f, 1.0f, 2.0f ), 1 );

	

	D3DXVECTOR4 vec( m_fBitReduce, 0.0f, 0.0f, 0.0f );
	m_pD3DDev->SetVertexShaderConstantF( CV_BITREDUCE, vec, 1 );

	vec = D3DXVECTOR4( 1.0f * m_fScale, m_fGrnRamp * m_fScale, m_fBluRamp * m_fScale, 0.0f );
	m_pD3DDev->SetVertexShaderConstantF( CV_RAMPSCALE, vec, 1 );


	vec = D3DXVECTOR4( m_fNDotLExtrudeThresh, 0.0f, 0.0f, 0.0f );
	m_pD3DDev->SetVertexShaderConstantF( CV_NDOTLTHRESH, vec, 1 );


	// Set directional light vector in object space	
	vec = D3DXVECTOR4( 0.125f, 0.25f, -1.0f, 1.0f );
	D3DXVec4Normalize( &vec, &vec );
	m_pD3DDev->SetVertexShaderConstantF( CV_DIRLIGHT1_DIR, vec, 1 );



	float half_h = 0.50f * 1.0f / ((float)m_nRTTHeight);
	float half_w = 0.50f * 1.0f / ((float)m_nRTTWidth);

	vec = D3DXVECTOR4( half_w, half_h, 0.0f, 0.0f );
	m_pD3DDev->SetVertexShaderConstantF( CV_HALF_TEXEL_SIZE, vec, 1 );



	if( m_nWireframe )
	{
		m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	}
	else
	{
		m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	}



	return( hr );
}



HRESULT PolygonalVolumeFog::Display4Textures( int i0, int i1, int i2, int i3,
											int b0, int b1, int b2, int b3  )
{
	// Displays 4 rendered texture results to screen

	// i0..i3 are indices into m_pRTTTexture[]
	// b0..b3 are indices of textures to brighten by rendering many times
	//   additively to the framebuffer.
	// Use b0..b3 = -1 to avoid brightening.
	// i0	is rendered to upper left
	// i1	is rendered to upper right
	// i2	is rendered to lower left
	// i3   is rendered to lower right
	//
	// b0 need not correspond to i0, meaning if you call:
	//         Display4Textures( 0,1,2,3, -1, 0, 3, -1 )
	//  then textures 0,1,2,3 will be displayed and textures 0 and 3 will
	//  be brightened.

	
	HRESULT hr = S_OK;
	FAIL_IF_NULL( m_pD3DDev );
		
	TextureDisplay * pTD = m_pTextureDisplay;
	FAIL_IF_NULL( pTD );


	SetRenderTargetsToDefault();


	// disable Z
	m_pD3DDev->SetRenderState( D3DRS_ZENABLE, false );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );


	pTD->SetTexture( 0, GetTextureHandle( i0 ) );
	pTD->SetTextureRect( 0, 0.0f, 0.5f, 0.5f, 0.0f );	// upper left

	pTD->SetTexture( 1, GetTextureHandle( i1 ) );
	pTD->SetTextureRect( 1, 0.5f, 1.0f, 0.5f, 0.0f );	// upper right

	pTD->SetTexture( 2, GetTextureHandle( i2 ) );
	pTD->SetTextureRect( 2, 0.0f, 0.5f, 1.0f, 0.5f );	// low left

	pTD->SetTexture( 3, GetTextureHandle( i3 ) ) ;
	pTD->SetTextureRect( 3, 0.5f, 1.0f, 1.0f, 0.5f );	// low right


	int i;
	for( i=0; i < 4; i++ )
	{
#ifdef NV_USING_D3D8
		m_pD3DDev->SETTEXTURESAMPLERSTATE( i, D3DTSS_MAGFILTER, D3DTEXF_POINT );
		m_pD3DDev->SETTEXTURESAMPLERSTATE( i, D3DTSS_MINFILTER, D3DTEXF_POINT );
		
	    m_pD3DDev->SETTEXTURESAMPLERSTATE( i, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
	    m_pD3DDev->SETTEXTURESAMPLERSTATE( i, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
#endif
#ifdef NV_USING_D3D9
		m_pD3DDev->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		m_pD3DDev->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		
	    m_pD3DDev->SetSamplerState( i, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
	    m_pD3DDev->SetSamplerState( i, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
#endif
	}


	pTD->RenderTexture( 0, true );
	pTD->RenderTexture( 1, true );
	pTD->RenderTexture( 2, true );
	pTD->RenderTexture( 3, true );


	// Now render additively to increase values displayed

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	m_pD3DDev->SetRenderState( D3DRS_ZENABLE, false );

	// Additive blending
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,	D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_BLENDOP,	D3DBLENDOP_ADD );

	int n;
	int in[4];
	in[0] = i0;
	in[1] = i1;
	in[2] = i2;
	in[3] = i3;

	// Brighten display if indices among the bN numbers

	for( n=0; n < 4; n++ )
	{
		if( in[n] == b0 || in[n] == b1 ||
			in[n] == b2 || in[n] == b3 )
		{
			for( i=0; i < m_nNumAdditivePasses; i++ )
			{
				pTD->RenderTexture( n, false );
			}
		}
	}

	return( hr );	
}


void	PolygonalVolumeFog::SRSC_MatWorldViewProjectTrans( D3DXMATRIX * pWVP_Transpose )
{
	assert( m_pD3DDev != NULL );
	assert( pWVP_Transpose != NULL );

	m_pD3DDev->SetVertexShaderConstantF( CV_WORLDVIEWPROJ_0, (float*) pWVP_Transpose, 4 );

}



void	PolygonalVolumeFog::SetParameters( MaxFogDepthComplexity max_depth_complexity,
										   float fThicknessToColorScale,
										   float fNearClip,
										   float fFarClip	)
{

	// Causes texture create & destroy, so re-design the way the 
	//  gradient textures are stored if you need to change
	//  dvpc often.

	// make sure fNearClip < fFarClip;
	if( fNearClip >= fFarClip )
	{
		if( fNearClip == fFarClip )
		{
			FMsg("PolygonalVolumeFog::SetParameters far clip cannot equal near clip!\n");
			assert( false );
			return;
		}

		float tmp = fNearClip;
		fNearClip = fFarClip;
		fFarClip = tmp;
	}


	m_fNearClip = fNearClip;
	m_fFarClip = fFarClip;

	m_MaxDepthComplexity = max_depth_complexity;
	float base_scale = 10.0f;
	float diff = 2.0f;

	switch( m_MaxDepthComplexity )
	{
	case PVF_1_SURFACES_24_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_256;
		m_fTexCrdPrecisionFactor = base_scale / (diff*diff*diff*diff);
		break;
	case PVF_2_SURFACES_21_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_128;
		m_fTexCrdPrecisionFactor = base_scale / (diff*diff*diff);
		break;
	case PVF_4_SURFACES_18_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_64;
		m_fTexCrdPrecisionFactor = base_scale / (diff*diff);
		break;
	case PVF_8_SURFACES_15_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_32;
		m_fTexCrdPrecisionFactor = base_scale / diff;
		break;
	case PVF_16_SURFACES_12_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_16;
		m_fTexCrdPrecisionFactor = base_scale;
		break;
	case PVF_32_SURFACES_9_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_8;
		m_fTexCrdPrecisionFactor = base_scale * diff;
		break;
	case PVF_64_SURFACES_6_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_4;
		m_fTexCrdPrecisionFactor = base_scale * diff * diff;
		break;
	case PVF_128_SURFACES_3_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_2;
		m_fTexCrdPrecisionFactor = base_scale * diff * diff * diff;
		break;

	default :
		FMsg("Unrecognized MaxFogDepthComplexity\n");
		assert( false );
		return;
		break;
	}



	float little_bands;

	switch( m_DepthValuesPerColorChannel )
	{
	case PVF_256 :
		m_fBitReduce = 1.0f;
		little_bands = 256.0f;
		break;
	case PVF_128 :
		m_fBitReduce = 0.5f;
		little_bands = 128.0f;
		break;
	case PVF_64 :
		m_fBitReduce = 0.25f;
		little_bands = 64.0f;
		break;
	case PVF_32 :
		m_fBitReduce = 0.125f;
		little_bands = 32.0f;
		break;
	case PVF_16 :
		m_fBitReduce = 0.0625f;
		little_bands = 16.0f;
		break;
	case PVF_8 :
		m_fBitReduce = 0.03125f;
		little_bands = 8.0f;
		break;
	case PVF_4 :
		m_fBitReduce = 0.015625;
		little_bands = 4.0f;
		break;
	case PVF_2 :
		m_fBitReduce = 0.0078125;
		little_bands = 2.0f;
		break;

	default:
		FDebug("unknown # depth values per channel -- using 16 as default\n");
		m_fBitReduce = 0.0625f;
		little_bands = 16.0f;
		assert( false );
	}



	m_fGrnRamp = m_fBitReduce * 256.0f;		// how many times green cycles for 1 red cycle
	m_fBluRamp = m_fGrnRamp * m_fGrnRamp;

	m_fGrnRamp = little_bands;
	m_fBluRamp = little_bands * little_bands;


	//@@ this may not be exactly right - just a quick guess
	m_fScale = 1.0f / (m_fFarClip - m_fNearClip);


	CreateTextures();

	SetThicknessToColorTexCoordScale( fThicknessToColorScale );

}



HRESULT	PolygonalVolumeFog::CreateTextures()
{
	HRESULT hr = S_OK;

	int size_divide = (int)( 1.0f / m_fBitReduce );

	// Create texture gradients used to encode depth as an RGB color.
	// Blue is a single gradient in a single texture.
	// Red and green gradients are held in one 2D texture with 
	//   each gradient going in one axis (green in X, red in Y )
	//
	// This class will generate it's own gradient textures, and
	//  the blue texture, which encodes the least significant bits
	//  of depth, are dithered to eliminate aliasing artifacts
	// There is a vertex shader 'dither control' variable to control
	//  the texture coordinates fetching from the dithered texture.


	// 2D gradient in red and green
	// No dithering
	CreateGradient2D( & m_pTexRedGreenDepthRamp,
					256 / size_divide, 256 / size_divide,
					0, (byte)( 255.0f * m_fBitReduce ),
					D3DXVECTOR3( 1.0f, 0.0f, 0.0f ),	// color mask u axis
					D3DXVECTOR3( 0.0f, 1.0f, 0.0f )  );	// color mask v axis RGB


	D3DCOLOR upper_color;
	upper_color = D3DCOLOR_ARGB( 0, 0, 1, 0 );	 //  1/255 of green

	// extra width for dithered values
	int width = 8 * 256 / size_divide;

	if( width > 1024 )
		width = 1024;

	// 64 in y for dithered values.

	CreateGradientDithered( & m_pTexBlueDepthRamp,
								width, 64,
								0, (byte)( 255.0f * m_fBitReduce ),
								D3DXVECTOR3( 0.0f, 0.0f, 1.0f ),	// blue
								& upper_color );


	return( hr );
}


void	PolygonalVolumeFog::FreeTexture( NV_IDIRECT3DTEXTURE * pTex )
{

	// If texture is in the list of textures to free,
	//  remove it.
	int i;
	int found = 0;

	for( i=0; i < m_vppFVTexToFree.size(); i++ )
	{
		NV_IDIRECT3DTEXTURE ** ppFVTex;
		ppFVTex = m_vppFVTexToFree.at(i);		
		if( ppFVTex != NULL )
		{
			if( *ppFVTex == pTex )
			{
				found++;
				SAFE_RELEASE( pTex );
				m_vppFVTexToFree.erase( m_vppFVTexToFree.begin() + i );
			}
		}
	}

	if( found == 1 )
	{
	}
	else if( found > 1 )
	{
		FDebug("Texture pointer included more than once! %x\n", pTex );		
	}
	else if( found == 0 && pTex != NULL )
	{
		FDebug("Texture pointer not found in list.  Freeing anyway! %x\n", pTex );
		SAFE_RELEASE( pTex );
	}
}



void	PolygonalVolumeFog::SetThicknessToColorTexCoordScale( float fTexCrdScale )
{
	m_fTexCrdScale = fTexCrdScale;

	// Set weights for converting RGB encoded depth to scalar
	//  depth value (texture coordinate) used to access a simple
	//  grayscale ramp texture


	// alpha is weighted as negative green, so dithering lowest
	//  low bit values can wrap down to the next lower green value

	m_vTexAddrWeights = D3DXVECTOR4(	1.0f, 
										1.0f / m_fGrnRamp,
										1.0f / m_fBluRamp,
										-1.0f / m_fGrnRamp );

	m_vTexAddrWeights.x *= m_fTexCrdScale * m_fTexCrdPrecisionFactor;
	m_vTexAddrWeights.y *= m_fTexCrdScale * m_fTexCrdPrecisionFactor;
	m_vTexAddrWeights.z *= m_fTexCrdScale * m_fTexCrdPrecisionFactor;
	m_vTexAddrWeights.w *= m_fTexCrdScale * m_fTexCrdPrecisionFactor;

	// If not using PS20, thickness calc will be 2x the result as
	//  with PS20, so divide the scale in half.
	// This is because D3D forces us to use _bx2 modifier as opposed
	//  to the _bias modifier.  
	if( !m_bUsePS20 )
	{
		m_vTexAddrWeights.x *= 0.5f;
		m_vTexAddrWeights.y *= 0.5f;
		m_vTexAddrWeights.z *= 0.5f;
		m_vTexAddrWeights.w *= 0.5f;
	}

}



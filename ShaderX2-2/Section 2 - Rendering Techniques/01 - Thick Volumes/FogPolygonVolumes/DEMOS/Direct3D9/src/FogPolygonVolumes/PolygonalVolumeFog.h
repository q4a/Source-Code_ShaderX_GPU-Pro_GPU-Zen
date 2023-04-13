/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\FogPolygonVolumes
File:  PolygonalVolumeFog.h

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

A class which takes a collection of polygonal objects and renders them as
volumes of fog.  The objects must be closed hulls.

The rendering supports ordinary solid polygonal models as well as extrudable
polygon models.  The extrudable models are the ShadowVolumeObject class 
(vertices in system memory) and the ShadowVolumeVBObject class (vertices in 
a hardware-friendly vertex buffer).  These models can be extruded in a 
vertex shader program based on some location in the scene such as the
position of a light.  Extruding objects this way allows them to form a 
volume of light or shadow so that objects can cast volumetric shadows
through the fog or haze.


The class computes fog thickness at each pixel of a texture render target.
The thickness is then converted into a fog color by converting thickness
to a texture coordinate and using that coordinate to access a 1D color ramp.
This way the 'look' of the fog can be easily changed by an artist.

Various controls exist to modify how fog thickness scales to the texture
coordinate, and how the depth of each object's faces is converted to 
intermediate values used in the computations.

The fog thickness is computed using rendering and color components, so
to achieve more than 8-bits of depth precision, the depth is encoded
as an RGB color value.  Each color channel, R, G, or B, contributes some
number of bits to the depth value, and the remainder of the bits are
reserved to hold overflow when several RGB-encoded values are added
together.  The more bits used for overflow, the fewer are available
to hold depth values, so there is a tradeoff between the number of 
RGB-encoded values that can be added together before error occurs, and
the precision of the depth values.  In other words, there is a trade-off
between the depth precision and the number of surfaces that can be added
together before error may occur.  This tradeoff and the numbers involved
are detailed below in the comments about:
	DepthValuesPerChannel and m_fBitReduce.

The effect was inspired by Microsoft's VolumeFog example in the DX8 and DX9
SDKs, and it makes a number of improvements on MSFT's approach:

 - 3 color channels are used to encode depth, as opposed to MSFT's 2.
	This allows more surfaces to be acumulated at higher depth precision
	before overflow artifacts occur.  It makes for a better tradeoff
	between depth precision and the number of surfaces that can be summed
	before overflow ocurrs.

 - Decoding of RGB thickness to fog color is performed as a floating-point
	texture addressing operation, which is more precise and allows an artist
	to paint a ramp which maps fog thickness to color.

 - Both positive and negative fog volumes are supported.  This allows 
	polygonal models to cut out areas of fog, or cast proper volume shadows
	through the fog.

 - Dithering of the low bits of depth precision is available.  This eliminates
	aliasing artifacts due to low depth precision, making thin fog objects
	appear dithered and noisy rather than severely aliased.  The noisyness can
	be dialed up or down by adjusting how the range of available depth colors
	maps to the scene (ie by moving the near and far clipping planes or changing
	a simple scale factor)

 - The RGB-encoded fog thickness value is rendered to a texture before being
	converted to a fog color.  This means you can perform several thickness-to-color
	conversions based on the thickness texture, rather than having to re-compute
	the thickness many times.  Also, various objects can apply different 
	scale factors or color ramps when decoding a single thickness texture.

 - A new method for a per-pixel rendering decision based on comparisons of
	RGB-encoded depth values.  This simplifies they way that fog occlusion by
	solid objects is handled.  It may eliminate the need for stencil buffer
	tests and additional passes when trying to handle solid objects that clip
	away various fog volume polygons.

 - Several rendering functions and methods are available to handle cases where
	solid objects intersect the fog volumes or obscure the fog.  A faster 
	function is provided for cases where it is guaranteed that no solids will
	intersect fog volumes.


It is possible to use the a stencil buffer test to calculate and render fog
over a small portion of the screen rather than calculating and blending the
fog over the entire screen.  Doing this may improve performance if you have 
a few small fog objects on screen that do not cover many pixels.  Code to 
demonstrate that is not included in this version.

  
-Greg James, NVIDIA


******************************************************************************/

#ifndef __PolygonalVolumeFog_H
#define __PolygonalVolumeFog_H


#include <vector>

#include "NVD3DVers.h"
#include <NV_D3DCommon/ShaderManager.h>			// for easy load/cleanup of shaders
#include <NV_D3DCommon/RenderToTextureBase.h>
#include <NV_D3DCommon/TextureDisplay.h>		// for FRECT

#include "PolygonalVolumeFogObjects.h"


using namespace std;


class PolygonalVolumeFog;
class SimpleVBObject;
class ShadowVolumeVBObject;
class TextureDisplay;


////////////////////////////////////////////////


class PolygonalVolumeFog : public RenderToTextureBase
{
public:
	enum DepthValuesPerChannel
	{
					//  | # of surfaces that can be rendered before artifacts occur
					//  |
					//  |				| depth values from front to back clip planes
		PVF_256,	//  1 surfaces      24-bit
		PVF_128,	//  2 surfaces		21-bit
		PVF_64,		//  4 surfaces		18-bit
		PVF_32,		//  8 surfaces		15-bit = 32768
		PVF_16,		//  16 surfaces		12-bit = 4096
		PVF_8,		//  32 surfaces		9-bit  = 512
		PVF_4,		//  64 surfaces		6-bit  = 64
		PVF_2,		//  128 surfaces	3-bit  = 8
		PVF_UNSET
	};

	// Maximum number of surfaces that can overlap before artifacts
	//  occur.  Front faces are handled separately from back faces, 
	//  so if the max depth complexity is two, that is two front 
	//  surfaces and two back surfaces.
	// The resulting depth precision between near and far clip planes
	//  is listed as the _ BIT_DEPTH
	enum MaxFogDepthComplexity
	{
		PVF_1_SURFACES_24_BIT_DEPTH,
		PVF_2_SURFACES_21_BIT_DEPTH,
		PVF_4_SURFACES_18_BIT_DEPTH,
		PVF_8_SURFACES_15_BIT_DEPTH,
		PVF_16_SURFACES_12_BIT_DEPTH,
		PVF_32_SURFACES_9_BIT_DEPTH,
		PVF_64_SURFACES_6_BIT_DEPTH,
		PVF_128_SURFACES_3_BIT_DEPTH,
		PVF_MAXDEPTHUNSET
	};

public:
	// The main interface functions
	PolygonalVolumeFog();
	~PolygonalVolumeFog();

	HRESULT Initialize( NV_D3DDEVICETYPE * pDev,
							int resolution_x,
							int resolution_y,
							PolygonalVolumeFog::MaxFogDepthComplexity max_depth_complexity,
							float fThicknessToColorScale,
							float fNearClip,
							float fFarClip,
							ShaderManager ** ppShaderManager = NULL );

	void	SetParameters( MaxFogDepthComplexity max_depth_complexity,
							float fThicknessToColorScale,
							float fNearClip, float fFarClip );

	void	SetThicknessToColorTexCoordScale( float fTexCrdScale );
	float	GetThicknessToColorTexCoordScale()	{ return( m_fTexCrdScale ); }; 


			// Main rendering function.  Call this after rendering
			// the ordinary scene.  It renders the fog object thicknesses
			// to offscreen textures, converts thickness to color via the
			// ppTexColorRamp, and blends the fog color to the scene.
	HRESULT RenderFogToScene( NV_IDIRECT3DTEXTURE ** ppTexColorRamp );


			// Doesn't handle solid objects intersecting the fog volume
			// Only fog thickness add and subtract
	HRESULT RenderFogToSceneNoIntersectors( NV_IDIRECT3DTEXTURE ** ppTexColorRamp );


			// Option to display fog calculation textures
	void	RenderPostFogDisplay();

	void	Render_FogObjectsWireframe( DWORD color, bool z_enable );


public:
	// Things you are welcome to change

	// Object collection has vectors of pointers to scene objects. 
	//  Scene objects include solids and fog volumes associated with
	//  matrices and other parameters needed to render them.
	// See the declaration of VolumeFogObjectColletion for a description
	//  of what each object type is.

	VolumeFogObjectCollection	* m_pFogSceneObjects;

		// Blend ops for rendering fog color to the screen
		// May be overridden by object collection settings
	DWORD	m_dwFogToScreenSrcBlend;
	DWORD	m_dwFogToScreenDestBlend;
	DWORD	m_dwFogToScreenBlendOp;

		// Texture that maps fog object thickness to a color
	NV_IDIRECT3DTEXTURE **	m_ppTexFogThicknessToColor;


	int		m_nWireframe;				// wireframe on/off and variation (backface, frontface)

	bool	m_bDisplayFogCalcTextures;	// diagnostic display of intermediate results
	int		m_nNumAdditivePasses;		// used for brightening diagnostic display
	int		m_nPermute;					// changes which textures are displayed in which locations


public:
	// Secondary interface functions.
	// You probably won't have to call these.

	HRESULT Free();
	void	FreeTexture( NV_IDIRECT3DTEXTURE * pTex );

	void	SetAllNull();

	HRESULT		CreateTextures();
	HRESULT		LoadShaders();

	// These functions create textures needed to encode object
	//  depth as an RGB color at each pixel
	void	CreateGradient( NV_IDIRECT3DTEXTURE ** ppTex,
							int width, int height,
							byte lower, byte upper,
							D3DXVECTOR3 color_mask  );	// RGB [0,1]

	void	CreateGradient2D( NV_IDIRECT3DTEXTURE ** ppTex,
							int width, int height,
							byte lower, byte upper,
							D3DXVECTOR3 col_mask_u,
							D3DXVECTOR3 col_mask_v );

	void	CreateGradientDithered( NV_IDIRECT3DTEXTURE ** ppTex,
									int width, int height,
									byte lower, byte upper,
									D3DXVECTOR3 color_mask,
									const D3DCOLOR * dither_upper );

	HRESULT Display4Textures( int i0, int i1, int i2, int i3,
							  int b0, int b1, int b2, int b3  );


	HRESULT	Tick_SetStuffBeforeRendering();

	void RenderObjects( vector< Object_FogVolume ** >		* pvObj );
	void RenderObjects( vector< Object_Solid ** >			* pvObj );
	void RenderObjects( vector< Object_ExtrusionVolume ** > * pvObj );
	void RenderObjects( vector< Object_ExtrusionVolume ** > * pvObj,
						float extrusion_scale );


protected:
	// These are called by the main rendering functions to accomplish various
	//  parts of the fog rendering.

	void	RenderP__FogVolumeDepthsWithDepthCompare();

	void	RenderP__SolidObjectsRGBDepth();

	void	RenderP13_SubtractFrontDepthsFromBackDepths();

	void	RenderP13_ConvertDifferenceToFogColor( NV_IDIRECT3DTEXTURE * pRampTex );

	void	RenderP20_FogSubtractConvertAndBlend( int nBack,
													int nFront,
													NV_IDIRECT3DTEXTURE * pRampTex );

public:  

	// SetRenderState (SRS) calls to set convenient combinations of renderstates
	void	SRS_SolidsToZAndColor();
	void	SRS_RenderFogToScene( DWORD color_ARGB );
	void	SRS_DepthDifferenceAndBlend();

	// Functions to set render state constants -- vertex & pixel shader constants
	void	SRSC_MatWorldViewProjectTrans( D3DXMATRIX * pWVP_Transpose );
	void	SRSC_ExtrusionOriginObjSpc( D3DXVECTOR4 * pOrigin );
	void	SRSC_ExtrusionDistance( float ex_dist );
	void	SRSC_ExtrusionInset( float ex_inset );

	// TextureDisplay is a convenient class to render textured rectangles
	//  to the screen or to a texture render target
	// It is used here to render a quad over the entire render target in 
	//  order to drive the pixel processing.

	TextureDisplay	* m_pTextureDisplay;

	// m_fScale is multiplied by the screen space vertex depth before
	//  the depth is converted to RGB color texture coordinates.  The
	//  near and far clip plane distances affect the value of m_fScale
	//  to map the full RGB depth color range to the full screen depth
	//  range.
	// You can modify m_fScale to increase or decrease the number of
	//  RGB ramps with depth, but increasing m_fScale will cause the
	//  RGB color range to repeat as distance increases.  This will
	//  cause errors if an object spans the depth at which the RGB
	//  color range wraps back around from its maximum to its minimum
	//  value.

	float	m_fScale;

	// Not an important parameter.  This can vary the N-dot-L threshold
	//  at which extrusion takes place.  Was experimental parameter,but
	//  costs nothing extra in the shader, so leaving it in.
	float	m_fNDotLExtrudeThresh;


protected:
	///////////////////////////////////////////////////////////////////
	// m_fBitReduce determines what percentage of each color
	//  channel 8-bit value is used for depth bits as opposed
	//  to "carry" bits.
	// (256 * m_fBitReduce) values are used for depth, and 
	// 256*(1-m_fBitReduce) values are used for carry or overflow values.
	//
	// If m_fBitReduce = 0.25 then values 0 to 63 are used in
	//  the RGB depth ramps to encode depth, and values 64-255 are
	//  open for holding overflow when two or more values in the
	//  range [0,64] are added together.  In this case, only 4 values
	//  could be added and guaranteed to not saturate the color value.
	//  This saturation would produce an error as depths are accumulated.
	//
	// "number of depth values" is the number of depth values that
	//  an RGB color can hold.  It is the depth resolution for a 
	//  particular choice of m_fBitReduce.
	//  The rest of the RGB color values are used for "carry" space
	//  so that several values can be added before saturation and error
	//  occurs.
	// 
	// The "RGB-to-depth conversion coefficients" is the 3D vector
	//  we use in a dot-product operation with the RGB-encoded depth
	//  to convert the RGB-encoded depth back to a single depth value.	
	//  The very small size of some of these coefficients means we
	//  must use floating point calculations to decode the RGB-encoded
	//  depth information.
	// This is using only RGB.  If you also use the alpha channel, RGBA,
	//  you can get more depth precision, but this will not work for
	//  pixel shaders 1.1 - 1.3.

	// m_fBitReduce	| max RGB   | depth | number | num    | RGB-to-depth 
	//	value		| in single | bits  | of     | depths | 
	//				| depth		| per   | depth  | summed | conversion
	//				| value		| RGB	| values | before | coefficients
	//				|			| color	|        | error  | (R,G,B)
	// ------------------------------------------------------------------
	// 1.0			| 255		| 24    | 2^24   | 1      | (1, 1/256, 1/(256*256)) 
	// 0.5			| 127		| 21    | 2^21   | 2      | (1, 1/128, 1/(128*128)) 
	// 0.25			| 63		| 18    | 262144 | 4      | (1, 1/64, 1/(64*64))
	// 0.125		| 31		| 15    | 32768  | 8      | (1, 1/32, 1/1024)
	// 0.0625		| 15		| 12    | 4096   | 16     | (1, 1/16, 1/256)
	// 0.03125		| 7			| 9     | 512    | 32     | (1, 1/8, 1/64)

	float m_fBitReduce;

	// m_fGrnRamp and m_fBluRamp are used to determine the encoding
	// of depth as an RGB color.  They determine the texture coordinate
	// scale for the green and blue color ramps, and are used to control
	// how many green ramps occur per red color increment and how many 
	// blue color ramps occur per green color increment.
	// The values of these depend on the choice of m_fBitReduce

	float	m_fGrnRamp;
	float	m_fBluRamp;
	D3DXVECTOR4	m_vTexAddrWeights;

	// Use two textures for RGB color ramps.  One holds red ramp in U axis
	//  and green ramp in V axis.  Other holds blue ramp in U axis.
	//  We could use a 3D texture instead and save a pixel shader texture
	//  fetch and color instruction, but 3D textures are slower.

	NV_IDIRECT3DTEXTURE *	m_pTexRedGreenDepthRamp;
	NV_IDIRECT3DTEXTURE *	m_pTexBlueDepthRamp;


	// Scale value which affects dot-product texture address operation
	//  conversion of RGB-encoded depth value to a scalar texture coordinate
	//  This can increate or decrease the range of texture coordinates used
	//  to access m_ppTexFogThicknessToColor to convert fog volume thickness
	//  into a fog color.
	float	m_fTexCrdScale;					// value the user can set
	float	m_fTexCrdPrecisionFactor;		// multiplied by user param so look is the
											// same for various choices of depth precision

	float	m_fNearClip;		// Set these via SetParameters(..)
	float	m_fFarClip;

protected:

	bool	m_bUsePS20;		// affects shader loading, texture creation
							// and rendering

	// indexes to rendered textures with depth and thickenss information
	int	m_nSolidDepths;
	int	m_nFrontfaceDepths;
	int	m_nBackfaceDepths;
	int	m_nDepthDifference;			// used only for ps.1.3 path


	// Clear color for RGB depth acumulation is black because surface
	//  depth values are additively blended.
	DWORD	m_dwRGBDepthClearColor;		// = 0x00000000;

	// Color for clearing the solid object depth texture is white
	// because that is the greatest depth value and solid depths are
	// not alpha blended into the texture render target.
	DWORD m_dwRGBSolidsClearColor;		// 	= 0x00FFFFFF;



	DepthValuesPerChannel	m_DepthValuesPerColorChannel;
	MaxFogDepthComplexity	m_MaxDepthComplexity;

	FRECT	m_RectFullCoverage;		// rect coords to cover the full screen

protected:

	// pointers to texture pointers to free on exit
	vector< NV_IDIRECT3DTEXTURE ** >  m_vppFVTexToFree;


	///////////////////////////////////
	// Indices to ShaderManager shaders
	///////////////////////////////////
	// ps.1.3 shader path
	// These are shaders used with DX8 ps.1.3 hardware

	// Replacement shader for TextureDisplay shader.
	SM_SHADER_INDEX		m_VSHI_RGBDifferencetoFogColor_11;

	// Converts RGB difference to texture coordinate and looks up 
	//   the fog color
	SM_SHADER_INDEX		m_PSHI_RGBDifferenceToFogColor_13;

	SM_SHADER_INDEX		m_PSHI_DepthToRGBAndCompare_13;

	SM_SHADER_INDEX		m_PSHI_SubtractRGBEncodedDepth_13;

	////////////////////////////////////////////////////////
	// ps_2_0 shader path
	// These are used with DX9 ps_2_0 hardware

	// Generates RGB-encoded depth value for the pixel rendered, compares
	//  it to an RGB-encoded depth value read from a texture and outputs
	//  the lesser of the two values.
	//  The RGB-encoded depth read from texture is the nearest solid object
	//  depth, and it is used to handle intersection of solids with the 
	//  fog volumes.
	SM_SHADER_INDEX		m_PSHI_DepthToRGBAndCompare_20;

	// Subtracts front depths from back depths, converts the difference
	//  to texture coordinate, and uses the coordinate to look up the
	//  fog color.
	SM_SHADER_INDEX		m_PSHI_SubtractRGBAndGetFogColor_20;

	////////////////////////////////////////////////////////
	// General shaders used on both ps.1.3 and ps_2_0 
	//  hardware

	SM_SHADER_INDEX		m_VSHI_DepthToTexcrd_ObjectExtruder;

	SM_SHADER_INDEX		m_PSHI_DepthToRGBEncode;


	// Calculates texture coordinates used to encode pixel depth
	//  as an RGB value.  A pixel shader uses the coordinates to
	//  sample RGB ramp textures and create an RGB-encoded depth
	//  value for each pixel of the triangle.
	SM_SHADER_INDEX		m_VSHI_DepthToTexcrdForRGB;

	// Same as above, but calculates a projected texture coordinate
	//  in texture unit 4.  This coordinate is used to map the screen-
	//  aligned fog calculation textures to arbitrary geometry in the 
	//  scene so that the texels of the screen-aligned texture always
	//  fall on their corresponding screen pixel, regardless of the
	//  shape or position of the geometry being rendered.
	SM_SHADER_INDEX		m_VSHI_DepthToTexcrdForRGB_TC4Proj;


};


#endif					// __PolygonalVolumeFog_H



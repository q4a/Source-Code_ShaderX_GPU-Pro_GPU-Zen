/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  TextDisplay.cpp

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



#include "NV_D3DCommon\TextDisplay.h"
#include "shared\NV_Common.h"
#include "shared\NV_Error.h"

#include "NVD3DVers.h"
#include "NV_D3DCommon\GeometryVB_Color.h"
#include "NV_D3DCommon\SimpleVBObject.h"
#include "NV_D3DCommon\SimpleObject.h"


#include "Constants.h"		// Be sure to include .\ in your effect's project
							//  include path

#include "eb_file.h"		// for GetFilePath()
#include <string>

using namespace std;

///////////////////////////////////////////////////////////////


#define		DEFAULT_GLYPH_FILE	"Glyphs_01.tga"


///////////////////////////////////////////////////////////////


TextDisplay::TextDisplay()
{
	m_VSH_TextDisplay = 0;

	m_pD3DDev = NULL;
	m_pObject = NULL;

	m_pGlyphs = NULL;
}


TextDisplay::~TextDisplay()
{
	Free();
}


HRESULT	TextDisplay::Free()
{
	// Initialize also calls free, so no need to free things on your own 
	//  before calling Intialize multiple times

	if( m_pD3DDev != NULL )
	{
		DEL_VSH( m_pD3DDev, m_VSH_TextDisplay );
	}

	SAFE_DELETE( m_pObject );

	FreeGlyphs();

	SAFE_RELEASE( m_pD3DDev );

	
	return( S_OK );
}


void TextDisplay::FreeGlyphs()
{
	SAFE_RELEASE( m_pGlyphs );

	m_fCharacterWidthInTexels = 0.0f;
	m_fCharacterHeightInTexels = 0.0f;
}


HRESULT		TextDisplay::Initialize( NV_D3DDEVICETYPE* pDev )
{
	HRESULT hr;
	hr = Initialize( pDev, DEFAULT_GLYPH_FILE );
	return( hr );
}


HRESULT		TextDisplay::Initialize( NV_D3DDEVICETYPE* pDev, char * glyph_filename )
{
	HRESULT hr = S_OK; 
	if( pDev == NULL )
	{
		FDebug("Can't init TextDisplay!\n");
		assert( false );
		return( E_FAIL );
	}

	Free();

	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();


	//////////////////////////////////
	// load glyphs texture

	if( glyph_filename == NULL )
	{
		hr = LoadGlyphs( DEFAULT_GLYPH_FILE );
	}
	else
	{
		hr = LoadGlyphs( glyph_filename );
	}

	if( FAILED(hr) )
	{
		return( hr );
	}

	//////////////////////////////////
	// Load vertex shader

	VertexColor_Decl decl;

	hr = effect_api::LoadAndCreateShader( m_pD3DDev,
										 effect_api::GetFilePath("TextDisplay.vso"),
										 decl.GetVShaderDeclaration(),
										 0,
										 SHADERTYPE_VERTEX,
										 & m_VSH_TextDisplay );
	if( FAILED(hr) )
	{
		assert( false );
		return( hr );		
	}



	//////////////////////////////////
	// create object to use in drawing

	SimpleObject * pObj = new SimpleObject;
	if( pObj == NULL )
	{
		FDebug("can't create simpleobject!\n");
		assert( false );
		return( E_FAIL );
	}

	pObj->m_PrimType = D3DPT_TRIANGLELIST;
	pObj->m_bIsValid = true;
	pObj->Allocate( 4 * 256, 6 * 256 );


	int i;
	BYTE x, y;
	int mi;

	for( i=0; i < pObj->m_wNumVerts; i++ )
	{
		// ARGB is mapped to wzyx in the vertex shader
		// x = x coord of vertex in the quad
		// y = y coord of vertex in the quad
		// z = character index (same for all verts in a quad)
		// 
		// This color value determines the index of the character
		//  and thus the texture coordinate of the glyph to display.
		//  It also determines the coordinates of each vertex of the 
		//  displayed quad on screen with the x,y members.

		mi = i % 4;

		if( mi == 0 || mi == 1 )
			x = 0;
		else
			x = 1;

		y = i % 2;

		pObj->m_pVertices[i].diffuse = D3DCOLOR_ARGB( 0, i/4, y, x );
	}


//	D3DCOLOR_ARGB(a,r,g,b) \
//		((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

	int quad;
	for( i=0; i < pObj->m_wNumInd; i += 6 )
	{
		quad = i / 6;

		if( quad * 4 + 3 >= pObj->m_wNumVerts )
		{
			FDebug("error in creating text quads object!\n");
			assert( false );
			i = pObj->m_wNumInd;	// stop the loop
		}

		pObj->m_pIndices[i  ] = quad * 4;
		pObj->m_pIndices[i+1] = quad * 4 + 1;
		pObj->m_pIndices[i+2] = quad * 4 + 2;

		pObj->m_pIndices[i+3] = quad * 4 + 1;
		pObj->m_pIndices[i+4] = quad * 4 + 3;
		pObj->m_pIndices[i+5] = quad * 4 + 2;
	}


	m_pObject = new GeometryVB_Color;
	if( m_pObject == NULL )
	{
		FDebug("can't make object!\n");
		assert( false );
		SAFE_DELETE( pObj );
		return( E_FAIL );
	}

	m_pObject->CreateFromSimpleObject( pObj, m_pD3DDev );

	SAFE_DELETE( pObj );

	//////////////////////////////////

	SetCharacterWidth( 0.02f );

	//////////////////////////////////

	OutputGlyphCharacters();


	return( hr );
}


void	TextDisplay::OutputGlyphCharacters()
{
	// prints characters to debug text
	// so you can copy them in another font
	// to create glyph textures
	
	int i;
	int v1, v2, v3, dv;
	dv = 32;
	v1 = 32;
	v2 = 65;
	v3 = 97;

	FDebug("Glyph characters:\n");

	for( i = v1; i < v1+dv; i++ )
	{
		FDebug( "%c", i );		
	}
	FDebug("\n");

	for( i = v2; i < v2+dv; i++ )
	{
		FDebug( "%c", i );		
	}
	FDebug("\n");

	for( i = v3; i < v3+dv; i++ )
	{
		FDebug( "%c", i );		
	}
	FDebug("\n\n");

}



HRESULT	TextDisplay::LoadGlyphs( char * filename )
{
	HRESULT hr = S_OK;

	if( filename == NULL )
	{
		assert( false );
		return( E_FAIL );		
	}
	if( m_pD3DDev == NULL )
	{
		assert( false );
		return( E_FAIL );
	}



	FDebug("Attempting to load glyphs:  %s\n", filename );

	string filepath = effect_api::GetFilePath( filename );

	FDebug("  Found glyphs file: %s\n", filepath.c_str() );


	hr = D3DXCreateTextureFromFileExA( m_pD3DDev,
		filepath.c_str(),
		D3DX_DEFAULT,				// width
		D3DX_DEFAULT,				// height 
		0,							// mip
		0,							// usage
		D3DFMT_UNKNOWN,				// format
		D3DPOOL_DEFAULT,			// pool
		D3DX_FILTER_BOX,
		D3DX_FILTER_BOX,
		0,							// 0 for no color key
		NULL,
		NULL,
		& m_pGlyphs );

	if (FAILED(hr))
	{
		assert( false );
		return hr;
	}


	//////////////////////////////////////////////////////////
	// Now get texture width, height from the texture itself

	if( m_pGlyphs == NULL )
	{
		assert( false );
		return( E_FAIL );
	}

	IDirect3DSurface8 * pSurf;

	hr = m_pGlyphs->GetSurfaceLevel( 0, &pSurf );
	if( FAILED(hr) )
	{
		assert( false );
		return( E_FAIL );
	}

	D3DSURFACE_DESC		desc;

	hr = pSurf->GetDesc( & desc );

	if( FAILED(hr))
	{
		assert( false );
		return( E_FAIL );
	}

	UINT w, h;
	w = desc.Width;
	h = desc.Height;

	FDebug("Glyphs texture is %u by %u\n", w, h );

	float texel_w, texel_h;		// size of one texel in width & height
	texel_w = 1.0f / ((float)w);
	texel_h = 1.0f / ((float)h);

	//////////////////////////////////////////////////////////
	// Read glyph description from the associated text file

			// have to duplicate it for some reason - 
			// LocalFree() not working as the docs say it should.  
			// Grrr.
	char * dup = new char[ strlen( filename ) + 10 ];
	strcpy( dup, filename );
	string fileup = strupr( dup );
	delete [] dup;


	// search for file types
	char * res = strstr( fileup.c_str(), ".TGA" );

	if( res != NULL )
	{
		FDebug("Glyph is a TGA file\n");
	}

	int not_found = -1;
	int pos;
	
	int vers, char_w, char_h;


	pos = fileup.find( "." );
	if( pos != not_found )
	{
		fileup.replace( pos, 4, ".txt" );
		FDebug("Trying to find glyph description file:  %s\n", fileup.c_str() );
		
		filepath = effect_api::GetFilePath( fileup );

		FDebug("  Found glyph desc file: %s\n", filepath.c_str() );

		FILE * fp;
		fp = fopen( filepath.c_str(), "rt" );

		if( fp == NULL )
		{
			FDebug("  Can't open glyph desc file!\n");
			assert( false );
			return( E_FAIL );
		}

		char buf[2048];
		fscanf( fp, "%s", buf );
		
		const int equal = 0;

		if( strcmp( buf, "VERSION") == equal )
		{
			fscanf( fp, "%d", &vers	);
			
			if( vers == 1 )
			{
				FDebug("  Glyph desc version: %d\n", vers );

				fscanf( fp, "%s %d\n", buf, &char_w );

				if( strcmp( buf, "WIDTH") != equal )
				{
					FDebug("Error reading glyph file!\n");
					assert( false );
					return( E_FAIL );
				}	

				fscanf( fp, "%s %d\n", buf, &char_h );

				if( strcmp( buf, "HEIGHT") != equal )
				{
					FDebug("Error reading glyph file!\n");
					assert( false );
					return( E_FAIL );
				}	
			}
			else
			{
				FDebug("  Unknown glyph description version!\n");
				assert( false );
				return( E_FAIL );
			}
		}



		FDebug("  Read glyph description:\n");
		FDebug("    Character Width:  %d\n", char_w );
		FDebug("    Character Height: %d\n", char_h );
		FDebug("\n");

		fclose( fp );
		fp = NULL;

	}
	else
	{
		FDebug("Can't make glyph description filename\n");
		assert( false );
		return( E_FAIL );
	}


	// Compute width and height of one character in texture coords

	m_fGlyphWidth	= texel_w * char_w;
	m_fGlyphHeight	= texel_h * char_h;

	// Store # texels per character width and height

	m_fCharacterWidthInTexels = char_w;
	m_fCharacterHeightInTexels = char_h;


	SAFE_RELEASE( pSurf );


	return( hr );
}




void	TextDisplay::Map0011ToScreen( float * out_x, float * out_y, float in_x, float in_y )
{
	// Maps coordinates in the range 0,0 (upper left) to 1,1 (lower right) 
	//   to D3D screen coords which are from [-1,1] in each axis
	// This scales the range [0,1] to the range [-1,1] in each axis

	if( out_x != NULL )
	{
		*out_x = -1.0f + 2.0f * in_x;	
	}

	if( out_y != NULL )
	{
		*out_y = -1.0f + 2.0f * (1.0f - in_y);
	}
}



				// positions are from 0,0 in upper left to 1,1 in lower right
HRESULT	TextDisplay::RenderText( char * text, float pos_x, float pos_y )
{
	//@@@ Need better way to set constants less often


	HRESULT hr = S_OK; 

	if( m_pD3DDev == NULL )
	{
		FDebug("Can't RenderText until I have a device!\n");
		assert( false );
		return( E_FAIL );
	}
	if( m_pObject == NULL )
	{
		assert( false );
		return( E_FAIL );
	}

	// Scale positions to D3D screen coords
	Map0011ToScreen( &pos_x, &pos_y, pos_x, pos_y );


	// CV_TDPARAM_1 contains:
	// x = glyph width in texture coords
	//	 = size of single character in texture coords
	//	 = number of pixels a character is wide * pixel size in texture coords
	// y = glyph height in texture coords
	// z = screen position x
	// w = screen position y

	D3DXVECTOR4	vec = D3DXVECTOR4( m_fGlyphWidth, m_fGlyphHeight, pos_x, pos_y );

	m_pD3DDev->SetVertexShaderConstant( CV_TDPARAM_1, &vec, 1 );


	const float masks[] = {	1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f, 
							0.0f, 0.0f, 0.0f, 1.0f };
	m_pD3DDev->SetVertexShaderConstant( CV_TDMASK1, &masks, 4 );



	vec = D3DXVECTOR4( m_fCharSizeX, m_fCharSizeY, 256.0f, 0.0f );
	m_pD3DDev->SetVertexShaderConstant( CV_TD_SIZE, &vec, 1 );

	vec = D3DXVECTOR4( 0.0f, 0.5f, 1.0f, 2.0f );
	m_pD3DDev->SetVertexShaderConstant( CV_TD_CONST1, &vec, 1 );

	vec = D3DXVECTOR4( 65.0f, 97.0f, 32.0f, 0.0f );
	m_pD3DDev->SetVertexShaderConstant( CV_TD_ROWTHRESH, &vec, 1 );

	vec = D3DXVECTOR4( 1.0f / 4.0f, 4.0f, 0.0f, 0.0f );
	m_pD3DDev->SetVertexShaderConstant( CV_TD_MODULO, &vec, 1 );


	//////////////////////////////////////////////////////////////

	// expand text into series of vhsader constants

	int len;
	int i;


	len = strlen( text );

	// We only have 96 * 4 vshader floats to work with, and 
	//  we use 1 float per character, so limit size of the 
	//  text that can be displayed in a single rendering call to
	//  256 characters, which will use 256/4 = 64 vshader constants

	if( len > 256 )
		len = 256;


//@@@@@ store float buffer somewhere for re-use

	// Copy char ASCII values to floating point constants.
	// Would be ideal if there were a way to supply chars directly as
	//  vshader constants, but unfortunately there is no way to do this.

	float tbuf[256];		// 256 / 4 = 64 vshader consts	
	for( i=0; i < len; i++ )
	{	
		tbuf[i] = text[i];
	}

	// Set vshader constants for the characters used
	DWORD ncon = (DWORD)( len / 4 + 1 );
	m_pD3DDev->SetVertexShaderConstant( CV_TD_TEXTBASE, & tbuf, ncon );


	m_pD3DDev->SetVertexShader( m_VSH_TextDisplay );

	m_pD3DDev->SetPixelShader( 0 );
	

	// Select Texture as output
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE	);

	// Diagnostic to display colors that determine character position
	//	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE	);


	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

	m_pD3DDev->SetTexture( 0, m_pGlyphs );


	////////////////////////////
	////////////////////////////
	// Draw only a subset of the object's vertices

	int numprim = len * 2;		// 2 tris for each letter.

	if( numprim * 3 > m_pObject->m_wNumInd )
	{
		FDebug("TextDisplay can only render %d characters at a time!\n", m_pObject->m_wNumInd / 6 );
		numprim = m_pObject->m_wNumInd / 3;
	}

	m_pObject->Draw( 0, numprim );

	return( hr );
}




void	TextDisplay::SetCharacterWidth( float width )
{
	// width is the size of each character in screen coords, 
	//  with screen coords in the range [0,1] to cover the entire
	//  screen

	// coords are remapped to [-1,1] range, so multiply width by two
	m_fCharSizeX =  2.0f * width;

	// calculate Y size to preserve aspect ratio of characters
	m_fCharSizeY = - m_fCharacterHeightInTexels * m_fCharSizeX / m_fCharacterWidthInTexels;

}


float TextDisplay::GetCharacterHeight()
{
	// returns char height in screen coords from [0,1]
	return( -m_fCharSizeY / 2.0f );		
}


void	TextDisplay::SetGoodRenderstates()
{
	// Sets renderstates for good rendering of
	//  the text (point sampling, no culling)

	if( m_pD3DDev != NULL )
	{
		m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );

        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
		m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

	}
	else
	{
		assert( false );
	}
}







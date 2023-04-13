/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  TextDisplay.h

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

Basic class to render text using vertex shader and glyph texture.

DX8 for now.  I may extend to OpenGL, time permitting, though OpenGL has it's
own built in text functionality.


******************************************************************************/

#ifndef _NV_TEXTDISPLAYDX8_H__
#define _NV_TEXTDISPLAYDX8_H__


#include <NVD3DVers.h>
#include <NV_D3DCommon\NVD3DCommonSwitch.h>

#include <string>


///////////////////////////////////////
// forward decl

class GeometryVB_Color;


///////////////////////////////////////

class TextDisplay
{
protected:

	NV_D3DDEVICETYPE*	m_pD3DDev;
	
	HRESULT		Free();
	void		FreeGlyphs();



	DWORD		m_VSH_TextDisplay;


	GeometryVB_Color		* m_pObject;		// collection of quads on which the 
												//  letters are rendered

	NV_IDIRECT3DTEXTURE*	m_pGlyphs;
	//@@@ rename
	float		m_fGlyphWidth;				// size of one character in texture coord units
	float		m_fGlyphHeight;				// ""

	float		m_fCharacterWidthInTexels;
	float		m_fCharacterHeightInTexels;

	float m_fCharSizeX;			// size on screen
	float m_fCharSizeY;


public:
	
	HRESULT		Initialize( NV_D3DDEVICETYPE* pDev );
	HRESULT		Initialize( NV_D3DDEVICETYPE* pDev, char * glyph_filename );

	HRESULT		LoadGlyphs( char * filename );

				// positions are from 0,0 in upper left to 1,1 in lower right
	HRESULT		RenderText( char * text, float pos_x, float pos_y );


	// utility functions
	void		Map0011ToScreen( float * out_x, float * out_y, float in_x, float in_y );

	void		SetCharacterWidth( float width );

	void		SetGoodRenderstates();	// Sets renderstates for good rendering of
										//  the text (point sampling, no culling)

	float		GetCharacterHeight();

	void		OutputGlyphCharacters();	// prints characters to debug text
											// so you can copy them in another font
											// to create glyph textures


	TextDisplay();
	~TextDisplay();
};



#endif							// _NV_TEXTDISPLAYDX8_H__







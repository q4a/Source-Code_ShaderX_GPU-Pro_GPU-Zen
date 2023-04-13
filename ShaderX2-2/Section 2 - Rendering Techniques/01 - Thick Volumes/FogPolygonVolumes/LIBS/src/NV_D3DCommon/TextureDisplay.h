/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  TextureDisplay.h

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


This is a class which can be compiled for either D3D8 or D3D9.  The API is
controlled by the #included NVD3DVers.h file, which each of your own projects
should maintain their own copy of, with separate NVD3DVers.h files for each
project.  As long as your #include path points to the location of your own
NVD3DVers.h, this common shared class will be compiled to use the version
of D3D that you define in your own NVD3DVers.h.
In this way, a single code base is easy to maintain between D3D8 and D3D9, 
and you do not have to worry about integrating changes to these common 
shared source files across two separate versions.
See the SAMPLE_NVD3DVers.h file in the same directory as this file for an 
example of what your own NVD3DVers.h must contain and define.





******************************************************************************/



#ifndef __NV_TEXTUREDISPLAY_89_H
#define __NV_TEXTUREDISPLAY_89_H

#include <NVD3DVers.h>
#include <NV_D3DCommon/ShaderManager.h>


//////////////////////////////////////////////

#define NTD_TEX		4


struct FRECT
{
	float left;
	float right;
	float top;
	float bottom;
};



class TextureDisplay
{
public:


	HRESULT Initialize( NV_D3DDEVICETYPE* pD3DDev,
						ShaderManager ** ppShaderManager,
						const char * vshader_filepath );

	HRESULT Initialize( NV_D3DDEVICETYPE* pD3DDev,
						ShaderManager ** ppShaderManager,
						UINT vsh_const_point_pos );
	HRESULT Free();

	std::string GetShaderProgramText( int vsh_const_point_pos );


	HRESULT	SetTexture( int n, NV_IDIRECT3DTEXTURE ** pTex );

	NV_IDIRECT3DTEXTURE * GetTexture( int n );

			// coordinates are from 0,0 to 1,1 to cover entire render target
	HRESULT SetTextureRect( int n, float left, float right, float top, float bottom );


	void	SetRenderState();			// sets state & stream source for rendering
	void	SetRenderStateConstColor( DWORD dwARGBColor );


	HRESULT RenderTextures();

	HRESULT RenderTexture( int texture_index, bool set_renderstate );
	HRESULT RenderTexture( int texture_index, bool set_renderstate, bool set_vertex_shader );


	void	ClearTextureData();

	////////////////////////////////////////////////////////////
	// Render a quad function that does not set any render
	//  states or use the array of established textures & rects.
	//  This lets you display whatever you like on your own, and
	//  set whatever render states you feel like.
	// It just positions the quad for you & renders it.

	HRESULT	RenderAlignedQuad( const FRECT * pRect );

	// Use set_vshader false to set and use your own vshader before
	//   calling this function.
	HRESULT	RenderAlignedQuad( const FRECT * pRect, bool set_vshader );

	////////////////////////////////////////////////////////////

	NV_VERTEXDECLTYPE *		GetVertexShaderDecl();


	TextureDisplay();
	virtual ~TextureDisplay();


	NV_IDIRECT3DTEXTURE **	m_ppTextures[NTD_TEX];

	FRECT					m_pRects[NTD_TEX];

	DWORD					m_dwFillMode;
	

private:

	typedef struct tagQuadVertex
    {
	    D3DXVECTOR3 Position;
        D3DXVECTOR2 Tex;
    } QuadVertex;


	NV_D3DDEVICETYPE*			m_pD3DDev;
	NV_VERTEXBUFFERTYPE*		m_pVertexBuffer;


	NV_VERTEXDECLTYPE		m_pVSHDecl[4];
#ifdef NV_USING_D3D9
	IDirect3DVertexDeclaration9 * m_pVertexDeclaration;
#endif


	SM_SHADER_INDEX		m_VSHI_Shader;

	ShaderManager	*	m_pShaderManager;
	ShaderManager	**	m_ppTDShaderManager;

		// Index of vshader constants where point position data starts
		// Class uses 4 vshader constants starting at this location
	UINT		m_uVSHConstPointPositionsBase;


	HRESULT Initialize( NV_D3DDEVICETYPE* pD3DDev,
						ShaderManager ** ppShaderManager );

	HRESULT CreateVShader( const char * filepath );
	HRESULT CreateVShader( int vsh_const_point_pos );

	void	SetVertexShader( SM_SHADER_INDEX shader );
};

















#endif						// __NV_TEXTUREDISPLAY_89_H

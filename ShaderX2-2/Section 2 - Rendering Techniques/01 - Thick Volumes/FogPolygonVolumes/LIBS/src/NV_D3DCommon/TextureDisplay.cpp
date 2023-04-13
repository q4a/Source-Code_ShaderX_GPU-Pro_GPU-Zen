/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  TextureDisplay.cpp

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


#include <shared/NV_Common.h>
#include <shared/NV_Error.h>


#include <NV_D3DCommon/TextureDisplay.h>
#include <NV_D3DCommon/ShaderManager.h>

/*
///@@@@@@@@@@@@@@ need to remove constants.h
#pragma message( "***** Texture display **** needs removal of Constants.h!!!" )

#include "Constants.h"					// You must create this file for each
										// project and make sure it is in your
										// build #include path settings

// In "Constants.h" you must define the following:
//	CV_PTS_BASE   :  4 consecutive float4 vectors (4 vertex shader const slots)

// Example Constants.h : 
// #define CV_PTS_BASE	10
// //					11
// //					12
// //					13
// #define MY_NEXT_OTHER_CONSTANT	14
*/

///////////////////////////////////////


TextureDisplay::TextureDisplay()
{
	ClearTextureData();

	m_pShaderManager	= NULL;
	m_ppTDShaderManager	= NULL;

	m_pVertexBuffer		= NULL;
	m_pD3DDev			= NULL;

#ifdef NV_USING_D3D9
	 m_pVertexDeclaration = NULL;
#endif

}


TextureDisplay::~TextureDisplay()
{
	Free();
}


HRESULT TextureDisplay::Free()
{
	ClearTextureData();

	SAFE_RELEASE( m_pVertexBuffer );

	SAFE_DELETE( m_pShaderManager );		// only deleted if != NULL
	m_ppTDShaderManager = NULL;

#ifdef NV_USING_D3D9
	SAFE_RELEASE( m_pVertexDeclaration );
#endif

	SAFE_RELEASE( m_pD3DDev );

	return( S_OK );
}

/////////////////////////////////////////////////////////

NV_VERTEXDECLTYPE * TextureDisplay::GetVertexShaderDecl()
{

#ifdef NV_USING_D3D8
	m_pVSHDecl[0] = D3DVSD_STREAM(0);
	m_pVSHDecl[1] = D3DVSD_REG( 0, D3DVSDT_FLOAT3 );	// pos
	m_pVSHDecl[2] = D3DVSD_REG( 1, D3DVSDT_FLOAT2 );	// tex
	m_pVSHDecl[3] = D3DVSD_END();
#endif

#ifdef NV_USING_D3D9

    D3DVERTEXELEMENT9 decl[] =
    {
        { 0, 0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
        { 0, 12,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
        D3DDECL_END()
    };

	int i;
	for( i=0; i < 3; i++ )
	{
		m_pVSHDecl[i] = decl[i];
	}

#endif

	return( m_pVSHDecl );
}



HRESULT TextureDisplay::CreateVShader( const char * filepath )
{
	HRESULT hr = S_OK;

	FAIL_IF_NULL( m_ppTDShaderManager );
	FAIL_IF_NULL( *m_ppTDShaderManager );

	ShaderManager * pMan = *m_ppTDShaderManager;

	//@@ No check to see if pMan was destroyed & re-created since
	//  we loaded our shader.  We could keep a pointer to the
	//  external shader manager to check if this has happened, but
	//  I'm not going to worry about that.

#ifdef NV_USING_D3D8

	hr = pMan->LoadAndCreateShader( filepath,
									GetVertexShaderDecl(), 
									0,	// usage
									SM_SHADERTYPE_VERTEX,
									& m_VSHI_Shader );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif

#ifdef NV_USING_D3D9

	hr = pMan->LoadAndCreateShader( filepath, SM_SHADERTYPE_VERTEX, & m_VSHI_Shader );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif


	return( hr );
}



HRESULT TextureDisplay::CreateVShader( int vsh_const_point_pos )
{
	HRESULT hr = S_OK;
	FAIL_IF_NULL( m_ppTDShaderManager );
	FAIL_IF_NULL( *m_ppTDShaderManager );

	std::string program;
	program = GetShaderProgramText( vsh_const_point_pos );

	ShaderManager * pMan = *m_ppTDShaderManager;

	std::string name = "TextureDisplay shader using base = ";
	char buf[20];
	name += itoa( vsh_const_point_pos, buf, 10 );


#ifdef NV_USING_D3D8

	hr = pMan->LoadAndCreateShaderFromMemory(	program.c_str(),
												name.c_str(),
												GetVertexShaderDecl(),
												NULL,
												SM_SHADERTYPE_VERTEX,
												& m_VSHI_Shader );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif

#ifdef NV_USING_D3D9

	hr = pMan->LoadAndCreateShaderFromMemory( program.c_str(),
												name.c_str(),
												SM_SHADERTYPE_VERTEX,
												& m_VSHI_Shader );
	ASSERT_AND_RET_IF_FAILED(hr);

#endif

	return( hr );
}






void TextureDisplay::SetVertexShader( SM_SHADER_INDEX shader )
{
	ASSERT_AND_RET_IF_NULL( m_ppTDShaderManager );
	ASSERT_AND_RET_IF_NULL( *m_ppTDShaderManager );

	ShaderManager * pMan = *m_ppTDShaderManager;

	HRESULT hr;
	hr = pMan->SetShader( shader );
	ASSERT_IF_FAILED(hr);

}



HRESULT TextureDisplay::Initialize( NV_D3DDEVICETYPE* pD3DDev,
									ShaderManager ** ppShaderManager,
									UINT vsh_const_point_pos )
{
	HRESULT hr = S_OK;

	hr = Initialize( pD3DDev, ppShaderManager );
	ASSERT_AND_RET_IF_FAILED( hr );

	hr = CreateVShader( vsh_const_point_pos );
	ASSERT_AND_RET_IF_FAILED( hr );

	m_uVSHConstPointPositionsBase = vsh_const_point_pos;

	return( hr );
}


HRESULT TextureDisplay::Initialize( NV_D3DDEVICETYPE* pD3DDev,
									ShaderManager ** ppShaderManager,
									const char * vshader_filepath )
{
	HRESULT hr = S_OK;

	hr = Initialize( pD3DDev, ppShaderManager );
	ASSERT_AND_RET_IF_FAILED( hr );

	hr = CreateVShader( vshader_filepath );
	ASSERT_AND_RET_IF_FAILED( hr );

	return( hr );
}



HRESULT TextureDisplay::Initialize( NV_D3DDEVICETYPE* pD3DDev,
									ShaderManager ** ppShaderManager )
{
	// ppShaderManager 
	//	 Handle or NULL.  
	//   Creates it's own ShaderManager if NULL
	//

	HRESULT hr = S_OK;
	FAIL_IF_NULL( pD3DDev );

	Free();

	pD3DDev->AddRef();
	m_pD3DDev = pD3DDev;

	/////////////////////////////////////////////////

	m_pShaderManager = NULL;
	m_ppTDShaderManager = NULL;
	bool create_shdrman = false;

	if( ppShaderManager == NULL )
	{
		create_shdrman = true;
	}
	else if( *ppShaderManager == NULL )
	{
		create_shdrman = true;
	}

	if( create_shdrman == true )
	{
		m_pShaderManager = new ShaderManager;
		FAIL_IF_NULL( m_pShaderManager );

		m_pShaderManager->Initialize( m_pD3DDev );
		m_ppTDShaderManager = & m_pShaderManager;
	}
	else
	{
		m_ppTDShaderManager = ppShaderManager;
	}

	
	/////////////////////////////////////////////////


	ClearTextureData();


	////////////////////////////////////////////////////////
	// Set a default layout of 4 textures on screen, one in 
	//  each corner

	// Upper left corner;
	SetTextureRect(  0, 0.0f, 0.5f, 0.0f, 0.5f );
	// upper right
	SetTextureRect(  1, 0.5f, 1.0f, 0.0f, 0.5f );
	
	// Lower left corner
	SetTextureRect(  2, 0.0f, 0.5f, 0.5f, 1.0f );
	SetTextureRect(  3, 0.5f, 1.0f, 0.5f, 1.0f );
	////////////////////////////////////////////////////////


	m_dwFillMode = D3DFILL_SOLID;


	////////////////////////////////////////////////////
	// Load and create vertex shader

	NV_VERTEXDECLTYPE * pVSHDecl = GetVertexShaderDecl();
	FAIL_IF_NULL( pVSHDecl );


#ifdef NV_USING_D3D9
	hr = m_pD3DDev->CreateVertexDeclaration( pVSHDecl, & m_pVertexDeclaration );
	ASSERT_AND_RET_IF_FAILED(hr);
#endif


	////////////////////////////////////////////////////
	// Allocate Vertex buffer

#ifdef NV_USING_D3D8
	hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(QuadVertex),
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										0, D3DPOOL_DEFAULT, &m_pVertexBuffer );
	ASSERT_AND_RET_IF_FAILED(hr);
#endif
#ifdef NV_USING_D3D9
	hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(QuadVertex),
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										0, D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL );
	ASSERT_AND_RET_IF_FAILED(hr);
#endif



	QuadVertex      *pBuff;
	int i;

	if (m_pVertexBuffer)
	{
		hr = m_pVertexBuffer->Lock(	0, 4 * sizeof(QuadVertex), 
									(NV_VBDATAHANDLE) & pBuff, 0);
		ASSERT_AND_RET_IF_FAILED(hr);



		float uv_base;
		float uv_max;

		uv_base = 0.0f;
		uv_max  = 1.0f; 

        for(i = 0; i < 4; i++ )
        {
			// Position will be overridden in vertex shader
			// according to the index value in position.x

			pBuff->Position = D3DXVECTOR3( i, i, i );

		    pBuff->Tex      = D3DXVECTOR2((i==0 || i==3) ? uv_base : uv_max , 
                                          (i<2)          ? uv_max : uv_base );
		    pBuff++;
        }
        m_pVertexBuffer->Unlock();
    }



	return( hr );
}


void TextureDisplay::ClearTextureData()
{
	int n;
	for( n=0; n < NTD_TEX; n++ )
	{
		m_ppTextures[n] = NULL;
	}
}


HRESULT	TextureDisplay::SetTexture( int n, NV_IDIRECT3DTEXTURE ** pTex )
{
	HRESULT hr = S_OK;

	if( n < NTD_TEX )
	{
	
		m_ppTextures[n] = pTex;

	}
	else
	{
		// Not enough texture pointers available!
		hr = E_FAIL;
		assert( false );
	}



	return( hr );
}


NV_IDIRECT3DTEXTURE * TextureDisplay::GetTexture( int n )
{
	NV_IDIRECT3DTEXTURE * pOut = NULL;
	NV_IDIRECT3DTEXTURE ** ppTex = NULL;


	if( n < NTD_TEX )
	{
		ppTex = m_ppTextures[n];

		if( ppTex != NULL )
		{	
			pOut = *ppTex;
		}
	}
	else
	{
		assert( false );
	}

	return( pOut );
}


HRESULT TextureDisplay::SetTextureRect( int n, float left, float right, float top, float bottom )
{
	// Coords specified so that 0,0 is upper left, 1,1 is lower right;

	HRESULT hr = S_OK;

	if( n < NTD_TEX )
	{
		// Translate 0,0 to 1,1 coords to those appropriate for 
		//  the 3D viewing in HCLIP space

		left	= left - 0.5f;
		right	= right - 0.5f;
		top		= ( 1.0f - top ) - 0.5f;
		bottom	= ( 1.0f - bottom ) - 0.5f;

		left	*= 2.0f;
		right	*= 2.0f;
		top		*= 2.0f;
		bottom	*= 2.0f;

		m_pRects[n].left = left;
		m_pRects[n].right = right;
		m_pRects[n].top = top;
		m_pRects[n].bottom = bottom;

	}



	return( hr );
}



HRESULT	TextureDisplay::RenderAlignedQuad( const FRECT * pRect, bool set_vshader )
{

	// Render using the default vertex shader
	//
	// pRect is from 0,0 to 1,1 to cover the render target
	//
	// pRect->top should be greater than pRect->bottom for 
	//  CCW winding order

	HRESULT hr;
	FAIL_IF_NULL( pRect );

	D3DXVECTOR4  vs[4];

	if( set_vshader )
	{
		SetVertexShader( m_VSHI_Shader );
	}

	float left, right, top, bottom;

	left	= pRect->left - 0.5f;
	right	= pRect->right - 0.5f;
	top		= ( 1.0f - pRect->top ) - 0.5f;
	bottom	= ( 1.0f - pRect->bottom ) - 0.5f;

	left	*= 2.0f;
	right	*= 2.0f;
	top		*= 2.0f;
	bottom	*= 2.0f;

	vs[0] = D3DXVECTOR4( left,  top, 1.0f, 1.0f );
	vs[1] = D3DXVECTOR4( right, top, 1.0f, 1.0f );
	vs[2] = D3DXVECTOR4( right, bottom, 1.0f, 1.0f );
	vs[3] = D3DXVECTOR4( left,  bottom, 1.0f, 1.0f );


#ifdef NV_USING_D3D8
	m_pD3DDev->SetVertexShaderConstant( m_uVSHConstPointPositionsBase, &vs[0], 4 );

	hr = m_pD3DDev->SetStreamSource( 0, m_pVertexBuffer,
										sizeof( QuadVertex ) );
	ASSERT_IF_FAILED(hr);

#endif

#ifdef NV_USING_D3D9

	m_pD3DDev->SetVertexShaderConstantF( m_uVSHConstPointPositionsBase, (float*)&vs[0], 4 );

	hr = m_pD3DDev->SetStreamSource( 0, m_pVertexBuffer,
										0,						// offset from start of buffer
										sizeof( QuadVertex ) );
	ASSERT_IF_FAILED(hr);

	// Also set the vertex declaration if using D3D9
	m_pD3DDev->SetVertexDeclaration( m_pVertexDeclaration );

#endif



	hr = m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	ASSERT_IF_FAILED( hr );

	return( hr );


}


HRESULT	TextureDisplay::RenderAlignedQuad( const FRECT * pRect )
{
	HRESULT hr = S_OK;

	hr = RenderAlignedQuad( pRect, true );

	return( hr );
}




void	TextureDisplay::SetRenderState()
{

	m_pD3DDev->SetPixelShader( 0 );

	///////////////////////////////////////////////

	m_pD3DDev->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
	m_pD3DDev->SetRenderState( D3DRS_FILLMODE,			m_dwFillMode );
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,	false );
	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,			false );
	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,		false );
	m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,			false );


	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE	);

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

}


void	TextureDisplay::SetRenderStateConstColor( DWORD dwARGBColor )
{

	m_pD3DDev->SetPixelShader( 0 );

	///////////////////////////////////////////////

	m_pD3DDev->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
	m_pD3DDev->SetRenderState( D3DRS_FILLMODE,			m_dwFillMode );
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,	false );
	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,			false );
	m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE,		false );
	m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,			false );


	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, dwARGBColor );

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR	);

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

}


HRESULT TextureDisplay::RenderTexture( int texture_index, 
										bool set_renderstate,
										bool set_vertex_shader )
{
	HRESULT hr = S_OK;
	FAIL_IF_NULL( m_ppTDShaderManager );
	FAIL_IF_NULL( *m_ppTDShaderManager );

	if( set_renderstate )
	{
		SetRenderState();
	}

	if( set_vertex_shader )
	{
		(*m_ppTDShaderManager)->SetShader( m_VSHI_Shader );
	}


	// Renders a quad to the display with the associated texture
	// Very simply for now...
	// Would be good to support other display configs later

	int n; 
	NV_IDIRECT3DTEXTURE ** ppTex;

	D3DXVECTOR4  vs[4];

	n = texture_index;

	ppTex = m_ppTextures[n];

	if( ppTex != NULL )
	{
		if( *ppTex != NULL )
		{

			vs[0] = D3DXVECTOR4( m_pRects[n].left, m_pRects[n].top, 1.0f, 1.0f );
			vs[1] = D3DXVECTOR4( m_pRects[n].right, m_pRects[n].top, 1.0f, 1.0f );
			vs[2] = D3DXVECTOR4( m_pRects[n].right, m_pRects[n].bottom, 1.0f, 1.0f );
			vs[3] = D3DXVECTOR4( m_pRects[n].left, m_pRects[n].bottom, 1.0f, 1.0f );

			#ifdef NV_USING_D3D8
				m_pD3DDev->SetVertexShaderConstant( m_uVSHConstPointPositionsBase, &vs[0], 4 );

				hr = m_pD3DDev->SetStreamSource( 0, m_pVertexBuffer,
													sizeof( QuadVertex ) );
				ASSERT_IF_FAILED(hr);

			#endif

			#ifdef NV_USING_D3D9

				m_pD3DDev->SetVertexShaderConstantF( m_uVSHConstPointPositionsBase, (float*)&vs[0], 4 );

				hr = m_pD3DDev->SetStreamSource( 0, m_pVertexBuffer,
													0,						// offset from start of buffer
													sizeof( QuadVertex ) );

				m_pD3DDev->SetVertexDeclaration( m_pVertexDeclaration );

				ASSERT_IF_FAILED(hr);

			#endif

			m_pD3DDev->SetTexture( 0, *ppTex );

			hr = m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
			ASSERT_IF_FAILED( hr );

		}
	}

	return( hr );	
}



HRESULT TextureDisplay::RenderTexture( int texture_index, bool set_renderstate )
{
	HRESULT hr = S_OK;
	hr = RenderTexture( texture_index, set_renderstate, true );
	return( hr );
}




HRESULT TextureDisplay::RenderTextures()
{
	HRESULT hr = S_OK;

	// Renders 4 quads to the display with the associated textures
	// Very simply for now...
	// Would be good to support other display configs later


	SetRenderState();

	int n; 

	for( n=0; n < NTD_TEX; n++ )
	{
		hr = RenderTexture( n, false );

		ASSERT_IF_FAILED(hr);
	}

	return( hr );
}



#define ADD_LINE( p, l ) \
{  program += l;  program += "\n"; }


std::string TextureDisplay::GetShaderProgramText( int vsh_const_point_pos )
{
	// Returns a string with the vertex shader program text
	// vsh_const_point_pos :  The vertex shader constant index
	//   where the point positions are set.  Four consecutive
	//   vertex shader constants are used starting with the 
	//   constant at this index.

	std::string program;
	program = "";

	std::string str;
	char buf[256];
	str = "// Custom TextureDisplay shader using PTS_BASE = ";
	str += itoa( vsh_const_point_pos, buf, 10 );
	
	ADD_LINE( program, str )
	ADD_LINE( program, "#define V_POS		v0									" )
	ADD_LINE( program, "#define V_TEX		v3									" )
	ADD_LINE( program, "vs.1.1													" )
	ADD_LINE( program, "dcl_position		v0									" )
	ADD_LINE( program, "dcl_texcoord0		v3									" )
						// Use vertex data for index into points 
	ADD_LINE( program, "mov		a0.x, V_POS										" )
						// Move vshader constant to the point position

//	ADD_LINE( program, "mov		oPos, c[a0.x + CV_PTS_BASE]
	// replacing CV_PTS_BASE with int value from vsh_const_point_pos
	str = "mov oPos, c[a0.x + ";
	str += itoa( vsh_const_point_pos, buf, 10 );
	str += "]";
	ADD_LINE( program, str )
						// output texture coords
	ADD_LINE( program, "mov		oT0, V_TEX									" )
	ADD_LINE( program, "mov		oT1, V_TEX									" )
	ADD_LINE( program, "mov		oT2, V_TEX									" )
	ADD_LINE( program, "mov		oT3, V_TEX									" )

/*
// Custom TextureDisplay shader using PTS_BASE = 10
#define V_POS		v0									
#define V_TEX		v3									
vs.1.1													
dcl_position		v0									
dcl_texcoord0		v3									
mov		a0.x, V_POS										
mov oPos, c[a0.x + 10]
mov		oT0, V_TEX									
mov		oT1, V_TEX									
mov		oT2, V_TEX									
mov		oT3, V_TEX	
*/


	// Uncomment to display shader program text
//	FMsg("Program: \n%s\n------------------\n", program.c_str() );

	return( program );
}

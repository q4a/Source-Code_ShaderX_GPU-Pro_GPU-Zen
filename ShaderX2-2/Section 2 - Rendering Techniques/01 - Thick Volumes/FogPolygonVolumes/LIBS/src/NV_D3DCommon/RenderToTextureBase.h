/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  RenderToTextureBase.h

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

Base class for D3D rendering classes.  It has:
ShaderManager
Direct3DDevice (either D3D8 or D3D9)
Render target surface / texture pairs for render-to-texture operations


NVD3DVers.h should be created for each project using this.  It is a simple
file which defines either D3D8 or D3D9 usage and #includes NVD3DCommonSwitch.h
to switch between D3D8 and D3D9.


******************************************************************************/


#ifndef _GJ_RTTBASE89_H
#define _GJ_RTTBASE89_H


#include <shared/NV_Common.h>
#include <shared/NV_Error.h>


#include <NVD3DVers.h>		// #includes D3D8 or D3D9


class ShaderManager;


#ifndef CHECK_BOUNDS_HR
#define CHECK_BOUNDS_HR( var, min, max )		\
	if( (var < min) || (var > max) )			\
	{	FDebug("Variable out of bounds!\n");	\
		assert( false ); return(E_FAIL);		\
	}
#endif


///////////////////////////////////////

class RenderToTextureBase
{
private:

	void			SetToNull();
	void			FreeTextureRenderTargets();


protected:

	int		m_nRTTWidth;		// size of render-to-texture surfaces
	int		m_nRTTHeight;

	NV_D3DDEVICETYPE	*	m_pD3DDev;

	// Arrays of render target textures and their corresponding surfaces 
	NV_IDIRECT3DSURFACE	** m_ppRTTSurface;
	NV_IDIRECT3DTEXTURE	** m_ppRTTTexture;
	int		m_nNumSurfaces;

	NV_IDIRECT3DSURFACE	*	m_pRTTDepthBuffer;
	D3DFORMAT				m_DepthFormat;
	DWORD					m_dwDepthClearFlags;

	// default color & depth buffers (to reset device to them after 
	//  rendering to texture)
    NV_IDIRECT3DSURFACE	* m_pDefaultBackbufferColor; 
    NV_IDIRECT3DSURFACE	* m_pDefaultBackbufferDepth; 


	ShaderManager	*	m_pXShaderManager;
	ShaderManager	**	m_ppShaderManager;

public:

	RenderToTextureBase();
	virtual ~RenderToTextureBase();


	HRESULT Initialize( NV_D3DDEVICETYPE * pDev,
						ShaderManager ** ppShaderManager,
						int width, int height,
						int num_surfaces,
						bool create_depth_buffer );
	HRESULT Free();


	inline int	GetSurfaceWidth()	{ return( m_nRTTWidth ); };
	inline int	GetSurfaceHeight()	{ return( m_nRTTHeight); };

	HRESULT	CreateTextureRenderTargets( int width,
										int height,
										int num_surfaces,
										bool create_depth_buffer,
										bool use_stencil = NULL );

	inline HRESULT	GetTexAndSurf( int index,
									NV_IDIRECT3DTEXTURE ** ppTex,
									NV_IDIRECT3DSURFACE ** ppSurf );
	inline HRESULT	GetTexAndSurf( NV_IDIRECT3DTEXTURE *** pppTex,
									NV_IDIRECT3DSURFACE *** pppSurf );

	inline NV_IDIRECT3DTEXTURE *	GetTexture( int index );
	inline NV_IDIRECT3DTEXTURE **	GetTextureHandle( int index );

	inline HRESULT	GetDepthBuffer( NV_IDIRECT3DSURFACE ** pSurf );

	inline int	GetNumSurfaces() { return( m_nNumSurfaces ); };

	inline ShaderManager * GetShaderManager()
	{	
		if( m_ppShaderManager != NULL )
			return( *m_ppShaderManager );
		else
			return( NULL );
	};

	inline DWORD GetDepthClearFlags();

	/////////////////////////////////////////////////
	HRESULT		GrabCurrentBackbuffersAsDefault();
	HRESULT		SetDefaultBuffersToThese(	NV_IDIRECT3DSURFACE * pColor,
											NV_IDIRECT3DSURFACE * pDepth );

	void		GetDefaultBackbuffers( NV_IDIRECT3DSURFACE ** ppColor,
										NV_IDIRECT3DSURFACE ** ppDepth );

	HRESULT		SetRenderTargetsToDefault();
	HRESULT		SetRenderTargetsToDefault( bool set_color, bool set_depth );	// if false, target is set to NULL
	HRESULT		SetRenderTargets( int color_index, int depth_index ); // -1 to set to NULL


};



inline DWORD RenderToTextureBase::GetDepthClearFlags()
{
	return( m_dwDepthClearFlags );
}


inline HRESULT RenderToTextureBase::GetTexAndSurf( int index,
									NV_IDIRECT3DTEXTURE ** ppTex,
									NV_IDIRECT3DSURFACE ** ppSurf )
{
	HRESULT hr = S_OK;

	if( ppTex != NULL )
	{
		if( index >= 0 && index < GetNumSurfaces() &&
			m_ppRTTTexture != NULL )
		{
			*ppTex = m_ppRTTTexture[ index ];
		}
		else
		{
			*ppTex = NULL;
		}
	}

	if( ppSurf != NULL )
	{
		if( index >= 0 && index < GetNumSurfaces() &&
			m_ppRTTSurface != NULL )
		{
			*ppSurf = m_ppRTTSurface[ index ];
		}
		else
		{
			*ppSurf = NULL;
		}
	}

	return( hr );
}


inline HRESULT	RenderToTextureBase::GetTexAndSurf( NV_IDIRECT3DTEXTURE *** pppTex,
													NV_IDIRECT3DSURFACE *** pppSurf )
{
	HRESULT hr = S_OK;

	FAIL_IF_NULL( pppTex );
	FAIL_IF_NULL( pppSurf );
	FAIL_IF_NULL( m_ppRTTSurface );
	FAIL_IF_NULL( m_ppRTTTexture );

	*pppTex		= m_ppRTTTexture;
	*pppSurf	= m_ppRTTSurface;

	return( hr );
}



inline HRESULT RenderToTextureBase::GetDepthBuffer( NV_IDIRECT3DSURFACE ** ppSurf )
{
	HRESULT hr = S_OK;
	FAIL_IF_NULL( ppSurf );
	*ppSurf = m_pRTTDepthBuffer;
	return( hr );
}


inline NV_IDIRECT3DTEXTURE * RenderToTextureBase::GetTexture( int index )
{
	CHECK_BOUNDS_NULL( index, 0, m_nNumSurfaces - 1 );	// returns if failed
	RET_NULL_IF_NULL( m_ppRTTTexture );

	return( m_ppRTTTexture[ index ] );
}

inline NV_IDIRECT3DTEXTURE **	RenderToTextureBase::GetTextureHandle( int index )
{
	CHECK_BOUNDS_NULL( index, 0, m_nNumSurfaces - 1 );	// returns if failed
	RET_NULL_IF_NULL( m_ppRTTTexture );

	return( &( m_ppRTTTexture[ index ]) );
}


#endif


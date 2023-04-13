/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  D3DDeviceAndHWInfo.cpp

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





#include <NV_D3DCommon/D3DDeviceAndHWInfo.h>

#include <shared/NV_Common.h>
#include <shared/NV_Error.h>




D3DDeviceAndHWInfo::D3DDeviceAndHWInfo()
{
	SetToNull();
}


D3DDeviceAndHWInfo::~D3DDeviceAndHWInfo()
{
	Free();
}

/////////////////////////////////////////

void D3DDeviceAndHWInfo::SetToNull()
{
	m_pD3DDev = NULL;

	m_bHasStencil = false;;

	m_fMaxPixelShaderVersion	= -1.0f;
	m_fMaxVertexShaderVersion	= -1.0f;

	m_dwDepthClearFlags	= 0;

	m_bColorDescValid	= false;
	m_bDepthDescValid	= false;
}


HRESULT D3DDeviceAndHWInfo::Free()
{
	SAFE_RELEASE( m_pD3DDev );
	SetToNull();
	return( S_OK );
}


HRESULT D3DDeviceAndHWInfo::Initialize( NV_D3DDEVICETYPE * pDev, bool verbose )
{
	FAIL_IF_NULL( pDev );

	HRESULT hr = S_OK;

	Free();

	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();

	hr = GetDepthStencilInfo( m_pD3DDev, verbose );
	ASSERT_AND_RET_IF_FAILED( hr );

	hr = GetHWShaderInfo( m_pD3DDev, verbose );
	ASSERT_AND_RET_IF_FAILED( hr );

	return( hr );
}


HRESULT D3DDeviceAndHWInfo::GetHWShaderInfo( NV_D3DDEVICETYPE * pDev,
											 bool verbose )
{
	FAIL_IF_NULL( pDev );

	HRESULT hr = S_OK;

	NV_D3DCAPS	caps;
	pDev->GetDeviceCaps( & caps );

	m_fMaxPixelShaderVersion =  (float) (D3DSHADER_VERSION_MAJOR( caps.PixelShaderVersion ));
	m_fMaxPixelShaderVersion += 0.1f * D3DSHADER_VERSION_MINOR( caps.PixelShaderVersion );

	m_fMaxVertexShaderVersion = (float) (D3DSHADER_VERSION_MAJOR( caps.VertexShaderVersion ));
	m_fMaxVertexShaderVersion += 0.1f * D3DSHADER_VERSION_MINOR( caps.VertexShaderVersion );


	if( verbose )
	{
		FMsg("Vertex shader version: %3.3f\n", m_fMaxVertexShaderVersion );
		FMsg("Pixel  shader version: %3.3f\n", m_fMaxPixelShaderVersion );
	}


	return( hr );
}

HRESULT D3DDeviceAndHWInfo::GetHWShaderInfo(	float * pfMaxVertexShaderVersion,
												float * pfMaxPixelShaderVersion		)
{
	if( pfMaxVertexShaderVersion != NULL )
	{
		*pfMaxVertexShaderVersion = m_fMaxVertexShaderVersion;
	}

	if( pfMaxPixelShaderVersion != NULL )
	{
		*pfMaxPixelShaderVersion = m_fMaxPixelShaderVersion;
	}
	return( S_OK );
}




HRESULT D3DDeviceAndHWInfo::GetDepthStencilInfo( NV_D3DDEVICETYPE * pDev,
												 bool verbose )
{
	// fill in info about stencil

	FAIL_IF_NULL( pDev );

	HRESULT hr = S_OK;

	NV_IDIRECT3DSURFACE * pDepthSurf = NULL;

	hr = pDev->GetDepthStencilSurface( &pDepthSurf );

	MSG_RET_IF_FAILED( hr, "Couldn't get depth stencil surface!\n", hr );

	hr = pDepthSurf->GetDesc( & m_DepthBufferDesc );

	pDepthSurf->Release();

	MSG_RET_IF_FAILED( hr, "Couldn't get depth surface desc!\n", hr );

	m_bHasStencil = ( m_DepthBufferDesc.Format == D3DFMT_D15S1)		||
					( m_DepthBufferDesc.Format == D3DFMT_D24S8)		||
					( m_DepthBufferDesc.Format == D3DFMT_D24X4S4 );
	
	if( m_bHasStencil )
	{
		m_dwDepthClearFlags = D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER;
	}
	else
	{
		m_dwDepthClearFlags = D3DCLEAR_ZBUFFER;
	}

	m_bDepthDescValid = true;

	if( verbose )
	{
		char fmt[128];
		
		switch( m_DepthBufferDesc.Format )
		{
		case D3DFMT_D15S1 :
			strcpy( fmt, "D3DFMT_D15S1");
			break;
		case D3DFMT_D24S8 :
			strcpy( fmt, "D3DFMT_D24S8");
			break;
		case D3DFMT_D24X4S4 :
			strcpy( fmt, "D3DFMT_D24X4S4");
			break;
		default :
			strcpy( fmt, "Unknown");
			break;
		}

		if( m_bHasStencil )
			FMsg("Device has stencil:  %s\n", fmt );
		else
			FMsg("Device does not have stencil: %u\n", m_DepthBufferDesc.Format );
		
	}

	return( hr );
}






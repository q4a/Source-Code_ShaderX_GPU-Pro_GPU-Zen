/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  D3DDeviceAndHWInfo.h

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

Class to determine buffer formats, the presence of stencil, hardware shader
support, etc.


******************************************************************************/



#ifndef __H_D3DDEVICEANDHWINFO_H_
#define __H_D3DDEVICEANDHWINFO_H_



#include <NVD3DVers.h>		// Each project has it's own version of NVD3DVers.h
							//  to select D3D8 or D3D9


class D3DDeviceAndHWInfo
{
protected:

	NV_D3DDEVICETYPE * m_pD3DDev;

	bool	m_bHasStencil;
	DWORD	m_dwDepthClearFlags;

	float	m_fMaxPixelShaderVersion;
	float	m_fMaxVertexShaderVersion;

	D3DSURFACE_DESC  m_ColorBufferDesc;
	D3DSURFACE_DESC  m_DepthBufferDesc;

	bool	m_bColorDescValid;
	bool	m_bDepthDescValid;

	void	SetToNull();

public:
	HRESULT Initialize( NV_D3DDEVICETYPE * pDev, bool verbose = false );
	HRESULT Free();

	HRESULT	GetDepthStencilInfo( NV_D3DDEVICETYPE * pDev, bool verbose = false );
	HRESULT GetHWShaderInfo( NV_D3DDEVICETYPE * pDev, bool verbose = false );
	HRESULT GetHWShaderInfo(	float * pfMaxVertexShaderVersion,
								float * pfMaxPixelShaderVersion		);

	DWORD	GetDepthClearFlags() { return( m_dwDepthClearFlags ); };

	D3DSURFACE_DESC * GetColorBufferDesc()  { return( m_bColorDescValid ? (& m_ColorBufferDesc) : NULL ); };
	D3DSURFACE_DESC * GetDepthBufferDesc()  { return( m_bDepthDescValid ? (& m_DepthBufferDesc) : NULL ); };


	D3DDeviceAndHWInfo();
	~D3DDeviceAndHWInfo();
};



#endif

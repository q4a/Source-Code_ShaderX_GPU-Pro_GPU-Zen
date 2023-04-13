/*********************************************************************NVMH4****
Path:  SDK\LIBS\inc\NV_D3DCommon
File:  NVD3DCommonSwitch.h

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

Defines things used by files under common\src\D3DCommon\ 
so they can switch between D3D8 and D3D9



******************************************************************************/


#ifndef _NVD3DCOMMON_89_H_
#define _NVD3DCOMMON_89_H_


#ifndef NV_USE_D3D8
#ifndef NV_USE_D3D9
#error NVD3DCommon : You must define either NV_USE_D3D8 or NV_USE_D3D9 before including NVD3DCommon.h
#endif
#endif


#ifdef NV_USE_D3D8

	// make sure D3D9 hasn't been included
	#ifdef _D3D9_H_
	#error NVD3DCommon : D3D9.h already included when asked to use D3D8!  Find and remove the use of d3d9.h 
	#endif

	#include <d3d8.h>

	// make sure D3D9 hasn't been included
	#ifdef __D3DX9_H__
	#error NVD3DCommon : D3Dx9.h already included when asked to use D3Dx8!  Find and remove the use of d3dx9.h 
	#endif

	#include <d3dx8.h>

#endif


#ifdef NV_USE_D3D9

	// make sure D3D9 hasn't been included
	#ifdef _D3D8_H_
	#error NVD3DCommon : D3D8.h already included when asked to use D3D9!  Find and remove the use of d3d8.h 
	#endif

	#include <d3d9.h>

	// make sure D3D9 hasn't been included
	#ifdef __D3DX8_H__
	#error NVD3DCommon : D3Dx8.h already included when asked to use D3Dx9!  Find and remove the use of d3dx8.h 
	#endif

	#include <d3dx9.h>

#endif


///////////////////////////////////////////

#include <shared/NVD3DMacros.h>

///////////////////////////////////////////

#ifdef NV_USE_D3D8

//#pragma comment( lib, "d3dx8.lib" )
//#pragma comment( lib, "d3d8.lib" )



// #pragma message("NVD3DCommon.h D3D8")
#define NV_USING_D3D8

#define NV_D3DDEVICETYPE	IDirect3DDevice8
#define NV_LPDIRECT3DDEVICE	IDirect3DDevice8*

#define NV_VERTEXDECLTYPE	DWORD
		// the handle returned from creating vertex shader
#define NV_VERTEXSHADERTYPE	DWORD
#define NV_PIXELSHADERTYPE	DWORD

#define NV_VERTEXBUFFERTYPE	IDirect3DVertexBuffer8
#define NV_INDEXBUFFERTYPE	IDirect3DIndexBuffer8

#define NV_VBDATAHANDLE		BYTE**
#define NV_VBLOCKDISCARD	0


#define NV_IDIRECT3DTEXTURE	IDirect3DTexture8 
#define NV_IDIRECT3DSURFACE	IDirect3DSurface8
#define NV_LPDIRECT3DTEXTURE LPDIRECT3DTEXTURE8
#define NV_LPDIRECT3DSURFACE LPDIRECT3DSURFACE8

#define NV_D3DCAPS			D3DCAPS8
#define NV_D3DVIEWPORT		D3DVIEWPORT8

#define SETTEXTURESAMPLERSTATE  SetTextureStageState

// v = device, c = ptr to color surface, d = ptr to depth surface
#define SET_RENDER_TARGET( v, c, d )  v->SetRenderTarget( c, d );

//////////////////////////////////////////
// Sampler state convenience definitions 

#define MAGFILTER		D3DTSS_MAGFILTER
#define MINFILTER		D3DTSS_MINFILTER
#define MIPFILTER		D3DTSS_MIPFILTER

#define ADDRESSU		D3DTSS_ADDRESSU
#define ADDRESSV		D3DTSS_ADDRESSV
#define ADDRESSW		D3DTSS_ADDRESSW


#endif			// NV_USE_D3D8

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


#ifdef NV_USE_D3D9

//#pragma comment( lib, "d3dx9dt.lib" )
//#pragma comment( lib, "d3d9.lib" )


// #pragma message("NVD3DCommon.h D3D9")
#define NV_USING_D3D9

#define NV_D3DDEVICETYPE	IDirect3DDevice9
#define NV_LPDIRECT3DDEVICE	IDirect3DDevice9*

#define NV_VERTEXDECLTYPE	D3DVERTEXELEMENT9
		// the pointer returned from creating vertex shader
#define NV_VERTEXSHADERTYPE	IDirect3DVertexShader9*
#define NV_PIXELSHADERTYPE	IDirect3DPixelShader9*


#define NV_VERTEXBUFFERTYPE	IDirect3DVertexBuffer9
#define NV_INDEXBUFFERTYPE	IDirect3DIndexBuffer9

#define NV_VBDATAHANDLE		void**
#define NV_VBLOCKDISCARD	0

#define NV_IDIRECT3DTEXTURE	IDirect3DTexture9
#define NV_IDIRECT3DSURFACE	IDirect3DSurface9
#define NV_LPDIRECT3DTEXTURE LPDIRECT3DTEXTURE9
#define NV_LPDIRECT3DSURFACE LPDIRECT3DSURFACE9

#define NV_D3DCAPS			D3DCAPS9
#define NV_D3DVIEWPORT		D3DVIEWPORT9

#define SETTEXTURESAMPLERSTATE  SetSamplerState


// v = device, c = ptr to color surface, d = ptr to depth surface
#define SET_RENDER_TARGET( v, c, d )	\
	v->SetRenderTarget( 0, c );			\
	v->SetDepthStencilSurface( d );


//////////////////////////////////////////
// Sampler state convenience definitions 

#define MAGFILTER		D3DSAMP_MAGFILTER
#define MINFILTER		D3DSAMP_MINFILTER
#define MIPFILTER		D3DSAMP_MIPFILTER

#define ADDRESSU		D3DSAMP_ADDRESSU
#define ADDRESSV		D3DSAMP_ADDRESSV
#define ADDRESSW		D3DSAMP_ADDRESSW



#endif			// NV_USE_D3D9


#endif		// _NVD3DCOMMON_89_H_
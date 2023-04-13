/*********************************************************************NVMH4****
Path:  SDK\DEMOS\common\src\D3DCommon
File:  SimpleVBObject.h

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



#ifndef __Simple_VB_Object_89_H__
#define __Simple_VB_Object_89_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <NVD3DVers.h>

class SimpleObject;


/////////////////////////////////////////////////

class SimpleVBObject
{
public:
	enum VBUsage
	{
		DYNAMIC,			// use dynamic vertex buffer
		STATIC				// use static vertex buffer
	};

protected:
	NV_D3DDEVICETYPE*	m_pD3DDev; 
	bool				m_bIsValid;
	D3DPRIMITIVETYPE	m_PrimType;

	DWORD				m_dwVertexSizeInBytes;

	bool				m_bDynamic;		// VBs created with USAGE_DYNAMIC?
	DWORD				m_dwIndexSizeInBytes;

#ifdef NV_USING_D3D9
    IDirect3DVertexDeclaration9*	m_pVertexDeclaration;
#endif


	HRESULT CreateVertexBuffer( UINT Length,
								DWORD Usage,
								DWORD FVF,
								D3DPOOL Pool,
								NV_VERTEXBUFFERTYPE** ppVertexBuffer,
								HANDLE* pHandle );

	HRESULT CreateIndexBuffer(	UINT Length,
								DWORD Usage,
								D3DFORMAT Format,
								D3DPOOL Pool,
								NV_INDEXBUFFERTYPE** ppIndexBuffer,
								HANDLE* pHandle );


public:
	
	NV_VERTEXBUFFERTYPE		*	m_pVertexBuffer;
	UINT						m_wNumVerts;

	NV_INDEXBUFFERTYPE		*	m_pIndexBuffer;
	UINT						m_wNumInd;


	virtual HRESULT CreateFromSimpleObject( SimpleObject * pObj,
											NV_D3DDEVICETYPE*  pD3DDev,
											VBUsage dynamic_or_static = SimpleVBObject::STATIC );

	virtual HRESULT UpdateFromSimpleObject( SimpleObject * pObj );
	virtual HRESULT UpdateIndicesFromSimpleObject( SimpleObject * pObj );
	virtual HRESULT UpdateVerticesFromSimpleObject( SimpleObject * pObj );

	virtual HRESULT Free();

	virtual HRESULT Draw();
	virtual HRESULT Draw( UINT start_index, UINT primitive_count );

	bool	IsValid();

	DWORD				GetBytesPerVertex() { return( m_dwVertexSizeInBytes );	};
	D3DPRIMITIVETYPE	GetPrimitiveType()	{ return( m_PrimType );				};

	SimpleVBObject();
	virtual ~SimpleVBObject();

	friend class ShadowVolumeVBObject8;
	friend class NVBModelLoader;
};



#endif			// __Simple_VB_Object_89_H__


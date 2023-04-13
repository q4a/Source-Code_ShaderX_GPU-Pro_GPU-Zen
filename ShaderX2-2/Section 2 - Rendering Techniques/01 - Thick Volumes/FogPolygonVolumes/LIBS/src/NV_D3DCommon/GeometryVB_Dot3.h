/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  GeometryVB_Dot3.h

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


#ifndef __NV_GEOVB_DOT3_89_H__
#define __NV_GEOVB_DOT3_89_H__


#include <NVD3DVers.h>

#include <shared/NV_Error.h>
#include <shared/NV_Common.h>


#include "NV_D3DCommon/SimpleVBObject.h"

#include <shared/CgD3DSemantics.h>



class SimpleObject;





// A special vertex with basis vectors included
class VertexDot3
{
public:
	VertexDot3(const D3DXVECTOR3& _Position, const D3DXVECTOR3& _Normal, const D3DXVECTOR2& _Texture)
		: Position(_Position), 
			Normal(_Normal),
			Texture(_Texture)
	{};

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 Diffuse;
	D3DXVECTOR2 Texture;
	D3DXVECTOR3 S;
	D3DXVECTOR3 T;
    D3DXVECTOR3 SxT;
};


class VertexDot3_Decl
{
#define VDOT3SZ		9


private:
	NV_VERTEXDECLTYPE m_pDecl[VDOT3SZ];
	NV_VERTEXDECLTYPE m_pCgDecl[VDOT3SZ];
public:
	NV_VERTEXDECLTYPE* GetVShaderDeclaration() { return( m_pDecl ); };
	NV_VERTEXDECLTYPE* GetCgDeclaration() { return( m_pCgDecl ); };

	DWORD	GetFVF()  { assert( false ); 
						// No FVF is possible for the 3-float color
						// But we could do 2 texture coords, with the first
						//  having 3 floats.  Still no FVF should be needed
						return( 0 ); };

#ifdef NV_USING_D3D8
	VertexDot3_Decl()
	{
		int i=0;
		m_pDecl[i++] = D3DVSD_STREAM(0);
		m_pDecl[i++] = D3DVSD_REG( 0, D3DVSDT_FLOAT3  );		// Pos
		m_pDecl[i++] = D3DVSD_REG( 1, D3DVSDT_FLOAT3  );		// Nrm
		m_pDecl[i++] = D3DVSD_REG( 2, D3DVSDT_FLOAT3  );		// Color ** 3 floats!
		m_pDecl[i++] = D3DVSD_REG( 3, D3DVSDT_FLOAT2  );		// Tex
		m_pDecl[i++] = D3DVSD_REG( 4, D3DVSDT_FLOAT3  );		// S
		m_pDecl[i++] = D3DVSD_REG( 5, D3DVSDT_FLOAT3  );		// T
		m_pDecl[i++] = D3DVSD_REG( 6, D3DVSDT_FLOAT3  );		// SxT
		m_pDecl[i++] = D3DVSD_END();

		i=0;
		m_pCgDecl[i++] = D3DVSD_STREAM(0);
		m_pCgDecl[i++] = D3DVSD_REG( CGDX8_POSITION, D3DVSDT_FLOAT3  );		// Pos
		m_pCgDecl[i++] = D3DVSD_REG( CGDX8_NORMAL, D3DVSDT_FLOAT3  );		// Nrm
		m_pCgDecl[i++] = D3DVSD_REG( CGDX8_DIFFUSE, D3DVSDT_FLOAT3  );		// Color ** 3 floats!
		m_pCgDecl[i++] = D3DVSD_REG( CGDX8_TEXCOORD0, D3DVSDT_FLOAT2  );	// Tex
		m_pCgDecl[i++] = D3DVSD_REG( CGDX8_TEXCOORD1, D3DVSDT_FLOAT3  );		// S
		m_pCgDecl[i++] = D3DVSD_REG( CGDX8_TEXCOORD2, D3DVSDT_FLOAT3  );		// T
		m_pCgDecl[i++] = D3DVSD_REG( CGDX8_TEXCOORD3, D3DVSDT_FLOAT3  );		// SxT
		m_pCgDecl[i++] = D3DVSD_END();

	};
#endif

#ifdef NV_USING_D3D9
	VertexDot3_Decl()
	{
		// assumes D3DDECL_END takes 1 D3DVERTEXELEMENT9

        D3DVERTEXELEMENT9 decl[] =
        {
			// 2nd param is offset in bytes
            { 0, 0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
            { 0, 12,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0 },
            { 0, 24,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,		0 },
			{ 0, 36,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },

			{ 0, 48,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	1 },
			{ 0, 60,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0 },
			{ 0, 72,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,	0 },

            D3DDECL_END()
        };

		int i;
		for( i=0; i < VDOT3SZ; i++ )
		{
			m_pDecl[i] = decl[i];
		}
	};
#endif


	~VertexDot3_Decl() {};
};



///////////


class GeometryVB_Dot3	: public SimpleVBObject
{

public:

	virtual HRESULT CreateFromSimpleObject( SimpleObject * pObj, NV_D3DDEVICETYPE*  pD3DDev );


	// Creates basis vectors for index triangle lists.
	HRESULT CreateBasisVectors(	NV_VERTEXBUFFERTYPE* pVertexBuffer,
								NV_INDEXBUFFERTYPE*	 pIndexBuffer );

	HRESULT FindAndFixDegenerateVertexBasis(	VertexDot3	* pVertices,
												DWORD		  dwNumVertices,
												void		* pIndices,
												DWORD		  dwNumIndices,
												bool	bUnifyNormals,
												bool	is32BitIndices);


	GeometryVB_Dot3();
	virtual ~GeometryVB_Dot3();

};




#endif					// __NV_GEOVB_DOT3_89_H__

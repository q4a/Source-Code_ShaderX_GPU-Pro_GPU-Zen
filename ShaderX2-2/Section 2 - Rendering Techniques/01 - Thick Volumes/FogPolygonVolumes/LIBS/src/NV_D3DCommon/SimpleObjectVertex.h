/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  SimpleObjectVertex.h

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


#ifndef __SimpleObjectVertex_89_H__
#define __SimpleObjectVertex_89_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <NVD3DVers.h>
#include <shared/CgD3DSemantics.h>


//////////////////////////////////////////////////////////

typedef struct SimpleObjVertex
{
	D3DXVECTOR3		pos;
	D3DXVECTOR3		nrm;

    D3DCOLOR		diffuse;		// ARGB
    D3DXVECTOR2		t0;
    
} SimpleObjVertex; 



#define SOBJVERT_FVF  (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL |	\
						D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0)  )	
							

#define SOVDS	6


class SimpleObjectVertexDecl
{
private:
	NV_VERTEXDECLTYPE m_pDecl[SOVDS];
	NV_VERTEXDECLTYPE m_pCgDecl[SOVDS];

public:

	NV_VERTEXDECLTYPE * GetVShaderDeclaration() { return( m_pDecl ); };
	NV_VERTEXDECLTYPE * GetCgDeclaration() { return( m_pCgDecl ); };

	DWORD	GetFVF()  { return( SOBJVERT_FVF ); };


#ifdef NV_USING_D3D8
	SimpleObjectVertexDecl()
	{
		m_pDecl[0] = D3DVSD_STREAM(0);
		m_pDecl[1] = D3DVSD_REG( 0, D3DVSDT_FLOAT3  );	// Pos
		m_pDecl[2] = D3DVSD_REG( 1, D3DVSDT_FLOAT3  );	// Normal
		m_pDecl[3] = D3DVSD_REG( 2, D3DVSDT_D3DCOLOR);	// Diffuse
		m_pDecl[4] = D3DVSD_REG( 3, D3DVSDT_FLOAT2  );	// Texture
		m_pDecl[5] = D3DVSD_END();

		m_pCgDecl[0] = D3DVSD_STREAM(0);
		m_pCgDecl[1] = D3DVSD_REG( D3DVSDE_POSITION,    D3DVSDT_FLOAT3  );	// Pos
		m_pCgDecl[2] = D3DVSD_REG( D3DVSDE_NORMAL,	    D3DVSDT_FLOAT3  );	// Normal
		m_pCgDecl[3] = D3DVSD_REG( D3DVSDE_DIFFUSE,	    D3DVSDT_D3DCOLOR);	// Diffuse
		m_pCgDecl[4] = D3DVSD_REG( D3DVSDE_TEXCOORD0,	D3DVSDT_FLOAT2  );	// Texture
		m_pCgDecl[5] = D3DVSD_END();


	};
#else
#ifdef NV_USING_D3D9
	SimpleObjectVertexDecl()
	{
		// assumes D3DDECL_END takes 1 D3DVERTEXELEMENT9

        D3DVERTEXELEMENT9 decl[] =
        {
            { 0, 0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
            { 0, 12,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0 },
            { 0, 24,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,		0 },
			{ 0, 28,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
            D3DDECL_END()
        };

		int i;
		for( i=0; i < SOVDS; i++ )
		{
			m_pDecl[i] = decl[i];
		}
	};
#endif
#endif


	~SimpleObjectVertexDecl() {};
};


#endif 


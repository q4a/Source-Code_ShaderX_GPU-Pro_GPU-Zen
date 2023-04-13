/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  GeometryVB_Color.h

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
Vertex buffer object with color only 


******************************************************************************/



#ifndef __NV_GEOVB_COLOR_H
#define __NV_GEOVB_COLOR_H


#include <NVD3DVers.h>
#include <NV_D3DCommon/SimpleVBObject.h>

#include <shared/CgD3DSemantics.h>



/////////////////////////////////////////////////
// Vertex with color only

struct VertexColor
{
	D3DCOLOR	Color;
};


class VertexColor_Decl
{

#define VPCDSZ	3

private:
	NV_VERTEXDECLTYPE m_pDecl[VPCDSZ];
	NV_VERTEXDECLTYPE m_pCgDecl[VPCDSZ];
public:
	NV_VERTEXDECLTYPE * GetVShaderDeclaration() { return( m_pDecl ); };
	NV_VERTEXDECLTYPE * GetCgDeclaration()		{ return( m_pCgDecl ); };
	DWORD	GetFVF()  { return(
								D3DFVF_DIFFUSE
								);  };

#ifdef NV_USING_D3D8
	VertexColor_Decl()
	{
		int i=0;
		m_pDecl[i++] = D3DVSD_STREAM(0);
		m_pDecl[i++] = D3DVSD_REG(0, D3DVSDT_D3DCOLOR  );
		m_pDecl[i++] = D3DVSD_END();

		i=0;
		m_pCgDecl[i++] = D3DVSD_STREAM(0);
		m_pCgDecl[i++] = D3DVSD_REG( CGDX8_DIFFUSE, D3DVSDT_D3DCOLOR );	// Color
		m_pCgDecl[i++] = D3DVSD_END();

	};
#else
#ifdef NV_USING_D3D9
	VertexColor_Decl()
	{
		// assumes D3DDECL_END takes 1 D3DVERTEXELEMENT9

        D3DVERTEXELEMENT9 decl[] =
        {
            { 0, 0,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,		0 },
            D3DDECL_END()
        };

		int i;
		for( i=0; i < VPCDSZ; i++ )
		{
			m_pDecl[i] = decl[i];
		}
	};
#endif
#endif



	~VertexColor_Decl() {};
};


class GeometryVB_Color	: public SimpleVBObject
{

public:

	virtual HRESULT CreateFromSimpleObject( SimpleObject * pObj, 
											NV_D3DDEVICETYPE*  pD3DDev );

	GeometryVB_Color();
	virtual ~GeometryVB_Color();
};


#endif

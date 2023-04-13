//-----------------------------------------------------------------------------
// File: CompressedMesh.cpp
//
// Desc: code for vertex codec's
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <cassert>
#include <D3D9types.h>
#include <D3DX9.h>
#include <DXErr9.h>
#include "DXUtil.h"
#include "D3DUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"

#include "CompressedMesh.h"

// the eigen solver
extern void tri_diag(double a[], double d[], double e[], double z[], int n,double tol);
extern int calc_eigenstructure(double d[],double e[],double z[], int n, double macheps);

//-----------------------------------------------------------------------------
// Name: ctor
// Desc:
//-----------------------------------------------------------------------------
CompressedMesh::CompressedMesh(  LPDIRECT3DDEVICE9 pD3D ) : 
	m_pD3D(pD3D),
	m_pVB(0),
	m_pIB(0),
	m_pDecl(0),
	m_vertexSize(0),
	m_numVertex(0),
	m_numIndices(0)
{
}

//-----------------------------------------------------------------------------
// Name: dtor
// Desc:
//-----------------------------------------------------------------------------
CompressedMesh::~CompressedMesh()
{
	SAFE_RELEASE( m_pIB );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pDecl );
}
//-----------------------------------------------------------------------------
// Name: SizeOfD3DVertexType
// Desc: size in bytes of a d3d vertex type
//-----------------------------------------------------------------------------
unsigned int CompressedMesh::SizeOfD3DVertexType( const unsigned int type )
{
	switch(type)
	{
	case D3DDECLTYPE_FLOAT1:
		return sizeof(float);
	case D3DDECLTYPE_FLOAT2:
		return sizeof(float) * 2;
	case D3DDECLTYPE_FLOAT3:
		return sizeof(float) * 3;
	case D3DDECLTYPE_FLOAT4:
		return sizeof(float) * 4;
	case D3DDECLTYPE_D3DCOLOR:
		return sizeof(unsigned char) * 4;
	case D3DDECLTYPE_UBYTE4:
		return sizeof(unsigned char) * 4;
	case D3DDECLTYPE_SHORT2:
		return sizeof(short) * 2;
	case D3DDECLTYPE_SHORT4:
		return sizeof(short) * 4;
// The following types are valid only with vertex shaders >= 2.0
// hmm I think this a doc mistake in Dx9? TODO Check with MS and ATI and NVdia
	case D3DDECLTYPE_UBYTE4N:	// Each of 4 bytes is normalized by dividing to 255.0
		return sizeof(unsigned char) * 4;
	case D3DDECLTYPE_SHORT2N:	// 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
		return sizeof(short) * 2;
	case D3DDECLTYPE_SHORT4N:	// 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
		return sizeof(short) * 4;
	case D3DDECLTYPE_USHORT2N:  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
		return sizeof(short) * 2;
	case D3DDECLTYPE_USHORT4N:  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
		return sizeof(short) * 4;
	case D3DDECLTYPE_UDEC3:		// 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
		return sizeof(unsigned int);
	case D3DDECLTYPE_DEC3N:		// 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
		return sizeof(unsigned int);
	case D3DDECLTYPE_FLOAT16_2: // Two 16-bit floating point values, expanded to (value, value, 0, 1)
		return sizeof(short) * 2;
	case D3DDECLTYPE_FLOAT16_4:	// Four 16-bit floating point values
		return sizeof(short) * 4;

	default:
		assert( false );
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: Draw
// Desc: Set the vertex buffer and index streams and then draw indexed primitive
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::Draw()
{
	HRESULT hRes;
	hRes = m_pD3D->SetStreamSource( 0, m_pVB, 0, m_vertexSize );
	if( FAILED(hRes) ) return hRes;
	hRes = m_pD3D->SetIndices( m_pIB );
	if( FAILED(hRes) ) return hRes;
	hRes = m_pD3D->SetVertexDeclaration( m_pDecl );
	if( FAILED(hRes) ) return hRes;

	hRes = m_pD3D->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_numVertex, 0, m_numIndices/3 );
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

HRESULT CompressedMesh::CopyIndexBuffer( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	m_numIndices = in->GetNumFaces() * 3;

	const unsigned int ibsize = m_numIndices * sizeof(WORD);
	// copy index buffer (16 bit only)
	hRes = m_pD3D->CreateIndexBuffer( ibsize, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL );
	if( FAILED(hRes) ) return hRes;

	WORD* inIBStream = 0;
	hRes = in->LockIndexBuffer( D3DLOCK_READONLY, (void**)&inIBStream);
	if( FAILED(hRes) ) return hRes;
	WORD* outIBStream = 0;
	hRes = m_pIB->Lock(0,0, (void**)&outIBStream, 0);
	if( FAILED(hRes) ) return hRes;

	memcpy(outIBStream, inIBStream, ibsize);

	// now unlock both index buffers
	hRes = m_pIB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockIndexBuffer();
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//--Compression functions -----------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: QuantiseNormal
// Desc: takes 3 floats and returns a quantised D3DCOLOR representing the input normal
//-----------------------------------------------------------------------------
unsigned int CompressedMesh::QuantiseNormal( const float nx, const float ny, const float nz)
{
	// -1.0 - 1.f -> 0.f - 255.f
	unsigned int ix = unsigned int( (nx * 127.f) + 128.f );
	unsigned int iy = unsigned int( (ny * 127.f) + 128.f );
	unsigned int iz = unsigned int( (nz * 127.f) + 128.f );
	unsigned int iw = 0; // we don't use the w component (just padding)

	// we pre-swap components due to use of D3DCOLOR (uses intel byte-handiness)
	unsigned int out = (iw << 24) | (ix << 16) | (iy << 8) | (iz << 0);

	return out;
}

//-----------------------------------------------------------------------------
// Name: QuantiseNormals
// Desc: takes a D3DXMesh and produces the same mesh with float position and quantised normals
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::QuantiseNormals( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	// retrieve the stream declaration of the input mesh
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	hRes = in->GetDeclaration( inDecl );
	if( FAILED(hRes) ) return hRes;

	// create a declaration D3D (fixed function registers)
	m_Decl[0].Stream	= 0;
	m_Decl[0].Offset	= 0;
	m_Decl[0].Type		= D3DDECLTYPE_FLOAT3;
	m_Decl[0].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[0].UsageIndex = 0;
	
	m_Decl[1].Stream	= 0;
	m_Decl[1].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_FLOAT3);
	m_Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[1].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[1].Usage		= D3DDECLUSAGE_NORMAL;
	m_Decl[1].UsageIndex = 0;
	
	// D3DDECL_END()
	m_Decl[2].Stream	= 0xFF;
	m_Decl[2].Offset	= 0;
	m_Decl[2].Type		= D3DDECLTYPE_UNUSED;
	m_Decl[2].Method	= 0;
	m_Decl[2].Usage		= 0;
	m_Decl[2].UsageIndex = 0;

	hRes = m_pD3D->CreateVertexDeclaration( m_Decl, &m_pDecl );
	if( FAILED(hRes) ) return hRes;

	m_vertexSize = SizeOfD3DVertexType(D3DDECLTYPE_FLOAT3) + SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_numVertex = in->GetNumVertices();
	const unsigned int vbSize = m_vertexSize * m_numVertex;

	hRes = m_pD3D->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	// copy position + quantise normals
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	float* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(unsigned int i=0;i < in->GetNumVertices();i++)
	{
		// copy 3 floats for position
		outStream[0] = inStream[0]; // pos.x
		outStream[1] = inStream[1]; // pos.y
		outStream[2] = inStream[2]; // pos.z

		// quantise normal
		float nx = inStream[3];
		float ny = inStream[4];
		float nz = inStream[5];
		unsigned int quant = QuantiseNormal( nx, ny, nz );
		((unsigned int*)outStream)[3] = quant;

		// next vertex
		inStream += 6;
		outStream += 4;
	}
	
	// now unlock both vertex buffers
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = CopyIndexBuffer(in);
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: ScaleAndOffsetPosition8bit
// Desc: takes a position vector and returns a D3DCOLOR scale and offset compressed version
//-----------------------------------------------------------------------------
unsigned int CompressedMesh::ScaleAndOffsetPosition8bit( const D3DXVECTOR3& in )
{
	D3DXVECTOR3 fv;
	
	fv[0] = (in[0] - m_soOffset[0]) / m_soScale[0];
	fv[1] = (in[1] - m_soOffset[1]) / m_soScale[1];
	fv[2] = (in[2] - m_soOffset[2]) / m_soScale[2];

	unsigned int ix = unsigned int (fv[0] * 255.f);
	unsigned int iy = unsigned int (fv[1] * 255.f);
	unsigned int iz = unsigned int (fv[2] * 255.f);
	unsigned int iw = 0; // don't use w component

	// we pre-swap components due to use of D3DCOLOR (uses intel byte-handiness)
	unsigned int out = (iw << 24) | (ix << 16) | (iy << 8) | (iz << 0);

	return out;
}

//-----------------------------------------------------------------------------
// Name: ScaleAndOffsetPosition
// Desc: takes a D3DXMesh and produces the same mesh with 8 bit scale and offset position 
//       and quantised normals
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::ScaleAndOffsetPosition8bit( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	unsigned int i;
	// retrieve the stream declaration of the input mesh
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	hRes = in->GetDeclaration( inDecl );
	if( FAILED(hRes) ) return hRes;

	// create a declaration D3D (fixed function registers)
	m_Decl[0].Stream	= 0;
	m_Decl[0].Offset	= 0;
	m_Decl[0].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[0].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[0].UsageIndex = 0;
	
	m_Decl[1].Stream	= 0;
	m_Decl[1].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[1].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[1].Usage		= D3DDECLUSAGE_NORMAL;
	m_Decl[1].UsageIndex = 0;
	
	// D3DDECL_END()
	m_Decl[2].Stream	= 0xFF;
	m_Decl[2].Offset	= 0;
	m_Decl[2].Type		= D3DDECLTYPE_UNUSED;
	m_Decl[2].Method	= 0;
	m_Decl[2].Usage		= 0;
	m_Decl[2].UsageIndex = 0;

	hRes = m_pD3D->CreateVertexDeclaration( m_Decl, &m_pDecl );
	if( FAILED(hRes) ) return hRes;

	m_vertexSize = SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR) + SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_numVertex = in->GetNumVertices();
	const unsigned int vbSize = m_vertexSize * m_numVertex;

	hRes = m_pD3D->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX identity;
	D3DXMatrixIdentity( &identity );
	// determinte scale and offset
	hRes = DetermineScaleAndOffset(in, identity, m_soScale, m_soOffset );
	if( FAILED(hRes) ) return hRes;

	// scale and offset position + copy normals
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	float* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(i=0;i < in->GetNumVertices();i++)
	{
		// scale and offset position
		D3DXVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		unsigned int so = ScaleAndOffsetPosition8bit( pos );
		((unsigned int*)outStream)[0] = so;

		// quantise normals
		((unsigned int*)outStream)[1] = QuantiseNormal(inStream[3], inStream[4], inStream[5]);
		// next vertex
		inStream += 6;
		outStream += 2;
	}
	
	// now unlock both vertex buffers
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = CopyIndexBuffer(in);
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ScaleAndOffsetPosition16bit
// Desc: takes a position vector and returns a D3DCOLOR scale and offset compressed version
//-----------------------------------------------------------------------------
void CompressedMesh::ScaleAndOffsetPosition16bit( const D3DXVECTOR3& in, 
														 short& x, short& y, short& z, short& w)
{
	D3DXVECTOR3 fv;
	
	fv[0] = (in[0] - m_soOffset[0]) / m_soScale[0];
	fv[1] = (in[1] - m_soOffset[1]) / m_soScale[1];
	fv[2] = (in[2] - m_soOffset[2]) / m_soScale[2];

	x = short( (fv[0]-0.5f) * 65535.f);
	y = short( (fv[1]-0.5f) * 65535.f);
	z = short( (fv[2]-0.5f) * 65535.f);
	w = short(-0.5f * 65535.f); // 0

}

//-----------------------------------------------------------------------------
// Name: ScaleAndOffsetPosition
// Desc: takes a D3DXMesh and produces the same mesh with 16 bit scale and offset position 
//       and quantised normals
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::ScaleAndOffsetPosition16bit( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	unsigned int i;
	// retrieve the stream declaration of the input mesh
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	hRes = in->GetDeclaration( inDecl );
	if( FAILED(hRes) ) return hRes;

	// create a declaration D3D (fixed function registers)
	m_Decl[0].Stream	= 0;
	m_Decl[0].Offset	= 0;
	m_Decl[0].Type		= D3DDECLTYPE_SHORT4;
	m_Decl[0].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[0].UsageIndex = 0;
	
	m_Decl[1].Stream	= 0;
	m_Decl[1].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_SHORT4);
	m_Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[1].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[1].Usage		= D3DDECLUSAGE_NORMAL;
	m_Decl[1].UsageIndex = 0;
	
	// D3DDECL_END()
	m_Decl[2].Stream	= 0xFF;
	m_Decl[2].Offset	= 0;
	m_Decl[2].Type		= D3DDECLTYPE_UNUSED;
	m_Decl[2].Method	= 0;
	m_Decl[2].Usage		= 0;
	m_Decl[2].UsageIndex = 0;

	hRes = m_pD3D->CreateVertexDeclaration( m_Decl, &m_pDecl );
	if( FAILED(hRes) ) return hRes;

	m_vertexSize = SizeOfD3DVertexType(D3DDECLTYPE_SHORT4) + SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_numVertex = in->GetNumVertices();
	const unsigned int vbSize = m_vertexSize * m_numVertex;

	hRes = m_pD3D->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX identity;
	D3DXMatrixIdentity( &identity );
	// determinte scale and offset
	hRes = DetermineScaleAndOffset(in, identity, m_soScale, m_soOffset );
	if( FAILED(hRes) ) return hRes;

	// scale and offset position + copy normals
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	float* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(i=0;i < in->GetNumVertices();i++)
	{
		// scale and offset position
		D3DXVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		short x,y,z,w;
		ScaleAndOffsetPosition16bit( pos, x, y, z, w );
		((short*)outStream)[0] = x;
		((short*)outStream)[1] = y;
		((short*)outStream)[2] = z;
		((short*)outStream)[3] = w;

		// quantise normals
		((unsigned int*)outStream)[2] = QuantiseNormal(inStream[3], inStream[4], inStream[5]);

		// next vertex
		inStream += 6;
		outStream += 3;
	}
	
	// now unlock both vertex buffers
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = CopyIndexBuffer(in);
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::DetermineScaleAndOffset(	ID3DXBaseMesh* in, D3DXMATRIX& transform,
												D3DXVECTOR3& scale, 
												D3DXVECTOR3& offset )
{
	HRESULT hRes;
	unsigned int i;

	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	D3DXVECTOR3 lowerRange, upperRange;
	lowerRange[0] = FLT_MAX;
	lowerRange[1] = FLT_MAX;
	lowerRange[2] = FLT_MAX;
	upperRange[0] = -FLT_MAX;
	upperRange[1] = -FLT_MAX;
	upperRange[2] = -FLT_MAX;

	// determine scale and offset	
	for(i=0;i < in->GetNumVertices();i++)
	{
		D3DXVECTOR3 vec(inStream[0], inStream[1], inStream[2]);
		D3DXVec3TransformCoord( &vec, &vec, &transform );

		lowerRange[0] = min(vec[0], lowerRange[0]);
		lowerRange[1] = min(vec[1], lowerRange[1]);
		lowerRange[2] = min(vec[2], lowerRange[2]);

		upperRange[0] = max(vec[0], upperRange[0]);
		upperRange[1] = max(vec[1], upperRange[1]);
		upperRange[2] = max(vec[2], upperRange[2]);

		// next vertex
		inStream += 6;
	}

	offset = lowerRange;
	scale = upperRange - lowerRange;

	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

HRESULT CompressedMesh::DetermineRotationMatrix( ID3DXBaseMesh* in, D3DXMATRIX& matrix)
{
	HRESULT hRes;
	unsigned int i,j,k;

	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	// lock index data
	WORD* inIBStream = 0;
	hRes = in->LockIndexBuffer( D3DLOCK_READONLY, (void**)&inIBStream);
	if( FAILED(hRes) ) return hRes;

	double sum[3] = {0,0,0};
	unsigned int n = in->GetNumFaces();

	// sum the position data from each triangle
	for(i=0;i < n;i++)
	{
		unsigned int i0 = inIBStream[(i * 3) + 0];
		unsigned int i1 = inIBStream[(i * 3) + 1];
		unsigned int i2 = inIBStream[(i * 3) + 2];

		sum[0] += inStream[(i0 * 6) + 0];
		sum[1] += inStream[(i0 * 6) + 1];
		sum[2] += inStream[(i0 * 6) + 2];

		sum[0] += inStream[(i1 * 6) + 0];
		sum[1] += inStream[(i1 * 6) + 1];
		sum[2] += inStream[(i1 * 6) + 2];

		sum[0] += inStream[(i2 * 6) + 0];
		sum[1] += inStream[(i2 * 6) + 1];
		sum[2] += inStream[(i2 * 6) + 2];
	}

	// calculate u
	double u[3];
	u[0] = (1.f/(3.f * n)) * sum[0];
	u[1] = (1.f/(3.f * n)) * sum[1];
	u[2] = (1.f/(3.f * n)) * sum[2];

	double R[3][3]; // doubles will help due to numerical precision issues

	// calculate rotation matrix
	for(i=0;i < n;i++)
	{
		unsigned int i0 = inIBStream[(i * 3) + 0];
		unsigned int i1 = inIBStream[(i * 3) + 1];
		unsigned int i2 = inIBStream[(i * 3) + 2];

		double p[3],q[3],r[3];

		p[0] = inStream[(i0 * 6) + 0] - u[0];
		p[1] = inStream[(i0 * 6) + 1] - u[1];
		p[2] = inStream[(i0 * 6) + 2] - u[2];

		q[0] = inStream[(i1 * 6) + 0] - u[0];
		q[1] = inStream[(i1 * 6) + 1] - u[1];
		q[2] = inStream[(i1 * 6) + 2] - u[2];

		r[0] = inStream[(i2 * 6) + 0] - u[0];
		r[1] = inStream[(i2 * 6) + 1] - u[1];
		r[2] = inStream[(i2 * 6) + 2] - u[2];

		for(unsigned int j = 0; j < 3; j++)
		{
			for(unsigned int k = 0; k < 3; k++)
			{
				R[j][k] =	(p[j]*p[k]) + 
							(q[j]*q[k]) +
							(r[j]*r[k]);
			}
		}
	}

	// average rotation sums
	for(j = 0; j < 3; j++)
	{
		for(k = 0; k < 3; k++)
		{
			R[j][k] = R[j][k] * (1.f/(3.f * n));
		}
	}

	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = in->UnlockIndexBuffer();
	if( FAILED(hRes) ) return hRes;

	// convert matrix
	double a[10],v[10];
	for(j = 0; j < 3; j++)
	{
		for(k = 0; k < 3; k++)
		{
			a[j * 3 + k+1] = R[j][k];
		}
	}

	// run it through an eigensolver
	// originally I used jacobi from Numerical Receipes, but that can't be
	// distributed, so I use this version.

	// tol and machineprecision are so high, because cube row taxes numerical precision
	// for 'proper' work at the scale of cube row, its time to dig out the numerical analsyse 
	// texts
	double ld[4], le[4];
	double tol = 1.e-30;
	tri_diag(a,ld,le,v,3,tol);

	double macheps = 1.e-30 ;//1.e-16;
	if( calc_eigenstructure(ld,le,v,3,macheps) == -1)
	{
		return E_FAIL;
	}

	// fill in D3D Matrix
	D3DXMatrixIdentity( &matrix );
	matrix._11 = (float)v[1]; matrix._21 = (float)v[2];	matrix._31 = (float)v[3];
	matrix._12 = (float)v[4]; matrix._22 = (float)v[5];	matrix._32 = (float)v[6];
	matrix._13 = (float)v[7]; matrix._23 = (float)v[8];	matrix._33 = (float)v[9];

	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition8bit
// Desc: takes a position vector and returns a D3DCOLOR scale and offset compressed version
//-----------------------------------------------------------------------------
unsigned int CompressedMesh::CompressionTransformPosition8bit( const D3DXVECTOR3& in )
{
	D3DXVECTOR3 fv;
	
	D3DXVec3TransformCoord( &fv, &in, &m_ctMatrix );

	unsigned int ix = unsigned int (fv[0] * 255.f);
	unsigned int iy = unsigned int (fv[1] * 255.f);
	unsigned int iz = unsigned int (fv[2] * 255.f);
	unsigned int iw = 255; // we need w = 1 for 4x4 transform

	// we pre-swap components due to use of D3DCOLOR (uses intel byte-handiness)
	unsigned int out = (iw << 24) | (ix << 16) | (iy << 8) | (iz << 0);

	return out;
}

//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition8bit
// Desc: takes a D3DXMesh and produces the same mesh with 8 bit compression transform
//       and quantised normals
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::CompressionTransformPosition8bit( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	unsigned int i;
	// retrieve the stream declaration of the input mesh
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	hRes = in->GetDeclaration( inDecl );
	if( FAILED(hRes) ) return hRes;

	// create a declaration D3D (fixed function registers)
	m_Decl[0].Stream	= 0;
	m_Decl[0].Offset	= 0;
	m_Decl[0].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[0].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[0].UsageIndex = 0;
	
	m_Decl[1].Stream	= 0;
	m_Decl[1].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[1].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[1].Usage		= D3DDECLUSAGE_NORMAL;
	m_Decl[1].UsageIndex = 0;
	
	// D3DDECL_END()
	m_Decl[2].Stream	= 0xFF;
	m_Decl[2].Offset	= 0;
	m_Decl[2].Type		= D3DDECLTYPE_UNUSED;
	m_Decl[2].Method	= 0;
	m_Decl[2].Usage		= 0;
	m_Decl[2].UsageIndex = 0;

	hRes = m_pD3D->CreateVertexDeclaration( m_Decl, &m_pDecl );
	if( FAILED(hRes) ) return hRes;

	m_vertexSize = SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR) + SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_numVertex = in->GetNumVertices();
	const unsigned int vbSize = m_vertexSize * m_numVertex;

	hRes = m_pD3D->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX rotMat;
	D3DXMatrixIdentity( &rotMat );

	hRes = DetermineRotationMatrix(in, rotMat);
	if( FAILED(hRes) ) return hRes;

	D3DXMatrixInverse( &rotMat, 0, &rotMat);

	D3DXVECTOR3 scale, offset;
	hRes = DetermineScaleAndOffset(in, rotMat, scale, offset);
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX scaleMat, offsetMat;
	D3DXMatrixScaling( &scaleMat, scale[0], scale[1], scale[2] );
	D3DXMatrixTranslation( &offsetMat, offset[0], offset[1], offset[2] );

	D3DXMatrixInverse( &scaleMat, 0, &scaleMat);
	D3DXMatrixInverse( &offsetMat, 0, &offsetMat);

	// m = r-1 * o-1 * s-1
	D3DXMATRIX combined;
	D3DXMatrixMultiply( &combined, &offsetMat, &scaleMat );
	D3DXMatrixMultiply( &combined, &rotMat, &combined );

	m_ctMatrix = combined;

	// transform position into compression space + copy normals
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	float* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(i=0;i < in->GetNumVertices();i++)
	{
		// scale and offset position
		D3DXVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		unsigned int ct = CompressionTransformPosition8bit( pos );
		((unsigned int*)outStream)[0] = ct;

		// quantise normals
		((unsigned int*)outStream)[1] = QuantiseNormal(inStream[3], inStream[4], inStream[5]);

		// next vertex
		inStream += 6;
		outStream += 2;
	}
	
	// now unlock both vertex buffers
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = CopyIndexBuffer(in);
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition16bit
// Desc: takes a position vector and returns a D3DCOLOR scale and offset compressed version
//-----------------------------------------------------------------------------
void CompressedMesh::CompressionTransformPosition16bit( const D3DXVECTOR3& in,
													    short& x, short& y, short&z, short& w)
{
	D3DXVECTOR3 fv;
	
	D3DXVec3TransformCoord( &fv, &in, &m_ctMatrix );

	x = short ( (fv[0]-0.5f) * 65535.f);
	y = short ( (fv[1]-0.5f) * 65535.f);
	z = short ( (fv[2]-0.5f) * 65535.f);
	w = 32767; // we need w = 1 for 4x4 transform

}

//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition16bit
// Desc: takes a D3DXMesh and produces the same mesh with 16 bit compression transform
//       and quantised normals
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::CompressionTransformPosition16bit( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	unsigned int i;
	// retrieve the stream declaration of the input mesh
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	hRes = in->GetDeclaration( inDecl );
	if( FAILED(hRes) ) return hRes;


	// create a declaration D3D (fixed function registers)
	m_Decl[0].Stream	= 0;
	m_Decl[0].Offset	= 0;
	m_Decl[0].Type		= D3DDECLTYPE_SHORT4;
	m_Decl[0].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[0].UsageIndex = 0;
	
	m_Decl[1].Stream	= 0;
	m_Decl[1].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_SHORT4);
	m_Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[1].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[1].Usage		= D3DDECLUSAGE_NORMAL;
	m_Decl[1].UsageIndex = 0;
	
	// D3DDECL_END()
	m_Decl[2].Stream	= 0xFF;
	m_Decl[2].Offset	= 0;
	m_Decl[2].Type		= D3DDECLTYPE_UNUSED;
	m_Decl[2].Method	= 0;
	m_Decl[2].Usage		= 0;
	m_Decl[2].UsageIndex = 0;

	hRes = m_pD3D->CreateVertexDeclaration( m_Decl, &m_pDecl );
	if( FAILED(hRes) ) return hRes;

	m_vertexSize = SizeOfD3DVertexType(D3DDECLTYPE_SHORT4) + SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_numVertex = in->GetNumVertices();
	const unsigned int vbSize = m_vertexSize * m_numVertex;

	hRes = m_pD3D->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX rotMat;
	D3DXMatrixIdentity( &rotMat );

	hRes = DetermineRotationMatrix(in, rotMat);
	if( FAILED(hRes) ) return hRes;

	D3DXMatrixInverse( &rotMat, 0, &rotMat);

	D3DXVECTOR3 scale, offset;
	hRes = DetermineScaleAndOffset(in, rotMat, scale, offset);
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX scaleMat, offsetMat;
	D3DXMatrixScaling( &scaleMat, scale[0], scale[1], scale[2] );
	D3DXMatrixTranslation( &offsetMat, offset[0], offset[1], offset[2] );

	D3DXMatrixInverse( &scaleMat, 0, &scaleMat);
	D3DXMatrixInverse( &offsetMat, 0, &offsetMat);

	// m = r * o-1 * s-1
	D3DXMATRIX combined;
	D3DXMatrixMultiply( &combined, &offsetMat, &scaleMat );
	D3DXMatrixMultiply( &combined, &rotMat, &combined );

	m_ctMatrix = combined;

	// transform position into compression space + copy normals
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	float* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(i=0;i < in->GetNumVertices();i++)
	{
		// scale and offset position
		D3DXVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		short x,y,z,w;
		CompressionTransformPosition16bit( pos, x, y, z, w );
		((short*)outStream)[0] = x;
		((short*)outStream)[1] = y;
		((short*)outStream)[2] = z;
		((short*)outStream)[3] = w;

		// quantise normals
		((unsigned int*)outStream)[2] = QuantiseNormal(inStream[3], inStream[4], inStream[5]);

		// next vertex
		inStream += 6;
		outStream += 3;
	}
	
	// now unlock both vertex buffers
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = CopyIndexBuffer(in);
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SlidingCompressionTransformPosition16bit
// Desc: takes a position vector and returns a D3DCOLOR compressed version
//-----------------------------------------------------------------------------
void CompressedMesh::SlidingCompressionTransformPosition16bit( const D3DXVECTOR3& pos,
															   const D3DXVECTOR3& norm,
																unsigned int* outStream)
{
	D3DXVECTOR3 out;
	
	D3DXVec3TransformCoord( &out, &pos, &m_ctMatrix );

	unsigned int x,y,z;

	// we have to use double as float only have 23bit precision and we require 24!
	double tx,ty,tz;

	tx = double(out.x) * 8388607.0; 
	ty = double(out.y) * 8388607.0; 
	tz = double(out.z) * 8388607.0;

	// we have 24 bits of precision
	x = unsigned int ( tx );
	y = unsigned int ( ty );
	z = unsigned int ( tz );

	// split position into from a 24 bit integer into 3 8 bit integer
	unsigned int ilx = (x & 0x0000FF) >> 0;
	unsigned int imx = (x & 0x00FF00) >> 8;
	unsigned int ihx = (x & 0xFF0000) >> 16;
	unsigned int ily = (y & 0x0000FF) >> 0;
	unsigned int imy = (y & 0x00FF00) >> 8;
	unsigned int ihy = (y & 0xFF0000) >> 16;
	unsigned int ilz = (z & 0x0000FF) >> 0;
	unsigned int imz = (z & 0x00FF00) >> 8;
	unsigned int ihz = (z & 0xFF0000) >> 16;

	// normals -1.0 - 1.f -> 0.f - 255.f
	unsigned int inx = unsigned int( (norm.x * 127.f) + 128.f );
	unsigned int iny = unsigned int( (norm.y * 127.f) + 128.f );
	unsigned int inz = unsigned int( (norm.z * 127.f) + 128.f );

	// we pre-swap components due to use of D3DCOLOR (uses intel byte-handiness)
	outStream[0] = (inx << 24) | (ilx << 16) | (ily << 8) | (ilz << 0);
	outStream[1] = (iny << 24) | (imx << 16) | (imy << 8) | (imz << 0);
	outStream[2] = (inz << 24) | (ihx << 16) | (ihy << 8) | (ihz << 0);

}


//-----------------------------------------------------------------------------
// Name: SlidingCompressionTransformPosition16bit
// Desc: takes a D3DXMesh and produces the same mesh with 16 bit compression transform
//       and quantised normals
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::SlidingCompressionTransformPosition16bit( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	unsigned int i;
	// retrieve the stream declaration of the input mesh
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	hRes = in->GetDeclaration( inDecl );
	if( FAILED(hRes) ) return hRes;

	// create a declaration D3D 
	m_Decl[0].Stream	= 0;
	m_Decl[0].Offset	= 0;
	m_Decl[0].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[0].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[0].UsageIndex = 0;
	
	m_Decl[1].Stream	= 0;
	m_Decl[1].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[1].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[1].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[1].UsageIndex = 1;

	m_Decl[2].Stream	= 0;
	m_Decl[2].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR) + SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_Decl[2].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[2].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[2].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[2].UsageIndex = 2;

		// D3DDECL_END()
	m_Decl[3].Stream	= 0xFF;
	m_Decl[3].Offset	= 0;
	m_Decl[3].Type		= D3DDECLTYPE_UNUSED;
	m_Decl[3].Method	= 0;
	m_Decl[3].Usage		= 0;
	m_Decl[3].UsageIndex = 0;

	hRes = m_pD3D->CreateVertexDeclaration( m_Decl, &m_pDecl );
	if( FAILED(hRes) ) return hRes;

	m_vertexSize =	SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR) + 
					SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR) +
					SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_numVertex = in->GetNumVertices();
	const unsigned int vbSize = m_vertexSize * m_numVertex;

	hRes = m_pD3D->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX rotMat;
	D3DXMatrixIdentity( &rotMat );

	hRes = DetermineRotationMatrix(in, rotMat);
	if( FAILED(hRes) ) return hRes;

	D3DXMatrixInverse( &rotMat, 0, &rotMat);

	D3DXVECTOR3 scale, offset;
	hRes = DetermineScaleAndOffset(in, rotMat, scale, offset);
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX scaleMat, offsetMat;
	D3DXMatrixScaling( &scaleMat, scale[0], scale[1], scale[2] );
	D3DXMatrixTranslation( &offsetMat, offset[0], offset[1], offset[2] );

	D3DXMatrixInverse( &scaleMat, 0, &scaleMat);
	D3DXMatrixInverse( &offsetMat, 0, &offsetMat);

	// m = r-1 * o-1 * s-1
	D3DXMATRIX combined;
	D3DXMatrixMultiply( &combined, &offsetMat, &scaleMat );
	D3DXMatrixMultiply( &combined, &rotMat, &combined );

	m_ctMatrix = combined;

	// transform position into compression space + copy normals
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	unsigned int* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(i=0;i < in->GetNumVertices();i++)
	{
		// scale and offset position
		D3DXVECTOR3 pos( inStream[0], inStream[1], inStream[2] );
		D3DXVECTOR3 norm( inStream[3], inStream[4], inStream[5] );

		SlidingCompressionTransformPosition16bit( pos, norm, outStream );

		// next vertex
		inStream += 6;
		outStream += 3;
	}
	
	// now unlock both vertex buffers
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = CopyIndexBuffer(in);
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CompressionTransformPositionDEC3N
// Desc: takes a position vector and returns a 32 bit integer compressed version
//-----------------------------------------------------------------------------
unsigned int CompressedMesh::CompressionTransformPositionDEC3N( const D3DXVECTOR3& in )
{
	D3DXVECTOR3 fv;
	
	D3DXVec3TransformCoord( &fv, &in, &m_ctMatrix );

   int x = (int) ((fv[0]-0.5f) * 1023.f);
   int y = (int) ((fv[1]-0.5f) * 1023.f);
   int z = (int) ((fv[2]-0.5f) * 1023.f);

   // these 2 bits are discarded by DEC3N
   unsigned int lost = 0x1;

   return (x & 0x3FF) | ((y & 0x3FF) << 10) | ((z & 0x3FF) << 20) | ((lost & 0x3) << 30);

}

//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition16bit
// Desc: takes a D3DXMesh and produces the same mesh with 16 bit compression transform
//       and quantised normals
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::CompressionTransformPositionDEC3N( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	unsigned int i;
	// retrieve the stream declaration of the input mesh
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	hRes = in->GetDeclaration( inDecl );
	if( FAILED(hRes) ) return hRes;

	// create a declaration D3D (fixed function registers)
	m_Decl[0].Stream	= 0;
	m_Decl[0].Offset	= 0;
	m_Decl[0].Type		= D3DDECLTYPE_DEC3N;
	m_Decl[0].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[0].UsageIndex = 0;
	
	m_Decl[1].Stream	= 0;
	m_Decl[1].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_DEC3N);
	m_Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[1].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[1].Usage		= D3DDECLUSAGE_NORMAL;
	m_Decl[1].UsageIndex = 0;
	
	// D3DDECL_END()
	m_Decl[2].Stream	= 0xFF;
	m_Decl[2].Offset	= 0;
	m_Decl[2].Type		= D3DDECLTYPE_UNUSED;
	m_Decl[2].Method	= 0;
	m_Decl[2].Usage		= 0;
	m_Decl[2].UsageIndex = 0;

	hRes = m_pD3D->CreateVertexDeclaration( m_Decl, &m_pDecl );
	if( FAILED(hRes) ) return hRes;

	m_vertexSize = SizeOfD3DVertexType(D3DDECLTYPE_DEC3N) + SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_numVertex = in->GetNumVertices();
	const unsigned int vbSize = m_vertexSize * m_numVertex;

	hRes = m_pD3D->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX rotMat;
	D3DXMatrixIdentity( &rotMat );

	hRes = DetermineRotationMatrix(in, rotMat);
	if( FAILED(hRes) ) return hRes;

	D3DXMatrixInverse( &rotMat, 0, &rotMat);

	D3DXVECTOR3 scale, offset;
	hRes = DetermineScaleAndOffset(in, rotMat, scale, offset);
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX scaleMat, offsetMat;
	D3DXMatrixScaling( &scaleMat, scale[0], scale[1], scale[2] );
	D3DXMatrixTranslation( &offsetMat, offset[0], offset[1], offset[2] );

	D3DXMatrixInverse( &scaleMat, 0, &scaleMat);
	D3DXMatrixInverse( &offsetMat, 0, &offsetMat);

	// m = r-1 * o-1 * s-1
	D3DXMATRIX combined = scaleMat;
	D3DXMatrixMultiply( &combined, &offsetMat, &combined);
	D3DXMatrixMultiply( &combined, &rotMat, &combined );

	m_ctMatrix = combined;

	// transform position into compression space + copy normals
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	float* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(i=0;i < in->GetNumVertices();i++)
	{
		// scale and offset position
		D3DXVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		unsigned int ct = CompressionTransformPositionDEC3N( pos );
		((unsigned int*)outStream)[0] = ct;

		// quantise normals
		((unsigned int*)outStream)[1] = QuantiseNormal(inStream[3], inStream[4], inStream[5]);

		// next vertex
		inStream += 6;
		outStream += 2;
	}
	
	// now unlock both vertex buffers
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = CopyIndexBuffer(in);
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CompressionTransformPositionDEC3N
// Desc: takes a position vector and returns a 32 bit integer compressed version
//-----------------------------------------------------------------------------
unsigned int CompressedMesh::CompressionTransformPosition101012( const D3DXVECTOR3& in )
{
	D3DXVECTOR3 fv;
	
	D3DXVec3TransformCoord( &fv, &in, &m_ctMatrix );

	int x = (int) (fv[0] * 1023.f); // 10 bits
	int y = (int) (fv[1] * 1023.f); // 10 bits
	int z = (int) (fv[2] * 4095.f); // 12 bits
	
	return (x & 0x3FF) | ((y & 0x3FF) << 10) | ((z & 0xFFF) << 20);
}


//-----------------------------------------------------------------------------
// Name: CreateSwapMatrix
// Desc: Creates a D3DXMATRIX that swaps axises
//-----------------------------------------------------------------------------
void CompressedMesh::CreateSwapMatrix( D3DXMATRIX& in, SW_AXIS xAxis, SW_AXIS yAxis, SW_AXIS zAxis )
{
	D3DXMatrixIdentity( &in );

	switch( xAxis )
	{
	case SWA_X:
		in._11 = 1.f;
		in._21 = 0.f;
		in._31 = 0.f;
		break;
	case SWA_Y:
		in._11 = 0.f;
		in._21 = 1.f;
		in._31 = 0.f;
		break;
	case SWA_Z:
		in._11 = 0.f;
		in._21 = 0.f;
		in._31 = 1.f;
		break;
	}

	switch( yAxis )
	{
	case SWA_X:
		in._12 = 1.f;
		in._22 = 0.f;
		in._32 = 0.f;
		break;
	case SWA_Y:
		in._12 = 0.f;
		in._22 = 1.f;
		in._32 = 0.f;
		break;
	case SWA_Z:
		in._12 = 0.f;
		in._22 = 0.f;
		in._32 = 1.f;
		break;
	}

	switch( zAxis )
	{
	case SWA_X:
		in._13 = 1.f;
		in._23 = 0.f;
		in._33 = 0.f;
		break;
	case SWA_Y:
		in._13 = 0.f;
		in._23 = 1.f;
		in._33 = 0.f;
		break;
	case SWA_Z:
		in._13 = 0.f;
		in._23 = 0.f;
		in._33 = 1.f;
		break;
	}

}

//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition16bit
// Desc: takes a D3DXMesh and produces the same mesh with 16 bit compression transform
//       and quantised normals
//-----------------------------------------------------------------------------
HRESULT CompressedMesh::CompressionTransformPosition101012( ID3DXBaseMesh* in )
{
	HRESULT hRes;
	unsigned int i;
	// retrieve the stream declaration of the input mesh
	D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
	hRes = in->GetDeclaration( inDecl );
	if( FAILED(hRes) ) return hRes;

	// create a declaration D3D 
	int curDeclNum = 0;

	// the 10,10,10 portion of the posision
	m_Decl[curDeclNum].Stream	= 0;
	m_Decl[curDeclNum].Offset	= 0;
	m_Decl[curDeclNum].Type		= D3DDECLTYPE_UDEC3;
	m_Decl[curDeclNum].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[curDeclNum].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[curDeclNum].UsageIndex = 0;
	
	curDeclNum++;

	// the 2 bits wasted of the posision (by stream offseting)
	m_Decl[curDeclNum].Stream	= 0;
	m_Decl[curDeclNum].Offset	= 0;
	m_Decl[curDeclNum].Type		= D3DDECLTYPE_UBYTE4;
	m_Decl[curDeclNum].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[curDeclNum].Usage		= D3DDECLUSAGE_POSITION;
	m_Decl[curDeclNum].UsageIndex = 1;

	curDeclNum++;

	// the normal
	m_Decl[curDeclNum].Stream	= 0;
	m_Decl[curDeclNum].Offset	= SizeOfD3DVertexType(D3DDECLTYPE_UDEC3);
	m_Decl[curDeclNum].Type		= D3DDECLTYPE_D3DCOLOR;
	m_Decl[curDeclNum].Method	= D3DDECLMETHOD_DEFAULT;
	m_Decl[curDeclNum].Usage		= D3DDECLUSAGE_NORMAL;
	m_Decl[curDeclNum].UsageIndex = 0;

	curDeclNum++;

	// D3DDECL_END()
	m_Decl[curDeclNum].Stream	= 0xFF;
	m_Decl[curDeclNum].Offset	= 0;
	m_Decl[curDeclNum].Type		= D3DDECLTYPE_UNUSED;
	m_Decl[curDeclNum].Method	= 0;
	m_Decl[curDeclNum].Usage		= 0;
	m_Decl[curDeclNum].UsageIndex = 0;

	hRes = m_pD3D->CreateVertexDeclaration( m_Decl, &m_pDecl );
	if( FAILED(hRes) ) return hRes;

	m_vertexSize = SizeOfD3DVertexType(D3DDECLTYPE_DEC3N) + SizeOfD3DVertexType(D3DDECLTYPE_D3DCOLOR);
	m_numVertex = in->GetNumVertices();
	const unsigned int vbSize = m_vertexSize * m_numVertex;

	hRes = m_pD3D->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_DEFAULT, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX rotMat;
	D3DXMatrixIdentity( &rotMat );

	hRes = DetermineRotationMatrix(in, rotMat);
	if( FAILED(hRes) ) return hRes;

	D3DXMatrixInverse( &rotMat, 0, &rotMat);

	D3DXVECTOR3 scale, offset;
	hRes = DetermineScaleAndOffset(in, rotMat, scale, offset);
	if( FAILED(hRes) ) return hRes;

	D3DXMATRIX scaleMat, offsetMat;
	D3DXMatrixScaling( &scaleMat, scale[0], scale[1], scale[2] );
	D3DXMatrixTranslation( &offsetMat, offset[0], offset[1], offset[2] );

	SW_AXIS minorAxis = SWA_X;
	SW_AXIS otherAxis = SWA_Y;
	SW_AXIS majorAxis = SWA_Z;

	// find major and minoraxis
	if( scale[0] > scale[1] )
	{
		if( scale[0] > scale[2] )
		{
			majorAxis = SWA_X;
			if( scale[1] > scale[2] )
				minorAxis = SWA_Z;
			else
				minorAxis = SWA_Y;
		} else
		{
			majorAxis = SWA_Z;
			minorAxis = SWA_Y;
		}
	} else
	{
		if( scale[1] > scale[2] )
		{
			majorAxis = SWA_Y;
			if( scale[0] > scale[2] )
				minorAxis = SWA_Z;
			else
				minorAxis = SWA_X;
		} else
		{
			majorAxis = SWA_Z;
			minorAxis = SWA_X;
		}
	}

	switch( majorAxis )
	{
	case SWA_X:
		switch( minorAxis )
		{
		case SWA_Y:
			otherAxis = SWA_Z;
			break;
		case SWA_Z:
			otherAxis = SWA_Y;
			break;
		}
	break;
	case SWA_Y:
		switch( minorAxis )
		{
		case SWA_X:
			otherAxis = SWA_Z;
			break;
		case SWA_Z:
			otherAxis = SWA_X;
			break;
		}
	break;
	case SWA_Z:
		switch( minorAxis )
		{
		case SWA_X:
			otherAxis = SWA_Y;
			break;
		case SWA_Y:
			otherAxis = SWA_X;
			break;
		}
	break;
	}

	// create a swap matrix that put the major component into Z 
	D3DXMATRIX swapMat;
	CreateSwapMatrix( swapMat, minorAxis, otherAxis, majorAxis );

	D3DXMatrixInverse( &scaleMat, 0, &scaleMat);
	D3DXMatrixInverse( &offsetMat, 0, &offsetMat);
	D3DXMatrixInverse( &swapMat, 0, &swapMat);

	// m = r-1 * o-1 * s-1 * sw-1
	D3DXMATRIX combined = swapMat;
	D3DXMatrixMultiply( &combined, &scaleMat, &combined );
	D3DXMatrixMultiply( &combined, &offsetMat, &combined);
	D3DXMatrixMultiply( &combined, &rotMat, &combined );

	m_ctMatrix = combined;

	// transform position into compression space + copy normals
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	float* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(i=0;i < in->GetNumVertices();i++)
	{
		// scale and offset position
		D3DXVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		unsigned int ct = CompressionTransformPosition101012( pos );
		((unsigned int*)outStream)[0] = ct;

		// quantise normals
		((unsigned int*)outStream)[1] = QuantiseNormal(inStream[3], inStream[4], inStream[5]);

		// next vertex
		inStream += 6;
		outStream += 2;
	}
	
	// now unlock both vertex buffers
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	hRes = CopyIndexBuffer(in);
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

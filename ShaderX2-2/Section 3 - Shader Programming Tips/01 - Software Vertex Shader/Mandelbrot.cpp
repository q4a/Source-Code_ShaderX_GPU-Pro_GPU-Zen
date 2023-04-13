// Mandelbrot.cpp: implementation of the MandelbrotShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shader.h"
#include "Mandelbrot.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
float MandelbrotShader::m_Colors[128][4];

MandelbrotShader::MandelbrotShader( char *szShaderInitial, char *szShaderOpt )
: Shader( szShaderInitial, szShaderOpt )
{
	m_fMinX = m_fMinZ = -2.0f;
	m_fMaxX = m_fMaxZ = 2.0f;

	float color0[4][4] = 
	{
		{1.0f, 1.0f, 0.0f, 0.0f },
		{0.0f, 0.0f, 0.7f, 0.0f },
		{0.0f, 0.0f, 1.0f, 0.0f },
		{0.7f, 0.7f, 0.0f, 0.0f },
	};
	float color1[4][4] = 
	{
		{0.0f, 0.0f, 0.7f, 0.0f },
		{0.0f, 0.0f, 1.0f, 0.0f },
		{0.7f, 0.7f, 0.0f, 0.0f },
		{1.0f, 1.0f, 0.0f, 0.0f },
	};

	for( int j=0; j<4; j++ )
	{
		for( int i=0; i<32; i++ )
		{
			float fbeta = (float)i / 31.0f;
			m_Colors[j*32+i][0] = fbeta * (color1[j][0] - color0[j][0]) + color0[j][0];
			m_Colors[j*32+i][1] = fbeta * (color1[j][1] - color0[j][1]) + color0[j][1];
			m_Colors[j*32+i][2] = fbeta * (color1[j][2] - color0[j][2]) + color0[j][2];
			m_Colors[j*32+i][3] = fbeta * (color1[j][3] - color0[j][3]) + color0[j][3];
		}
	}
}

MandelbrotShader::~MandelbrotShader()
{
}

HRESULT MandelbrotShader::FrameMove( float fTime, D3DXMATRIXA16 &matWorld, D3DXMATRIXA16 &matView, D3DXMATRIXA16 &matProj )
{
    FLOAT fSecsPerFrame = fTime;

    // Set up the vertex shader constants
    {
        D3DXMATRIXA16 mat;
		D3DXMatrixMultiply( &mat, &matWorld, &matView );
        D3DXMatrixMultiply( &mat, &mat, &matProj );
        D3DXMatrixTranspose( &mat, &mat );

		D3DXVECTOR4 vA( m_fMinX, m_fMinZ, m_fMaxX, m_fMaxZ );
        D3DXVECTOR4 vD( D3DX_PI, 1.0f/(2.0f*D3DX_PI), 2.0f*D3DX_PI, 0.05f );

        m_pd3dDevice->SetVertexShaderConstantF(  0, (float*)&mat,  4 );
        m_pd3dDevice->SetVertexShaderConstantF(  4, (float*)&vA,   1 );
        m_pd3dDevice->SetVertexShaderConstantF(  7, (float*)&vD,   1 );

        m_pd3dDevice->SetVertexShaderConstantF(  10, (float*)m_Colors[0],   128 );

		int iLoop[4] = {248,0,1,0};
		float fRange = 64.0f;
		D3DXVECTOR4 scaleStuff(0.5f,4,1.0f / fRange,0.5f);
		D3DXVECTOR4 colorStuff(128,2,0,fRange * 0.01f / 8.0f);

        m_pd3dDevice->SetVertexShaderConstantI(  0, iLoop, 1 );
        m_pd3dDevice->SetVertexShaderConstantF(  8, (float*)&scaleStuff,   1 );
        m_pd3dDevice->SetVertexShaderConstantF(  9, (float*)&colorStuff,   1 );

    }
	return S_OK;
}


void MandelbrotShader::SetMinx( float fMinx )
{
	m_fMinX = fMinx;
}

void MandelbrotShader::SetMinz( float fMinz )
{
	m_fMinZ = fMinz;
}

void MandelbrotShader::SetMaxx( float fMaxx )
{
	m_fMaxX = fMaxx;
}

void MandelbrotShader::SetMaxz( float fMaxz )
{
	m_fMaxZ = fMaxz;
}


// Shader.cpp: implementation of the Shader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Shader::Shader( char *szShaderInitial, char *szShaderOpt )
{
	strcpy( m_szShaderInitial, szShaderInitial );
	strcpy( m_szShaderOpt, szShaderOpt );

	m_dwDimension	= 180; // Any larger than 181 will overflow 16-bit index buffer
    m_dwNumIndices	= (m_dwDimension - 1) * (m_dwDimension - 1) * 6;
    m_dwNumVertices	= m_dwDimension * m_dwDimension;

	m_pVB = NULL;
    m_pIB = NULL;
	m_pd3dDevice = NULL;
	m_d3dSettings = NULL;
    m_pVertexShaderInitial = NULL;
    m_pVertexShaderOpt = NULL;
    m_pVertexDeclaration = NULL;

	m_ActiveShader = SHADER_INITIAL;
}

Shader::~Shader()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}

HRESULT Shader::FrameMove( float fTime, D3DXMATRIXA16 &matWorld, D3DXMATRIXA16 &matView, D3DXMATRIXA16 &matProj )
{
	return S_OK;
}

HRESULT Shader::Render()
{

	m_pd3dDevice->SetVertexDeclaration( m_pVertexDeclaration );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DXVECTOR2) );

	switch( m_ActiveShader )
	{
		case SHADER_INITIAL:
			if (m_pVertexShaderInitial)
				m_pd3dDevice->SetVertexShader( m_pVertexShaderInitial );
			break;
		case SHADER_OPT:
			if (m_pVertexShaderOpt)
				m_pd3dDevice->SetVertexShader( m_pVertexShaderOpt );
			break;
	}

	m_pd3dDevice->SetIndices( m_pIB );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices,
                                            0, m_dwNumIndices/3 );


	return S_OK;
}

D3DVERTEXELEMENT9 * Shader::GetDecl()
{
	static D3DVERTEXELEMENT9 decl[] = 
        {
            { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
            D3DDECL_END()
        };
	return decl;
}

HRESULT Shader::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, CD3DSettings &d3dSettings )
{
	HRESULT hr;

	m_pd3dDevice = pd3dDevice;
	m_d3dSettings = &d3dSettings;

    // Create vertex shaders
   {
        LPD3DXBUFFER pCode;

        if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( GetDecl(), &m_pVertexDeclaration ) ) )
        {
            return hr;
        }


        DWORD dwFlags = 0;
#if defined( _DEBUG ) || defined( DEBUG )
        dwFlags |= D3DXSHADER_DEBUG;
#endif
		pCode = NULL;

        // Assemble the vertex shader from the file
        if( FAILED( hr = D3DXAssembleShaderFromFile( m_szShaderInitial, 
                                                     NULL, NULL, dwFlags, &pCode, NULL ) ) )
        {
            return hr;
        }

        // Create the vertex shader
        hr = m_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
                                               &m_pVertexShaderInitial );

		pCode->Release();
 
		// Make sure we have support for VS 3.0
        BOOL enable30 =  (d3dSettings.GetVertexProcessingType() == SOFTWARE_VP) || d3dSettings.PDeviceInfo()->Caps.VertexShaderVersion >= D3DVS_VERSION(3,0);

        if( FAILED( hr = D3DXAssembleShaderFromFile( m_szShaderOpt, 
                                                     NULL, NULL, dwFlags, &pCode, NULL ) ) )
        {
            return hr;
        }

        // Create the vertex shader
		if (enable30)
		{
			hr = m_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
				                                   &m_pVertexShaderOpt );
			SAFE_RELEASE(pCode);
	        if( FAILED(hr) )
		        return hr;

		}
		else
		{
			m_pVertexShaderOpt = NULL;
		}
		SAFE_RELEASE( pCode );

	}

	return S_OK;
}

HRESULT Shader::RestoreDeviceObjects()
{
    HRESULT hr;

    // Create index buffer
    {
        WORD* pIndices;

        if( FAILED( hr = m_pd3dDevice->CreateIndexBuffer( m_dwNumIndices*sizeof(WORD),
                                                          0, D3DFMT_INDEX16,
                                                          D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
            return hr;

        if( FAILED( hr = m_pIB->Lock( 0, 0, (void**)&pIndices, 0 ) ) )
		{
			SAFE_RELEASE( m_pIB );
            return hr;
		}

        for( DWORD y=1; y<m_dwDimension; y++ )
        {
            for( DWORD x=1; x<m_dwDimension; x++ )
            {
                *pIndices++ = (WORD)( (y-1)*m_dwDimension + (x-1) );
                *pIndices++ = (WORD)( (y-0)*m_dwDimension + (x-1) );
                *pIndices++ = (WORD)( (y-1)*m_dwDimension + (x-0) );

                *pIndices++ = (WORD)( (y-1)*m_dwDimension + (x-0) );
                *pIndices++ = (WORD)( (y-0)*m_dwDimension + (x-1) );
                *pIndices++ = (WORD)( (y-0)*m_dwDimension + (x-0) );
            }
        }

        if( FAILED( hr = m_pIB->Unlock() ) )
		{
			SAFE_RELEASE( m_pIB );
            return hr;
		}
    }


    // Create vertex buffer
    {
        D3DXVECTOR2 *pVertices;
		DWORD usage = (m_d3dSettings->GetVertexProcessingType() == SOFTWARE_VP) ? D3DUSAGE_SOFTWAREPROCESSING : 0;

        if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( m_dwNumVertices*sizeof(D3DXVECTOR2),
                                                           D3DUSAGE_WRITEONLY | usage, 0,
                                                           D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
            return hr;

        
        if( FAILED( hr = m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
            return hr;

        for( DWORD y=0; y<m_dwDimension; y++ )
        {
            for( DWORD x=0; x<m_dwDimension; x++ )
            {
                *pVertices = D3DXVECTOR2( ((float)x / (float)(m_dwDimension-1) - 0.5f),
                                            ((float)y / (float)(m_dwDimension-1) - 0.5f) );

				pVertices++;
            }
        }

        if( FAILED( hr = m_pVB->Unlock() ) )
            return hr;
    }

    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CLIPPING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    return S_OK;
}

HRESULT Shader::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pVB );
	return S_OK;
}

HRESULT Shader::DeleteDeviceObjects()
{
    SAFE_RELEASE( m_pVertexShaderInitial );
    SAFE_RELEASE( m_pVertexShaderOpt );
    SAFE_RELEASE( m_pVertexDeclaration );
	return S_OK;
}


void Shader::SetActiveShader( ShaderType act )
{
	m_ActiveShader = act;
}

void Shader::SetTessellation( int iTess )
{
	m_dwDimension = iTess;
    m_dwNumIndices	= (m_dwDimension - 1) * (m_dwDimension - 1) * 6;
    m_dwNumVertices	= m_dwDimension * m_dwDimension;
	InvalidateDeviceObjects();
	RestoreDeviceObjects();
}

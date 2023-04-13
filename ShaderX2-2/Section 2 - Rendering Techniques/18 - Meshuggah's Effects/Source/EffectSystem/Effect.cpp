///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e EffectSystem::CEffect.
 * 
 * @file
 *  Effect.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e EffectSystem::CEffect.
 */



#include "stdafx.h"
#include "Effect.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"
#include "D3DInit.h"
#include "ShaderSourceList.h"
#include "Clamp.h"
#include "EffectTransition.h"

#include <vector>
#include <string>



using namespace EffectSystem;
using namespace ScrambledArchive;
using namespace Shared;
using namespace std;



CEffect::CEffect( IDirect3DDevice8* pD3DDev, CScrambledArchive* pkScrambledArchive,
                  CTextureManager* pkTextureManager, CEffectTransition* pkEffectTransition,
                  bool bBrowserMode )
: m_bRegistered( false )
, m_bBrowserMode( bBrowserMode )
, m_fEffectStart( 0 )
, m_fEffectEnd( 0 )
, m_pD3DDev( pD3DDev )
, m_pkScrambledArchive( pkScrambledArchive )
, m_pkTextureManager( pkTextureManager )
, m_pkEffectTransition( pkEffectTransition )
{
    DBG_ASSERT( ( false != bBrowserMode ) || ( 0 != m_pkEffectTransition ) );
    DBG_ASSERT( 0 != m_pkScrambledArchive );
    DBG_ASSERT( 0 != m_pkTextureManager );
    DBG_ASSERT( 0 != m_pD3DDev );

    m_pD3DDev->AddRef();
}



CEffect::~CEffect()
{
}



void 
CEffect::Destroy()
{
    delete this;
}



void 
CEffect::OnMouseMove( int uiMouseX, int MouseY )
{
}



void
CEffect::OnMouseButton( EButtonID eButton, int uiMouseX, int MouseY )
{
}



void 
CEffect::OnKey( unsigned int uiKey, unsigned int uiFlags, bool bDown )
{
}



void
CEffect::FreeAllResources()
{
    FreeAppManagedResources();

    m_pD3DDev->Release();
}



float 
CEffect::GetAspectRatio()
{
    D3DVIEWPORT8 sViewport;
    m_pD3DDev->GetViewport( &sViewport );
    
    return( (float) sViewport.Width / (float) sViewport.Height );
}



unsigned int
CEffect::GetPreferedVertexShaderUsageFlag()
{
    D3DDEVICE_CREATION_PARAMETERS sD3DCreationParams;
    m_pD3DDev->GetCreationParameters( &sD3DCreationParams );

    if( 0 != ( sD3DCreationParams.BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) )
    {
        // run vertex shader in software emulation
        return( D3DUSAGE_SOFTWAREPROCESSING );
    }
    
    // run vertex shader in hw (prefered usage for D3DCREATE_HARDWARE_VERTEXPROCESSING
    // and D3DCREATE_MIXED_VERTEXPROCESSING)    
    return( 0 ); 
}



bool 
CEffect::CheckPixelShaderCap( unsigned int uiPSMajor, unsigned int uiPSMinor )
{
    D3DCAPS8 sCaps;        
    m_pD3DDev->GetDeviceCaps( &sCaps );

    if( D3DSHADER_VERSION_MAJOR( sCaps.PixelShaderVersion ) < uiPSMajor &&
        D3DSHADER_VERSION_MINOR( sCaps.PixelShaderVersion ) < uiPSMinor )
    {
        return( false );
    }
    
    return( true );
}



bool 
CEffect::CheckVertexShaderCap( unsigned int uiVSMajor, unsigned int uiVSMinor )
{
    // if a vertex shader is supposed to run in software
    // then return true since d3d vertex shader emulation is able
    // to run any vertex shader
    if( D3DUSAGE_SOFTWAREPROCESSING == GetPreferedVertexShaderUsageFlag() )
    {
        return( true );
    }

    D3DCAPS8 sCaps;        
    m_pD3DDev->GetDeviceCaps( &sCaps );

    if( D3DSHADER_VERSION_MAJOR( sCaps.VertexShaderVersion ) < uiVSMajor &&
        D3DSHADER_VERSION_MINOR( sCaps.VertexShaderVersion ) < uiVSMinor )
    {
        return( false );
    }
    
    return( true );
}



bool 
CEffect::IsTextureFormatSupported( D3DFORMAT eTextureFormat, D3DRESOURCETYPE eResourceType, DWORD dwUsage )
{
    IDirect3D8* pD3D;
    if( FAILED( m_pD3DDev->GetDirect3D( &pD3D ) ) )
    {
        return( false );
    }

    D3DDEVICE_CREATION_PARAMETERS sD3DCreationParams;
    m_pD3DDev->GetCreationParameters( &sD3DCreationParams );

    D3DDISPLAYMODE sAdapterDisplayMode;
    if( FAILED( pD3D->GetAdapterDisplayMode( 
                        sD3DCreationParams.AdapterOrdinal, 
                        &sAdapterDisplayMode
                        ) ) )
    {
        pD3D->Release();
        return( false );
    }

    bool bTextureFormatSupported( false );
    if( SUCCEEDED( pD3D->CheckDeviceFormat( 
                                    sD3DCreationParams.AdapterOrdinal,
                                    sD3DCreationParams.DeviceType,
                                    sAdapterDisplayMode.Format,
                                    dwUsage,
                                    eResourceType,
                                    eTextureFormat 
                                    ) ) )
    {
        bTextureFormatSupported = true;
    }
    pD3D->Release();

    return( bTextureFormatSupported );
}



bool 
CEffect::LoadShaderSources( SShaderSourceList& colShaders, 
                            const SShaderDescriptor* const pShaderDesc,
                            unsigned int uiNumShaders ) const
{
    colShaders.m_colEntries.clear();
    
    for( unsigned int i( 0 ); i < uiNumShaders; ++i )
    {
        vector< unsigned char > colTmp;
        if( 0 != pShaderDesc[ i ].m_pcVSFilePath &&
            false == m_pkScrambledArchive->DecompressFile( pShaderDesc[ i ].m_pcVSFilePath, colTmp ) )
        {
            return( false );
        }
        string strVertexShader;
        strVertexShader.insert( strVertexShader.begin(), colTmp.begin(), colTmp.end() );

        if( 0 != pShaderDesc[ i ].m_pcPSFilePath &&
            false == m_pkScrambledArchive->DecompressFile( pShaderDesc[ i ].m_pcPSFilePath, colTmp ) )
        {
            return( false );
        }
        string strPixelShader;
        strPixelShader.insert( strPixelShader.begin(), colTmp.begin(), colTmp.end() );

        colShaders.m_colEntries.push_back( 
            SShaderSourceList::SShaderSource( pShaderDesc[ i ].m_pcShaderName, strVertexShader, strPixelShader ) );
    }

    return( true );
}



float 
CEffect::GetBassLineValue( float fTime, 
                           const unsigned char* const pcBassLine, 
                           unsigned int uiBassLineSize )
{
    // map effect relative time to bass line array range
    float fIndex( fTime / GetEffectRuntime() );
    int iMapIndex( (int) ( fIndex * uiBassLineSize ) );

    float a( pcBassLine[ Clamp( iMapIndex    , 0, (int) uiBassLineSize - 1 ) ] );
    float b( pcBassLine[ Clamp( iMapIndex + 1, 0, (int) uiBassLineSize - 1 ) ] );

    float fBlend( fIndex - (int) fIndex );
    
    return( ( a + fBlend * ( b - a ) ) / 255.0f );
}



void 
CEffect::DrawBackground( const char* const pcBackgroundTexture )
{
    // get viewport settings to render quad
    D3DVIEWPORT8 sViewPort;
    m_pD3DDev->GetViewport( &sViewPort );

    struct SVertex
    {
        SVertex( const D3DXVECTOR4& pos, const D3DXVECTOR2& tex )
        : vPos( pos )
        , vTex( tex )
        {
        }

        D3DXVECTOR4 vPos;
        D3DXVECTOR2 vTex;
    };

    SVertex quad[ 4 ] =
    {
        {
            SVertex( D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ), 
                     D3DXVECTOR2( 0.0f, 0.0f ) )
        },

        {
            SVertex( D3DXVECTOR4( 0.0f, (float) sViewPort.Height , 1.0f, 1.0f ), 
                     D3DXVECTOR2( 0.0f, 1.0f ) )
        },

        {
            SVertex( D3DXVECTOR4( (float) sViewPort.Width, 0.0f, 1.0f, 1.0f ),
                     D3DXVECTOR2( 1.0f, 0.0f ) )
        },

        {
            SVertex( D3DXVECTOR4( (float) sViewPort.Width, (float) sViewPort.Height , 1.0f, 1.0f ),
                     D3DXVECTOR2( 1.0f, 1.0f ) )
        }
    };

    // set render states
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );         
    m_pD3DDev->SetRenderState( D3DRS_ZENABLE, FALSE );

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    m_pD3DDev->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    m_pD3DDev->SetPixelShader( 0 );

    // set texture
    IDirect3DBaseTexture8* pkBackgroundTexture( m_pkTextureManager->GetTexture( pcBackgroundTexture ) );
    m_pD3DDev->SetTexture( 0, pkBackgroundTexture ); 

    // set texture stage states
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

    m_pD3DDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, quad, sizeof( SVertex ) );

    m_pD3DDev->SetTexture( 0, 0 ); 
    pkBackgroundTexture->Release();
}
///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e EffectSystem::CEffectTransition.
 *
 * @file
 *  EffectTransition.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  26.01.2002
 * @brief
 *  Implementation of \e EffectSystem::CEffectTransition.
 */



#include "stdafx.h"
#include "EffectTransition.h"
#include "EffectException.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"

#include "TransitionShaderConstants.h"

namespace
{
    // wrap shader code in an anonymous namespace to 
    // avoid any possible conflicts with other modules as
    // shader assembler doesn't declare shader byte code arrays 
    // static when writing them to outfile

#   include "Shader\Transition.psh"
}



using namespace EffectSystem;
using namespace ScrambledArchive;
using namespace Shared;



const char* CEffectTransition::sc_pcTransitionTexture0( "Transitions\\Transition0.jpg" );
const char* CEffectTransition::sc_pcTransitionTexture1( "Transitions\\Transition1.jpg" );
const char* CEffectTransition::sc_pcTransitionTexture2( "Transitions\\Transition2.jpg" );
const char* CEffectTransition::sc_pcTransitionTexture3( "Transitions\\Transition3.jpg" );



CEffectTransition::CEffectTransition( IDirect3DDevice8* pD3DDev, CScrambledArchive* pkScrambledArchive,
                                      CTextureManager* pkTextureManager )
: m_pD3DDev( pD3DDev )
, m_pkScrambledArchive( pkScrambledArchive )
, m_pkTextureManager( pkTextureManager )
, m_dwPSHandle( 0 )
{
    try
    {
        DBG_ASSERT( 0 != m_pkScrambledArchive );
        DBG_ASSERT( 0 != m_pkTextureManager );
        DBG_ASSERT( 0 != m_pD3DDev );

        m_pD3DDev->AddRef();

        // load textures
        if( false == m_pkTextureManager->LoadJPEGTexture( sc_pcTransitionTexture0, true, true, false ) )
        {
            throw( CEffectException( "Failed to load transition texture!" ) );
        }

        if( false == m_pkTextureManager->LoadJPEGTexture( sc_pcTransitionTexture1, true, true, false ) )
        {
            throw( CEffectException( "Failed to load transition texture!" ) );
        }

        if( false == m_pkTextureManager->LoadJPEGTexture( sc_pcTransitionTexture2, true, true, false ) )
        {
            throw( CEffectException( "Failed to load transition texture!" ) );
        }

        if( false == m_pkTextureManager->LoadJPEGTexture( sc_pcTransitionTexture3, true, true, false ) )
        {
            throw( CEffectException( "Failed to load transition texture!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwTransitionPixelShader, &m_dwPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

    }
    catch( CEffectException& kException )
    {
        kException.ReportError();

        // do clean up here as the destructor won't be called!
        FreeAllResources();

        throw;
    }
}



CEffectTransition::~CEffectTransition()
{
    FreeAllResources();
}



CEffectTransition* 
CEffectTransition::Create( IDirect3DDevice8* pD3DDev, 
                           ScrambledArchive::CScrambledArchive* pkScrambledArchive,
                           Shared::CTextureManager* pkTextureManager )
{
    CEffectTransition* pkEffectTransition( 0 );

    try
    {
        pkEffectTransition = new CEffectTransition( pD3DDev, pkScrambledArchive, pkTextureManager );
    }
    catch( ... )
    {
    }

    return( pkEffectTransition );
}



void
CEffectTransition::FreeAllResources()
{
    if( 0 != m_dwPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwPSHandle );
    }

    m_pD3DDev->Release();
}



void 
CEffectTransition::ColorBlend( float fTime,
                               float fBlendStart, unsigned char ucAStart,
                               float fBlendEnd, unsigned char ucAEnd,
                               unsigned char ucR, unsigned char ucG, unsigned char ucB )
{
    DBG_ASSERT( fBlendStart < fBlendEnd );

    unsigned char ucA;
    if( fTime < fBlendStart )
    {
        ucA = ucAStart;
    }
    else
    if( fTime > fBlendEnd )
    {
        ucA = ucAEnd;
    }
    else
    {
        float fBlend( ( fTime - fBlendStart ) / ( fBlendEnd - fBlendStart ) );
        ucA = (unsigned char) ( ucAStart + fBlend * ( (int) ucAEnd - (int) ucAStart ) );
    }

    // get viewport settings to render quad
    D3DVIEWPORT8 sViewPort;
    m_pD3DDev->GetViewport( &sViewPort );

    D3DXVECTOR4 vPos[ 4 ] =
    {
        D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ),
        D3DXVECTOR4( 0.0f, (float) sViewPort.Height , 1.0f, 1.0f ),
        D3DXVECTOR4( (float) sViewPort.Width, 0.0f, 1.0f, 1.0f ),
        D3DXVECTOR4( (float) sViewPort.Width, (float) sViewPort.Height , 1.0f, 1.0f )
    };

    // setup color
    unsigned int uiColor = ( (unsigned int) ucA << 24 ) + 
                           ( (unsigned int) ucR << 16 ) + 
                           ( (unsigned int) ucG <<  8 ) + 
                           ( (unsigned int) ucB <<  0 );

    // set render states
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );         
    m_pD3DDev->SetRenderState( D3DRS_ZENABLE, FALSE );
    m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, uiColor );      

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    m_pD3DDev->SetVertexShader( D3DFVF_XYZRHW );
    m_pD3DDev->SetPixelShader( 0 );

    // set texture stage states
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    m_pD3DDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vPos, sizeof( D3DXVECTOR4 ) );
}



void 
CEffectTransition::ColorBlendTexture( float fTime,
                                      float fBlendStart, unsigned char ucAStart,
                                      float fBlendEnd, unsigned char ucAEnd,
                                      unsigned char ucR, unsigned char ucG, unsigned char ucB,
                                      const char* const pcTransitionTexture )
{
    DBG_ASSERT( fBlendStart < fBlendEnd );

    // due to how the shader operates adjust the alpha range
    // an alpha value of 153 is enough to cover the entire screen with the given color (ucR, ucG, ucB)
    ucAStart = ( ucAStart * 153 ) / 255;
    ucAEnd = ( ucAEnd * 153 ) / 255;

    unsigned char ucA;
    if( fTime < fBlendStart )
    {
        ucA = ucAStart;
    }
    else
    if( fTime > fBlendEnd )
    {
        ucA = ucAEnd;
    }
    else
    {
        float fBlend( ( fTime - fBlendStart ) / ( fBlendEnd - fBlendStart ) );
        ucA = (unsigned char) ( ucAStart + fBlend * ( (int) ucAEnd - (int) ucAStart ) );
    }

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

    // setup pixel shader constants
    m_pD3DDev->SetPixelShaderConstant( CP_TRANSITION_COLOR, 
                                       D3DXVECTOR4( ucR / 255.0f, ucG / 255.0f, ucB / 255.0f, 1.0f ), 1 );

    m_pD3DDev->SetPixelShaderConstant( CP_TRANSITION_TIME, 
                                       D3DXVECTOR4( ucA / 255.0f, ucA / 255.0f, ucA / 255.0f, ucA / 255.0f ), 1 );

    // set render states
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );         
    m_pD3DDev->SetRenderState( D3DRS_ZENABLE, FALSE );

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    m_pD3DDev->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    m_pD3DDev->SetPixelShader( m_dwPSHandle );

    // set texture
    IDirect3DBaseTexture8* pkTransitionTexture( m_pkTextureManager->GetTexture( pcTransitionTexture ) );
    m_pD3DDev->SetTexture( 0, pkTransitionTexture ); 

    // set texture stage states
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    m_pD3DDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, quad, sizeof( SVertex ) );

    m_pD3DDev->SetTexture( 0, 0 ); 
    pkTransitionTexture->Release();
}

///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CInfiniteZoom.
 *
 * @file
 *  InfiniteZoom.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  25.02.2002
 * @brief
 *  Implementation of \e Effects::CInfiniteZoom.
 */



#include "StdAfx.h"
#include "InfiniteZoom.h"
#include "InfiniteZoomDialog.h"
#include "EffectException.h"
#include "ShaderSourceList.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"
#include "EffectTransition.h"
#include "resource.h"
#include "..\..\Help\ContextIDs.h"

#include <string>

#include "InfiniteZoomConsts.h"
#include "InfiniteZoomShaderConstants.h"

namespace
{
    // wrap shader code in an anonymous namespace to 
    // avoid any possible conflicts with other modules as
    // shader assembler doesn't declare shader byte code arrays 
    // static when writing them to outfile

#   include "Shader\InfiniteZoom.psh"
#   include "Shader\InfiniteZoom.vsh"
}



using namespace Effects;
using namespace EffectSystem;
using namespace Shared;
using namespace ScrambledArchive;
using namespace std;



CInfiniteZoom::CInfiniteZoom( IDirect3DDevice8* pD3DDev, 
                              CScrambledArchive* pkScrambledArchive,
                              CTextureManager* pkTextureManager, 
                              CEffectTransition* pkEffectTransition,
                              bool bBrowserMode )
: CEffect( pD3DDev, pkScrambledArchive, pkTextureManager, pkEffectTransition, bBrowserMode )
, m_pkVertexBuffer( 0 )
, m_pkZoomSettingsDlg( 0 )
, m_dwVSHandle( 0 )
, m_dwPSHandle( 0 )
, m_colBassSpectrum()
{
    try
    {
        ConfirmRequirements();

        // load keyframe texture
        for( int i( 0 ); i < NUM_ZOOM_KEYS; ++i )
        {
            if( false == m_pkTextureManager->LoadDDSTexture( c_pcTextureNames[ i ], true ) )
            {
                throw( CEffectException( "Failed to load keyframe texture for infinite zoom!" ) );
            }
        }

        if( false == m_bBrowserMode )
        {
            // load credits texture
            if( false == m_pkTextureManager->LoadJPEGTexture( c_pcCreditsTexture, false, true, false ) )
            {
                throw( CEffectException( "Failed to load credits texture for infinite zoom!" ) );
            }

            // load bass spectrum of music
            if( false == pkScrambledArchive->DecompressFile( c_pcBassSpectrum, m_colBassSpectrum )  )
            {
                throw( CEffectException( "Failed to load bass spectrum!" ) );
            }
        }

        // create vertex shader
        DWORD dwInfiniteZoomDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // position 
                D3DVSD_REG( 1, D3DVSDT_FLOAT2 ), // 2d texture coords
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader( 
                            dwInfiniteZoomDecl, 
                            dwInfiniteZoomVertexShader, 
                            &m_dwVSHandle, 
                            GetPreferedVertexShaderUsageFlag() 
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwInfiniteZoomPixelShader, &m_dwPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // initialize all app managed resources
        InitAppManagedResources();

        // create settings dialog if we are in browser mode
        if( false != m_bBrowserMode )
        {
            m_pkZoomSettingsDlg = new CInfiniteZoomDialog();
            if( 0 == m_pkZoomSettingsDlg ||
                FALSE == m_pkZoomSettingsDlg->Create( IDD_INFINITE_ZOOM ) )
            {
                throw( CEffectException( "Failed to create effect settings dialog!" ) );
            }
            
            m_pkZoomSettingsDlg->ShowWindow( SW_SHOW );
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



CInfiniteZoom::~CInfiniteZoom()
{
    FreeAllResources();
}



void 
CInfiniteZoom::FreeAllResources()
{
    if( 0 != m_dwVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwVSHandle );
    }

    if( 0 != m_dwPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwPSHandle );
    }

    if( 0 != m_pkZoomSettingsDlg )
    {
        m_pkZoomSettingsDlg->DestroyWindow();
        delete m_pkZoomSettingsDlg;
    }

    CEffect::FreeAllResources();
}



const char* const 
CInfiniteZoom::GetEffectName()
{
    return( c_pcEffectName );
}



bool 
CInfiniteZoom::IsEffectDialogVisible()
{
    DBG_ASSERT( false != m_bBrowserMode );
    return( ( false != m_pkZoomSettingsDlg->IsWindowVisible() ) ? true : false );
}



void 
CInfiniteZoom::ShowEffectDialog( bool bShow )
{
    DBG_ASSERT( false != m_bBrowserMode );
    int iShowCmd( ( false != bShow ) ? SW_SHOW : SW_HIDE );
    m_pkZoomSettingsDlg->ShowWindow( iShowCmd );
}



unsigned int 
CInfiniteZoom::GetHTMLHelpID()
{
    return( ID_HELPCONTEXT_INFINITEZOOM );
}



void 
CInfiniteZoom::InitAppManagedResources()
{
    HRESULT hr;

    // create dynamic vertex buffer
    DBG_ASSERT( 0 == m_pkVertexBuffer );
    if( FAILED( hr = m_pD3DDev->CreateVertexBuffer( 
                        4 * NUM_ZOOM_KEYS * sizeof( SVertex ), 
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
                        0, 
                        D3DPOOL_DEFAULT,
                        &m_pkVertexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic vertex buffer!" ) );
    }

    // lock the entire vertex buffer
    SVertex* pVertices; 
    if( FAILED( hr = m_pkVertexBuffer->Lock( 
                      0, 
                      0, 
                      (BYTE **) &pVertices, 
                      D3DLOCK_DISCARD  ) ) )
    {
        throw( CEffectException( "Failed to lock dynamic vertex buffer!" ) );
    }

    // initialize quads for fractal zoom
    for( int i( 0 ); i < NUM_ZOOM_KEYS; ++i )
    {
        pVertices[ 0 ].vPos = D3DXVECTOR3( c_pfZoomKeys[ i + 1 ][ 0 ] - c_pfZoomKeys[ i + 1 ][ 2 ], 
                                           c_pfZoomKeys[ i + 1 ][ 1 ] + c_pfZoomKeys[ i + 1 ][ 2 ],
                                           0.0f );
        pVertices[ 0 ].vTex = D3DXVECTOR2( 0.0f, 0.0f );


        pVertices[ 1 ].vPos = D3DXVECTOR3( c_pfZoomKeys[ i + 1 ][ 0 ] - c_pfZoomKeys[ i + 1 ][ 2 ], 
                                           c_pfZoomKeys[ i + 1 ][ 1 ] - c_pfZoomKeys[ i + 1 ][ 2 ],
                                           0.0f );
        pVertices[ 1 ].vTex = D3DXVECTOR2( 0.0f, 1.0f );


        pVertices[ 2 ].vPos = D3DXVECTOR3( c_pfZoomKeys[ i + 1 ][ 0 ] + c_pfZoomKeys[ i + 1 ][ 2 ], 
                                           c_pfZoomKeys[ i + 1 ][ 1 ] + c_pfZoomKeys[ i + 1 ][ 2 ],
                                           0.0f );
        pVertices[ 2 ].vTex = D3DXVECTOR2( 1.0f, 0.0f );


        pVertices[ 3 ].vPos = D3DXVECTOR3( c_pfZoomKeys[ i + 1 ][ 0 ] + c_pfZoomKeys[ i + 1 ][ 2 ], 
                                           c_pfZoomKeys[ i + 1 ][ 1 ] - c_pfZoomKeys[ i + 1 ][ 2 ],
                                           0.0f );
        pVertices[ 3 ].vTex = D3DXVECTOR2( 1.0f, 1.0f );

        pVertices += 4;
    }

    m_pkVertexBuffer->Unlock();
}



void 
CInfiniteZoom::FreeAppManagedResources()
{
    SafeRelease( m_pkVertexBuffer );
}



void
CInfiniteZoom::ConfirmRequirements()
{
    D3DCAPS8 sCaps;
    m_pD3DDev->GetDeviceCaps( &sCaps );

    // check mip mapping capability
    if( 0 == ( sCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP ) )
    {
        throw( CEffectException( "Device doesn't support mip mapping!" ) );
    }

    // check tri linear filtering capability
    if( 0 == ( sCaps.TextureCaps & D3DPTFILTERCAPS_MIPFLINEAR ) )
    {
        throw( CEffectException( "Device doesn't support trilinear filtering!" ) );
    }

    // check vertex shader caps
    if( false == CheckVertexShaderCap( 1, 1 ) )
    {
        throw( CEffectException( "Device doesn't support vertex shader version 1.1!" ) );
    }

    // check pixel shader caps
    if( false == CheckPixelShaderCap( 1, 1 ) )
    {
        throw( CEffectException( "Device doesn't support pixel shader version 1.1!" ) );
    }

    // check support for compressed textures
    if( false == IsTextureFormatSupported( D3DFMT_DXT1, D3DRTYPE_TEXTURE ) )
    {
        throw( CEffectException( "Device doesn't support compressed textures (DXT1)!" ) );
    }
}



void 
CInfiniteZoom::SetShaderConstants()
{
    // vertex shader constants    
    D3DXMATRIX matTemp;
    GetWorldViewProjection( matTemp );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDVIEWPROJ_0, &matTemp, 4 ); 
}



void 
CInfiniteZoom::Update( float fTime )
{
    float fZoomLength;
    float fZoomPosition;
    float fIndex;
    float fIndexPos;
    int   iIndex;

    if( false != m_bBrowserMode )
    {        
        DBG_ASSERT( 0 != m_pkZoomSettingsDlg );

        fZoomLength   = m_pkZoomSettingsDlg->GetZoomLength();
        fZoomPosition = m_pkZoomSettingsDlg->GetZoomPosition();
    }
    else
    {   
        fTime = GetRelativeTime( fTime );

        fZoomLength   = GetEffectRuntime();
        fZoomPosition = fmodf( fTime, fZoomLength );
    }    
    
    fIndex    = ( NUM_ZOOM_KEYS - 1 ) * fZoomPosition / fZoomLength;
    fIndexPos = fIndex - ( int ) fIndex;       
    iIndex    = 1 + ( int ) ( fIndex );


    D3DXVECTOR3 vEye;
    D3DXVec3CatmullRom( &vEye, &D3DXVECTOR3(  c_pfZoomKeys[ iIndex - 1 ][ 0 ], 
                                              c_pfZoomKeys[ iIndex - 1 ][ 1 ],
                                             -c_pfZoomKeys[ iIndex - 1 ][ 2 ] ),

                               &D3DXVECTOR3(  c_pfZoomKeys[ iIndex     ][ 0 ], 
                                              c_pfZoomKeys[ iIndex     ][ 1 ],
                                             -c_pfZoomKeys[ iIndex     ][ 2 ] ),

                               &D3DXVECTOR3(  c_pfZoomKeys[ iIndex + 1 ][ 0 ], 
                                              c_pfZoomKeys[ iIndex + 1 ][ 1 ],
                                             -c_pfZoomKeys[ iIndex + 1 ][ 2 ] ),

                               &D3DXVECTOR3(  c_pfZoomKeys[ iIndex + 2 ][ 0 ], 
                                              c_pfZoomKeys[ iIndex + 2 ][ 1 ],
                                             -c_pfZoomKeys[ iIndex + 2 ][ 2 ] ),
                               fIndexPos );

    D3DXMatrixTranslation( &m_matWorld, -vEye.x, -vEye.y, -vEye.z );

    if( false != m_bBrowserMode )
    {
        D3DXMatrixRotationZ( &m_matView, m_pkZoomSettingsDlg->GetZRotationAngle() );
    }
    else
    {
        D3DXMatrixRotationZ( &m_matView, -0.5f * fZoomPosition );
    }

    D3DXMatrixPerspectiveFovLH( &m_matProj, 
        D3DXToRadian( 90.0f ), GetAspectRatio(), 1e-4f, 32768.0f );

    if( SUCCEEDED( m_pD3DDev->BeginScene() ) )
    {
        m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, 1.0f, 0 );
            
        // set basic render states
        m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
        m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );         
        m_pD3DDev->SetRenderState( D3DRS_ZENABLE, FALSE );
        
        m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE, TRUE ); 
        m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_LOCALVIEWER, FALSE );

        m_pD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_CLIPPING, TRUE );

        // disable alpha blending
        m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

        // set texture stage states
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

        m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

        // set shaders
        SetShaderConstants();
        m_pD3DDev->SetVertexShader( m_dwVSHandle );
        m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );
        m_pD3DDev->SetPixelShader( m_dwPSHandle );

        for( int i( ( ( int ) fIndex ) - 2 ); i < NUM_ZOOM_KEYS; ++i )
        {
            if( i >= 0 )
            {
                IDirect3DBaseTexture8* pkTexture( m_pkTextureManager->GetTexture( c_pcTextureNames[ i ] ) );

                m_pD3DDev->SetTexture( 0, pkTexture );  
                m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 4 * i, 2 );
                
                SafeRelease( pkTexture );
            }
        }

        if( false == m_bBrowserMode )
        {
            RenderCredits( fTime );
        }


        if( false == m_bBrowserMode )
        {
            if( fTime < 1.0f )
            {
                m_pkEffectTransition->ColorBlend( 
                            fTime, 
                            0.0f, 255,
                            1.0f,   0, 
                            0, 0, 0
                          );
            }
            else
            if( fTime > GetEffectRuntime() - 0.5f )
            {              
                m_pkEffectTransition->ColorBlend( 
                            fTime, 
                            GetEffectRuntime() - 0.5f,   0,
                            GetEffectRuntime()       , 255, 
                            0, 0, 0
                          );
            }
        }


        m_pD3DDev->SetTexture( 0, 0 );        

        m_pD3DDev->EndScene();                      
        m_pD3DDev->Present( 0, 0, 0, 0 );
    }
}



bool 
CInfiniteZoom::GetShaderSources( SShaderSourceList& colShaders )
{
    const SShaderDescriptor c_shaderDesc[] =
    {
        {
            "Infinite Zoom",
            "InfiniteZoom\\InfiniteZoom.vsa",
            "InfiniteZoom\\InfiniteZoom.psa"
        }
    };

    return( LoadShaderSources( colShaders, c_shaderDesc, 1 ) );
}



void 
CInfiniteZoom::RenderCredits( float fTime )
{
    if( fTime < 1.0f || fTime > 17.0f )
    {
        return;
    }

    const float c_fTextLifeSpan( 2.0f );

    // calculate text effect parameters
    float fFade( 0.0f );
    float fStretch( GetBassLineValue( fTime, &m_colBassSpectrum[ 0 ], 
                                      (unsigned int) m_colBassSpectrum.size() ) );

    fTime -= 1.0f;

    float fVertTexOffset( 0.125f * ( (int) ( fTime / c_fTextLifeSpan ) ) );
    float fCycle( fmodf( fTime, c_fTextLifeSpan ) );

    if( fCycle < 0.25f )
    {
        fFade = 4.0f * ( fCycle );
    }   
    else
    if( fCycle < 1.75f )
    {
        fStretch *= ( 0.5f * sinf( 0.1234f - fTime * 2.5f ) * cosf( fTime * 5.0f ) + 0.5f ) *
                    ( 1.778f * ( fCycle - 1.0f ) * ( fCycle - 1.0f ) );
        fFade    = 1.0f;
    }   
    else
    if( fCycle < 2.00f )
    {
        fFade = 4.0f * ( 2.0f - fCycle );
    }

    // setup transformation matrices
    D3DXMatrixIdentity( &m_matWorld );
    m_pD3DDev->SetTransform( D3DTS_WORLD, &m_matWorld );

    D3DXMatrixTranslation( &m_matView, 
                           0.0f, 
                           -0.5f + 0.125f * sinf( 2.5f * (int) ( fTime / c_fTextLifeSpan ) ), 
                           0.825f );
    m_pD3DDev->SetTransform( D3DTS_VIEW, &m_matView);

    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DXToRadian( 90.0f ), GetAspectRatio(), 0.1f, 16.0f );
    m_pD3DDev->SetTransform( D3DTS_PROJECTION, &m_matProj );

    IDirect3DBaseTexture8* pkTexture( m_pkTextureManager->GetTexture( c_pcCreditsTexture ) );
    m_pD3DDev->SetTexture( 0, pkTexture );  

    // set render states
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );

    m_pD3DDev->SetVertexShader( D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE );
    m_pD3DDev->SetPixelShader( 0 );


    struct MsgVertex
    {
        D3DXVECTOR3  vPos;
        unsigned int uiCol;
        D3DXVECTOR2  vTex;
    };

    MsgVertex msgQuad[ 4 ];

    if( fStretch > 0.0f )
    {   
        unsigned int uiColor( (unsigned int) ( fFade * 255.0f ) );
        uiColor = ( uiColor << 24 ) + ( uiColor << 16 ) + ( uiColor << 8 ) + ( uiColor );

        fStretch /= 64.0f;
        for( int i( 0 ); i < 64; ++i )
        {
            msgQuad[ 0 ].vPos = D3DXVECTOR3( -1.0f + i * fStretch,  0.0625f, 0.0f );
            msgQuad[ 1 ].vPos = D3DXVECTOR3( -1.0f + i * fStretch, -0.0625f, 0.0f );
            msgQuad[ 2 ].vPos = D3DXVECTOR3(  1.0f + i * fStretch,  0.0625f, 0.0f );
            msgQuad[ 3 ].vPos = D3DXVECTOR3(  1.0f + i * fStretch, -0.0625f, 0.0f );

            msgQuad[ 0 ].vTex = D3DXVECTOR2( 0.0f,          fVertTexOffset );
            msgQuad[ 1 ].vTex = D3DXVECTOR2( 0.0f, 0.125f + fVertTexOffset );
            msgQuad[ 2 ].vTex = D3DXVECTOR2( 1.0f,          fVertTexOffset );
            msgQuad[ 3 ].vTex = D3DXVECTOR2( 1.0f, 0.125f + fVertTexOffset );

            msgQuad[ 0 ].uiCol = uiColor;
            msgQuad[ 1 ].uiCol = uiColor;
            msgQuad[ 2 ].uiCol = uiColor;        
            msgQuad[ 3 ].uiCol = uiColor;

            m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
            m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

            m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
            m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
            m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );

            m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0x08080808 );       

            m_pD3DDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, msgQuad, sizeof( MsgVertex ) );

            fStretch = -fStretch;
        }
    }
    else
    {
        msgQuad[ 0 ].vPos = D3DXVECTOR3( -1.0f,  0.0625f, 0.0f );
        msgQuad[ 1 ].vPos = D3DXVECTOR3( -1.0f, -0.0625f, 0.0f );
        msgQuad[ 2 ].vPos = D3DXVECTOR3(  1.0f,  0.0625f, 0.0f );
        msgQuad[ 3 ].vPos = D3DXVECTOR3(  1.0f, -0.0625f, 0.0f );
        
        msgQuad[ 0 ].vTex = D3DXVECTOR2( 0.0f,          fVertTexOffset );
        msgQuad[ 1 ].vTex = D3DXVECTOR2( 0.0f, 0.125f + fVertTexOffset );
        msgQuad[ 2 ].vTex = D3DXVECTOR2( 1.0f,          fVertTexOffset );
        msgQuad[ 3 ].vTex = D3DXVECTOR2( 1.0f, 0.125f + fVertTexOffset );
    
        msgQuad[ 0 ].uiCol = 0xFFFFFFFF;
        msgQuad[ 1 ].uiCol = 0xFFFFFFFF;
        msgQuad[ 2 ].uiCol = 0xFFFFFFFF;
        msgQuad[ 3 ].uiCol = 0xFFFFFFFF;

        m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

        m_pD3DDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, msgQuad, sizeof( MsgVertex ) );
    }

    SafeRelease( pkTexture );    
}
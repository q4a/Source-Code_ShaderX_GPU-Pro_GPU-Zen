///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CVolumetricLight.
 *
 * @file
 *  VolumetricLight.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  05.03.2002
 * @brief
 *  Implementation of \e Effects::CVolumetricLight.
 */



#include "StdAfx.h"
#include "VolumetricLight.h"
#include "VolumetricLightDialog.h"
#include "EffectException.h"
#include "ShaderSourceList.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"
#include "EffectTransition.h"
#include "Random.h"
#include "Clamp.h"
#include "resource.h"
#include "..\..\Help\ContextIDs.h"

#include <string>

#include "ShaderConstants.h"

namespace
{
    // wrap shader code in an anonymous namespace to 
    // avoid any possible conflicts with other modules as
    // shader assembler doesn't declare shader byte code arrays 
    // static when writing them to outfile

#   include "Shader\VolumetricLight.psh"
#   include "Shader\VolumetricLight.vsh"
#   include "Shader\TextPlane.psh"
#   include "Shader\TextPlane.vsh"
}



using namespace Effects;
using namespace EffectSystem;
using namespace Shared;
using namespace ScrambledArchive;
using namespace std;



const char c_pcEffectName[]   = "Volumetric Light";
const char c_pcLightMap[]     = "VolumetricLight\\Light.raw";
const char c_pcTextMap[]      = "VolumetricLight\\Text.raw";
const char c_pcTextTexture[]  = "VolumetricLight\\Text.jpg";
const char c_pcBeamTexture[]  = "VolumetricLight\\Beam.dds";
const char c_pcBassSpectrum[] = "VolumetricLight\\BassSpectrum.raw";
const char c_pcBackground[]   = "VolumetricLight\\DigitalBlackhole.jpg";

// We generate one big triangle strip to efficiently render all beams.
// To form one beam we need 4 vertices.
const unsigned int c_uiNumVertices( CVolumetricLight::NUM_BEAMS * 4 );

// Since we put the strips of multiple beams into one big strip to gain a considerable performance boost,
// we need to artificially insert two indices to connect two independent strips for every but the last one.
const unsigned int c_uiNumStripIndices( CVolumetricLight::NUM_BEAMS * ( 4 + 2 ) - 2 );

const unsigned int c_uiNumStripTriangles( c_uiNumStripIndices - 2 );

const unsigned int c_uiTextMapSize( 256 );

const unsigned int c_uiRenderToTextureSize( 512 );

const float CVolumetricLight::sc_fDefaultLightIntensity( 0.15f );
const float CVolumetricLight::sc_fDefaultLightZPos( 0.3f );



CVolumetricLight::CVolumetricLight( IDirect3DDevice8* pD3DDev,
                                    CScrambledArchive* pkScrambledArchive,
                                    CTextureManager* pkTextureManager,
                                    CEffectTransition* pkEffectTransition,
                                    bool bBrowserMode )
: CEffect( pD3DDev, pkScrambledArchive, pkTextureManager, pkEffectTransition, bBrowserMode )
, m_pkVertexBuffer( 0 )
, m_pkIndexBuffer( 0 )
, m_pkTextPlaneVertexBuffer( 0 )
, m_pkQuadVertexBuffer( 0 )
, m_pkRenderTexture( 0 )
, m_pkVolumetricLightSettingsDlg( 0 )
, m_fLightIntensity( sc_fDefaultLightIntensity )
, m_fLightZPos( sc_fDefaultLightZPos )
, m_dwVolumetricLightPSHandle( 0 )
, m_dwVolumetricLightVSHandle( 0 )
, m_dwTextPlanePSHandle( 0 )
, m_dwTextPlaneVSHandle( 0 )
, m_colTextMap()
, m_colBassSpectrum()
{
    try
    {
        ConfirmRequirements();

        // load light map
        vector< unsigned char > colLightMap;
        if( false == pkScrambledArchive->DecompressFile( c_pcLightMap, colLightMap ) ||
            sizeof( unsigned int ) * NUM_BEAMS != colLightMap.size() )
        {
            throw( CEffectException( "Failed to load light map!" ) );
        }

        // load text map
        if( false == pkScrambledArchive->DecompressFile( c_pcTextMap, m_colTextMap ) ||
            c_uiTextMapSize * c_uiTextMapSize < m_colTextMap.size() )
        {
            throw( CEffectException( "Failed to load text map!" ) );
        }

        // load texture texture
        if( false == m_pkTextureManager->LoadJPEGTexture( c_pcTextTexture, false, true, false ) )
        {
            throw( CEffectException( "Failed to load text texture!" ) );
        }

        // load beam texture
        if( false == m_pkTextureManager->LoadDDSTexture( c_pcBeamTexture, true ) )
        {
            throw( CEffectException( "Failed to load beam texture!" ) );
        }

        // load background texture
        if( false == m_pkTextureManager->LoadJPEGTexture( c_pcBackground, false, false, false ) )
        {
            throw( CEffectException( "Failed to load background texture!" ) );
        }

        if( false == m_bBrowserMode )
        {
            // load bass spectrum of music
            if( false == pkScrambledArchive->DecompressFile( c_pcBassSpectrum, m_colBassSpectrum )  )
            {
                throw( CEffectException( "Failed to load bass spectrum!" ) );
            }
        }

        // create vertex shader
        DWORD dwVolumetricLightDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),    // position
                D3DVSD_REG( 1, D3DVSDT_D3DCOLOR ),  // color
                D3DVSD_REG( 2, D3DVSDT_FLOAT1 ),    // 1D texture
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader(
                            dwVolumetricLightDecl,
                            dwVolumetricLightVertexShader,
                            &m_dwVolumetricLightVSHandle,
                            GetPreferedVertexShaderUsageFlag()
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwVolumetricLightPixelShader, &m_dwVolumetricLightPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // create vertex shader
        DWORD dwTextPlaneDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),    // position
                D3DVSD_REG( 1, D3DVSDT_FLOAT2 ),    // 2D texture
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader(
                            dwTextPlaneDecl,
                            dwTextPlaneVertexShader,
                            &m_dwTextPlaneVSHandle,
                            GetPreferedVertexShaderUsageFlag()
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwTextPlanePixelShader, &m_dwTextPlanePSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // initialize all app managed resources
        InitAppManagedResources();

        // create settings dialog if we are in browser mode
        if( false != m_bBrowserMode )
        {
            m_pkVolumetricLightSettingsDlg = new CVolumetricLightDialog();
            if( 0 == m_pkVolumetricLightSettingsDlg ||
                FALSE == m_pkVolumetricLightSettingsDlg->Create( IDD_VOLUMETRICLIGHT ) )
            {
                throw( CEffectException( "Failed to create effect settings dialog!" ) );
            }
        }

        // setup beams
        CRandom kRnd;
        for( int i( 0 ); i < NUM_BEAMS; ++i )
        {
            m_pLightBeams[ i ].vDir =
                D3DXVECTOR3( (float) kRnd.GetInRange( -0.01, 0.01 ) +
                             -0.5f + ( i % NUM_BEAMS_PER_DIM ) / (float) NUM_BEAMS_PER_DIM,
                             (float) kRnd.GetInRange( -0.01, 0.01 ) +
                             -0.5f + ( i / NUM_BEAMS_PER_DIM ) / (float) NUM_BEAMS_PER_DIM,
                             -1.0f );

            D3DXVec3Normalize( &m_pLightBeams[ i ].vDir, &m_pLightBeams[ i ].vDir );

            m_pLightBeams[ i ].uiColor = ( colLightMap[ i * 4 + 2 ] <<  0 ) +
                                         ( colLightMap[ i * 4 + 1 ] <<  8 ) +
                                         ( colLightMap[ i * 4 + 0 ] << 16 ) +
                                         ( colLightMap[ i * 4 + 3 ] << 24 );
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



CVolumetricLight::~CVolumetricLight()
{
    FreeAllResources();
}



void
CVolumetricLight::FreeAllResources()
{
    if( 0 != m_dwVolumetricLightVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwVolumetricLightVSHandle );
    }

    if( 0 != m_dwVolumetricLightPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwVolumetricLightPSHandle );
    }

    if( 0 != m_dwTextPlaneVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwTextPlaneVSHandle );
    }

    if( 0 != m_dwTextPlanePSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwTextPlanePSHandle );
    }

    if( 0 != m_pkVolumetricLightSettingsDlg )
    {
        m_pkVolumetricLightSettingsDlg->DestroyWindow();
        delete m_pkVolumetricLightSettingsDlg;
    }

    CEffect::FreeAllResources();
}



const char* const
CVolumetricLight::GetEffectName()
{
    return( c_pcEffectName );
}



bool
CVolumetricLight::IsEffectDialogVisible()
{
    DBG_ASSERT( false != m_bBrowserMode );
    return( ( false != m_pkVolumetricLightSettingsDlg->IsWindowVisible() ) ? true : false );
}



void
CVolumetricLight::ShowEffectDialog( bool bShow )
{
    DBG_ASSERT( false != m_bBrowserMode );
    int iShowCmd( ( false != bShow ) ? SW_SHOW : SW_HIDE );
    m_pkVolumetricLightSettingsDlg->ShowWindow( iShowCmd );
}



unsigned int
CVolumetricLight::GetHTMLHelpID()
{
    return( ID_HELPCONTEXT_VOLUMETRICLIGHT );
}



void
CVolumetricLight::InitAppManagedResources()
{
    // create render texture
    DBG_ASSERT( 0 == m_pkRenderTexture );
    if( FAILED( D3DXCreateTexture(
                                    m_pD3DDev,                                  // device
                                    c_uiRenderToTextureSize,                   // width
                                    c_uiRenderToTextureSize,                   // height
                                    1,                                          // num of mip maps
                                    D3DUSAGE_RENDERTARGET,                      // usage
                                    D3DFMT_A8R8G8B8,                            // format
                                    D3DPOOL_DEFAULT,                            // pool
                                    &m_pkRenderTexture                          // texture
                                ) ) )
    {
        throw( CEffectException( "Failed to create texture as render target!" ) );
    }

    // create dynamic vertex buffer
    DBG_ASSERT( 0 == m_pkVertexBuffer );
    if( FAILED( m_pD3DDev->CreateVertexBuffer(
                        c_uiNumVertices * sizeof( SVertex ),
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                        0,
                        D3DPOOL_DEFAULT,
                        &m_pkVertexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic vertex buffer!" ) );
    }

    // create and initialize dynamic index buffer
    DBG_ASSERT( 0 == m_pkIndexBuffer );
    if( FAILED( m_pD3DDev->CreateIndexBuffer(
                        c_uiNumStripIndices * sizeof( WORD ),
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                        D3DFMT_INDEX16,
                        D3DPOOL_DEFAULT,
                        &m_pkIndexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic index buffer!" ) );
    }

    // lock the entire index buffer
    WORD* pIndices;
    if( FAILED( m_pkIndexBuffer->Lock(
                      0,
                      0,
                      (BYTE **) &pIndices,
                      D3DLOCK_DISCARD  ) ) )
    {
        throw( CEffectException( "Failed to lock dynamic index buffer!" ) );
    }

    // set indices
    int iIndex( 0 );
    for( int a( 0 ); a < NUM_BEAMS; ++a )
    {
        for( int i( 0 ); i < 4; ++i, ++pIndices, ++iIndex )
        {
            *pIndices = iIndex;
        }

        // connect two strips by inserting last index of previous strip
        // and first index of current strip => creates four degenerated triangles
        if(  NUM_BEAMS - 1 > a )
        {
            pIndices[ 0 ] = iIndex - 1;
            pIndices[ 1 ] = iIndex;

            pIndices += 2;
        }
    }

    m_pkIndexBuffer->Unlock();

    // create dynamic vertex buffer
    DBG_ASSERT( 0 == m_pkTextPlaneVertexBuffer );
    if( FAILED( m_pD3DDev->CreateVertexBuffer(
                        4 * sizeof( STextPlaneVertex ),
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                        0,
                        D3DPOOL_DEFAULT,
                        &m_pkTextPlaneVertexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic vertex buffer!" ) );
    }

    // lock the entire vertex buffer
    STextPlaneVertex* pVertices;
    if( FAILED( m_pkTextPlaneVertexBuffer->Lock(
                      0,
                      0,
                      (BYTE **) &pVertices,
                      D3DLOCK_DISCARD  ) ) )
    {
        throw( CEffectException( "Failed to lock dynamic vertex buffer!" ) );
    }

    pVertices[ 0 ].vPos = D3DXVECTOR3( -0.5f,
                                        0.5f,
                                        0.0f );
    pVertices[ 0 ].vTex = D3DXVECTOR2( 0.0f, 0.0f );


    pVertices[ 1 ].vPos = D3DXVECTOR3( -0.5f,
                                       -0.5f,
                                        0.0f );
    pVertices[ 1 ].vTex = D3DXVECTOR2( 0.0f, 1.0f );


    pVertices[ 2 ].vPos = D3DXVECTOR3(  0.5f,
                                        0.5f,
                                        0.0f );
    pVertices[ 2 ].vTex = D3DXVECTOR2( 1.0f, 0.0f );


    pVertices[ 3 ].vPos = D3DXVECTOR3(  0.5f,
                                       -0.5f,
                                        0.0f );
    pVertices[ 3 ].vTex = D3DXVECTOR2( 1.0f, 1.0f );

    m_pkTextPlaneVertexBuffer->Unlock();

    // create dynamic vertex buffer
    DBG_ASSERT( 0 == m_pkQuadVertexBuffer );
    if( FAILED( m_pD3DDev->CreateVertexBuffer(
                        4 * sizeof( SQuadVertex ),
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                        D3DFVF_XYZRHW | D3DFVF_TEX1,
                        D3DPOOL_DEFAULT,
                        &m_pkQuadVertexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic vertex buffer!" ) );
    }
}



void
CVolumetricLight::FreeAppManagedResources()
{
    SafeRelease( m_pkVertexBuffer );
    SafeRelease( m_pkIndexBuffer );
    SafeRelease( m_pkTextPlaneVertexBuffer );
    SafeRelease( m_pkQuadVertexBuffer );
    SafeRelease( m_pkRenderTexture );
}



void
CVolumetricLight::ConfirmRequirements()
{
    // make sure the index buffer for all triangle strips doesn't exceed 65536
    DBG_ASSERT_STATIC( c_uiNumStripIndices <= 65536, Too_many_indices_for_triangle_strip );

    // make sure the vertex buffer can hold c_uiNumVertices vertices
    DBG_ASSERT_STATIC( c_uiNumVertices <= 65536, Too_many_vertices_for_vertex_buffer );


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

    // check multi texturing capability
    if( 2 > sCaps.MaxSimultaneousTextures )
    {
        throw( CEffectException( "Device doesn't support 2 simultaneous textures!" ) );
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

    // check support for rendering to a texture
    if( false == IsTextureFormatSupported( D3DFMT_A8R8G8B8, D3DRTYPE_TEXTURE, D3DUSAGE_RENDERTARGET  ) )
    {
        throw( CEffectException( "Device doesn't support rendering to texture format D3DFMT_A8R8G8B8!" ) );
    }
}



void
CVolumetricLight::SetShaderConstants()
{
    // vertex shader constants
    D3DXMATRIX matTemp;
    GetWorldViewProjection( matTemp );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDVIEWPROJ_0, &matTemp, 4 );
}



unsigned int
CVolumetricLight::GetColorIntensity( unsigned int uiColor, float fIntensity ) const
{
    //unsigned char ucAlpha( ( uiColor >> 24 ) & 0xFF );
    //unsigned char ucRed(   ( uiColor >> 16 ) & 0xFF );
    //unsigned char ucGreen( ( uiColor >>  8 ) & 0xFF );
    //unsigned char ucBlue(  ( uiColor >>  0 ) & 0xFF );

    //return( ( (int) ( m_fLightIntensity * fIntensity * ucAlpha ) << 24 ) +
    //        ( (int) ( m_fLightIntensity * fIntensity * ucRed   ) << 16 ) +
    //        ( (int) ( m_fLightIntensity * fIntensity * ucGreen ) <<  8 ) +
    //        ( (int) ( m_fLightIntensity * fIntensity * ucBlue  ) <<  0 ) );

    return( D3DXCOLOR( uiColor ) * fIntensity * m_fLightIntensity );
}



void
CVolumetricLight::RenderBeamsToTexture( float fTime )
{
    // save old render target
    IDirect3DSurface8* pkBackBufferColor;
    m_pD3DDev->GetRenderTarget( &pkBackBufferColor );

    IDirect3DSurface8* pkBackBufferDepth;
    m_pD3DDev->GetDepthStencilSurface( &pkBackBufferDepth );

    // set beams render texture as new render target
    IDirect3DSurface8* pkRenderSurface;
    m_pkRenderTexture->GetSurfaceLevel( 0, &pkRenderSurface );
    m_pD3DDev->SetRenderTarget( pkRenderSurface, 0 );
    pkRenderSurface->Release();

    m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET, 0x0, 1.0f, 0 );

    D3DXVECTOR3 vLightPos( -0.5f * cosf( fTime ), 0.28f * cosf( 0.33f * fTime ), m_fLightZPos );

    SetShaderConstants();

    // render beams I
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );
    m_pD3DDev->SetIndices( m_pkIndexBuffer, 0 );
    m_pD3DDev->SetVertexShader( m_dwVolumetricLightVSHandle );
    m_pD3DDev->SetPixelShader( m_dwVolumetricLightPSHandle );

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    IDirect3DBaseTexture8* pkBeamTexture( m_pkTextureManager->GetTexture( c_pcBeamTexture ) );
    m_pD3DDev->SetTexture( 0, pkBeamTexture );

    // lock vertex buffer to write new data
    SVertex *pVertices;
    m_pkVertexBuffer->Lock(
                            0,
                            0,
                            (BYTE **) &pVertices,
                            D3DLOCK_DISCARD
                            );

    // render part of beams that are behind the xy plane
    for( unsigned int i( 0 ); i < NUM_BEAMS; ++i )
    {
        // calculate beam width vector based on beam direction and current view
        D3DXVec3Cross(
                        &m_pLightBeams[ i ].vWidth,
                        &m_pLightBeams[ i ].vDir,
                        &D3DXVECTOR3( m_matView( 0, 2 ), m_matView( 1, 2 ), m_matView( 2, 2 ) )
                        );
        D3DXVec3Normalize( &m_pLightBeams[ i ].vWidth, &m_pLightBeams[ i ].vWidth );
        m_pLightBeams[ i ].vWidth *= 0.02f;

        // setup first to vertices of light beam
        pVertices[ 0 ].vPos = vLightPos;
        pVertices[ 0 ].vTex = 0.5f;
        pVertices[ 0 ].dwColor = GetColorIntensity( m_pLightBeams[ i ].uiColor, 1.0f );

        pVertices[ 1 ].vPos = vLightPos;
        pVertices[ 1 ].vTex = 0.5f;
        pVertices[ 1 ].dwColor = GetColorIntensity( m_pLightBeams[ i ].uiColor, 1.0f );

        // raycast on xy plane
        D3DXVECTOR3 vDir( m_pLightBeams[ i ].vDir );
        float fT( -vLightPos.z / vDir.z );
        m_pLightBeams[ i ].fX = fT * vDir.x + vLightPos.x;
        m_pLightBeams[ i ].fY = fT * vDir.y + vLightPos.y;
        m_pLightBeams[ i ].fIntensity = Clamp( 1.0f / fT, 0.0f, 1.0f );

        D3DXVECTOR3 vPos( m_pLightBeams[ i ].fX, m_pLightBeams[ i ].fY, 0.0f );

        // setup next two vertices
        pVertices[ 2 ].vPos = vPos + m_pLightBeams[ i ].vWidth;
        pVertices[ 2 ].vTex = 0.0f;
        pVertices[ 2 ].dwColor = GetColorIntensity( m_pLightBeams[ i ].uiColor, m_pLightBeams[ i ].fIntensity );

        pVertices[ 3 ].vPos = vPos - m_pLightBeams[ i ].vWidth;
        pVertices[ 3 ].vTex = 1.0f;
        pVertices[ 3 ].dwColor = GetColorIntensity( m_pLightBeams[ i ].uiColor, m_pLightBeams[ i ].fIntensity );

        // advance pointer
        pVertices += 4;
    }

    // unlock vertex buffer and render beams
    m_pkVertexBuffer->Unlock();
    m_pD3DDev->DrawIndexedPrimitive(
                                        D3DPT_TRIANGLESTRIP,
                                        0,
                                        c_uiNumVertices,
                                        0,
                                        c_uiNumStripTriangles
                                    );


    // render text plane (xy)
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );

    m_pD3DDev->SetStreamSource( 0, m_pkTextPlaneVertexBuffer, sizeof( STextPlaneVertex ) );
    m_pD3DDev->SetVertexShader( m_dwTextPlaneVSHandle );
    m_pD3DDev->SetPixelShader( m_dwTextPlanePSHandle );

    IDirect3DBaseTexture8* pkTextTexture( m_pkTextureManager->GetTexture( c_pcTextTexture ) );
    m_pD3DDev->SetTexture( 0, pkTextTexture );

    m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );


    // render beams II
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );
    m_pD3DDev->SetIndices( m_pkIndexBuffer, 0 );
    m_pD3DDev->SetVertexShader( m_dwVolumetricLightVSHandle );
    m_pD3DDev->SetPixelShader( m_dwVolumetricLightPSHandle );

    m_pD3DDev->SetTexture( 0, pkBeamTexture );


    // lock vertex buffer to write new data
    m_pkVertexBuffer->Lock(
                            0,
                            0,
                            (BYTE **) &pVertices,
                            D3DLOCK_DISCARD
                            );

    // render part of visible beams that are in front of the xy plane
    for( unsigned int i( 0 ); i < NUM_BEAMS; ++i )
    {
        D3DXVECTOR3 vPos( m_pLightBeams[ i ].fX, m_pLightBeams[ i ].fY, 0.0f );

        // check if raycast result is within our transparency map
        if( m_pLightBeams[ i ].fX >= -0.5f && m_pLightBeams[ i ].fX <= 0.5f &&
            m_pLightBeams[ i ].fY >= -0.5f && m_pLightBeams[ i ].fY <= 0.5f )
        {
            // get texture coords
            int iX( (int) ( ( m_pLightBeams[ i ].fX + 0.5f ) * ( c_uiTextMapSize - 1 ) ) );
            int iY( (int) ( ( m_pLightBeams[ i ].fY + 0.5f ) * ( c_uiTextMapSize - 1 ) ) );

            // get transparency value and use it to modulate the beams light intensity
            unsigned char ucTransparency( m_colTextMap[ iY * c_uiTextMapSize + iX ] );
            float fIntensity( m_pLightBeams[ i ].fIntensity * ucTransparency / 255.0f );

            pVertices[ 0 ].dwColor = GetColorIntensity( m_pLightBeams[ i ].uiColor, fIntensity );
            pVertices[ 1 ].dwColor = GetColorIntensity( m_pLightBeams[ i ].uiColor, fIntensity );
        }
        else
        {
            pVertices[ 0 ].dwColor = GetColorIntensity( m_pLightBeams[ i ].uiColor, m_pLightBeams[ i ].fIntensity );
            pVertices[ 1 ].dwColor = GetColorIntensity( m_pLightBeams[ i ].uiColor, m_pLightBeams[ i ].fIntensity );
        }

        // setup vertices
        pVertices[ 0 ].vPos = vPos + m_pLightBeams[ i ].vWidth;
        pVertices[ 0 ].vTex = 0.0f;

        pVertices[ 1 ].vPos = vPos - m_pLightBeams[ i ].vWidth;
        pVertices[ 1 ].vTex = 1.0f;

        vPos += 2 * m_pLightBeams[ i ].vDir;

        pVertices[ 2 ].vPos = vPos + 2 * m_pLightBeams[ i ].vWidth;
        pVertices[ 2 ].vTex = 0.0f;
        pVertices[ 2 ].dwColor = 0;

        pVertices[ 3 ].vPos = vPos - 2 * m_pLightBeams[ i ].vWidth;
        pVertices[ 3 ].vTex = 1.0f;
        pVertices[ 3 ].dwColor = 0;

        // advance pointer
        pVertices += 4;
    }


    // unlock vertex buffer and render beams
    m_pkVertexBuffer->Unlock();
    m_pD3DDev->DrawIndexedPrimitive(
                                        D3DPT_TRIANGLESTRIP,
                                        0,
                                        c_uiNumVertices,
                                        0,
                                        c_uiNumStripTriangles
                                    );


    SafeRelease( pkTextTexture );
    SafeRelease( pkBeamTexture );

    // reset render target
    m_pD3DDev->SetRenderTarget( pkBackBufferColor, pkBackBufferDepth );
    pkBackBufferColor->Release();
    pkBackBufferDepth->Release();
}



void
CVolumetricLight::DrawBeams()
{
    m_pD3DDev->SetTexture( 0, m_pkRenderTexture );

    // get viewport settings to render quad
    D3DVIEWPORT8 sViewPort;
    m_pD3DDev->GetViewport( &sViewPort );

    // lock vertex buffer and write data
    SQuadVertex *pVertices;
    m_pkQuadVertexBuffer->Lock( 0,
                            0,
                            (BYTE **) &pVertices,
                            D3DLOCK_DISCARD );

    pVertices[ 0 ].vPos = D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f );
    pVertices[ 0 ].vTex = D3DXVECTOR2( 0.0f, 0.0f );

    pVertices[ 1 ].vPos = D3DXVECTOR4( 0.0f, (float) sViewPort.Height , 1.0f, 1.0f );
    pVertices[ 1 ].vTex = D3DXVECTOR2( 0.0f, 1.0f );

    pVertices[ 2 ].vPos = D3DXVECTOR4( (float) sViewPort.Width, 0.0f, 1.0f, 1.0f );
    pVertices[ 2 ].vTex = D3DXVECTOR2( 1.0f, 0.0f );

    pVertices[ 3 ].vPos = D3DXVECTOR4( (float) sViewPort.Width, (float) sViewPort.Height , 1.0f, 1.0f );
    pVertices[ 3 ].vTex = D3DXVECTOR2( 1.0f, 1.0f );

    m_pkQuadVertexBuffer->Unlock();

    // draw beams from texture
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    m_pD3DDev->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    m_pD3DDev->SetPixelShader( 0 );

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    // since we use pre-transformed triangle data
    // there is no need to set transformation matrices
    m_pD3DDev->SetStreamSource( 0, m_pkQuadVertexBuffer, sizeof( SQuadVertex ) );
    m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}



void
CVolumetricLight::Update( float fTime )
{
    if( false != m_bBrowserMode )
    {
        fTime = m_pkVolumetricLightSettingsDlg->GetTime();
    }
    else
    {
        fTime = GetRelativeTime( fTime );

        float fBassValue( GetBassLineValue( fTime,
                                            &m_colBassSpectrum[ 0 ],
                                            (unsigned int) m_colBassSpectrum.size() ) );

        // change light intensity according to bass line
        m_fLightIntensity *= 0.7f;
        m_fLightIntensity += 0.3f * sc_fDefaultLightIntensity * ( 0.5f + 0.5f * fBassValue );

        m_fLightZPos *= 0.7f;
        m_fLightZPos += 0.3f * ( sc_fDefaultLightZPos - 0.1f * ( 0.5f + 0.5f * fBassValue ) );
    }

    D3DXMatrixIdentity( &m_matWorld );

    D3DXMatrixLookAtLH( &m_matView,
                        &D3DXVECTOR3( 0.0f, 0.0f, -0.5f ),
                        &D3DXVECTOR3( 0.0f, 0.0f,  0.0f ),
                        &D3DXVECTOR3( 0.0f, 1.0f,  0.0f ) );

    D3DXMatrixPerspectiveFovLH( &m_matProj,
        D3DXToRadian( 90.0f ), GetAspectRatio(), 0.001f, 4.0f );

    if( SUCCEEDED( m_pD3DDev->BeginScene() ) )
    {
        DrawBackground( c_pcBackground );

        // set basic render states
        m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
        m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
        m_pD3DDev->SetRenderState( D3DRS_ZENABLE, FALSE );

        m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE, TRUE );
        m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_LOCALVIEWER, FALSE );

        m_pD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_CLIPPING, TRUE );

        RenderBeamsToTexture( 0.85f * fTime );
        DrawBeams();


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
                            255, 255, 255
                          );
            }
        }


        m_pD3DDev->SetTexture( 0, 0 );

        m_pD3DDev->EndScene();
        m_pD3DDev->Present( 0, 0, 0, 0 );
    }
}



bool
CVolumetricLight::GetShaderSources( SShaderSourceList& colShaders )
{
    const SShaderDescriptor c_shaderDesc[] =
    {
        {
            "Volumetric Light",
            "VolumetricLight\\VolumetricLight.vsa",
            "VolumetricLight\\VolumetricLight.psa"
        },

        {
            "Text Plane",
            "VolumetricLight\\TextPlane.vsa",
            "VolumetricLight\\TextPlane.psa"
        }
    };

    return( LoadShaderSources( colShaders, c_shaderDesc, 2 ) );
}

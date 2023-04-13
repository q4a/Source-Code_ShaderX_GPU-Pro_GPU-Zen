///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::Beams.
 *
 * @file
 *  Beams.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Implementation of \e Effects::Beams.
 */



#include "StdAfx.h"
#include "Beams.h"
#include "BeamsDialog.h"
#include "EffectException.h"
#include "ShaderSourceList.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"
#include "EffectTransition.h"
#include "Geometry.h"
#include "resource.h"
#include "..\..\Help\ContextIDs.h"

#include <string>

#include "RenderSunShaderConstants.h"
#include "BeamsShaderConstants.h"

namespace
{
    // wrap shader code in an anonymous namespace to 
    // avoid any possible conflicts with other modules as
    // shader assembler doesn't declare shader byte code arrays 
    // static when writing them to outfile

#   include "Shader\RenderSun.psh"
#   include "Shader\RenderSun.vsh"

#   include "Shader\Beams.psh"
}



using namespace Effects;
using namespace EffectSystem;
using namespace Shared;
using namespace ScrambledArchive;
using namespace std;



const char c_pcEffectName[]   = "Beams";
const char c_pcSunTexture[]   = "Beams\\SunSurface.jpg";
const char c_pcStarsTexture[] = "Beams\\Stars.jpg";



const unsigned int c_uiRenderToTextureSize( 256 );



CBeams::CBeams( IDirect3DDevice8* pD3DDev, 
                CScrambledArchive* pkScrambledArchive,
                CTextureManager* pkTextureManager,
                CEffectTransition* pkEffectTransition,
                bool bBrowserMode )
: CEffect( pD3DDev, pkScrambledArchive, pkTextureManager, pkEffectTransition, bBrowserMode )
, m_pkSphere( 0 )
, m_pkSunRenderTexture( 0 )
, m_pkBeamsRenderTexture( 0 )
, m_pkVertexBuffer( 0 )
, m_pkBeamSettingsDlg( 0 )
, m_dwRenderSunPSHandle( 0 )
, m_dwRenderSunVSHandle( 0 )
, m_dwBeamsPSHandle( 0 )
{
    try
    {
        ConfirmRequirements();

        // load suns surface texture
        if( false == m_pkTextureManager->LoadJPEGTexture( c_pcSunTexture, true, true, false ) )
        {
            throw( CEffectException( "Failed to load sun texture!" ) );
        }

        // load stars backdrop texture
        if( false == m_pkTextureManager->LoadJPEGTexture( c_pcStarsTexture, false, true, false ) )
        {
            throw( CEffectException( "Failed to load stars texture!" ) );
        }

        // create a sphere mesh used for rendering the sun and the backdrop
        if( false == CGeometry::CreateSphere( 1.0f, 30, 30, m_pD3DDev, m_pkSphere, 6.0f, 6.0f ) )
        {
            throw( CEffectException( "Failed to create mesh!" ) );
        }

        // create vertex shader
        DWORD dwRenderSunDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // position 
                D3DVSD_REG( 1, D3DVSDT_FLOAT2 ), // 2D texture
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader( 
                            dwRenderSunDecl, 
                            dwRenderSunVertexShader, 
                            &m_dwRenderSunVSHandle, 
                            GetPreferedVertexShaderUsageFlag() 
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwRenderSunPixelShader, &m_dwRenderSunPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }
        
        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwBeamsPixelShader, &m_dwBeamsPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // initialize all app managed resources
        InitAppManagedResources();

        // create settings dialog if we are in browser mode
        if( false != m_bBrowserMode )
        {
            m_pkBeamSettingsDlg = new CBeamsDialog();
            if( 0 == m_pkBeamSettingsDlg ||
                FALSE == m_pkBeamSettingsDlg->Create( IDD_BEAMS ) )
            {
                throw( CEffectException( "Failed to create effect settings dialog!" ) );
            }
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



CBeams::~CBeams()
{
    FreeAllResources();
}



void
CBeams::FreeAllResources()
{
    if( 0 != m_dwRenderSunVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwRenderSunVSHandle );
    }

    if( 0 != m_dwRenderSunPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwRenderSunPSHandle );
    }

    if( 0 != m_dwBeamsPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwBeamsPSHandle );
    }

    if( 0 != m_pkBeamSettingsDlg )
    {
        m_pkBeamSettingsDlg->DestroyWindow();
        delete m_pkBeamSettingsDlg;
    }

    SafeRelease( m_pkSphere );

    CEffect::FreeAllResources();
}



const char* const 
CBeams::GetEffectName()
{
    return( c_pcEffectName );
}



bool 
CBeams::IsEffectDialogVisible()
{
    DBG_ASSERT( false != m_bBrowserMode );
    return( ( false != m_pkBeamSettingsDlg->IsWindowVisible() ) ? true : false );
}



void 
CBeams::ShowEffectDialog( bool bShow )
{
    DBG_ASSERT( false != m_bBrowserMode );
    int iShowCmd( ( false != bShow ) ? SW_SHOW : SW_HIDE );
    m_pkBeamSettingsDlg->ShowWindow( iShowCmd );
}



unsigned int 
CBeams::GetHTMLHelpID()
{
    return( ID_HELPCONTEXT_BEAMS );
}



void 
CBeams::InitAppManagedResources()
{
    // create render texture
    DBG_ASSERT( 0 == m_pkSunRenderTexture );
    if( FAILED( D3DXCreateTexture(
                                    m_pD3DDev,                                  // device
                                    c_uiRenderToTextureSize,                   // width
                                    c_uiRenderToTextureSize,                   // height
                                    1,                                          // num of mip maps
                                    D3DUSAGE_RENDERTARGET,                      // usage
                                    D3DFMT_A8R8G8B8,                            // format
                                    D3DPOOL_DEFAULT,                            // pool
                                    &m_pkSunRenderTexture                       // texture
                                ) ) )
    {
        throw( CEffectException( "Failed to create texture as render target!" ) );
    }

    // create render texture
    DBG_ASSERT( 0 == m_pkBeamsRenderTexture );
    if( FAILED( D3DXCreateTexture(
                                    m_pD3DDev,                                  // device
                                    c_uiRenderToTextureSize,                   // width
                                    c_uiRenderToTextureSize,                   // height
                                    1,                                          // num of mip maps
                                    D3DUSAGE_RENDERTARGET,                      // usage
                                    D3DFMT_A8R8G8B8,                            // format
                                    D3DPOOL_DEFAULT,                            // pool
                                    &m_pkBeamsRenderTexture                     // texture
                                ) ) )
    {
        throw( CEffectException( "Failed to create texture as render target!" ) );
    }

    // create dynamic vertex buffer
    DBG_ASSERT( 0 == m_pkVertexBuffer );
    if( FAILED( m_pD3DDev->CreateVertexBuffer( 
                        4 * sizeof( SVertex ), 
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
                        0, 
                        D3DPOOL_DEFAULT,
                        &m_pkVertexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic vertex buffer!" ) );
    }
}



void 
CBeams::FreeAppManagedResources()
{   
    SafeRelease( m_pkSunRenderTexture );
    SafeRelease( m_pkBeamsRenderTexture );    
    SafeRelease( m_pkVertexBuffer ); 
}



void 
CBeams::ConfirmRequirements()
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

    // check multi texturing capability
    if( 4 > sCaps.MaxSimultaneousTextures )
    {
        throw( CEffectException( "Device doesn't support 4 simultaneous textures!" ) );
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
CBeams::RenderSphere()
{
    IDirect3DVertexBuffer8* pkVB;       
    m_pkSphere->GetVertexBuffer( &pkVB );
    
    IDirect3DIndexBuffer8* pkIB;
    m_pkSphere->GetIndexBuffer( &pkIB );

    m_pD3DDev->SetStreamSource( 0, pkVB, sizeof( SSphereVertex ) );    
    m_pD3DDev->SetIndices( pkIB, 0 );   
    m_pD3DDev->DrawIndexedPrimitive( 
                                        D3DPT_TRIANGLELIST, 
                                        0, 
                                        m_pkSphere->GetNumVertices(), 
                                        0, 
                                        m_pkSphere->GetNumFaces() 
                                    );
    pkVB->Release();
    pkIB->Release();
}



void 
CBeams::SetRenderSunShaderConstants( float fTime )
{
    // vertex shader constants    
    D3DXMATRIX matTemp;
    GetWorldViewProjection( matTemp );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDVIEWPROJ_0, &matTemp, 4 ); 

    // some constants
    m_pD3DDev->SetVertexShaderConstant( CV_CONSTANTS, &D3DXVECTOR4( 0.0f, 0.5f, 1.0f, 2.0f ), 1 ); 

    // texture scale factor
    m_pD3DDev->SetVertexShaderConstant( CV_TEXSCALE, &D3DXVECTOR4( 0.5f, 0.5f, 0.0f, 0.0f ), 1 ); 

    m_pD3DDev->SetVertexShaderConstant( CV_TEXOFFSET_0, 
        &D3DXVECTOR4(                + 0.000f,                  0.000f, 0.0f, 0.0f ), 1 ); 
    m_pD3DDev->SetVertexShaderConstant( CV_TEXOFFSET_1, 
        &D3DXVECTOR4( -0.11f * fTime + 0.123f,                  0.231f, 0.0f, 0.0f ), 1 ); 
    m_pD3DDev->SetVertexShaderConstant( CV_TEXOFFSET_2, 
        &D3DXVECTOR4(  0.07f * fTime + 0.312f,                - 0.321f, 0.0f, 0.0f ), 1 ); 
    m_pD3DDev->SetVertexShaderConstant( CV_TEXOFFSET_3, 
        &D3DXVECTOR4(                - 0.213f, -0.12f * fTime - 0.132f, 0.0f, 0.0f ), 1 ); 

    // pixel shader constants
    m_pD3DDev->SetPixelShaderConstant( CP_SURFACE_BLEND_VALUE, D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 0.25f ), 1 );
}



void
CBeams::RenderSun( float fTime )
{
    // save old render target 
    IDirect3DSurface8* pkBackBufferColor; 
    m_pD3DDev->GetRenderTarget( &pkBackBufferColor );

    IDirect3DSurface8* pkBackBufferDepth; 
    m_pD3DDev->GetDepthStencilSurface( &pkBackBufferDepth );

    // set sun render texture as new render target
    IDirect3DSurface8* pkRenderSurface;
    m_pkSunRenderTexture->GetSurfaceLevel( 0, &pkRenderSurface );
    m_pD3DDev->SetRenderTarget( pkRenderSurface, 0 );
    pkRenderSurface->Release();

    // render sun to texture
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // get suns surface texture
    IDirect3DBaseTexture8* pkSurfaceTexture( m_pkTextureManager->GetTexture( c_pcSunTexture ) );

    // set textures, filter and address modes
    for( unsigned int i( 0 ); i < 4; ++i )
    {
        m_pD3DDev->SetTextureStageState( i, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( i, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

        m_pD3DDev->SetTextureStageState( i, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
        m_pD3DDev->SetTextureStageState( i, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

        m_pD3DDev->SetTexture( i, pkSurfaceTexture );   
    }

    // set shaders
    m_pD3DDev->SetVertexShader( m_dwRenderSunVSHandle );
    m_pD3DDev->SetPixelShader( m_dwRenderSunPSHandle );
    SetRenderSunShaderConstants( fTime );
 
    // render sun
    m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET, 0x0, 1.0f, 0 );
    RenderSphere();

    SafeRelease( pkSurfaceTexture );

    // reset render target
    m_pD3DDev->SetRenderTarget( pkBackBufferColor, pkBackBufferDepth );   
    pkBackBufferColor->Release();
    pkBackBufferDepth->Release();
}



void
CBeams::RenderBeams()
{
    // save old render target 
    IDirect3DSurface8* pkBackBufferColor; 
    m_pD3DDev->GetRenderTarget( &pkBackBufferColor );

    IDirect3DSurface8* pkBackBufferDepth; 
    m_pD3DDev->GetDepthStencilSurface( &pkBackBufferDepth );

    // set beams render texture as new render target
    IDirect3DSurface8* pkRenderSurface;
    m_pkBeamsRenderTexture->GetSurfaceLevel( 0, &pkRenderSurface );
    m_pD3DDev->SetRenderTarget( pkRenderSurface, 0 );
    pkRenderSurface->Release();

    // render beams to texture
    m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET, 0x0, 1.0f, 0 );

    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    m_pD3DDev->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    m_pD3DDev->SetPixelShader( m_dwBeamsPSHandle );

    m_pD3DDev->SetTexture( 0, m_pkSunRenderTexture );   

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    D3DVIEWPORT8 sViewPort;
    m_pD3DDev->GetViewport( &sViewPort );

    // do a radial blur type of effect to fake beams
    for( unsigned int i( 0 ); i < NUM_BLURSTEPS; ++i ) 
    {
        // set shader constants
        float fTransparency( 0.125f * (float) ( NUM_BLURSTEPS - i ) / (float) NUM_BLURSTEPS );        
        m_pD3DDev->SetPixelShaderConstant( CP_TRANSPARENCY_VALUE, 
            D3DXVECTOR4( fTransparency, fTransparency, fTransparency, fTransparency), 1 );

        // get texture offset to fake blur
        float fUVOffset( i * 0.0075f );

        // lock vertex buffer and write data for one quad
        SVertex *pVertices;
        m_pkVertexBuffer->Lock( 0,
                                0,
                                (BYTE **) &pVertices,
                                D3DLOCK_DISCARD );

        pVertices[ 0 ].vPos = D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f );
        pVertices[ 0 ].vTex = D3DXVECTOR2( fUVOffset, fUVOffset );

        pVertices[ 1 ].vPos = D3DXVECTOR4( 0.0f, (float) sViewPort.Height , 1.0f, 1.0f );
        pVertices[ 1 ].vTex = D3DXVECTOR2( fUVOffset, 1.0f - fUVOffset);

        pVertices[ 2 ].vPos = D3DXVECTOR4( (float) sViewPort.Width, 0.0f, 1.0f, 1.0f );
        pVertices[ 2 ].vTex = D3DXVECTOR2( 1.0f - fUVOffset, fUVOffset );

        pVertices[ 3 ].vPos = D3DXVECTOR4( (float) sViewPort.Width, (float) sViewPort.Height , 1.0f, 1.0f );
        pVertices[ 3 ].vTex = D3DXVECTOR2( 1.0f - fUVOffset, 1.0f - fUVOffset );

        m_pkVertexBuffer->Unlock();

        // render quad and blend it against background
        m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );    
        m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }

    // reset render target
    m_pD3DDev->SetRenderTarget( pkBackBufferColor, pkBackBufferDepth );
    pkBackBufferColor->Release();
    pkBackBufferDepth->Release();
}



void 
CBeams::DrawBackdrop()
{
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // get stars backdrop texture
    IDirect3DBaseTexture8* pkStarsTexture( m_pkTextureManager->GetTexture( c_pcStarsTexture ) );
    m_pD3DDev->SetTexture( 0, pkStarsTexture ); 

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    // set shaders
    m_pD3DDev->SetVertexShader( D3DFVF_XYZ | D3DFVF_TEX1 );
    m_pD3DDev->SetPixelShader( 0 );

    // render backdrop
    D3DXMatrixScaling( &m_matWorld, 8.0f, 8.0f, 8.0f );
    m_pD3DDev->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pD3DDev->SetTransform( D3DTS_VIEW, &m_matView);
    m_pD3DDev->SetTransform( D3DTS_PROJECTION, &m_matProj );

    RenderSphere();

    SafeRelease( pkStarsTexture );

    // mask out sun area
    D3DXMatrixScaling( &m_matWorld, 1.0f, 1.0f, 1.0f );
    m_pD3DDev->SetTransform( D3DTS_WORLD, &m_matWorld );

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0x0 );
    RenderSphere();
}



void 
CBeams::DrawBeams()
{
    // set beam texture we've just generated
    m_pD3DDev->SetTexture( 0, m_pkBeamsRenderTexture );

    // get viewport settings to render quad
    D3DVIEWPORT8 sViewPort;
    m_pD3DDev->GetViewport( &sViewPort );

    // lock vertex buffer and write data for one quad
    SVertex *pVertices;
    m_pkVertexBuffer->Lock( 0,
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

    m_pkVertexBuffer->Unlock();

    // draw beams from texture
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    m_pD3DDev->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    m_pD3DDev->SetPixelShader( 0 );

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    
    m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );    
    m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}



void 
CBeams::Update( float fTime )
{
    if( false != m_bBrowserMode )
    {
        fTime = m_pkBeamSettingsDlg->GetTime();
    }
    else
    {
        fTime = GetRelativeTime( fTime );
    }

    D3DXMatrixIdentity( &m_matWorld );
    
    D3DXMatrixLookAtLH( &m_matView, &D3DXVECTOR3( 2.75f * sinf( fTime ), 2.75f * cosf( fTime * 0.5f ), 3.0f ), 
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    D3DXMatrixPerspectiveFovLH( &m_matProj, 
        D3DXToRadian( 60.0f ), GetAspectRatio(), 0.1f, 16.0f );

    if( SUCCEEDED( m_pD3DDev->BeginScene() ) )
    {   
        // set basic render states
        m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );         
        m_pD3DDev->SetRenderState( D3DRS_ZENABLE, FALSE );
        
        m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE, TRUE ); 
        m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_LOCALVIEWER, FALSE );

        m_pD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_CLIPPING, TRUE );

        RenderSun( fTime );   
        RenderBeams(); 

        DrawBackdrop();
        DrawBeams();


        if( false == m_bBrowserMode )
        {
            if( fTime < 1.0f )
            {
                m_pkEffectTransition->ColorBlendTexture( 
                            fTime, 
                            0.0f, 255,
                            1.0f,   0, 
                            0, 0, 0,
                            CEffectTransition::sc_pcTransitionTexture2
                          );
            }
            else
            if( fTime > GetEffectRuntime() - 1.0f )
            {              
                m_pkEffectTransition->ColorBlendTexture( 
                            fTime, 
                            GetEffectRuntime() - 1.0f,   0,
                            GetEffectRuntime()       , 255, 
                            255, 255, 255,
                            CEffectTransition::sc_pcTransitionTexture3
                          );
            }
        }


        m_pD3DDev->SetTexture( 0, 0 );  
        m_pD3DDev->SetTexture( 1, 0 );
        m_pD3DDev->SetTexture( 2, 0 );
        m_pD3DDev->SetTexture( 3, 0 );

        m_pD3DDev->EndScene();                      
        m_pD3DDev->Present( 0, 0, 0, 0 );
    }
}



bool 
CBeams::GetShaderSources( SShaderSourceList& colShaders )
{
    const SShaderDescriptor c_shaderDesc[] =
    {
        {
            "Sun Surface",
            "Beams\\RenderSun.vsa",
            "Beams\\RenderSun.psa"
        },

        {
            "Beams",        
            0,
            "Beams\\Beams.psa"
        }
    };

    return( LoadShaderSources( colShaders, c_shaderDesc, 2 ) );
}

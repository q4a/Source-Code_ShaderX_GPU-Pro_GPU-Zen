///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CDisplacmentMap.
 *
 * @file
 *  DisplacementMap.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Implementation of \e Effects::CDisplacmentMap.
 */



#include "StdAfx.h"
#include "DisplacementMap.h"
#include "DisplacementMapDialog.h"
#include "EffectException.h"
#include "ShaderSourceList.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"
#include "EffectTransition.h"
#include "resource.h"
#include "..\..\Help\ContextIDs.h"

#include <string>

#include "DisplacementMapConstants.h"

namespace
{
    // wrap shader code in an anonymous namespace to 
    // avoid any possible conflicts with other modules as
    // shader assembler doesn't declare shader byte code arrays 
    // static when writing them to outfile

#include "Shader\DisplacementMap.psh"
#include "Shader\DisplacementMap.vsh"
}



using namespace Effects;
using namespace EffectSystem;
using namespace Shared;
using namespace ScrambledArchive;
using namespace std;



const char c_pcEffectName[]      = "Displacement Map";
const char c_pcDisplacementMap[] = "DisplacementMap\\Logo.raw";
const char c_pcLogoTexture[]     = "DisplacementMap\\Logo.jpg";
const char c_pcEnvTexture[]      = "Shared\\Cubemap.jpg";
const char c_pcBassSpectrum[]    = "DisplacementMap\\BassSpectrum.raw";
const char c_pcBackground[]      = "DisplacementMap\\Bpft.jpg";



const unsigned int c_uiDisplacementMapSize( 256 );

const float CDisplacementMap::sc_fDefaultHeightScale( 0.02f / 256.0f );



CDisplacementMap::CDisplacementMap( IDirect3DDevice8* pD3DDev, 
                                    CScrambledArchive* pkScrambledArchive,
                                    CTextureManager* pkTextureManager,
                                    CEffectTransition* pkEffectTransition,
                                    bool bBrowserMode )
: CEffect( pD3DDev, pkScrambledArchive, pkTextureManager, pkEffectTransition, bBrowserMode )
, m_pkVertexBuffer( 0 )
, m_pkDisplacementMapSettingsDlg( 0 )
, m_dwVSHandle( 0 )
, m_dwPSHandle( 0 )
, m_fHeightScale( sc_fDefaultHeightScale )
, m_vViewer( 0.0f, 0.0f, 0.0f )
, m_colDisplacementMap()
, m_colBassSpectrum()
{
    try
    {
        ConfirmRequirements();

        // load displacement map
        if( false == pkScrambledArchive->DecompressFile( c_pcDisplacementMap, m_colDisplacementMap ) ||
            c_uiDisplacementMapSize * c_uiDisplacementMapSize != m_colDisplacementMap.size() )
        {
            throw( CEffectException( "Failed to load displacement map!" ) );
        }

        // load logo texture
        if( false == m_pkTextureManager->LoadJPEGTexture( c_pcLogoTexture, true, true, false ) )
        {
            throw( CEffectException( "Failed to load stars texture!" ) );
        }

        // load cube environment map
        if( false == m_pkTextureManager->LoadJPEGCubeTexture( c_pcEnvTexture, false, true, false ) )
        {
            throw( CEffectException( "Failed to load environment texture!" ) );
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
        DWORD dwDisplacementMapDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // position 
                D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // normal
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader( 
                            dwDisplacementMapDecl, 
                            dwDisplacementMapVertexShader, 
                            &m_dwVSHandle, 
                            GetPreferedVertexShaderUsageFlag() 
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwDisplacementMapPixelShader, &m_dwPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // initialize all app managed resources
        InitAppManagedResources();
        
        // create settings dialog if we are in browser mode
        if( false != m_bBrowserMode )
        {
            m_pkDisplacementMapSettingsDlg = new CDisplacementMapDialog;
            if( 0 == m_pkDisplacementMapSettingsDlg ||
                FALSE == m_pkDisplacementMapSettingsDlg->Create( IDD_DISPLACEMENTMAP ) )
            {
                throw( CEffectException( "Failed to create effect settings dialog!" ) );
            }
            
            m_pkDisplacementMapSettingsDlg->ShowWindow( SW_SHOW );
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



CDisplacementMap::~CDisplacementMap()
{
    FreeAllResources();
}



void
CDisplacementMap::FreeAllResources()
{
    if( 0 != m_dwVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwVSHandle );
    }

    if( 0 != m_dwPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwPSHandle );
    }

    if( 0 != m_pkDisplacementMapSettingsDlg )
    {
        m_pkDisplacementMapSettingsDlg->DestroyWindow();
        delete m_pkDisplacementMapSettingsDlg;
    }

    CEffect::FreeAllResources();
}



const char* const 
CDisplacementMap::GetEffectName()
{
    return( c_pcEffectName );
}



bool 
CDisplacementMap::IsEffectDialogVisible()
{
    DBG_ASSERT( false != m_bBrowserMode );
    return( ( false != m_pkDisplacementMapSettingsDlg->IsWindowVisible() ) ? true : false );
}



void 
CDisplacementMap::ShowEffectDialog( bool bShow )
{
    DBG_ASSERT( false != m_bBrowserMode );
    int iShowCmd( ( false != bShow ) ? SW_SHOW : SW_HIDE );
    m_pkDisplacementMapSettingsDlg->ShowWindow( iShowCmd );
}



unsigned int 
CDisplacementMap::GetHTMLHelpID()
{
    return( ID_HELPCONTEXT_DISPLACEMENTMAP );
}



void 
CDisplacementMap::InitAppManagedResources()
{
    // create dynamic vertex buffer
    DBG_ASSERT( 0 == m_pkVertexBuffer );
    if( FAILED( m_pD3DDev->CreateVertexBuffer(  2 * c_uiDisplacementMapSize * sizeof( SVertex ), 
                                                D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
                                                0, 
                                                D3DPOOL_DEFAULT,
                                                &m_pkVertexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic vertex buffer!" ) );
    }
}



void 
CDisplacementMap::FreeAppManagedResources()
{   
    SafeRelease( m_pkVertexBuffer ); 
}



void 
CDisplacementMap::ConfirmRequirements()
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
}



static inline float 
GetCoord( unsigned int uiI )
{
    const float c_fGridSpacing( 1.0f / ( (float) ( c_uiDisplacementMapSize - 1 ) ) );
    return( -1.0f + uiI * c_fGridSpacing * 2.0f );    
}



inline int 
CDisplacementMap::GetDisplacement( unsigned int uiX, unsigned int uiY )
{    
    unsigned int uiAddr( ( uiX & ( c_uiDisplacementMapSize - 1 ) ) +
                         ( uiY & ( c_uiDisplacementMapSize - 1 ) ) * c_uiDisplacementMapSize );
    return( m_colDisplacementMap[ uiAddr ] );
}



void 
CDisplacementMap::SetupVertex( unsigned int uiX, unsigned int uiY, SVertex* pVertex )
{
    pVertex->vPos.x = GetCoord( uiX );
    pVertex->vPos.y = GetCoord( uiY );
    pVertex->vPos.z = -GetDisplacement( uiX, uiY ) * m_fHeightScale;

    pVertex->vNormal.x = ( GetDisplacement( uiX - 1, uiY ) - GetDisplacement( uiX + 1, uiY ) ) * m_fHeightScale;
    pVertex->vNormal.y = ( GetDisplacement( uiX, uiY - 1 ) - GetDisplacement( uiX, uiY + 1 ) ) * m_fHeightScale;
    pVertex->vNormal.z = -10.0f / 256.0f;    
}



void 
CDisplacementMap::SetShaderConstants()
{
    // vertex shader constants    
    D3DXMATRIX matTemp;
    GetWorldViewProjection( matTemp );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDVIEWPROJ_0, &matTemp, 4 ); 

    // viewer position
    m_pD3DDev->SetVertexShaderConstant( CV_VIEWERPOS, 
                                        &D3DXVECTOR4( m_vViewer.x, m_vViewer.y, m_vViewer.z, 0.0f ), 1 );
    // some constants
    m_pD3DDev->SetVertexShaderConstant( CV_CONSTANTS, &D3DXVECTOR4( 0.0f, 0.5f, 1.0f, 2.0f ), 1 ); 

    // pixel shader constants    
    m_pD3DDev->SetPixelShaderConstant( CP_MATERIALCOLOR, &D3DXVECTOR4( 1.00f, 0.98f, 0.53f, 1.00f ), 1 );   
    m_pD3DDev->SetPixelShaderConstant( CP_BKCOLOR, &D3DXVECTOR4( 1.00f, 1.00f, 1.00f, 1.00f ), 1 ); 
}



void 
CDisplacementMap::Update( float fTime )
{
    if( false != m_bBrowserMode )
    {
        fTime = m_pkDisplacementMapSettingsDlg->GetTime();
        m_fHeightScale = m_pkDisplacementMapSettingsDlg->GetHeightScale();
    }
    else
    {
        fTime = GetRelativeTime( fTime );

        m_fHeightScale *= 0.8f;
        m_fHeightScale += 0.2f * sc_fDefaultHeightScale * 
                                 ( 0.5f + 2.5f * GetBassLineValue( fTime, &m_colBassSpectrum[ 0 ], 
                                                       (unsigned int) m_colBassSpectrum.size() ) );
    }

    D3DXMatrixIdentity( &m_matWorld );
    
    m_vViewer = D3DXVECTOR3( 1.25f * sinf( fTime ), 1.25f * cosf( fTime * 0.5f ), -1.5f );
    D3DXMatrixLookAtLH( &m_matView, &m_vViewer, 
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    D3DXMatrixPerspectiveFovLH( &m_matProj, 
        D3DXToRadian( 60.0f ), GetAspectRatio(), 0.1f, 16.0f );

    if( SUCCEEDED( m_pD3DDev->BeginScene() ) )
    {   
        m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, 1.0f, 0 );

        DrawBackground( c_pcBackground );

        // set basic render states
        m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );         
        m_pD3DDev->SetRenderState( D3DRS_ZENABLE, TRUE );
        m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );       
        m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

        m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE, TRUE ); 
        m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_LOCALVIEWER, FALSE );

        m_pD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_CLIPPING, TRUE );

        m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
        m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

        // set logo texture
        IDirect3DBaseTexture8* pkTexture( m_pkTextureManager->GetTexture( c_pcLogoTexture ) );
        m_pD3DDev->SetTexture( 0, pkTexture );   

        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

        // set env texture 
        IDirect3DBaseTexture8* pkEnvTexture( m_pkTextureManager->GetTexture( c_pcEnvTexture ) );
        m_pD3DDev->SetTexture( 1, pkEnvTexture );   

        m_pD3DDev->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );

        // set shaders
        m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );
        m_pD3DDev->SetVertexShader( m_dwVSHandle );
        m_pD3DDev->SetPixelShader( m_dwPSHandle );

        SetShaderConstants();

        for( unsigned int y( 0 ); y < c_uiDisplacementMapSize - 1; ++y )
        {
            SVertex *pVertices;
            m_pkVertexBuffer->Lock( 0,
                                    0,
                                    (BYTE **) &pVertices,
                                    D3DLOCK_DISCARD );

            for( unsigned int x( 0 ); x < c_uiDisplacementMapSize; ++x )
            {
                SetupVertex( x, y, pVertices++ );
                SetupVertex( x, y + 1, pVertices++ );
            }       

            m_pkVertexBuffer->Unlock();
            m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 * ( c_uiDisplacementMapSize - 1 ) );
        }

        SafeRelease( pkTexture );
        SafeRelease( pkEnvTexture );



        if( false == m_bBrowserMode )
        {
            if( fTime < 1.0f )
            {
                m_pkEffectTransition->ColorBlendTexture( 
                            fTime, 
                            0.0f, 255,
                            1.0f,   0, 
                            255, 255, 255,
                            CEffectTransition::sc_pcTransitionTexture3
                          );
            }
            else
            if( fTime > GetEffectRuntime() - 1.0f )
            {              
                m_pkEffectTransition->ColorBlend( 
                            fTime, 
                            GetEffectRuntime() - 1.0f,   0,
                            GetEffectRuntime()       , 255, 
                            0, 0, 0
                          );
            }
        }


        m_pD3DDev->SetTexture( 0, 0 );
        m_pD3DDev->SetTexture( 1, 0 );   

        m_pD3DDev->EndScene();                      
        m_pD3DDev->Present( 0, 0, 0, 0 );
    }
}



bool 
CDisplacementMap::GetShaderSources( SShaderSourceList& colShaders )
{
    const SShaderDescriptor c_shaderDesc[] =
    {
        {
            "Displacement Map",
            "DisplacementMap\\DisplacementMap.vsa",
            "DisplacementMap\\DisplacementMap.psa"
        }
    };

    return( LoadShaderSources( colShaders, c_shaderDesc, 1 ) );
}

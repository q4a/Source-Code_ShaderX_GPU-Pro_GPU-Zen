///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::COceanScene.
 *
 * @file
 *  OceanScene.h
 * @author
 *  Carsten Wenzel
 * @date
 *  07.03.2002
 * @brief
 *  Implementation of \e Effects::COceanScene.
 */



#include "StdAfx.h"
#include "OceanScene.h"
#include "OceanSceneDialog.h"
#include "EffectException.h"
#include "ShaderSourceList.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"
#include "EffectTransition.h"
#include "Geometry.h"
#include "resource.h"
#include "..\..\Help\ContextIDs.h"

#include <string>

#include "ShaderConstants.h"
#include "OceanShaderConstants.h"

namespace
{
    // wrap shader code in an anonymous namespace to 
    // avoid any possible conflicts with other modules as
    // shader assembler doesn't declare shader byte code arrays 
    // static when writing them to outfile

#   include "Shader\Sky.psh"
#   include "Shader\Sky.vsh"

#   include "Shader\Ocean.psh"
#   include "Shader\Ocean.vsh"
}



using namespace Effects;
using namespace EffectSystem;
using namespace Shared;
using namespace ScrambledArchive;
using namespace std;



const char c_pcEffectName[]    = "Ocean Scene";
const char c_pcSkyTexture[]    = "OceanScene\\Sky.jpg";
const char c_pcSkyRefTexture[] = "OceanScene\\SkySmall.jpg";

const unsigned int c_uiGridSize
(
    COceanWaveGenerator::GRIDSIZE + 1
);

// With a grid size of c_uiGridSize we get c_uiGridSize * c_uiGridSize vertices
const unsigned int c_uiNumVertices
(
    c_uiGridSize * c_uiGridSize
);

// To build one horizontal strip we need 2 * c_uiGridSize indices and there are c_uiGridSize - 1
// strips we have to build. Since we put all horizontal strips into one big strip
// to gain a considerable performance boost, we need to artificially insert two indices
// to connect two independent strips for every but the last one.
const unsigned int c_uiNumStripIndices
(
    ( c_uiGridSize - 1 ) * ( 2 * c_uiGridSize + 2 ) - 2
);

const unsigned int c_uiNumStripTriangles
(
    c_uiNumStripIndices - 2
);

const unsigned int c_uiBumpTextureSize( COceanWaveGenerator::GRIDSIZE );

const float COceanScene::sc_fDefaultDynamicRangeOfEnvMap( 4.0f );



COceanScene::COceanScene( IDirect3DDevice8* pD3DDev,
                          CScrambledArchive* pkScrambledArchive,
                          CTextureManager* pkTextureManager,
                          CEffectTransition* pkEffectTransition,
                          bool bBrowserMode )
: CEffect( pD3DDev, pkScrambledArchive, pkTextureManager, pkEffectTransition, bBrowserMode )
, m_pkSphere( 0 )
, m_pkVertexBuffer( 0 )
, m_pkIndexBuffer( 0 )
, m_pkBumpTexture( 0 )
, m_pkOceanSceneSettingsDlg( 0 )
, m_vViewer( 0.0f, 0.0f, 0.0f )
, m_vDirection( 0.0f, 0.0f, 0.0f )
, m_dwSkyVSHandle( 0 )
, m_dwSkyPSHandle( 0 )
, m_dwOceanVSHandle( 0 )
, m_dwOceanPSHandle( 0 )
, m_fDynamicRangeOfEnvMap( sc_fDefaultDynamicRangeOfEnvMap )
, m_kWaveGen()
{
    try
    {
        ConfirmRequirements();

        // load cube environment maps
        if( false == m_pkTextureManager->LoadJPEGCubeTexture( c_pcSkyTexture, false, true, false ) )
        {
            throw( CEffectException( "Failed to load sky texture!" ) );
        }

        if( false == m_pkTextureManager->LoadJPEGCubeTexture( c_pcSkyRefTexture, false, true, false ) )
        {
            throw( CEffectException( "Failed to load sky texture!" ) );
        }

        // create a sphere mesh used for rendering the sun and the backdrop
        if( false == CGeometry::CreateSphere( 1.0f, 30, 30, m_pD3DDev, m_pkSphere, 1.0f, 1.0f ) )
        {
            throw( CEffectException( "Failed to create mesh!" ) );
        }

        // create bump texture
        if( FAILED( D3DXCreateTexture(
                            m_pD3DDev,                                  // device
                            c_uiBumpTextureSize,                        // width
                            c_uiBumpTextureSize,                        // height
                            1,                                          // num of mip maps
                            0,                                          // usage
                            D3DFMT_Q8W8V8U8,                            // format
                            D3DPOOL_MANAGED,                            // pool
                            &m_pkBumpTexture                            // texture
                            ) ) )
        {
            throw( CEffectException( "Couldn't create bump texture!" ) );
        }

        // create vertex shader
        DWORD dwSkyDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // position
                D3DVSD_REG( 1, D3DVSDT_FLOAT2 ), // 2D texture
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader(
                            dwSkyDecl,
                            dwSkyVertexShader,
                            &m_dwSkyVSHandle,
                            GetPreferedVertexShaderUsageFlag()
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwSkyPixelShader, &m_dwSkyPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // create vertex shader
        DWORD dwOceanDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // position
                D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // normal
                D3DVSD_REG( 2, D3DVSDT_FLOAT2 ), // 2D texture
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader(
                            dwOceanDecl,
                            dwOceanVertexShader,
                            &m_dwOceanVSHandle,
                            GetPreferedVertexShaderUsageFlag()
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwOceanPixelShader, &m_dwOceanPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // initialize all app managed resources
        InitAppManagedResources();

        // create settings dialog if we are in browser mode
        if( false != m_bBrowserMode )
        {
            m_pkOceanSceneSettingsDlg = new COceanSceneDialog();
            if( 0 == m_pkOceanSceneSettingsDlg ||
                FALSE == m_pkOceanSceneSettingsDlg->Create( IDD_OCEANSCENE ) )
            {
                throw( CEffectException( "Failed to create effect settings dialog!" ) );
            }

            m_pkOceanSceneSettingsDlg->ShowWindow( SW_SHOW );
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



COceanScene::~COceanScene()
{
    FreeAllResources();
}



void
COceanScene::FreeAllResources()
{
    if( 0 != m_dwSkyVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwSkyVSHandle );
    }

    if( 0 != m_dwSkyPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwSkyPSHandle );
    }

    if( 0 != m_dwOceanVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwOceanVSHandle );
    }

    if( 0 != m_dwOceanPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwOceanPSHandle );
    }

    if( 0 != m_pkOceanSceneSettingsDlg )
    {
        m_pkOceanSceneSettingsDlg->DestroyWindow();
        delete m_pkOceanSceneSettingsDlg;
    }

    SafeRelease( m_pkSphere );
    SafeRelease( m_pkBumpTexture );

    CEffect::FreeAllResources();
}



const char* const
COceanScene::GetEffectName()
{
    return( c_pcEffectName );
}



bool
COceanScene::IsEffectDialogVisible()
{
    DBG_ASSERT( false != m_bBrowserMode );
    return( ( false != m_pkOceanSceneSettingsDlg->IsWindowVisible() ) ? true : false );
}



void
COceanScene::ShowEffectDialog( bool bShow )
{
    DBG_ASSERT( false != m_bBrowserMode );
    int iShowCmd( ( false != bShow ) ? SW_SHOW : SW_HIDE );
    m_pkOceanSceneSettingsDlg->ShowWindow( iShowCmd );
}



unsigned int
COceanScene::GetHTMLHelpID()
{
    return( ID_HELPCONTEXT_OCEANSCENE );
}



void
COceanScene::InitAppManagedResources()
{
    HRESULT hr;

    // create dynamic vertex buffer
    DBG_ASSERT( 0 == m_pkVertexBuffer );
    if( FAILED( hr = m_pD3DDev->CreateVertexBuffer(
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
    if( FAILED( hr = m_pD3DDev->CreateIndexBuffer(
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
    if( FAILED( hr = m_pkIndexBuffer->Lock(
                      0,
                      0,
                      (BYTE **) &pIndices,
                      D3DLOCK_DISCARD  ) ) )
    {
        throw( CEffectException( "Failed to lock dynamic index buffer!" ) );
    }

    // set indices
    int iIndex = 0;
    for( unsigned int a( 0 ); a < c_uiGridSize - 1; ++a )
    {
        for( unsigned int i( 0 ); i < c_uiGridSize; ++i, pIndices += 2, ++iIndex )
        {
            pIndices[ 0 ] = iIndex;
            pIndices[ 1 ] = iIndex + c_uiGridSize;
        }

        // connect two strips by inserting last index of previous strip
        // and first index of current strip => creates four degenerated triangles
        if( c_uiGridSize - 2 > a )
        {
            pIndices[ 0 ] = iIndex + c_uiGridSize - 1;
            pIndices[ 1 ] = iIndex;

            pIndices += 2;
        }
    }

    m_pkIndexBuffer->Unlock();
}



void
COceanScene::FreeAppManagedResources()
{
    SafeRelease( m_pkVertexBuffer );
    SafeRelease( m_pkIndexBuffer );
}



void
COceanScene::ConfirmRequirements()
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
}



void
COceanScene::RenderSphere()
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
COceanScene::SetSkyShaderConstants()
{
    // vertex shader constants
    D3DXMATRIX matTemp;
    GetWorldViewProjection( matTemp );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDVIEWPROJ_0, &matTemp, 4 );
}



void
COceanScene::RenderSky()
{
    D3DXMatrixIdentity( &m_matWorld );

    D3DXMatrixLookAtLH( &m_matView, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
        &m_vDirection, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    // set sky texture
    IDirect3DBaseTexture8* pkSkyTexture( m_pkTextureManager->GetTexture( c_pcSkyTexture ) );
    m_pD3DDev->SetTexture( 0, pkSkyTexture );

    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pD3DDev->SetRenderState( D3DRS_ZENABLE, FALSE );

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );

    // set shaders
    m_pD3DDev->SetVertexShader( m_dwSkyVSHandle );
    m_pD3DDev->SetPixelShader( m_dwSkyPSHandle );
    SetSkyShaderConstants();

    RenderSphere();

    SafeRelease( pkSkyTexture );
}



void
COceanScene::SetOceanShaderConstants( int x, int z )
{
    // vertex shader constants
    D3DXMATRIX matTemp;
    GetWorldViewProjection( matTemp );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDVIEWPROJ_0, &matTemp, 4 );

    // ocean mesh xz scale
    m_pD3DDev->SetVertexShaderConstant( CV_MESH_XYZ_SCALE,
                                        &D3DXVECTOR4( 512.0f, 8.0f, 512.0f, 1.0f ), 1 );
    // ocean mesh xz offset
    m_pD3DDev->SetVertexShaderConstant( CV_MESH_XYZ_OFFSET,
                                        &D3DXVECTOR4(  x * 512.0f, 0.0f, z * 512.0f, 0.0f ), 1 );

    // bump texture uv scale
    m_pD3DDev->SetVertexShaderConstant( CV_BUMP_UV_SCALE,
                                        &D3DXVECTOR4( 10.0f, 10.0f, 0.0f, 0.0f ), 1 );

    // viewer position
    m_pD3DDev->SetVertexShaderConstant( CV_VIEWERPOS,
                                        &D3DXVECTOR4( m_vViewer.x, m_vViewer.y, m_vViewer.z, 0.0f ), 1 );

    // curvature
    m_pD3DDev->SetVertexShaderConstant( CV_CURVATURE, &D3DXVECTOR4( 0.00001f, 0.0f, 0.0f, 0.0f ), 1 );

    // power factor for fresnel term approximation
    m_pD3DDev->SetVertexShaderConstant( CV_FRESNELAPPROX_POWFACTOR, &D3DXVECTOR4( 8.0f, 0.0f, 0.0f, 0.0f ), 1 );

    // factor for dynamic range of env map
    m_pD3DDev->SetVertexShaderConstant( CV_DYNAMIC_RANGE,
        &D3DXVECTOR4( m_fDynamicRangeOfEnvMap, 1.0f / 4.0f, 0.0f, 0.0f ), 1 );

    // some constants
    m_pD3DDev->SetVertexShaderConstant( CV_CONSTANTS, &D3DXVECTOR4( 0.0f, 0.5f, 1.0f, 2.0f ), 1 );


    // pixel shader constants
    m_pD3DDev->SetPixelShaderConstant( CP_OCEAN_COLOR_LIGHT, &D3DXVECTOR4( 67.5f / 255.0f, 127.5f / 255.0f, 139.5f / 255.0f, 0.0f ), 1 );
    m_pD3DDev->SetPixelShaderConstant( CP_OCEAN_COLOR_DARK, &D3DXVECTOR4( 36.0f / 255.0f, 76.5f / 255.0f, 93.0f / 255.0f, 0.0f ), 1 );
}



inline float
COceanScene::GetCoord( int i, float fDisplace )
{
    const float c_fScale( 1.0f / (float) ( c_uiGridSize - 1 ) );
    return( i * c_fScale + fDisplace );
}



void
COceanScene::RenderOcean( float fTime )
{
    // update world and view matrix
    D3DXMatrixIdentity( &m_matWorld );

    D3DXMatrixLookAtLH( &m_matView, &m_vViewer,
        &D3DXVECTOR3( m_vViewer + m_vDirection ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    // update ocean height map
    m_kWaveGen.UpdateOceanMap( fTime * 0.75f );

    // update bump map for ripples on ocean surface
    BuildBumpMap();

    // update ocean mesh
    SVertex *pVertices;
    m_pkVertexBuffer->Lock( 0,
                            0,
                            (BYTE **) &pVertices,
                            D3DLOCK_DISCARD );

    for( unsigned int z( 0 ); z < c_uiGridSize; ++z )
    {
        for( unsigned int x( 0 ); x < c_uiGridSize; ++x )
        {
            pVertices->vPos.x = GetCoord( x, m_kWaveGen.GetOceanDisplaceX( x, z ) );
            pVertices->vPos.y = m_kWaveGen.GetOceanHeight( x, z );
            pVertices->vPos.z = GetCoord( z, m_kWaveGen.GetOceanDisplaceY( x, z ) );

            pVertices->vNormal.x = m_kWaveGen.GetOceanNormalX( x, z );
            pVertices->vNormal.y = 64.0f;
            pVertices->vNormal.z = m_kWaveGen.GetOceanNormalY( x, z );

            pVertices->vTex.x =        (float) x / (float) ( c_uiGridSize - 1 );
            pVertices->vTex.y = 1.0f - (float) z / (float) ( c_uiGridSize - 1 );

            ++pVertices;
        }
    }
    m_pkVertexBuffer->Unlock();

    m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );
    m_pD3DDev->SetIndices( m_pkIndexBuffer, 0 );


    // set basic render states
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

    m_pD3DDev->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

    // set texture stage states
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    m_pD3DDev->SetTextureStageState( 3, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 3, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 3, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );

    IDirect3DBaseTexture8* pkSkyRefTexture( m_pkTextureManager->GetTexture( c_pcSkyRefTexture ) );

    m_pD3DDev->SetTexture( 0, m_pkBumpTexture );
    m_pD3DDev->SetTexture( 1, 0 );
    m_pD3DDev->SetTexture( 2, 0 );
    m_pD3DDev->SetTexture( 3, pkSkyRefTexture );

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    m_pD3DDev->SetVertexShader( m_dwOceanVSHandle );
    m_pD3DDev->SetPixelShader( m_dwOceanPSHandle );

    for( int z( -2 ); z < 2; ++z )
    {
        for( int x( -2 ); x < 2; ++x )
        {
            SetOceanShaderConstants( x, z );
            m_pD3DDev->DrawIndexedPrimitive(
                                            D3DPT_TRIANGLESTRIP,
                                            0,
                                            c_uiNumVertices,
                                            0,
                                            c_uiNumStripTriangles
                                            );
        }
    }

    SafeRelease( pkSkyRefTexture );
}



void
COceanScene::UpdateViewer( float fTime )
{
    m_vViewer = D3DXVECTOR3( 15.0f * sinf( fTime ), 50.0f, 15.0f * cosf( fTime ) );
    m_vDirection = D3DXVECTOR3( 0.0, 45.0f, 0.0f ) - m_vViewer;
}



void
COceanScene::Update( float fTime )
{
    if( false != m_bBrowserMode )
    {
        // update ocean settings based on user input
        m_pkOceanSceneSettingsDlg->UpdateOceanSettings( m_kWaveGen );

        m_fDynamicRangeOfEnvMap = m_pkOceanSceneSettingsDlg->GetDynamicRangeOfEnvMap();
        fTime = m_pkOceanSceneSettingsDlg->GetTime();

        UpdateViewer( fTime * 0.25f );
    }
    else
    {
        fTime = GetRelativeTime( fTime );
        UpdateViewer( fTime * ( 2.0f * D3DX_PI / GetEffectRuntime() ) );
    }

    D3DXMatrixPerspectiveFovLH( &m_matProj,
        D3DXToRadian( 90.0f ), GetAspectRatio(), 0.1f, 1024.0f );

    if( SUCCEEDED( m_pD3DDev->BeginScene() ) )
    {
        m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, 1.0f, 0 );

        // set basic render states
        m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

        m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE, TRUE );
        m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_LOCALVIEWER, FALSE );

        m_pD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_CLIPPING, TRUE );

        RenderSky();
        RenderOcean( 0.25f * fTime );

        if( false == m_bBrowserMode )
        {
            if( fTime < 1.0f )
            {
                m_pkEffectTransition->ColorBlendTexture(
                            fTime,
                            0.0f, 255,
                            1.0f,   0,
                            0, 0, 0,
                            CEffectTransition::sc_pcTransitionTexture1
                          );
            }
            else
            if( fTime > GetEffectRuntime() - 1.0f )
            {
                m_pkEffectTransition->ColorBlendTexture(
                            fTime,
                            GetEffectRuntime() - 1.0f,   0,
                            GetEffectRuntime()       , 255,
                            0, 0, 0,
                            CEffectTransition::sc_pcTransitionTexture2
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
COceanScene::GetShaderSources( SShaderSourceList& colShaders )
{
    const SShaderDescriptor c_shaderDesc[] =
    {
        {
            "Sky",
            "OceanScene\\Sky.vsa",
            "OceanScene\\Sky.psa"
        },

        {
            "Ocean",
            "OceanScene\\Ocean.vsa",
            "OceanScene\\Ocean.psa"
        }
    };

    return( LoadShaderSources( colShaders, c_shaderDesc, 2 ) );
}



void
COceanScene::BuildBumpMap()
{
    D3DLOCKED_RECT rect;
    if( SUCCEEDED( m_pkBumpTexture->LockRect( 0, &rect, 0, 0 ) ) )
    {
        for( unsigned int z( 0 ); z < c_uiBumpTextureSize; ++z )
        {
            unsigned int* puiBumps( (unsigned int *) ( (unsigned char*) rect.pBits + rect.Pitch * z ) );
            for( unsigned int x( 0 ); x < c_uiBumpTextureSize; ++x, ++puiBumps )
            {
                D3DXVECTOR3 vNormal(
                                     m_kWaveGen.GetOceanNormalX( x, z ),
                                     64.0f,
                                     m_kWaveGen.GetOceanNormalY( x, z )
                                    );

                D3DXVec3Normalize( &vNormal, &vNormal );
                vNormal *= 127.5f;

                *puiBumps = ( (unsigned int) ( (unsigned char) vNormal.z ) << 16 ) |
                            ( (unsigned int) ( (unsigned char) vNormal.y ) <<  8 ) |
                            ( (unsigned int) ( (unsigned char) vNormal.x ) <<  0 );
            }
        }

        m_pkBumpTexture->UnlockRect( 0 );
    }
}
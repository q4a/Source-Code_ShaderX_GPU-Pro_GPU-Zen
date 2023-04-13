///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CSoftObjects.
 *
 * @file
 *  SoftObjects.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  08.03.2002
 * @brief
 *  Implementation of \e Effects::CSoftObjects.
 */



#include "StdAfx.h"
#include "SoftObjects.h"
#include "SoftObjectsDialog.h"
#include "EffectException.h"
#include "ShaderSourceList.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"
#include "EffectTransition.h"
#include "Geometry.h"
#include "resource.h"
#include "..\..\Help\ContextIDs.h"

#include "SoftObjectsShaderConstants.h"

namespace
{
    // wrap shader code in an anonymous namespace to 
    // avoid any possible conflicts with other modules as
    // shader assembler doesn't declare shader byte code arrays 
    // static when writing them to outfile

#   include "Shader\SoftObjects.psh"
#   include "Shader\SoftObjects.vsh"
#   include "Shader\Environment.psh"
#   include "Shader\Environment.vsh"
}



using namespace Effects;
using namespace EffectSystem;
using namespace ScrambledArchive;
using namespace Shared;
using namespace std;



const char c_pcEffectName[]   = "Soft Objects";
const char c_pcEnvTexture[]   = "Shared\\Cubemap.jpg";
const char c_pcBassSpectrum[] = "SoftObjects\\BassSpectrum.raw";

const unsigned int c_uiSizeOfVertexBuffer( 1024 );
const unsigned int c_uiSizeOfIndexBuffer( 2 * c_uiSizeOfVertexBuffer );



CSoftObjects::CSoftObjects( IDirect3DDevice8* pD3DDev, 
                            CScrambledArchive* pkScrambledArchive,
                            CTextureManager* pkTextureManager, 
                            CEffectTransition* pkEffectTransition,
                            bool bBrowserMode ) 
: CEffect( pD3DDev, pkScrambledArchive, pkTextureManager, pkEffectTransition, bBrowserMode )
, m_pkVertexBuffer( 0 )
, m_pkIndexBuffer( 0 )
, m_pkSphere( 0 )
, m_pkSoftObjectsSettingsDlg( 0 )
, m_pVertices( 0 )
, m_pIndices( 0 )
, m_uiVertexBufferOffset( 0 )
, m_uiIndexBufferOffset( 0 )
, m_uiVerticesToAdd( 0 )
, m_uiIndicesToAdd( 0 )
, m_dwSoftObjectsVSHandle( 0 )
, m_dwSoftObjectsPSHandle( 0 )
, m_dwEnvironmentVSHandle( 0 )
, m_dwEnvironmentPSHandle( 0 )
, m_vViewer( ( false != bBrowserMode ) ? D3DXVECTOR3( 1.25f, 0.5f, 0.5f ) : D3DXVECTOR3( 0.0f, 0.5f, 0.4f ) )
, m_fBlobsAttractor( 0.33f )
, m_colBassSpectrum()
, m_kPolygonizer()
{
    try
    {
        ConfirmRequirements();

        // load cube environment map
        if( false == m_pkTextureManager->LoadJPEGCubeTexture( c_pcEnvTexture, false, true, false ) )
        {
            throw( CEffectException( "Failed to load environment texture!" ) );
        }

        if( false == m_bBrowserMode )
        {
            // load bass spectrum of music
            if( false == pkScrambledArchive->DecompressFile( c_pcBassSpectrum, m_colBassSpectrum )  )
            {
                throw( CEffectException( "Failed to load bass spectrum!" ) );
            }
        }

        // create a sphere mesh used for rendering the backdrop
        if( false == CGeometry::CreateSphere( 1.0f, 30, 30, m_pD3DDev, m_pkSphere, 1.0f, 1.0f ) )
        {
            throw( CEffectException( "Failed to create mesh!" ) );
        }

        // create vertex shader
        DWORD dwSoftObjectsDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // position 
                D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // normal
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader( 
                            dwSoftObjectsDecl, 
                            dwSoftObjectsVertexShader, 
                            &m_dwSoftObjectsVSHandle, 
                            GetPreferedVertexShaderUsageFlag() 
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwSoftObjectsPixelShader, &m_dwSoftObjectsPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // create vertex shader
        DWORD dwEnvironmentDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // position 
                D3DVSD_REG( 1, D3DVSDT_FLOAT2 ), // 2D texture
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader( 
                            dwEnvironmentDecl, 
                            dwEnvironmentVertexShader, 
                            &m_dwEnvironmentVSHandle, 
                            GetPreferedVertexShaderUsageFlag() 
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwEnvironmentPixelShader, &m_dwEnvironmentPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // initialize all app managed resources
        InitAppManagedResources();

        // set triangle container for polygonizer
        m_kPolygonizer.SetTriangleContainer( this );

        // create settings dialog if we are in browser mode
        if( false != m_bBrowserMode )
        {
            m_pkSoftObjectsSettingsDlg = new CSoftObjectsDialog( );
            if( 0 == m_pkSoftObjectsSettingsDlg ||
                FALSE == m_pkSoftObjectsSettingsDlg->Create( IDD_SOFT_OBJECTS ) )
            {
                throw( CEffectException( "Failed to create effect settings dialog!" ) );
            }
            
            m_pkSoftObjectsSettingsDlg->ShowWindow( SW_SHOW );
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



CSoftObjects::~CSoftObjects()
{
    FreeAllResources();
}



void
CSoftObjects::FreeAllResources()
{
    if( 0 != m_dwSoftObjectsVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwSoftObjectsVSHandle );
    }

    if( 0 != m_dwSoftObjectsPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwSoftObjectsPSHandle );
    }

    if( 0 != m_dwEnvironmentVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwEnvironmentVSHandle );
    }

    if( 0 != m_dwEnvironmentPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwEnvironmentPSHandle );
    }

    if( 0 != m_pkSoftObjectsSettingsDlg )
    {
        m_pkSoftObjectsSettingsDlg->DestroyWindow();
        delete m_pkSoftObjectsSettingsDlg;
    }

    SafeRelease( m_pkSphere );

    CEffect::FreeAllResources();
}



const char* const 
CSoftObjects::GetEffectName()
{
    return( c_pcEffectName );
}



bool 
CSoftObjects::IsEffectDialogVisible()
{
    DBG_ASSERT( false != m_bBrowserMode );
    return( ( false != m_pkSoftObjectsSettingsDlg->IsWindowVisible() ) ? true : false );
}



void 
CSoftObjects::ShowEffectDialog( bool bShow )
{
    DBG_ASSERT( false != m_bBrowserMode );
    int iShowCmd( ( false != bShow ) ? SW_SHOW : SW_HIDE );
    m_pkSoftObjectsSettingsDlg->ShowWindow( iShowCmd );
}



unsigned int 
CSoftObjects::GetHTMLHelpID()
{
    return( ID_HELPCONTEXT_SOFTOBJETCS );
}



void 
CSoftObjects::InitAppManagedResources()
{
    HRESULT hr;

    // create dynamic vertex buffer
    DBG_ASSERT( 0 == m_pkVertexBuffer );
    if( FAILED( hr = m_pD3DDev->CreateVertexBuffer( 
                        c_uiSizeOfVertexBuffer * sizeof( SVertex ), 
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
                        0, 
                        D3DPOOL_DEFAULT,
                        &m_pkVertexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic vertex buffer!" ) );
    }

    // create dynamic index buffer
    DBG_ASSERT( 0 == m_pkIndexBuffer );
    if( FAILED( hr = m_pD3DDev->CreateIndexBuffer(  
                        c_uiSizeOfIndexBuffer * sizeof( WORD ), 
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
                        D3DFMT_INDEX16, 
                        D3DPOOL_DEFAULT,                        
                        &m_pkIndexBuffer ) ) )
    {
        throw( CEffectException( "Failed to create dynamic index buffer!" ) );
    }
}



void 
CSoftObjects::FreeAppManagedResources()
{
    SafeRelease( m_pkVertexBuffer );
    SafeRelease( m_pkIndexBuffer );
}



void 
CSoftObjects::ConfirmRequirements()
{
    // make sure the index buffer doesn't exceed 65536
    DBG_ASSERT_STATIC( c_uiSizeOfIndexBuffer <= 65536, Too_many_indices_for_triangle_list );

    // make sure the vertex buffer can hold c_uiNumVertices vertices
    DBG_ASSERT_STATIC( c_uiSizeOfVertexBuffer <= 65536, Too_many_vertices_for_vertex_buffer );


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



void 
CSoftObjects::SetShaderConstants()
{
    // vertex shader constants    
    D3DXMATRIX matTemp;
    GetWorldViewProjection( matTemp );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDVIEWPROJ_0, &matTemp, 4 ); 

    // viewer position
    m_pD3DDev->SetVertexShaderConstant( CV_VIEWERPOS, 
                                        &D3DXVECTOR4( m_vViewer.x, m_vViewer.y, m_vViewer.z, 0.0f ), 1 );

    // refraction constant
    m_pD3DDev->SetVertexShaderConstant( CV_REFRACT, 
                                        &D3DXVECTOR4( 0.4f, 0.4f, 0.4f, 0.4f ), 1 );

    // fresnel factor
    m_pD3DDev->SetVertexShaderConstant( CV_FRENSEL_FACTOR, 
                                        &D3DXVECTOR4( 2.0f, 0.0f, 0.0f, 0.0f ), 1 );

    // some constants
    m_pD3DDev->SetVertexShaderConstant( CV_CONSTANTS, 
                                        &D3DXVECTOR4( 0.0f, 0.5f, 1.0f, 2.0f ), 1 );
}



void
CSoftObjects::RenderSphere()
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
CSoftObjects::RenderEnvironment()
{    
    // set env texture 
    IDirect3DBaseTexture8* pkEnvTexture( m_pkTextureManager->GetTexture( c_pcEnvTexture ) );
    m_pD3DDev->SetTexture( 0, pkEnvTexture );   

    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );

    // set shaders
    m_pD3DDev->SetVertexShader( m_dwEnvironmentVSHandle );
    m_pD3DDev->SetPixelShader( m_dwEnvironmentPSHandle );

    // set world transform for environment
    D3DXMATRIX matTemp;
    D3DXMatrixTranslation( &m_matWorld, 0.5f, 0.5f, 0.5f );    
    D3DXMatrixScaling( &matTemp, 8.0f, 8.0f, 8.0f );
    D3DXMatrixMultiply( &m_matWorld, &matTemp, &m_matWorld );

    SetShaderConstants();
    RenderSphere();

    SafeRelease( pkEnvTexture );
}



void
CSoftObjects::RenderSoftObjects( float fTime )
{
    // set world transform for soft objects
    D3DXMatrixIdentity( &m_matWorld );

    // set texture stage states
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );

    m_pD3DDev->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pD3DDev->SetTextureStageState( 1, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );

    // set texture
    IDirect3DBaseTexture8* pkEnvTexture( m_pkTextureManager->GetTexture( c_pcEnvTexture ) );
    m_pD3DDev->SetTexture( 0, pkEnvTexture );   
    m_pD3DDev->SetTexture( 1, pkEnvTexture );   

    // set shaders
    SetShaderConstants();
    m_pD3DDev->SetVertexShader( m_dwSoftObjectsVSHandle );
    m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );
    m_pD3DDev->SetIndices( m_pkIndexBuffer, 0 );
    m_pD3DDev->SetPixelShader( m_dwSoftObjectsPSHandle );

    // render surface
    if( false == m_bBrowserMode )
    {
        m_kPolygonizer.RenderSurface( 3.0f * fTime );
    }
    else
    {
        m_kPolygonizer.RenderSurface( fTime );
    }
    Flush();

    SafeRelease( pkEnvTexture );
}



void 
CSoftObjects::Update( float fTime )
{
    if( false != m_bBrowserMode )
    {
        fTime = m_pkSoftObjectsSettingsDlg->GetTime();

        m_kPolygonizer.SetNumSoftObjects( m_pkSoftObjectsSettingsDlg->GetNumSoftObjects() );
        m_kPolygonizer.SetGridSize( m_pkSoftObjectsSettingsDlg->GetGridSize() );
        m_kPolygonizer.SetIsoThreshold( m_pkSoftObjectsSettingsDlg->GetIsoThreshold() );
    }
    else
    {
        fTime = GetRelativeTime( fTime );        

        // change blobs attractor according to bass line
        m_fBlobsAttractor *= 0.5f;
        m_fBlobsAttractor += 0.5f * ( 0.33f + 0.66f * GetBassLineValue( fTime, 
                                                           &m_colBassSpectrum[ 0 ], 
                                                           (unsigned int) m_colBassSpectrum.size() ) );
            
        m_kPolygonizer.SetBlobsAttractor( m_fBlobsAttractor );
    }


    D3DXMatrixLookAtLH( &m_matView, &m_vViewer, 
        &D3DXVECTOR3( 0.5f, 0.5f, 0.5f ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    D3DXMatrixPerspectiveFovLH( &m_matProj, 
        D3DXToRadian( 90.0f ), GetAspectRatio(), 0.1f, 16.0f );

    if( SUCCEEDED( m_pD3DDev->BeginScene() ) )
    {
        m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );

        // set basic render states
        m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW ); 
        m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );         
        m_pD3DDev->SetRenderState( D3DRS_ZENABLE, TRUE );
        m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        
        m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE, TRUE ); 
        m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_LOCALVIEWER, FALSE );

        m_pD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pD3DDev->SetRenderState( D3DRS_CLIPPING, TRUE );

        // disable alpha blending
        m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

        RenderEnvironment();
        RenderSoftObjects( fTime );


        if( false == m_bBrowserMode )
        {
            if( fTime < 0.5f )
            {
                m_pkEffectTransition->ColorBlend( 
                            fTime, 
                            0.0f, 255,
                            0.5f,   0, 
                            255, 255, 255
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
                            CEffectTransition::sc_pcTransitionTexture0
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
CSoftObjects::GetShaderSources( SShaderSourceList& colShaders )
{
    const SShaderDescriptor c_shaderDesc[] =
    {
        {
            "Soft Objects",
            "SoftObjects\\SoftObjects.vsa",
            "SoftObjects\\SoftObjects.psa"
        },

        {
            "Environment",
            "SoftObjects\\Environment.vsa",
            "SoftObjects\\Environment.psa"
        }
    };
    return( LoadShaderSources( colShaders, c_shaderDesc, 2 ) );
}



void 
CSoftObjects::Begin( unsigned int uiVerticesToAdd, unsigned int uiIndicesToAdd )
{
    DBG_ASSERT( uiVerticesToAdd < c_uiSizeOfVertexBuffer );
    DBG_ASSERT( uiIndicesToAdd < c_uiSizeOfIndexBuffer );
    DBG_ASSERT( 0 == uiIndicesToAdd % 3 );

    // check if buffers have been locked
    if( 0 != m_pVertices && 0 != m_pIndices )
    {
        // check if new data will fit into buffers 
        if( m_uiVertexBufferOffset + uiVerticesToAdd > c_uiSizeOfVertexBuffer ||
            m_uiIndexBufferOffset + uiIndicesToAdd > c_uiSizeOfIndexBuffer )
        {
            Flush();
        }
    }

    DBG_ASSERT( 0 == m_pVertices && 0 == m_pIndices ||
                0 != m_pVertices && 0 != m_pIndices );

    // do we need to (re)lock buffers
    if( 0 == m_pVertices && 0 == m_pIndices )
    {
        m_pkVertexBuffer->Lock(
                                0,
                                0,
                                (BYTE **) &m_pVertices,
                                D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD 
                               );

        m_pkIndexBuffer->Lock(
                               0,
                               0,
                               (BYTE **) &m_pIndices,
                               D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD 
                               );
    }

    m_uiVerticesToAdd = uiVerticesToAdd;
    m_uiIndicesToAdd = uiIndicesToAdd;
}



void 
CSoftObjects::AddVertices( const CSoftObjectsPolygonizer::SVertex* const pVertices )
{
    for( unsigned int i( 0 ); i < m_uiVerticesToAdd; ++i, ++m_pVertices )
    {
        m_pVertices->vPos    = pVertices[ i ].vPos;
        m_pVertices->vNormal = pVertices[ i ].vNormal;
    }
}



void 
CSoftObjects::AddIndices( const int* const pIndices )
{
    for( unsigned int i( 0 ); i < m_uiIndicesToAdd; ++i, ++m_pIndices )
    {
        *m_pIndices = m_uiVertexBufferOffset + pIndices[ i ];
    }
}



void 
CSoftObjects::End()
{
    m_uiVertexBufferOffset += m_uiVerticesToAdd;
    m_uiIndexBufferOffset += m_uiIndicesToAdd;
}



void
CSoftObjects::Flush()
{
    if( 0 != m_pVertices && 0 != m_pIndices )
    {
        // unlock buffers
        m_pkVertexBuffer->Unlock();
        m_pkIndexBuffer->Unlock();

        // render all recorded triangles
        m_pD3DDev->DrawIndexedPrimitive( 
                            D3DPT_TRIANGLELIST, 
                            0,
                            m_uiVertexBufferOffset,
                            0, 
                            m_uiIndexBufferOffset / 3 
                            );

        // invalidate pointers
        m_pVertices = 0;
        m_pIndices = 0;

        // set buffer offsets to zero
        m_uiVertexBufferOffset = 0;
        m_uiIndexBufferOffset = 0;
    }
}

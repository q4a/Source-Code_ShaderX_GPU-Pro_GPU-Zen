///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CHair.
 *
 * @file
 *  Hair.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  06.03.2002
 * @brief
 *  Implementation of \e Effects::CHair.
 */



#include "StdAfx.h"
#include "Hair.h"
#include "HairDialog.h"
#include "EffectException.h"
#include "ShaderSourceList.h"
#include "ScrambledArchive.h"
#include "TextureManager.h"
#include "EffectTransition.h"
#include "Random.h"
#include "Clamp.h"
#include "math.h"
#include "resource.h"
#include "..\..\Help\ContextIDs.h"

#include <string>

#include "HairShaderConstants.h"

namespace
{
    // wrap shader code in an anonymous namespace to 
    // avoid any possible conflicts with other modules as
    // shader assembler doesn't declare shader byte code arrays 
    // static when writing them to outfile

#   include "Shader\Hair.psh"
#   include "Shader\Hair.vsh"
}



using namespace Effects;
using namespace EffectSystem;
using namespace Shared;
using namespace ScrambledArchive;
using namespace std;



const char c_pcEffectName[]   = "Hair";
const char c_pcHairTexture[]  = "Hair\\Hair1D.dds";
const char c_pcBassSpectrum[] = "Hair\\BassSpectrum.raw";
const char c_pcBackground[]   = "Hair\\Synthesia.jpg";


// We generate one big triangle strip to efficiently render all hairs.
// To form one hair we need 2 * ( NUM_HAIR_JOINTS + 1 ) vertices.
const unsigned int c_uiNumVertices( CHair::NUM_HAIRS * 2 * ( CHair::NUM_HAIR_JOINTS + 1 ) );

// To connect all vertices of one hair to a triangle strip we need 2 * ( NUM_HAIR_JOINTS + 1 ) indices.
// Since we put the strips of multiple hairs into one big strip to gain a considerable performance boost,
// we need to artificially insert two indices to connect two independent strips for every but the last one.
const unsigned int c_uiNumStripIndices( CHair::NUM_HAIRS * ( 2 * ( CHair::NUM_HAIR_JOINTS + 1 ) + 2 ) - 2 );

const unsigned int c_uiNumStripTriangles( c_uiNumStripIndices - 2 );

// Size of anisotropic texture used for shading the hair.
const unsigned int c_uiAnisotropicTextureSize( 64 );

// Default settings for hair physics
const float CHair::sc_fDefaultGravity( -0.030f );
const float CHair::sc_fDefaultInertia(  0.005f );



CHair::CHair( IDirect3DDevice8* pD3DDev,
              CScrambledArchive* pkScrambledArchive,
              CTextureManager* pkTextureManager,
              CEffectTransition* pkEffectTransition,
              bool bBrowserMode )
: CEffect( pD3DDev, pkScrambledArchive, pkTextureManager, pkEffectTransition, bBrowserMode )
, m_pkVertexBuffer( 0 )
, m_pkIndexBuffer( 0 )
, m_pkAnisotropicTexture( 0 )
, m_pkHairSettingsDlg( 0 )
, m_dwVSHandle( 0 )
, m_dwPSHandle( 0 )
, m_vGravity( 0.0f, 0.0f, 0.0f )
, m_vViewer( 0.0f, 0.0f, 0.0f )
, m_vAnisotropySettings( 0.2f, 0.9f, 0.7f, 25.0f )
, m_vDiffuseColor( 0.7f, 0.6f, 0.0f, 1.0f )
, m_vSpecularColor( 1.0f, 0.9f, 0.8f, 1.0f )
, m_matInertia()
, m_fGravity( sc_fDefaultGravity )
, m_fInertia( sc_fDefaultInertia )
, m_colBassSpectrum()
{
    try
    {
        ConfirmRequirements();

        // load hair texture
        if( false == m_pkTextureManager->LoadDDSTexture( c_pcHairTexture, true ) )
        {
            throw( CEffectException( "Failed to load hair texture!" ) );
        }

        // create anisotropic texture
        if( FAILED( D3DXCreateTexture(
                            m_pD3DDev,                                  // device
                            c_uiAnisotropicTextureSize,                 // width
                            c_uiAnisotropicTextureSize,                 // height
                            D3DX_DEFAULT ,                              // num of mip maps
                            0,                                          // usage
                            D3DFMT_A8R8G8B8,                            // format
                            D3DPOOL_MANAGED,                            // pool
                            &m_pkAnisotropicTexture                     // texture
                            ) ) )
        {
            throw( CEffectException( "Couldn't create anisotropic texture!" ) );
        }
        CalcAnisotropicTexture();

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
        DWORD dwHairDecl[] =
        {
            D3DVSD_STREAM( 0 ),
                D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // position
                D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // normal
                D3DVSD_REG( 2, D3DVSDT_FLOAT1 ), // 1D texture
            D3DVSD_END()
        };
        if( FAILED( m_pD3DDev->CreateVertexShader(
                            dwHairDecl,
                            dwHairVertexShader,
                            &m_dwVSHandle,
                            GetPreferedVertexShaderUsageFlag()
                            ) ) )
        {
            throw( CEffectException( "Failed to create vertex shader!" ) );
        }

        // create pixel shader
        if( FAILED( m_pD3DDev->CreatePixelShader( dwHairPixelShader, &m_dwPSHandle ) ) )
        {
            throw( CEffectException( "Failed to create pixel shader!" ) );
        }

        // initialize all app managed resources
        InitAppManagedResources();

        // setup hairs
        CRandom kRnd;
        for( int i( 0 ); i < NUM_HAIRS; ++i )
        {
            float fZ( (float) kRnd.GetInRange( -1.0, 1.0 ) );
            float fW( sqrtf( 1.0f - fZ * fZ ) );

            float fT( 2.0f * D3DX_PI * (float) kRnd.Get() );
            m_pHairs[ i ].vDir.x = fW * cosf( fT );
            m_pHairs[ i ].vDir.y = fW * sinf( fT );
            m_pHairs[ i ].vDir.z = fZ;

            m_pHairs[ i ].fLength = (float) kRnd.GetInRange( 0.09, 0.10 );
            m_pHairs[ i ].fTex1D  = (float) kRnd.GetInRange();
        }

        // create settings dialog if we are in browser mode
        if( false != m_bBrowserMode )
        {
            m_pkHairSettingsDlg = new CHairDialog( m_vDiffuseColor, m_vSpecularColor, m_vAnisotropySettings );
            if( 0 == m_pkHairSettingsDlg ||
                FALSE == m_pkHairSettingsDlg->Create( IDD_HAIR ) )
            {
                throw( CEffectException( "Failed to create effect settings dialog!" ) );
            }

            m_pkHairSettingsDlg->ShowWindow( SW_SHOW );
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



CHair::~CHair()
{
    FreeAllResources();
}



void
CHair::FreeAllResources()
{
    if( 0 != m_dwVSHandle )
    {
        m_pD3DDev->DeleteVertexShader( m_dwVSHandle );
    }

    if( 0 != m_dwPSHandle )
    {
        m_pD3DDev->DeletePixelShader( m_dwPSHandle );
    }

    if( 0 != m_pkHairSettingsDlg )
    {
        m_pkHairSettingsDlg->DestroyWindow();
        delete m_pkHairSettingsDlg;
    }

    SafeRelease( m_pkAnisotropicTexture );

    CEffect::FreeAllResources();
}



const char* const
CHair::GetEffectName()
{
    return( c_pcEffectName );
}



bool
CHair::IsEffectDialogVisible()
{
    DBG_ASSERT( false != m_bBrowserMode );
    return( ( false != m_pkHairSettingsDlg->IsWindowVisible() ) ? true : false );
}



void
CHair::ShowEffectDialog( bool bShow )
{
    DBG_ASSERT( false != m_bBrowserMode );
    int iShowCmd( ( false != bShow ) ? SW_SHOW : SW_HIDE );
    m_pkHairSettingsDlg->ShowWindow( iShowCmd );
}



unsigned int
CHair::GetHTMLHelpID()
{
    return( ID_HELPCONTEXT_HAIR );
}


void
CHair::InitAppManagedResources()
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
    for( int a( 0 ); a < NUM_HAIRS; ++a )
    {
        for( int i( 0 ); i < NUM_HAIR_JOINTS + 1; ++i, pIndices += 2, iIndex += 2 )
        {
            pIndices[ 0 ] = iIndex;
            pIndices[ 1 ] = iIndex + 1;
        }

        // connect two strips by inserting last index of previous strip
        // and first index of current strip => creates four degenerated triangles
        if(  NUM_HAIRS - 1 > a )
        {
            pIndices[ 0 ] = iIndex - 1;
            pIndices[ 1 ] = iIndex;

            pIndices += 2;
        }
    }

    m_pkIndexBuffer->Unlock();
}



void
CHair::FreeAppManagedResources()
{
    SafeRelease( m_pkVertexBuffer );
    SafeRelease( m_pkIndexBuffer );
}



void
CHair::ConfirmRequirements()
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
}



void
CHair::SetShaderConstants()
{
    // vertex shader constants
    D3DXMATRIX matTemp;
    GetWorldViewProjection( matTemp );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDVIEWPROJ_0, &matTemp, 4 );

    // world transform
    D3DXMatrixTranspose( &matTemp, &m_matWorld );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLD_0, &matTemp, 4 );

    // inverse transposed world transform
    D3DXMATRIX matWorldInvTransp;
    D3DXMatrixInverse( &matWorldInvTransp, 0, &m_matWorld );
    m_pD3DDev->SetVertexShaderConstant( CV_WORLDIT_0, &matTemp, 4 );

    // viewer position
    m_pD3DDev->SetVertexShaderConstant( CV_VIEWERPOS,
                                        &D3DXVECTOR4( m_vViewer.x, m_vViewer.y, m_vViewer.z, 0.0f ), 1 );
    // light position
    m_pD3DDev->SetVertexShaderConstant( CV_LIGHTPOS,
                                        &D3DXVECTOR4( 10.0f, 10.0f, 10.0f, 0.0f ), 1 );
    // some constants
    m_pD3DDev->SetVertexShaderConstant( CV_CONSTANTS,
                                        &D3DXVECTOR4( 0.0f, 0.5f, 1.0f, 2.0f ), 1 );

    // pixel shader constants
    m_pD3DDev->SetPixelShaderConstant( CP_DIFFUSE_COLOR, &m_vDiffuseColor, 1 );
    m_pD3DDev->SetPixelShaderConstant( CP_SPECULAR_COLOR, &m_vSpecularColor, 1 );
}



void
CHair::UpdateViewerPosition( float fTime )
{
    m_vViewer = D3DXVECTOR3( 0.75f * sinf( 0 * fTime ), 0.75f * cosf( 0 * fTime * 0.5f ), 2.5f );
}



inline void
CHair::CalcHairRotationMatrix( D3DXMATRIX& matTransform, float fTime )
{
    D3DXMatrixRotationYawPitchRoll( &matTransform,
                                    1.75f * sinf(  1.50f * fTime + 0.6f ),
                                    3.00f * sinf( -1.00f * fTime + 2.1f ),
                                    2.15f * sinf(  0.75f * fTime - 0.7f ) );
}



inline void
CHair::CalcHairTranslationMatrix( D3DXMATRIX& matTransform, float fTime )
{
    D3DXMatrixIdentity( &matTransform );
}



void
CHair::CalcHairDynamics( float fTime )
{
    D3DXMATRIX matHairRotation;
    D3DXMATRIX matHairTranslation;

    CalcHairRotationMatrix( matHairRotation, fTime );
    CalcHairTranslationMatrix( matHairTranslation, fTime );

    // calculate world transformation
    m_matWorld = matHairRotation * matHairTranslation;

    // build gravity vector from transformation matrix (i.e. f * y-axis)
    m_vGravity = D3DXVECTOR3( m_matWorld( 0, 1 ), m_matWorld( 1, 1 ), m_matWorld( 2, 1 ) );

    // compute inertia matrix in two steps
    const float c_fDelta( 0.1f );

    // 1. compute inertia matrix for rotation
    // calculate the 2nd derivative of the rotation matrix
    D3DXMATRIX matInertiaRotationPrev;
    CalcHairRotationMatrix( matInertiaRotationPrev, fTime - c_fDelta );

    D3DXMATRIX matInertiaRotationNext;
    CalcHairRotationMatrix( matInertiaRotationNext, fTime + c_fDelta );

    D3DXMATRIX matInertiaRotation( 2 * matHairRotation );
    matInertiaRotation -= matInertiaRotationPrev ;
    matInertiaRotation -= matInertiaRotationNext;
    matInertiaRotation *= 1.0f / ( c_fDelta * c_fDelta );

    // multiply by the inverse hair rotation transform to get
    // the inertia vector back into local space
    D3DXMATRIX matHairRotationInv;
    D3DXMatrixInverse( &matHairRotationInv, 0, &matHairRotation );
    matInertiaRotation *= matHairRotationInv;

    // 2. compute inertia matrix for translation
    // calculate the 2nd derivative of the translation matrix
    D3DXMATRIX matInertiaTranslationPrev;
    CalcHairTranslationMatrix( matInertiaTranslationPrev, fTime - c_fDelta );

    D3DXMATRIX matInertiaTranslationNext;
    CalcHairTranslationMatrix( matInertiaTranslationNext, fTime + c_fDelta );

    D3DXMATRIX matInertiaTranslation( 2 * matHairTranslation );
    matInertiaTranslation -= matInertiaTranslationPrev ;
    matInertiaTranslation -= matInertiaTranslationNext;
    matInertiaTranslation *= 1.0f / ( c_fDelta * c_fDelta );

    // combine inertia matrices
    m_matInertia = matInertiaRotation + matInertiaTranslation;
}



void
CHair::Update( float fTime )
{
    if( false != m_bBrowserMode )
    {
        fTime = m_pkHairSettingsDlg->GetTime();

        // update hair color
        m_pkHairSettingsDlg->GetDiffuseColor( m_vDiffuseColor );
        m_pkHairSettingsDlg->GetSpecularColor( m_vSpecularColor );

        // check if anisotropy parameters have changed
        if( false != m_pkHairSettingsDlg->GetAnisotropySettings( m_vAnisotropySettings ) )
        {
            CalcAnisotropicTexture();
        }

        // get physics settings
        m_fGravity = m_pkHairSettingsDlg->GetGravity();
        m_fInertia = m_pkHairSettingsDlg->GetInertia();
    }
    else
    {
        fTime = GetRelativeTime( fTime );

        m_fInertia = sc_fDefaultInertia * 0.5f;

        m_fGravity *= 0.8f;
        m_fGravity += 0.2f * ( sc_fDefaultGravity * 0.25f -
                               0.05f * GetBassLineValue( fTime, &m_colBassSpectrum[ 0 ],
                                                         (unsigned int) m_colBassSpectrum.size() ) );
    }

    UpdateViewerPosition( fTime );
    CalcHairDynamics( fTime );

    D3DXMatrixLookAtLH( &m_matView, &m_vViewer,
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    D3DXMatrixPerspectiveFovLH( &m_matProj,
        D3DXToRadian( 60.0f ), GetAspectRatio(), 0.1f, 4.0f );


    if( SUCCEEDED( m_pD3DDev->BeginScene() ) )
    {
        m_pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, 1.0f, 0 );

        // render background
        DrawBackground( c_pcBackground );

        // set basic render states
        m_pD3DDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
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

        // set textures
        IDirect3DBaseTexture8* pkHairTexture( m_pkTextureManager->GetTexture( c_pcHairTexture ) );
        m_pD3DDev->SetTexture( 0, m_pkAnisotropicTexture );
        m_pD3DDev->SetTexture( 1, pkHairTexture );

        // set texture stage states
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pD3DDev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

        m_pD3DDev->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
        m_pD3DDev->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

        // set shaders
        SetShaderConstants();
        m_pD3DDev->SetVertexShader( m_dwVSHandle );
        m_pD3DDev->SetStreamSource( 0, m_pkVertexBuffer, sizeof( SVertex ) );
        m_pD3DDev->SetIndices( m_pkIndexBuffer, 0 );
        m_pD3DDev->SetPixelShader( m_dwPSHandle );

        // lock vertex buffer to write new data
        SVertex *pVertices;
        m_pkVertexBuffer->Lock( 0,
                                0,
                                (BYTE **) &pVertices,
                                D3DLOCK_DISCARD );

        // render hairs
        for( unsigned int i( 0 ); i < NUM_HAIRS; ++i )
        {
            // set initial values for the hair and compute vector to generate a strip
            D3DXVECTOR3 vPos( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
            D3DXVECTOR3 vDir( m_pHairs[ i ].vDir );

            D3DXVECTOR3 vStrip;
            D3DXVec3Cross( &vStrip, &vDir, &m_vGravity );
            D3DXVec3Normalize( &vStrip, &vStrip );
            vStrip *= 0.02f;

            float fLength( m_pHairs[ i ].fLength );
            float fTexOffsetA( m_pHairs[ i ].fTex1D );
            float fTexOffsetB( m_pHairs[ i ].fTex1D + 0.15f );

            // calculate inertia vector
            D3DXVECTOR3 vInertia;
            D3DXVec3TransformNormal( &vInertia, &m_pHairs[ i ].vDir, &m_matInertia );

            D3DXVECTOR3 vInfluence( m_fInertia * vInertia + m_fGravity * m_vGravity );

            // process all hair joints
            for( unsigned int j( 0 ); j < NUM_HAIR_JOINTS + 1; ++j )
            {
                // compute normalized joint direction and the tangent
                vDir += vInfluence;
                D3DXVec3Normalize( &vDir, &vDir );

                // store joint coordinate and tangent to arrays
                pVertices[ 0 ].vPos = vPos;
                pVertices[ 0 ].vTangent = vDir;
                pVertices[ 0 ].fTex1D = fTexOffsetA;

                D3DXVec3Add( &pVertices[ 1 ].vPos, &vPos, &vStrip );
                pVertices[ 1 ].vTangent = vDir;
                pVertices[ 1 ].fTex1D = fTexOffsetB;

                // advance pointer
                pVertices += 2;

                // update position with scaled direction
                vDir *= fLength;
                vPos += vDir;
            }
        }

        // unlock vertex buffer and render hair
        m_pkVertexBuffer->Unlock();
        m_pD3DDev->DrawIndexedPrimitive(
                                        D3DPT_TRIANGLESTRIP,
                                        0,
                                        c_uiNumVertices,
                                        0,
                                        c_uiNumStripTriangles
                                        );


        if( false == m_bBrowserMode )
        {
            if( fTime < 1.0f )
            {
                m_pkEffectTransition->ColorBlendTexture(
                            fTime,
                            0.0f, 255,
                            1.0f,   0,
                            0, 0, 0,
                            CEffectTransition::sc_pcTransitionTexture0
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
                            CEffectTransition::sc_pcTransitionTexture1
                          );
            }
        }

        SafeRelease( pkHairTexture );
        m_pD3DDev->SetTexture( 0, 0 );
        m_pD3DDev->SetTexture( 1, 0 );

        m_pD3DDev->EndScene();
        m_pD3DDev->Present( 0, 0, 0, 0 );
    }
}



bool
CHair::GetShaderSources( SShaderSourceList& colShaders )
{
    const SShaderDescriptor c_shaderDesc[] =
    {
        {
            "Hair",
            "Hair\\Hair.vsa",
            "Hair\\Hair.psa"
        }
    };

    return( LoadShaderSources( colShaders, c_shaderDesc, 1 ) );
}



void
CHair::CalcAnisotropicTexture()
{
    // get settings for anistropic texture
    float fAmb( m_vAnisotropySettings.x );
    float fDiff( m_vAnisotropySettings.y );
    float fSpec( m_vAnisotropySettings.z );
    float fGloss( m_vAnisotropySettings.w );

    vector< unsigned int >
        colAnisotropicTexture( c_uiAnisotropicTextureSize * c_uiAnisotropicTextureSize );

    // calculate anisotropic texture using the given parameters
    for( unsigned int a( 0 ); a < c_uiAnisotropicTextureSize; ++a )
    {
        // calculate ambient and diffuse values
        float fLxT( 2.0f * a / (float) c_uiAnisotropicTextureSize - 1.0f );      // map to -1..1
        float fLxN( sqrtf( 1.0f - fLxT * fLxT ) );

        float fAmbientDiffuse = Clamp( fAmb + fDiff * fLxN, 0.0f, 1.0f );
        unsigned char ucAmbientDiffuse = (unsigned char) ( fAmbientDiffuse * 255.0f );

        for( unsigned int i( 0 ); i < c_uiAnisotropicTextureSize; ++i )
        {
            // calculate specular value
            float fVxT( 2.0f * i / (float) c_uiAnisotropicTextureSize - 1.0f );  // map to -1..1
            float fVxR( Clamp( fLxN * sqrtf( 1.0f - fVxT * fVxT ) - fLxT * fVxT, 0.0f, 1.0f ) );

            float fSpecular( Clamp( fSpec * powf( fVxR, fGloss ), 0.0f, 1.0f ) );
            unsigned char ucSpecular( (unsigned char) ( fSpecular * 255.0f ) );

            colAnisotropicTexture[ a * c_uiAnisotropicTextureSize + i ] =
                ( ucAmbientDiffuse ) + ( ucAmbientDiffuse <<  8 ) + ( ucAmbientDiffuse << 16 ) + ( ucSpecular << 24 );
        }
    }

    // get access to mip level 0 of texture
    IDirect3DSurface8* pkTextureSurface( 0 );
    m_pkAnisotropicTexture->GetSurfaceLevel( 0, &pkTextureSurface );

    // blit raw data into texture
    RECT srcRect;
    srcRect.left  = srcRect.top    = 0;
    srcRect.right = srcRect.bottom = c_uiAnisotropicTextureSize;
    D3DXLoadSurfaceFromMemory( pkTextureSurface,                   // dest texture surface
                               0,                                  // dest palette
                               0,                                  // dest rect
                               &colAnisotropicTexture[ 0 ],        // src
                               D3DFMT_A8R8G8B8,                    // src format
                               c_uiAnisotropicTextureSize *
                               sizeof( unsigned int ),             // src pitch
                               0,                                  // src palette
                               &srcRect,                           // src rect
                               D3DX_DEFAULT,                       // filter
                               0                                   // color key
                               );
    pkTextureSurface->Release();

    // create mipmap levels
    D3DXFilterTexture( m_pkAnisotropicTexture, 0, 0, D3DX_DEFAULT );
}

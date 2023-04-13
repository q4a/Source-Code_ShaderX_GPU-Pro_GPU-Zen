///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e EffectSystem::CEffectSequencer.
 *
 * @file
 *  EffectManager.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  12.10.2001
 * @brief
 *  Implementation of \e EffectSystem::CEffectSequencer.
 */



#include "stdafx.h"
#include "stdDeleteObject.h"
#include "EffectSequencer.h"
#include "Effect.h"
#include "EffectException.h"
#include "EffectTransition.h"
#include "MP3Player.h"
#include "TextureManager.h"
#include "ScrambledArchive.h"
#include "D3DInit.h"
#include <algorithm>

#include "VolumetricLight\VolumetricLight.h"
#include "SoftObjects\SoftObjects.h"
#include "Hair\Hair.h"
#include "OceanScene\OceanScene.h"
#include "Beams\Beams.h"
#include "DisplacementMap\DisplacementMap.h"
#include "InfiniteZoom\InfiniteZoom.h"



using namespace std;
using namespace Shared;
using namespace ScrambledArchive;
using namespace EffectSystem;
using namespace Effects;



const char c_pucDataArchive[] = "IDR_DATA";
const char c_pucMP3File[]     = "Music\\BooLapsuusRemix.mp3";

static bool s_bInstanceCreated( false );



CEffectSequencer::CEffectSequencer()
: m_bInitialized( false )
, m_eStatus( STOPPED )
, m_bEnableMusic( false )
, m_bLoopDemo( false )
, m_bWasPlaying( false )

, m_fTotalRuntime( 0 )
, m_pD3DDev( 0 )

, m_colEffects()
, m_itCurrentEffect( 0 )
, m_kTimer()
, m_pkScrambledArchive( 0 )
, m_pkTextureManager( 0 )
, m_pkMP3Player( 0 )
, m_pkEffectTransition( 0 )
{
    s_bInstanceCreated = true;
}



CEffectSequencer::~CEffectSequencer()
{
    Stop();

    if( 0 != m_pD3DDev )
    {
        m_pD3DDev->Release();
    }

    for_each( m_colEffects.begin(), m_colEffects.end(), Shared::TDeleteObject() );

    delete m_pkScrambledArchive;
    delete m_pkTextureManager;
    delete m_pkMP3Player;
    delete m_pkEffectTransition;

    s_bInstanceCreated = false;
}



CEffectSequencer*
CEffectSequencer::Create( HWND hWnd, IDirect3DDevice8* pD3DDev, bool bEnableMusic, bool bLoopDemo )
{
    CEffectSequencer* pkSequencer( 0 );

    if( false == s_bInstanceCreated )
    {
        pkSequencer = new CEffectSequencer;

        if( false == ( pkSequencer->Initialize( hWnd, pD3DDev, bEnableMusic, bLoopDemo ) ) )
        {
            delete pkSequencer;
            pkSequencer = 0;
        }
    }

    return( pkSequencer );
}



bool
CEffectSequencer::Initialize( HWND hWnd, IDirect3DDevice8* pD3DDev,
                              bool bEnableMusic, bool bLoopDemo )
{
    DBG_ASSERT( 0 != pD3DDev );

    if( 0 == pD3DDev )
    {
        return( false );
    }

    m_bEnableMusic = bEnableMusic;
    m_bLoopDemo = bLoopDemo;

    m_pD3DDev = pD3DDev;
    m_pD3DDev->AddRef();

    if( 0 == ( m_pkScrambledArchive = CScrambledArchive::CreateFromResource( c_pucDataArchive ) ) )
    {
        DBG_ASSERTM( 0, "CEffectSequencer::InitializeSequencer() - Failed to load data archive!" );
        return( false );
    }

    if( 0 == ( m_pkTextureManager = CTextureManager::CreateAndBindToArchive( m_pD3DDev, m_pkScrambledArchive ) ) )
    {
        DBG_ASSERTM( 0, "CEffectSequencer::InitializeSequencer() - Failed to bind texture manager to data archive!" );
        return( false );
    }

    if( 0 == ( m_pkEffectTransition = CEffectTransition::Create( m_pD3DDev, m_pkScrambledArchive, m_pkTextureManager ) ) )
    {
        DBG_ASSERTM( 0, "CEffectSequencer::InitializeSequencer() - Failed to create effect transition manager!" );
        return( false );
    }

    if( false == InitializeMusic( hWnd ) )
    {
        DBG_ASSERTM( 0, "CEffectSequencer::InitializeSequencer() - Failed to initialize music!" );
        return( false );
    }

    if( false == InitializeEffects() )
    {
        DBG_ASSERTM( 0, "CEffectSequencer::InitializeSequencer() - Failed to initialize effects!" );
        return( false );
    }

    m_itCurrentEffect = m_colEffects.begin();

    m_bInitialized = true;

    return( true );
}



bool
CEffectSequencer::InitializeEffects()
{
    bool bSuccess( false );

    CEffect* pkEffect( 0 );
    try
    {
        pkEffect = new CVolumetricLight( m_pD3DDev,
            m_pkScrambledArchive, m_pkTextureManager, m_pkEffectTransition, false );
        RegisterEffect( pkEffect, 22.0f );

        pkEffect = new CSoftObjects( m_pD3DDev,
            m_pkScrambledArchive, m_pkTextureManager, m_pkEffectTransition, false );
        RegisterEffect( pkEffect, 14.0f );

        pkEffect = new CHair( m_pD3DDev,
            m_pkScrambledArchive, m_pkTextureManager, m_pkEffectTransition, false );
        RegisterEffect( pkEffect, 14.0f );

        pkEffect = new COceanScene( m_pD3DDev,
            m_pkScrambledArchive, m_pkTextureManager, m_pkEffectTransition, false );
        RegisterEffect( pkEffect, 22.0f );

        pkEffect = new CBeams( m_pD3DDev,
            m_pkScrambledArchive, m_pkTextureManager, m_pkEffectTransition, false );
        RegisterEffect( pkEffect, 14.0f );

        pkEffect = new CDisplacementMap( m_pD3DDev,
            m_pkScrambledArchive, m_pkTextureManager, m_pkEffectTransition, false );
        RegisterEffect( pkEffect, 14.0f );

        pkEffect = new CInfiniteZoom( m_pD3DDev,
            m_pkScrambledArchive, m_pkTextureManager, m_pkEffectTransition, false );
        RegisterEffect( pkEffect, 18.0f );

        bSuccess = true;
    }
    catch( ... )
    {
    }

    return( bSuccess );
}



bool
CEffectSequencer::InitializeMusic( HWND hWnd )
{
    if( false == m_bEnableMusic )
    {
        return( true );
    }

    m_pkMP3Player = CMP3Player::Create( hWnd, *m_pkScrambledArchive, c_pucMP3File );
    return( 0 != m_pkMP3Player );
}



bool
CEffectSequencer::RegisterEffect( CEffect* pkEffect, float fEffectRuntime )
{
    if( 0 == pkEffect || pkEffect->IsRegistered() )
    {
        return( false );
    }

    pkEffect->Register( m_fTotalRuntime, fEffectRuntime );
    m_fTotalRuntime += fEffectRuntime;
    m_colEffects.push_back( pkEffect );

    return( true );
}



void
CEffectSequencer::Reset()
{
    if( false != m_bEnableMusic )
    {
        m_pkMP3Player->Reset();
    }
    m_itCurrentEffect = m_colEffects.begin();
    m_kTimer.Reset();
}



void
CEffectSequencer::Play()
{
    if( PLAYING != m_eStatus )
    {
        if( false != m_bEnableMusic )
        {
            m_pkMP3Player->Play();
        }
        m_kTimer.Start();
        m_eStatus = PLAYING;
    }
}



void
CEffectSequencer::Pause()
{
    if( PLAYING == m_eStatus )
    {
        if( false != m_bEnableMusic )
        {
            m_pkMP3Player->Pause();
        }
        m_kTimer.Pause();
        m_eStatus = PAUSED;
    }
}



void
CEffectSequencer::Stop()
{
    if( PLAYING == m_eStatus || PAUSED == m_eStatus )
    {
        if( false != m_bEnableMusic )
        {
            m_pkMP3Player->Stop();
        }
        m_itCurrentEffect = m_colEffects.begin();
        m_kTimer.Stop();
        m_eStatus = STOPPED;
    }
}



bool
CEffectSequencer::HandleDeviceReset( D3DPRESENT_PARAMETERS& sD3DPresentParameters, bool& bDeviceReady )
{
    HRESULT hr;
    // test the cooperative level to see if it's okay to render
    if( FAILED( hr = m_pD3DDev->TestCooperativeLevel() ) )
    {
        bDeviceReady = false;

        // pause demo until device was successfully reset
        if( false != IsPlaying() )
        {
            m_bWasPlaying = true;
            Pause();
        }

        // if the device was lost but there is still hope to reset it
        // do not render until we get it back
        if( D3DERR_DEVICELOST == hr )
        {
            return( true );
        }

        // check if the device needs to be reset
        if( D3DERR_DEVICENOTRESET == hr )
        {
            // if the demo runs in windowed mode
            // read the possibly new desktop mode
            if( FALSE != sD3DPresentParameters.Windowed )
            {
                LPDIRECT3D8 pD3D( 0 );
                m_pD3DDev->GetDirect3D( &pD3D );

                D3DDISPLAYMODE sDisplayMode;
                if( FAILED( pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &sDisplayMode ) ) )
                {
                    pD3D->Release();
                    return( false );
                }
                sD3DPresentParameters.BackBufferFormat = sDisplayMode.Format;

                if( false == CD3DInit::GetInstance().GetZBufferFormat( pD3D,
                                                                       sDisplayMode.Format,
                                                                       sD3DPresentParameters.AutoDepthStencilFormat ) )
                {
                    pD3D->Release();
                    return( false );
                }

                pD3D->Release();
            }

            // free app managed resources of all effects
            EffectList::iterator itCur( m_colEffects.begin() );
            EffectList::iterator itEnd( m_colEffects.end() );
            while( itCur != itEnd )
            {
                (*itCur)->FreeAppManagedResources();
                ++itCur;
            }

            // reset d3d device
            if( FAILED( m_pD3DDev->Reset( &sD3DPresentParameters ) ) )
            {
                DBG_ASSERTM( 0, "CEffectSequencer::HandleDeviceReset() - d3d device reset failed!" );
                return( false );
            }

            try
            {
                // re-init app managed resources of all effect
                EffectList::iterator itCur( m_colEffects.begin() );
                EffectList::iterator itEnd( m_colEffects.end() );
                while( itCur != itEnd )
                {
                    (*itCur)->InitAppManagedResources();
                    ++itCur;
                }
            }
            catch( CEffectException kException )
            {
                kException.ReportError();
                return( false );
            }

            // demo was paused and now that the device has been
            // successfully reset resume demo
            if( false != m_bWasPlaying )
            {
                Play();
                m_bWasPlaying = false;
            }

            bDeviceReady = true;
        }
    }
    else
    {
        bDeviceReady = true;
    }

    return( true );
}



bool
CEffectSequencer::FrameUpdate( D3DPRESENT_PARAMETERS& sD3DPresentParameters )
{
    // handle a lost device first
    bool bDeviceReady( true );
    if( false == HandleDeviceReset( sD3DPresentParameters, bDeviceReady ) )
    {
        Stop();
        return( false );
    }

    // do not render until d3d device is reset
    if( false == bDeviceReady )
    {
        return( true );
    }

    float fTime( m_kTimer.GetTime() );
    while( (*m_itCurrentEffect)->GetEffectEnd() < fTime )
    {
        ++m_itCurrentEffect;
        if( m_colEffects.end() == m_itCurrentEffect )
        {
            if( false == m_bLoopDemo )
            {
                Stop();
                return( false );
            }
            else
            {
                Reset();
                return( true );
            }
        }
    }

    (*m_itCurrentEffect)->Update( fTime );

    return( true );
}



bool
CEffectSequencer::FrameUpdate( float fTime )
{
    // bail out if d3d device is lost
    if( FAILED( m_pD3DDev->TestCooperativeLevel() ) )
    {
        return( false );
    }

    // find the according effect for the given time
    m_itCurrentEffect = m_colEffects.begin();
    while( (*m_itCurrentEffect)->GetEffectEnd() < fTime )
    {
        ++m_itCurrentEffect;
        if( m_colEffects.end() == m_itCurrentEffect )
        {
            // we're at the end of the demo
            return( false );
        }
    }

    (*m_itCurrentEffect)->Update( fTime );

    return( true );
}
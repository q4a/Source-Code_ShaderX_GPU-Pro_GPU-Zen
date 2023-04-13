///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e EffectSystem::CEffectSequencer.
 *
 * @file
 *  EffectManager.h
 * @author
 *  Carsten Wenzel
 * @date
 *  12.10.2001
 * @brief
 *  Declaration of \e EffectSystem::CEffectSequencer.
 */



#ifndef _EFFECTSEQUENCER_H_INCLUDED_
#define _EFFECTSEQUENCER_H_INCLUDED_

#pragma once



#include <list>
#include "Timer.h"



namespace ScrambledArchive
{
    class CScrambledArchive;
}

namespace Shared
{
    class CTextureManager;
    class CMP3Player;
}



namespace EffectSystem
{


    class CEffect;
    class CEffectTransition;



    /**
    * An effect sequencer. Used to play an arbitrary number of effects as a demo (Meshuggah demo mode).
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  12.10.2001
    * @brief
    *  An effect sequencer.
    */
    class CEffectSequencer
    {
    public:
        /**
        * Destructor.
        *
        * @brief
        *  Destructor.
        */
        ~CEffectSequencer();

        /**
        * Creates an effect sequencer object. The creation of sequencers is restricts
        * to one sequencer at a time.
        *
        * @brief
        *  Creates an effect sequencer object.
        * @param hWnd
        *  Handle of window the sequencer belongs to (needed to setup music system).
        * @param pD3DDev
        *  Pointer to a valid D3D interface.
        * @param bEnableMusic
        *  Set to \b true if music should be enabled else \b false.
        * @param bLoopDemo
        *  Set to \b true if demo should be looped else \b false.
        * @return
        *  \b Returns NULL if creation failed, i.e. a sequencer has already been created or
        * Initialze() failed else it returns a pointer to a CEffectSequencer object.
        */
        static CEffectSequencer* Create( HWND hWnd, IDirect3DDevice8* pD3DDev,
                                         bool bEnableMusic, bool bLoopDemo );

        /**
        * Return boolean that indicates if demo is playing or not.
        *
        * @brief
        *  Return boolean that indicates if demo is playing or not.
        * @return
        *  #m_bIsPlaying.
        */
        bool IsPlaying();

        /**
        * Starts or resumes playback of demo.
        *
        * @brief
        *  Starts or resumes playback of demo.
        */
        void Play();

        /**
        * Pauses playback of demo. Call Play() to resume playback.
        *
        * @brief
        *  Pauses playback of demo.
        */
        void Pause();

        /**
        * Stops the effect sequencer. That is, it stops music playback and the
        * internal timer. Furthermore it sets #m_itCurrentEffect to the first entry in the
        * list of registered effects. Calling Start() after Stop() would cause the sequencer
        * to start demo playback from the beginning.
        *
        * @brief
        *  Stops the effect sequencer.
        */
        void Stop();

        /**
        * Resets the effect sequencer. Music playback and timer will be reset. Furthermore
        * #m_itCurrentEffect will be set to the first entry in the list of registered effects.
        * #m_bIsPlaying remains unchanged. That is, calling Reset() while the demo is playing
        * would effectively restart demo playback. Otherwise it would just "seek" to the beginning.
        *
        * @brief
        *  Resets the effect sequencer.
        */
        void Reset();

        /**
        * Shows the next frame / updates the current effect in the sequencer. Calls HandleDeviceReset()
        * to handle D3D device resets. It automatically loops the demo when it reached its end. In
        * case this method fails the demo will be stopped (i.e. Stop() is called before returning to
        * the caller).
        *
        * @brief
        *  Show the next frame / updates the current effect in the sequencer.
        * @param sD3DPresentParameters
        *  Valid structure of current D3D present parameters.
        * @return
        *  \b true if a frame update was successful, \b false if frame update was
        *  unsuccessful (i.e. HandleDeviceReset() failed) or the demo has ended.
        */
        bool FrameUpdate( D3DPRESENT_PARAMETERS& sD3DPresentParameters );

        /**
        * Renders the according frame of the demo for the given time \e fTime. This method should be used
        * when capturing the demo to a video stream as it is possible to request a certain frames directly
        * by specifying a certain time (that is, the caller as direct control over the frame update process).
        *
        * @param fTime
        *  Time of the frame to be captured.
        * @return
        *  \b true if a frame update was successful, \b false if frame update was
        *  unsuccessful (i.e. render device got lost) or the demo has ended.
        * @brief
        *  Renders the according frame of the demo for the given time \e fTime.
        */
        bool FrameUpdate( float fTime );

    private:
        /**
        * Defines a list of effects.
        *
        * @brief
        *  Defines a list of effects.
        */
        typedef std::list< CEffect* > EffectList;

        /**
        * Demo playback status.
        *
        * @brief
        *  Demo playback status.
        */
        enum EDemoStatus
        {
            /**
            * Demo is stopped.
            */
            STOPPED,

            /**
            * Demo is paused.
            */
            PAUSED,

            /**
            * Demo is running.
            */
            PLAYING
        };

    private:
        /**
        * Default constructor.
        *
        * @brief
        *  Default constructor.
        */
        CEffectSequencer();

        /**
        * Registers an effect and adds it to the list of effects in the sequencer.
        *
        * @brief
        *  Registers an effect and adds it to the list of effects in the sequencer.
        * @param pkEffect
        *  Pointer to an effect object.
        * @param fEffectRunTime
        *  Total runtime  of the effect in seconds.
        * @return
        *  \b true if effect was successfully registered
        */
        bool RegisterEffect( CEffect* pkEffect, float fEffectRunTime );

        /**
        * Initializes the sequencer. It creates a scrambled data archive that effects will use to
        * load data from, a texture manager that will be used as a static texture pool for effects.
        * Furthermore it initializes the music system (see InitializeMusic()) and creates all effects
        * (see InitializeEffects()) that should be played. After a successful call to this method the
        * demo is ready to be played.
        *
        * @brief
        *  Initializes the sequencer.
        * @param hWnd
        *  Handle of window the sequencer belongs to (needed to setup music system).
        * @param pD3DDev
        *  Pointer to a valid D3D interface.
        * @param bEnableMusic
        *  Set to \b true if music should be enabled else \b false.
        * @param bLoopDemo
        *  Set to \b true if demo should be looped else \b false.
        * @return
        *  \b true if initialzation was successful else \b false.
        */
        bool Initialize( HWND hWnd, IDirect3DDevice8* pD3DDev,
                         bool bEnableMusic, bool bLoopDemo );

        /**
        * Creates and initializes all effects. All effects will be stored in #m_colEffects.
        *
        * @brief
        *  Creates and initializes all effects.
        * @return
        *  \b true if all effects were initialized successfully else \b
        *  false. That happens when the D3D device pointed to by #m_pD3DDev is incompatible with
        *  one or more of the effects, i.e. it doesn't have all necessary device capabilities
        *  to properly run this/these effect(s).
        */
        bool InitializeEffects();

        /**
        * Initializes the music system. This will also load and prepare the music for playback.
        *
        * @brief
        *  Initializes the music system.
        * @param hWnd
        *  Handle of window the sequencer belongs to.
        * @return
        *  \b true if music system was successfully initialized else \b false.
        */
        bool InitializeMusic( HWND hWnd );

        /**
        * Handles D3D device lost / reset situations. FrameUpdate() calls this method prior to updating
        * the current effect and rendering the new frame. As soon as a lost device is detected
        * it pauses the demo and returns. In a subsequent call of this method when it is possible
        * to reset the device it will perform a reset to gain full control over the D3D device again.
        * If the demo runs in windowed mode it will first re-read the desktop mode to accomodate for
        * possible changes in color resolution). In case the reset was successful the demo will be
        * resumed. All application managed resources of each registered effect will be freed before the
        * device gets reset and re-initialized thereafter.
        *
        * @brief
        *  Handles D3D device loss / reset situations.
        * @param sD3DPresentParameters
        *  Valid structure of current D3D present parameters.
        * @param bDeviceReady
        *  Will be set to \b true if the device is ready to use.
        * @return
        *  \b false if D3D device reset or re-initializing of effect managed resources failed
        *  else \b true.
        */
        bool HandleDeviceReset( D3DPRESENT_PARAMETERS& sD3DPresentParameters, bool& bDeviceReady );

        /**
        * Indicates whether sequencer is initialized or not.
        *
        * @brief
        *  Indicates whether sequencer is initialized or not.
        */
        bool m_bInitialized;

        /**
        * Current status of demo playback.
        *
        * @brief
        *  Current status of demo playback.
        */
        EDemoStatus m_eStatus;

        /**
        * Indicates whether music should enabled or not.
        *
        * @brief
        *  Indicates whether music should be enabled or not.
        */
        bool m_bEnableMusic;

        /**
        * Indicates whether demo should looped or not.
        *
        * @brief
        *  Indicates whether demo should looped or not.
        */
        bool m_bLoopDemo;

        /**
        * Indicates whether demo was playing before a device reset was initiated.
        *
        * @brief
        *  Indicates whether demo was playing before a device reset was initiated.
        */
        bool m_bWasPlaying;

        /**
        * Total runtime (in seconds) of demo. It's the sum of the runtime of all effects
        * stored in #m_colEffects.
        *
        * @brief
        *  Total runtime (in seconds) of demo.
        */
        float m_fTotalRuntime;

        /**
        * Pointer to a D3D interface.
        *
        * @brief
        *  Pointer to a D3D interface.
        */
        IDirect3DDevice8* m_pD3DDev;

        /**
        * List of all effects the sequencer should play.
        *
        * @brief
        *  List of all effects the sequencer should play.
        */
        EffectList m_colEffects;

        /**
        * Pointer (i.e. stl iterator) to the currently running effect of #m_colEffects
        * in the sequencer.
        *
        * @brief
        *  Pointer (i.e. stl iterator) to the currently running effect of #m_colEffects
        *  in the sequencer.
        */
        EffectList::iterator m_itCurrentEffect;

        /**
        * A timer to sequence and time effects.
        *
        * @brief
        *  A timer to sequence and time effects.
        */
        Shared::CTimer m_kTimer;

        /**
        * Pointer to a scrambled data archive. It gets created in Initialize().
        *
        * @brief
        *  Pointer to a scrambled data archive.
        */
        ScrambledArchive::CScrambledArchive* m_pkScrambledArchive;

        /**
        * A shared pool for static textures. To be used by all effects.
        *
        * @brief
        *  A shared pool for static textures.
        */
        Shared::CTextureManager* m_pkTextureManager;

        /**
        * A player for mp3 music files. It gets created in InitializeMusic().
        *
        * @brief
        *  A player for mp3 music files.
        */
        Shared::CMP3Player* m_pkMP3Player;

        /**
        * An effect transition manager.
        *
        * @brief
        *  An effect transition manager.
        */
        CEffectTransition* m_pkEffectTransition;
    };



#include "EffectSequencer.inl"



} // namespace EffectSystem



#endif // #ifndef _EFFECTSEQUENCER_H_INCLUDED_

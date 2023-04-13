///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Shared::CMP3Player.
 *
 * @file
 *  MP3Player.h
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Declaration of \e Shared::CMP3Player.
 */



#ifndef _MP3PLAYER_H_INCLUDED_
#define _MP3PLAYER_H_INCLUDED_

#pragma once



#include <string>
#include <vector>




/** 
 * Defintion of a stream handle. Taken from bass.h to avoid including it in this header.
 * 
 * @brief
 *  Defintion of a stream handle.
 */
typedef DWORD HSTREAM;

namespace ScrambledArchive
{
    class CScrambledArchive;
}



namespace Shared
{



    /**
    * A mp3 player class designed for playing music in the demo.
    * Basically, it's a wrapper around Ian Luck's BASS soundsystem.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  27.02.2002
    * @brief
    *  A mp3 player class designed for playing music in the demo.
    */
    class CMP3Player
    {
    public:
        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CMP3Player();
        
        /**
        * Creates a \e CMP3Player object ready to play the specified song.
        * 
        * @param hWnd
        *  Handle of window for which the mp3 player object should be created. Required for DirectSound.
        * @param kArchive
        *  Archive containing the song that should be played.
        * @param strSong
        *  Name of song that should be loaded from #kArchive for playback.
        * @return
        *  Pointer to the created \e CMP3Player object. \b NULL if creation failed 
        *  (see CMP3Player::Initialize() and CMP3Player::LoadSong() for possible reasons).
        * @brief
        *  Creates a \e CMP3Player object ready to play the specified song.
        */
        static CMP3Player* Create( HWND hWnd, 
                                   const ScrambledArchive::CScrambledArchive& kArchive, 
                                   const std::string& strSong );

        /**
        * Is playback currently activated?
        * 
        * @return
        *  \b true if playback is activated else \b false.
        * @brief
        *  Is playback currently activated?
        */
        bool IsPlaying() const;

        /**
        * Starts/resumes music playback.
        * 
        * @brief
        *  Starts/resumes music playback.
        */
        void Play();

        /**
        * Pauses music playback.
        * 
        * @brief
        *  Pauses music playback.
        */
        void Pause();

        /**
        * Stops music playback. Playback will start at the beginning of the song the next
        * time CMP3Player::Play() is called.
        *
        * @brief
        *  Stops music playback.
        */
        void Stop();

        /**
        * Resets music playback. If music was playing before it will start at the beginning 
        * of the song. If playback is either paused or stopped then music playback will start at
        * the beginning the next time CMP3Player::Play() is called.
        * 
        * @brief
        *  Resets music playback.
        */
        void Reset();

    private:  
        /** 
        * All possible playback states.
        * 
        * @brief
        *  All possible playback states.
        */
        enum EPlayerStatus
        {
            /** 
            * Music playback is stopped.
            */
            STOPPED,

            /** 
            * Music playback is paused.
            */
            PAUSED,

            /** 
            * Music is playing.
            */
            PLAYING
        };

    private:  
        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CMP3Player();

        /**
        * Decompresses and loads bass.dll. Gets the entry points for all BASS function used 
        * by the mp3 player. Initializes and starts the sound system.
        * 
        * @param hWnd
        *  Handle of window for which BASS should be used. Necessary for DirectSound.
        * @return
        *  \b true if successful else \b false.
        *  - Possible causes for a failed method call:
        *    -# Decompressing bass.dll from internal dll archive failed
        *    -# LoadLibrary() failed
        *    -# Retrieving the entry point for one of the used BASS functions failed
        *    -# Initializing and/or starting BASS failed
        *
        * @brief
        *  Initializes the mp3 player.
        */
        bool Initialize( HWND hWnd );

        /**
        * Loads a song which should be used for music playback.
        * 
        * @param kArchive
        *  Archive from which song should be loaded.
        * @param strSong
        *  Name of the song the should be loaded.
        * @return
        *  \b true if successful else \b false.
        *  - Possible causes for a failed method call:
        *    -# Decompressing the song from \e kArchive failed 
        *    -# Creating a memory file stream for song in BASS failed
        *
        * @brief
        *  Loads a song which should be used for music playback.
        */
        bool LoadSong( const ScrambledArchive::CScrambledArchive& kArchive, 
                       const std::string& strSong );

        /**
        * Retrieves the entry point for all used BASS functions.
        * 
        * @return
        *  \b true if successful else \b false (if the entry point for any
        *   of the BASS functions used by the mp3 player wasn't found).
        * @brief
        *  Retrieves the entry point for all used BASS functions.
        */
        bool LoadBassFunctions();

    private:
        /** 
        * Flag indicating that mp3 player is initialized.
        * 
        * @brief
        *  Flag indicating that mp3 player is initialized.
        */
        bool m_bInitialized;

        /** 
        * Current playback status of mp3 player.
        * 
        * @brief
        *  Current playback status of mp3 player.
        */
        EPlayerStatus m_eStatus;

        /** 
        * Handle to bass.dll.
        * 
        * @brief
        *  Handle to bass.dll.
        */
        HINSTANCE m_hBassDll;

        /** 
        * Handle to BASS mp3 stream.
        * 
        * @brief
        *  Handle to BASS mp3 stream.
        */
        HSTREAM m_hMP3Stream;

        /** 
        * Actual mp3 data of loaded song used for playback.
        * 
        * @brief
        *  Actual mp3 data of loaded song used for playback.
        */
        std::vector< unsigned char > m_colMP3Data;

        /** 
        * Temporary file for bass.dll. Needed to call LoadLibrary().
        * 
        * @brief
        *  Temporary file for bass.dll. Needed to call LoadLibrary().
        */
        std::string m_strTempBassFile;
    };



#include "MP3Player.inl"



} // namespace Shared



#endif // #ifndef _MP3PLAYER_H_INCLUDED_
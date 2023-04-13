///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CMP3Player.
 *
 * @file
 *  MP3Player.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Implementation of \e Shared::CMP3Player.
 */



#include "StdAfx.h"
#include "MP3Player.h"
#include "ScrambledArchive.h"
#include "Debug.h"

#define BASSDEF( f ) ( WINAPI *f )    // define the functions as pointers
#include "bass.h"



using namespace Shared;
using namespace ScrambledArchive;
using namespace std;



const char c_pcDllArchive[]         = "IDR_3RDPARTY_DLL";
const char c_pcBassDllArchivePath[] = "Bass\\Bass.dll";
const char c_pcTempBassDllPrefix[]  = "Bass";

static bool s_bInstanceCreated( false );



bool 
LoadBassFunction( HMODULE hModule, void*& lpFunction, const char* pcFunctionName )
{
    //__asm
    //{
    //    push pcFunctionName
    //    push hHandle
    //    mov eax, GetProcAddress
    //    call eax
    //    mov ebx, lpFunction
    //    mov dword ptr[ ebx ], eax
    //}

    lpFunction = GetProcAddress( hModule, pcFunctionName );
    return( ( 0 != lpFunction ) ? ( true ) : ( false ) );
}



CMP3Player* 
CMP3Player::Create( HWND hWnd, 
                    const ScrambledArchive::CScrambledArchive& kArchive, 
                    const std::string& strSong )
{
    CMP3Player* pkPlayer( 0 );

    if( false == s_bInstanceCreated )
    {
        pkPlayer = new CMP3Player;

        if( false == ( pkPlayer->Initialize( hWnd ) &&
                       pkPlayer->LoadSong( kArchive, strSong ) ) )
        {
            delete pkPlayer;
            pkPlayer = 0;
        }
    }

    return( pkPlayer );
}



CMP3Player::CMP3Player()
: m_bInitialized( false )
, m_eStatus( STOPPED )
, m_hBassDll( 0 )
, m_hMP3Stream( 0 )
, m_colMP3Data()
, m_strTempBassFile()
{
    s_bInstanceCreated = true;
}



CMP3Player::~CMP3Player()
{
    // free bass sound system and all its resources
    if( false != m_bInitialized )
    {
        BASS_Free();
        
        m_hMP3Stream = 0;
        m_eStatus = STOPPED;
    }

    // unload bass.dll
    if( 0 != m_hBassDll )
    {
        ::FreeLibrary( m_hBassDll );
        m_hBassDll = 0;
    }

    // remove temporary file
    if( false == m_strTempBassFile.empty() )
    {
        ::DeleteFile( m_strTempBassFile.c_str() );
        m_strTempBassFile.clear();
    }

    s_bInstanceCreated = false;
}



bool
CMP3Player::LoadBassFunctions()
{
#define PARAM_EXPAND( f ) m_hBassDll, (void *&) f, #f

    bool bReturn( true );
    bReturn &= LoadBassFunction( PARAM_EXPAND( BASS_Init ) );
    bReturn &= LoadBassFunction( PARAM_EXPAND( BASS_Free ) );
    bReturn &= LoadBassFunction( PARAM_EXPAND( BASS_Start ) );
    bReturn &= LoadBassFunction( PARAM_EXPAND( BASS_StreamCreateFile ) );
    bReturn &= LoadBassFunction( PARAM_EXPAND( BASS_StreamPlay ) );
    bReturn &= LoadBassFunction( PARAM_EXPAND( BASS_ChannelPause ) );
    bReturn &= LoadBassFunction( PARAM_EXPAND( BASS_ChannelSetPosition ) );

    return( bReturn );
}



bool
CMP3Player::Initialize( HWND hWnd )
{
    CScrambledArchive* pkArchive( CScrambledArchive::CreateFromResource( c_pcDllArchive ) );
    if( 0 != pkArchive )
    {
        // create a temporary file path for the bass dll
        char pcTempPath[ MAX_PATH ];
        char pcTempFile[ MAX_PATH ];

        GetTempPath( MAX_PATH, pcTempPath );
        GetTempFileName( pcTempPath, c_pcTempBassDllPrefix, 0 , pcTempFile );
        m_strTempBassFile = pcTempFile;

        // get bass dll from archive
        bool bDll( pkArchive->DecompressFileToDisk( c_pcBassDllArchivePath, m_strTempBassFile ) );
        
        // destroy archive object since we don't need it any longer
        delete pkArchive;

        // check if we successfully acquired the dll from the archive
        if( false == bDll )
        {
            DBG_ASSERTM( 0, "CMP3Player::Initialize() - Couldn't load bass.dll from archive!" );
            return( false );
        }

        // load bass.dll into our address space
        if( 0 == ( m_hBassDll = ::LoadLibrary( m_strTempBassFile.c_str() ) ) )
        {
            DBG_ASSERTM( 0, "CMP3Player::Initialize() - LoadLibrary() failed!" );
            return( false );
        }

        // bind all bass function we intend to call later
        if( false == LoadBassFunctions() )
        {
            DBG_ASSERTM( 0, "CMP3Player::Initialize() - Bass library function import failed!" );
            return( false );
        }

        // initialize and start bass
        if( ( FALSE == BASS_Init( -1, 44100, 0, hWnd )  ) || 
            ( FALSE == BASS_Start() ) )
        {
            DBG_ASSERTM( 0, "CMP3Player::Initialize() - BASS_Init() / BASS_Start() failed!" );
            return( false );
        }

        m_bInitialized = true;
    }

    return( true );
}



bool 
CMP3Player::LoadSong( const CScrambledArchive& kArchive, const string& strSong )
{
    // decompress mp3 data into m_colMP3Data
    // any data stored m_colMP3Data will be discarded
    if( false == kArchive.DecompressFile( strSong, m_colMP3Data ) )
    {
        DBG_ASSERTM( 0, "CMP3Player::LoadSong() - Failed to decompress song!" );
        return( false );
    }

    // create bass in memory file stream
    if( 0 == ( m_hMP3Stream = BASS_StreamCreateFile( 
        TRUE, &m_colMP3Data[ 0 ], 0, (DWORD) m_colMP3Data.size(), BASS_MP3_SETPOS ) ) )
    {
        DBG_ASSERTM( 0, "CMP3Player::LoadSong() - Failed to create stream!" );
        return( false );
    }

    return( true );
}



void 
CMP3Player::Play()
{
    DBG_ASSERT( false != m_bInitialized && 0 != m_hMP3Stream );
    if( PLAYING != m_eStatus )
    {
        m_eStatus = PLAYING;
        BASS_StreamPlay( m_hMP3Stream, FALSE, 0 );
    }
}



void 
CMP3Player::Pause()
{
    DBG_ASSERT( false != m_bInitialized && 0 != m_hMP3Stream );
    if( PLAYING == m_eStatus )
    {
        m_eStatus = PAUSED;

        BASS_ChannelPause( m_hMP3Stream );
    }
}
 


void 
CMP3Player::Stop()
{
    DBG_ASSERT( false != m_bInitialized && 0 != m_hMP3Stream );
    if( PLAYING == m_eStatus || PAUSED == m_eStatus )
    {
        if( PLAYING == m_eStatus )
        {
            BASS_ChannelPause( m_hMP3Stream );
        }       
        BASS_ChannelSetPosition( m_hMP3Stream, 0 );

        m_eStatus = STOPPED;
    }
}



void 
CMP3Player::Reset()
{
    DBG_ASSERT( false != m_bInitialized && 0 != m_hMP3Stream );
    BASS_ChannelSetPosition( m_hMP3Stream, 0 );
}
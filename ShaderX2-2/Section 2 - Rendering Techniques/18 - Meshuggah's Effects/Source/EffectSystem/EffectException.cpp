///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e EffectSystem::CEffectException.
 *
 * @file
 *  EffectException.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  27.11.2001
 * @brief
 *  Implementation of \e EffectSystem::CEffectException.
 */



#include "stdafx.h"
#include "EffectException.h"
#include "Debug.h"
#include "stdio.h"



using namespace EffectSystem;



const char c_pcLogFilepath[] = "EffectException.log";

bool CEffectException::s_bIsFullscreen( false );



void
CEffectException::ReportError()
{
#ifdef _DEBUG
    FILE* pErrorLog( fopen( c_pcLogFilepath, "a" ) );
    if( 0 != pErrorLog )
    {        
        fseek( pErrorLog, 0, SEEK_END );
        fwrite( what(), 1, strlen( what() ), pErrorLog );
        fwrite( "\n", 1, 1, pErrorLog );
        fclose( pErrorLog );
    }
#endif

    if( false == s_bIsFullscreen )
    {
        ::MessageBox( 0, what(), "An error occurred!", MB_OK | MB_ICONINFORMATION );
    }
}



void 
CEffectException::SetFullscreenFlag( bool bIsFullscreen )
{
    s_bIsFullscreen = bIsFullscreen;
}



void
CEffectException::PrepareLogFile()
{
#ifdef _DEBUG
    FILE* pErrorLog( fopen( c_pcLogFilepath, "a" ) );
    if( 0 != pErrorLog )
    {
        const char c_pcLogHeader[] = "\n\n--- New log session started ---\n\n";

        fseek( pErrorLog, 0, SEEK_END );
        fwrite( c_pcLogHeader, 1, strlen( c_pcLogHeader ), pErrorLog );
        fclose( pErrorLog );
    }
#endif
}
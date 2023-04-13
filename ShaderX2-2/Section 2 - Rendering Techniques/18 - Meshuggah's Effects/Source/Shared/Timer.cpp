///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CTimer.
 *
 * @file
 *  Timer.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Implementation of \e Shared::CTimer.
 */



#include "stdafx.h"
#include "timer.h"
#include "mmsystem.h"



using namespace Shared;



void 
CTimer::Initialize()
{
    __int64 qTicksPerSec( 0 );
        
    if( FALSE != QueryPerformanceFrequency( reinterpret_cast< LARGE_INTEGER* >( &qTicksPerSec ) ) )
    {
        // high precision timer is available
        m_fSecondsPerTick = (float) ( 1.0 / (double) qTicksPerSec );        
        m_bHighResTimer = true;
    }
    else
    {
        // use MM timer instead     
        m_fSecondsPerTick = 1e-3f;
        m_bHighResTimer = false;        
    }
}




__int64 
CTimer::GetCounter() const
{
    __int64 qCounter;
    if( false != m_bHighResTimer )
    {
        QueryPerformanceCounter( reinterpret_cast< LARGE_INTEGER* >( &qCounter ) );     
    }
    else
    {
        qCounter = timeGetTime();
    }

    return( qCounter );
}
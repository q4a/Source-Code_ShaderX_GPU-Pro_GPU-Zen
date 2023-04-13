///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CTimer (inline methods).
 *
 * @file
 *  Timer.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Implementation of \e Shared::CTimer (inline methods).
 */



#ifndef _TIMER_INL_INCLUDED_
#define _TIMER_INL_INCLUDED_

#pragma once



inline 
CTimer::CTimer( float fTimeOffset )
: m_bHighResTimer( false )
, m_eStatus( STOPPED )
, m_fSecondsPerTick( 0 )
, m_fTimeOffset( fTimeOffset )
, m_qCounterStart( 0 )
{
    Initialize();
}



inline 
CTimer::~CTimer()
{
}



inline bool 
CTimer::IsRunning() const
{
    return( RUNNING == m_eStatus );
}



inline void 
CTimer::Reset()
{
    if( RUNNING == m_eStatus )
    {
        m_qCounterStart = GetCounter();
    }
    m_fTimeOffset = 0.0f;
}



inline void 
CTimer::Start()
{
    if( RUNNING != m_eStatus )
    {
        m_qCounterStart = GetCounter();
        m_eStatus = RUNNING;
    }
}



inline void 
CTimer::Pause()
{
    if( RUNNING == m_eStatus )
    {
        m_fTimeOffset = GetTime();
        m_eStatus = PAUSED;
    }
}



inline void 
CTimer::Stop()
{
    if( RUNNING == m_eStatus || PAUSED == m_eStatus )
    {
        m_fTimeOffset = 0.0f;
        m_eStatus = STOPPED;
    }
}



inline float 
CTimer::GetTime() const
{           
    if( RUNNING != m_eStatus )
    {
        return( m_fTimeOffset );        
    }
    else
    {
        return( m_fTimeOffset + m_fSecondsPerTick * ( GetCounter() - m_qCounterStart ) );
    }
}



inline void 
CTimer::SetTime( float fTime )
{
    m_fTimeOffset = fTime;
    if( RUNNING == m_eStatus )
    {
        m_qCounterStart = GetCounter();
    }
}



#endif // #ifndef _TIMER_INL_INCLUDED_
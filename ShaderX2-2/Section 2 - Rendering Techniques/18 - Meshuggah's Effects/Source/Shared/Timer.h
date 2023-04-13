///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Shared::CTimer.
 *
 * @file
 *  Timer.h
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Declaration of \e Shared::CTimer.
 */



#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#pragma once



namespace Shared
{



    /**
    * A timer class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  27.02.2002
    * @brief
    *  A timer class.
    */
    class CTimer
    {
    public:
        /**
        * Constructor.
        * 
        * @param fTimeOffset
        *  Time offset in seconds. Timing starts at this value.
        * @brief
        *  Constructor.
        */
        CTimer( float fTimeOffset = 0.0f );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CTimer();

        /**
        * Is timer currently running?
        * 
        * @return
        *  \b true if timer is running else \b false.
        * @brief
        *  Is timer currently running?
        */
        bool IsRunning() const;

        /**
        * Starts/resumes the timer.
        * 
        * @brief
        *  Starts/resumes the timer.
        */
        void Start();

        /**
        * Pauses the timer.
        * 
        * @brief
        *  Pauses the timer.
        */
        void Pause();

        /**
        * Stops the timer.
        * 
        * @brief
        *  Stops the timer.
        */
        void Stop();

        /**
        * Resets the timer to zero. Doesn't affect its status.
        * 
        * @brief
        *  Resets the timer to zero.
        */
        void Reset();

        /**
        * Gets the current time in seconds.
        * 
        * @return
        *  Current time in seconds.
        * @brief
        *  Gets the current time in seconds.
        */
        float GetTime() const;  

        /**
        * Sets the current time.
        * 
        * @param fTime
        *  Current time in seconds.
        * @brief
        *  Sets the current time.
        */
        void SetTime( float fTime );

    private:
        /** 
        * All possible timer states.
        * 
        * @brief
        *  All possible timer states.
        */
        enum ETimerStatus
        {
            /** 
            * Timer is stopped.
            */ 
            STOPPED,

            /** 
            * Timer is paused.
            */ 
            PAUSED,

            /** 
            * Timer is running.
            */ 
            RUNNING
        };

        /**
        * Initializes the timer. Tries to use a high precision timer via \c QueryPerformanceFrequency() first.
        * If this is not available it uses a MM timer instead. #m_bHighResTimer will be set accordingly to 
        * reflect the initialization result.
        * 
        * @brief
        *  Initializes the timer.
        */
        void Initialize();

        /**
        * Gets the current counter value. It comes from either \c QueryPerformanceCounter()
        * or a MM timer depending on the value of #m_bHighResTimer.
        * 
        * @return
        *  Current counter value. It will be used in CTimer::GetTime() to get the current time.
        * @brief
        *  Gets the current counter value.
        */
        __int64 GetCounter() const;

    private:
        /** 
        * High resolution timer is used.
        * 
        * @brief
        *  High resolution timer is used.
        */
        bool m_bHighResTimer;

        /** 
        * Current status of timer.
        * 
        * @brief
        *  Current status of timer.
        */
        ETimerStatus m_eStatus;

        /** 
        * Used to convert counter values to a time.
        * 
        * @brief
        *  Used to convert counter values to a time.
        */
        float m_fSecondsPerTick;

        /** 
        * Timer offset in seconds.
        * 
        * @brief
        *  Timer offset in seconds.
        */
        float m_fTimeOffset;

        /** 
        * Value of counter when timer was last started.
        * Used as the reference value for calculating the current time in CTimer::GetTime().
        * 
        * @brief
        *  Value of counter when timer was last started.
        */
        __int64 m_qCounterStart;
    };



#include "Timer.inl"



} // namespace Shared



#endif // #ifndef _TIMER_H_INCLUDED_
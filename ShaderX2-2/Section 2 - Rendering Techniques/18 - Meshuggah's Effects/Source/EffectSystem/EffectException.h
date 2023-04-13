///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e EffectSystem::CEffectException.
 *
 * @file
 *  EffectException.h
 * @author
 *  Carsten Wenzel
 * @date
 *  27.11.2001
 * @brief
 *  Declaration of \e EffectSystem::CEffectException.
 */



#ifndef _EFFECTEXCEPTION_H_INCLUDED_
#define _EFFECTEXCEPTION_H_INCLUDED_

#pragma once



#include <exception>


namespace EffectSystem
{



    /**
    * An effect specific exception class. Mostly used to throw exceptions during effect creation.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  23.11.2001
    * @brief
    *  An effect specific exception class.
    */
    class CEffectException : public std::exception
    {
    public:

        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        * @param pcReason
        *  String that describes the reason for the exception.
        */
        CEffectException( const char* pcReason )
        : exception( pcReason )
        {
        }

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
         */
        virtual ~CEffectException()
        {
        }

        /**
        * Reports an error to the user.
        * 
        * @brief
        *  Reports error to the user.
        */
        void ReportError();

        /**
        * Sets #c_bIsFullscreen.
        * 
        * @param bIsFullscreen
        *  Flag that indicates the program is running in fullscreen mode.
        * @brief
        *  Sets #c_bIsFullscreen.
        */
        static void SetFullscreenFlag( bool bIsFullscreen );

        /**
        * Prepares the effect exception log file.
        * 
        * @brief
        *  Prepares the effect exception log file.
        */
        static void PrepareLogFile();

    private:
        /** 
        * Indicates that the program is running in fullscreen mode.
        * This is relevent information when an effect exception is throw as in fullscreen mode
        * a message dialog won't show up and the program "seems to hang".
        * 
        * @brief
        *  Indicates that the program is running in fullscreen mode.
        */
        static bool s_bIsFullscreen;
    };



} // namespace EffectSystem



#endif // #ifndef _EFFECTEXCEPTION_H_INCLUDED_
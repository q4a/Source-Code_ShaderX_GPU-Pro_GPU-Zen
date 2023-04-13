///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration and implementation of \e Shared::CTMException. For internal use in 
 * TextureManager.cpp only!
 *
 * @file
 *  TMException.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  13.11.2001
 * @brief
 *  Declaration and implementation of \e Shared::CTMException.
 */



#ifndef _TMEXCEPTION_INL_INCLUDED_
#define _TMEXCEPTION_INL_INCLUDED_

#pragma once



#include <exception>



namespace Shared
{



    /**
    * A texture manager exception class. Used to throw an exception in case of
    * an error during texture loading / converting.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  28.02.2002
    * @brief
    *  A texture manager exception class.
    */
    class CTMException : public std::exception  
    {
    public:
        /**
        * Constructor.
        * 
        * @param pcReason
        *  String describing the error.
        * @param hr
        *  HRESULT value of a (D3DX) function indicating an error.
        * @brief
        *  Constructor.
        */
        CTMException( const char* pcReason, HRESULT hr = E_FAIL )
        : std::exception( pcReason )
        , m_hr( hr )
        {
        }

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CTMException()
        {
        }

        /**
        * Returns the HRESULT value of the exception.
        * 
        * @return
        *  HRESULT value of the exception.
        * @brief
        *  Returns the HRESULT value of the exception.
        */
        HRESULT GetHRESULT()
        {
            return( m_hr );
        }

        /**
        * Reports the error. In release builds this method is empty.
        * 
        * @brief
        *  Reports the error. 
        */
        void ReportError()
        {
            DBG_ASSERTM( 0, what() );
            DBG_TRACED3D( GetHRESULT() );
        }

    private:
        /** 
        * HRESULT value of the exception. Gives a specific detail about the error.
        * 
        * @brief
        *  HRESULT value of the exception.
        */
        HRESULT m_hr;
    };



} // namespace shared



#endif // #ifndef _TMEXCEPTION_INL_INCLUDED_
///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Some rather useful debugging macros.
 *
 * @file
 *  Debug.h
 * @author
 *  Carsten Wenzel
 * @date
 *  10.11.2001
 * @brief
 * Some rather useful debugging macros.
 */



#ifndef _SSJ_ASSERT_H_ 
#define _SSJ_ASSERT_H_



#pragma once



#ifdef _DEBUG



/** 
 * Prototype for an assert handler.
 * It is only available in debug builds.
 * 
 * @param pcExp
 *  Assertion expression.
 * @param pcMsg
 *  Message to be displayed.
 * @param pcFile
 *  File where assertion failed.
 * @param iLine
 *  Line where assertion failed.
 * @param bIgnoreAlways
 *  [out] Should same assertion be ignored next time.
 * @brief
 *  Prototype for an assert handler.
 */
typedef bool (ASSERTHANDLER) ( const char *pcExp, const char *pcMsg, const char *pcFile, int iLine, bool *bIgnoreAlways );



/**
 * Allows to set a user specific assert handler. If \e pAssertHandler is \b NULL the  
 * default handler will be set.
 * 
 * @brief
 *  Allows to set a user specific assert handler.
 * @param pAssertHandler
 *  Pointer to a user specific assert handler.
 */
void SetAssertHandler( ASSERTHANDLER *pAssertHandler = 0 );



/**
 * Enables or disables the minimization of the app window in case an assertion failed. 
 * For single monitor fullscreen DirectX applications.
 *
 * @brief
 *  Enables or disables the minimization of the app window in case an assertion failed.
 * @param bEnable
 *  Flag specifing the new behaviour.
 */
void AssertMinimizeApp( bool bEnable );



/**
 * Helper function for the assert macro.
 * 
 * @brief
 *  For internal use only.
 */
bool HandleAssert( const char *pcExp, const char *pcMsg, const char *pcFile, int iLine, bool *bIgnoreAlways );



    /** 
    * An assertion macro.
    * 
    * @brief
    *  An assertion macro.
    */
#   define DBG_ASSERT( exp ) \
    { \
        static bool bIgnoreAlways = false; \
        if( !bIgnoreAlways && !( exp ) ) \
        { \
            if( HandleAssert( #exp, 0, __FILE__, __LINE__, &bIgnoreAlways ) ) \
                { _asm { int 3 } } \
        } \
    }



/** 
 * Extension of #DBG_ASSERT which allows to specify a message.
 * 
 * @brief
 *  Extension of #DBG_ASSERT which allows to specify a message.
 */
#   define DBG_ASSERTM( exp, msg ) \
    { \
        static bool bIgnoreAlways = false; \
        if( !bIgnoreAlways && !( exp ) ) \
        { \
            if( HandleAssert( #exp, msg, __FILE__, __LINE__, &bIgnoreAlways ) ) \
                { _asm { int 3 } } \
        } \
    }

#else

#   define DBG_ASSERT( exp )
#   define DBG_ASSERTM( exp, msg )

#endif // #ifdef _DEBUG



#ifdef _DEBUG



    /** 
    * Template structure for compile time assertion.
    * 
    * @brief
    *  Template structure for compile time assertion.
    */
    template< bool > 
    struct TCompileTimeChecker
    {
        // constructor which accepts anything
        // this does the trick ;) Thanks to Andrei Alexandrescu!
        TCompileTimeChecker( ... );
    };

    /** 
    * Template structure specialization for compile time assertion.
    * 
    * @brief
    *  Template structure for specialization time assertion.
    */
    template<> 
    struct TCompileTimeChecker< false >
    {
        // An empty specialized template struture.
        // The compiler will complain about that we when try to used it in 
        // the macro below, which is exactly what we want!
    };



    /** 
    * Compile time assertion macro. Use it like this 
    * \c DBG_ASSERT_STATIC( false, This_will_always_fail )
    * 
    * @brief
    *  Compile time assertion macro.
    */
#   define DBG_ASSERT_STATIC( exp, msg ) \
    { \
        class ERROR_##msg {}; \
        (void) sizeof( TCompileTimeChecker< ( exp ) != 0 >( ( ERROR_##msg() ) ) ); \
    }

#else

#   define DBG_ASSERT_STATIC( exp, msg )

#endif // _DEBUG



#ifdef _DEBUG

/**
 * Helper function for the DBG_TRACE macro.
 * 
 * @brief
 *  For internal use only.
 */
void Trace( const char *pcMsg, ... );



/** 
 * A trace macro.
 * 
 * @brief
 *  A trace macro.
 */
#   define DBG_TRACE Trace

#else

#if( _MSC_VER >= 1210 )
#   define DBG_TRACE ( __noop )
#else
#   define DBG_TRACE ( (void) 0 )
#endif

#endif // #ifdef _DEBUG



#ifdef _DEBUG

/**
 * Helper function for the #DBG_TRACED3D macro.
 * 
 * @brief
 *  For internal use only.
 */
void TraceD3D( HRESULT hr, char* pcError, const char *pcFile, int iLine );

/**
 * Helper function for the #DBG_TRACED3D macro. This function is placed in the header
 * so that you don't need to link to d3dx8.lib in a project that doesn't use
 * the #DBG_TRACED3D macro.
 * 
 * @brief
 *  For internal use only.
 */
inline void
GetD3DErrorString( HRESULT hr, char* pcError )
{

#ifdef DIRECT3D_VERSION

    D3DXGetErrorString( hr, pcError, 256 );

#else

    pcError[ 0 ] = 0;

#endif

}

/** 
 * A macro to trace D3D related HRESULTs with the corresponding error information.
 * 
 * @brief
 *  A macro to trace D3D related HRESULTs with the corresponding error information.
 */ 
#   define DBG_TRACED3D( hr ) \
    { \
        char pcError[ 256 ]; \
        GetD3DErrorString( hr, pcError ); \
        TraceD3D( hr, pcError, __FILE__ , __LINE__ ); \
    }

#else

#   define DBG_TRACED3D( hr )

#endif



#endif // #ifdef _SSJ_ASSERT_H_

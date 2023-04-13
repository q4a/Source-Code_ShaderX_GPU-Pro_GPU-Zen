///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration and implementation of \e Shared::Clamp template function.
 *
 * @file
 *  Clamp.h
 * @author
 *  Carsten Wenzel
 * @date
 *  03.12.2001
 * @brief
 *  Declaration and implementation of \e Shared::Clamp template function. 
 */



#ifndef _CLAMP_H_INCLUDED_
#define _CLAMP_H_INCLUDED_


namespace Shared
{



/**
 * Template function that clamps input to a given interval. That is, 
 * the clamped value x' is in [lower, upper].
 * 
 * @param x
 *  The value to be clamped.
 * @param lower
 *  Lower value of clamp interval.
 * @param upper
 *  Upper value of clamp interval.
 * @return
 *  Clamped value (x').
 * @brief
 *  Template function that clamps input to a given interval.
 */
template < typename T >
inline T 
Clamp( T x, T lower, T upper )
{
    if( x < lower )
    {
        return( lower );
    }
    else
    if( x > upper )
    {
        return( upper );
    }

    return( x );
}



} // namespace Shared



#endif // #ifndef _CLAMP_H_INCLUDED_
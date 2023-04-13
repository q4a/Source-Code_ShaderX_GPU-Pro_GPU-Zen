///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CRandom (inline methods).
 *
 * @file
 *  Random.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  01.12.2001
 * @brief
 *  Implementation of \e Shared::CRandom (inline methods).
 */



#ifndef _RANDOM_INL_INCLUDED_
#define _RANDOM_INL_INCLUDED_

#pragma once



inline 
CRandom::CRandom( unsigned long ulSeed )
: m_ulIndex( 0 )
{
    SetSeed( ulSeed );
}



inline
CRandom::~CRandom()
{
}



inline unsigned long
CRandom::GetInteger()
{
    if( N == m_ulIndex )
    {
        Reload();
    }

    unsigned long ulY = m_ulState[ m_ulIndex++ ];
    ulY ^= TemperingShiftU( ulY );
    ulY ^= TemperingShiftS( ulY ) & TEMPERING_MASK_B;
    ulY ^= TemperingShiftT( ulY ) & TEMPERING_MASK_C;
    ulY ^= TemperingShiftL( ulY );

    return( ulY ); 
}



inline double
CRandom::Get()
{
    // map random value to [0, 1) 
    return( GetInteger() * (double) 2.3283064365386963e-10 ); // i.e. GetInteger() / 2^32
}



inline double
CRandom::GetInRange( double dLower, double dUpper )
{
    return( dLower + Get() * ( dUpper - dLower ) );
}



inline unsigned long 
CRandom::TemperingShiftU( unsigned long ulX )
{
    return( ulX >> 11 );
}



inline unsigned long 
CRandom::TemperingShiftS( unsigned long ulX )
{
    return( ulX << 7 );
}



inline unsigned long 
CRandom::TemperingShiftT( unsigned long ulX )
{
    return( ulX << 15 );
}
    


inline unsigned long
CRandom::TemperingShiftL( unsigned long ulX )
{
    return( ulX >> 18 );
}



#endif // #ifndef _RANDOM_INL_INCLUDED_

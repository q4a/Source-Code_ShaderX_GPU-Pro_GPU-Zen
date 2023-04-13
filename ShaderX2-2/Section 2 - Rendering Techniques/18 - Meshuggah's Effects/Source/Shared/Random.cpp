///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CRandom.
 *
 * @file
 *  Random.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  01.12.2001
 * @brief
 *  Implementation of \e Shared::CRandom.
 */



#include "stdafx.h"
#include "Random.h"
#include "math.h"



using namespace Shared;



void 
CRandom::SetSeed( unsigned long ulSeed )
{
    m_ulState[ 0 ] = ( ulSeed | 1 ) & 0xFFFFFFFFU;
    for( m_ulIndex = 1; m_ulIndex < N; ++m_ulIndex )
    {
        m_ulState[ m_ulIndex ] = ( m_ulState[ m_ulIndex - 1 ] * GENERATOR ) & 0xFFFFFFFFU;
    }
}



void 
CRandom::Reload()
{
    const unsigned long c_ulMag01[ 2 ] = { 0x0, MATRIX_A };

    unsigned long ulY;
    for( m_ulIndex = 0; m_ulIndex < N - M; ++m_ulIndex ) 
    {
        ulY = ( m_ulState[ m_ulIndex     ] & UPPER_MASK ) |
              ( m_ulState[ m_ulIndex + 1 ] & LOWER_MASK );
        
        m_ulState[ m_ulIndex ] = m_ulState[ m_ulIndex + M ] ^ ( ulY >> 1 ) ^ c_ulMag01[ ulY & 0x1 ];
    }
    
    for( ; m_ulIndex < N  - 1; ++m_ulIndex)
    {
        ulY = ( m_ulState[ m_ulIndex     ] & UPPER_MASK ) | 
              ( m_ulState[ m_ulIndex + 1 ] & LOWER_MASK );

        m_ulState[ m_ulIndex ] = m_ulState[ m_ulIndex + ( M - N ) ] ^ ( ulY >> 1 ) ^ c_ulMag01[ ulY & 0x1 ];
    }

    ulY = ( m_ulState[ N - 1 ] & UPPER_MASK ) | ( m_ulState[ 0 ] & LOWER_MASK );
    m_ulState[ N - 1 ] = m_ulState[ M - 1 ] ^ ( ulY >> 1 ) ^ c_ulMag01[ ulY & 0x1 ];
    
    m_ulIndex = 0;
}



double
CRandom::GetGauss( double dMean, double dStdDeviation )
{
    double dX1; 
    double dX2;
    double dW;

    // perform box muller test
    do
    {
        dX1 = 2.0 * Get() - 1.0;
        dX2 = 2.0 * Get() - 1.0;
        dW  = dX1 * dX1 + dX2 * dX2;
    } while( dW >= 1.0 );
    
    dW = sqrt( -2.0f * log( dW ) / dW );
    dX1 *= dW;

    return( dMean + dX1 * dStdDeviation );
}



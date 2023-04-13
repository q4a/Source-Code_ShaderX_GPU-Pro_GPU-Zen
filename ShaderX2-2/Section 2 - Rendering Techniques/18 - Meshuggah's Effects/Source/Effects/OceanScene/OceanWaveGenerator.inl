    ///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e COceanWaveGenerator (inline methods).
 *
 * @file
 *  OceanWaveGenerator.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  22.12.2001
 * @brief
 *  Implementation of \e COceanWaveGenerator (inline methods).
 */



#ifndef _OCEANWAVEGENERATOR_INL_INCLUDED_
#define _OCEANWAVEGENERATOR_INL_INCLUDED_

#pragma once



inline float 
COceanWaveGenerator::GetOceanHeight( int x, int y )
{
    return( -m_colH[ OffsetWrap( x, y ) ].real() );
}



inline float 
COceanWaveGenerator::GetOceanDisplaceX( int x, int y )
{
    return( m_colD[ OffsetWrap( x, y ) ].real() * m_fChoppyWaveFactor );
}
 


inline float 
COceanWaveGenerator::GetOceanDisplaceY( int x, int y )
{
    return( m_colD[ OffsetWrap( x, y ) ].imag() * m_fChoppyWaveFactor );
}



inline float 
COceanWaveGenerator::GetOceanNormalX( int x, int y )
{
    return( m_colN[ OffsetWrap( x, y ) ].real() );
}
 


inline float 
COceanWaveGenerator::GetOceanNormalY( int x, int y )
{
    return( m_colN[ OffsetWrap( x, y ) ].imag() );
}



inline float 
COceanWaveGenerator::GetKx( int x )
{
    return ( 2.0f * sc_fPi * x / sc_fGridDelta );
}



inline float 
COceanWaveGenerator::GetKy( int y )
{
    return ( 2.0f * sc_fPi * y / sc_fGridDelta );
}



inline float 
COceanWaveGenerator::sqrf( float x )
{
    return ( x * x );
}



inline float 
COceanWaveGenerator::PhillipsSpectrum( float fKx, float fKy )
{
    float fKLength( sqrtf( sqrf( fKx ) + sqrf( fKy ) ) );

    if( fKLength < 1e-8f )
    {
        fKLength = 1e-8f;
    }

    float fScale( 1.0f / fKLength );
    fKx *= fScale;
    fKy *= fScale;

    return ( m_fWaveHeight * 
             expf( -1.0f / sqrf( fKLength * m_fLargestPossibleWave ) - sqrf( fKLength * m_fSuppressSmallWaves ) ) * 
             powf( fKx * m_fWindX + fKy * m_fWindY, m_fDirectionalDependence ) / powf( fKLength, 4.0f ) );
}



inline float 
COceanWaveGenerator::GetAngularFrequency( float fKLength )
{
    // take water depth into account
    //return( sqrtf( sc_fGravity * fKLength * tanhf( fKLength * sc_fDepth ) ) );

    // special version for deep water
    return( sqrtf( sc_fGravity * fKLength ) );
}



inline int 
COceanWaveGenerator::IndexH0( int x, int y )
{
    return( x + ( GRIDSIZE / 2 ) + ( GRIDSIZE + 1 ) * ( y + ( GRIDSIZE / 2 ) ) );
}



inline int 
COceanWaveGenerator::Index( int x, int y )
{
    return( x + ( GRIDSIZE / 2 ) + GRIDSIZE * ( y + ( GRIDSIZE / 2 ) ) );
}



inline int 
COceanWaveGenerator::IndexFFT( int x, int y )
{
    // get the appropriate index to store data in the right order for the 2d fft
    // (that is, dc is stored at (0, 0) and not at ( GRIDSIZE / 2, GRIDSIZE / 2 ) !!!)
    return( ( x & ( GRIDSIZE - 1 ) ) + GRIDSIZE * ( y & ( GRIDSIZE - 1 ) ) );
}



inline int 
COceanWaveGenerator::Offset( int x, int y )
{
    return( x + GRIDSIZE * y );
}



inline int 
COceanWaveGenerator::OffsetWrap( int x, int y )
{
    return( Offset( x & ( GRIDSIZE - 1 ), y & ( GRIDSIZE - 1 ) ) );
}



#endif // #ifndef _OCEANWAVEGENERATOR_INL_INCLUDED_
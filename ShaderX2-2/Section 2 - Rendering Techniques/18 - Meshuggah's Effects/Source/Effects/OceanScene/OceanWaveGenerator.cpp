///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e COceanWaveGenerator.
 *
 * @file
 *  OceanWaveGenerator.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  22.12.2001
 * @brief
 *  Implementation of \e COceanWaveGenerator.
 */



#include "StdAfx.h"
#include "OceanWaveGenerator.h"
#include "Random.h"
#include "math.h"



using namespace Effects;
using namespace Shared;
using namespace std;



const float COceanWaveGenerator::sc_fPi( 3.141592654f );

const float COceanWaveGenerator::sc_fGravity( 9.81f );

const float COceanWaveGenerator::sc_fDefaultWindDirection( D3DX_PI * 0.5f );

const float COceanWaveGenerator::sc_fDefaultWindSpeed( 16.0f );

const float COceanWaveGenerator::sc_fDefaultWaveHeight( 256.0f );

// the bigger the value the more aligned waves will be to the wind direction 
// !!! must be a multiple of two and >= 2 !!!
const float COceanWaveGenerator::sc_fDefaultDirectionalDependence( 2.0f );

const float COceanWaveGenerator::sc_fDefaultChoppyWaveFactor( 0.015f );

// suppress waves much smaller that the largest possible wave increasing this value will smooth the waves
const float COceanWaveGenerator::sc_fDefaultSuppressSmallWavesFactor( 0.00005f );

const float COceanWaveGenerator::sc_fGridDelta( 1.0f );



COceanWaveGenerator::COceanWaveGenerator()
: m_colH0( ( GRIDSIZE + 1 ) * ( GRIDSIZE + 1 ) )
, m_colH( GRIDSIZE * GRIDSIZE )
, m_colD( GRIDSIZE * GRIDSIZE )
, m_colN( GRIDSIZE * GRIDSIZE )
, m_colAngularFreq( GRIDSIZE * GRIDSIZE )
, m_colKScale( GRIDSIZE * GRIDSIZE )
, m_fWindX( 0.0f )
, m_fWindY( 0.0f )
, m_fWindSpeed( 0.0f )
, m_fLargestPossibleWave( 0.0f )
, m_fSuppressSmallWaves( 0.0f )
, m_fWaveHeight( 0.0f )
, m_fDirectionalDependence( 0.0f )
, m_fChoppyWaveFactor( 0.0f )
, m_fSuppressSmallWavesFactor( 0.0f )
{
    SetOceanParameters( 4357, sc_fDefaultWindDirection, sc_fDefaultWindSpeed,
                        sc_fDefaultWaveHeight, sc_fDefaultDirectionalDependence,
                        sc_fDefaultChoppyWaveFactor, sc_fDefaultSuppressSmallWavesFactor );
}



COceanWaveGenerator::~COceanWaveGenerator()
{
}



void 
COceanWaveGenerator::SetOceanParameters( unsigned long ulSeed, float fWindDirection, float fWindSpeed,
                                         float fWaveHeight, float fDirectionalDependence, 
                                         float fChoppyWavesFactor, float fSuppressSmallWavesFactor )
{
    m_fWindX                    = cosf( fWindDirection );    
    m_fWindY                    = sinf( fWindDirection );
    m_fWindSpeed                = fWindSpeed;
    m_fWaveHeight               = fWaveHeight;
    m_fDirectionalDependence    = fDirectionalDependence;
    m_fChoppyWaveFactor         = fChoppyWavesFactor;
    m_fSuppressSmallWavesFactor = fSuppressSmallWavesFactor;

    m_fLargestPossibleWave =  m_fWindSpeed * m_fWindSpeed / sc_fGravity;   
    m_fSuppressSmallWaves  = m_fLargestPossibleWave * m_fSuppressSmallWavesFactor;

    // init H0
    CRandom kRnd( ulSeed );
    float fOneBySqrtTwo( 1.0f / sqrtf( 2.0f ) );
    for( int j( -GRIDSIZE / 2 ); j <= GRIDSIZE / 2; ++j )
    {
        for( int i( -GRIDSIZE / 2 ); i <= GRIDSIZE / 2; ++i )
        {
            Complex cmpRnd( (float) kRnd.GetGauss(), (float) kRnd.GetGauss() );
            cmpRnd *= fOneBySqrtTwo * sqrtf( PhillipsSpectrum( GetKx( i ), GetKy( j ) ) );

            m_colH0[ IndexH0( i, j ) ] = cmpRnd;
        }
    } 

    // precalc length of K
    for( int j( -GRIDSIZE / 2 ); j < GRIDSIZE / 2; ++j )
    {
        for( int i( -GRIDSIZE / 2 ); i < GRIDSIZE / 2; ++i )
        {
            float fKLength( sqrtf( sqrf( GetKx( i ) ) + sqrf( GetKy( j ) ) ) );
            if( fKLength < 1e-8f )
            {
                fKLength = 1e-8f;
            }
            m_colKScale[ Index( i, j ) ] = 1.0f / fKLength;
        }
    }

    // init angular frequencies
    for( int j( -GRIDSIZE / 2 ); j < GRIDSIZE / 2; ++j )
    {
        for( int i( -GRIDSIZE / 2 ); i < GRIDSIZE / 2; ++i )
        {
            float fKLength( sqrtf( sqrf( GetKx( i ) ) + sqrf( GetKy( j ) ) ) );
            m_colAngularFreq[ Index( i, j ) ] = GetAngularFrequency( fKLength );
        }
    }
}



void 
COceanWaveGenerator::FFT( int iDir, float* real, float* imag )
{
    // fft code appears courtesy of Paul Bourke

    long nn,i,i1,j,k,i2,l,l1,l2;
    float c1,c2,treal,timag,t1,t2,u1,u2,z;
    
    nn = GRIDSIZE;
    
    // Do the bit reversal
    i2 = nn >> 1;
    j = 0;
    for( i = 0; i < nn - 1; ++i )
    {
        if( i < j )
        {
            treal = real[ i ];
            timag = imag[ i ];
            real[ i ] = real[ j ];
            imag[ i ] = imag[ j ];
            real[ j ] = treal;
            imag[ j ] = timag;
        }

        k = i2;
        while( k <= j )
        {
            j -= k;
            k >>= 1;
        }

        j += k;
    }
    
    // Compute the FFT
    c1 = -1.0f;
    c2 = 0.0f;
    l2 = 1;
    for( l = 0; l < LOG_GRIDSIZE; ++l )
    {
        l1 = l2;
        l2 <<= 1;
        u1 = 1.0;
        u2 = 0.0;
        for( j = 0; j < l1; ++j )
        {
            for( i = j; i < nn; i += l2 )
            {
                i1 = i + l1;
                t1 = u1 * real[ i1 ] - u2 * imag[ i1 ];
                t2 = u1 * imag[ i1 ] + u2 * real[ i1 ];
                real[ i1 ] = real[ i ] - t1;
                imag[ i1 ] = imag[ i ] - t2;
                real[ i ] += t1;
                imag[ i ] += t2;
            }

            z =  u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }

        c2 = sqrtf( ( 1.0f - c1 ) / 2.0f );
        
        if( 1 == iDir )
        {
            c2 = -c2;
        }

        c1 = sqrtf( ( 1.0f + c1 ) / 2.0f );
    }
    
    // Scaling for forward transform
    if( 1 == iDir )
    {
        for( i = 0; i < nn; ++i )
        {
            real[ i ] /= (float) nn;
            imag[ i ] /= (float) nn;
        }
    }
}



void 
COceanWaveGenerator::FFT2D( int iDir, vector< Complex >& cmp )
{
    float real[ GRIDSIZE ];
    float imag[ GRIDSIZE ];

    // Transform the rows
    for( int j( 0 ); j < GRIDSIZE; ++j )
    {
        for( int i( 0 ); i < GRIDSIZE; ++i )
        {
            real[ i ] = cmp[ Offset( i, j ) ].real();
            imag[ i ] = cmp[ Offset( i, j ) ].imag();
        }
        
        FFT( iDir, real, imag );
        
        for( int i( 0 ); i < GRIDSIZE; ++i )
        {
            cmp[ Offset( i, j ) ].real( real[ i ] );
            cmp[ Offset( i, j ) ].imag( imag[ i ] );
        }
    }
    
    // Transform the columns
    for( int i( 0 ); i < GRIDSIZE; ++i )
    {
        for( int j( 0 ); j < GRIDSIZE; ++j )
        {
            real[ j ] = cmp[ Offset( i, j ) ].real();
            imag[ j ] = cmp[ Offset( i, j ) ].imag();
        }
        
        FFT( iDir, real, imag );
        
        for( int j( 0 ); j < GRIDSIZE; ++j )
        {
            cmp[ Offset( i, j ) ].real( real[ j ] );
            cmp[ Offset( i, j ) ].imag( imag[ j ] );
        }
    }
}



void 
COceanWaveGenerator::UpdateOceanMap( float fTime )
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // initial version
    //
    //for( int j( -GRIDSIZE / 2 ); j < GRIDSIZE / 2; ++j )
    //{
    //    for( int i( -GRIDSIZE / 2 ); i < GRIDSIZE / 2; ++i )
    //    {
    //        float   fKx( GetKx( i ) ),
    //                fKy( GetKy( j ) );
    //
    //        float   fKLength( sqrtf( sqrf( fKx ) + sqrf( fKy ) ) ),
    //                fAngularFreq( GetAngularFrequency( fKLength ) );
    //
    //        m_colH[ IndexFFT( i, j ) ] =       h0[ IndexH0(  i,  j ) ]   * exp( complex< float >( 0.0f,  fAngularFreq * fTime ) ) + 
    //                                     conj( h0[ IndexH0( -i, -j ) ] ) * exp( complex< float >( 0.0f, -fAngularFreq * fTime ) );
    //    }
    //}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // optimizations
    //
    // 1. get rid of calling complex< float >::exp() twice
    //      exp(  i * w ) = ( cosf( w ),  sinf( w ) ) and
    //      exp( -i * w ) = ( cosf( w ), -sinf( w ) )
    //
    // 2. get rid of...
    //      complex< float >::operator *() 
    //          ( a + ib ) * ( c + id ) = ( ac + bd ) + i( ad + bc )
    //      ... and complex< float >::operator +()
    //          ( a + ib ) + ( c + id ) = ( a + c ) + i( c + d )
    //
    //    by doing the two multiplications and one addition explicitly we also get rid of
    //    constructing two temporary objects and calling one assignment operator
    //
    // 3. precalculate angular frequencies and inverse length of all K

    for( int j( -GRIDSIZE / 2 ); j < GRIDSIZE / 2; ++j )
    {
        for( int i( -GRIDSIZE / 2 ); i < GRIDSIZE / 2; ++i )
        {
            float fAngularFreq( m_colAngularFreq[ Index( i, j ) ] * fTime );
            float fCos( cosf( fAngularFreq ) );
            float fSin( sinf( fAngularFreq ) );

            // update height
            m_colH[ IndexFFT( i, j ) ].real( ( m_colH0[ IndexH0( i, j ) ].real() + m_colH0[ IndexH0( -i, -j ) ].real() ) * fCos -
                                             ( m_colH0[ IndexH0( i, j ) ].imag() + m_colH0[ IndexH0( -i, -j ) ].imag() ) * fSin );

            m_colH[ IndexFFT( i, j ) ].imag( ( m_colH0[ IndexH0( i, j ) ].real() - m_colH0[ IndexH0( -i, -j ) ].real() ) * fSin + 
                                             ( m_colH0[ IndexH0( i, j ) ].imag() - m_colH0[ IndexH0( -i, -j ) ].imag() ) * fCos );

            // update normal
            float fKx( GetKx( i ) );
            float fKy( GetKy( j ) );

            // Put the two functions for n(Kx) and n(Ky) into one complex array
            // (that is, multiply n(Ky) by i and add it to n(Kx) ) to do one FFT instead of two.
            // This assumes that the inverse of both transforms are real functions (as it is the case here)!
            // After the FFT the x component of the normal will be stored in the real part, the y component 
            // in the complex part of m_colN respectively.
            m_colN[ IndexFFT( i, j ) ].real( -m_colH[ IndexFFT( i, j ) ].imag() * fKx - m_colH[ IndexFFT( i, j ) ].real() * fKy );
            m_colN[ IndexFFT( i, j ) ].imag(  m_colH[ IndexFFT( i, j ) ].real() * fKx - m_colH[ IndexFFT( i, j ) ].imag() * fKy );

            // update displacement vector for choppy waves 
            fKx *= m_colKScale[ Index( i, j ) ];
            fKy *= m_colKScale[ Index( i, j ) ];

            // Use the same trick described above for d(Kx) and d(Ky) to do one FFT instead of two.
            m_colD[ IndexFFT( i, j ) ].real(  m_colH[ IndexFFT( i, j ) ].imag() * fKx + m_colH[ IndexFFT( i, j ) ].real() * fKy );
            m_colD[ IndexFFT( i, j ) ].imag( -m_colH[ IndexFFT( i, j ) ].real() * fKx + m_colH[ IndexFFT( i, j ) ].imag() * fKy );
        }
    }
    
    FFT2D( -1, m_colH );    
    FFT2D( -1, m_colN );    
    FFT2D( -1, m_colD );    
}



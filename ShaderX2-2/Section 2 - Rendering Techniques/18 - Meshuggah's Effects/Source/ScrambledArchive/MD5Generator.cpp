///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e ScrambledArchive::CMD5Generator.
 * 
 * @file
 *  MD5Generator.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Implementation of \e ScrambledArchive::CMD5Generator.
 */



#include "stdafx.h"

#include "MD5Generator.h"
#include "MD5Constants.h"
#include <fstream>
#include "stdio.h"



using namespace std;
using namespace ScrambledArchive;



CMD5Generator::CMD5Generator()
{
    memset( m_pucInputBuffer, 0, sizeof( m_pucInputBuffer ) );
    memset( m_puiCount, 0, 2 * sizeof( m_puiCount ) );

    m_puiMD5[ 0 ] = MD5_INIT_STATE_0;
    m_puiMD5[ 1 ] = MD5_INIT_STATE_1;
    m_puiMD5[ 2 ] = MD5_INIT_STATE_2;
    m_puiMD5[ 3 ] = MD5_INIT_STATE_3;
}



CMD5Generator::~CMD5Generator()
{
}



bool
CMD5Generator::GetMD5( SMD5& sMD5, const unsigned char* const pucData, unsigned int uiLength )
{
    if( 0 == pucData && uiLength > 0 )
    {
        return( false );
    }

    CMD5Generator kMD5;
    kMD5.Update( pucData, uiLength );
    kMD5.Final( sMD5.m_pucSignature );

    return( true );
}



bool
CMD5Generator::GetMD5( SMD5& sMD5, const std::string& strFilePath )
{
    ifstream kInStream( strFilePath.c_str(), ios::in | ios::binary );
    if( false == kInStream.good() )
    {
        return( false );
    }

    CMD5Generator kMD5;       
    while( kInStream.good() )
    {
        unsigned char pucBuf[ 1024 ];
        kInStream.read( (char *) &pucBuf[ 0 ], sizeof( pucBuf ) );

        kMD5.Update( pucBuf, kInStream.gcount() );
    }

    kInStream.close();
    kMD5.Final( sMD5.m_pucSignature );

    return( true );
}



inline unsigned int 
CMD5Generator::RotateLeft( unsigned int uiX, int iN )
{
    return( ( uiX << iN ) | ( uiX >> ( 32 - iN ) ) );
}



inline void 
CMD5Generator::FF( unsigned int& uiA, unsigned int uiB, unsigned int uiC, unsigned int uiD, 
                   unsigned int uiX, unsigned int uiS, unsigned int uiT )
{
    unsigned int uiF( ( uiB & uiC ) | ( ~uiB & uiD ) );
    uiA += uiF + uiX + uiT;
    uiA  = RotateLeft( uiA, uiS );
    uiA += uiB;
}



inline void 
CMD5Generator::GG( unsigned int& uiA, unsigned int uiB, unsigned int uiC, unsigned int uiD, 
                   unsigned int uiX, unsigned int uiS, unsigned int uiT )
{
    unsigned int uiG( ( uiB & uiD ) | ( uiC & ~uiD ) );
    uiA += uiG + uiX + uiT;
    uiA  = RotateLeft( uiA, uiS );
    uiA += uiB;
}



inline void 
CMD5Generator::HH( unsigned int& uiA, unsigned int uiB, unsigned int uiC, unsigned int uiD, 
                   unsigned int uiX, unsigned int uiS, unsigned int uiT )
{
    unsigned int uiH( ( uiB ^ uiC ^ uiD ) );
    uiA += uiH + uiX + uiT;
    uiA  = RotateLeft( uiA, uiS );
    uiA += uiB;
}



inline void 
CMD5Generator::II( unsigned int& uiA, unsigned int uiB, unsigned int uiC, unsigned int uiD, 
                   unsigned int uiX, unsigned int uiS, unsigned int uiT )
{
    unsigned int uiI( ( uiC ^ ( uiB | ~uiD ) ) );
    uiA += uiI + uiX + uiT;
    uiA  = RotateLeft( uiA, uiS );
    uiA += uiB;
}



void 
CMD5Generator::Decode( unsigned int* puiOutput, const unsigned char* const pucInput, unsigned int uiLength )
{
    for( unsigned int i( 0 ), j( 0 ); j < uiLength; ++i, j += 4 ) 
    {
        puiOutput[ i ] =   ( (unsigned int) pucInput[ j     ] )         | 
                         ( ( (unsigned int) pucInput[ j + 1 ] ) <<  8 ) |
                         ( ( (unsigned int) pucInput[ j + 2 ] ) << 16 ) |
                         ( ( (unsigned int) pucInput[ j + 3 ] ) << 24 ); 
    }
}



void 
CMD5Generator::Encode( unsigned char* pucOutput, const unsigned int* const puiInput, unsigned int uiLength )
{
    for( unsigned int i( 0 ), j( 0 ); j < uiLength; ++i, j += 4 ) 
    {
        pucOutput[ j     ] = (unsigned char)  ( puiInput[i]        & 0xFF );
        pucOutput[ j + 1 ] = (unsigned char)( ( puiInput[i] >>  8) & 0xFF );
        pucOutput[ j + 2 ] = (unsigned char)( ( puiInput[i] >> 16) & 0xFF );
        pucOutput[ j + 3 ] = (unsigned char)( ( puiInput[i] >> 24) & 0xFF );
    }
}



void 
CMD5Generator::Transform( const unsigned char* const pucBlock )
{
    unsigned int a( m_puiMD5[ 0 ] );
    unsigned int b( m_puiMD5[ 1 ] );
    unsigned int c( m_puiMD5[ 2 ] );
    unsigned int d( m_puiMD5[ 3 ] );

    //unsigned int X[ 16 ];
    //Decode( X, pucBlock, 64 );

    // on little endian machines we can simply do a cast instead of Decode()
    const unsigned int* const X( (const unsigned int* const) &pucBlock[ 0 ] );

    FF( a, b, c, d, X[  0 ], MD5_S11, MD5_T01 ); 
    FF( d, a, b, c, X[  1 ], MD5_S12, MD5_T02 ); 
    FF( c, d, a, b, X[  2 ], MD5_S13, MD5_T03 ); 
    FF( b, c, d, a, X[  3 ], MD5_S14, MD5_T04 ); 
    FF( a, b, c, d, X[  4 ], MD5_S11, MD5_T05 ); 
    FF( d, a, b, c, X[  5 ], MD5_S12, MD5_T06 ); 
    FF( c, d, a, b, X[  6 ], MD5_S13, MD5_T07 ); 
    FF( b, c, d, a, X[  7 ], MD5_S14, MD5_T08 ); 
    FF( a, b, c, d, X[  8 ], MD5_S11, MD5_T09 ); 
    FF( d, a, b, c, X[  9 ], MD5_S12, MD5_T10 ); 
    FF( c, d, a, b, X[ 10 ], MD5_S13, MD5_T11 ); 
    FF( b, c, d, a, X[ 11 ], MD5_S14, MD5_T12 ); 
    FF( a, b, c, d, X[ 12 ], MD5_S11, MD5_T13 ); 
    FF( d, a, b, c, X[ 13 ], MD5_S12, MD5_T14 ); 
    FF( c, d, a, b, X[ 14 ], MD5_S13, MD5_T15 ); 
    FF( b, c, d, a, X[ 15 ], MD5_S14, MD5_T16 ); 

    GG( a, b, c, d, X[  1 ], MD5_S21, MD5_T17 ); 
    GG( d, a, b, c, X[  6 ], MD5_S22, MD5_T18 ); 
    GG( c, d, a, b, X[ 11 ], MD5_S23, MD5_T19 ); 
    GG( b, c, d, a, X[  0 ], MD5_S24, MD5_T20 ); 
    GG( a, b, c, d, X[  5 ], MD5_S21, MD5_T21 ); 
    GG( d, a, b, c, X[ 10 ], MD5_S22, MD5_T22 ); 
    GG( c, d, a, b, X[ 15 ], MD5_S23, MD5_T23 ); 
    GG( b, c, d, a, X[  4 ], MD5_S24, MD5_T24 ); 
    GG( a, b, c, d, X[  9 ], MD5_S21, MD5_T25 ); 
    GG( d, a, b, c, X[ 14 ], MD5_S22, MD5_T26 ); 
    GG( c, d, a, b, X[  3 ], MD5_S23, MD5_T27 ); 
    GG( b, c, d, a, X[  8 ], MD5_S24, MD5_T28 ); 
    GG( a, b, c, d, X[ 13 ], MD5_S21, MD5_T29 ); 
    GG( d, a, b, c, X[  2 ], MD5_S22, MD5_T30 ); 
    GG( c, d, a, b, X[  7 ], MD5_S23, MD5_T31 ); 
    GG( b, c, d, a, X[ 12 ], MD5_S24, MD5_T32 ); 

    HH( a, b, c, d, X[  5 ], MD5_S31, MD5_T33 ); 
    HH( d, a, b, c, X[  8 ], MD5_S32, MD5_T34 ); 
    HH( c, d, a, b, X[ 11 ], MD5_S33, MD5_T35 ); 
    HH( b, c, d, a, X[ 14 ], MD5_S34, MD5_T36 ); 
    HH( a, b, c, d, X[  1 ], MD5_S31, MD5_T37 ); 
    HH( d, a, b, c, X[  4 ], MD5_S32, MD5_T38 ); 
    HH( c, d, a, b, X[  7 ], MD5_S33, MD5_T39 ); 
    HH( b, c, d, a, X[ 10 ], MD5_S34, MD5_T40 ); 
    HH( a, b, c, d, X[ 13 ], MD5_S31, MD5_T41 ); 
    HH( d, a, b, c, X[  0 ], MD5_S32, MD5_T42 ); 
    HH( c, d, a, b, X[  3 ], MD5_S33, MD5_T43 ); 
    HH( b, c, d, a, X[  6 ], MD5_S34, MD5_T44 ); 
    HH( a, b, c, d, X[  9 ], MD5_S31, MD5_T45 ); 
    HH( d, a, b, c, X[ 12 ], MD5_S32, MD5_T46 ); 
    HH( c, d, a, b, X[ 15 ], MD5_S33, MD5_T47 ); 
    HH( b, c, d, a, X[  2 ], MD5_S34, MD5_T48 ); 

    II( a, b, c, d, X[  0 ], MD5_S41, MD5_T49 ); 
    II( d, a, b, c, X[  7 ], MD5_S42, MD5_T50 ); 
    II( c, d, a, b, X[ 14 ], MD5_S43, MD5_T51 ); 
    II( b, c, d, a, X[  5 ], MD5_S44, MD5_T52 ); 
    II( a, b, c, d, X[ 12 ], MD5_S41, MD5_T53 ); 
    II( d, a, b, c, X[  3 ], MD5_S42, MD5_T54 ); 
    II( c, d, a, b, X[ 10 ], MD5_S43, MD5_T55 ); 
    II( b, c, d, a, X[  1 ], MD5_S44, MD5_T56 ); 
    II( a, b, c, d, X[  8 ], MD5_S41, MD5_T57 ); 
    II( d, a, b, c, X[ 15 ], MD5_S42, MD5_T58 ); 
    II( c, d, a, b, X[  6 ], MD5_S43, MD5_T59 ); 
    II( b, c, d, a, X[ 13 ], MD5_S44, MD5_T60 ); 
    II( a, b, c, d, X[  4 ], MD5_S41, MD5_T61 ); 
    II( d, a, b, c, X[ 11 ], MD5_S42, MD5_T62 ); 
    II( c, d, a, b, X[  2 ], MD5_S43, MD5_T63 ); 
    II( b, c, d, a, X[  9 ], MD5_S44, MD5_T64 ); 

    m_puiMD5[ 0 ] += a;
    m_puiMD5[ 1 ] += b;
    m_puiMD5[ 2 ] += c;
    m_puiMD5[ 3 ] += d;
}



void 
CMD5Generator::Update( const unsigned char* const pucInput, unsigned int uiInputLen )
{
    unsigned int uiIndex( (unsigned int) ( ( m_puiCount[ 0 ] >> 3) & 0x3F ) );

    if( ( m_puiCount[ 0 ] += uiInputLen << 3 )  <  ( uiInputLen << 3 ) )
    {
        ++m_puiCount[ 1 ];
    }

    m_puiCount[ 1 ] += ( uiInputLen >> 29 );

    unsigned int i( 0 );

    unsigned int uiPartLen( 64 - uiIndex );
    if( uiInputLen >= uiPartLen )   
    {
        memcpy( &m_pucInputBuffer[ uiIndex ], pucInput, uiPartLen );        
        Transform( m_pucInputBuffer );

        for( i = uiPartLen; i + 63 < uiInputLen; i += 64 ) 
        {
            Transform( &pucInput[ i ] );
        }

        uiIndex = 0;
    } 
    else 
    {
        i = 0;
    }

    memcpy( &m_pucInputBuffer[ uiIndex ], &pucInput[ i ], uiInputLen - i );
}



void 
CMD5Generator::Final( unsigned char* pucMD5 )
{
    unsigned char pucBits[ 8 ];
    Encode( pucBits, m_puiCount, 8 );

    unsigned int uiIndex( (unsigned int) ( (m_puiCount[ 0 ] >> 3 ) & 0x3F ) );
    unsigned int uiPadLen( ( uiIndex < 56 ) ? ( 56 - uiIndex ) : ( 120 - uiIndex ) );

    static unsigned char s_pucPadding[ 64 ] =
    {
      0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
      0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
      0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    Update( s_pucPadding, uiPadLen );
    Update( pucBits, 8 );
    Encode( pucMD5, m_puiMD5, SMD5::SIGNATURESIZE );
}

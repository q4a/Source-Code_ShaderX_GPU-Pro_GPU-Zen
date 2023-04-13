///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e ScrambledArchive::CScrambleCodec.
 * 
 * @file
 *  ScrambleCodec.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Implementation of \e ScrambledArchive::CScrambleCodec.
 */



#include "stdafx.h"

#include "ScrambleCodec.h"
#include "io.h"
#include "stdio.h"



using namespace std;
using namespace ScrambledArchive;



CScrambleCodec::CScrambleCodec()
{
}



CScrambleCodec::~CScrambleCodec()
{
}



bool
CScrambleCodec::CreateScrambledArchive( const string strInFile, 
                                        const string strOutFile, const CMD5Generator::SMD5& sMD5 )
{
    FILE* inFile  = fopen( strInFile.c_str(), "rb" );
    FILE* outFile = fopen( strOutFile.c_str(), "wb" );
    if( 0 == inFile || 0 == outFile )
    {
        if( 0 != inFile )
        {
            DBG_ASSERTM( 0, "CScrambleCodec::CreateScrambledArchive() : Failed to open input file!" );
            fclose( inFile );
        }

        if( 0 != outFile )
        {
            DBG_ASSERTM( 0, "CScrambleCodec::CreateScrambledArchive() : Failed to open output file!" );
            fclose( outFile );
        }

        return( false );
    }

    // create and write header
    SCryptoHeader sHeader( _filelength( fileno( inFile ) ), sMD5 );
    fwrite( &sHeader, 1, sizeof( SCryptoHeader ), outFile );

    // create and initialize cipher text blocks
    unsigned char pucSrc[ TEA_DATASIZE ];        
    unsigned char pucDst[ TEA_DATASIZE ];
    
    memset( pucDst, 0, TEA_DATASIZE );
    
    // scramble all bytes, add padding bytes if necessary
    unsigned int uiBytesScrambled( 0 );
    while( uiBytesScrambled < sHeader.m_uiOrigDataSize )
    {
        // read TEA_DATASIZE bytes from source file
        if( uiBytesScrambled + TEA_DATASIZE <= sHeader.m_uiOrigDataSize )
        {
            fread( pucSrc, 1, TEA_DATASIZE, inFile );
        }
        else
        {
            // we're at the end of the file and there's not enough data 
            // for the last encipher block so add padding bytes
            unsigned int uiBytesLeft( sHeader.m_uiOrigDataSize - uiBytesScrambled );
            fread( pucSrc, 1, uiBytesLeft, inFile );

            for( unsigned int uiI( uiBytesLeft ); uiI < TEA_DATASIZE; ++uiI )
            {
                pucSrc[ uiI ] = uiI - uiBytesLeft;
            }
        }
   
        // combine the current plain text block with the result of the last enciphered text block
        for( unsigned int uiI( 0 ); uiI < TEA_DATASIZE; ++uiI )
        {
            pucSrc[ uiI ] ^= pucDst[ uiI ];
        }

        Encipher( 
                    (unsigned int *) &pucSrc[ 0 ], 
                    (unsigned int *) &pucDst[ 0 ], 
                    (unsigned int *) &sHeader.m_sMD5.m_pucSignature[ 0 ] 
                );

        fwrite( pucDst, 1, TEA_DATASIZE, outFile );        
        uiBytesScrambled += TEA_DATASIZE;
    }

    fclose( inFile );
    fclose( outFile );

    return( true );
}



bool
CScrambleCodec::DecipherArchive( unsigned char* pucData, unsigned int uiDataSize,
                                 unsigned char*& pucDecipheredArchive, unsigned int& uiDecipheredArchiveSize )
{
    SCryptoHeader& sHeader( *( SCryptoHeader *) pucData );

    // check crypto header tag
    if( SCryptoHeader::CHT_CRYPTOTAG != sHeader.m_uiTag )
    {
        DBG_ASSERTM( 0, "CScrambleCodec::DecipherArchive() : Cannot decipher the given file. Tag in crypto header doesn't match!" );
        return( false );
    }

    // set return values
    pucDecipheredArchive = pucData + sizeof( SCryptoHeader );    
    uiDecipheredArchiveSize = sHeader.m_uiOrigDataSize;

    // bail out if there's nothing to decipher
    if( 0 == uiDecipheredArchiveSize )
    {
        return( true );
    }

    // retrieve the total amount of scrambled bytes
    unsigned int uiNumBytesToDecipher( uiDataSize - sizeof( SCryptoHeader ) );

    // bail out if uiNumBytesToDecipher is not a multiple of TEA_DATASIZE
    // or uiNumBytesToDecipher doesn't match with uiDecipheredArchiveLength
    if( 0 != ( uiNumBytesToDecipher & ( TEA_DATASIZE - 1 ) ) ||
        uiNumBytesToDecipher < uiDecipheredArchiveSize || 
        TEA_DATASIZE <= ( uiNumBytesToDecipher - uiDecipheredArchiveSize ) )
    {
        DBG_ASSERTM( 0, "CScrambleCodec::DecipherArchive() : Encrypted archive seems to be corrupt!" );
        return( false );
    }

    // create and initialize cipher text blocks
    unsigned char pucSrc[ TEA_DATASIZE ];
    unsigned char pucDst[ TEA_DATASIZE ];

    memset( pucDst, 0, TEA_DATASIZE );

    // set ptr of current cipher block
    unsigned char* pucCurrentBlock( pucDecipheredArchive );

    // decipher all blocks
    unsigned int uiBytesDeciphered( 0 );
    while( uiBytesDeciphered < uiNumBytesToDecipher )
    {
        memcpy( pucSrc, pucCurrentBlock, TEA_DATASIZE );

        Decipher( 
                    (unsigned int *) &pucSrc[ 0 ], 
                    (unsigned int *) &pucCurrentBlock[ 0 ], 
                    (unsigned int *) &sHeader.m_sMD5.m_pucSignature[ 0 ] 
                );
            
        for( unsigned int uiI( 0 ); uiI < TEA_DATASIZE; ++uiI )
        {
            pucCurrentBlock[ uiI ] ^= pucDst[ uiI ];
            pucDst[ uiI ] = pucSrc[ uiI ];
        }
        
        // advance ptr of current block to the next one
        pucCurrentBlock += TEA_DATASIZE;
        uiBytesDeciphered += TEA_DATASIZE;
    }

    return( true );
}



inline void 
CScrambleCodec::Encipher( const unsigned int* const puiV, 
                          unsigned int* const puiW, 
                          const unsigned int* const puiK )
{
   unsigned int uiY( puiV[ 0 ] ),
                uiZ( puiV[ 1 ] ),

                uiSum( 0 ), 
                uiN( N );

   while( uiN-- > 0 )
   {
        uiY   += ( uiZ << 4 ^ uiZ >> 5 ) + uiZ ^ uiSum + puiK[ uiSum & 3 ];
        uiSum += DELTA;
        uiZ   += ( uiY << 4 ^ uiY >> 5 ) + uiY ^ uiSum + puiK[ uiSum >> 11 & 3 ];
   }
   
   puiW[ 0 ] = uiY; 
   puiW[ 1 ] = uiZ;
}



inline void 
CScrambleCodec::Decipher( const unsigned int* const puiV, 
                          unsigned int* const puiW, 
                          const unsigned int* const puiK )
{
   unsigned int uiY( puiV[ 0 ] ),
                uiZ( puiV[ 1 ] ),

                uiSum( SUM ),   
                uiN( N );

   while( uiN-- > 0 )
   {
        uiZ   -= ( uiY << 4 ^ uiY >> 5 ) + uiY ^ uiSum + puiK[ uiSum >> 11 & 3 ];
        uiSum -= DELTA;
        uiY   -= ( uiZ << 4 ^ uiZ >> 5 ) + uiZ ^ uiSum + puiK[ uiSum & 3 ];
   }
   
   puiW[ 0 ] = uiY; 
   puiW[ 1 ] = uiZ;
}

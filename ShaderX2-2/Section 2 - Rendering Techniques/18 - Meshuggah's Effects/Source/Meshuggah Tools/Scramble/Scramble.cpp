#include "stdafx.h"
#include "windows.h"
#include "..\..\ScrambledArchive\ScrambleCodec.h"
#include "stdio.h"



using namespace ScrambledArchive;



int main( int argc, char* argv[] )
{
    if( argc < 3 )
    {
        printf( "Not enough parameters specified!\n" );
        printf( "syntax: Scramble inFile outFile\n" );
        return -1;
    }


    CMD5Generator::SMD5 sMD5;
    if( false == CMD5Generator::GetMD5( sMD5, argv[ 1 ] ) )
    {
        printf( "Couldn't generate MD5 signature!\n" );
        return -1;
    }


    if( false == CScrambleCodec::CreateScrambledArchive( argv[ 1 ], argv[ 2 ], sMD5 ) )
    {
        printf( "Couldn't scramble %s!\n", argv[ 1 ] );
        return -1;
    }


    printf( "%s successfully scrambled to %s.\nMD5 = 0x", argv[ 1 ], argv[ 2 ] );
    for( unsigned int i( 0 ); i < CMD5Generator::SMD5::SIGNATURESIZE; ++i )
    {
        printf( "%.2x", sMD5.m_pucSignature[ i ] );
    }
    printf( "\n" );


    return 0;
}

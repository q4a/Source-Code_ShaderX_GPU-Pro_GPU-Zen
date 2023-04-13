#include "stdafx.h"

#include "windows.h"
#include "stdio.h"
#include "io.h"


int main( int argc, char* argv[] )
{
    if( argc < 4 )
    {
        printf( "Not enough parameters specified!\n" );
        printf( "syntax: Patch exeFile dataFile resName\n" );
        return -1;
    }


    FILE* f( fopen( argv[ 2 ], "rb" ) );
    if( 0 == f )
    {
        printf( "Couldn't open %s!\n", argv[ 2 ] );
        return -1;
    }

    
    unsigned int uiFileSize( _filelength( fileno( f ) ) );
    unsigned char* pucData( new unsigned char[ uiFileSize ] );
    if( 0 == pucData )
    {
        printf( "Not enough memory to load %s!\n", argv[ 2 ] );
        fclose( f );
        return -1;
    }
    
    fread( pucData, 1, uiFileSize, f );
    fclose( f );

    
    HANDLE hExe( BeginUpdateResource( argv[ 1 ], TRUE ) );
    if( 0 == hExe )
    {
        printf( "Couldn't open %s for patching!\n", argv[ 1 ] );
        delete [] pucData;
        return -1;
    }


    if( FALSE != UpdateResource( hExe, RT_RCDATA, argv[ 3 ], 
                                 MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ), pucData, uiFileSize ) )
    {
        if( FALSE != EndUpdateResource( hExe, FALSE ) )
        {
            printf( "%s successfully patched!\n", argv[ 1 ] );
            printf( "%s was inserted as %s.\n", argv[ 2 ], argv[ 3 ] );

            delete [] pucData;
            return 0;
        }
    }
    

    printf( "Resource update was unsuccessful!\n" );
    

    delete [] pucData;
    return -1;
}

///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CTextureManager.
 *
 * @file
 *  TextureManager.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  28.02.2002
 * @brief
 *  Implementation of \e Shared::CTextureManager.
 */



#include "StdAfx.h"
#include "Debug.h"
#include "TextureManager.h"
#include "ScrambledArchive.h"
#include "TMException.inl"
#include <vector>
#include <algorithm>



extern "C"
{
#   include "stdio.h"
#   include "jpeglib.h"
}



using namespace Shared;
using namespace std;
using namespace ScrambledArchive;



const D3DCUBEMAP_FACES c_cubeFaces[] =
{
    // dds compatible order
    D3DCUBEMAP_FACE_POSITIVE_X,             // right
    D3DCUBEMAP_FACE_NEGATIVE_X,             // left
    D3DCUBEMAP_FACE_POSITIVE_Y,             // top
    D3DCUBEMAP_FACE_NEGATIVE_Y,             // bottom
    D3DCUBEMAP_FACE_POSITIVE_Z,             // front
    D3DCUBEMAP_FACE_NEGATIVE_Z,             // back
};



METHODDEF( void ) JPEGErrorExit( j_common_ptr cinfo )
{
    char cBuffer[ JMSG_LENGTH_MAX ];
    (*cinfo->err->format_message) ( cinfo, cBuffer );
    throw CTMException( cBuffer );
}



inline void
JPEGCleanUp( jpeg_decompress_struct& cinfo )
{
    try
    {
        jpeg_finish_decompress( &cinfo );
    }
    catch( ... )
    {
    }

    try
    {
        jpeg_destroy_decompress( &cinfo );
    }
    catch( ... )
    {
    }
}



CTextureManager::CTextureManager( IDirect3DDevice8* pD3DDev, const CScrambledArchive* pkArchive )
: m_pkArchive( pkArchive )
, m_pD3DDev( pD3DDev )
, m_colTextures()
{
    if( 0 != m_pD3DDev )
    {
        m_pD3DDev->AddRef();
    }
}



CTextureManager::~CTextureManager()
{
    if( 0 != m_pD3DDev )
    {
        m_pD3DDev->Release();
    }

    for_each( m_colTextures.begin(), m_colTextures.end(), ReleaseTextures() );
}



bool 
CTextureManager::LoadDDSTexture( const std::string& strFileName, bool bMipMaps )
{
    if( false != IsTextureRegistered( strFileName ) )
    {
        return( true );
    }

    vector< unsigned char > colDDSData;
    if( false == m_pkArchive->DecompressFile( strFileName, colDDSData ) )
    {
        return( false );
    }

    // load texture
    IDirect3DTexture8* pkTexture( 0 );
    if( FAILED( D3DXCreateTextureFromFileInMemoryEx( 
                        m_pD3DDev,                                // d3d device
                        &colDDSData[ 0 ],                         // src data
                        (UINT) colDDSData.size(),                 // data length
                        D3DX_DEFAULT,                             // width
                        D3DX_DEFAULT,                             // height
                        ( false != bMipMaps ) ? D3DX_DEFAULT : 1, // mipmaps
                        0,                                        // usage
                        D3DFMT_UNKNOWN,                           // format
                        D3DPOOL_MANAGED,                          // pool
                        D3DX_DEFAULT,                             // filter
                        D3DX_DEFAULT,                             // mip filter
                        0,                                        // color key
                        0,                                        // src info ptr
                        0,                                        // palette ptr
                        &pkTexture                                // texture
                        ) ) )
    {
        return( false );        
    }

    RegisterTexture( strFileName, pkTexture );

    return( true );
}



bool
CTextureManager::LoadJPEGTexture( const string& strFileName, 
                                  bool bAlpha, bool bMipMaps, bool bCompressed )
{
    if( false != IsTextureRegistered( strFileName ) )
    {
        return( true );
    }

    vector< unsigned char > colJPEGData;
    if( false == m_pkArchive->DecompressFile( strFileName, colJPEGData ) )
    {
        return( false );
    }

    IDirect3DTexture8*      pkTexture( 0 );
    bool                    bSuccess( false );
    jpeg_decompress_struct  cinfo;

    try
    {
        // setup jpeg error routines and overwrite error_exit with our own
        jpeg_error_mgr jerr;
        cinfo.err       = jpeg_std_error( &jerr );
        jerr.error_exit = JPEGErrorExit;

        // prepare decompression
        jpeg_create_decompress( &cinfo );
        jpeg_stdio_src( &cinfo, 0 );

        // load jpeg data from memory
        cinfo.src->bytes_in_buffer = colJPEGData.size();
        cinfo.src->next_input_byte = (JOCTET *) &colJPEGData[ 0 ];

        // read header
        jpeg_read_header( &cinfo, TRUE );

        // set to high quality decompression
        cinfo.dct_method          = JDCT_FLOAT;
        cinfo.do_fancy_upsampling = TRUE;
        cinfo.do_block_smoothing  = TRUE;

        // start decompression
        jpeg_start_decompress( &cinfo );

        // allocate memory for one scanline
        JSAMPARRAY jpegBuf( (*cinfo.mem->alloc_sarray) ( (j_common_ptr) &cinfo, JPOOL_IMAGE,
            cinfo.output_width * cinfo.out_color_components, 1 ) );

        // get texture dimension
        unsigned int uiWidth( cinfo.output_width );
        unsigned int uiHeight( cinfo.output_height );

        if( false != bAlpha )
        {
            // height must be multiple of two and greater than zero
            if( 1 == ( uiHeight & 1 ) || 0 == uiHeight )
            {
                throw( CTMException( "Unexpected texture format!" ) );
            }

            // get real height of texture
            uiHeight >>= 1;
        }

        // create temporary decompression buffer (32bit ARGB)
        vector< unsigned int > colRawTexture( uiWidth * uiHeight );

        // texture is rgb
        if( 3 == cinfo.out_color_components )
        {
            unsigned int uiAlphaValue( ( false != bAlpha ) ? 0x0 : 0xFF000000 );
            while( cinfo.output_scanline < uiHeight )
            {
                unsigned int uiAddrRGB( cinfo.output_scanline * uiWidth );
                unsigned int uiPixelOffset( 0 );

                unsigned char* pucScanline( jpegBuf[ 0 ] );
                jpeg_read_scanlines( &cinfo, jpegBuf, 1 );

                for( unsigned int i( 0 ); i < uiWidth; ++i, uiPixelOffset += 3 )
                {
                    colRawTexture[ uiAddrRGB + i ] =
                        ( *( pucScanline + uiPixelOffset + 0 ) << 16 ) +
                        ( *( pucScanline + uiPixelOffset + 1 ) <<  8 ) +
                        ( *( pucScanline + uiPixelOffset + 2 )       ) + uiAlphaValue;
                }
            }

            if( false != bAlpha )
            {
                // read alpha part of texture
                while( cinfo.output_scanline < uiHeight << 1 )
                {
                    unsigned int uiAddrRGB( ( cinfo.output_scanline - uiHeight ) * uiWidth );
                    unsigned int uiPixelOffset( 0 );

                    unsigned char* pucScanline( jpegBuf[ 0 ] );
                    jpeg_read_scanlines( &cinfo, jpegBuf, 1 );

                    for( unsigned int i( 0 ); i < uiWidth; ++i, uiPixelOffset += 3 )
                    {
                        colRawTexture[ uiAddrRGB + i ] |=
                            ( ( *( pucScanline + uiPixelOffset + 0 ) +
                                *( pucScanline + uiPixelOffset + 1 ) +
                                *( pucScanline + uiPixelOffset + 2 ) ) / 3 ) << 24;
                    }
                }
            }
        }
        // texture is gray scale
        else
        {
            unsigned int uiAlphaValue( ( false != bAlpha ) ? 0x0 : 0xFF000000 );
            while( cinfo.output_scanline < uiHeight )
            {
                unsigned int uiAddrRGB( cinfo.output_scanline * uiWidth );

                unsigned char* pucScanline( jpegBuf[ 0 ] );
                jpeg_read_scanlines( &cinfo, jpegBuf, 1 );

                for( unsigned int i( 0 ); i < uiWidth; ++i )
                {
                    colRawTexture[ uiAddrRGB + i ] =
                        ( *( pucScanline + i ) << 16 ) +
                        ( *( pucScanline + i ) <<  8 ) +
                        ( *( pucScanline + i )       ) + uiAlphaValue;
                }
            }

            if( false != bAlpha )
            {
                // read alpha part of texture
                while( cinfo.output_scanline < uiHeight << 1 )
                {
                    unsigned int uiAddrRGB( ( cinfo.output_scanline - uiHeight ) * uiWidth );

                    unsigned char* pucScanline( jpegBuf[ 0 ] );
                    jpeg_read_scanlines( &cinfo, jpegBuf, 1 );

                    for( unsigned int i( 0 ); i < uiWidth; ++i )
                    {
                        colRawTexture[ uiAddrRGB + i ] |=
                            *( pucScanline + i ) << 24;
                    }
                }
            }
        }

        HRESULT hr;

        // create d3d texture
        if( FAILED( hr = D3DXCreateTexture( 
                                m_pD3DDev,                                   // device
                                uiWidth,                                     // width
                                uiHeight,                                    // height
                                ( false != bMipMaps ) ? D3DX_DEFAULT : 1,    // num of mip maps
                                0,                                           // usage
                                ChooseTextureFormat( bAlpha, bCompressed ),  // format
                                D3DPOOL_MANAGED,                             // pool
                                &pkTexture                                   // texture
                                ) ) )
        {
            throw( CTMException( "Couldn't create texture!", hr ) );
        }

        // get access to mip level 0 of texture
        IDirect3DSurface8* pkTextureSurface( 0 );
        if( FAILED( hr = pkTexture->GetSurfaceLevel( 
                                0,                          // mip map level
                                &pkTextureSurface           // texture surface
                                ) ) )
        {
            throw( CTMException( "Couldn't get texture surface!", hr ) );
        }

        // blit raw data into texture
        RECT srcRect;
        InitRect( srcRect, uiWidth, uiHeight );
        if( FAILED( hr = D3DXLoadSurfaceFromMemory( 
                                pkTextureSurface,                   // dest texture surface
                                0,                                  // dest palette
                                0,                                  // dest rect
                                &colRawTexture[ 0 ],                // src
                                D3DFMT_A8R8G8B8,                    // src format
                                uiWidth * sizeof( unsigned int ),   // src pitch
                                0,                                  // src palette
                                &srcRect,                           // src rect
                                D3DX_DEFAULT,                       // filter
                                0                                   // color key
                                ) ) )
        {
            throw( CTMException( "Couldn't copy texture!", hr ) );
        }

        pkTextureSurface->Release();

        if( false != bMipMaps )
        {
            // create mipmap levels
            if( FAILED( hr = D3DXFilterTexture( 
                                pkTexture,          // texture
                                0,                  // palette
                                0,                  // mip map level (+ subsequent)
                                D3DX_DEFAULT        // mip filter
                                ) ) )
            {
                throw( CTMException( "Couldn't filter texture!", hr ) );
            }
        }

        bSuccess = true;
    }
    catch( CTMException& kException )
    {
        kException.ReportError();

        if( 0 != pkTexture )
        {
            pkTexture->Release();
        }
    }

    JPEGCleanUp( cinfo );

    if( false == bSuccess )
    {
        return( false );
    }

    RegisterTexture( strFileName, pkTexture );

    return( true );
}



bool
CTextureManager::LoadJPEGCubeTexture( const string& strFileName, 
                                      bool bAlpha, bool bMipMaps, bool bCompressed )
{
    if( false != IsTextureRegistered( strFileName ) )
    {
        return( true );
    }

    vector< unsigned char > colJPEGData;
    if( false == m_pkArchive->DecompressFile( strFileName, colJPEGData ) )
    {
        return( false );
    }

    IDirect3DCubeTexture8*  pkTexture( 0 );
    bool                    bSuccess( false );
    jpeg_decompress_struct  cinfo;

    try
    {
        // setup jpeg error routines and overwrite error_exit with our own
        jpeg_error_mgr jerr;
        cinfo.err       = jpeg_std_error( &jerr );
        jerr.error_exit = JPEGErrorExit;

        // prepare decompression
        jpeg_create_decompress( &cinfo );
        jpeg_stdio_src( &cinfo, 0 );

        // load jpeg data from memory
        cinfo.src->bytes_in_buffer = colJPEGData.size();
        cinfo.src->next_input_byte = (JOCTET *) &colJPEGData[ 0 ];

        // read header
        jpeg_read_header( &cinfo, TRUE );

        // set to high quality decompression
        cinfo.dct_method          = JDCT_FLOAT;
        cinfo.do_fancy_upsampling = TRUE;
        cinfo.do_block_smoothing  = TRUE;

        // start decompression
        jpeg_start_decompress( &cinfo );

        // allocate memory for one scanline
        JSAMPARRAY jpegBuf( (*cinfo.mem->alloc_sarray) ( (j_common_ptr) &cinfo, JPOOL_IMAGE,
            cinfo.output_width * cinfo.out_color_components, 1 ) );

        // get texture dimension
        unsigned int uiWidth( cinfo.output_width );
        unsigned int uiHeight( cinfo.output_height );
        
        if( false == bAlpha )
        {
            // height must be six times width and greater than zero
            if( uiHeight != uiWidth * 6 || uiHeight == 0 )
            {
                throw( CTMException( "Unexpected texture format!") );
            }

            // get real height of texture for one side of the cube
            uiHeight /= 6;
        }
        else
        {
            // height must be twelve times width and greater than zero
            if( uiHeight != uiWidth * 12 || uiHeight == 0 )
            {
                throw( CTMException( "Unexpected texture format!") );
            }

            // get real height of texture for one side of the cube
            uiHeight /= 12;
        }


        // create temporary decompression buffer (32bit ARGB)
        vector< unsigned int > colRawTexture( uiWidth * uiHeight );

        // create d3d cube texture
        HRESULT hr;
        if( FAILED( hr = D3DXCreateCubeTexture(
                                m_pD3DDev,                                   // device
                                uiWidth,                                     // width / height
                                ( false != bMipMaps ) ? D3DX_DEFAULT : 1,    // num of mip maps
                                0,                                           // usage
                                ChooseTextureFormat( bAlpha, bCompressed ),  // format
                                D3DPOOL_MANAGED,                             // pool
                                &pkTexture                                   // texture
                                ) ) )
        {
            throw( CTMException( "Couldn't create cube texture!") );
        }

        for( int i( 0 ); i < 6; ++i )
        {
            // texture is rgb
            if( 3 == cinfo.out_color_components )
            {
                unsigned int uiAddAlpha( ( false != bAlpha ) ? 2 : 1 );
                unsigned int uiAlphaValue( ( false != bAlpha ) ? 0x0 : 0xFF000000 );
                while( cinfo.output_scanline < uiHeight * ( uiAddAlpha * i + 1 ) )
                {
                    unsigned int uiAddrRGB( ( cinfo.output_scanline - 
                        ( uiHeight * uiAddAlpha * i ) ) * uiWidth );
                    unsigned int uiPixelOffset( 0 );

                    unsigned char* pucScanline( jpegBuf[ 0 ] );
                    jpeg_read_scanlines( &cinfo, jpegBuf, 1 );

                    for( unsigned int i( 0 ); i < uiWidth; ++i, uiPixelOffset += 3 )
                    {
                        colRawTexture[ uiAddrRGB + i ] =
                            ( *( pucScanline + uiPixelOffset + 0 ) << 16 ) +
                            ( *( pucScanline + uiPixelOffset + 1 ) <<  8 ) +
                            ( *( pucScanline + uiPixelOffset + 2 )       ) + uiAlphaValue;
                    }
                }

                if( false != bAlpha )
                {
                    // read alpha part of texture
                    while( cinfo.output_scanline < uiHeight * 2 * ( i + 1  ) )
                    {
                        unsigned int uiAddrRGB( ( cinfo.output_scanline - ( uiHeight * ( 2 * i + 1 ) ) ) * uiWidth );
                        unsigned int uiPixelOffset( 0 );

                        unsigned char* pucScanline( jpegBuf[ 0 ] );
                        jpeg_read_scanlines( &cinfo, jpegBuf, 1 );

                        for( unsigned int i( 0 ); i < uiWidth; ++i, uiPixelOffset += 3 )
                        {
                            colRawTexture[ uiAddrRGB + i ] |=
                                ( ( *( pucScanline + uiPixelOffset + 0 ) +
                                    *( pucScanline + uiPixelOffset + 1 ) +
                                    *( pucScanline + uiPixelOffset + 2 ) ) / 3 ) << 24;
                        }
                    }
                }
            }
            // texture is gray scale
            else
            {
                unsigned int uiAddAlpha( ( false != bAlpha ) ? 2 : 1 );
                unsigned int uiAlphaValue( ( false != bAlpha ) ? 0x0 : 0xFF000000 );
                while( cinfo.output_scanline < uiHeight * ( uiAddAlpha * i + 1 ) )
                {
                    unsigned int uiAddrRGB( ( cinfo.output_scanline - 
                        ( uiHeight * uiAddAlpha * i ) ) * uiWidth );

                    unsigned char* pucScanline( jpegBuf[ 0 ] );
                    jpeg_read_scanlines( &cinfo, jpegBuf, 1 );

                    for( unsigned int i( 0 ); i < uiWidth; ++i )
                    {
                        colRawTexture[ uiAddrRGB + i ] =
                            ( *( pucScanline + i ) << 16 ) +
                            ( *( pucScanline + i ) <<  8 ) +
                            ( *( pucScanline + i )       ) + uiAlphaValue;
                    }
                }

                if( false != bAlpha )
                {
                    // read alpha part of texture
                    while( cinfo.output_scanline < uiHeight * 2 * ( i + 1  ) )
                    {
                        unsigned int uiAddrRGB( ( cinfo.output_scanline - ( uiHeight * ( 2 * i + 1 ) ) ) * uiWidth );

                        unsigned char* pucScanline( jpegBuf[ 0 ] );
                        jpeg_read_scanlines( &cinfo, jpegBuf, 1 );

                        for( unsigned int i( 0 ); i < uiWidth; ++i )
                        {
                            colRawTexture[ uiAddrRGB + i ] |=
                                *( pucScanline + i ) << 24;
                        }
                    }
                }
            }

            // get access to mip level 0 of texture
            IDirect3DSurface8* pkCubeTextureSurface( 0 );
            if( FAILED( hr = pkTexture->GetCubeMapSurface( 
                                    c_cubeFaces[ i ],          // cube map face
                                    0,                         // mip map level
                                    &pkCubeTextureSurface      // texture surface
                                    ) ) )
            {
                throw( CTMException( "Couldn't get surface of cube texture face!", hr ) );
            }

            // blit raw data into texture
            RECT srcRect;
            InitRect( srcRect, uiWidth, uiHeight );
            if( FAILED( hr = D3DXLoadSurfaceFromMemory( 
                                    pkCubeTextureSurface,               // dest texture surface
                                    0,                                  // dest palette
                                    0,                                  // dest rect
                                    &colRawTexture[ 0 ],                // src
                                    D3DFMT_A8R8G8B8,                    // src format
                                    uiWidth * sizeof( unsigned int ),   // src pitch
                                    0,                                  // src palette
                                    &srcRect,                           // src rect
                                    D3DX_DEFAULT,                       // filter
                                    0                                   // color key
                                    ) ) )
            {
                pkCubeTextureSurface->Release();
                throw( CTMException( "Couldn't copy cube texture face!", hr ) );
            }

            pkCubeTextureSurface->Release();
        }

        if( false != bMipMaps )
        {
            // create mipmap levels
            if( FAILED( hr = D3DXFilterCubeTexture( 
                                    pkTexture,                          // texture
                                    0,                                  // palette
                                    0,                                  // mip map level (+ subsequent)
                                    D3DX_DEFAULT                        // mip filter
                                    ) ) )
            {
                throw( CTMException( "Couldn't filter cube texture!", hr ) );
            }
        }

        bSuccess = true;
    }
    catch( CTMException& kException )
    {
        kException.ReportError();

        if( 0 != pkTexture )
        {
            pkTexture->Release();
        }
    }

    JPEGCleanUp( cinfo );

    if( false == bSuccess )
    {
        return( false );
    }

    RegisterTexture( strFileName, pkTexture );

    return( true );
}
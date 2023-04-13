///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CAVICompressor.
 *
 * @file
 *  AVICompressor.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  21.12.2001
 * @brief
 *  Implementation of \e Shared::CAVICompressor.
 */



#include "StdAfx.h"
#include "AVICompressor.h"
#include <exception>



using namespace Shared;
using namespace std;



CAVICompressor::CAVICompressor( const std::string& strAVIFileName,
                                unsigned int uiFrameRate,
                                unsigned int uiXRes, unsigned int uiYRes )
: m_pAVIFile( 0 )
, m_pAVIStream( 0 )
, m_pAVICompressedVideoStream( 0 )
, m_pAVIAudioStream( 0 )
, m_uiFrameRate( uiFrameRate )
, m_uiXRes( uiXRes )
, m_uiYRes( uiYRes )
, m_uiFrameCount( 0 )
, m_colAVIVideoBuffer( uiXRes * uiYRes * 3 )
{
    try
    {
        // check version
        if( 0x010a > HIWORD( VideoForWindowsVersion() ) )
        {
            throw( exception( "Video for Windows Version too old!" ) );
        }

        // open video file
        if( FAILED( AVIFileOpen(
                                 &m_pAVIFile,
                                 strAVIFileName.c_str(),
                                 OF_WRITE | OF_CREATE,
                                 0
                                ) ) )
        {
            throw( exception( "Failed to open destination avi file!" ) );
        }

        // bitmap structure of a video frame
        BITMAPINFO bmpVideoFormat =
        {
            {
                sizeof( BITMAPINFO ),
                m_uiXRes,
                m_uiYRes,
                1,
                24,
                BI_RGB,
                m_uiXRes * m_uiYRes * 3,
                0,
                0,
                0,
                0
            }
        };

        // fill in the header for the video stream
        AVISTREAMINFO streamHeader;
        memset( &streamHeader, 0, sizeof( streamHeader ) );
        streamHeader.fccType                = streamtypeVIDEO;
        streamHeader.fccHandler             = 0;
        streamHeader.dwScale                = 1;
        streamHeader.dwRate                 = m_uiFrameRate;
        streamHeader.dwSuggestedBufferSize  = 0;
        streamHeader.dwQuality              = -1;

        SetRect(
                 &streamHeader.rcFrame,
                 0,
                 0,
                 (int) bmpVideoFormat.bmiHeader.biWidth,
                 (int) bmpVideoFormat.bmiHeader.biHeight
                );

        // create the stream
        if( FAILED( AVIFileCreateStream(
                                         m_pAVIFile,
                                         &m_pAVIStream,
                                         &streamHeader
                                        ) ) )
        {
            throw( exception( "Failed to create the stream!" ) );
        }

        // get save options
        AVICOMPRESSOPTIONS opts;
        AVICOMPRESSOPTIONS FAR * ptrOpts[ 1 ] =
        {
            &opts
        };

        memset( &opts, 0, sizeof( opts ) );
        if( 0 == AVISaveOptions(
                                 0,
                                 ICMF_CHOOSE_KEYFRAME | ICMF_CHOOSE_DATARATE,
                                 1,
                                 &m_pAVIStream,
                                 (LPAVICOMPRESSOPTIONS FAR *) &ptrOpts
                                ) )
        {
            throw( exception( "Failed to get save options!" ) );
        }

        // create a compressed video stream
        if( FAILED( AVIMakeCompressedStream(
                                             &m_pAVICompressedVideoStream,
                                             m_pAVIStream,
                                             &opts,
                                             0
                                            ) ) )
        {
            throw( exception( "Failed to create a compressed video stream!" ) );
        }

        // set format of compressed video stream
        if( FAILED( AVIStreamSetFormat(
                                        m_pAVICompressedVideoStream,
                                        0,
                                        &bmpVideoFormat,
                                        sizeof( bmpVideoFormat )
                                        ) ) )
        {
            throw( exception( "Failed to set the format of the compressed video stream!" ) );
        }
    }
    catch( exception& e )
    {
        ::MessageBox( 0, e.what(), "AVICompressor reports an error!", MB_OK | MB_ICONSTOP );
        CloseAVI();

        throw;
    }
}



CAVICompressor::~CAVICompressor()
{
    CloseAVI();
}



void
CAVICompressor::CloseAVI()
{
    if( 0 != m_pAVIAudioStream )
    {
        AVIStreamClose( m_pAVIAudioStream );
    }

    if( 0 != m_pAVICompressedVideoStream )
    {
        AVIStreamClose( m_pAVICompressedVideoStream );
    }

    if( 0 != m_pAVIStream )
    {
        AVIStreamClose( m_pAVIStream );
    }

    if( 0 != m_pAVIFile )
    {
        AVIFileClose( m_pAVIFile );
    }

    AVIFileExit();
}



bool
CAVICompressor::WriteVideoFrame( IDirect3DDevice8*& pD3DDev )
{
    bool bRet( true );

    IDirect3DSurface8* pBackBuffer;
    if( SUCCEEDED( pD3DDev->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) ) )
    {
        // surface has to be 32 bit per color
        D3DSURFACE_DESC backBufferDesc;
        pBackBuffer->GetDesc( &backBufferDesc );

        if( D3DFMT_A8R8G8B8 != backBufferDesc.Format &&
            D3DFMT_X8R8G8B8 != backBufferDesc.Format )
        {
            bRet = false;
        }

        // lock surface to capture rendered frame
        D3DLOCKED_RECT rect;
        if( SUCCEEDED( pBackBuffer->LockRect( &rect, 0, D3DLOCK_READONLY ) ) )
        {
            unsigned int uiAddr( 0 );
            for( unsigned int y( 0 ); y < m_uiYRes; ++y )
            {
                unsigned int* puiColors( (unsigned int *) ( (unsigned char*) rect.pBits +
                                                            rect.Pitch * ( m_uiYRes - 1 - y ) ) );
                for( unsigned int x( 0 ); x < m_uiXRes; ++x )
                {
                    m_colAVIVideoBuffer[ uiAddr + 0 ] = (*puiColors       ) & 0xFF;
                    m_colAVIVideoBuffer[ uiAddr + 1 ] = (*puiColors >>  8 ) & 0xFF;
                    m_colAVIVideoBuffer[ uiAddr + 2 ] = (*puiColors >> 16 ) & 0xFF;

                    ++puiColors;
                    uiAddr += 3;
                }
            }
            pBackBuffer->UnlockRect();

            HRESULT hr;
            if( FAILED( hr = AVIStreamWrite(
                                            m_pAVICompressedVideoStream,
                                            m_uiFrameCount,
                                            1,
                                            &m_colAVIVideoBuffer[ 0 ],
                                            m_uiXRes * m_uiYRes * 3,
                                            0,
                                            0,
                                            0
                                            ) ) )
            {
                bRet = false;
            }

            ++m_uiFrameCount;
        }
        pBackBuffer->Release();
    }
    else
    {
        bRet = false;
    }

    return( bRet );
}
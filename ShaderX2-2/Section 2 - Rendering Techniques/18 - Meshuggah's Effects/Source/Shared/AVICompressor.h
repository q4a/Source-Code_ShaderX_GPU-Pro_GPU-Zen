///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Shared::CAVICompressor.
 *
 * @file
 *  AVICompressor.h
 * @author
 *  Carsten Wenzel
 * @date
 *  21.12.2001
 * @brief
 *  Declaration of \e Shared::CAVICompressor.
 */



#ifndef _AVICOMPRESSOR_H_INCLUDED_
#define _AVICOMPRESSOR_H_INCLUDED_

#pragma once



#include "vfw.h"
#include <string>
#include <vector>



namespace Shared
{



    /**
    * This class implements an AVI video compressor.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  21.12.2001
    * @brief
    *  This class implements an AVI video compressor.
    */
    class CAVICompressor
    {
    public:
        /**
        * Constructor.
        * 
        * @param strAVIFileName
        *  File name to save AVI as.
        * @param uiFrameRate
        *  Frame rate of AVI video.
        * @param uiXRes
        *  Horizontal resolution of AVI video.
        * @param uiYRes
        *  Vertical resolution of AVI video.
        * @throws std::exception
        *  If AVI compressor could not be initialized.
        * @brief
        *  Constructor.
        */
        CAVICompressor( const std::string& strAVIFileName, 
                        unsigned int uiFrameRate,
                        unsigned int uiXRes, unsigned int uiYRes );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CAVICompressor();        

        /**
        * Grabs a frame from the current back buffer of the D3D device
        * and writes it to the AVI file.
        *
        * @param pD3DDev
        *  Pointer to a D3D device which should used for frame grabbing.
        * @return
        *  \b true if successful else \b false. 
        *  - Possible causes for a failed method call:
        *    -# back buffer surface could not be locked
        *    -# back buffer surface is not 32 bit ARGB
        *    -# writing the frame to the AVI stream failed
        *
        * @brief
        *  Grabs a frame from the current back buffer of the D3D device
        *  and writes it to the AVI file.
        */
        bool WriteVideoFrame( IDirect3DDevice8*& pD3DDev );

    private:
        /**
        * Closes all streams and the AVI file. Releases all allocated resources.
        * 
        * @brief
        *  Closes all streams and the AVI file. Releases all allocated resources.
        */
        void CloseAVI();

    private:
        /** 
        * AVI file used to write result of capturing.
        * 
        * @brief
        *  AVI file used to write result of capturing.
        */
        PAVIFILE m_pAVIFile;
        
        /** 
        * AVI stream.
        * 
        * @brief
        *  AVI stream.
        */
        PAVISTREAM m_pAVIStream;
        
        /** 
        * AVI video sub stream.
        * 
        * @brief
        *  AVI video sub stream.
        */
        PAVISTREAM m_pAVICompressedVideoStream;

        /** 
        * AVI audio sub stream.
        * 
        * @brief
        *  AVI audio sub stream.
        */
        PAVISTREAM m_pAVIAudioStream;

        /** 
        * Framerate of AVI.
        * 
        * @brief
        *  Framerate of AVI.
        */
        unsigned int m_uiFrameRate;

        /** 
        * Horizontal resolution of AVI.
        * 
        * @brief
        *  Horizontal resolution of AVI.
        */
        unsigned int m_uiXRes;

        /** 
        * Vertical resolution of AVI.
        * 
        * @brief
        *  Vertical resolution of AVI.
        */
        unsigned int m_uiYRes;

        /** 
        * Frame counter of AVI.
        * 
        * @brief
        *  Frame counter of AVI.
        */
        unsigned int m_uiFrameCount;

        /** 
        * Temporary back buffer for an uncompressed video frame.
        * Used to convert pixels from a D3D back buffer to 24 bit RGB values and 
        * feed the AVI compressor.
        *
        * @brief
        *  Temporary back buffer for an uncompressed video frame.
        */
        std::vector< unsigned char > m_colAVIVideoBuffer;
    };



} // namespace Shared



#endif // #ifndef _AVICOMPRESSOR_H_INCLUDED_
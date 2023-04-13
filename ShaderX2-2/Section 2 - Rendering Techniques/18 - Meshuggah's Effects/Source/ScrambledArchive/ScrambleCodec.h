///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e ScrambledArchive::CScrambleCodec.
 * 
 * @file
 *  ScrambleCodec.h
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Declaration of \e ScrambledArchive::CScrambleCodec.
 */



#ifndef _SCRAMBLECODEC_H_INCLUDED_
#define _SCRAMBLECODEC_H_INCLUDED_

#pragma once



#include "MD5Generator.h"
#include <string>



namespace ScrambledArchive
{



    /**
    * A scramble codec using TEA do code and decode data.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  A scramble codec using TEA do code and decode data.
    */
    class CScrambleCodec  
    {
    public:       
        /**
        * Creates a scrambled version of a given file using its MD5 key for encryption.
        * Padding bytes will be added to fill up the data to a multiple of #TEA_DATASIZE bytes.
        * A SCryptoHeader will be put in front of the scrambled data which is necessary for deciphering.

        * @param strInFile
        *  File to scramble.
        * @param strOutFile
        *  File to write scrambled data to.
        * @param sMD5
        *  MD5 of input file.
        * @return
        *  \b true if successful else \b false.
        * @brief
        *  Creates a scrambled version of a given file using its MD5 key for encryption.
        */
        static bool CreateScrambledArchive( const std::string strInFile, 
            const std::string strOutFile, const CMD5Generator::SMD5& sMD5 );

        /**
        * Deciphers scrambled data in place.
        * 
        * @param pucData
        *  Pointer to scrambled data.
        * @param uiDataSize
        *  Size of scrambled data (including header and padding bytes).
        * @param pucDecipheredArchive
        *  Pointer to unscrambled data.
        * @param uiDecipheredArchiveSize
        *  Size of unscrambled data.
        * @return
        *  \b true if successful else \b false.
        * @brief
        *  Deciphers scrambled data in place.
        */
        static bool DecipherArchive( unsigned char* pucData, unsigned int uiDataSize,
            unsigned char*& pucDecipheredArchive, unsigned int& uiDecipheredArchiveSize );
       
    private:
        /** 
        * Tiny encryption specific constants. These are crucial for TEA to work!
        * 
        * @brief
        *  Tiny encryption specific constants. These are crucial for TEA to work!
        */
        enum ETeaConsts
        {
            /** 
            * Data size for a cipher block (64 bits).
            */
            TEA_DATASIZE =  8,

            /** 
            * 128 bit key.
            */
            TEA_KEYSIZE  = 16,

            /** 
            * Delta for TEA.
            */
            DELTA = 0x9E3779B9,
            
            /** 
            * Number of en/decoding cycles.
            */
            N     = 32,

            /** 
            * 32 bit sum of DELTA * N.
            */
            SUM   = 0xC6EF3720  // SUM = DELTA * N
        };

        /**
        * Crypto header structure. 
        * CScrambleCodec::CreateScrambledArchive places this before a scrambled archive.
        * It contains necessary information for deciphering the scrambled archive later using 
        * CScrambleCodec::DecipherArchive.
        *
        * @author
        *  Carsten Wenzel
        * @date
        *  25.02.2002
        * @brief
        *  Crypto header structure.
        */
        struct SCryptoHeader
        {
            /** 
            * Crypto tag. For verification purposes.
            * 
            * @brief
            *  Crypto tag.
            */
            enum ECryptoHeaderTag
            {
                /** 
                * "CRYP"
                */
                CHT_CRYPTOTAG = 0x50595243  // C.R.Y.P.
            };

            /** 
            * Tag of crypto header.
            * 
            * @brief
            *  Tag of crypto header.
            */
            unsigned int m_uiTag;

            /** 
            * Size of original, unscrambled data.
            * 
            * @brief
            *  Size of original, unscrambled data.
            */
            unsigned int m_uiOrigDataSize;

            /** 
            * MD5 key for data.
            * 
            * @brief
            *  MD5 key for data.
            */
            CMD5Generator::SMD5 m_sMD5;

            /**
            * Constructor.
            * 
            * @brief
            *  Constructor.
            */
            SCryptoHeader();

            /**
            * Constructor.
            * 
            * @param uiOrigDataSize
            *  Size of original, unscrambled data.
            * @param sMD5
            *  MD5 key for data.
            * @brief
            *  Constructor.
            */
            SCryptoHeader( unsigned int uiOrigDataSize, const CMD5Generator::SMD5& sMD5 );
        };

    private:
        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CScrambleCodec();

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CScrambleCodec();

        /**
        * TEA encipher.
        * 
        * @param puiV
        *  Pointer to 64 bit of input data.
        * @param puiW
        *  Pointer to 64 bit of output data.
        * @param puiK
        *  Pointer to 128 bit key.
        * @brief
        *  TEA encipher.
        */
        static void Encipher( const unsigned int* const puiV, 
            unsigned int* const puiW, const unsigned int* const puiK );

        /**
        * TEA decipher.
        * 
        * @param puiV
        *  Pointer to 64 bit of input data.
        * @param puiW
        *  Pointer to 64 bit of output data.
        * @param puiK
        *  Pointer to 128 bit key.
        * @brief
        *  TEA decipher.
        */
        static void Decipher( const unsigned int* const puiV, 
            unsigned int* const puiW, const unsigned int* const puiK );
    };



#include "ScrambleCodec.inl"



} // namespace ScrambledArchive



#endif // #ifndef _SCRAMBLECODEC_H_INCLUDED_

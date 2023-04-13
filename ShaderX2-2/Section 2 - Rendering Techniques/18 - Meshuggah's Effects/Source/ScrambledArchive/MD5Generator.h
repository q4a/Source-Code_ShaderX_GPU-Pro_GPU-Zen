///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e ScrambledArchive::CMD5Generator.
 * 
 * @file
 *  MD5Generator.h
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Declaration of \e ScrambledArchive::CMD5Generator.
 */



#ifndef _MD5GENERATOR_H_INCLUDED_
#define _MD5GENERATOR_H_INCLUDED_

#pragma once



#include <string>



namespace ScrambledArchive
{



    /**
    * A MD5 (message digest 5) generator class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  A MD5 (message digest 5) generator class.
    */
    class CMD5Generator  
    {
    public:
        /**
        * A structure storing a MD5 signature.
        *
        * @author
        *  Carsten Wenzel
        * @date
        *  25.02.2002
        * @brief
        *  A structure storing a MD5 signature.
        */
        struct SMD5
        {
            enum ESignatureSize
            {
                /** 
                * MD5 signature size in bytes.
                */
                SIGNATURESIZE = 16
            };

            /** 
            * MD5 signature array.
            * 
            * @brief
            *  MD5 signature array.
            */
            unsigned char m_pucSignature[ SIGNATURESIZE ];
        };

        /**
        * Generates a MD5 (message digest 5) signature from data stored at \e pucData.
        * 
        * @brief
        *  Generates a MD5 (message digest 5) signature from data stored at \e pucData.
        * @param sMD5
        *  Contains the generated MD5 signature if method returned successfully.
        * @param pucData
        *  Ptr to data for which a MD5 signature should be generated from. Can be \bNULL!
        * @param uiLength
        *  Length of data. \bMust be \bNULL if \e pucData is \NULL.
        * @return
        *  \b true if successfull else \b false. This happens if \e pucData == 0 and \e uiLength > 0.
        */
        static bool GetMD5( SMD5& sMD5, const unsigned char* const pucData, unsigned int uiLength );

        /**
        * Generates a MD5 (message digest 5) signature from a file at \e strFilePath.
        * 
        * @brief
        *  Generates a MD5 (message digest 5) signature from a file at \e strFilePath.
        * @param sMD5
        *  Contains the generated MD5 signature if method returned successfully.
        * @param strFilePath
        *  A string that contains the path of the file.
        * @return
        *  \b true if successfull else \b false (if it wasn't possible to open the file).
        */
        static bool GetMD5( SMD5& sMD5, const std::string& strFilePath );

    protected:
        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CMD5Generator();

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CMD5Generator();

        /**
        * MD5 Transform.
        * 
        * @brief
        *  MD5 Transform.
        */
        void Transform( const unsigned char* const pucBlock );

        /**
        * MD5 Update.
        * 
        * @brief
        *  MD5 Update.
        */
        void Update( const unsigned char* const pucInput, unsigned int uiInputLen );

        /**
        * MD5 Final.
        * 
        * @brief
        *  MD5 Final.
        */
        void Final( unsigned char* pucMD5 );

        /**
        * MD5 RotateLeft.
        * 
        * @brief
        *  MD5 RotateLeft.
        */
        unsigned int RotateLeft( unsigned int uiX, int iN );

        /**
        * MD5 FF.
        * 
        * @brief
        *  MD5 FF.
        */
        void FF( unsigned int& uiA, unsigned int uiB, unsigned int uiC, unsigned int uiD, 
                unsigned int uiX, unsigned int uiS, unsigned int uiT );

        /**
        * MD5 GG.
        * 
        * @brief
        *  MD5 GG.
        */
        void GG( unsigned int& uiA, unsigned int uiB, unsigned int uiC, unsigned int uiD, 
                unsigned int uiX, unsigned int uiS, unsigned int uiT );

        /**
        * MD5 HH.
        * 
        * @brief
        *  MD5 HH.
        */
        void HH( unsigned int& uiA, unsigned int uiB, unsigned int uiC, unsigned int uiD, 
                unsigned int uiX, unsigned int uiS, unsigned int uiT );

        /**
        * MD5 II.
        * 
        * @brief
        *  MD5 II.
        */
        void II( unsigned int& uiA, unsigned int uiB, unsigned int uiC, unsigned int uiD, 
                unsigned int uiX, unsigned int uiS, unsigned int uiT );

        /**
        * Unpacks a bunch of dwords into bytes.
        * 
        * @brief
        *  Unpacks a bunch of dwords into bytes.
        */
        void Decode( unsigned int* puiOutput, const unsigned char* const pucInput, unsigned int uiLength );

        /**
        * Packs a bunch of bytes into dwords.
        * 
        * @brief
        *  Packs a bunch of bytes into dwords.
        */
        void Encode( unsigned char* pucOutput, const unsigned int* const puiInput, unsigned int uiLength );

    private:
        /**
        * MD5 input buffer.
        * 
        * @brief
        *  MD5 input buffer.
        */
        unsigned char m_pucInputBuffer[ 64 ];

        /**
        * MD5 64 bit counter.
        * 
        * @brief
        *  MD5 64 bit counter.
        */
        unsigned int  m_puiCount[ 2 ];          

        /**
        * MD5 key.
        * 
        * @brief
        *  MD5 key.
        */
        unsigned int  m_puiMD5[ 4 ];            
    };



} // namespace ScrambledArchive



#endif // #ifndef _MD5GENERATOR_H_INCLUDED_

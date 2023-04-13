///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Contains structures of ZIP's local, file and directory headers. 
 * 
 * @file
 *  ZipStructure.h
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Contains structures of ZIP's local, file and directory headers. 
 */



#ifndef __ZIPSTRUCTURE_H_INCLUDED__
#define __ZIPSTRUCTURE_H_INCLUDED__

#pragma once



namespace ScrambledArchive
{


// align all structure on two-byte boundaries (the way they are stored on disk)
// that is, no padding bytes are inserted by the compiler to allow for faster access
#pragma pack( 2 )



    /**
    * Local zip header structure.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  Local zip header structure.
    */
    struct SZipLocalHeader
    {
        /** 
        * Local zip header constants.
        * 
        * @brief
        *  Local zip header constants.
        */
        enum EZipLocalHeaderConstants
        {
            /** 
            * Signature of zip local header.
            */
            SIGNATURE   = 0x04034b50,

            /** 
            * Flag for stored entry.
            */
            COMP_STORE  = 0,

            /** 
            * Flag for compressed entry.
            */
            COMP_DEFLAT = 8,
        };

        unsigned int    uiSig;
        unsigned short  usVersion;
        unsigned short  usFlag;
        unsigned short  usCompression;      // COMP_xxxx
        unsigned short  usModTime;
        unsigned short  usModDate;
        unsigned int    uiCrc32;
        unsigned int    uiComprSize;
        unsigned int    uiUncomprSize;
        unsigned short  usFnameLen;         // Filename string follows header.
        unsigned short  usXtraLen;          // Extra field follows filename.
    };



    /**
    * Zip directory header structure.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  Zip directory header structure.
    */
    struct SZipDirHeader
    {
        /** 
        * Zip directory header constants.
        * 
        * @brief
        *  Zip directory header constants.
        */
        enum EZipDirHeaderConstants
        {
            /** 
            * Signature of zip directory header.
            */
            SIGNATURE = 0x06054b50,
        };

        unsigned int    uiSig;
        unsigned short  usNumDisks;
        unsigned short  usStartDisk;
        unsigned short  usNumDirEntries;
        unsigned short  usTotalDirEntries;
        unsigned int    uiDirSize;
        unsigned int    uiDirOffset;
        unsigned short  usCmntLen;
    };



    /**
    * Zip directory file header structure.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  Zip directory file header structure.
    */
    struct SZipDirFileHeader
    {
        /** 
        * Zip directory file header constants.
        * 
        * @brief
        *  Zip directory file header constants.
        */
        enum EZipDirFileHeaderConstants
        {
            /** 
            * Signature of zip directory file header.
            */
            SIGNATURE   = 0x02014b50,

            /** 
            * Flag for stored entry.
            */
            COMP_STORE  = 0,

            /** 
            * Flag for compressed entry.
            */
            COMP_DEFLAT = 8,
        };

        unsigned int    uiSig;
        unsigned short  usVerMade;
        unsigned short  usVerNeeded;
        unsigned short  usFlag;
        unsigned short  usCompression;      // COMP_xxxx
        unsigned short  usModTime;
        unsigned short  usModDate;
        unsigned int    uiCrc32;
        unsigned int    uiComprSize;        // Compressed size
        unsigned int    uiUncomprSize;      // Uncompressed size
        unsigned short  usFnameLen;         // Filename string follows header.
        unsigned short  usXtraLen;          // Extra field follows filename.
        unsigned short  usCmntLen;          // Comment field follows extra field.
        unsigned short  usDiskStart;
        unsigned short  usIntAttr;
        unsigned int    uiExtAttr;
        unsigned int    uiHdrOffset;

        char* GetName() const
        {
            return( (char *) ( this + 1 ) );
        }

        char* GetExtra() const
        {
            return( GetName() + usFnameLen );
        }

        char* GetComment() const
        {
            return( GetExtra() + usXtraLen );
        }
    };



#pragma pack()



} // namespace ScrambledArchive



#endif // #ifdef __ZIPSTRUCTURE_H_INCLUDED__

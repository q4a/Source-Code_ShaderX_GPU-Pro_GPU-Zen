///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e ScrambledArchive::CScrambledArchive.
 * 
 * @file
 *  ScrambledArchive.h
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Declaration of \e ScrambledArchive::CScrambledArchive.
 */



#ifndef _SCRAMBLEDARCHIVE_H_INCLUDED_
#define _SCRAMBLEDARCHIVE_H_INCLUDED_

#pragma once



#include <map>
#include <list>
#include <string>
#include <vector>



namespace ScrambledArchive
{



    struct SZipDirFileHeader;



    /**
    * A class allowing read access to an in-memory zip archive that was scrambled 
    * using ScrambledArchive::CScrambleCodec::CreateScrambledArchive().
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  A class allowing read access to an in-memory zip archive that was scrambled 
    *  using ScrambledArchive::CScrambleCodec::CreateScrambledArchive().
    */
    class CScrambledArchive
    {
    public:
        /** 
        * A type describing a list of files/directories in an archive.
        * 
        * @brief
        *  A type describing a list of files/directories in an archive.
        */
        typedef std::list< std::string > ArchiveList;

    public:
        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CScrambledArchive();

        /**
        * Creates a \e CScrambledArchive object for a given archive resource.
        * 
        * @param strArchiveResource
        *  Name of resource representing an archive.
        * @return
        *  Pointer to a \e CScrambledArchive if successful otherwise \b NULL.
        * @brief
        *  Creates a \e CScrambledArchive object for a given archive resource.
        */
        static CScrambledArchive* CreateFromResource( const std::string& strArchiveResource );

        /**
        * Returns a list of files / directories in the archive.
        * 
        * @param kArchiveList
        *  List that recieves the archive index.
        * @brief
        *  Returns a list of files / directories in the archive.
        */
        void GetArchiveList( ArchiveList& kArchiveList ) const;

        /**
        * Decompresses a file from the archive to memory.
        * 
        * @param strPathName
        *  Path name of file to decompress.
        * @param colUncompressedData
        *  Array containing the decompressed data. Previous content will be destroyed 
        *  and allocated memory freed.
        * @return
        *  \b true if compression was successful else \b false.
        * @brief
        *  Decompresses a file from the archive to memory.
        */
        bool DecompressFile( const std::string& strPathName, 
            std::vector< unsigned char >& colUncompressedData ) const;

        /**
        * Decompresses a file from the archive to disk.
        * 
        * @param strPathName
        *  Path name of file to decompress.
        * @param strWriteTo
        *  File path to write the decompressed data to.
        * @return
        *  \b true if compression was successful else \b false.
        * @brief
        *  Decompresses a file from the archive to disk.
        */
        bool DecompressFileToDisk( const std::string& strPathName, 
            const std::string& strWriteTo ) const;    

    private:
        enum EDecompressionBufferSize
        {
            /** 
            * Size of decompression buffer.
            */
            DECOMP_BUF_SIZE = 16384
        };

        /** 
        * A type mapping file names to zip directory file header structures
        * 
        * @brief
        *  A type mapping file names to zip directory file header structures
        */
        typedef std::map< std::string, SZipDirFileHeader* > IndexMap;

    private:
        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CScrambledArchive();

        /**
        * Reads the directory of the archive.
        * 
        * @brief
        *  _ShortDescription_
        */
        bool ReadDirectory();

        /**
        * Scans for the zip directory header in the archive.
        * 
        * @param pucDirHeader
        *  Pointer to the start of the zip directory header in the archive.
        * @return
        *  \b true if directory header was found else \b false.
        * @brief
        *  Scans for the zip directory header in the archive.
        */
        bool FindDirectoryHeader( unsigned char*& pucDirHeader ) const;

        /**
        * Loads a scrambled zip archive from the executable's resource.
        * If the archive hasn't been loaded before it will be unscrambled and added to an internal 
        * archive registry for multiple reuse.
        *
        * @param strArchiveResource
        *  Resource name of scrambled archive in executable.
        * @return
        *  \b true if archive could be loaded else \b false.
        * @brief
        *  Loads a scrambled zip archive from the executable's resource.
        */
        bool LoadFromResource( const std::string& strArchiveResource );

        /** 
        * Pointer to start of archive in memory.
        * 
        * @brief
        *  Pointer to start of archive in memory.
        */
        unsigned char* m_pucArchive;

        /** 
        * Size of archive in bytes.
        * 
        * @brief
        *  Size of archive in bytes.
        */
        unsigned int m_uiSizeOfArchive;

        /** 
        * The file index of the archive.
        * 
        * @brief
        *  The file index of the archive.
        */
        IndexMap m_colArchiveIndex;
    };



#include "ScrambledArchive.inl"



} // namespace ScrambledArchive



#endif // #ifndef _SCRAMBLEDARCHIVE_H_INCLUDED_

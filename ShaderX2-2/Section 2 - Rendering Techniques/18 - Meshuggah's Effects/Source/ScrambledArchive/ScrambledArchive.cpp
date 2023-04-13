///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e ScrambledArchive::CScrambledArchive.
 * 
 * @file
 *  ScrambledArchive.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Implementation of \e ScrambledArchive::CScrambledArchive.
 */



#include "stdafx.h"

#include "ScrambleCodec.h"
#include "ScrambledArchive.h"
#include "ArchiveRegistry.inl"
#include "ZipStructure.h"
#include "zlib.h"
#include <fstream>



using namespace std;
using namespace ScrambledArchive;



CScrambledArchive::CScrambledArchive()
: m_pucArchive( 0 )
, m_uiSizeOfArchive( 0 )
, m_colArchiveIndex()
{
}



CScrambledArchive::~CScrambledArchive()
{
}



CScrambledArchive*
CScrambledArchive::CreateFromResource( const string& strArchiveResource )
{
    CScrambledArchive* pNewArchive( new CScrambledArchive );
    
    if( false != pNewArchive->LoadFromResource( strArchiveResource ) )
    {
        return( pNewArchive );
    }

    delete pNewArchive;

    return( 0 );
}



bool
CScrambledArchive::LoadFromResource( const string& strArchiveResource )
{
    HRSRC  hInfoBlock;
    HANDLE hRes;

    // check if we have previously loaded and deciphered the requested archive 
    CArchiveRegistry::SArchiveInfo sArchiveInfo;
    if( false != CArchiveRegistry::GetInstance().GetArchive( strArchiveResource, sArchiveInfo  ) )
    {
        m_pucArchive = sArchiveInfo.m_pucArchive;
        m_uiSizeOfArchive = sArchiveInfo.m_uiSizeOfArchive;
    }
    else
    {
        // find the requested scrambled archive in our module
        if( 0 == ( hInfoBlock = FindResource( GetModuleHandle( NULL ), 
            strArchiveResource.c_str(), RT_RCDATA ) ) )
        {
            DBG_ASSERTM( 0, "CScrambledArchive::LoadFromResource() : Couldn't find requested archive resource!" );
            return( false );
        }

        // get size
        unsigned int uiResSize( SizeofResource( NULL, hInfoBlock ) );
        if( 0 == uiResSize )
        {
            DBG_ASSERTM( 0, "CScrambledArchive::LoadFromResource() : Couldn't get size of archive resource!" );
            return( false );
        }

        // load resource
        if( 0 == ( hRes = LoadResource( NULL, hInfoBlock ) ) )
        {
            DBG_ASSERTM( 0, "CScrambledArchive::LoadFromResource() : Couldn't load archive resource!" );
            return( false );
        }

        // get pointer to resource data
        unsigned char* pucData( (unsigned char *) LockResource( hRes ) );
        if( 0 == pucData )
        {
            DBG_ASSERTM( 0, "CScrambledArchive::LoadFromResource() : Couldn't get a pointer to archive resource!" );
            return( false );
        }

        // try to remove memory write protection (since we're 
        // going to manipulate the executable's memory!)
        unsigned int uiOldProt;
        if( FALSE != VirtualProtect( pucData, uiResSize, PAGE_READWRITE, (unsigned long *) &uiOldProt ) )
        {
            // decipher archive
            bool bDeciphered( CScrambleCodec::DecipherArchive( pucData, 
                                                               uiResSize, m_pucArchive, m_uiSizeOfArchive ) );

            // re-enable memory write protection
            VirtualProtect( pucData, uiResSize, uiOldProt, (unsigned long *) &uiOldProt );

            if( false == bDeciphered )
            {
                return( false );
            }

            // fill archive info structure 
            sArchiveInfo.m_pucRawData        = 0;
            sArchiveInfo.m_pucArchive        = m_pucArchive;
            sArchiveInfo.m_uiSizeOfArchive   = m_uiSizeOfArchive;
            sArchiveInfo.m_bAutoDeleteMemory = false;
        }
        else
        {
            // Win98/ME maps RT_RCDATA resources to 0x80000000 - 0xbfffffff (shared memory pool)
            // which sucks as it cannot be write-enabled :P

            // create a copy of the archive resource
            // the memory will be freed automatically by the archive registry at program exit
            unsigned char* pucResourceCopy( new unsigned char[ uiResSize ] );
            if( 0 == pucResourceCopy )
            {
                DBG_ASSERTM( 0, "CScrambledArchive::LoadFromResource() : Not enough memory to create an archive resource copy!" );
                return( false );
            }
            
            // copy enciphered archive resource
            memcpy( pucResourceCopy, pucData, uiResSize );

            // decipher 
            bool bDeciphered( CScrambleCodec::DecipherArchive( pucResourceCopy, 
                                                               uiResSize, m_pucArchive, m_uiSizeOfArchive ) );

            if( false == bDeciphered )
            {
                delete pucResourceCopy;
                return( false );
            }

            // fill archive info structure 
            sArchiveInfo.m_pucRawData        = pucResourceCopy;
            sArchiveInfo.m_pucArchive        = m_pucArchive;
            sArchiveInfo.m_uiSizeOfArchive   = m_uiSizeOfArchive;
            sArchiveInfo.m_bAutoDeleteMemory = true;
        }

        // archive has been successfully loaded and deciphered
        // add it to the archive registry
        CArchiveRegistry::GetInstance().RegisterArchive( strArchiveResource, sArchiveInfo );
    }

    // read directory
    return( ReadDirectory() );
}



bool
CScrambledArchive::FindDirectoryHeader( unsigned char*& pucDirHeader ) const
{
    if( m_uiSizeOfArchive >= sizeof( SZipDirHeader ) )
    {
        unsigned char* pucTempPtr( m_pucArchive + m_uiSizeOfArchive - sizeof( SZipDirHeader ) );

        // maximum of 64k for a global comment, start a reverse search
        for( unsigned int uiCount( 0 ); uiCount < 0x10000; ++uiCount )
        {
            if( pucTempPtr < m_pucArchive )
            {
                return( false );
            }

            // check directory header signature
            SZipDirHeader& sDirHeader( *( (SZipDirHeader *) pucTempPtr ) );
            if( SZipDirHeader::SIGNATURE == sDirHeader.uiSig )
            {
                pucDirHeader = pucTempPtr;
                return( true );
            }

            --pucTempPtr;
        }
    }

    return( false );
}



bool
CScrambledArchive::ReadDirectory()
{
    // find zip directory header
    unsigned char* pucDirHeader( 0 );
    if( false == FindDirectoryHeader( pucDirHeader ) )
    {
        return( false );
    }

    // get zip directory header
    SZipDirHeader& sDirHeader( *( (SZipDirHeader *) pucDirHeader ) );

    // get pointer to first file header in the zip directory
    unsigned char* pucDirData( pucDirHeader - sDirHeader.uiDirSize );

    // create file index
    bool bSuccess( true );
    for( unsigned int uiI( 0 ); uiI < sDirHeader.usNumDirEntries && bSuccess == true; ++uiI )
    {
        SZipDirFileHeader& sFileHeader( *( (SZipDirFileHeader *) pucDirData ) );

        if( SZipDirFileHeader::SIGNATURE == sFileHeader.uiSig )
        {
            pucDirData += sizeof( SZipDirFileHeader );

            // convert unix slash to dos backslash
            string strPathName( sFileHeader.GetName(), sFileHeader.usFnameLen );
            for( unsigned int uiA( 0 ); uiA < sFileHeader.usFnameLen; ++uiA )
            {
                if( '/' == strPathName[ uiA ] )
                {
                    strPathName[ uiA ] = '\\';
                }
            }

            // save pointer to allow quick indexing
            m_colArchiveIndex.insert( IndexMap::value_type( strPathName, &sFileHeader ) );

            // skip name, extra and comment fields
            pucDirData += sFileHeader.usFnameLen +
                          sFileHeader.usXtraLen +
                          sFileHeader.usCmntLen;
        }
        // signature is wrong
        else
        {
            bSuccess = false;
            break;
        }
    }

    return( bSuccess );
}



void
CScrambledArchive::GetArchiveList( ArchiveList& kArchiveList ) const
{
    // clear list
    kArchiveList.clear();

    // get start and end to iterate thru map
    IndexMap::const_iterator itCurrent( m_colArchiveIndex.begin() );
    IndexMap::const_iterator itEnd( m_colArchiveIndex.end() );

    // copy all entries
    while( itCurrent != itEnd )
    {
        kArchiveList.push_back( ( *itCurrent ).first );
        ++itCurrent;
    }
}




bool
CScrambledArchive::DecompressFile( const string& strPathName, 
                                   vector< unsigned char >& colUncompressedData ) const
{
    // check if file is in archive
    IndexMap::const_iterator itLookup( m_colArchiveIndex.find( strPathName ) );
    if( m_colArchiveIndex.end() != itLookup )
    {
        // get file header from directory
        SZipDirFileHeader& sDirFileHeader( *( *itLookup ).second );

        // get the local file header
        SZipLocalHeader&   sLocalFileHeader( *( (SZipLocalHeader *) ( m_pucArchive + sDirFileHeader.uiHdrOffset ) ) );

        // check signature
        if( sLocalFileHeader.uiSig != SZipLocalHeader::SIGNATURE )
        {
            return( false );
        }

        // check if entry is a directory
        if( strPathName.rfind( '\\' ) == strPathName.size() - 1 )
        {
            return( true );
        }

        // get pointer to compressed data
        unsigned char* pucCompressedData( m_pucArchive + sDirFileHeader.uiHdrOffset +
            sizeof( SZipLocalHeader ) + sLocalFileHeader.usFnameLen + sLocalFileHeader.usXtraLen );

        // create temporary buffer for uncompressed data
        vector< unsigned char > colTempData( sLocalFileHeader.uiUncomprSize );

        // data is not compressed
        if( SZipLocalHeader::COMP_STORE == sLocalFileHeader.usCompression )
        {
            memcpy( (void *) &colTempData[ 0 ], pucCompressedData, sLocalFileHeader.uiComprSize );

            // swap data buffers 
            // as a result "colUncompressedData" will contain the decompressed data 
            // of "colTempData" and vice versa
            colUncompressedData.swap( colTempData );

            // all allocated memory of "colUncompressedData" that now belongs to
            // "colTempData" will be freed when "colTempData" goes out of scope
            return( true );
        }
        else
        // data is compressed
        if( SZipLocalHeader::COMP_DEFLAT == sLocalFileHeader.usCompression  )
        {
            z_stream stream;
            stream.next_in   = (Bytef *) pucCompressedData;
            stream.avail_in  = (uInt) sLocalFileHeader.uiComprSize;
            stream.next_out  = (Bytef *) &colTempData[ 0 ];
            stream.avail_out = sLocalFileHeader.uiUncomprSize;
            stream.zalloc    = (alloc_func) 0x0;
            stream.zfree     = (free_func) 0x0;

            // Perform inflation. wbits < 0 indicates no zlib header inside the data.
            int iErr( inflateInit2( &stream, -MAX_WBITS ) );
            if( Z_OK == iErr )
            {
                iErr = inflate( &stream, Z_SYNC_FLUSH );
                inflateEnd( &stream );                

                if( stream.total_out == sLocalFileHeader.uiUncomprSize )
                {
                    // swap data buffers 
                    // as a result "colUncompressedData" will contain the decompressed data 
                    // of "colTempData" and vice versa
                    colUncompressedData.swap( colTempData );

                    // all allocated memory of "colUncompressedData" that now belongs to
                    // "colTempData" will be freed when "colTempData" goes out of scope
                    return( true );
                }
            }
        }
    }

    return( false );
}



bool
CScrambledArchive::DecompressFileToDisk( const string& strPathName, 
                                         const string& strWriteTo ) const
{
    // check if file is archive
    IndexMap::const_iterator itLookup( m_colArchiveIndex.find( strPathName ) );
    if( m_colArchiveIndex.end() != itLookup )
    {
        // get file header from the directory
        SZipDirFileHeader& sDirFileHeader( *( *itLookup ).second );

        // get the local file header
        SZipLocalHeader&   sLocalFileHeader( *( (SZipLocalHeader *) ( m_pucArchive + sDirFileHeader.uiHdrOffset ) ) );

        // check signature
        if( SZipLocalHeader::SIGNATURE != sLocalFileHeader.uiSig )
        {
            return( false );
        }

        // check if entry is a directory
        if( strPathName.rfind( '\\' ) == strPathName.size() - 1 )
        {
            return( true );
        }

        // get pointer to compressed data
        unsigned char* pucCompressedData( m_pucArchive + sDirFileHeader.uiHdrOffset +
            sizeof( SZipLocalHeader ) + sLocalFileHeader.usFnameLen + sLocalFileHeader.usXtraLen );

        // data is not compressed
        if( SZipLocalHeader::COMP_STORE == sLocalFileHeader.usCompression )
        {
            ofstream f( strWriteTo.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
            if( false != f.is_open() )
            {
                f.write( (char *) pucCompressedData, sLocalFileHeader.uiComprSize );
                f.close();

                return( true );
            }
        }
        else
        // data is compressed
        if( SZipLocalHeader::COMP_DEFLAT == sLocalFileHeader.usCompression  )
        {
            bool bSuccess( false );

            ofstream f( strWriteTo.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
            if( false != f.is_open() )
            {
                z_stream stream;
                stream.next_in   = (Bytef *) pucCompressedData;
                stream.avail_in  = sLocalFileHeader.uiComprSize;
                stream.zalloc    = (alloc_func) 0x0;
                stream.zfree     = (free_func) 0x0;

                // init decompressor and start decrunching
                int iErr( inflateInit2( &stream, -MAX_WBITS ) );
                if( Z_OK == iErr )
                {
                    // create temporary buffer for uncompressed data
                    vector< unsigned char > colTempData( DECOMP_BUF_SIZE );

                    // how much data has been decompressed so far
                    unsigned int uiTotal( 0 );
                    do
                    {
                        // reset output buffer information
                        stream.next_out  = (Bytef *) &colTempData[ 0 ];
                        stream.avail_out = DECOMP_BUF_SIZE;

                        // and decompress
                        iErr = inflate( &stream, Z_SYNC_FLUSH );
                        if( Z_OK == iErr || 
                            Z_STREAM_END == iErr )
                        {
                            // write data that was decompressed by the last call of inflate()
                            f.write( (char *) &colTempData[ 0 ], stream.total_out - uiTotal );

                            // update status
                            uiTotal = stream.total_out;
                        }
                        else
                        {
                            break;
                        }
                    } while( uiTotal < sLocalFileHeader.uiUncomprSize );

                    // if decompression was successful then set bSuccess
                    if( Z_OK == inflateEnd( &stream ) )
                    {
                        bSuccess = true;
                    }
                }

                // close output file
                f.close();

                return( bSuccess );
            }
        }
    }

    return( false );
}
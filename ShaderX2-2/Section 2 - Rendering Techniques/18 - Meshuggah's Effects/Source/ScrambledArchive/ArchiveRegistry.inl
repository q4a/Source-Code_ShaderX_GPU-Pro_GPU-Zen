///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration and implementation of e\ ScrambledArchive::CArchiveRegistry. 
 * For internal use in ScrambledArchive.cpp only!
 *
 * @file
 *  ArchiveRegistry.h
 * @author
 *  Carsten Wenzel
 * @date
 *  01.02.2002
 * @brief
 *  Declaration and implementation of e\ ScrambledArchive::CArchiveRegistry. 
 *  For internal use in ScrambledArchive.cpp only!
 */



#ifndef _ARCHIVEREGISTRY_H_INCLUDED_
#define _ARCHIVEREGISTRY_H_INCLUDED_

#pragma once



#include <string>
#include <map>



namespace ScrambledArchive
{



    /**
    * An archive registry singleton. Registers all information about an archive once it has been deciphered.    
    * This is to prevent archives from being deciphered twice when multiple CScrambledArchive 
    * instances of the same archive are created. At program exit it also auto-deletes the memory of archives 
    * that had to be copied from the executable's resource in order to decipher it because memory write-protection
    * could not be disabled (as it would be the case under Win98/ME).
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  An archive registry singleton.
    */
    class CArchiveRegistry
    {
    public:
        /**
        * Archive info structure.
        *
        * @author
        *  Carsten Wenzel
        * @date
        *  25.02.2002
        * @brief
        *  Archive info structure.
        */
        struct SArchiveInfo
        {
            /** 
            * Pointer to raw memory holding archive data.
            *
            * @brief
            *  Pointer to raw memory holding archive data.
            */
            unsigned char* m_pucRawData;

            /** 
            * Pointer to archive data. 
            *
            * @brief
            *  Pointer to archive data. 
            */
            unsigned char* m_pucArchive;

            /** 
            * Size of archive. 
            *
            * @brief
            *  Size of archive. 
            */
            unsigned int m_uiSizeOfArchive;

            /** 
            * Auto delete memory at #m_pucRawData when the archive registry is destroyed. 
            *
            * @brief
            *  Auto delete memory at #m_pucRawData when the archive registry is destroyed. 
            */
            bool m_bAutoDeleteMemory;
        };

        /** 
        * A type mapping archive names to archive information structures
        * 
        * @brief
        *  A type mapping archive names to archive information structures
        */
        typedef std::map< std::string, SArchiveInfo > ArchiveCollection;

    public:
        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CArchiveRegistry();

        /**
        * Returns the single instance of the archive registry.
        * 
        * @return
        *  The single instance of the archive registry.
        * @brief
        *  Returns the single instance of the archive registry.
        */
        static CArchiveRegistry& GetInstance();

        /**
        * Retrieves the archive information for an archive.
        * 
        * @param strArchiveName
        *  Name of the archive for which the archive information should be retrieved.
        * @param sArchiveInfo
        *  Structure to be filled with the archive information.
        * @return
        *  \b true if retrieval was successful else \b false (archive is not in registry).
        * @brief
        *  Retrieves the archive information for #strArchiveName.
        */
        bool GetArchive( const std::string& strArchiveName, SArchiveInfo& sArchiveInfo );

        /**
        * Registers an archive.
        * 
        * @param strArchiveName
        *  Name of the archive to be registered.
        * @param sArchiveInfo
        *  Structure with the archive information.
        * @return
        *  \b true if successfully registered else \b (archive has already been registered).
        * @brief
        *  Registers an archive.
        */
        bool RegisterArchive( const std::string& strArchiveName, const SArchiveInfo& sArchiveInfo );

    private:
        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CArchiveRegistry();

    private:
        /** 
        * A map holding the information of all registered archives.
        * 
        * @brief
        *  A map holding the information of all registered archives.
        */
        ArchiveCollection m_colArchives;
    };

    

    inline
    CArchiveRegistry::CArchiveRegistry()
    {
    }



    inline
    CArchiveRegistry::~CArchiveRegistry()
    {
        ArchiveCollection::iterator itCur( m_colArchives.begin() );
        ArchiveCollection::iterator itEnd( m_colArchives.end() );

        while( itCur != itEnd )
        {
            if( false != (*itCur).second.m_bAutoDeleteMemory )
            {
                delete (*itCur).second.m_pucRawData;
            }

            ++itCur;
        }
    }



    inline CArchiveRegistry& 
    CArchiveRegistry::GetInstance()
    {
        static CArchiveRegistry s_kInstance;
        return( s_kInstance );
    }



    inline bool
    CArchiveRegistry::GetArchive( const std::string& strArchiveName, SArchiveInfo& sArchiveInfo )
    {
        ArchiveCollection::iterator itLookup( m_colArchives.find( strArchiveName ) );
        if( itLookup != m_colArchives.end() )
        {
            sArchiveInfo = (*itLookup).second;
            return( true );
        }

        return( false );    
    }



    inline bool
    CArchiveRegistry::RegisterArchive( const std::string& strArchiveName, const SArchiveInfo& sArchiveInfo )
    {
        ArchiveCollection::iterator itLookup( m_colArchives.find( strArchiveName ) );
        if( itLookup != m_colArchives.end() )
        {
            return( false );
        }

        m_colArchives.insert( ArchiveCollection::value_type( strArchiveName, sArchiveInfo ) );
        return( true );    
    }



} // namespace ScrambledArchive



#endif // #ifndef _ARCHIVEREGISTRY_H_INCLUDED_
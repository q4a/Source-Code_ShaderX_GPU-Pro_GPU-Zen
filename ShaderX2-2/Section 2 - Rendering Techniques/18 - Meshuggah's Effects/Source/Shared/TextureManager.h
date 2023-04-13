///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Shared::CTextureManager.
 *
 * @file
 *  TextureManager.h
 * @author
 *  Carsten Wenzel
 * @date
 *  28.02.2002
 * @brief
 *  Declaration of \e Shared::CTextureManager.
 */



#ifndef _TEXTUREMANAGER_H_INCLUDED_
#define _TEXTUREMANAGER_H_INCLUDED_

#pragma once



#include <map>
#include <string>



namespace ScrambledArchive
{
    class CScrambledArchive;
}



namespace Shared
{



    /**
    * A D3D texture manager class. Responsible for loading / converting textures and giving shared access to them.
    * All texture are created using \c D3DPOOL_MANAGED. This means they don't have to be reloaded in case of a
    * device lost / reset scenario.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  28.02.2002
    * @brief
    *  A D3D texture manager class.
    */
    class CTextureManager
    {
    public:
        /**
        * Destructor.
        *
        * @brief
        *  Destructor.
        */
        ~CTextureManager();

        /**
        * Creates a instance of \e CTextureManager.
        *
        * @param pD3DDev
        *  Pointer to a D3D device interface. Used to create and load textures.
        * @param pkArchive
        *  Archive from which textures should be loaded.
        * @return
        *  \b true of succuessful else \b false.
        * @brief
        *  Creates a instance of \e CTextureManager.
        */
        static CTextureManager* CreateAndBindToArchive( IDirect3DDevice8* pD3DDev,
            const ScrambledArchive::CScrambledArchive* pkArchive );

        /**
        * Returns a texture from the texture manager. Its reference count is incremented upon return.
        *
        * @param strFileName
        *  Name of texture that should be retrieved.
        * @return
        *  Pointer to an \e IDirect3DBaseTexture8 interface representing the texture.
        *  \b NULL if texture was not found.
        * @brief
        *  Returns a texture from the texture manager.
        */
        IDirect3DBaseTexture8* GetTexture( const std::string& strFileName ) const;

        /**
        * Loads a DDS texture into the manager.
        *
        * @param strFileName
        *  Name of texture in #m_pkArchive that should be loaded.
        * @param bMipMaps
        *  Should mip maps be generated or used if they are already stored in the DDS file.
        * @return
        *  \b true if loading was successful else \b false.
        * @brief
        *  Loads a DDS texture into the manager.
        */
        bool LoadDDSTexture( const std::string& strFileName, bool bMipMaps );

        /**
        * Loads a JPEG texture into the manager.
        * An alpha channel is stored as a second image below the actual image.
        * It must have the width and height of the original.
        *
        * @param strFileName
        *  Name of texture in #m_pkArchive that should be loaded.
        * @param bAlpha
        *  Is an alpha image attached at the bottom of the texture.
        * @param bMipMaps
        *  Should mip maps be generated.
        * @param bCompressed
        *  Store as texture using a compressed format.
        *  \b DXT3 is used if texture contains an alpha channel; \b DXT1 if not.
        * @return
        *  \b true if loading was successful else \b false.
        * @brief
        *  Loads a JPEG texture into the manager.
        */
        bool LoadJPEGTexture( const std::string& strFileName,
                              bool bAlpha, bool bMipMaps, bool bCompressed );

        /**
        * Loads a JPEG cube texture into the manager.
        * An alpha channel is stored interleaved as a second image below the actual image for each face.
        * It must have the same dimension as the original. The sequential order is as follows:
        * ( RGB_FACE0, ALPHA_FACE0 ), ( RGB_FACE1, ALPHA_FACE1 ), ... ( RGB_FACE5, ALPHA_FACE5 )
        *
        * @param strFileName
        *  Name of texture in #m_pkArchive that should be loaded.
        * @param bAlpha
        *  Is an alpha image attached at the bottom of each face of the cube texture.
        * @param bMipMaps
        *  Should mip maps be generated.
        * @param bCompressed
        *  Store as texture using a compressed format.
        *  \b DXT3 is used if texture contains an alpha channel; \b DXT1 if not.
        * @return
        *  \b true if loading was successful else \b false.
        * @brief
        *  Loads a JPEG cube texture into the manager.
        */
        bool LoadJPEGCubeTexture( const std::string& strFileName,
                                  bool bAlpha, bool bMipMaps, bool bCompressed );

    private:
        /**
        * A type mapping names to textures.
        *
        * @brief
        *  A type mapping names to textures.
        */
        typedef std::map< std::string, IDirect3DBaseTexture8* > TextureCatalog;

        /**
        * A structure used in conjuction with STL's \c for_each algorithm
        * to conveniently release textures from the manager's internal map.
        *
        * @author
        *  Carsten Wenzel
        * @date
        *  28.02.2002
        * @brief
        *  A structure used in conjuction with STL's \c for_each algorithm
        *  to conveniently release textures from the manager's internal map.
        */
        struct ReleaseTextures
        {
            /**
            * Calls \c IUnknown::Release() on \e val.second.
            *
            * @brief
            *  Calls \c IUnknown::Release() on \e val.second.
            */
            void operator() ( TextureCatalog::value_type& val );
        };

    private:
        /**
        * Constructor.
        *
        * @param pD3DDev
        *  Pointer to a D3D device. Used to create and load textures.
        * @param pkArchive
        *  Archive from which textures should be loaded.
        * @brief
        *  Constructor.
        */
        CTextureManager( IDirect3DDevice8* pD3DDev,
            const ScrambledArchive::CScrambledArchive* pkArchive );

        /**
        * Indicates if a texture is already loaded/registered in the manager.
        *
        * @param strFileName
        *  Name of the texture to check.
        * @return
        *  \b true if texture is already loaded/registered else \b false.
        * @brief
        *  Indicates if a texture is already registered in the manager.
        */
        bool IsTextureRegistered( const std::string& strFileName ) const;

        /**
        * Registers a texture in the manager.
        *
        * @param strFileName
        *  Name of the texture to be registered.
        * @param pkTexture
        *  Pointer to an \e IDirect3DBaseTexture8 interface representing the texture.
        * @brief
        *  Registers a texture in the manager.
        */
        void RegisterTexture( const std::string& strFileName, IDirect3DBaseTexture8* pkTexture );

        /**
        * Data archive from which texture will be loaded.
        *
        * @brief
        *  Data archive from which texture will be loaded.
        */
        const ScrambledArchive::CScrambledArchive* m_pkArchive;

        /**
        * Pointer to a D3D device interface. Used to create and load textures.
        *
        * @brief
        *  Pointer to a D3D device interface. Used to create and load textures.
        */
        IDirect3DDevice8* m_pD3DDev;

        /**
        * Map for the texture catalog. Used to bind texture to names.
        *
        * @brief
        *  Map for the texture catalog.
        */
        TextureCatalog m_colTextures;

    private:
        /**
        * Copy constructor. Not defined!!!
        *
        * @brief
        *  Copy constructor. Not defined!!!
        */
        CTextureManager( const CTextureManager& kRhs );

        /**
        * Assignment operator. Not defined!!!
        *
        * @brief
        *  Assignment operator. Not defined!!!
        */
        CTextureManager& operator =( const CTextureManager& kRhs );
    };



#include "TextureManager.inl"



} // namespace Shared



#endif // #ifndef _TEXTUREMANAGER_H_INCLUDED_
///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CTextureManager (inline methods).
 *
 * @file
 *  TextureManager.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  28.02.2002
 * @brief
 *  Implementation of \e Shared::CTextureManager (inline methods).
 */



#ifndef _TEXTUREMANAGER_INL_INCLUDED_
#define _TEXTUREMANAGER_INL_INCLUDED_

#pragma once



inline CTextureManager* 
CTextureManager::CreateAndBindToArchive( IDirect3DDevice8* pD3DDev, const ScrambledArchive::CScrambledArchive* pkArchive )
{
    DBG_ASSERT( 0 != pD3DDev );
    DBG_ASSERT( 0 != pkArchive );

    if( 0 == pD3DDev || 0 == pkArchive )
    {
        return( 0 );
    }

    return( new CTextureManager( pD3DDev, pkArchive) );
}



inline IDirect3DBaseTexture8* 
CTextureManager::GetTexture( const std::string& strFileName ) const
{
    TextureCatalog::const_iterator itLookup( m_colTextures.find( strFileName ) );
    if( m_colTextures.end() != itLookup )
    {
        IDirect3DBaseTexture8* pkTexture( ( *itLookup ).second );
        pkTexture->AddRef();
        return( pkTexture );
    }

    return( 0 );
}



inline bool 
CTextureManager::IsTextureRegistered( const std::string& strFileName ) const
{
    TextureCatalog::const_iterator itLookup( m_colTextures.find( strFileName ) );
    if( m_colTextures.end() != itLookup )
    {
        if( 0 != (*itLookup).second )
        {
            return( true );
        }
    }

    return( false );
}
    


inline void 
CTextureManager::RegisterTexture( const std::string& strFileName, IDirect3DBaseTexture8* pkTexture )
{
    DBG_ASSERT( false == IsTextureRegistered( strFileName ) );
    DBG_ASSERT( 0 != pkTexture );

    m_colTextures.insert( TextureCatalog::value_type( strFileName, pkTexture ) );    
}



inline void
InitRect( RECT& rect, unsigned int uiWidth, unsigned int uiHeight )
{
    rect.left   = 0;
    rect.top    = 0;
    rect.right  = uiWidth;
    rect.bottom = uiHeight;
}



inline D3DFORMAT 
ChooseTextureFormat( bool bAlpha, bool bCompressed )
{
    D3DFORMAT eTextureFormat( D3DFMT_A8R8G8B8 );

    if( false != bCompressed  )
    {
        if( false != bAlpha )
        {
            eTextureFormat = D3DFMT_DXT3;
        }
        else
        {
            eTextureFormat = D3DFMT_DXT1;
        }
    }

    return( eTextureFormat );
}



inline void
CTextureManager::ReleaseTextures::operator ()( CTextureManager::TextureCatalog::value_type& val )
{
    HRESULT hr( val.second->Release() );
    DBG_ASSERTM( hr == 0, "Texture is still being referenced!" );
}



#endif // #ifndef _TEXTUREMANAGER_INL_INCLUDED_
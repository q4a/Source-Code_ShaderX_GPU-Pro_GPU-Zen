///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e EffectSystem::TEffectFactory (inline methods).
 * 
 * @file
 *  EffectFactory.inl
 * @author
 *  Carste Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e EffectSystem::TEffectFactory (inline methods).
 */



#ifndef _EFFECTFACTORY_INL_INCLUDED_
#define _EFFECTFACTORY_INL_INCLUDED_

#pragma once



template< typename T >
inline IEffect*
TEffectFactory< T >::CreateInstance( IDirect3DDevice8* pD3DDev, 
                                     ScrambledArchive::CScrambledArchive* pkScrambledArchive,
                                     Shared::CTextureManager* pkTextureManager )
{
    T* pkEffect( 0 );

    try
    {
        // effects created by using a factory are 
        // always supposed to run in browser mode!
        pkEffect = new T( pD3DDev, pkScrambledArchive, pkTextureManager, 0, true );
    }
    catch( ... )
    {
    }

    return( pkEffect );
}



template< typename T >
inline const char* const
TEffectFactory< T >::GetEffectName()
{
    return( T::GetEffectName() );
}



#endif // #ifndef _EFFECTFACTORY_INL_INCLUDED_

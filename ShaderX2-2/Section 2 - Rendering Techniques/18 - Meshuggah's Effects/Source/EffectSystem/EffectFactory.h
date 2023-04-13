///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e EffectSystem::TEffectFactory.
 *
 * @file
 *  EffectFactory.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Declaration of \e EffectSystem::TEffectFactory.
 */



#ifndef _EFFECTFACTORY_H_INCLUDED_
#define _EFFECTFACTORY_H_INCLUDED_

#pragma once



namespace ScrambledArchive
{
    class CScrambledArchive;
}

namespace Shared
{
    class CTextureManager;
}



namespace EffectSystem
{



    class IEffect;



    /**
    * An abstract effect factory base class.
    * This allows to store several different effect factories in the same container.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  09.11.2001
    * @brief
    *  An abstract effect factory base class.
    */
    class CEffectFactory
    {
    public:
        /**
        * Creates an instance of an effect ready to use for meshuggah browser mode.
        *
        * @brief
        *  Creates an instance of an effect ready to use for meshuggah browser mode.
        * @param pD3DDev
        *  Pointer to a D3D device interface.
        * @param pkScrambledArchive
        *  Pointer to a data archive. Enables the effect to load additional data.
        * @param pkTextureManager
        *  Pointer to a texture manager. Enables the effect to load/get static textures.
        * @return
        *  Pointer to the created effect. \b NULL if creation failed.
        */
        virtual IEffect* CreateInstance( IDirect3DDevice8* pD3DDev,
                                         ScrambledArchive::CScrambledArchive* pkScrambledArchive,
                                         Shared::CTextureManager* pkTextureManager ) = 0;
    };



    /**
    * An effect factory template. It is used to simply the creation of effect in when running
    * Meshuggah's browser mode.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  A effect factory template.
    */
    template< class T >
    class TEffectFactory : public CEffectFactory
    {
    public:
        /**
        * Creates an instance of an effect of type \e T.
        *
        * @brief
        *  Creates an instance of an effect of type \e T.
        * @see
        *  EffectSystem::CEffectFactory::CreateInstance()
        */
        virtual IEffect* CreateInstance( IDirect3DDevice8* pD3DDev,
                                         ScrambledArchive::CScrambledArchive* pkScrambledArchive,
                                         Shared::CTextureManager* pkTextureManager );

        /**
        * Returns the name of an effect of type \e T.
        *
        * @brief
        *  Returns the name of an effect of type \e T.
        * @return
        *  The name of an effect of type \e T.
        */
        static const char* const GetEffectName();
    };



#include "EffectFactory.inl"



} // namespace EffectSystem



#endif // #ifndef _EFFECTFACTORY_H__INCLUDED_

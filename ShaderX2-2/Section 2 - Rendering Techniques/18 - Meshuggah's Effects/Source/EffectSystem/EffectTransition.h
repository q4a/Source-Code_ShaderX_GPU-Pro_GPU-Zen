///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declartion of \e EffectSystem::CEffectTransition.
 *
 * @file
 *  EffectTransition.h
 * @author
 *  Carsten Wenzel
 * @date
 *  26.01.2002
 * @brief
 *  Declartion of \e EffectSystem::CEffectTransition.
 */



#ifndef _EFFECTTRANSITION_H_INCLUDED_
#define _EFFECTTRANSITION_H_INCLUDED_

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



    /**
    * An effect transition manager.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  26.01.2002
    * @brief
    *  An effect transition manager.
    */
    class CEffectTransition
    {
    public:
        /** 
        * Filepath to effect transition texture 0.
        * 
        * @brief
        *  Filepath to effect transition texture 0.
        */
        static const char* sc_pcTransitionTexture0;

        /** 
        * Filepath to effect transition texture 1.
        * 
        * @brief
        *  Filepath to effect transition texture 1.
        */
        static const char* sc_pcTransitionTexture1;

        /** 
        * Filepath to effect transition texture 2.
        * 
        * @brief
        *  Filepath to effect transition texture 2.
        */
        static const char* sc_pcTransitionTexture2;

        /** 
        * Filepath to effect transition texture 3.
        * 
        * @brief
        *  Filepath to effect transition texture 3.
        */
        static const char* sc_pcTransitionTexture3;

    public:
        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CEffectTransition();

        /**
        * Creates an effect transition object.
        * 
        * @param pD3DDev
        *  Pointer to a D3D device interface.
        * @param pkScrambledArchive
        *  Pointer to a data archive to load transition textures from.
        * @param pkTextureManager
        *  Pointer to a shared texture pool.
        * @return
        *  An instance of CEffectTransition. Returns \b NULL if creation failed!
        * @brief
        *  Creates an effec transition object.
        */
        static CEffectTransition* Create( IDirect3DDevice8* pD3DDev, 
                                          ScrambledArchive::CScrambledArchive* pkScrambledArchive,
                                          Shared::CTextureManager* pkTextureManager );

        /**
        * Fades the content of the current backbuffer to the given color using the given alpha values and times.
        * Should be called inbetween BeginScene()/EndScene().
        *
        * @param fTime
        *  Time.
        * @param fBlendStart
        *  Time when blend effect starts.
        * @param ucAStart
        *  Alpha value at fBlendStart.
        * @param fBlendEnd
        *  Time when blend effect ends.
        * @param ucAEnd
        *  Alpha value at fBlendEnd.
        * @param ucR
        *  R value of color to blend to.
        * @param ucG
        *  G value of color to blend to.
        * @param ucB
        *  B value of color to blend to.
        * @brief
        *  Fades the content of the current backbuffer to the given color using the given alpha values and times.
        */
        void ColorBlend( float fTime,
                         float fBlendStart, unsigned char ucAStart,
                         float fBlendEnd, unsigned char ucAEnd,
                         unsigned char ucR, unsigned char ucG, unsigned char ucB );

        /**
        * Fades the content of the current backbuffer to the given color using the given transition texture,
        * alpha values and times. Should be called inbetween BeginScene()/EndScene().
        *
        * @param fTime
        *  Time.
        * @param fBlendStart
        *  Time when blend effect starts.
        * @param ucAStart
        *  Alpha value at fBlendStart.
        * @param fBlendEnd
        *  Time when blend effect ends.
        * @param ucAEnd
        *  Alpha value at fBlendEnd.
        * @param ucR
        *  R value of color to blend to.
        * @param ucG
        *  G value of color to blend to.
        * @param ucB
        *  B value of color to blend to.
        * @param pcTransitionTexture
        *  Filepath of transition texture to use.
        * @brief
        *  Fades the content of the current backbuffer to the given color using the given alpha values and times.
        */
        void ColorBlendTexture( float fTime,
                                float fBlendStart, unsigned char ucAStart,
                                float fBlendEnd, unsigned char ucAEnd,
                                unsigned char ucR, unsigned char ucG, unsigned char ucB,
                                const char* const pcTransitionTexture );

    private:
        /**
        * Constructor.
        * 
        * @param pD3DDev
        *  Pointer to a D3D device interface.
        * @param pkScrambledArchive
        *  Pointer to a data archive to load transition textures from.
        * @param pkTextureManager
        *  Pointer to a shared texture pool.
        * @brief
        *  Constructor.
        */
        CEffectTransition( IDirect3DDevice8* pD3DDev, 
                           ScrambledArchive::CScrambledArchive* pkScrambledArchive,
                           Shared::CTextureManager* pkTextureManager );

        /**
        * Frees all allocated resources.
        * 
        * @brief
        *  Frees all allocated resources.
        */
        void FreeAllResources();

    private:
        /** 
        * Pointer to a D3D device interface.
        * 
        * @brief
        *  Pointer to a D3D device interface.
        */
        IDirect3DDevice8* m_pD3DDev;

        /** 
        * Pointer to a data archive to load transition textures from.
        * 
        * @brief
        *  Pointer to a data archive to load transition textures from.
        */
        ScrambledArchive::CScrambledArchive* m_pkScrambledArchive;

        /** 
        * Pointer to a shared texture pool.
        * 
        * @brief
        *  Pointer to a shared texture pool.
        */
        Shared::CTextureManager* m_pkTextureManager;

        /** 
        * Handle to the effect transition pixel shader.
        * 
        * @brief
        *  Handle to the effect transition pixel shader.
        */
        DWORD m_dwPSHandle;
    };



} // namespace EffectSystem



#endif // #ifndef _EFFECTTRANSITION_H_INCLUDED_
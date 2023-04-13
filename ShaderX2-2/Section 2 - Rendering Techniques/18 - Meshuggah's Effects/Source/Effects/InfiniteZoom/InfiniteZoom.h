///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Definition of \e Effects::CInfiniteZoom.
 *
 * @file
 *  InfiniteZoom.h
 * @author
 *  Carsten Wenzel
 * @date
 *  25.02.2002
 * @brief
 *  Definition of \e Effects::CInfiniteZoom.
 */



#ifndef _INFINITEZOOM_H_INCLUDED_
#define _INFINITEZOOM_H_INCLUDED_

#pragma once



#include "Effect.h"
#include <vector>



namespace Effects
{



    class CInfiniteZoomDialog;



    /**
    * <em>Infinite zoom</em> effect class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  <em>Infinite zoom</em> effect class.
    */
    class CInfiniteZoom : public EffectSystem::CEffect
    {
    public:
        enum ENumZoomKeys
        {
            /** 
            * Number of zoom keys.
            */
            NUM_ZOOM_KEYS = 10
        };

    public:
        /**
        * Constructor.
        * 
        * @param pD3DDev
        *  Pointer to a D3D device interface.
        * @param pkScrambledArchive
        *  Pointer to a data archive. Enables the effect to load additional data.
        * @param pkTextureManager
        *  Pointer to a texture manager. Enables the effect to load/get static textures.
        * @param pkEffectTransition
        *  Pointer to an effect transition manager.
        * @param bBrowserMode
        *  If set to \b true then an effect parameters dialog will be created.
        * @throws EffectSystem::CEffectException
        *  If required device capabilitiesare unsupported or effect resources could not be loaded.
        *  For further error information check thrown exception.
        * @brief
        *  Constructor.
        */
        CInfiniteZoom( IDirect3DDevice8* pD3DDev, 
                       ScrambledArchive::CScrambledArchive* pkScrambledArchive,
                       Shared::CTextureManager* pkTextureManager,
                       EffectSystem::CEffectTransition* pkEffectTransition,
                       bool bBrowserMode );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CInfiniteZoom();

        /**
        * Returns the name of the effect. EffectSystem::TEffectFactory relies on this method being 
        * implemented by each effect class.
        * 
        * @return
        *  Name of the effect.
        * @brief
        *  Returns the name of the effect.
        */
        static const char* const GetEffectName();

    public:
        virtual void InitAppManagedResources();
        virtual void FreeAppManagedResources();

        virtual void Update( float fTime = -1.0f );

        virtual bool GetShaderSources( EffectSystem::SShaderSourceList& colShaders );

        virtual bool IsEffectDialogVisible();
        virtual void ShowEffectDialog( bool bShow );

        virtual unsigned int GetHTMLHelpID();

    protected:
        virtual void ConfirmRequirements();
        virtual void FreeAllResources();

    private:
        /** 
        * Structure for a vertex used to render the zoom.
        * 
        * @brief
        *  Structure for a vertex used to render the zoom.
        */
        struct SVertex
        {
            /** 
            * Position.
            * 
            * @brief
            *  Position.
            */
            D3DXVECTOR3 vPos;

            /** 
            * 2D texture coordinate.
            * 
            * @brief
            *  2D texture coordinate.
            */
            D3DXVECTOR2 vTex;
        };

    private:
        /**
        * Sets the shader constants for <em>Infinite zoom</em> shader.
        * 
        * @brief
        *  Sets the shader constants for <em>Infinite zoom</em> shader.
        */
        void SetShaderConstants();

        /**
        * Renders the credits if effect is running in demo mode.
        * 
        * @param fTime
        *  Current effect relative time.
        * @brief
        *  Renders the credits if effect is running in demo mode.
        */
        void RenderCredits( float fTime );

    private:
        /** 
        * Vertex buffer for all quads of zoom.
        * 
        * @brief
        *  Vertex buffer for all quads of zoom.
        */
        IDirect3DVertexBuffer8* m_pkVertexBuffer;

        /** 
        * Pointer to effect settings dialog in browser mode.
        * 
        * @brief
        *  Pointer to effect settings dialog in browser mode.
        */
        CInfiniteZoomDialog* m_pkZoomSettingsDlg;

        /** 
        * <em>Infinite zoom</em> vertex shader handle.
        * 
        * @brief
        *  <em>Infinite zoom</em> vertex shader handle.
        */
        DWORD m_dwVSHandle;

        /** 
        * <em>Infinite zoom</em> pixel shader handle.
        * 
        * @brief
        *  <em>Infinite zoom</em> pixel shader handle.
        */
        DWORD m_dwPSHandle;

        /** 
        * Bass spectrum used to synch the effect to music in demo mode.
        * 
        * @brief
        *  Bass spectrum used to synch the effect to music in demo mode.
        */
        std::vector< unsigned char > m_colBassSpectrum;
    };



} // namespace Effects



#endif // #ifndef _INFINITEZOOM_H_INCLUDED_
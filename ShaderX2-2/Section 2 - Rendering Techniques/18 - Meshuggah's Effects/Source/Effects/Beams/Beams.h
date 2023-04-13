///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::Beams.
 *
 * @file
 *  Beams.h
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Declaration of \e Effects::Beams.
 */



#ifndef _BEAMS_H_INCLUDED_
#define _BEAMS_H_INCLUDED_

#pragma once



#include "Effect.h"



namespace Effects
{



    class CBeamsDialog;



    /**
    * \e Beams effect class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  04.03.2002
    * @brief
    *  \e Beams effect class.
    */
    class CBeams : public EffectSystem::CEffect
    {
    public:
        enum ENumSlices
        {
            /** 
            * Number of steps for radial blur.
            */
            NUM_BLURSTEPS = 64
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
        *  If required device capabilities are unsupported or effect resources could not be loaded.
        *  For further error information check thrown exception.
        * @brief
        *  Constructor.
        */
        CBeams( IDirect3DDevice8* pD3DDev, 
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
        virtual ~CBeams();

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
        * Structure for a transformed vertex used to render the radial blur / a screen quad.
        * 
        * @brief
        *  Structure for a transformed vertex used to render the radial blur / a screen quad.
        */
        struct SVertex
        {
            /** 
            * Transformed position in texture or on screen.
            * 
            * @brief
            *  Transformed position in texture or on screen.
            */
            D3DXVECTOR4 vPos;

            /** 
            * 2D texture coordinate.
            * 
            * @brief
            *  2D texture coordinate.
            */
            D3DXVECTOR2 vTex;
        };

        /** 
        * Structure for a sphere vertex used to render the sun.
        * 
        * @brief
        *  Structure for a sphere vertex used to render the sun.
        */
        struct SSphereVertex
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
        * Sets shader constants for <em>Render sun</em> shader.
        * 
        * @param fTime
        *  Current effect relative time (in seconds).
        * @brief
        *  Sets shader constants for <em>Render sun</em> shader.
        */
        void SetRenderSunShaderConstants( float fTime );

        /** 
        * Renders a sphere. Uses #m_pkSphere.
        * 
        * @brief
        *  Renders a sphere.
        */
        void RenderSphere();
        
        /**
        * Renders the sun into #m_pkSunRenderTexture.
        * 
        * @param fTime
        *  Current effect relative time (in seconds).
        * @brief
        *  Renders the sun.
        */
        void RenderSun( float fTime );

        /** 
        * Creates the sun beams by rendering a radial blur 
        * using #m_pkSunRenderTexture into #m_pkBeamsRenderTexture.
        * 
        * @brief
        *  Creates the sun beams.
        */
        void RenderBeams();

        /** 
        * Draws beams from #m_pkBeamsRenderTexture into back buffer.
        * 
        * @brief
        *  Draws beams.
        */
        void DrawBeams();

        /** 
        * Draws stars backdrop.
        * 
        * @brief
        *  Draws stars backdrop.
        */
        void DrawBackdrop();

    private:
        /** 
        * Sphere mesh. Used to render stars backdrop and sun surface.
        * 
        * @brief
        *  Sphere mesh.
        */
        ID3DXMesh* m_pkSphere;

        /** 
        * Render texture for sun surface.
        * Used as source texture for radial blur.
        * 
        * @brief
        *  Render texture for stars backdrop and sun surface.
        */
        IDirect3DTexture8* m_pkSunRenderTexture;

        /** 
        * Render texture for sun beams.
        * Used as destintation texture for radial blur.
        *
        * @brief
        *  Render texture for sun beams.
        */
        IDirect3DTexture8* m_pkBeamsRenderTexture;

        /** 
        * Vertex buffer for radial blur / a screen quad.
        * 
        * @brief
        *  Vertex buffer for radial blur / a screen quad.
        */
        IDirect3DVertexBuffer8* m_pkVertexBuffer;

        /** 
        * Pointer to effect settings dialog in browser mode.
        * 
        * @brief
        *  Pointer to effect settings dialog in browser mode.
        */
        CBeamsDialog* m_pkBeamSettingsDlg;

        /** 
        * <em>Render sun</em> pixel shader handle.
        * 
        * @brief
        *  <em>Render sun</em> pixel shader handle.
        */
        DWORD m_dwRenderSunPSHandle;

        /** 
        * <em>Render sun</em> vertex shader handle.
        * 
        * @brief
        *  <em>Render sun</em> vertex shader handle.
        */
        DWORD m_dwRenderSunVSHandle;

        /** 
        * \e Beams pixel shader handle.
        * 
        * @brief
        *  \e Beams pixel shader handle.
        */
        DWORD m_dwBeamsPSHandle;
    };



} // namespace Effects



#endif // #ifndef _BEAMS_H_INCLUDED_
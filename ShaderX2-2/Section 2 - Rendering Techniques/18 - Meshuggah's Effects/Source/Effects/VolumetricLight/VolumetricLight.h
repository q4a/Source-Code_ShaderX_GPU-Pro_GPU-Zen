///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CVolumetricLight.
 *
 * @file
 *  VolumetricLight.h
 * @author
 *  Carsten Wenzel
 * @date
 *  05.03.2002
 * @brief
 *  Declaration of \e Effects::CVolumetricLight.
 */



#ifndef _VOLUMETRICLIGHT_H_INCLUDED_
#define _VOLUMETRICLIGHT_H_INCLUDED_

#pragma once



#include "Effect.h"
#include <vector>



namespace Effects
{



    class CVolumetricLightDialog;



    /**
    * <em>Volumetric ligth</em> effect class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  04.03.2002
    * @brief
    *  <em>Volumetric ligth</em> effect class.
    */
    class CVolumetricLight : public EffectSystem::CEffect
    {
    public:
        enum EVolumetricLightEnums
        {
            /** 
            * Number of beams per dimension. Represents the resolution of a square light map.
            */
            NUM_BEAMS_PER_DIM = 32,

            /** 
            * Number of beams for volumetric light. Equal to <tt>NUM_BEAMS_PER_DIM * NUM_BEAMS_PER_DIM</tt>.
            */
            NUM_BEAMS         = 1024 // NUM_BEAMS_PER_DIM * NUM_BEAMS_PER_DIM
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
        CVolumetricLight( IDirect3DDevice8* pD3DDev, 
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
        virtual ~CVolumetricLight();

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

    public:
        /** 
        * Default light intensity.
        * 
        * @brief
        *  Default light intensity.
        */
        static const float sc_fDefaultLightIntensity;

        /** 
        * Default z position of light behind x/y plane.
        * 
        * @brief
        *  Default z position of light behind x/y plane.
        */
        static const float sc_fDefaultLightZPos;

    protected:
        virtual void ConfirmRequirements();
        virtual void FreeAllResources();

    private:
        /** 
        * Structure for a vertex of a light beam.
        * 
        * @brief
        *  Structure for a vertex of a light beam.
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
            * Light color (from light map) times intensity.
            * 
            * @brief
            *  Light color (from light map) times intensity.
            */
            DWORD dwColor;

            /** 
            * 1D texture coordinate for beam texture.
            * 
            * @brief
            *  1D texture coordinate for beam texture.
            */
            float vTex;
        };

        /** 
        * Structure for a vertex used to render the text plane.
        * 
        * @brief
        *  Structure for a vertex used to render the text plane.
        */
        struct STextPlaneVertex
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
            *  2D texture coordinate
            */
            D3DXVECTOR2 vTex;
        };

        /** 
        * Structure for a transformed vertex used to render screen quads.
        * 
        * @brief
        *  Structure for a transformed vertex used to render screen quads.
        */
        struct SQuadVertex
        {
            /** 
            * Screen position.
            * 
            * @brief
            *  Screen position.
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
        * Structure of a light beam.
        * 
        * @brief
        *  Structure of a light beam.
        */
        struct SLightBeam
        {
            /** 
            * Direction of beam.
            * 
            * @brief
            *  Direction of beam.
            */
            D3DXVECTOR3 vDir;

            /** 
            * Color of beam from light map.
            * 
            * @brief
            *  Color of beam from light map.
            */
            unsigned int uiColor;

            /** 
            * Vector used to form a quad representing the ligth beam.
            * 
            * @brief
            *  Vector used to form a quad representing the ligth beam.
            */
            D3DXVECTOR3 vWidth;
            
            /** 
            * x coordinate of current intersection point with text (x/y) plane.
            * 
            * @brief
            *  x coordinate of current intersection point with text (x/y) plane.
            */
            float fX;

            /** 
            * y coordinate of current intersection point with text (x/y) plane.
            * 
            * @brief
            *  y coordinate of current intersection point with text (x/y) plane.
            */
            float fY;

            /** 
            * Light intensity at current intersection point with text (x/y) plane.
            * 
            * @brief
            *  Light intensity at current intersection point with text (x/y) plane.
            */
            float fIntensity;
        };

    private:
        /**
        * Sets shader constants for <em>Volumetric light</em> and <em>Text plane</em> shader.
        * 
        * @brief
        *  Sets shader constants for <em>Volumetric light</em> and <em>Text plane</em> shader.
        */
        void SetShaderConstants();

        /** 
        * Modulates a given intensity value to a color.
        * 
        * @brief
        *  Modulates a given intensity value to a color.
        */
        unsigned int GetColorIntensity( unsigned int uiColor, float fIntensity ) const;

        /** 
        * Renders light beams to a texture.
        * 
        * @param fTime
        *  Current effect relative time (in seconds).
        * @brief
        *  Renders light beams to a texture.
        */
        void RenderBeamsToTexture( float fTime );        

        /** 
        * Draws beams from #m_pkRenderTexture into back buffer.
        * 
        * @brief
        *  Draws beams.
        */
        void DrawBeams();

    private:
        /** 
        * Vertex buffer for light beams.
        * 
        * @brief
        *  Vertex buffer for light beams.
        */
        IDirect3DVertexBuffer8* m_pkVertexBuffer;

        /** 
        * Index buffer for light beams.
        * 
        * @brief
        *  Index buffer for light beams.
        */
        IDirect3DIndexBuffer8* m_pkIndexBuffer;

        /** 
        * Vertex buffer for text plane.
        * 
        * @brief
        *  Vertex buffer for text plane.
        */
        IDirect3DVertexBuffer8* m_pkTextPlaneVertexBuffer;

        /** 
        * Vertex buffer for a screen quad.
        * 
        * @brief
        *  Vertex buffer for a screen quad.
        */
        IDirect3DVertexBuffer8* m_pkQuadVertexBuffer;

        /** 
        * Render texture for light beams.
        * 
        * @brief
        *  Render texture for light beams.
        */
        IDirect3DTexture8* m_pkRenderTexture;

        /** 
        * Pointer to effect settings dialog in browser mode.
        * 
        * @brief
        *  Pointer to effect settings dialog in browser mode.
        */
        CVolumetricLightDialog* m_pkVolumetricLightSettingsDlg;

        /** 
        * Current global light intensity.
        * 
        * @brief
        *  Current global light intensity.
        */
        float m_fLightIntensity;        

        /** 
        * Current z position of light.
        * 
        * @brief
        *  Current z position of light.
        */
        float m_fLightZPos;

        /** 
        * <em>Volumetric light</em> pixel shader handle.
        * 
        * @brief
        *  <em>Volumetric light</em> pixel shader handle.
        */
        DWORD m_dwVolumetricLightPSHandle;

        /** 
        * <em>Volumetric light</em> vertex shader handle.
        * 
        * @brief
        *  <em>Volumetric light</em> vertex shader handle.
        */
        DWORD m_dwVolumetricLightVSHandle;

        /** 
        * <em>Text plane</em> pixel shader handle.
        * 
        * @brief
        *  <em>Text plane</em> pixel shader handle.
        */
        DWORD m_dwTextPlanePSHandle;

        /** 
        * <em>Text plane</em> vertex shader handle.
        * 
        * @brief
        *  <em>Text plane</em> vertex shader handle.
        */
        DWORD m_dwTextPlaneVSHandle;

        /** 
        * Map with bitmap text to determine the result of a <em>ray hits text plane<em> test.
        * 
        * @brief
        *  Map with bitmap text to determine the result of a <em>ray hits text plane<em> test.
        */
        std::vector< unsigned char > m_colTextMap;               

        /** 
        * Bass spectrum used to synch the effect to music in demo mode.
        * 
        * @brief
        *  Bass spectrum used to synch the effect to music in demo mode.
        */
        std::vector< unsigned char > m_colBassSpectrum;

        /** 
        * Array of beams for volumetric light.
        * 
        * @brief
        *  Array of beams for volumetric light.
        */
        SLightBeam m_pLightBeams[ NUM_BEAMS ]; 
    };



} // namespace Effects



#endif // #ifndef _VOLUMETRICLIGHT_H_INCLUDED_
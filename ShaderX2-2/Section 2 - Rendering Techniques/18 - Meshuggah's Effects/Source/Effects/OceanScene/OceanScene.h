///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::COceanScene.
 *
 * @file
 *  OceanScene.h
 * @author
 *  Carsten Wenzel
 * @date
 *  07.03.2002
 * @brief
 *  Declaration of \e Effects::COceanScene.
 */



#ifndef _OCEANSCENE_H_INCLUDED_
#define _OCEANSCENE_H_INCLUDED_

#pragma once



#include "Effect.h"
#include "OceanWaveGenerator.h"



namespace Effects
{



    class COceanSceneDialog;



    /**
    * <em>Ocean scene</em> effect class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  07.03.2002
    * @brief
    *  <em>Ocean scene</em> effect class.
    */
    class COceanScene : public EffectSystem::CEffect
    {
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
        COceanScene( IDirect3DDevice8* pD3DDev, 
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
        virtual ~COceanScene();

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
        * Default value for dynamic range of environment map.
        * 
        * @brief
        *  Default value for dynamic range of environment map.
        */
        static const float sc_fDefaultDynamicRangeOfEnvMap;

    protected:
        virtual void ConfirmRequirements();
        virtual void FreeAllResources();

    private:
        /** 
        * Structure of a sphere vertex used to render the sky.
        * 
        * @brief
        *  Structure of a sphere vertex used to render the sky.
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

        /** 
        * Structure of a ocean mesh vertex.
        * 
        * @brief
        *  Structure of a ocean mesh vertex.
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
            * Normal.
            * 
            * @brief
            *  Normal.
            */
            D3DXVECTOR3 vNormal;

            /** 
            * 2D texture coordinate for bump map.
            * 
            * @brief
            *  2D texture coordinate for bump map.
            */
            D3DXVECTOR2 vTex;
        };

    private:
        /**
        * Sets shader constants for \e Sky shader. 
        * 
        * @brief
        *  Sets shader constants for \e Sky shader.
        */
        void SetSkyShaderConstants();

        /** 
        * Renders a sphere. Uses #m_pkSphere.
        * 
        * @brief
        *  Renders a sphere.
        */
        void RenderSphere();

        /** 
        * Renders sky.
        * 
        * @brief
        *  Renders sky.
        */
        void RenderSky();
        
        /** 
        * Updates bump map.
        * 
        * @brief
        *  Updates bump map.
        */
        void BuildBumpMap();

        /**
        * Sets shader constants for \e Ocean shader. 
        * 
        * @param x
        *  Tile index (x) for ocean mesh.
        * @param z
        *  Tile index (z) for ocean mesh.
        * @brief
        *  Sets shader constants for \e Ocean shader.
        */
        void SetOceanShaderConstants( int x, int z );

        /**
        * Helper function to calculate coordinates of a ocean mesh vertex.
        * 
        * @param i
        *  x or z index in ocean height map.
        * @param fDisplace
        *  Displacement value for <em>choppy waves</em>.
        * @return
        *  Coordinate of ocean mesh (either x or z).
        * @brief
        *  Helper function to calculate coordinates of a ocean mesh vertex.
        */
        float GetCoord( int i, float fDisplace );

        /**
        * Renders ocean mesh.
        * 
        * @param fTime
        *  Current effect relative time (in seconds).
        * @brief
        *  Renders ocean mesh.
        */
        void RenderOcean( float fTime );

        /**
        * Updates viewer position.
        * 
        * @param fTime
        *  Current effect relative time (in seconds).
        * @brief
        *  Updates viewer position.
        */
        void UpdateViewer( float fTime );

    private:
        /** 
        * Sphere mesh. Used to render sky.
        * 
        * @brief
        *  Sphere mesh.
        */
        ID3DXMesh* m_pkSphere;

        /** 
        * Vertex buffer for ocean mesh.
        * 
        * @brief
        *  Vertex buffer for ocean mesh.
        */
        IDirect3DVertexBuffer8* m_pkVertexBuffer;

        /** 
        * Index buffer for ocean mesh.
        * 
        * @brief
        *  Index buffer for ocean mesh.
        */
        IDirect3DIndexBuffer8* m_pkIndexBuffer;

        /** 
        * Bump texture.
        * 
        * @brief
        *  Bump texture.
        */
        IDirect3DTexture8* m_pkBumpTexture;

        /** 
        * Pointer to effect settings dialog in browser mode.
        * 
        * @brief
        *  Pointer to effect settings dialog in browser mode.
        */
        COceanSceneDialog* m_pkOceanSceneSettingsDlg;

        /** 
        * Current viewer position.
        * 
        * @brief
        *  Current viewer position.
        */
        D3DXVECTOR3 m_vViewer;

        /** 
        * Current viewer direction.
        * 
        * @brief
        *  Current viewer direction.
        */
        D3DXVECTOR3 m_vDirection;

        /** 
        * \e Sky vertex shader handle.
        * 
        * @brief
        *  \e Sky vertex shader handle.
        */
        DWORD m_dwSkyVSHandle;

        /** 
        * \e Sky pixel shader handle.
        * 
        * @brief
        *  \e Sky pixel shader handle.
        */
        DWORD m_dwSkyPSHandle;

        /** 
        * \e Ocean vertex shader handle.
        * 
        * @brief
        *  \e Ocean vertex shader handle.
        */
        DWORD m_dwOceanVSHandle;

        /** 
        * \e Ocean pixel shader handle.
        * 
        * @brief
        *  \e Ocean pixel shader handle.
        */
        DWORD m_dwOceanPSHandle;

        /** 
        * Dynamic range of environment map.
        * 
        * @brief
        *  Dynamic range of environment map.
        */
        float m_fDynamicRangeOfEnvMap;

        /** 
        * Ocean height map generator.
        * 
        * @brief
        *  Ocean height map generator.
        */
        COceanWaveGenerator m_kWaveGen;
    };



} // namespace Effects




#endif // #ifndef _OCEANSCENE_H_INCLUDED_
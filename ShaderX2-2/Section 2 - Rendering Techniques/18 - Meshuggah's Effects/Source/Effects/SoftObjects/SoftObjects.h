///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CSoftObjects.
 *
 * @file
 *  SoftObjects.h
 * @author
 *  Carsten Wenzel
 * @date
 *  08.03.2002
 * @brief
 *  Declaration of \e Effects::CSoftObjects.
 */



#ifndef _SOFTOBJECTS_H_INCLUDED_
#define _SOFTOBJECTS_H_INCLUDED_

#pragma once



#include "Effect.h"
#include "SoftObjectsPolygonizer.h"
#include <vector>



namespace Effects
{



    class CSoftObjectsDialog;



    /**
    * <em>Soft objects</em> effect class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  08.03.2002
    * @brief
    *  <em>Soft objects</em> effect class.
    */
    class CSoftObjects : 
        public EffectSystem::CEffect, 
        private CSoftObjectsPolygonizer::ITriangleContainer
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
        CSoftObjects( IDirect3DDevice8* pD3DDev, 
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
        virtual ~CSoftObjects();

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
        * Structure of a sphere vertex used to render the environment.
        * 
        * @brief
        *  Structure of a sphere vertex used to render the environment.
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
        * Structure of a iso mesh vertex.
        * 
        * @brief
        *  Structure of a iso mesh vertex.
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
        };

    private:
        /**
        * Sets shader constants for \e Environment and <em>Soft objects</em> shader. 
        * 
        * @brief
        *  Sets shader constants for \e Environment and <em>Soft objects</em> shader. 
        */
        void SetShaderConstants();

        /** 
        * Renders a sphere. Uses #m_pkSphere.
        * 
        * @brief
        *  Renders a sphere.
        */
        void RenderSphere();

        /** 
        * Renders environment.
        * 
        * @brief
        *  Renders environment.
        */
        void RenderEnvironment();

        /**
        * Renders soft objects.
        * 
        * @param fTime
        *  Current effect relative time (in seconds).
        * @brief
        *  Renders soft objects.
        */
        void RenderSoftObjects( float fTime );

        virtual void Begin( unsigned int uiVerticesToAdd, unsigned int uiIndicesToAdd );
        virtual void AddVertices( const CSoftObjectsPolygonizer::SVertex* const pVertices );
        virtual void AddIndices( const int* const pIndices );
        virtual void End();

        /** 
        * Flushs vertex and index buffer after rendering of soft objects is done.
        * 
        * @brief
        *  Flushs vertex and index buffer after rendering of soft objects is done.
        */
        void Flush();

    private:
        /** 
        * Vertex buffer for iso mesh.
        * 
        * @brief
        *  Vertex buffer for iso mesh.
        */
        IDirect3DVertexBuffer8* m_pkVertexBuffer;

        /** 
        * Index buffer for iso mesh.
        * 
        * @brief
        *  Index buffer for iso mesh.
        */
        IDirect3DIndexBuffer8* m_pkIndexBuffer;

        /** 
        * Sphere mesh. Used to render environment.
        * 
        * @brief
        *  Sphere mesh.
        */
        ID3DXMesh* m_pkSphere;

        /** 
        * Pointer to effect settings dialog in browser mode.
        * 
        * @brief
        *  Pointer to effect settings dialog in browser mode.
        */
        CSoftObjectsDialog* m_pkSoftObjectsSettingsDlg;

        /** 
        * Pointer to access vertex buffer #m_pkVertexBuffer.
        * 
        * @brief
        *  Pointer to access vertex buffer.
        */
        SVertex* m_pVertices;

        /** 
        * Pointer to access index buffer #m_pkIndexBuffer.
        * 
        * @brief
        *  Pointer to access index buffer.
        */
        WORD* m_pIndices;

        /** 
        * Last used index in vertex buffer. 
        * Used to correctly add vertices via AddVertices() and to detect buffer overruns.
        * 
        * @brief
        *  Last used index in vertex buffer.
        */
        unsigned int m_uiVertexBufferOffset;

        /** 
        * Last used index in index buffer.
        * Used to correctly add vertices via AddIndices() and to detect buffer overruns.
        * 
        * @brief
        *  Last used index in index buffer.
        */
        unsigned int m_uiIndexBufferOffset;

        /** 
        * Number of vertices to add after calling Begin().
        * 
        * @brief
        *  Number of vertices to add after calling Begin().
        */
        unsigned int m_uiVerticesToAdd;

        /** 
        * Number of indices to add after calling Begin().
        * 
        * @brief
        *  Number of indices to add after calling Begin().
        */
        unsigned int m_uiIndicesToAdd;

        /** 
        * \e <em>Soft objects</em> vertex shader handle.
        * 
        * @brief
        *  \e <em>Soft objects</em> vertex shader handle.
        */
        DWORD m_dwSoftObjectsVSHandle;

        /** 
        * \e Environment pixel shader handle.
        * 
        * @brief
        *  \e Environment pixel shader handle.
        */
        DWORD m_dwSoftObjectsPSHandle;

        /** 
        * \e Environment vertex shader handle.
        * 
        * @brief
        *  \e Environment vertex shader handle.
        */
        DWORD m_dwEnvironmentVSHandle;

        /** 
        * \e Environment pixel shader handle.
        * 
        * @brief
        *  \e Environment pixel shader handle.
        */
        DWORD m_dwEnvironmentPSHandle;

        /** 
        * Current viewer position.
        * 
        * @brief
        *  Current viewer position.
        */
        D3DXVECTOR3 m_vViewer;

        /** 
        * Position attractor to synch effect to music in demo mode.
        * 
        * @brief
        *  Position attractor to synch effect to music in demo mode.
        */
        float m_fBlobsAttractor;

        /** 
        * Bass spectrum used to synch the effect to music in demo mode.
        * 
        * @brief
        *  Bass spectrum used to synch the effect to music in demo mode.
        */
        std::vector< unsigned char > m_colBassSpectrum;

        /** 
        * Soft objects polygonizer.
        * 
        * @brief
        *  Soft objects polygonizer.
        */
        CSoftObjectsPolygonizer m_kPolygonizer;
    };



} // namespace Effects



#endif // #ifndef _SOFTOBJECTS_H_INCLUDED_
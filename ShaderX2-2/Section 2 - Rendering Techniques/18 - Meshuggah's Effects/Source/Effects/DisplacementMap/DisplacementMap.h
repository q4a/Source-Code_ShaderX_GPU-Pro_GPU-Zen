///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CDisplacmentMap.
 *
 * @file
 *  DisplacementMap.h
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Declaration of \e Effects::CDisplacmentMap.
 */



#ifndef _DISPLACEMENTMAP_H_INCLUDED_
#define _DISPLACEMENTMAP_H_INCLUDED_

#pragma once



#include "Effect.h"
#include <vector>


namespace Effects
{



    class CDisplacementMapDialog;



    /**
    * <em>Displacement map</em> effect class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  04.03.2002
    * @brief
    *  <em>Displacement map</em> effect class.
    */
    class CDisplacementMap : public EffectSystem::CEffect
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
        CDisplacementMap( IDirect3DDevice8* pD3DDev, 
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
        virtual ~CDisplacementMap();

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
        * Default height scale value.
        * 
        * @brief
        *  Default height scale value.
        */
        static const float sc_fDefaultHeightScale;

    protected:
        virtual void ConfirmRequirements();
        virtual void FreeAllResources();

    private:
        /** 
        * Structure for a vertex of the displaced mesh.
        * 
        * @brief
        *  Structure for a vertex of the displaced mesh.
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
        * Gets the height value of the displacment map at (<tt>uiX</tt>, <tt>uiY</tt>).
        * 
        * @param uiX
        *  x coordinate for lookup in displacement map.
        * @param uiY
        *  y coordinate for lookup in displacement map.
        * @return
        *  Height value of the displacment map at (<tt>uiX</tt>, <tt>uiY</tt>).
        * @brief
        *  Gets the height value of the displacment map at (<tt>uiX</tt>, <tt>uiY</tt>).
        */
        int GetDisplacement( unsigned int uiX, unsigned int uiY );

        /**
        * Sets shader constants for <em>Displacement map</em> shader.
        * 
        * @brief
        *  Sets shader constants for <em>Displacement map</em> shader.
        */
        void SetShaderConstants();

        /**
        * Sets up a displaced vertex.
        * 
        * @param uiX
        *  x coordinate for position and lookup in displacement map.
        * @param uiY
        *  y coordinate for position and lookup in displacement map.
        * @param pVertex
        *  Pointer to structure receiving the displaced vertex.
        * @brief
        *  Sets up a displaced vertex.
        */
        void SetupVertex( unsigned int uiX, unsigned int uiY, SVertex* pVertex );

    private:
        /** 
        * Vertex buffer for displaced vertices.
        * 
        * @brief
        *  Vertex buffer for displaced vertices.
        */
        IDirect3DVertexBuffer8* m_pkVertexBuffer;

        /** 
        * Pointer to effect settings dialog in browser mode.
        * 
        * @brief
        *  Pointer to effect settings dialog in browser mode.
        */
        CDisplacementMapDialog* m_pkDisplacementMapSettingsDlg;

        /** 
        * <em>Displacement map</em> vertex shader handle.
        * 
        * @brief
        *  <em>Displacement map</em> vertex shader handle.
        */
        DWORD m_dwVSHandle;

        /** 
        * <em>Displacement map</em> pixel shader handle.
        * 
        * @brief
        *  <em>Displacement map</em> pixel shader handle.
        */
        DWORD m_dwPSHandle;

        /** 
        * Current height scale value for displacement map.
        * 
        * @brief
        *  Current height scale value for displacement map.
        */
        float m_fHeightScale;

        /** 
        * Current viewer position.
        * 
        * @brief
        *  Current viewer position.
        */
        D3DXVECTOR3 m_vViewer;

        /** 
        * Displacement map.
        * 
        * @brief
        *  Displacement map.
        */
        std::vector< unsigned char > m_colDisplacementMap;

        /** 
        * Bass spectrum used to synch the effect to music in demo mode.
        * 
        * @brief
        *  Bass spectrum used to synch the effect to music in demo mode.
        */
        std::vector< unsigned char > m_colBassSpectrum;
    };



} // namespace Effects




#endif // #ifndef _DISPLACEMENTMAP_H_INCLUDED_
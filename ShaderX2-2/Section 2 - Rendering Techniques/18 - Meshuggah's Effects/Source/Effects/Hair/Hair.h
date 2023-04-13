///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effcts::CHair.
 *
 * @file
 *  Hair.h
 * @author
 *  Carsten Wenzel
 * @date
 *  06.03.2002
 * @brief
 *  Declaration of \e Effcts::CHair.
 */



#ifndef _HAIR_H_INCLUDED_
#define _HAIR_H_INCLUDED_

#pragma once



#include "Effect.h"
#include <vector>



namespace Effects
{



    class CHairDialog;



    /**
    * \e Hair effect class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  04.03.2002
    * @brief
    *  \e Hair effect class.
    */
    class CHair : public EffectSystem::CEffect
    {
    public:
        enum EHairEnums
        {
            /**
            * Number of joints per hair.
            */
            NUM_HAIR_JOINTS = 10,

            /**
            * Number of hairs.
            */
            NUM_HAIRS       = 2000
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
        CHair( IDirect3DDevice8* pD3DDev, 
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
        virtual ~CHair();

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
        * Default influence of gravity.
        * 
        * @brief
        *  Default influence of gravity.
        */
        static const float sc_fDefaultGravity;

        /** 
        * Default influence of inertia.
        * 
        * @brief
        *  Default influence of inertia.
        */
        static const float sc_fDefaultInertia;

    protected:
        virtual void ConfirmRequirements();
        virtual void FreeAllResources();

    private:
        /** 
        * Structure for a vertex used to form a hair.
        * 
        * @brief
        *  Structure for a vertex used to form a hair.
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
            * Tangent.
            * 
            * @brief
            *  Tangent.
            */
            D3DXVECTOR3 vTangent;

            /** 
            * 1D texture coordinate.
            * 
            * @brief
            *  1D texture coordinate.
            */
            float fTex1D;
        };

        /** 
        * Structure of a hair.
        * 
        * @brief
        *  Structure of a hair.
        */
        struct SHair
        {
            /** 
            * Direction of hair.
            * 
            * @brief
            *  Direction of hair.
            */
            D3DXVECTOR3 vDir;

            /** 
            * Length of hair.
            * 
            * @brief
            *  Length of hair.
            */
            float fLength;

            /** 
            * 1D texture coordinate.
            * 
            * @brief
            *  1D texture coordinate.
            */
            float fTex1D;
        };

    private:
        /**
        * Updates viewer position.
        * 
        * @param fTime
        *  Current effect relative time (in seconds).
        * @brief
        *  Updates viewer position.
        */
        void UpdateViewerPosition( float fTime );

        /**
        * Calculates hair rotation matrix.
        * 
        * @param matTransform
        *  Reference to struture that should receive the hair rotation matrix.
        * @param fTime
        *  Time for which matrix should be calculated.
        * @brief
        *  Calculates hair rotation matrix.
        */
        void CalcHairRotationMatrix( D3DXMATRIX& matTransform, float fTime );

        /**
        * Calculates hair translation matrix.
        * 
        * @param matTransform
        *  Reference to struture that should receive the hair translation matrix.
        * @param fTime
        *  Time for which matrix should be calculated.
        * @brief
        *  Calculates hair translation matrix.
        */
        void CalcHairTranslationMatrix( D3DXMATRIX& matTransform, float fTime );

        /**
        * Calculates factors for hair dynamics for given time. 
        * Updates #m_vGravity and #m_matInertia.
        * 
        * @param fTime
        *  Current effect relative time (in seconds).
        * @brief
        *  Calculates factors for hair dynamics for given time.
        */
        void CalcHairDynamics( float fTime );

        /**
        * Sets shader constants for \e Hair shader. 
        * 
        * @brief
        *  Sets shader constants for \e Hair shader.
        */
        void SetShaderConstants();

        /** 
        * Generates an \e anisotropy lookup texture.
        * Calculation is based on #m_vAnisotropySettings.
        * 
        * @brief
        *  Generates an \e anisotropy lookup texture.
        */
        void CalcAnisotropicTexture();
    
    private:
        /** 
        * Vertex buffer for hairs.
        * 
        * @brief
        *  Vertex buffer for hairs.
        */
        IDirect3DVertexBuffer8* m_pkVertexBuffer;

        /** 
        * Index buffer for hairs.
        * 
        * @brief
        *  Index buffer for hairs.
        */
        IDirect3DIndexBuffer8* m_pkIndexBuffer;

        /** 
        * \e Anisotropy lookup texture.
        * 
        * @brief
        *  \e Anisotropy lookup texture.
        */
        IDirect3DTexture8* m_pkAnisotropicTexture;

        /** 
        * Pointer to effect settings dialog in browser mode.
        * 
        * @brief
        *  Pointer to effect settings dialog in browser mode.
        */
        CHairDialog* m_pkHairSettingsDlg;

        /** 
        * \e Hair vertex shader handle.
        * 
        * @brief
        *  \e Hair vertex shader handle.
        */
        DWORD m_dwVSHandle;

        /** 
        * \e Hair pixel shader handle.
        * 
        * @brief
        *  \e Hair pixel shader handle.
        */
        DWORD m_dwPSHandle;

        /** 
        * Current gravity vector.
        * 
        * @brief
        *  Current gravity vector.
        */
        D3DXVECTOR3 m_vGravity;

        /** 
        * Current viewer position.
        * 
        * @brief
        *  Current viewer position.
        */
        D3DXVECTOR3 m_vViewer;

        /** 
        * Current anisotropy settings.
        * 
        * @brief
        *  Current anisotropy position.
        */
        D3DXVECTOR4 m_vAnisotropySettings;

        /** 
        * Current diffuse hair color.
        * 
        * @brief
        *  Current diffuse hair color.
        */
        D3DXVECTOR4 m_vDiffuseColor;

        /** 
        * Current specular hair color.
        * 
        * @brief
        *  Current specular hair color.
        */
        D3DXVECTOR4 m_vSpecularColor;

        /** 
        * Current inertia matrix. Used to calculate a per hair inertia vector.
        * 
        * @brief
        *  Current inertia matrix.
        */
        D3DXMATRIX m_matInertia;

        /** 
        * Current influence of gravity.
        * 
        * @brief
        *  Current influence of gravity.
        */
        float m_fGravity;

        /** 
        * Current influence of inertia.
        * 
        * @brief
        *  Current influence of inertia.
        */
        float m_fInertia;

        /** 
        * Bass spectrum used to synch the effect to music in demo mode.
        * 
        * @brief
        *  Bass spectrum used to synch the effect to music in demo mode.
        */
        std::vector< unsigned char > m_colBassSpectrum;

        /** 
        * Array of hairs.
        * 
        * @brief
        *  Array of hairs.
        */
        SHair m_pHairs[ NUM_HAIRS ];        
    };



} // namespace Effects



#endif // #ifndef _HAIR_H_INCLUDED_
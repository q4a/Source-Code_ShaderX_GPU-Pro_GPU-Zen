///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::CBrowserRenderView.
 * 
 * @file
 *  BrowserRenderView.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Declaration of \e WindowSystem::CBrowserRenderView.
 */



#ifndef _BROWSERRENDERVIEW_H_INCLUDED_
#define _BROWSERRENDERVIEW_H_INCLUDED_

#pragma once



namespace EffectSystem
{
    class IEffect;
    class CEffectFactory;
}

namespace ScrambledArchive
{
    class CScrambledArchive;
}

namespace Shared
{
    class CTextureManager;
}



namespace WindowSystem
{



    /**
    * A render view that displays an user selected effect.
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  A render view that displays an user selected effect.
    */
    class CBrowserRenderView : public CView
    {
    public:
        /**
        * Updates frame in render view.
        * 
        * @return
        *  \b true if successful else \b false.
        *  - Possible reasons:
        *   -# D3D device was previously lost and a reset failed
        * @brief
        *  Updates frame in render view.
        */
        bool FrameUpdate();

        /**
        * Creates a new effect to be used for rendering.
        * 
        * @param pkNewEffectFactory
        *  Pointer to an EffectSystem::CEffectFactory used to create the effect.
        * @return
        *  Pointer to an EffectSystem::IEffect interface representing the created effect.
        * @brief
        *  Creates a new effect to be used for rendering.
        */
        EffectSystem::IEffect* SetNewEffect( EffectSystem::CEffectFactory* pkNewEffectFactory );

        /**
        * Is there an effect currently selected.
        * 
        * @return
        *  \b true if an effect is currently selected else \b false.
        * @brief
        *  Is there an effect currently selected.
        */
        bool EffectAvailable();

    protected:
        DECLARE_DYNCREATE( CBrowserRenderView )    

        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CBrowserRenderView();           

        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CBrowserRenderView();

        /** 
        * Redraws content of render view. Useful when effect settings dialog is being dragged
        * around over render view.
        * 
        * @brief
        *  Redraws content of render view.
        */
        virtual void OnDraw( CDC* pDC );

    #ifdef _DEBUG
        /** 
        * MFC debug hook. Only available in debug builds.
        * 
        * @brief
        *  MFC debug hook. Only available in debug builds.
        */
        virtual void AssertValid() const;

        /** 
        * MFC debug hook. Only available in debug builds.
        * 
        * @brief
        *  MFC debug hook. Only available in debug builds.
        */
        virtual void Dump( CDumpContext& dc ) const;
    #endif

        /** 
        * Message handler for \c WM_SIZE messages.
        * Forces a D3D device reset by setting #m_bForceReset to \b true.
        * 
        * @brief
        *  Message handler for \c WM_SIZE messages.
        */
        afx_msg void OnSize( UINT nType, int cx, int cy );

        /**
        * Message handler for \c WM_CREATE messages. Creates the window, initializes the D3D device #m_pD3DDev 
        * using the settings from Application::CMeshuggahApp::GetD3DPresentParameters() (copied into 
        * #m_sD3DPresParams), creates the scambled archive object #m_pkScrambledArchive and the texture manager
        * object #m_pkTextureManager.
        * 
        * @param lpCreateStruct
        *  Window creation parameters.
        * @return
        *  \b 0 if successful. \b -1 if creation failed.
        * @brief
        *  Message handler for \c WM_CREATE messages.
        */
        afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );

        /** 
        * Handles \c WM_DESTROY messages. Releases D3D device interface, current effect as well as
        * scrambled archive and texture manager.
        * 
        * @brief
        *  Handles \c WM_DESTROY messages.
        */
        afx_msg void OnDestroy();

        /** 
        * Shows effect specific help.
        * 
        * @brief
        *  Shows effect specific help.
        */
        afx_msg void OnContextHelp();

        /** 
        * GUI update handler for showing effect specific help.
        * 
        * @brief
        *  GUI update handler for showing effect specific help.
        */
        afx_msg void OnUpdateContextHelp( CCmdUI* pCmdUI );

        /** 
        * Toggels effect settings dialog.
        * 
        * @brief
        *  Toggels effect settings dialog.
        */
        afx_msg void OnEffectSettingsDialog();

        /** 
        * GUI update handler for toggeling effect settings dialog.
        * 
        * @brief
        *  GUI update handler for toggeling effect settings dialog.
        */
        afx_msg void OnUpdateEffectSettingsDialog( CCmdUI* pCmdUI );

        DECLARE_MESSAGE_MAP()

    private:
        /** 
        * A D3D device interface.
        * 
        * @brief
        *  A D3D device interface.
        */
        IDirect3DDevice8* m_pD3DDev;

        /** 
        * Present parameters used for D3D device creation.
        * 
        * @brief
        *  Present parameters used for D3D device creation.
        */
        D3DPRESENT_PARAMETERS m_sD3DPresParams;

        /** 
        * Flag indicating that a D3D device was forced. Happens in CBrowserRenderView::OnSize().
        * The reset will be initiated the next time CBrowserRenderView::FrameUpdate() is called.
        * 
        * @brief
        *  Flag indicating that a D3D device was forced.
        */
        bool m_bForceReset;

        /** 
        * Pointer to an EffectSystem::IEffect interface representing the current effect.
        * 
        * @brief
        *  Pointer to an EffectSystem::IEffect interface representing the current effect.
        */
        EffectSystem::IEffect* m_pkCurrentEffect;

        /** 
        * A pointer to a scrambled archive. Necessary for effect creation.
        *
        * @see
        *  EffectSystem::CEffectFactory::CreateInstance()
        * @brief
        *  A pointer to a scrambled archive.
        */
        ScrambledArchive::CScrambledArchive* m_pkScrambledArchive;

        /** 
        * A pointer to a texture manager. Necessary for effect creation.
        * 
        * @brief
        *  A pointer to a texture manager.
        */
        Shared::CTextureManager* m_pkTextureManager;
    };



#include "BrowserRenderView.inl"



} // namespace WindowSystem



#endif // #ifndef _BROWSERRENDERVIEW_H_INCLUDED_

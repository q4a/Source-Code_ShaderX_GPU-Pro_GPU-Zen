///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::CDemoMainFrame.
 * 
 * @file
 *  DemoMainFrm.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Declaration of \e WindowSystem::CDemoMainFrame.
 */



#ifndef _DEMOMAINFRM_H_INCLUDED_
#define _DEMOMAINFRM_H_INCLUDED_

#pragma once



#include "RenderWnd.h"
#include "DemoLoaderWnd.h"



namespace EffectSystem
{
    class CEffectSequencer;
}



namespace WindowSystem
{



    /**
    * The mainframe class of the demo.
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  The mainframe class of the demo.
    */
    class CDemoMainFrame : public CFrameWnd
                         , public IRenderWnd
    {
    public:
        /**
        * Constructor.
        * 
        * @param bEnableMusic
        *  Should music playback be enabled.
        * @param bLoopDemo
        *  Should demo run in a loop.
        * @brief
        *  Constructor.
        */
        CDemoMainFrame( bool bEnableMusic, bool bLoopDemo );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CDemoMainFrame();

        /** 
        * Sets all window properties. The window size is set to its default (640x480).
        * 
        * @brief
        *  Sets all window properties.
        */
        virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
        
        /** 
        * Handles windows system command messages (\c WM_SYSCOMMAND). During demo playback it ignores
        * any \c SC_SCREENSAVE or \c SC_MONITORPOWER messages.
        * 
        * @brief
        *  Handles windows system command messages.
        */
        afx_msg void OnSysCommand( UINT nID, LPARAM lParam );

        /** 
        * Handles keyboard input.
        * - Handled keys
        *   -# ESC, quit demo
        *   -# SPACE, pause demo
        *   -# r or R, restart demo
        *   -# s or S, stop demo
        * 
        * @brief
        *  Handles user keyboard input.
        */
        afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );

        /** 
        * Handles \c WM_DESTROY messages. Releases #m_pkEffectSequencer and the D3D device interface
        * by calling CDemoMainFrame::ReleaseD3DDevice().
        * 
        * @brief
        *  Handles \c WM_DESTROY messages.
        */
        afx_msg void OnDestroy();

        virtual bool RenderFrame();

        /** 
        * Starts demo playback.
        * 
        * @brief
        *  Starts demo playback.
        */
        void PlayDemo();

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

    protected:
        /** 
        * Handles \c WM_CREATE messages. Creates the window, initializes the D3D device #m_pD3DDev 
        * using the settings from Application::CMeshuggahApp::GetD3DPresentParameters() (copied into 
        * #m_sD3DPresParams) and creates the effect sequencer object #m_pkEffectSequencer.
        * 
        * @param lpCreateStruct
        *  Window creation parameters.
        * @return
        *  \b 0 if successful. \b -1 if creation failed.
        * @brief
        *  Handles \c WM_CREATE messages.
        */
        afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );

        DECLARE_MESSAGE_MAP()
    
    private:
        /** 
        * Releases the D3D interface (#m_pD3DDev).
        * 
        * @brief
        *  Releases the D3D interface (#m_pD3DDev).
        */
        void ReleaseD3DDevice();

    private:
        /** 
        * Effect sequencer.
        * 
        * @brief
        *  Effect sequencer.
        */
        EffectSystem::CEffectSequencer* m_pkEffectSequencer;

        /** 
        * A pointer to a D3D device interface.
        * 
        * @brief
        *  A pointer to a D3D device interface.
        */
        IDirect3DDevice8* m_pD3DDev;

        /** 
        * D3D present parameters.
        * 
        * @brief
        *  D3D present parameters.
        */
        D3DPRESENT_PARAMETERS m_sD3DPresParams;

        /** 
        * "Enable music" flag. Used for creation of #m_pkEffectSequencer.
        * 
        * @brief
        *  "Enable music" flag.
        */
        bool m_bEnableMusic;

        /** 
        * "Loop demo" flag. Used for creation of #m_pkEffectSequencer.
        * 
        * @brief
        *  "Loop demo" flag.
        */
        bool m_bLoopDemo;

        /** 
        * The "please wait" window. Shown while demo is loading.
        * 
        * @brief
        *  The "please wait" window.
        */
        CDemoLoaderWnd m_kDemoLoaderWnd;
    };



} // namespace WindowSystem



#endif // #ifndef _DEMOMAINFRM_H_INCLUDED_

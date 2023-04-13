///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::CBrowserMainFrame.
 * 
 * @file
 *  BrowserMainFrm.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Declaration of \e WindowSystem::CBrowserMainFrame.
 */



#ifndef _BROWSERMAINFRM_H_INCLUDED_
#define _BROWSERMAINFRM_H_INCLUDED_

#pragma once



#include "RenderWnd.h"
#include "Timer.h"



namespace WindowSystem
{



    class CBrowserRenderView;
    class CBrowserShaderFormView;



    /**
    * The mainframe window class of the effect browser window.
    * It is split into a tree view for effect
    * enumeration and a render view that displays a selected effect.
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  The mainframe window class of the effect browser.
    */
    class CBrowserMainFrame : public CFrameWnd
                            , public IRenderWnd
    {
    public:
        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CBrowserMainFrame();

        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CBrowserMainFrame();

        /** 
        * Creates client window. Initializes splitter window and all its embedded views.
        * 
        * @brief
        *  Creates client window.
        */
        virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );

        /** 
        * Sets all window properties. Sets the window size to three quaters of the desktop resolution
        * and centers the window.
        * 
        * @brief
        *  Sets all window properties.
        */
        virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

        /** 
        * Command message handler. Forwards all message to render view in order to process
        * effect related GUI actions (show effect related help, toggle effect settings dialog, etc.).
        * 
        * @brief
        *  Command message handler.
        */
        virtual BOOL OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );

        /**
        * Delegates a message to the render view by calling 
        * WindowSystem::CBrowserRenderView::FrameUpdate() on #m_pkRenderView.
        * 
        * @see
        *  WindowSystem::IRenderWnd::RenderFrame().
        * @brief
        *  Renders a new frame.
        */
        virtual bool RenderFrame();

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
        * Message handler for \c WM_CREATE messages. Creates the window, loads and initializes 
        * a high-color toolbar and initializes status bar.
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
        * Message handler for \c WM_DESTROY. Closes HTML help window if open before
        * browser mainframe is shut down.
        * 
        * @brief
        *  Message handler for WM_DESTROY.
        */
        afx_msg void OnDestroy();

        /** 
        * Shows application information in HTML help window.
        * 
        * @brief
        * Shows application information in HTML help window.
        */
        afx_msg void OnAppAbout();

        /** 
        * Shows default help page in HTML help window.
        * 
        * @brief
        *  Shows default help page in HTML help window.
        */
        afx_msg void OnDefaultHelp();       

        DECLARE_MESSAGE_MAP()

    protected:  
        /** 
        * Status bar.
        * 
        * @brief
        *  Status bar.
        */
        CStatusBar m_wndStatusBar;

        /** 
        * Toolbar.
        * 
        * @brief
        *  Toolbar.
        */
        CToolBar m_wndToolBar;

        /** 
        * Main (vertical) splitter.
        * 
        * @brief
        *  Main (vertical) splitter.
        */
        CSplitterWnd m_wndMainSplitter;

        /** 
        * Horizontal sub splitter seperating the effect 
        * tree view from the shader from view.
        * 
        * @brief
        *  Horizontal sub splitter seperating the effect 
        *  tree view from the shader from view.
        */
        CSplitterWnd m_wndLeftSubSplitter;

    private:
        /** 
        * Time of last frame update (in seconds).
        * 
        * @brief
        *  Time of last frame update (in seconds).
        */
        float m_fLastUpdate;

        /** 
        * Pointer to render view.
        * 
        * @brief
        *  Pointer to render view.
        */
        CBrowserRenderView* m_pkRenderView;

        /** 
        * Pointer to shader form view.
        * 
        * @brief
        *  Pointer to shader form view.
        */
        CBrowserShaderFormView* m_pkShaderView;

        /** 
        * High-color image list for toolbar.
        * 
        * @brief
        *  High-color image list for toolbar.
        */
        CImageList m_kToolBarImageList;

        /** 
        * Timer to measure the effect frame rate.
        * 
        * @brief
        *  Timer to measure the effect frame rate.
        */
        Shared::CTimer m_kTimer;
    };



} // namespace WindowSystem



#endif // #ifndef _BROWSERMAINFRM_H_INCLUDED_

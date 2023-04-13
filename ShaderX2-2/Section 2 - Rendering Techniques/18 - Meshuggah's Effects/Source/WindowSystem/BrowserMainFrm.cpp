///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CBrowserMainFrame.
 *
 * @file
 *  BrowserMainFrm.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e WindowSystem::CBrowserMainFrame.
 */



#include "stdafx.h"

#include "BrowserMainFrm.h"
#include "BrowserRenderView.h"
#include "BrowserTreeView.h"
#include "BrowserShaderFormView.h"
#include "Clamp.h"
#include "Meshuggah.h"


#include "resource.h"
#include "Help\ContextIDs.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace WindowSystem;
using namespace Shared;
using namespace Application;



BEGIN_MESSAGE_MAP( CBrowserMainFrame, CFrameWnd )
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_COMMAND( ID_APP_ABOUT, OnAppAbout )
    ON_COMMAND( ID_DEFAULT_HELP, OnDefaultHelp )
END_MESSAGE_MAP()



static UINT s_uiIndicators[] =
{
    ID_SEPARATOR,
    ID_FPS_INFO,
    ID_FPS
};



CBrowserMainFrame::CBrowserMainFrame()
: m_fLastUpdate( 0.0f )
, m_pkRenderView( 0 )
, m_pkShaderView( 0 )
, m_kToolBarImageList()
, m_kTimer()
{    
    m_kTimer.Start();
}



CBrowserMainFrame::~CBrowserMainFrame()
{
}



int
CBrowserMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( -1 == CFrameWnd::OnCreate( lpCreateStruct ) )
    {
        return( -1 );
    }

    if( FALSE == m_wndToolBar.CreateEx( this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE |
                                        CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS |
                                        CBRS_FLYBY | CBRS_SIZE_DYNAMIC ) ||
        FALSE == m_wndToolBar.LoadToolBar( IDR_MAINFRAME ) )
    {
        TRACE0( "Failed to create tool bar\n" );
        return( -1 );
    }

    m_kToolBarImageList.Create( 16, 16, ILC_COLOR32 | ILC_MASK, 1, 0 );
    m_kToolBarImageList.Add( AfxGetApp()->LoadIcon( IDI_EFFECTSETTINGS ) );
    m_kToolBarImageList.Add( AfxGetApp()->LoadIcon( IDI_HELP ) );
    m_kToolBarImageList.Add( AfxGetApp()->LoadIcon( IDI_INFO ) );
    m_wndToolBar.GetToolBarCtrl().SetImageList( &m_kToolBarImageList );

    if( FALSE == m_wndStatusBar.Create( this ) ||
        FALSE == m_wndStatusBar.SetIndicators( s_uiIndicators,
                                               sizeof( s_uiIndicators )/ sizeof( UINT ) ) )
    {
        TRACE0( "Failed to create status bar\n ");
        return( -1 );
    }

    m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );
    EnableDocking( CBRS_ALIGN_ANY );
    DockControlBar( &m_wndToolBar );

    return( 0 );
}



BOOL
CBrowserMainFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext )
{
    CRect rectClient;
    GetClientRect( rectClient );

    if( FALSE == m_wndMainSplitter.CreateStatic( this, 1, 2 ) )
    {
        return( FALSE );
    }

    if( FALSE == m_wndMainSplitter.CreateView( 0, 1, RUNTIME_CLASS( CBrowserRenderView ),
        CSize( rectClient.Width(), rectClient.Height() ), pContext ) )
    {
        return( FALSE );
    }

    if( FALSE == m_wndLeftSubSplitter.CreateStatic( &m_wndMainSplitter, 2, 1,
        WS_CHILD | WS_VISIBLE, m_wndMainSplitter.IdFromRowCol( 0, 0 ) ) )
    {
        return( FALSE );
    }

    if( FALSE == m_wndLeftSubSplitter.CreateView( 0, 0,
        RUNTIME_CLASS( CBrowserTreeView ), CSize( 0, 0 ), pContext ) )
    {
        return( FALSE );
    }

    if( FALSE == m_wndLeftSubSplitter.CreateView( 1, 0,
        RUNTIME_CLASS( CBrowserShaderFormView ), CSize( 0, 0 ), pContext ) )
    {
        return( FALSE );
    }

    m_wndMainSplitter.SetColumnInfo( 0, 250, 0 );
    m_wndLeftSubSplitter.SetRowInfo( 0, 150, 0 );

    RecalcLayout();

    m_wndMainSplitter.SetActivePane( 0, 1 );
    m_pkRenderView = static_cast< CBrowserRenderView* >( m_wndMainSplitter.GetPane( 0, 1 ) );
    m_pkShaderView = static_cast< CBrowserShaderFormView* >( m_wndLeftSubSplitter.GetPane( 1, 0 ) );

    static_cast< CBrowserTreeView* >( m_wndLeftSubSplitter.GetPane( 0, 0 ) )->
        SetRenderView( m_pkRenderView );
    static_cast< CBrowserTreeView* >( m_wndLeftSubSplitter.GetPane( 0, 0 ) )->
        SetShaderView( m_pkShaderView );

    return( TRUE );
}



BOOL
CBrowserMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
    if( FALSE == CFrameWnd::PreCreateWindow( cs ) )
    {
        return( FALSE );
    }

    int iBrowserXRes( ( ::GetSystemMetrics( SM_CXSCREEN ) * 3 ) / 4 );
    int iBrowserYRes( ( ::GetSystemMetrics( SM_CYSCREEN ) * 3 ) / 4 );

    cs.x = ( ::GetSystemMetrics( SM_CXSCREEN ) - iBrowserXRes ) / 2;
    cs.y = ( ::GetSystemMetrics( SM_CYSCREEN ) - iBrowserYRes ) / 2;

    cs.cx = iBrowserXRes;
    cs.cy = iBrowserYRes;

    return( TRUE );
}



#ifdef _DEBUG
void
CBrowserMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}



void
CBrowserMainFrame::Dump( CDumpContext& dc ) const
{
    CFrameWnd::Dump( dc );
}
#endif //_DEBUG



BOOL 
CBrowserMainFrame::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
    if( 0 != m_pkRenderView && 
        m_pkRenderView->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
    {
        return( TRUE );
    }

    return( CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo) );
}



void 
CBrowserMainFrame::OnDestroy()
{
    // make sure that html help window get closed
    // before we shut down (otherwise our temp html 
    // help file is still under control of htmlhelp
    // and can't be deleted)    
    CMeshuggahApp::GetApp().CloseHTMLHelpWindow();

    CFrameWnd::OnDestroy();
}



void 
CBrowserMainFrame::OnAppAbout()
{
    CMeshuggahApp::GetApp().HtmlHelp( ID_HELPCONTEXT_APPABOUT );
}



void 
CBrowserMainFrame::OnDefaultHelp()
{
    CMeshuggahApp::GetApp().HtmlHelp( ID_HELPCONTEXT_DEFHELP );
}



bool
CBrowserMainFrame::RenderFrame()
{
    DBG_ASSERT( 0 != m_pkRenderView );
    if( FALSE == IsIconic() && 0 != GetFocus() )
    {
        CString strFps;
        if( false != m_pkRenderView->FrameUpdate() )
        {
            if( false != m_pkRenderView->EffectAvailable() )
            {
                float fUpdateDelta( m_kTimer.GetTime() - m_fLastUpdate );
                if( fUpdateDelta > 1e-6f )
                {
                    float fFps( 1.0f / fUpdateDelta );
                    strFps.Format( _T( "%3.1f" ), Clamp( fFps, 0.0f, 999.9f ) );
                }
                m_fLastUpdate += fUpdateDelta;
            }
        }
        else
        {
            OnClose();
            return( false );
        }

        m_wndStatusBar.SetPaneText( m_wndStatusBar.CommandToIndex( ID_FPS ), strFps );
    }

    return( true );
}
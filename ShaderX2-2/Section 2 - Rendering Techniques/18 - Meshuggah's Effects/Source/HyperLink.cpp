///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Application::CHyperLink.
 *
 * @file
 *  HyperLink.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Implementation of \e Application::CHyperLink.
 */



#include "stdafx.h"
#include "HyperLink.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Application;



const unsigned int c_uiTimerID( 0x0000 );
const unsigned int c_uiToolTipID( 0x0001 );
const COLORREF c_colorLink( RGB( 0, 0, 238 ) );
const COLORREF c_colorVisited( RGB( 85, 26, 139 ) );
const COLORREF c_colorHover( RGB( 255, 0, 0 ) );
     


CHyperLink::CHyperLink( CString strLink, CString strSerializeVisitedState )
: m_strLink( strLink )
, m_strSerializeVisitedState( strSerializeVisitedState )
, m_hBrush( GetSysColorBrush( COLOR_3DFACE ) )
, m_bVisited( false )
, m_bHovering( false )
, m_kFont()
, m_kToolTip()
{
    if( false == m_strSerializeVisitedState.IsEmpty() )
    {
        m_bVisited = AfxGetApp()->GetProfileInt( _T( "Links" ), 
            m_strSerializeVisitedState, 0 ) ? true : false;
    }
}



CHyperLink::CHyperLink( unsigned int uiLinkID, unsigned int uiSerializeID )
: m_strLink()
, m_strSerializeVisitedState()
, m_hBrush( GetSysColorBrush( COLOR_3DFACE ) )
, m_bVisited( false )
, m_bHovering( false )
, m_kFont()
, m_kToolTip()
{
    m_strLink.LoadString( uiLinkID );
    
    if( -1 != uiSerializeID )
    {
        m_strSerializeVisitedState.LoadString( uiSerializeID );
        m_bVisited = AfxGetApp()->GetProfileInt( _T( "Links" ), 
            m_strSerializeVisitedState, 0 ) ? true : false;
    }
}



CHyperLink::~CHyperLink()
{
    m_kFont.DeleteObject();
}



BEGIN_MESSAGE_MAP( CHyperLink, CStatic )
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_MOUSEMOVE()
    ON_WM_NCHITTEST()
    ON_WM_SETCURSOR()
    ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



HBRUSH 
CHyperLink::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
    if( false != m_bHovering )
    {
        pDC->SetTextColor( c_colorHover );
    }
    else
    if( false != m_bVisited )
    {
        pDC->SetTextColor( c_colorVisited );
    }
    else
    {
        pDC->SetTextColor( c_colorLink );
    }

    pDC->SetBkMode( TRANSPARENT );

    return( m_hBrush );
}



UINT 
CHyperLink::OnNcHitTest( CPoint point ) 
{   
    return( HTCLIENT );
}



void 
CHyperLink::PreSubclassWindow() 
{
    CFont* pFont( GetFont() ); 
    if( 0 != pFont  )
    {
        LOGFONT lf;
        pFont->GetLogFont( &lf );
        
        lf.lfUnderline = 1;
        if( FALSE != m_kFont.CreateFontIndirect( &lf ) )
        {
            CStatic::SetFont( &m_kFont );    
        }
    }

    CRect rectHyperLink; 
    GetClientRect( rectHyperLink );
    m_kToolTip.Create( this );
    m_kToolTip.AddTool( this, m_strLink, rectHyperLink, c_uiToolTipID );

    CStatic::PreSubclassWindow();
}



BOOL 
CHyperLink::PreTranslateMessage( MSG* pMsg ) 
{
    m_kToolTip.RelayEvent( pMsg );
    return( CStatic::PreTranslateMessage( pMsg ) );
}



BOOL 
CHyperLink::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message ) 
{
    static SLinkCursor s_sCursor;
    ::SetCursor( s_sCursor.m_hLinkCursor );
    return( TRUE );
}



void 
CHyperLink::OnMouseMove( UINT nFlags, CPoint point ) 
{
    if( false == m_bHovering )
    {
        m_bHovering = true;
        Invalidate();
        SetTimer( c_uiTimerID, 100, NULL );
    }
    
    CStatic::OnMouseMove( nFlags, point );
}



void 
CHyperLink::OnLButtonDown( UINT nFlags, CPoint point ) 
{   
    ShellExecute( NULL, _T( "open" ), m_strLink, NULL, NULL, SW_SHOWNORMAL );

    m_bVisited = true;
    if( false == m_strSerializeVisitedState.IsEmpty() )
    {
        AfxGetApp()->WriteProfileInt( _T( "Links" ), m_strSerializeVisitedState, 1 );
    }
    
    CStatic::OnLButtonDown( nFlags, point );
}



void 
CHyperLink::OnTimer( UINT nIDEvent ) 
{    
    CPoint kMousePos( GetMessagePos() );
    ScreenToClient( &kMousePos );

    CRect rectHyperLink;
    GetClientRect( rectHyperLink );

    if( FALSE == rectHyperLink.PtInRect( kMousePos ) )
    {
        m_bHovering = false;
        KillTimer( c_uiTimerID );
        Invalidate();
    }
    
    CStatic::OnTimer( nIDEvent );
}



BOOL 
CHyperLink::DestroyWindow() 
{
    KillTimer( c_uiTimerID );
    
    return( CStatic::DestroyWindow() );
}



CHyperLink::SLinkCursor::SLinkCursor()
: m_hLinkCursor( 0 )
{
    CString strWndDir;
    GetWindowsDirectory( strWndDir.GetBuffer( MAX_PATH ), MAX_PATH );
    strWndDir.ReleaseBuffer();
    strWndDir += _T( "\\winhlp32.exe" );

    HMODULE hModule( LoadLibrary( strWndDir ) );
    if( 0 != hModule )
    {
        HCURSOR hHandCursor( ::LoadCursor( hModule, MAKEINTRESOURCE( 106 ) ) );
        if( 0 != hHandCursor )
        {
            m_hLinkCursor = CopyCursor( hHandCursor );
        }
    }

    FreeLibrary( hModule );
}



CHyperLink::SLinkCursor::~SLinkCursor()
{
    DestroyCursor( m_hLinkCursor );
}

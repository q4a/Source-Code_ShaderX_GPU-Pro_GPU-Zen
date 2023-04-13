///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CBrowserShaderFormView.
 *
 * @file
 *  BrowserShaderFormView.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  08.11.2001
 * @brief
 *  Implementation of \e WindowSystem::CBrowserShaderFormView.
 */



#include "stdafx.h"
#include "BrowserShaderFormView.h"
#include "Effect.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace WindowSystem;
using namespace EffectSystem;



IMPLEMENT_DYNCREATE( CBrowserShaderFormView, CFormView )



CBrowserShaderFormView::CBrowserShaderFormView()
: CFormView( CBrowserShaderFormView::IDD )
, m_kEditVS( _T( "Vertex shader" ) )
, m_kEditPS( _T( "Pixel shader" ) )
, m_colShaders()
{
}



CBrowserShaderFormView::~CBrowserShaderFormView()
{
}



BEGIN_MESSAGE_MAP( CBrowserShaderFormView, CFormView )
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY( TCN_SELCHANGE, IDC_SHADERTAB, OnSelChangeShaderTab )
END_MESSAGE_MAP()



#ifdef _DEBUG
void CBrowserShaderFormView::AssertValid() const
{
    CFormView::AssertValid();
}

void CBrowserShaderFormView::Dump( CDumpContext& dc ) const
{
    CFormView::Dump( dc );
}
#endif //_DEBUG



int 
CBrowserShaderFormView::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
    if( -1 == CFormView::OnCreate( lpCreateStruct ) )
    {
        return( -1 );
    }
    
    unsigned int uiEditFlags( WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN | ES_READONLY |
                    WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | ES_AUTOHSCROLL );

    m_kEditVS.Create( uiEditFlags, CRect( 0, 0, 0, 0 ), this, IDC_VERTEXSHADEREDIT );
    m_kEditVS.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );

    m_kEditPS.Create( uiEditFlags, CRect( 0, 0, 0, 0 ), this, IDC_PIXELSHADEREDIT );
    m_kEditPS.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );

    return( 0 );
}



void CBrowserShaderFormView::OnSize( UINT nType, int cx, int cy ) 
{
    const int c_iBorder( 3 );
    const int c_iTabCtrlHeight( 22 );
    const int c_iEditCtrlIndent( 6 + c_iBorder );

    // set new client rect
    CRect rectClient( 0, 0, cx, cy );

    // set new scroll size to prevent scrollbars of our form view from showing up
    SetScrollSizes( MM_TEXT, CSize( rectClient.Width(), rectClient.Height() ) );

    // calculate new height for each edit ctrl
    int iEditCtrlHeight( ( rectClient.Height() - c_iTabCtrlHeight - 2 * c_iBorder ) / 2 );

    // resize tab ctrl
    CTabCtrl* pkTabCtrl( static_cast< CTabCtrl* >( GetDlgItem( IDC_SHADERTAB ) ) );      
    if( 0 != pkTabCtrl && 
        pkTabCtrl->GetSafeHwnd() )
    {
        CRect rectCopy( rectClient );
        rectCopy.DeflateRect( c_iBorder, c_iBorder, c_iBorder, c_iBorder );
        pkTabCtrl->MoveWindow( rectCopy );    
    }

    // resize vertex shader edit ctrl
    if( 0 != m_kEditVS.GetSafeHwnd() )
    {
        CRect rectCopy( rectClient );
        rectCopy.DeflateRect( c_iEditCtrlIndent, 0, c_iEditCtrlIndent, 0 );
        rectCopy.top = c_iTabCtrlHeight + c_iEditCtrlIndent;
        rectCopy.bottom = rectCopy.top + iEditCtrlHeight - c_iEditCtrlIndent;
        m_kEditVS.MoveWindow( rectCopy );
    }

    // resize pixel shader edit ctrl
    if( 0 != m_kEditPS.GetSafeHwnd() )
    {
        CRect rectCopy( rectClient );
        rectCopy.DeflateRect( c_iEditCtrlIndent, 0, c_iEditCtrlIndent, 0 );        
        rectCopy.top = c_iTabCtrlHeight + iEditCtrlHeight + c_iEditCtrlIndent - 
            c_iEditCtrlIndent / 2;
        rectCopy.bottom = rectCopy.top + iEditCtrlHeight - c_iEditCtrlIndent;
        m_kEditPS.MoveWindow( rectCopy );
    }

    CFormView::OnSize( nType, cx, cy );
}



void 
CBrowserShaderFormView::OnSelChangeShaderTab( NMHDR* pNMHDR, LRESULT* pResult ) 
{
    CTabCtrl* pkTabCtrl( static_cast< CTabCtrl* >( GetDlgItem( IDC_SHADERTAB ) ) );     

    TCITEM sItem;
    sItem.mask = TCIF_PARAM;
    if( FALSE != pkTabCtrl->GetItem( pkTabCtrl->GetCurSel(), &sItem ) )
    {
        SetShaderCode( *reinterpret_cast< SShaderSourceList::SShaderSource* >( sItem.lParam ) );
    }
    
    *pResult = 0;
}



void 
CBrowserShaderFormView::SetShaderSources( IEffect* pkEffect )
{
    CTabCtrl* pkTabCtrl( static_cast< CTabCtrl* >( GetDlgItem( IDC_SHADERTAB ) ) );
    pkTabCtrl->DeleteAllItems();

    m_colShaders.m_colEntries.clear();
    m_kEditVS.SetWindowText( _T( "" ) );
    m_kEditPS.SetWindowText( _T( "" ) );

    DBG_ASSERT( 0 != pkEffect );
    pkEffect->GetShaderSources( m_colShaders );

    SShaderSourceList::ShaderSourceList::iterator itCur( m_colShaders.m_colEntries.begin() );
    SShaderSourceList::ShaderSourceList::iterator itEnd( m_colShaders.m_colEntries.end() );
    
    unsigned int uiItemCount( 0 );
    while( itCur != itEnd )
    {
        TCITEM sItem;
        sItem.pszText = (LPSTR) (*itCur).m_strShaderName.c_str();        
        sItem.lParam  = (LPARAM) &(*itCur);
        sItem.mask    = TCIF_TEXT | TCIF_PARAM;

        pkTabCtrl->InsertItem( uiItemCount, &sItem );
        
        ++uiItemCount;
        ++itCur;
    }

    if( 0 != uiItemCount )
    {
        SetShaderCode( (*m_colShaders.m_colEntries.begin() ) );
    }
}



void 
CBrowserShaderFormView::SetShaderCode( SShaderSourceList::SShaderSource& sShaderSource )
{
    CString strCode;

    strCode.Format( _T( "%s" ), sShaderSource.m_strVertexCode.c_str() );
    m_kEditVS.SetWindowText( strCode );

    strCode.Format( _T( "%s" ), sShaderSource.m_strPixelCode.c_str() );
    m_kEditPS.SetWindowText( strCode );
}

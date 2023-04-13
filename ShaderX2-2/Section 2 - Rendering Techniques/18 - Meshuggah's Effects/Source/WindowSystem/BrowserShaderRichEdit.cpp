///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CBrowserShaderRichEdit.
 *
 * @file
 *  BrowserShaderRichEdit.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  08.11.2001
 * @brief
 *  Implementation of \e WindowSystem::CBrowserShaderRichEdit.
 */



#include "stdafx.h"
#include "BrowserShaderRichEdit.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace WindowSystem;



const c_uiToolTipID( 1 );



CBrowserShaderRichEdit::CBrowserShaderRichEdit( CString strShaderEditToolTip )
: m_kFont()
, m_kToolTip()
, m_strToolTip( strShaderEditToolTip )
{
}



CBrowserShaderRichEdit::~CBrowserShaderRichEdit()
{
    m_kFont.DeleteObject();
}


BEGIN_MESSAGE_MAP( CBrowserShaderRichEdit, CRichEditCtrl )
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()



int 
CBrowserShaderRichEdit::OnCreate( LPCREATESTRUCT lpCreateStruct )  
{
    if( -1  == CRichEditCtrl::OnCreate( lpCreateStruct ) )
    {
        return( -1 );
    }
    
    if( FALSE == m_kFont.CreatePointFont( 90, "Courier New" ) )
    {
        return( -1 );
    }

    SetFont( &m_kFont );

    m_kToolTip.Create( this );
    m_kToolTip.AddTool( this, m_strToolTip, CRect( 0, 0, 0, 0 ), c_uiToolTipID );

    return 0;
}



BOOL 
CBrowserShaderRichEdit::PreTranslateMessage( MSG* pMsg )
{
    m_kToolTip.RelayEvent( pMsg );
    return( CRichEditCtrl::PreTranslateMessage( pMsg ) );
}



void 
CBrowserShaderRichEdit::OnSize( UINT nType, int cx, int cy )
{
    CRichEditCtrl::OnSize( nType, cx, cy );

    m_kToolTip.SetToolRect( this, (UINT_PTR) c_uiToolTipID, CRect( 0, 0, cx, cy ) );
}

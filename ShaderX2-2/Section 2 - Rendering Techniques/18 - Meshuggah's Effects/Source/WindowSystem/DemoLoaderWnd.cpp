///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CDemoLoaderWnd.
 *
 * @file
 *  DemoLoaderWnd.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  02.03.2002
 * @brief
 *  Implementation of \e WindowSystem::CDemoLoaderWnd.
 */



#include "stdafx.h"
#include "DemoLoaderWnd.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace WindowSystem;



CDemoLoaderWnd::CDemoLoaderWnd()
{
}



CDemoLoaderWnd::~CDemoLoaderWnd()
{
    m_kBitmap.DeleteObject();
}



BEGIN_MESSAGE_MAP( CDemoLoaderWnd, CWnd )
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()



BOOL 
CDemoLoaderWnd::Create( CWnd* pParentWnd )
{
    if( FALSE == m_kBitmap.LoadBitmap( IDB_PLEASEWAIT ) )
    {
        return( FALSE );
    }

    BITMAP bm;
    m_kBitmap.GetBitmap( &bm );

    return( CreateEx( 0,
                      AfxRegisterWndClass( 0, AfxGetApp()->LoadStandardCursor( IDC_ARROW ) ),
                      0, 
                      WS_POPUP | WS_VISIBLE, 
                      0, 
                      0, 
                      bm.bmWidth, 
                      bm.bmHeight, 
                      pParentWnd->GetSafeHwnd(), 
                      0
                     ) );
}



int 
CDemoLoaderWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( -1 == CWnd::OnCreate( lpCreateStruct ) )
    {
        return( -1 );
    }

    CenterWindow();
    return( 0 );
}



void 
CDemoLoaderWnd::OnPaint()
{
    CPaintDC dc( this );

    CDC dcImage;
    if( FALSE == dcImage.CreateCompatibleDC( &dc ) )
    {
        return;
    }

    BITMAP bm;
    m_kBitmap.GetBitmap( &bm );

    CBitmap* pOldBitmap( dcImage.SelectObject( &m_kBitmap ) );
    dc.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY );
    dcImage.SelectObject( pOldBitmap );
}
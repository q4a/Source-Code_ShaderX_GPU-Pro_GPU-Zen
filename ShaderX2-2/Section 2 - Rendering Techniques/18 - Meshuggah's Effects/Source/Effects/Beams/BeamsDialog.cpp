///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CBeamsDialog.
 *
 * @file
 *  BeamsDialog.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Implementation of \e Effects::CBeamsDialog.
 */



#include "stdafx.h"
#include "BeamsDialog.h"
#include "Beams.h"

#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Effects;
using namespace Shared;



CBeamsDialog::CBeamsDialog( CWnd* pParent )
: CDialog( IDD_BEAMS, pParent )
, m_kTimer()
{
}



CBeamsDialog::~CBeamsDialog()
{
}



void
CBeamsDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}



BEGIN_MESSAGE_MAP( CBeamsDialog, CDialog )
END_MESSAGE_MAP()



BOOL
CBeamsDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_kTimer.Start();

    return( TRUE );
}



void
CBeamsDialog::OnOK()
{
    //CDialog::OnOK();
}



LRESULT
CBeamsDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    // pause all timers when dragging dialog
    switch( message )
    {
    case WM_MOVING:
        {
            m_kTimer.Pause();
        }
        break;

    case WM_EXITSIZEMOVE:
        {
            m_kTimer.Start();
        }
    default:
        break;
    }

    return( CDialog::WindowProc( message, wParam, lParam ) );
}



float
CBeamsDialog::GetTime()
{
    return( m_kTimer.GetTime() );
}

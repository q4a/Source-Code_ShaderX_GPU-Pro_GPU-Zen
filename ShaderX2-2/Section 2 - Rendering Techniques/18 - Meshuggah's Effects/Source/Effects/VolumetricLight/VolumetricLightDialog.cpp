///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CVolumetricLightDialog.
 *
 * @file
 *  VolumetricLightDialog.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  05.03.2002
 * @brief
 *  Implementation of \e Effects::CVolumetricLightDialog.
 */



#include "stdafx.h"
#include "VolumetricLightDialog.h"

#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Effects;
using namespace Shared;



CVolumetricLightDialog::CVolumetricLightDialog( CWnd* pParent )
: CDialog( IDD_VOLUMETRICLIGHT, pParent )
, m_kTimer()
{
}



CVolumetricLightDialog::~CVolumetricLightDialog()
{
}



void 
CVolumetricLightDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}



BEGIN_MESSAGE_MAP( CVolumetricLightDialog, CDialog )
END_MESSAGE_MAP()



BOOL 
CVolumetricLightDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();

    m_kTimer.Start();

    return( TRUE );
}



void 
CVolumetricLightDialog::OnOK()
{
    //CDialog::OnOK();
}



LRESULT 
CVolumetricLightDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
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
CVolumetricLightDialog::GetTime()
{
    return( m_kTimer.GetTime() );
}

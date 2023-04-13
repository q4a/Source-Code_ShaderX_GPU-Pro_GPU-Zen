///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CDisplacmentMapDialog.
 *
 * @file
 *  DisplacementMapDialog.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Implementation of \e Effects::CDisplacmentMapDialog.
 */



#include "stdafx.h"
#include "DisplacementMapDialog.h"
#include "DisplacementMap.h"

#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Effects;
using namespace Shared;



CDisplacementMapDialog::CDisplacementMapDialog( CWnd* pParent )
: CDialog( IDD_DISPLACEMENTMAP, pParent )
, m_iHeightScale( (int) ( CDisplacementMap::sc_fDefaultHeightScale * 256.0f * 1000.0f ) )
, m_kTimer()
{
}



CDisplacementMapDialog::~CDisplacementMapDialog()
{
}



void 
CDisplacementMapDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Slider( pDX, IDC_HEIGHTSCALE, m_iHeightScale );
}



BEGIN_MESSAGE_MAP( CDisplacementMapDialog, CDialog )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_HEIGHTSCALE, OnReleasedCaptureHeightScale )
END_MESSAGE_MAP()



BOOL 
CDisplacementMapDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_HEIGHTSCALE ) )->SetRange( 0, 100 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_HEIGHTSCALE ) )->SetTicFreq( 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_HEIGHTSCALE ) )->SetPageSize( 1 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_HEIGHTSCALE ) )->SetPos( m_iHeightScale );

    UpdateHeightScaleEdit();

    m_kTimer.Start();
    
    return( TRUE );
}



void 
CDisplacementMapDialog::OnReleasedCaptureHeightScale( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateHeightScaleEdit();
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
CDisplacementMapDialog::OnOK()
{
    //CDialog::OnOK();
}



LRESULT 
CDisplacementMapDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
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
CDisplacementMapDialog::GetTime()
{
    return( m_kTimer.GetTime() );
}



float 
CDisplacementMapDialog::GetHeightScale()
{
    return( (float) m_iHeightScale / ( 256.0f * 1000.0f ) );
}
       


void 
CDisplacementMapDialog::UpdateHeightScaleEdit()
{
    CString str;
    str.Format( "%f", GetHeightScale() * 256.0f );
    GetDlgItem( IDC_HEIGHTSCALE_EDIT )->SetWindowText( str );
}

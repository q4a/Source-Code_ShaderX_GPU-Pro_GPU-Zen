///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CSoftObjectsDialog.
 *
 * @file
 *  SoftObjectsDialog.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  13.03.2002
 * @brief
 *  Implementation of \e Effects::CSoftObjectsDialog.
 */



#include "stdafx.h"
#include "SoftObjectsDialog.h"
#include "SoftObjectsPolygonizer.h"

#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Effects;
using namespace Shared;



CSoftObjectsDialog::CSoftObjectsDialog( CWnd* pParent )
: CDialog( IDD_SOFT_OBJECTS, pParent )
, m_uiNumSoftObjects( CSoftObjectsPolygonizer::sc_uiNumSoftObjectsDefault )
, m_ucGridSize( CSoftObjectsPolygonizer::sc_ucGridSizeDefault )
, m_iIsoThreshold( (int) ( CSoftObjectsPolygonizer::sc_fIsoThresholdDefault * 1000.0f ) )
, m_kTimer()
{
    DBG_ASSERT( m_iIsoThreshold >= 100 && m_iIsoThreshold <= 1000 );
}



CSoftObjectsDialog::~CSoftObjectsDialog()
{    
}



void 
CSoftObjectsDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );

    DDX_Slider( pDX, IDC_NUM_SOFTOBJECTS, (int&) m_uiNumSoftObjects );
    DDX_Slider( pDX, IDC_GRIDSIZE, (int&) m_ucGridSize );
    DDX_Slider( pDX, IDC_THRESHOLD, m_iIsoThreshold );
}



BEGIN_MESSAGE_MAP( CSoftObjectsDialog, CDialog )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_NUM_SOFTOBJECTS, OnReleasedCaptureNumSoftObjects )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_GRIDSIZE, OnReleasedCaptureGridSize )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_ISOTHRESHOLD, OnReleasedCaptureIsoThreshold )
END_MESSAGE_MAP()



BOOL 
CSoftObjectsDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_NUM_SOFTOBJECTS ) )->SetRange( 1, 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_NUM_SOFTOBJECTS ) )->SetTicFreq( 1 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_NUM_SOFTOBJECTS ) )->SetPageSize( 1 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_NUM_SOFTOBJECTS ) )->SetPos( m_uiNumSoftObjects );

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GRIDSIZE ) )->SetRange( 10, 75 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GRIDSIZE ) )->SetTicFreq( 5 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GRIDSIZE ) )->SetPageSize( 1 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GRIDSIZE ) )->SetPos( m_ucGridSize );

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_ISOTHRESHOLD ) )->SetRange( 100, 1000 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_ISOTHRESHOLD ) )->SetTicFreq( 100 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_ISOTHRESHOLD ) )->SetPageSize( 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_ISOTHRESHOLD ) )->SetPos( m_iIsoThreshold );

    UpdateNumSoftObjectsEdit();
    UpdateGridSizeEdit();
    UpdateIsoThresholdEdit();

    m_kTimer.Start();

    return( TRUE );
}



void 
CSoftObjectsDialog::OnOK()
{
    //CDialog::OnOK();
}



void 
CSoftObjectsDialog::OnReleasedCaptureNumSoftObjects( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateNumSoftObjectsEdit();
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
CSoftObjectsDialog::OnReleasedCaptureGridSize( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateGridSizeEdit();
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
CSoftObjectsDialog::OnReleasedCaptureIsoThreshold( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateIsoThresholdEdit();
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



float 
CSoftObjectsDialog::GetTime()
{
    return( m_kTimer.GetTime() );
}



unsigned int 
CSoftObjectsDialog::GetNumSoftObjects()
{
    return( m_uiNumSoftObjects );
}



unsigned char
CSoftObjectsDialog::GetGridSize()
{
    return( m_ucGridSize );
}



float
CSoftObjectsDialog::GetIsoThreshold()
{
    return( m_iIsoThreshold * 0.001f );
}



LRESULT 
CSoftObjectsDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
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



void 
CSoftObjectsDialog::UpdateNumSoftObjectsEdit()
{
    CString str;
    str.Format( "%d", GetNumSoftObjects() );
    GetDlgItem( IDC_NUM_SOFTOBJECTS_EDIT )->SetWindowText( str );
}



void 
CSoftObjectsDialog::UpdateGridSizeEdit()
{
    CString str;
    str.Format( "%d", GetGridSize() );
    GetDlgItem( IDC_GRIDSIZE_EDIT )->SetWindowText( str );
}
        


void 
CSoftObjectsDialog::UpdateIsoThresholdEdit()
{
    CString str;
    str.Format( "%f", GetIsoThreshold() );
    GetDlgItem( IDC_ISOTHRESHOLD_EDIT )->SetWindowText( str );
}
///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CInfiniteZoomDialog.
 *
 * @file
 *  InfiniteZoomDialog.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  25.02.2002
 * @brief
 *  Implementation of \e Effects::CInfiniteZoomDialog.
 */



#include "stdafx.h"
#include "InfiniteZoomDialog.h"

#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Effects;
using namespace Shared;



const int c_iSliderLength( 1000 );



CInfiniteZoomDialog::CInfiniteZoomDialog( CWnd* pParent )
: CDialog( IDD_INFINITE_ZOOM, pParent )
, m_iZoomPos( 0 )
, m_bRotateZ( FALSE )
, m_bAnimate( FALSE )
, mc_fZoomLength( 30.0f )
, m_kTimer()
, m_kRotationTimer()
{
}



CInfiniteZoomDialog::~CInfiniteZoomDialog()
{
}



void 
CInfiniteZoomDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );

    DDX_Slider( pDX, IDC_ZOOMPOS, m_iZoomPos );
    DDX_Check( pDX, IDC_ROTATEZ, m_bRotateZ );
    DDX_Check( pDX, IDC_ANIMATE, m_bAnimate );
}



BEGIN_MESSAGE_MAP( CInfiniteZoomDialog, CDialog )
    ON_BN_CLICKED( IDC_ANIMATE, OnAnimate )
    ON_BN_CLICKED( IDC_ROTATEZ, OnRotateZ )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_ZOOMPOS, OnReleasedCaptureZoomPos )
END_MESSAGE_MAP()



BOOL 
CInfiniteZoomDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_ZOOMPOS ) )->SetRange( 0, c_iSliderLength );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_ZOOMPOS ) )->SetTicFreq( c_iSliderLength / 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_ZOOMPOS ) )->SetPageSize( c_iSliderLength / 100 );
    
    return( TRUE );
}



void 
CInfiniteZoomDialog::OnRotateZ()
{
    if( FALSE != UpdateData() )
    {
        if( FALSE != m_bRotateZ ) 
        {
            m_kRotationTimer.Start();
        }
        else
        {
            m_kRotationTimer.Pause();
        }
    }
}



void 
CInfiniteZoomDialog::OnAnimate()
{
    if( FALSE != UpdateData() )
    {
        static_cast< CSliderCtrl* >( GetDlgItem( IDC_ZOOMPOS ) )->EnableWindow( !m_bAnimate );
        static_cast< CButton* >( GetDlgItem( IDC_ROTATEZ ) )->EnableWindow( m_bAnimate );

        if( FALSE != m_bAnimate )
        {
            int iSliderPos( static_cast< CSliderCtrl* >( GetDlgItem( IDC_ZOOMPOS ) )->GetPos() );      
        
            m_kTimer.SetTime( mc_fZoomLength * (float) iSliderPos / (float) c_iSliderLength );
            m_kTimer.Start();
        }
        else
        {
            m_kTimer.Stop();

            static_cast< CButton* >( GetDlgItem( IDC_ROTATEZ ) )->SetCheck( BST_UNCHECKED );
            OnRotateZ();
        }
    }
}



void
CInfiniteZoomDialog::OnReleasedCaptureZoomPos( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
CInfiniteZoomDialog::OnOK()
{
    CDialog::OnOK();
}



float 
CInfiniteZoomDialog::GetZoomLength() const
{
    return( mc_fZoomLength );
}



float 
CInfiniteZoomDialog::GetZoomPosition() const
{
    if( FALSE != m_bAnimate )
    {
        float fZoomPos( fmodf( m_kTimer.GetTime(), mc_fZoomLength ) );
        
        static_cast< CSliderCtrl* >( GetDlgItem( IDC_ZOOMPOS ) )->
            SetPos( (int) ( c_iSliderLength * fZoomPos / mc_fZoomLength ) );        

        return( fZoomPos );
    }
    else
    {
        // make sure that the return value is in [0, mc_fZoomLength) !!!
        return( mc_fZoomLength * ( (float) m_iZoomPos / (float) ( c_iSliderLength + 1 ) ) );
    }    
}



float 
CInfiniteZoomDialog::GetZRotationAngle() const
{
    return( -0.5f * m_kRotationTimer.GetTime() );
}



LRESULT 
CInfiniteZoomDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    // pause all timers when dragging dialog
    switch( message )
    {
    case WM_MOVING:
        {
            m_kTimer.Pause();
            if( FALSE != m_bRotateZ )
            {
                m_kRotationTimer.Pause();
            }
        }
        break;

    case WM_EXITSIZEMOVE:
        {
            m_kTimer.Start();
            if( FALSE != m_bRotateZ )
            {
                m_kRotationTimer.Start();
            }
        }
    default:
        break;
    }

    return( CDialog::WindowProc( message, wParam, lParam ) );
}

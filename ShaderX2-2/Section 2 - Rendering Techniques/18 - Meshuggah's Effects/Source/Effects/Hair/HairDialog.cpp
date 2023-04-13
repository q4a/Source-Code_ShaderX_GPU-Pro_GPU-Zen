///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CHairDialog.
 *
 * @file
 *  HairDialog.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  06.03.2002
 * @brief
 *  Implementation of \e Effects::CHairDialog.
 */



#include "stdafx.h"
#include "HairDialog.h"
#include "Hair.h"

#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Effects;
using namespace Shared;



CColorCtrl::CColorCtrl( const COLORREF& col )
: m_hBrush( ::CreateSolidBrush( col ) )
{
}



CColorCtrl::~CColorCtrl()
{
    ::DeleteObject( m_hBrush );
}



BEGIN_MESSAGE_MAP( CColorCtrl, CStatic )
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



HBRUSH 
CColorCtrl::CtlColor( CDC* pDC, UINT nCtlColor )
{
    return( m_hBrush );
}



void
CColorCtrl::SetColor( const COLORREF& col )
{
    if( 0 != m_hBrush )
    {
        ::DeleteObject( m_hBrush );
    }   
    m_hBrush = ::CreateSolidBrush( col );
}



CHairDialog::CHairDialog( const D3DXVECTOR4& vDiffuseColor, 
                          const D3DXVECTOR4& vSpecularColor,
                          const D3DXVECTOR4& vAnisotropySettings,
                          CWnd* pParent )
: CDialog( IDD_HAIR, pParent )
, m_sDiffuseColor( RGB( (int) ( vDiffuseColor.x * 255 ),
                        (int) ( vDiffuseColor.y * 255 ),
                        (int) ( vDiffuseColor.z * 255 ) ) )
, m_sSpecularColor( RGB( (int) ( vSpecularColor.x * 255 ),
                         (int) ( vSpecularColor.y * 255 ),
                         (int) ( vSpecularColor.z * 255 ) ) )
, m_kDiffuseColorCtrl( RGB( 0, 0, 0 ) )
, m_kSpecularColorCtrl( RGB( 0, 0, 0 ) )
, m_iAmbient( (int) ( vAnisotropySettings.x * 100.0f ) )
, m_iDiffuse( (int) ( vAnisotropySettings.y * 100.0f ) )
, m_iSpecular( (int) ( vAnisotropySettings.z * 100.0f ) )
, m_iGloss( (int) vAnisotropySettings.w )
, m_bAnsiotropyParamsChanged( false )
, m_iGravity( (int) ( -CHair::sc_fDefaultGravity * 1e6 ) )
, m_iInertia( (int) (  CHair::sc_fDefaultInertia * 1e7 ) )
, m_kTimer()
{
}



CHairDialog::~CHairDialog()
{
}



void 
CHairDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );

    DDX_Slider( pDX, IDC_AMBIENT, m_iAmbient );
    DDX_Slider( pDX, IDC_DIFFUSE, m_iDiffuse );
    DDX_Slider( pDX, IDC_SPECULAR, m_iSpecular );
    DDX_Slider( pDX, IDC_GLOSS, m_iGloss );
    DDX_Slider( pDX, IDC_GRAVITY, m_iGravity );
    DDX_Slider( pDX, IDC_INERTIA, m_iInertia );

    DDX_Control( pDX, IDC_DIFF_COLOR, m_kDiffuseColorCtrl );
    DDX_Control( pDX, IDC_SPEC_COLOR, m_kSpecularColorCtrl );
}



BEGIN_MESSAGE_MAP( CHairDialog, CDialog )
    ON_BN_CLICKED( IDC_DIFFUSE_COLOR, OnDiffuseColor )
    ON_BN_CLICKED( IDC_SPECULAR_COLOR, OnSpecularColor )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_AMBIENT, OnReleasedCaptureAmbient )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_DIFFUSE, OnReleasedCaptureDiffuse )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_SPECULAR, OnReleasedCaptureSpecular )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_GLOSS, OnReleasedCaptureGloss )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_GRAVITY, OnReleasedCaptureGravity )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_INERTIA, OnReleasedCaptureInertia )
END_MESSAGE_MAP()



BOOL 
CHairDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_AMBIENT ) )->SetRange( 0, 100 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_AMBIENT ) )->SetTicFreq( 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_AMBIENT ) )->SetPageSize( 1 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_AMBIENT ) )->SetPos( m_iAmbient );

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIFFUSE ) )->SetRange( 0, 100 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIFFUSE ) )->SetTicFreq( 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIFFUSE ) )->SetPageSize( 1 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIFFUSE ) )->SetPos( m_iDiffuse );

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_SPECULAR ) )->SetRange( 0, 100 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_SPECULAR ) )->SetTicFreq( 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_SPECULAR ) )->SetPageSize( 1 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_SPECULAR ) )->SetPos( m_iSpecular );

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GLOSS ) )->SetRange( 0, 100 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GLOSS ) )->SetTicFreq( 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GLOSS ) )->SetPageSize( 1 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GLOSS ) )->SetPos( m_iGloss );

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GRAVITY ) )->SetRange( 0, 50000 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GRAVITY ) )->SetTicFreq( 2500 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GRAVITY ) )->SetPageSize( 250 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_GRAVITY ) )->SetPos( m_iGravity );

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_INERTIA ) )->SetRange( 0, 200000 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_INERTIA ) )->SetTicFreq( 10000 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_INERTIA ) )->SetPageSize( 1000 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_INERTIA ) )->SetPos( m_iInertia );

    m_kDiffuseColorCtrl.SetColor( m_sDiffuseColor );
    m_kSpecularColorCtrl.SetColor( m_sSpecularColor );

    UpdateAmbientEdit();
    UpdateDiffuseEdit();
    UpdateSpecularEdit();
    UpdateGlossEdit();
    UpdateGravityEdit();
    UpdateInertiaEdit();

    m_kTimer.Start();

    return( TRUE );
}



void 
CHairDialog::OnOK()
{
    //CDialog::OnOK();
}



void 
CHairDialog::OnReleasedCaptureAmbient( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        *pResult = 0;
        m_bAnsiotropyParamsChanged = true;
        UpdateAmbientEdit();
    }
    else
    {
        *pResult = 1;
    }
}



void 
CHairDialog::OnReleasedCaptureDiffuse( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        *pResult = 0;
        m_bAnsiotropyParamsChanged = true;
        UpdateDiffuseEdit();
    }
    else
    {
        *pResult = 1;
    }
}



void 
CHairDialog::OnReleasedCaptureSpecular( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        *pResult = 0;
        m_bAnsiotropyParamsChanged = true;
        UpdateSpecularEdit();
    }
    else
    {
        *pResult = 1;
    }
}



void 
CHairDialog::OnReleasedCaptureGloss( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        *pResult = 0;
        m_bAnsiotropyParamsChanged = true;
        UpdateGlossEdit();
    }
    else
    {
        *pResult = 1;
    }
}



void 
CHairDialog::OnReleasedCaptureGravity( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        *pResult = 0;
        UpdateGravityEdit();
    }
    else
    {
        *pResult = 1;
    }
}



void 
CHairDialog::OnReleasedCaptureInertia( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        *pResult = 0;
        UpdateInertiaEdit();
    }
    else
    {
        *pResult = 1;
    }
}



void
CHairDialog::OnDiffuseColor()
{
    m_kTimer.Pause();

    CColorDialog kDlg( m_sDiffuseColor, CC_FULLOPEN | CC_SOLIDCOLOR );
    if( IDOK == kDlg.DoModal() )
    {
        m_sDiffuseColor = kDlg.GetColor();
        m_kDiffuseColorCtrl.SetColor( m_sDiffuseColor );
    }

    m_kTimer.Start();
}



void
CHairDialog::OnSpecularColor()
{
    m_kTimer.Pause();

    CColorDialog kDlg( m_sSpecularColor, CC_FULLOPEN | CC_SOLIDCOLOR );
    if( IDOK == kDlg.DoModal() )
    {
        m_sSpecularColor = kDlg.GetColor();
        m_kSpecularColorCtrl.SetColor( m_sSpecularColor );
    }

    m_kTimer.Start();
}



LRESULT 
CHairDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
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
CHairDialog::GetTime()
{
    return( m_kTimer.GetTime() );
}



void 
CHairDialog::GetDiffuseColor( D3DXVECTOR4& vDiffuseColor )
{
    vDiffuseColor = D3DXVECTOR4( GetRValue( m_sDiffuseColor ) / 255.0f,
                                 GetGValue( m_sDiffuseColor ) / 255.0f,
                                 GetBValue( m_sDiffuseColor ) / 255.0f,
                                 1.0f );
}



void 
CHairDialog::GetSpecularColor( D3DXVECTOR4& vSpecularColor )
{
    vSpecularColor = D3DXVECTOR4( GetRValue( m_sSpecularColor ) / 255.0f,
                                  GetGValue( m_sSpecularColor ) / 255.0f,
                                  GetBValue( m_sSpecularColor ) / 255.0f,
                                  1.0f );
}



bool
CHairDialog::GetAnisotropySettings( D3DXVECTOR4& vAnisotropySettings )
{
    if( false != m_bAnsiotropyParamsChanged )
    {
        vAnisotropySettings = D3DXVECTOR4( m_iAmbient / 100.0f,
                                           m_iDiffuse / 100.0f,
                                           m_iSpecular / 100.0f,
                                           (float) m_iGloss );

        m_bAnsiotropyParamsChanged = false;
        return( true );
    }

    return( false );
}



float 
CHairDialog::GetGravity()
{
    return( (float) m_iGravity * -1e-6f );
}



float 
CHairDialog::GetInertia()
{
    return( (float) m_iInertia * 1e-7f );
}



void 
CHairDialog::UpdateAmbientEdit()
{
    CString str;
    str.Format( "%f", m_iAmbient / 100.0f );
    GetDlgItem( IDC_AMBIENT_EDIT )->SetWindowText( str );
}



void 
CHairDialog::UpdateDiffuseEdit()
{
    CString str;
    str.Format( "%f", m_iDiffuse / 100.0f );
    GetDlgItem( IDC_DIFFUSE_EDIT )->SetWindowText( str );
}



void 
CHairDialog::UpdateSpecularEdit()
{
    CString str;
    str.Format( "%f", m_iSpecular / 100.0f );
    GetDlgItem( IDC_SPECULAR_EDIT )->SetWindowText( str );
}



void 
CHairDialog::UpdateGlossEdit()
{
    CString str;
    str.Format( "%f", (float) m_iGloss );
    GetDlgItem( IDC_GLOSS_EDIT )->SetWindowText( str );
}



void 
CHairDialog::UpdateGravityEdit()
{
    CString str;
    str.Format( "%f", -GetGravity() );
    GetDlgItem( IDC_GRAVITY_EDIT )->SetWindowText( str );
}



void 
CHairDialog::UpdateInertiaEdit()
{
    CString str;
    str.Format( "%f", GetInertia() );
    GetDlgItem( IDC_INERTIA_EDIT )->SetWindowText( str );
}

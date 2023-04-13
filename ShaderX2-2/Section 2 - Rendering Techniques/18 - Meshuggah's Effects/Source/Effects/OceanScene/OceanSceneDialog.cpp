///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::COceanSceneDialog.
 *
 * @file
 *  OceanScene.h
 * @author
 *  Carsten Wenzel
 * @date
 *  07.03.2002
 * @brief
 *  Implementation of \e Effects::COceanSceneDialog.
 */



#include "stdafx.h"
#include "OceanSceneDialog.h"
#include "OceanWaveGenerator.h"
#include "OceanScene.h"

#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Effects;
using namespace Shared;



COceanSceneDialog::COceanSceneDialog( CWnd* pParent )
: CDialog( IDD_OCEANSCENE, pParent )
, m_iWindDir( ( (int) D3DXToDegree( COceanWaveGenerator::sc_fDefaultWindDirection ) ) )
, m_iWindSpeed( (int) COceanWaveGenerator::sc_fDefaultWindSpeed )
, m_iWaveHeight( (int) COceanWaveGenerator::sc_fDefaultWaveHeight )
, m_iDirectionalDependence( (int) COceanWaveGenerator::sc_fDefaultDirectionalDependence )
, m_iChoppyWavesFactor( (int) ( COceanWaveGenerator::sc_fDefaultChoppyWaveFactor * 1e3f ) )
, m_iSuppressSmallWavesFactor( (int) ( COceanWaveGenerator::sc_fDefaultSuppressSmallWavesFactor * 1e5f ) )
, m_iDynamicRangeOfEnvMap( (int) ( COceanScene::sc_fDefaultDynamicRangeOfEnvMap * 1e2f ) )
, m_ulSeed( 0 )
, m_bSettingsChanged( false )
, m_kTimer()
, m_kRnd()
{
}



COceanSceneDialog::~COceanSceneDialog()
{
}



void 
COceanSceneDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Slider( pDX, IDC_WINDDIR, m_iWindDir );
    DDX_Slider( pDX, IDC_WINDSPEED, m_iWindSpeed );
    DDX_Slider( pDX, IDC_WAVEHEIGHT, m_iWaveHeight );
    DDX_Slider( pDX, IDC_DIRDEPENDENCE, m_iDirectionalDependence );
    DDX_Slider( pDX, IDC_CHOPPYWAVES, m_iChoppyWavesFactor );
    DDX_Slider( pDX, IDC_SUPPRESS_SMALLWAVES, m_iSuppressSmallWavesFactor );
    DDX_Slider( pDX, IDC_DYNAMICRANGE, m_iDynamicRangeOfEnvMap );
}



BEGIN_MESSAGE_MAP( COceanSceneDialog, CDialog )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_WINDDIR, OnReleasedCaptureWindDirection )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_WINDSPEED, OnReleasedCaptureWindSpeed )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_WAVEHEIGHT, OnReleasedCaptureWaveHeight )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_DIRDEPENDENCE, OnReleasedCaptureDirectionalDependence )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_CHOPPYWAVES, OnReleasedCaptureChoppyWavesFactor )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_SUPPRESS_SMALLWAVES, OnReleasedCaptureSuppressSmallWavesFactor )
    ON_BN_CLICKED( IDC_NEWHEIGHTDATA, OnNewHeightData )
    ON_NOTIFY( NM_RELEASEDCAPTURE, IDC_DYNAMICRANGE, OnReleasedCaptureDynamicRangeOfEnvMap )
END_MESSAGE_MAP()



BOOL 
COceanSceneDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WINDDIR ) )->SetRange( 0, 360 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WINDDIR ) )->SetTicFreq( 30 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WINDDIR ) )->SetPageSize( 1 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WINDDIR ) )->SetPos( m_iWindDir );
    UpdateWindDirectionEdit();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WINDSPEED ) )->SetRange( 1, 51 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WINDSPEED ) )->SetTicFreq( 5 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WINDSPEED ) )->SetPageSize( 1 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WINDSPEED ) )->SetPos( m_iWindSpeed );
    UpdateWindSpeedEdit();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WAVEHEIGHT ) )->SetRange( 0, 512 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WAVEHEIGHT ) )->SetTicFreq( 32 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WAVEHEIGHT ) )->SetPageSize( 8 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_WAVEHEIGHT ) )->SetPos( m_iWaveHeight );
    UpdateWaveHeightEdit();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIRDEPENDENCE ) )->SetRange( 2, 16 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIRDEPENDENCE ) )->SetTicFreq( 2 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIRDEPENDENCE ) )->SetPageSize( 2 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIRDEPENDENCE ) )->SetPos( m_iDirectionalDependence );
    UpdateDirectionalDependenceEdit();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_CHOPPYWAVES ) )->SetRange( 0, 50 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_CHOPPYWAVES ) )->SetTicFreq( 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_CHOPPYWAVES ) )->SetPageSize( 1 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_CHOPPYWAVES ) )->SetPos( m_iChoppyWavesFactor );
    UpdateChoppyWavesFactorEdit();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_SUPPRESS_SMALLWAVES ) )->SetRange( 0, 50 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_SUPPRESS_SMALLWAVES ) )->SetTicFreq( 10 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_SUPPRESS_SMALLWAVES ) )->SetPageSize( 1 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_SUPPRESS_SMALLWAVES ) )->SetPos( m_iSuppressSmallWavesFactor );
    UpdateSuppressSmallWavesFactorEdit();

    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DYNAMICRANGE ) )->SetRange( 100, 400 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DYNAMICRANGE ) )->SetTicFreq( 100 );
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DYNAMICRANGE ) )->SetPageSize( 20 );    
    static_cast< CSliderCtrl* >( GetDlgItem( IDC_DYNAMICRANGE ) )->SetPos( m_iDynamicRangeOfEnvMap );
    UpdateDynamicRangeOfEnvMapEdit();

    m_kTimer.Start();

    return( TRUE );
}



void 
COceanSceneDialog::OnReleasedCaptureWindDirection( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateWindDirectionEdit();
        m_bSettingsChanged = true;
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
COceanSceneDialog::OnReleasedCaptureWindSpeed( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateWindSpeedEdit();
        m_bSettingsChanged = true;
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
COceanSceneDialog::OnReleasedCaptureWaveHeight( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateWaveHeightEdit();
        m_bSettingsChanged = true;
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
COceanSceneDialog::OnReleasedCaptureDirectionalDependence( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        // make sure m_iDirectionalDependence isn't odd
        if( 0 != ( m_iDirectionalDependence & 0x01 ) )
        {
            m_iDirectionalDependence &= 0xFFFFFFFE;
            static_cast< CSliderCtrl* >( GetDlgItem( IDC_DIRDEPENDENCE ) )->SetPos( m_iDirectionalDependence );
        }

        UpdateDirectionalDependenceEdit();
        m_bSettingsChanged = true;
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
COceanSceneDialog::OnReleasedCaptureChoppyWavesFactor( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateChoppyWavesFactorEdit();
        m_bSettingsChanged = true;
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
COceanSceneDialog::OnReleasedCaptureSuppressSmallWavesFactor( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateSuppressSmallWavesFactorEdit();
        m_bSettingsChanged = true;
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void
COceanSceneDialog::OnNewHeightData()
{
    m_ulSeed = m_kRnd.GetInteger();
    m_bSettingsChanged = true;
}



void 
COceanSceneDialog::OnReleasedCaptureDynamicRangeOfEnvMap( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( FALSE != UpdateData() )
    {
        UpdateDynamicRangeOfEnvMapEdit();
        *pResult = 0;
    }
    else
    {
        *pResult = 1;
    }
}



void 
COceanSceneDialog::OnOK()
{
    //CDialog::OnOK();
}



LRESULT 
COceanSceneDialog::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
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
COceanSceneDialog::GetTime()
{
    return( m_kTimer.GetTime() );
}



float 
COceanSceneDialog::GetDynamicRangeOfEnvMap()
{
    return( (float) m_iDynamicRangeOfEnvMap * 1e-2f );
}



void 
COceanSceneDialog::UpdateOceanSettings( COceanWaveGenerator& kOceanWaveGenerator )
{
    if( false != m_bSettingsChanged )
    {
        kOceanWaveGenerator.SetOceanParameters( 
                                m_ulSeed,
                                D3DXToRadian( (float) m_iWindDir ),
                                (float) m_iWindSpeed,
                                (float) m_iWaveHeight,
                                (float) m_iDirectionalDependence,
                                (float) m_iChoppyWavesFactor * 1e-3f,
                                (float) m_iSuppressSmallWavesFactor * 1e-5f
                                            );
        m_bSettingsChanged = false;
    }
}



void 
COceanSceneDialog::UpdateWindDirectionEdit()
{
    CString str;
    str.Format( "%d", m_iWindDir );
    GetDlgItem( IDC_WINDDIR_EDIT )->SetWindowText( str );
}



void 
COceanSceneDialog::UpdateWindSpeedEdit()
{
    CString str;
    str.Format( "%d", m_iWindSpeed );
    GetDlgItem( IDC_WINDSPEED_EDIT )->SetWindowText( str );
}



void 
COceanSceneDialog::UpdateWaveHeightEdit()
{
    CString str;
    str.Format( "%d", m_iWaveHeight );
    GetDlgItem( IDC_WAVEHEIGHT_EDIT )->SetWindowText( str );
}



void 
COceanSceneDialog::UpdateDirectionalDependenceEdit()
{
    CString str;
    str.Format( "%d", m_iDirectionalDependence );
    GetDlgItem( IDC_DIRDEPENDENCE_EDIT )->SetWindowText( str );
}



void 
COceanSceneDialog::UpdateChoppyWavesFactorEdit()
{
    CString str;
    str.Format( "%1.3f", (float) m_iChoppyWavesFactor * 1e-3f );
    GetDlgItem( IDC_CHOPPYWAVES_EDIT )->SetWindowText( str );
}



void 
COceanSceneDialog::UpdateSuppressSmallWavesFactorEdit()
{
    CString str;
    str.Format( "%1.5f", (float) m_iSuppressSmallWavesFactor * 1e-5f );
    GetDlgItem( IDC_SUPPRESS_SMALLWAVES_EDIT )->SetWindowText( str );
}



void
COceanSceneDialog::UpdateDynamicRangeOfEnvMapEdit()
{
    CString str;
    str.Format( "%1.2f", GetDynamicRangeOfEnvMap() );
    GetDlgItem( IDC_DYNAMICRANGE_EDIT )->SetWindowText( str );
}
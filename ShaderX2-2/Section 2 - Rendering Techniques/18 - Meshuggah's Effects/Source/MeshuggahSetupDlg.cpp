///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Application::CMeshuggahSetupDlg.
 * 
 * @file
 *  MeshuggahSetupDlg.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  12.10.2001
 * @brief
 *  Implementation of \e Application::CMeshuggahSetupDlg.
 */



#include "stdafx.h"
#include "Meshuggah.h"
#include "MeshuggahSetupDlg.h"
#include "D3DInit.h"
#include "EffectException.h"

#include "Help\ContextIDs.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Application;
using namespace Shared;
using namespace EffectSystem;



struct SDisplayMode
{
    unsigned int uiWidth;
    unsigned int uiHeight;
    unsigned int uiCtrlID;

    bool bAvailable;
};



const unsigned int c_uiNumDisplayModes( 6 );

static SDisplayMode s_sModeList[ c_uiNumDisplayModes ] = 
{ 
    {  640,  480, IDC_640X480,      false }, 
    {  800,  600, IDC_800X600,      false }, 
    { 1024,  768, IDC_1024X768,     false },
    { 1152,  864, IDC_1152X864,     false },
    { 1280,  960, IDC_1280X960,     false }, 
    { 1280, 1024, IDC_1280X1024,    false },    
};




CMeshuggahSetupDlg::CMeshuggahSetupDlg( D3DPRESENT_PARAMETERS& sD3DPresParams, CWnd* pParent )
: CDialog( CMeshuggahSetupDlg::IDD, pParent )
, m_kLinkOnion( IDS_LINK_ONION, IDS_LINK_ONION_VISITED )
, m_kLink4fo( IDS_LINK_4FO, IDS_LINK_4FO_VISITED )
, m_kLinkUpdate( IDS_LINK_UPDATE, IDS_LINK_UPDATE_VISITED )
, m_iResolutionID( AfxGetApp()->GetProfileInt( _T( "Settings" ), _T( "DemoResolution" ), 0 ) )
, m_bFullscreen( AfxGetApp()->GetProfileInt( _T( "Settings" ), _T( "Fullscreen" ), 1 ) )
, m_bTripleBuffering( AfxGetApp()->GetProfileInt( _T( "Settings" ), _T( "TripleBuffering" ), 0 ) )
, m_bMusic( AfxGetApp()->GetProfileInt( _T( "Settings" ), _T( "MusicEnabled" ), 1 ) )
, m_bLoop( AfxGetApp()->GetProfileInt( _T( "Settings" ), _T( "LoopDemo" ), 0 ) )
, m_eStartUpMode( DEFAULT )
, m_sD3DPresParams( sD3DPresParams )
, m_pD3D( 0 )
{
    if( BROWSER == AfxGetApp()->GetProfileInt( _T( "Settings" ), _T( "StartUp" ), DEFAULT ) )
    {
        m_eStartUpMode = BROWSER;
    }

    memset( &m_sD3DPresParams, 0, sizeof( D3DPRESENT_PARAMETERS ) );
}



CMeshuggahSetupDlg::~CMeshuggahSetupDlg()
{
    if( 0 != m_pD3D )
    {
        m_pD3D->Release();
    }
}



void 
CMeshuggahSetupDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );

    DDX_Control( pDX, IDC_LINK_ONION, m_kLinkOnion );
    DDX_Control( pDX, IDC_LINK_4FO, m_kLink4fo );
    DDX_Control( pDX, IDC_LINK_UPDATE, m_kLinkUpdate );

    DDX_Radio( pDX, IDC_640X480, m_iResolutionID );
    DDX_Check( pDX, IDC_FULLSCREEN, m_bFullscreen );
    DDX_Check( pDX, IDC_LOOP, m_bLoop );
    DDX_Check( pDX, IDC_MUSIC, m_bMusic );
    DDX_Check( pDX, IDC_TRIPLE_BUFFERING, m_bTripleBuffering );
}



BEGIN_MESSAGE_MAP( CMeshuggahSetupDlg, CDialog )
    ON_BN_CLICKED( IDC_DEMO, OnDemo )
    ON_BN_CLICKED( IDC_BROWSER, OnBrowser )
    ON_BN_CLICKED( IDC_FULLSCREEN, OnFullscreen )
    ON_BN_CLICKED( IDC_HTMLHELP, OnHtmlHelp )
    ON_WM_DESTROY()
END_MESSAGE_MAP()



CMeshuggahSetupDlg::EStartUpMode 
CMeshuggahSetupDlg::GetStartUpMode()
{
    return( m_eStartUpMode );
}



bool
CMeshuggahSetupDlg::SetPresentParameters()
{
    DBG_ASSERT( FALSE == ( BROWSER == m_eStartUpMode ) || 
                FALSE == m_bFullscreen );

    // set the fullscreen flag for possible effect exceptions
    CEffectException::SetFullscreenFlag( ( FALSE != m_bFullscreen ) ? true : false );

    // fill present parameter structure
    m_sD3DPresParams.BackBufferWidth            = ( FALSE != m_bFullscreen ) ? s_sModeList[ m_iResolutionID ].uiWidth : 0;
    m_sD3DPresParams.BackBufferHeight           = ( FALSE != m_bFullscreen ) ? s_sModeList[ m_iResolutionID ].uiHeight : 0;
    m_sD3DPresParams.BackBufferCount            = ( FALSE != m_bFullscreen && FALSE != m_bTripleBuffering ) ? 2 : 1;
    m_sD3DPresParams.MultiSampleType            = D3DMULTISAMPLE_NONE;      
    m_sD3DPresParams.SwapEffect                 = ( FALSE != m_bFullscreen ) ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_COPY;   
    m_sD3DPresParams.Windowed                   = ( FALSE != m_bFullscreen ) ? FALSE : TRUE;
    m_sD3DPresParams.EnableAutoDepthStencil     = TRUE;
    m_sD3DPresParams.Flags                      = 0; // or D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    m_sD3DPresParams.FullScreen_RefreshRateInHz = ( FALSE != m_bFullscreen ) ? D3DPRESENT_RATE_DEFAULT : 0;    
    
    if( FALSE != m_bFullscreen )
    {
        if( FALSE != m_bTripleBuffering )
        {
            m_sD3DPresParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        }
        else
        {
            m_sD3DPresParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        }
    }
    else
    {
        m_sD3DPresParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    }

    bool bRet( true );
    if( FALSE != m_bFullscreen )
    {
        if( false == CD3DInit::GetInstance().Get32bitColorBufferFormat( m_pD3D, m_sD3DPresParams.BackBufferFormat ) )
        {
            AfxMessageBox( IDS_D3D_NO_COLORBUFFER );
            bRet = false;
        }
    }
    else
    {
        D3DDISPLAYMODE sDisplayMode;
        if( SUCCEEDED( m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &sDisplayMode ) ) )
        {
            m_sD3DPresParams.BackBufferFormat = sDisplayMode.Format;
        }
        else
        {
            AfxMessageBox( IDS_D3D_NO_ADAPTER_DISPLAYMODE );
            bRet = false;
        }
        
    }

    if( false != bRet && 
        false == CD3DInit::GetInstance().GetZBufferFormat( m_pD3D, m_sD3DPresParams.BackBufferFormat, 
                                                           m_sD3DPresParams.AutoDepthStencilFormat )  )
    {
        AfxMessageBox( IDS_D3D_NO_SUITABLE_ZBUFFER );       
        bRet = false;
    }

    return( bRet );
}



bool 
CMeshuggahSetupDlg::LoopDemo()
{
    return( ( false != m_bLoop ) ? true : false );
}



bool 
CMeshuggahSetupDlg::MusicEnabled()
{
    return( ( false != m_bMusic ) ? true : false );
}



void 
CMeshuggahSetupDlg::OnOK() 
{
    UpdateData();

    AfxGetApp()->WriteProfileInt( _T( "Settings" ), _T( "StartUp" ), m_eStartUpMode );
    AfxGetApp()->WriteProfileInt( _T( "Settings" ), _T( "DemoResolution" ), m_iResolutionID );
    AfxGetApp()->WriteProfileInt( _T( "Settings" ), _T( "Fullscreen" ), m_bFullscreen );
    AfxGetApp()->WriteProfileInt( _T( "Settings" ), _T( "TripleBuffering" ), m_bTripleBuffering );
    AfxGetApp()->WriteProfileInt( _T( "Settings" ), _T( "MusicEnabled" ), m_bMusic );
    AfxGetApp()->WriteProfileInt( _T( "Settings" ), _T( "LoopDemo" ), m_bLoop );

    if( false != SetPresentParameters() )
    {
        CDialog ::OnOK();
    }
    else
    {
        CDialog ::OnCancel();
    }
}



void 
CMeshuggahSetupDlg::OnDemo() 
{
    UpdateData();
    m_eStartUpMode = DEMO;
    UpdateDlgItems();
}



void 
CMeshuggahSetupDlg::OnBrowser() 
{
    UpdateData();
    m_eStartUpMode = BROWSER;
    UpdateDlgItems();
}



void 
CMeshuggahSetupDlg::OnFullscreen() 
{
    UpdateData();
    UpdateDlgItems();
}



BOOL 
CMeshuggahSetupDlg::OnInitDialog() 
{
    CDialog ::OnInitDialog();

    HICON hIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ) );
    SetIcon( hIcon, TRUE );
    SetIcon( hIcon, FALSE );

    m_pD3D = CD3DInit::GetInstance().Direct3DCreate8();
    if( 0 == m_pD3D )
    {
        AfxMessageBox( IDS_D3D_NODX81 );
        
        CDialog::OnCancel();
        return( FALSE );
    }

    unsigned int uiNumDispModes( m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT ) );
    if( 0 == uiNumDispModes )
    {
        AfxMessageBox( IDS_D3D_NODISPLAYMODES );

        CDialog::OnCancel();
        return( FALSE );
    }
    
    for( unsigned int i( 0 ); i < uiNumDispModes; ++i )
    {
        HRESULT        hr( 0 );
        D3DDISPLAYMODE sDisplayMode;        
        if( FAILED( hr = m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, i, &sDisplayMode ) ) )
        {
            AfxMessageBox( IDS_D3D_DISPENUMFAILED );

            CDialog::OnCancel();
            return( FALSE );
        }       

        for( unsigned int a( 0 ); a < c_uiNumDisplayModes; ++a )
        {
            if( s_sModeList[ a ].uiHeight == sDisplayMode.Height &&
                s_sModeList[ a ].uiWidth  == sDisplayMode.Width )
            {
                s_sModeList[ a ].bAvailable = true;
            }
        }
    }

    // fall back to the closest resolution supported by the device
    // in case the last saved resolution is no longer available
    if( false == s_sModeList[ m_iResolutionID ].bAvailable )
    {
        while( m_iResolutionID > 0 )
        {
            if( false != s_sModeList[ --m_iResolutionID ].bAvailable )
            {
                break;
            }
        }
    }

    UpdateDlgItems();

    // set version info
    CString strVersion;
    CMeshuggahApp::GetApp().GetVersionString( strVersion );
    GetDlgItem( IDC_VERSIONINFO )->SetWindowText( strVersion );

    return( TRUE );
}



void 
CMeshuggahSetupDlg::UpdateDlgItems()
{
    if( DEMO == m_eStartUpMode )
    {
        static_cast< CButton* >( GetDlgItem( IDC_DEMO ) )->SetCheck( 1 );

        GetDlgItem( IDC_DEMOSETTINGS_FRAME )->EnableWindow();
        GetDlgItem( IDC_FULLSCREEN )->EnableWindow();
        if( FALSE == m_bFullscreen )
        {
            m_bTripleBuffering = FALSE;
        }
        GetDlgItem( IDC_TRIPLE_BUFFERING )->EnableWindow( m_bFullscreen );
        GetDlgItem( IDC_MUSIC )->EnableWindow();
        GetDlgItem( IDC_LOOP )->EnableWindow();

        GetDlgItem( IDC_RESOLUTION_FRAME )->EnableWindow( m_bFullscreen );
        for( unsigned int a( 0 ); a < c_uiNumDisplayModes; ++a )
        {
            if( false != s_sModeList[ a ].bAvailable )
            {
                GetDlgItem( s_sModeList[ a ].uiCtrlID )->EnableWindow( m_bFullscreen );
            }
        }
    }
    else
    {
        static_cast< CButton* >( GetDlgItem( IDC_BROWSER ) )->SetCheck( 1 );

        GetDlgItem( IDC_DEMOSETTINGS_FRAME )->EnableWindow( FALSE);

        m_bFullscreen = FALSE;        
        GetDlgItem( IDC_FULLSCREEN )->EnableWindow( FALSE);
        
        m_bTripleBuffering = FALSE;
        GetDlgItem( IDC_TRIPLE_BUFFERING )->EnableWindow( FALSE );
        
        m_bMusic = FALSE;
        GetDlgItem( IDC_MUSIC )->EnableWindow( FALSE );
        
        m_bLoop = FALSE;
        GetDlgItem( IDC_LOOP )->EnableWindow( FALSE);

        GetDlgItem( IDC_RESOLUTION_FRAME )->EnableWindow( FALSE );
        for( unsigned int a( 0 ); a < c_uiNumDisplayModes; ++a )
        {
            if( false != s_sModeList[ a ].bAvailable )
            {
                GetDlgItem( s_sModeList[ a ].uiCtrlID )->EnableWindow( FALSE );
            }
        }
    }

    UpdateData( FALSE );
}



void
CMeshuggahSetupDlg::OnHtmlHelp()
{
    CMeshuggahApp::GetApp().HtmlHelp( ID_HELPCONTEXT_USAGE );
}



void
CMeshuggahSetupDlg::OnDestroy()
{
    // make sure that html help window get closed
    // before we shut down (otherwise our temp html 
    // help file is still under control of htmlhelp
    // and can't be deleted)    
    CMeshuggahApp::GetApp().CloseHTMLHelpWindow();

    CDialog::OnDestroy();    
}
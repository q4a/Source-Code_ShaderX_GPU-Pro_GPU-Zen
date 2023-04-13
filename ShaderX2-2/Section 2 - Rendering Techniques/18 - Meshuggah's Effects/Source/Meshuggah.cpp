///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Application::CMeshuggahApp.
 * 
 * @file
 *  Meshuggah.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e Application::CMeshuggahApp.
 */



#include "stdafx.h"

#include "Meshuggah.h"
#include "resource.h"

#include "MeshuggahSetupDlg.h"
#include "RenderWnd.h"
#include "BrowserMainFrm.h"
#include "DemoMainFrm.h"
#include "AVICaptureMainFrm.h"
#include "ScrambledArchive.h"
#include "EffectException.h"
#include <vector>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Application;
using namespace WindowSystem;
using namespace std;
using namespace ScrambledArchive;
using namespace EffectSystem;



const char c_pcDataArchive[]             = "IDR_DATA";
const char c_pcHTMLHelpFileArchivePath[] = "Help\\Meshuggah.chm";
const char c_pcTempHTMLHelpFileName[]    = "Meshuggah.chm";



BEGIN_MESSAGE_MAP( CMeshuggahApp, CWinApp )
END_MESSAGE_MAP()



CMeshuggahApp CMeshuggahApp::s_kTheApp;



CMeshuggahApp::CMeshuggahApp()
: CWinApp()
, m_sD3DPresParams()
, m_pRenderWnd( 0 )
, m_strTempHTMLHelpFile()
{
    memset( &m_sD3DPresParams, 0, sizeof( D3DPRESENT_PARAMETERS ) );    
}



CMeshuggahApp::~CMeshuggahApp()
{
    // delete our temporary html help file
    // the html help windows was previously closed
    // by CBrowserMainFrame::OnDestroy()
    if( false == m_strTempHTMLHelpFile.empty() )
    {
        ::DeleteFile( m_strTempHTMLHelpFile.c_str() );        
    }
}



BOOL 
CMeshuggahApp::InitInstance()
{
    CWinApp::InitInstance();

    if( FALSE == AfxOleInit() )
    {
        AfxMessageBox( IDP_OLE_INIT_FAILED );
        return( FALSE );
    }

    AfxInitRichEdit();
    
    AfxEnableControlContainer();

    SetRegistryKey( ID_REGISTRYKEY );

    CEffectException::PrepareLogFile();

    // parse command line
    CString strCommandLine( GetCommandLine() );
    
    if( 0 <= strCommandLine.Find( _T( "/capture" ) ) )
    {
        CAVICaptureMainFrame* pFrame( new CAVICaptureMainFrame );       
        HBRUSH                hBrush( reinterpret_cast< HBRUSH >( GetStockObject( BLACK_BRUSH ) ) );
        LPCTSTR               pStrWndClassName( AfxRegisterWndClass( 0, 0, hBrush, LoadIcon( IDR_MAINFRAME ) ) );
        DWORD                 dwStyle( ( WS_SYSMENU | WS_OVERLAPPED ) & ~( WS_MINIMIZEBOX | WS_THICKFRAME ) );

        CString strMainFrame;
        strMainFrame.LoadString( IDR_CAPTUREMAINFRAME );

        if( FALSE != pFrame->Create( pStrWndClassName, strMainFrame, dwStyle ) )
        {
            m_pMainWnd = pFrame;
            m_pRenderWnd = static_cast< IRenderWnd* >( pFrame );

            m_pMainWnd->ShowWindow( SW_SHOW );
            m_pMainWnd->UpdateWindow();

            return( TRUE );
        }
    }
    else
    {
        CMeshuggahSetupDlg kDlg( m_sD3DPresParams );
        if( IDOK == kDlg.DoModal() )
        {
            if( CMeshuggahSetupDlg::DEMO == kDlg.GetStartUpMode() )
            {
                CDemoMainFrame* pFrame( new CDemoMainFrame( kDlg.MusicEnabled(), kDlg.LoopDemo() ) );
                HBRUSH          hBrush( reinterpret_cast< HBRUSH >( GetStockObject( BLACK_BRUSH ) ) );
                LPCTSTR         pStrWndClassName( AfxRegisterWndClass( 0, 0, hBrush, LoadIcon( IDR_MAINFRAME ) ) );
                DWORD           dwStyle( ( WS_SYSMENU | WS_OVERLAPPED ) & ~( WS_MINIMIZEBOX | WS_THICKFRAME ) );
        
                CString strMainFrame;
                strMainFrame.LoadString( IDR_DEMOMAINFRAME );

                if( FALSE != pFrame->Create( pStrWndClassName, strMainFrame, dwStyle ) )
                {
                    m_pMainWnd = pFrame;
                    m_pRenderWnd = static_cast< IRenderWnd* >( pFrame );

                    m_pMainWnd->ShowWindow( SW_SHOW );
                    m_pMainWnd->UpdateWindow();

                    pFrame->PlayDemo();

                    return( TRUE );
                }
            }
            else
            {
                CBrowserMainFrame* pFrame( new CBrowserMainFrame );
                if( FALSE != pFrame->LoadFrame( IDR_MAINFRAME ) )
                {
                    m_pMainWnd = pFrame;
                    m_pRenderWnd = static_cast< IRenderWnd* >( pFrame );

                    m_pMainWnd->ShowWindow( SW_SHOW );
                    m_pMainWnd->UpdateWindow();

                    return( TRUE );
                }
            }  
        }
    }

    return( FALSE );
}



BOOL 
CMeshuggahApp::OnIdle( LONG lCount )
{
    if( FALSE != CWinApp::OnIdle( lCount ) )
    {
        return( TRUE );
    }

    if( 0 != m_pMainWnd )
    {
        // m_pRenderWnd should always be a valid ptr if m_pMainWnd isn't NULL
        return( false != m_pRenderWnd->RenderFrame() ? TRUE : FALSE );
    }

    return( FALSE );
}



bool
CMeshuggahApp::GetVersionString( CString& strVersion )
{
    TCHAR strExePath[ MAX_PATH ];
    ::GetModuleFileName( AfxGetApp()->m_hInstance, strExePath, MAX_PATH );

    DWORD dwHandle;
    unsigned int uiFileVersionInfoSize( ::GetFileVersionInfoSize( strExePath, &dwHandle ) );

    vector< unsigned char > colTempBuffer( uiFileVersionInfoSize );
    if( false == ::GetFileVersionInfo( 
                            strExePath, 
                            dwHandle, 
                            uiFileVersionInfoSize, 
                            &colTempBuffer[ 0 ] 
                            ) )
    {
        return( false );
    }

    VS_FIXEDFILEINFO* psFileInfo;
    unsigned int      uiDataLength;
    if( false == ::VerQueryValue( &colTempBuffer[ 0 ], TEXT( "\\" ), 
                                  (void **) &psFileInfo, &uiDataLength ) )
    {
        return( false );
    }

    CString strAppTitle;
    strAppTitle.LoadString( AFX_IDS_APP_TITLE );

    strVersion.Format( _T( "%s v%d.%d" ), strAppTitle,
        HIWORD( psFileInfo->dwProductVersionMS ),
        LOWORD( psFileInfo->dwProductVersionMS ) );

    if( 0 != psFileInfo->dwProductVersionLS )
    {
        CString strTemp;
        if( 0 != LOWORD( psFileInfo->dwProductVersionLS ) )
        {
            strTemp.Format( _T( " (Revision %d.%d)" ), 
                HIWORD( psFileInfo->dwProductVersionLS ),
                LOWORD( psFileInfo->dwProductVersionLS ) );
        }
        else
        {
            strTemp.Format( _T( " (Revision %d)" ), 
                HIWORD( psFileInfo->dwProductVersionLS ) );
        }

        strVersion += strTemp;
    }

    if( 0 != ( psFileInfo->dwFileFlags & VS_FF_PRERELEASE ) )
    {
        strVersion += _T( "\nPrefinal" );
    }
    else
    {    
        strVersion += _T( "\nFinal" );
    }
    
    if( 0 != ( psFileInfo->dwFileFlags & VS_FF_DEBUG ) )
    {
        strVersion += _T( " debug build." );
    }
    else
    {
        strVersion += _T( " release build." );
    }

    return( true );
}



void
CMeshuggahApp::LoadHTMLHelpFile()
{
    if( false != m_strTempHTMLHelpFile.empty() )
    {
        CScrambledArchive* pkArchive( CScrambledArchive::CreateFromResource( c_pcDataArchive ) );
        if( 0 != pkArchive )
        {
            char pcTempPath[ MAX_PATH ];
            GetTempPath( MAX_PATH, pcTempPath );

            m_strTempHTMLHelpFile = pcTempPath; 
            m_strTempHTMLHelpFile += c_pcTempHTMLHelpFileName;

            // get html help file from archive
            bool bHTMLHelpFile( pkArchive->DecompressFileToDisk( c_pcHTMLHelpFileArchivePath, m_strTempHTMLHelpFile ) );
            DBG_ASSERTM( false != bHTMLHelpFile, "CMeshuggahApp::LoadHTMLHelpFile() - Couldn't load html help file from archive!" );

            // destroy archive object since we don't need it any longer
            delete pkArchive;
        }
    }
}



void 
CMeshuggahApp::HtmlHelp( DWORD_PTR dwData, UINT nCmd )
{
    LoadHTMLHelpFile();
    m_hWndHTMLHelp = ::HtmlHelp( 0, m_strTempHTMLHelpFile.c_str(), HH_HELP_CONTEXT, dwData );
}



void
CMeshuggahApp::CloseHTMLHelpWindow()
{
    if( FALSE != IsWindow( m_hWndHTMLHelp ) )
    {
        ::SendMessage( m_hWndHTMLHelp, WM_CLOSE, 0, 0 );        
    }
}

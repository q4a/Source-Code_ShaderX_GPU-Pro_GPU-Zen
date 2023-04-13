///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of the macro helper functions.
 *
 * @file
 *  Debug.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  10.11.2001
 * @brief
 * Implementation of the macro helper functions.
 */



#include "StdAfx.h"
#include "windows.h"
#include "stdio.h"
#include "Debug.h"



#ifdef _DEBUG



// prototype of our default handler to initialze the function pointer
bool DefaultAssertHandler( const char *pcExp, const char *pcMsg, const char *pcFile, int iLine, bool *bIgnoreAlways );



static ASSERTHANDLER    *s_pAssertHandler   = &DefaultAssertHandler;
static bool             s_bMinimizeApp      = false;



enum EAssertDialogControls
{
    IDC_FILENAME                = 1000,
    IDC_LINENUMBER,
    IDC_EXPRESSION,
    IDC_MESSAGE,
    IDC_FRAME,
    IDC_DEBUG,
    IDC_IGNORE,
    IDC_IGNOREALWAYS,
    IDC_EXIT,

    IDC_STATIC                  =  0xFFFF
};



static const char   *s_pcExp    = NULL;
static const char   *s_pcMsg    = NULL;
static const char   *s_pcFile   = NULL;

static char         s_strLineNum[ 12 + 1 ];



void 
SetAssertHandler( ASSERTHANDLER *pAssertHandler ) 
{
    if( 0 != pAssertHandler )
    {
        s_pAssertHandler = pAssertHandler;
    }
    else
    {
        s_pAssertHandler = &DefaultAssertHandler;
    }
}



void 
AssertMinimizeApp( bool bMinimize )
{
    s_bMinimizeApp = bMinimize;
}



///////////////////////////////////////////////////////////////////////////////////////////
// real pain starts here!!! 
// a hardcoded "debug assertion failed" dialog for the default assert handler
// to avoid any possible interference with the app's resource templates
///////////////////////////////////////////////////////////////////////////////////////////



void 
CopyToWideChar( unsigned short** ppStrOut, const char* pStrIn )
{
    unsigned int uiLen( lstrlen( pStrIn ) );
    unsigned short* pStrOut( *ppStrOut );

    if( 0 != uiLen )
    {
        uiLen = MultiByteToWideChar( CP_ACP, 0, pStrIn, uiLen, pStrOut, uiLen );
    }

    pStrOut[ uiLen++ ] = L'\0'; // Add the null terminator
    *ppStrOut += uiLen;
}



void 
AddDialogControl( unsigned short** pp, unsigned int uiStyle, unsigned int uiExtStyle,
                  short sSX, short sSY, short sCX, short sCY, 
                  unsigned short sID, const char* pStrClassName, const char* pStrTitle )
{
    // DWORD align the current ptr    
    // use __int64 to avoid warnings in win64 compiler test run -- CW
    DLGITEMTEMPLATE* p( ( DLGITEMTEMPLATE *) ( ( ( (__int64) *pp ) + 3 ) & ~3 ) ); 

    p->style           = uiStyle | WS_CHILD | WS_VISIBLE;
    p->dwExtendedStyle = uiExtStyle;
    p->x               = sSX;
    p->y               = sSY;
    p->cx              = sCX;
    p->cy              = sCY;
    p->id              = sID;

    // advance ptr
    *pp = (unsigned short *) ( ++p ); 

    // set class name
    CopyToWideChar( (unsigned short **) pp, pStrClassName );    
    // set title
    CopyToWideChar( (unsigned short **) pp, pStrTitle );     

    // skip extra stuff
    ++(*pp);
}



DLGTEMPLATE* 
BuildDialogTemplate()
{
    const int iNumDlgItems( 14 ); 

    // allocate memory for building the template
    DLGTEMPLATE* pDlgTemplate( new DLGTEMPLATE[ 64 ] );
    if( 0 == pDlgTemplate )
    {
        return( 0 );
    }

    ZeroMemory( pDlgTemplate, 64 * sizeof( DLGTEMPLATE ) );
    
    // fill in the DLGTEMPLATE info
    DLGTEMPLATE* pDT( pDlgTemplate );
    pDT->style           = DS_MODALFRAME | DS_NOIDLEMSG | DS_SETFOREGROUND |
                           DS_3DLOOK | DS_CENTER | WS_POPUP | WS_VISIBLE |
                           WS_CAPTION | DS_SETFONT;
    pDT->dwExtendedStyle = 0L;
    pDT->cdit            = iNumDlgItems;
    pDT->x               = 0;
    pDT->y               = 0;
    pDT->cx              = 346;
    pDT->cy              = 127;

    // add menu array, class array, dlg title, font size and font name
    unsigned short* pW( (unsigned short *) ( ++pDT ) );
    
    // set menu array to nothing
    *pW++ = L'\0';
    
    // set class array to nothing
    *pW++ = L'\0';
    CopyToWideChar( (unsigned short **) &pW, TEXT( "Debug Assertion Failed!" ) );   
    
    // font size
    *pW++ = 8;

    // font name
    CopyToWideChar( (unsigned short **) &pW, TEXT( "Arial" ) );

    AddDialogControl( &pW, SS_RIGHT | SS_CENTERIMAGE, 0, 11, 15, 38, 10, IDC_STATIC, TEXT( "STATIC" ), TEXT( "File" ) );
    AddDialogControl( &pW, SS_RIGHT | SS_CENTERIMAGE, 0, 11, 31, 38, 10, IDC_STATIC, TEXT( "STATIC" ), TEXT( "Line" ) );
    AddDialogControl( &pW, SS_RIGHT | SS_CENTERIMAGE, 0, 11, 46, 38, 11, IDC_STATIC, TEXT( "STATIC" ), TEXT( "Expression" ) );
    AddDialogControl( &pW, SS_RIGHT | SS_CENTERIMAGE, 0, 11, 62, 38, 10, IDC_STATIC, TEXT( "STATIC" ), TEXT( "Message" ) );
    
    AddDialogControl( &pW, WS_GROUP | BS_GROUPBOX, 0, 5, 2, 336, 120, IDC_FRAME, TEXT( "BUTTON" ), TEXT( "" ) );

    AddDialogControl( &pW, WS_BORDER | ES_AUTOHSCROLL | ES_READONLY, 0, 55, 14, 280, 12, IDC_FILENAME, TEXT( "EDIT" ), TEXT( "" ) );
    AddDialogControl( &pW, WS_BORDER | ES_AUTOHSCROLL | ES_READONLY, 0, 55, 30, 280, 12, IDC_LINENUMBER, TEXT( "EDIT" ), TEXT( "" ) );
    AddDialogControl( &pW, WS_BORDER | ES_AUTOHSCROLL | ES_READONLY, 0, 55, 46, 280, 12, IDC_EXPRESSION, TEXT( "EDIT" ), TEXT( "" ) );
    AddDialogControl( &pW, WS_BORDER | ES_AUTOHSCROLL | ES_READONLY, 0, 55, 61, 280, 12, IDC_MESSAGE, TEXT( "EDIT" ), TEXT( "" ) );
    
    AddDialogControl( &pW, WS_GROUP | BS_PUSHBUTTON | WS_TABSTOP, 0, 11, 103, 62, 13, IDC_DEBUG, TEXT( "BUTTON" ), TEXT( "Debug" ) );
    AddDialogControl( &pW, BS_PUSHBUTTON | WS_TABSTOP, 0, 98, 103, 62, 13, IDC_IGNORE, TEXT( "BUTTON" ), TEXT( "Ignore" ) );
    AddDialogControl( &pW, BS_PUSHBUTTON | WS_TABSTOP, 0, 185, 103, 62, 13, IDC_IGNOREALWAYS, TEXT( "BUTTON" ), TEXT( "Ignore always" ) );
    AddDialogControl( &pW, BS_PUSHBUTTON | WS_TABSTOP, 0, 272, 103, 62, 13, IDC_EXIT, TEXT( "BUTTON" ), TEXT( "Exit" ) );

    AddDialogControl( &pW, SS_LEFT | WS_GROUP | WS_TABSTOP, WS_EX_OVERLAPPEDWINDOW | WS_EX_LEFT, 11, 78, 324, 19, IDC_STATIC, TEXT( "STATIC" ), TEXT( " Note that all information is copied into the clipboard (use Crtl+V to paste it in a text window).\n It's also sent to a debug log window if a debugger is present." ) );    

    return( pDlgTemplate );
}



BOOL 
CALLBACK Callback( HWND hWnd, unsigned int uiMsg, WPARAM wparam, LPARAM lparam )
{
   switch( uiMsg )
    {
        case WM_INITDIALOG:
        {           
            ::SetWindowText( ::GetDlgItem( hWnd, IDC_FILENAME ), s_pcFile );
            ::SetWindowText( ::GetDlgItem( hWnd, IDC_LINENUMBER ), s_strLineNum );
            ::SetWindowText( ::GetDlgItem( hWnd, IDC_EXPRESSION ), s_pcExp );
            ::SetWindowText( ::GetDlgItem( hWnd, IDC_MESSAGE ), s_pcMsg );

            char strModulePath[ MAX_PATH ];
            GetModuleFileName( NULL, strModulePath, MAX_PATH );
            
            const char* pcModuleName( strrchr( strModulePath, '\\' ) );
            pcModuleName = pcModuleName ? pcModuleName + 1 : strModulePath;
            
            char strTitle[ MAX_PATH + 16 ];
            sprintf( strTitle, "In Module \"%s\"", pcModuleName );
            ::SetWindowText( ::GetDlgItem( hWnd, IDC_FRAME ), strTitle );

            // paste it to clipboard
            if( FALSE != OpenClipboard( 0 ) )
            {
                char strBuf[ 2048 ];
                sprintf( strBuf, "Debug assertion failed in...\r\nApplication: %s\r\nFilename: %s\r\nLine: %s\r\nExpression: %s\r\nMessage: %s\r\n", 
                    pcModuleName, s_pcFile, s_strLineNum, s_pcExp, s_pcMsg ? s_pcMsg : "");
                
                HGLOBAL hMem( GlobalAlloc( GHND | GMEM_DDESHARE, strlen( strBuf ) + 1 ) );
                if( 0 != hMem )
                {
                    char* pMem( (char *) GlobalLock( hMem ) );
                    strcpy( pMem, strBuf );
                    GlobalUnlock( hMem );
                    EmptyClipboard();
                    SetClipboardData( CF_TEXT, hMem );
                }
            
                CloseClipboard();
            }

            return( TRUE );
        }

        case WM_COMMAND:
        {
            switch ( LOWORD( wparam ) )
            {
            case IDC_DEBUG:
            case IDC_IGNORE:
            case IDC_IGNOREALWAYS:
            case IDC_EXIT:
                {
                    EndDialog( hWnd, LOWORD( wparam ) );
                    return( TRUE );
                }
            }
        }
   }
    return( FALSE );
}



bool 
DefaultAssertHandler( const char *pcExp, const char *pcMsg, const char *pcFile, int iLine, bool *bIgnoreAlways )
{
    DWORD dwLastErr( GetLastError() );
    
    // EnterCriticalSection(...)

    s_pcFile = pcFile;
    s_pcExp  = pcExp;
    s_pcMsg  = pcMsg;   
    itoa( iLine , s_strLineNum , 10 );
  
    // log it 
    char strLogMsg[ 2048 ];
    sprintf( strLogMsg, "%s(%i) : Assertion (%s) failed. %s\n", pcFile, iLine, pcExp, pcMsg ? pcMsg : "" );
    OutputDebugString( strLogMsg );

    HWND hWnd( GetActiveWindow() );
    DLGTEMPLATE* pDlgSelect( BuildDialogTemplate() );

    if( 0 != pDlgSelect )
    {   
        if( ( 0 != hWnd ) && ( false != s_bMinimizeApp ) )
        {
            SendMessage( hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0 );
        }

        INT_PTR ipRet( DialogBoxIndirectParam( NULL, pDlgSelect, hWnd, (DLGPROC) Callback, 0L ) );

        if( ( 0 != hWnd ) && ( false != s_bMinimizeApp ) )
        {
            SendMessage( hWnd, WM_SYSCOMMAND, SC_RESTORE, 0 );
        }

        delete pDlgSelect;

        switch( ipRet )
        {
        case IDC_IGNOREALWAYS:
            {
                *bIgnoreAlways = true;
            }            
        case IDC_IGNORE:
            {
                return( false );
            }            
        case IDC_DEBUG:
            {
                return( true );
            }
        case IDC_EXIT:
            {
                exit( 0 );
                break;
            }
        }
    }
    else
    {
        MessageBeep( -1 );
        OutputDebugString( "Couldn't create assert dialog !!!" );
    }

    // LeaveCriticalSection(...)
    
    SetLastError( dwLastErr );
    return true;
}



bool 
HandleAssert( const char *pcExp, const char *pcMsg, const char *pcFile, int iLine, bool *bIgnoreAlways )
{   
    // s_pAssertHandler is initially set to our default handler
    return( (*s_pAssertHandler) ( pcExp, pcMsg, pcFile, iLine, bIgnoreAlways ) );      
}



void 
Trace( const char *pcFormat, ... )
{
    const unsigned int TEMP_BUFFER_SIZE( 2048 );

    if( 0 != pcFormat )
    {
        va_list argList;
        va_start( argList, pcFormat );

        char pcTrace[ TEMP_BUFFER_SIZE ];
        pcTrace[ 0 ] = 0;
        _vsnprintf( pcTrace, TEMP_BUFFER_SIZE - 1, pcFormat, argList );

        va_end( argList );
        OutputDebugString( pcTrace );
    }
}



void 
TraceD3D( HRESULT hr, char* pcError, const char *pcFile, int iLine )
{
    const unsigned int TEMP_BUFFER_SIZE( 2048 );

    char strLogMsg[ TEMP_BUFFER_SIZE ];
    sprintf( strLogMsg, "%s(%i) : D3D hResult = 0x%x - %s\n", pcFile, iLine, hr, pcError );
    OutputDebugString( strLogMsg );
}



#endif // #ifdef _DEBUG
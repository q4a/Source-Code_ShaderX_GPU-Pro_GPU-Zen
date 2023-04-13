///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Application::CMeshuggahApp. 
 * 
 * @file
 *  Meshuggah.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Declaration of \e Application::CMeshuggahApp.
 */



#ifndef _MESHUGGAH_H_INCLUDED_
#define _MESHUGGAH_H_INCLUDED_

#pragma once



#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif



#include <string>



namespace WindowSystem
{
    class IRenderWnd;
}



namespace Application
{



    /**
    * %Application class for both demo and browser.
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  %Application class for both demo and browser.
    */
    class CMeshuggahApp : public CWinApp
    {
    public:    
        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
	    ~CMeshuggahApp();

        /**
        * Returns the application instance.
        * 
        * @return
        *  The application instance (that is, #s_kTheApp).
        * @brief
        *  Returns the application instance.
        */
        static CMeshuggahApp& GetApp();

        /**
        * Gets a copy of the D3D present parameters (#m_sD3DPresParams).
        * 
        * @param sPresentParameters
        *  Structure that should receive a copy of the D3D present parameters.
        * @brief
        *  Gets a copy of the D3D present parameters (#m_sD3DPresParams).
        */
        void GetD3DPresentParameters( D3DPRESENT_PARAMETERS& sPresentParameters );

        /**
        * Creates a string containing the application version based on the version info
        * in the executable's resource.
        * 
        * @param strVersion
        *  Reference of a string the should receive the version info.
        * @return
        *  \b true if successful else \b false.
        * @brief
        *  Creates a string containing the application version based on the version info
        *  in the executable's resource.
        */
        bool GetVersionString( CString& strVersion );

        /** 
        * Initializes the application. Runs the setup dialog and then 
        * starts either browser or demo.
        * 
        * @brief
        *  Initializes the application.
        */
	    virtual BOOL InitInstance();

        /** 
        * Handles idle processing time. Frames updates are initiated here.
        * 
        * @brief
        *  Handles idle processing time.
        */
        virtual BOOL OnIdle( LONG lCount );

        /** 
        * Calls HTML Help API to display help for a given help context.
        * Uses the temporary help file stored in #m_strTempHTMLHelpFile by
        * CMeshuggahApp::LoadHTMLHelpFile().
        * 
        * @param dwData
        *  ID of the help context.
        * @param nCmd
        *  Display command value.
        * @brief
        *  Calls HTML Help API to display help for a given help context.
        */
        virtual void HtmlHelp( DWORD_PTR dwData, UINT nCmd = 0x000F );
        
        /**
        * Closes the HTML help window.
        * 
        * @brief
        *  Closes the HTML help window.
        */
        void CloseHTMLHelpWindow();

	    DECLARE_MESSAGE_MAP()

    private:
        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
	    CMeshuggahApp();

        /** 
        * Loads the compressed HTML file into a temporary file.
        * Helper function for CMeshuggahApp::HtmlHelp().
        *
        * @brief
        *  Loads the compressed HTML file into a temporary file.
        */
        void LoadHTMLHelpFile();

        /** 
        * The instance of our application class.
        * 
        * @brief
        *  The instance of our application class.
        */
        static CMeshuggahApp s_kTheApp;

        /** 
        * D3D present parameters. They are set Application::CMeshuggahSetupDlg and stored here.
        * 
        * @brief
        *  D3D present parameters.
        */
        D3DPRESENT_PARAMETERS m_sD3DPresParams;

        /** 
        * Pointer to the render window. Either demo or browser.
        * CMeshuggahApp::OnIdle() uses this pointer to update the render window.
        *
        * @brief
        *  Pointer to the render window.
        */
        WindowSystem::IRenderWnd* m_pRenderWnd;

        /** 
        * Name of the temporary file containg the compressed HTML help.
        * 
        * @brief
        *  Name of the temporary file containg the compressed HTML help.
        */
        std::string m_strTempHTMLHelpFile;

        /** 
        * Handle to the HTML help window.
        * 
        * @brief
        *  Handle to the HTML help window.
        */
        HWND m_hWndHTMLHelp;
    };



#include "Meshuggah.inl"



} // namespace Application



#endif // #ifndef _MESHUGGAH_H_INCLUDED_

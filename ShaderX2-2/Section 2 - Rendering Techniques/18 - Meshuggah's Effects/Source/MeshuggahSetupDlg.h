///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Application::CMeshuggahSetupDlg.
 * 
 * @file
 *  MeshuggahSetupDlg.h
 * @author
 *  Carsten Wenzel
 * @date
 *  12.10.2001
 * @brief
 *  Declaration of \e Application::CMeshuggahSetupDlg.
 */



#ifndef _MESHUGGAHSETUPDLG_H_INCLUDED_
#define _MESHUGGAHSETUPDLG_H_INCLUDED_

#pragma once



#include "resource.h"
#include "HyperLink.h"



namespace Application
{



    /**
    * Setup dialog class.
    * Enables the user to select between browser and demo mode and to specify 
    * certain demo relevant settings. According to those decisions made by the user a D3D present parameter
    * struction is filled, ready to use for D3D device creation.
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  12.10.2001
    * @brief
    *  Setup dialog class.
    */
    class CMeshuggahSetupDlg : public CDialog
    {
    public:
        /** 
        * Enumeration of all start up modes.
        * 
        * @brief
        *  Enumeration of all start up modes.
        */
        enum EStartUpMode
        {
            /** 
            * Demo.
            */
            DEMO,

            /** 
            * Browser.
            */
            BROWSER,

            /** 
            * Default.
            */
            DEFAULT = DEMO
        };

    private:
        enum EDialogID
        { 
            /** 
            * Resource ID of dialog template.
            */
            IDD = IDD_SETUP 
        };

        /** 
        * Hyperlink control for Onion's email address.
        * 
        * @brief
        *  Hyperlink control for Onion's email address.
        */
        CHyperLink m_kLinkOnion;

        /** 
        * Hyperlink control for 4fo website.
        * 
        * @brief
        *  Hyperlink control for 4fo website.
        */
        CHyperLink m_kLink4fo;

        /** 
        * Hyperlink control for Meshuggah website.
        * 
        * @brief
        *  Hyperlink control for Meshuggah website.
        */
        CHyperLink m_kLinkUpdate;

        /** 
        * ID of currently selected screen resolution for demo.
        * 
        * @brief
        *  ID of currently selected screen resolution for demo.
        */
        int m_iResolutionID;

        /** 
        * Fullscreen flag for demo.
        * 
        * @brief
        *  Fullscreen flag for demo.
        */
        BOOL m_bFullscreen;

        /** 
        * "Loop demo" flag for demo.
        * 
        * @brief
        *  "Loop demo" flag for demo.
        */
        BOOL m_bLoop;

        /** 
        * "Music enabled" flag for demo.
        * 
        * @brief
        *  "Music enabled" flag for demo.
        */
        BOOL m_bMusic;

        /** 
        * "Triple buffering" flag for demo.
        * 
        * @brief
        *  "Triple buffering" flag for demo.
        */
        BOOL m_bTripleBuffering;

    public:
        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        * @param sD3DPresParams
        *  Reference to a D3D present parameters structure.
        * @param pParent
        *  Pointer to parent window.
        */
        CMeshuggahSetupDlg( D3DPRESENT_PARAMETERS& sD3DPresParams, CWnd* pParent = 0 );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CMeshuggahSetupDlg();

        /**
        * Returns the start up mode that was chosen by the user.
        * 
        * @brief
        *  Returns the start up mode that was chosen by the user.
        * @return
        *  Start up mode.
        */
        EStartUpMode GetStartUpMode();

        /**
        * Is demo looping enabled?
        * 
        * @brief
        *  Is demo looping enabled?
        * @return
        *  \b true if enabled, \b false if disabled.
        */
        bool LoopDemo();

        /**
        * Is music enabled?
        * 
        * @brief
        *  Is music enabled?
        * @return
        *  \b true if enabled, \b false if disabled.
        */
        bool MusicEnabled();

    protected:
        /**
        * Validates all user input in setup dialog.
        * 
        * @brief
        *  Validates all user input in setup dialog.
        */
        virtual void DoDataExchange( CDataExchange* pDX );    

        /** 
        * Message handler for "Start" button.
        * 
        * @brief
        *  Message handler for "Start" button.
        */
        virtual void OnOK();

        /** 
        * Initializes the setup dialog.
        * 
        * @brief
        *  Initializes the setup dialog.
        */
        virtual BOOL OnInitDialog();

        /** 
        * Message handler for "Demo" radio button.
        * 
        * @brief
        *  Message handler for "Demo" radio button.
        */
        afx_msg void OnDemo();

        /** 
        * Message handler for "Browser" radio button.
        * 
        * @brief
        *  Message handler for "Browser" radio button.
        */
        afx_msg void OnBrowser();

        /** 
        * Message handler for "Fullscreen" check box.
        * 
        * @brief
        *  Message handler for "Fullscreen" check box.
        */
        afx_msg void OnFullscreen();

        /** 
        * Message handler for "Help" button.
        * 
        * @brief
        *  Message handler for "Help" button.
        */
        afx_msg void OnHtmlHelp();

        /** 
        * Message handler for \c WM_DESTROY. Closes HTML help window if open before
        * dialog shuts down.
        * 
        * @brief
        *  Message handler for WM_DESTROY.
        */
        afx_msg void OnDestroy();

        DECLARE_MESSAGE_MAP()

    private:
        /**
        * Fills the present parameter structure according to the users settings.
        * 
        * @brief
        *  Fills the present parameter structure according to the users settings.
        * @return
        *  \b true if succeeded, \b false if it wasn't possible to find a suitable color/depth buffer
        *  combination.
        */
        bool SetPresentParameters();

        /**
        * Updates all dialog items when user changed the settings.
        * 
        * @brief
        *  Updates all dialog items when user changed the settings.
        */
        void UpdateDlgItems();

        /** 
        * Holds the selected start mode.
        * 
        * @brief
        *  Holds the selected start mode.
        */
        EStartUpMode m_eStartUpMode;

        /** 
        * Reference to a D3D present parameters structure.
        * 
        * @brief
        *  Reference to a D3D present parameters structure.
        */
        D3DPRESENT_PARAMETERS& m_sD3DPresParams;

        /** 
        * Pointer to an \e IDirect3D8 interface.
        * 
        * @brief
        *  Pointer to an \e IDirect3D8 interface.
        */
        IDirect3D8* m_pD3D;
    };



} // namespace Application



#endif // #ifndef _MESHUGGAHSETUPDLG_H_INCLUDED_

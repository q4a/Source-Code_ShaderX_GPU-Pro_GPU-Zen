///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CInfiniteZoomDialog.
 *
 * @file
 *  InfiniteZoomDialog.h
 * @author
 *  Carsten Wenzel
 * @date
 *  25.02.2002
 * @brief
 *  Declaration of \e Effects::CInfiniteZoomDialog.
 */



#ifndef _INFINITEZOOMDIALOG_H_INCLUDED_
#define _INFINITEZOOMDIALOG_H_INCLUDED_

#pragma once



#include "Timer.h"



namespace Effects
{



    /**
    * Dialog class for <em>Infinte zoom</em> effect settings.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  25.02.2002
    * @brief
    *  Dialog class for <em>Infinte zoom</em> effect settings.
    */
    class CInfiniteZoomDialog : public CDialog
    {
    public:
        /**
        * Constructor.
        * 
        * @param pParent
        *  Pointer to parent window.
        * @brief
        *  Constructor.
        */
        CInfiniteZoomDialog( CWnd* pParent = 0 );   

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CInfiniteZoomDialog();
        
        /**
        * Returns length of zoom in seconds.
        * 
        * @return
        *  Length of zoom  in seconds.
        * @brief
        *  Returns length of zoom in seconds.
        */
        float GetZoomLength() const;

        /**
        * Returns current zoom postion in seconds. Depends on the user settings in the dialog.
        * 
        * @return
        *  Current zoom postion in seconds.
        * @brief
        *  Returns current zoom postion in seconds.
        */
        float GetZoomPosition() const;        

        /**
        * Returns the current z rotation angle of the zoom in radians. <em>Enable rotation around z axis</em>
        * has to be enabled in the dialog.
        * 
        * @return
        *  Current z rotation angle.
        * @brief
        *  Returns the current z rotation angle of the zoom in radians.
        */
        float GetZRotationAngle() const;

    protected:
        /**
        * Validates all user input in effect dialog.
        * 
        * @brief
        *  Validates all user input in effect dialog.
        */
        virtual void DoDataExchange( CDataExchange* pDX );

        /**
        * Initializes effect dialog.
        * 
        * @brief
        *  Initializes effect dialog.
        */
        virtual BOOL OnInitDialog();

        /**
        * Windows message handler. Pauses #m_kTimer and 
        * #m_kRotationTimer while dialog is being dragged around.
        * 
        * @brief
        *  Windows message handler.
        */
        virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

        /**
        * Handler for OK button. Empty function to prevent the dialog from being closed
        * when user accidentally presses \e Return.
        * 
        * @brief
        *  Handler for default button.
        */
        virtual void OnOK();

        /**
        * Message handler for \e Animate check box.
        * 
        * @brief
        *  Message handler for \e Animate check box.
        */
        afx_msg void OnAnimate();

        /**
        * Message handler for <em>Zoom position</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Zoom position</em> slider bar.
        */
        afx_msg void OnReleasedCaptureZoomPos( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Enable z rotation</em> check box.
        * 
        * @brief
        *  Message handler for <em>Enable z rotation</em> check box.
        */
        afx_msg void OnRotateZ();

        DECLARE_MESSAGE_MAP()        

    private:
        /** 
        * Current <em>Zoom position</em> value mapped from slider bar.
        * 
        * @brief
        *  Curretn <em>Zoom position</em> value mapped from slider bar.
        */
        int m_iZoomPos;

        /** 
        * <em>Enable z rotation</em> mapped from check box.
        * 
        * @brief
        *  <em>Enable z rotation</em> mapped from check box.
        */
        BOOL m_bRotateZ;

        /** 
        * \e Animate mapped from check button.
        * 
        * @brief
        *  \e Animate mapped from check button.
        */
        BOOL m_bAnimate;

        /** 
        * Length of zoom in seconds in browser mode.
        * 
        * @brief
        *  Length of zoom in seconds in browser mode.
        */
        const float mc_fZoomLength;

        /** 
        * Timer for zoom animation in browser mode.
        * 
        * @brief
        *  Timer for zoom animation in browser mode.
        */
        Shared::CTimer m_kTimer;

        /** 
        * Timer for zoom rotation in browser mode.
        * 
        * @brief
        *  Timer for zoom rotation in browser mode.
        */
        Shared::CTimer m_kRotationTimer;
    };



} // namespace Effects



#endif // #ifndef _INFINITEZOOMDIALOG_H_INCLUDED_
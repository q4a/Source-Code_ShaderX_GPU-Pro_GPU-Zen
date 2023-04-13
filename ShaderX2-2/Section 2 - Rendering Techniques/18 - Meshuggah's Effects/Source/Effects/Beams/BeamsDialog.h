///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CBeamsDialog.
 *
 * @file
 *  BeamsDialog.h
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Declaration of \e Effects::CBeamsDialog.
 */



#ifndef _BEAMSDIALOG_H_INCLUDED_
#define _BEAMSDIALOG_H_INCLUDED_

#pragma once



#include "Timer.h"



namespace Effects
{



    /**
    * Dialog class for \e Beams effect settings.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  04.03.2002
    * @brief
    *  Dialog class for \e Beams effect settings.
    */
    class CBeamsDialog : public CDialog
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
        CBeamsDialog( CWnd* pParent = 0 );   

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CBeamsDialog();        

        /**
        * Returns the current time of the effect.
        * 
        * @return
        *  The current time of the effect.
        * @brief
        *  Returns the current time of the effect.
        */
        float GetTime();

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
        * Windows message handler. Pauses #m_kTimer while dialog 
        * is being dragged around.
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

        DECLARE_MESSAGE_MAP()        

    private:
        /** 
        * Timer for effect in browser mode.
        * 
        * @brief
        *  Timer for effect in browser mode.
        */
        Shared::CTimer m_kTimer;
    };



} // namespace Effects



#endif // #ifndef _BEAMSDIALOG_H_INCLUDED_
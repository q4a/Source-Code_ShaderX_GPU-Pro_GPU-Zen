///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CDisplacmentMapDialog.
 *
 * @file
 *  DisplacementMapDialog.h
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Declaration of \e Effects::CDisplacmentMapDialog.
 */



#ifndef _DISPLACEMENTMAPDIALOG_H_INCLUDED_
#define _DISPLACEMENTMAPDIALOG_H_INCLUDED_

#pragma once



#include "Timer.h"



namespace Effects
{



    /**
    * Dialog class for <em>Displacement map</em> effect settings.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  04.03.2002
    * @brief
    *  Dialog class for <em>Displacement map</em> effect settings.
    */
    class CDisplacementMapDialog : public CDialog
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
        CDisplacementMapDialog( CWnd* pParent = 0 );   

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CDisplacementMapDialog();      

        /**
        * Returns the current time of the effect.
        * 
        * @return
        *  The current time of the effect.
        * @brief
        *  Returns the current time of the effect.
        */
        float GetTime();

        /**
        * Returns the value to be used for scaling heights in the displacement map.
        * 
        * @return
        *  Scale value for heights in the displacement map.
        * @brief
        *  Returns the value to be used for scaling heights in the displacement map.
        */
        float GetHeightScale();
        
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

        /**
        * Message handler for <em>Height scale</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Height scale</em> slider bar.
        */
        afx_msg void OnReleasedCaptureHeightScale( NMHDR* pNMHDR, LRESULT* pResult );

        DECLARE_MESSAGE_MAP()        

    private:
        /**
        * Updates <em>Height scale</em> label. 
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Height scale</em> label.
        */
        void UpdateHeightScaleEdit();

    private:
        /** 
        * Current <em>Height scale</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Height scale</em> value mapped from slider bar.
        */
        int m_iHeightScale;

        /** 
        * Timer for effect in browser mode.
        * 
        * @brief
        *  Timer for effect in browser mode.
        */
        Shared::CTimer m_kTimer;
    };



} // namespace Effects



#endif // #ifndef _DISPLACEMENTMAPDIALOG_H_INCLUDED_
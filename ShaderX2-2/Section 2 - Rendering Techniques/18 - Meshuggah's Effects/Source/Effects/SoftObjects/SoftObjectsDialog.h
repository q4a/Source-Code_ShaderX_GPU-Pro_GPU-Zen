///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CSoftObjectsDialog.
 *
 * @file
 *  SoftObjectsDialog.h
 * @author
 *  Carsten Wenzel
 * @date
 *  13.03.2002
 * @brief
 *  Declaration of \e Effects::CSoftObjectsDialog.
 */



#ifndef _SOFTOBJECTSDIALOG_H_INCLUDED_
#define _SOFTOBJECTSDIALOG_H_INCLUDED_

#pragma once



#include "Timer.h"



namespace Effects
{



    /**
    * Dialog class for <em>Ocean scene</em> effect settings.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  08.03.2002
    * @brief
    *  Dialog class for <em>Ocean scene</em> effect settings.
    */
    class CSoftObjectsDialog : public CDialog
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
        CSoftObjectsDialog( CWnd* pParent = 0 );   

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CSoftObjectsDialog();      

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
        * Returns current number of soft objects.
        * 
        * @return
        *  The current number of soft objects.
        * @brief
        *  Returns current number of soft objects.
        */
        unsigned int GetNumSoftObjects();

        /**
        * Returns current grid size.
        * 
        * @return
        *  The current grid size..
        * @brief
        *  Returns current grid size.
        */
        unsigned char GetGridSize();

        /**
        * Returns current iso threshold value.
        * 
        * @return
        *  The current iso threshold value.
        * @brief
        *  Returns current iso threshold value.
        */
        float GetIsoThreshold();

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
        * Message handler for <em>Number of objects</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Number of objects</em> slider bar.
        */
        afx_msg void OnReleasedCaptureNumSoftObjects( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Grid size</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Grid size</em> slider bar.
        */
        afx_msg void OnReleasedCaptureGridSize( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Iso threshold</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Iso threshold</em> slider bar.
        */
        afx_msg void OnReleasedCaptureIsoThreshold( NMHDR* pNMHDR, LRESULT* pResult );

        DECLARE_MESSAGE_MAP()        

    private:
        /**
        * Updates <em>Number of objects</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Number of objects</em> label.
        */
        void UpdateNumSoftObjectsEdit();

        /**
        * Updates <em>Grid size</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Grid size</em> label.
        */
        void UpdateGridSizeEdit();

        /**
        * Updates <em>Iso threshold</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Iso threshold</em> label.
        */
        void UpdateIsoThresholdEdit();

    private:
        /** 
        * Current <em>Number of objects</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Number of objects</em> value mapped from slider bar.
        */
        unsigned int m_uiNumSoftObjects;

        /** 
        * Current <em>Grid size</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Grid size</em> value mapped from slider bar.
        */
        unsigned char m_ucGridSize;

        /** 
        * Current <em>Iso threshold</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Iso threshold</em> value mapped from slider bar.
        */
        int m_iIsoThreshold;

        /** 
        * Timer for effect in browser mode.
        * 
        * @brief
        *  Timer for effect in browser mode.
        */
        Shared::CTimer m_kTimer;
    };



} // namespace Effects



#endif // #ifndef _SOFTOBJECTSDIALOG_H_INCLUDED_
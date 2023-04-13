///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CHairDialog.
 *
 * @file
 *  HairDialog.h
 * @author
 *  Carsten Wenzel
 * @date
 *  06.03.2002
 * @brief
 *  Declaration of \e Effects::CHairDialog.
 */



#ifndef _HAIRDIALOG_H_INCLUDED_
#define _HAIRDIALOG_H_INCLUDED_

#pragma once



#include "Timer.h"



namespace Effects
{



    /**
    * A specialized static control class having a specific background color.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  06.03.2002
    * @brief
    * A specialized static control class having a specific background color.
    */
    class CColorCtrl : public CStatic
    {
    public:
        /**
        * Constructor.
        * 
        * @param col
        *  Default color for color control.
        * @brief
        *  Constructor.
        */
        CColorCtrl( const COLORREF& col );

        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CColorCtrl();

        /**
        * Sets new color for color control. Updates #m_hBrush.
        * 
        * @param col
        *  New color for color control.
        * @return
        * @brief
        *  Sets new color for color control.
        */
        void SetColor( const COLORREF& col );

    protected:
        /** 
        * Prepares drawing of control. Returns #m_hBrush.
        * 
        * @brief
        *  Prepares drawing of control.
        */
        afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );

        DECLARE_MESSAGE_MAP()

    private:
        /** 
        * A solid brush used to draw the control. 
        * 
        * @brief
        *  A solid brush used to draw the control.
        */
        HBRUSH m_hBrush;
    };
    
    

    /**
    * Dialog class for \e Hair effect settings.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  06.03.2002
    * @brief
    *  Dialog class for \e Hair effect settings.
    */
    class CHairDialog : public CDialog
    {
    public:
        /**
        * Constructor.
        * 
        * @param vDiffuseColor
        *  Diffuse hair color.
        * @param vSpecularColor
        *  Specular hair color.
        * @param vAnisotropySettings
        *  Anisotropy settings.
        * @param pParent
        *  Pointer to parent window.
        * @brief
        *  Constructor.
        */
        CHairDialog( const D3DXVECTOR4& vDiffuseColor, 
                     const D3DXVECTOR4& vSpecularColor,
                     const D3DXVECTOR4& vAnisotropySettings,
                     CWnd* pParent = 0 );   

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CHairDialog();     

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
        * Gets anisotropy settings.
        * 
        * @param vAnisotropicSettings
        *  Reference to a vector receiving the new anisotropy settings.
        * @return
        *  \b true if anisotropy settings had changed before this method was called 
        *  (that is, #m_bAnsiotropyParamsChanged is \b true) else \b false.
        * @brief
        *  Gets anisotropy settings.
        */
        bool GetAnisotropySettings( D3DXVECTOR4& vAnisotropicSettings );

        /**
        * Gets current diffuse hair color.
        * 
        * @param vDiffuseColor
        *  Reference to a vector receiving the current diffuse hair color.
        * @brief
        *  Gets current diffuse hair color.
        */
        void GetDiffuseColor( D3DXVECTOR4& vDiffuseColor );

        /**
        * Gets current specular hair color.
        * 
        * @param vSpecularColor
        *  Reference to a vector receiving the current specular hair color.
        * @brief
        *  Gets current specular hair color.
        */
        void GetSpecularColor( D3DXVECTOR4& vSpecularColor );

        /**
        * Gets current gravity value.
        * 
        * @return
        *  Current inertia value.
        * @brief
        *  Gets current gravity value.
        */
        float GetGravity();

        /**
        * Gets current inertia value.
        * 
        * @return
        *  Current inertia value.
        * @brief
        *  Gets current inertia value.
        */
        float GetInertia();
        
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
        * Message handler for <em>Diffuse color</em> button.
        * 
        * @brief
        *  Message handler for <em>Diffuse color</em> button.
        */
        afx_msg void OnDiffuseColor();

        /**
        * Message handler for <em>Specular color</em> button.
        * 
        * @brief
        *  Message handler for <em>Specular color</em> button.
        */
        afx_msg void OnSpecularColor();

        /**
        * Message handler for <em>Ambient light</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Ambient light</em> slider bar.
        */
        afx_msg void OnReleasedCaptureAmbient( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Diffuse light</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Diffuse light</em> slider bar.
        */
        afx_msg void OnReleasedCaptureDiffuse( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Specular light</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Specular light</em> slider bar.
        */
        afx_msg void OnReleasedCaptureSpecular( NMHDR* pNMHDR, LRESULT* pResult );
        
        /**
        * Message handler for <em>Gloss factor</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Gloss factor</em> slider bar.
        */
        afx_msg void OnReleasedCaptureGloss( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for \e Gravity slider bar.
        * 
        * @brief
        *  Message handler for \e Gravity slider bar.
        */
        afx_msg void OnReleasedCaptureGravity( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for \e Inertia slider bar.
        * 
        * @brief
        *  Message handler for \e Inertia slider bar.
        */
        afx_msg void OnReleasedCaptureInertia( NMHDR* pNMHDR, LRESULT* pResult );

        DECLARE_MESSAGE_MAP()        

    private:
        /**
        * Updates <em>Ambient light</em> label.
        * New value is based on current position of the corresponding slider bar.
        *
        * @brief
        *  Updates <em>Ambient light</em> label.
        */
        void UpdateAmbientEdit();

        /**
        * Updates <em>Diffuse light</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Diffuse light</em> label.
        */
        void UpdateDiffuseEdit();

        /**
        * Updates <em>Specular light</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Specular light</em> label.
        */
        void UpdateSpecularEdit();
        
        /**
        * Updates <em>Gloss factor</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Gloss factor</em> label.
        */
        void UpdateGlossEdit();

        /**
        * Updates \e Gravity label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates \e Gravity label.
        */
        void UpdateGravityEdit();

        /**
        * Updates \e Inertia label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates \e Inertia label.
        */
        void UpdateInertiaEdit();

    private:
        /** 
        * Current <em>diffuse hair</em> color.
        * 
        * @brief
        *  Current <em>diffuse hair</em> color.
        */
        COLORREF m_sDiffuseColor;

        /** 
        * Current <em>specular hair</em> color.
        * 
        * @brief
        *  Current <em>specular hair</em> color.
        */
        COLORREF m_sSpecularColor;
        
        /** 
        * Color control for diffuse hair color.
        * 
        * @brief
        *  Color control for diffuse hair color.
        */
        CColorCtrl m_kDiffuseColorCtrl;

        /** 
        * Color control for specular hair color.
        * 
        * @brief
        *  Color control for specular hair color.
        */
        CColorCtrl m_kSpecularColorCtrl;

        /** 
        * Current <em>ambient light</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>ambient light</em> value mapped from slider bar.
        */
        int m_iAmbient;

        /** 
        * Current <em>diffuse light</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>diffuse light</em> value mapped from slider bar.
        */
        int m_iDiffuse;

        /** 
        * Current <em>specular light</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>specular light</em> value mapped from slider bar.
        */
        int m_iSpecular;

        /** 
        * Current <em>gloss factor</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>gloss factor</em> value mapped from slider bar.
        */
        int m_iGloss;

        /** 
        * A flag indicating that anisotropy parameters have changed.
        * 
        * @brief
        *  A flag indicating that anisotropy parameters have changed.
        */
        bool m_bAnsiotropyParamsChanged;

        /** 
        * Current \e Gravity value mapped from slider bar.
        * 
        * @brief
        *  Current \e Gravity value mapped from slider bar.
        */
        int m_iGravity;

        /** 
        * Current \e Inertia value mapped from slider bar.
        * 
        * @brief
        *  Current \e Inertia value mapped from slider bar.
        */
        int m_iInertia;

        /** 
        * Timer for effect in browser mode.
        * 
        * @brief
        *  Timer for effect in browser mode.
        */
        Shared::CTimer m_kTimer;
    };



} // namespace Effects



#endif // #ifndef _HAIRDIALOG_H_INCLUDED_
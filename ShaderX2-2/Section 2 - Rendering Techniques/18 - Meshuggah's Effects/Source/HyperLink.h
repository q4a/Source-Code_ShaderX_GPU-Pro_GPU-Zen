///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Application::CHyperLink.
 *
 * @file
 *  HyperLink.h
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Declaration of \e Application::CHyperLink.
 */



#ifndef _HYPERLINK_H_INCLUDED_
#define _HYPERLINK_H_INCLUDED_

#pragma once



namespace Application
{



    /**
    * A hyperlink control for dialogs. Derived from MFC's \e CStatic. With tool tip
    * to show link address and different colors for normal, visisted, hovering states. 
    * The \e typical link cursor is dynamically loaded from winhlp32.exe the first time
    * it is needed.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  28.02.2002
    * @brief
    *  A hyperlink control for dialogs.
    */
    class CHyperLink : public CStatic
    {
    public:
        /**
        * Constructor.
        * 
        * @param strLink
        *  A string containing a \b http://, \b ftp:// or \b mailto: link that should
        *  be opend in a browser or email client when clicked on the control.
        * @param strSerializeVisitedState
        *  A string used to serialize the "visited" state in the registry under
        *  HKEY_CURRENT_USER\Software\Onion\Meshuggah\Links\ \e strSerializeVisitedState.
        *  If empty then serialization will be omitted.
        * @brief
        *  Constructor.
        */
        CHyperLink( CString strLink, CString strSerializeVisitedState = "" );

        /**
        * Constructor.
        * 
        * @param uiLinkID
        *  An ID of a string in the resource's string table containing a \b http://,
        *  \b ftp:// or \b mailto: link that should be opend in a browser or email
        *  client when clicked on the control.
        * @param uiSerializeID
        *  An ID of a string in the resource's string table used to serialize the "visited" state in the 
        *  registry under HKEY_CURRENT_USER\Software\Onion\Meshuggah\Links\ \e strSerializeVisitedState.
        *  If the default parameter is used then serialization will be omitted.
        * @brief
        *  Constructor.
        */
        CHyperLink( unsigned int uiLinkID, unsigned int uiSerializeID = -1 );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CHyperLink();

    public:
        /** 
        * Destroys the hyperlink control.
        * 
        * @brief
        *  Destroys the hyperlink control.
        */
        virtual BOOL DestroyWindow();

        /** 
        * Pre-translates messages. Enables to relay events to the tool tip.
        * 
        * @brief
        *  Pre-translates messages.
        */
        virtual BOOL PreTranslateMessage( MSG* pMsg );

    protected:
        /** 
        * Initializes tool tip and hyperlink font.
        * 
        * @brief
        *  Initializes tool tip and hyperlink font.
        */
        virtual void PreSubclassWindow();

    protected:
        /** 
        * Sets the control's text and background color.
        * 
        * @brief
        *  Sets the control's text and background color.
        */
        afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );

        /** 
        * Handles mouse moves on control.
        * 
        * @brief
        *  Handles mouse moves on control.
        */
        afx_msg void OnMouseMove( UINT nFlags, CPoint point );

        /** 
        * Forces hit tests to be done by \e CHyperLink.
        * 
        * @brief
        *  Forces hit tests to be done by \e CHyperLink.
        */
        afx_msg UINT OnNcHitTest( CPoint point );

        /** 
        * Sets mouse cursor for control.
        * 
        * @brief
        *  Sets mouse cursor for control.
        */
        afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );

        /** 
        * Timer function to update hover state.
        * 
        * @brief
        *  Timer function to update hover state.
        */
        afx_msg void OnTimer( UINT nIDEvent );

        /** 
        * Handles "Left mouse button down" messages.
        * 
        * @brief
        *  Handles "Left mouse button down" messages.
        */
        afx_msg void OnLButtonDown( UINT nFlags, CPoint point );

        DECLARE_MESSAGE_MAP()

    private:
        /** 
        * A string containing a \b http://, \b ftp:// or \b mailto: link that should
        * be opend in a browser or email client when clicked on the control.
        * 
        * @brief
        *  A string containing a \b http://, \b ftp:// or \b mailto: link that should
        *  be opend in a browser or email client when clicked on the control.
        */
        CString m_strLink;            

        /** 
        * A string used to serialize the "visited" state in the registry under
        * HKEY_CURRENT_USER\Software\Onion\Meshuggah\Links\ \e strSerializeVisitedState.
        * If empty then serialization will be omitted.
        *
        * @brief
        *  A string used to serialize the "visited" state in the registry under
        *  HKEY_CURRENT_USER\Software\Onion\Meshuggah\Links\ \e strSerializeVisitedState.
        *  If empty then serialization will be omitted.
        */
        CString m_strSerializeVisitedState;

        /** 
        * A brush used to draw the control.
        * 
        * @brief
        *  A brush used to draw the control.
        */
        HBRUSH m_hBrush;

        /** 
        * Vistited state. Used for serialization and drawing the control.
        * 
        * @brief
        *  Vistited state.
        */
        bool m_bVisited;

        /** 
        * Hovering state. Indicates that mouse is over control.
        * Used for drawing the control.
        * 
        * @brief
        *  Hovering state.
        */
        bool m_bHovering;

        /** 
        * Font for drawing the control. Glyphs are underlined.
        * 
        * @brief
        *  Font for drawing the control. Glyphs are underlined.
        */
        CFont m_kFont;

        /** 
        * Tool tip for showing the link address.
        * 
        * @brief
        *  Tool tip for showing the link address.
        */
        CToolTipCtrl m_kToolTip;

        /**
        * Structure containing a handle to the \e typical windows link cursor.
        *
        * @author
        *  Carsten Wenzel
        * @date
        *  28.02.2002
        * @brief
        *  Structure containing a handle to the \e typical windows link cursor.
        */
        struct SLinkCursor
        {
            /**
            * Constructor. Loads the \e typical windows link cursor from winhlp32.exe.
            * 
            * @brief
            *  Constructor.
            */
            SLinkCursor();

            /** 
            * Destructor.
            * 
            * @brief
            *  Destructor.
            */
            ~SLinkCursor();

            /** 
            * Cursor handle.
            * 
            * @brief
            *  Cursor handle.
            */
            HCURSOR m_hLinkCursor;
        };
    };



} // namespace Application



#endif // #ifndef _HYPERLINK_H_INCLUDED_

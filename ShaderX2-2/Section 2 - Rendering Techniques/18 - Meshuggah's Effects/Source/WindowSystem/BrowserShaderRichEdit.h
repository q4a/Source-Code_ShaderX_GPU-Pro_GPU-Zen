///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::CBrowserShaderRichEdit.
 *
 * @file
 *  BrowserShaderRichEdit.h
 * @author
 *  Carsten Wenzel
 * @date
 *  08.11.2001
 * @brief
 *  Declaration of \e WindowSystem::CBrowserShaderRichEdit.
 */



#ifndef _BROWSERSHADERRICHEDIT_H_INCLUDED_
#define _BROWSERSHADERRICHEDIT_H_INCLUDED_

#pragma once



namespace WindowSystem
{

    
    
    /**
    * A specialized rich edit control class to display shader source code.
    * Embedded in WindowSystem::CBrowserShaderFormView.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  02.03.2002
    * @brief
    *  A specialized rich edit control class to display shader source code.
    */
    class CBrowserShaderRichEdit : public CRichEditCtrl
    {
    public:
        /**
        * Constructor.
        * 
        * @param strShaderEditToolTip
        *  Text that should be displayed as a quick when mouse is over control.
        * @brief
        *  Constructor.
        */
        CBrowserShaderRichEdit( CString strShaderEditToolTip );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CBrowserShaderRichEdit();

        /** 
        * Pre-translates messages. Enables to relay events to the tool tip.
        * 
        * @brief
        *  Pre-translates messages.
        */
        virtual BOOL PreTranslateMessage( MSG* pMsg );

    protected:
        /**
        * Message handler for \c WM_CREATE messages.
        * Creates the control, loads <em>Courier New</em> into #m_kFont to be used for 
        * text display and attaches a tool tip using #m_strToolTip.
        * 
        * @param lpCreateStruct
        *  Window creation parameters.
        * @return
        *  \b 0 if successful. \b -1 if creation failed.
        * @brief
        *  Message handler for \c WM_CREATE messages.
        */
        afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );

        /** 
        * Message handler for \c WM_SIZE messages. Resizes rectangle 
        * of tool tip (#m_kToolTip) when parent's window size changes.
        * 
        * @brief
        *  Message handler for \c WM_SIZE messages.
        */
        afx_msg void OnSize( UINT nType, int cx, int cy );

        DECLARE_MESSAGE_MAP()

    private:
        /** 
        * Font used for text in rich edit control.
        * 
        * @brief
        *  Font used for text in rich edit control.
        */
        CFont m_kFont;

        /** 
        * Tool tip control. 
        * 
        * @brief
        *  Tool tip control.
        */
        CToolTipCtrl m_kToolTip;

        /** 
        * Text for tool tip control.
        * 
        * @brief
        *  Text for tool tip control.
        */
        CString m_strToolTip;
    };



} // namespace WindowSystem



#endif // #ifndef _BROWSERSHADERRICHEDIT_H_INCLUDED_
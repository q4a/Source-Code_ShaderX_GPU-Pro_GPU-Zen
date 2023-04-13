///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::CBrowserShaderFormView.
 *
 * @file
 *  BrowserShaderFormView.h
 * @author
 *  Carsten Wenzel
 * @date
 *  08.11.2001
 * @brief
 *  Declaration of \e WindowSystem::CBrowserShaderFormView.
 */



#ifndef _BROWSERSHADERFORMVIEW_H_INCLUDED_
#define _BROWSERSHADERFORMVIEW_H_INCLUDED_

#pragma once



#include "resource.h"
#include "BrowserShaderRichEdit.h"
#include "ShaderSourceList.h"



namespace EffectSystem
{
    class IEffect;
}



namespace WindowSystem
{



    /**
    * A specialized form view class containing a tab control to select a shader and 
    * two WindowSystem::CBrowserShaderRichEdit controls to display the associated
    * vertex and pixel shader source codes.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  02.03.2002
    * @brief
    *  A specialized form view class.
    */
    class CBrowserShaderFormView : public CFormView
    {
    public:
        enum EFormView
        { 
            /** 
            * Resource ID of form view's dialog template.
            */
            IDD = IDD_SHADERFORMVIEW 
        };
        
        /**
        * Updates the entire shader form view. Clears #m_colShaders and copies all
        * shaders of an effect to it. Updates tab control and displays the first shader defined.
        * 
        * @param pkEffect
        *  Pointer to newly selected effect.
        * @brief
        *  Updates the entire shader form view.
        */
        void SetShaderSources( EffectSystem::IEffect* pkEffect );

    protected:
        DECLARE_DYNCREATE( CBrowserShaderFormView )

        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CBrowserShaderFormView();

        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CBrowserShaderFormView();

    #ifdef _DEBUG
        /** 
        * MFC debug hook. Only available in debug builds.
        * 
        * @brief
        *  MFC debug hook. Only available in debug builds.
        */
        virtual void AssertValid() const;

        /** 
        * MFC debug hook. Only available in debug builds.
        * 
        * @brief
        *  MFC debug hook. Only available in debug builds.
        */
        virtual void Dump( CDumpContext& dc ) const;
    #endif

        /**
        * Message handler for \c WM_CREATE messages.
        * Creates form view from dialog template and two write-protected
        * rich edit controls for vertex and pixel shader sources.
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
        * Message handler for \c WM_SIZE messages. Resizes the control when
        * parent's window size changes. Recalculates the layout of the tab 
        * and rich edit controls.
        * 
        * @brief
        *  Message handler for \c WM_SIZE messages.
        */
        afx_msg void OnSize( UINT nType, int cx, int cy );

        /** 
        * Handles selection changes of tab control. Displays the sources of the
        * newly selected shader.
        * 
        * @brief
        *  Handles selection changes of tab control.
        */
        afx_msg void OnSelChangeShaderTab( NMHDR* pNMHDR, LRESULT* pResult );

        DECLARE_MESSAGE_MAP()

    private:
        /**
        * Loads the specified vertex and pixel shader source into the two richedit controls
        * 
        * @param sShaderSource
        *  Structure containing the shader source.
        * @brief
        *  Loads the specified vertex and pixel shader source into the two richedit controls
        */
        void SetShaderCode( EffectSystem::SShaderSourceList::SShaderSource& sShaderSource );

        /** 
        * Rich edit control for vertex shader source.
        * 
        * @brief
        *  Rich edit control for vertex shader source.
        */
        CBrowserShaderRichEdit m_kEditVS;

        /** 
        * Rich edit control for pixel shader source.
        * 
        * @brief
        *  Rich edit control for pixel shader source.
        */
        CBrowserShaderRichEdit m_kEditPS;

        /** 
        * List of all shaders used by the current effect.
        * 
        * @see
        *  CBrowserShaderFormView::SetShaderSources().
        * @brief
        *  List of all shaders used by the current effect.
        */
        EffectSystem::SShaderSourceList m_colShaders;
    };



} // namespace WindowSystem



#endif // #ifndef _BROWSERSHADERFORMVIEW_H_INCLUDED_
///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::CDemoLoaderWnd.
 *
 * @file
 *  DemoLoaderWnd.h
 * @author
 *  Carsten Wenzel
 * @date
 *  02.03.2002
 * @brief
 *  Declaration of \e WindowSystem::CDemoLoaderWnd.
 */



#ifndef _DEMOLOADERDLG_H_INCLUDED_
#define _DEMOLOADERDLG_H_INCLUDED_

#pragma once



#include "resource.h"



namespace WindowSystem
{

    

    /**
    * A window class for a demo loader screen.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  02.03.2002
    * @brief
    *  A window class for a demo loader screen.
    */
    class CDemoLoaderWnd : public CWnd
    {
    public:
        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CDemoLoaderWnd();

        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CDemoLoaderWnd();

        /**
        * Creates the window. Loads the "Please wait" bitmap from the executable's
        * resource and creates a window of that size.
        * 
        * @param pParentWnd
        *  Pointer to paraent window.
        * @return
        *  \b true if window creation was successful else \b false.
        * @brief
        *  Creates the window.
        */
        BOOL Create( CWnd* pParentWnd = 0 );
        
    protected:
        /**
        * Message handler for \c WM_CREATE messages.
        * Creates the window and centers it.
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
        * Message handler for \c WM_PAINT messages. Paints the window.
        * 
        * @brief
        *  Message handler for \c WM_PAINT messages.
        */
        afx_msg void OnPaint();

        DECLARE_MESSAGE_MAP()

    private:
        /** 
        * "Please wait" bitmap.
        * 
        * @brief
        *  "Please wait" bitmap.
        */
        CBitmap m_kBitmap;
    };



} // namespace WindowSystem



#endif // #ifndef _DEMOLOADERDLG_H_INCLUDED_
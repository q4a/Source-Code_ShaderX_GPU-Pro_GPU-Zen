///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Predefined header file for the \e WindowSystem subproject.
 * 
 * @file
 *  StdAfx.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Predefined header file for the \e WindowSystem subproject.
 */



#ifndef _STDAFX_H__INCLUDED_
#define _STDAFX_H__INCLUDED_

#pragma once



#define WIN32_LEAN_AND_MEAN		
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		
#endif

#include <afx.h>
#include <afxwin.h>
#include <afxext.h>         
#include <afxcview.h>

#include "D3D8.h"
#include "D3DX8.h"

#include "Debug.h"



#pragma warning ( disable : 4786 )



/** 
 * The namespace \e WindowSystem contains all main window classes for both demo and browser.
 * 
 * @brief
 *  The namespace \e WindowSystem contains all main window classes for both demo and browser.
 */
namespace WindowSystem
{
}



// The following comments are just provided for Doxygen to properly create an inheritance diagram
// for window/dialog/application classes herein. All other MFC classes that weren't derived from are 
// left out as it would have been too tedious to included them all! ;)

/** 
 * MFC's \e CWnd. See MSDN for further information.
 * 
 * @class CWnd
 *  
 * @brief
 *  MFC's \e CWnd.
 */



/** 
 * MFC's \e CFrameWnd. See MSDN for further information.
 * 
 * @class CFrameWnd
 *  
 * @brief
 *  MFC's \e CFrameWnd.
 */



/** 
 * MFC's \e CDialog. See MSDN for further information.
 * 
 * @class CDialog
 *  
 * @brief
 *  MFC's \e CDialog.
 */



/** 
 * MFC's \e CStatic. See MSDN for further information.
 * 
 * @class CStatic
 *  
 * @brief
 *  MFC's \e CStatic.
 */



/** 
 * MFC's \e CRichEditCtrl. See MSDN for further information.
 * 
 * @class CRichEditCtrl
 *  
 * @brief
 *  MFC's \e CRichEditCtrl.
 */



/** 
 * MFC's \e CView. See MSDN for further information.
 * 
 * @class CView
 *  
 * @brief
 *  MFC's \e CView.
 */



/** 
 * MFC's \e CFromView. See MSDN for further information.
 * 
 * @class CFromView
 *  
 * @brief
 *  MFC's \e CFormView.
 */



/** 
 * MFC's \e CTreeView. See MSDN for further information.
 * 
 * @class CTreeView
 *  
 * @brief
 *  MFC's \e CTreeView.
 */



#endif // #ifndef _STDAFX_H__INCLUDED_

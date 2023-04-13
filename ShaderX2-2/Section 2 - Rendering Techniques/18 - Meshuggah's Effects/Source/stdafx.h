///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Predefined header file for the \e Meshuggah subproject.
 * 
 * @file
 *  StdAfx.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Predefined header file for the \e Meshuggah subproject.
 */



#ifndef _STDAFX_H_INCLUDED_
#define _STDAFX_H_INCLUDED_

#pragma once



// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400       // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400     // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif                      

#ifndef _WIN32_WINDOWS      // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE           // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400    // Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include "D3D8.h"
#include "D3DX8.h"

#include "Debug.h"

#pragma warning ( disable : 4786 )



/** 
 * The namespace \e Application contains the entry point for both demo and browser.
 * 
 * @brief
 *  The namespace \e Application contains the entry point for both demo and browser.
 */
namespace Application
{
}



// The following comments are just provided for Doxygen to properly create an inheritance diagram
// for all window/dialog/application classes herein. All other MFC classes that weren't derived from are 
// left out as it would have been too tedious to included them all! ;)

/** 
 * MFC's \e CWinApp. See MSDN for further information.
 * 
 * @class CWinApp
 *
 * @brief
 *  MFC's \e CWinApp.
 */



#endif // #ifndef _STDAFX_H_INCLUDED_

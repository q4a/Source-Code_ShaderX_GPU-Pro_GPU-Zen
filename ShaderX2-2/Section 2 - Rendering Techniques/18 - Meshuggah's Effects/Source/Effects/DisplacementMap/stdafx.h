///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Predefined header file for the \e Effects subproject (<em>Displacement map</em> effect).
 *
 * @file
 *  stdafx.h
 * @author
 *  Carsten Wenzel
 * @date
 *  17.12.2001
 * @brief
 *  Predefined header file for the \e Effects subproject (<em>Displacement map</em> effect).
 */



#ifndef _STDAFX_H_INCLUDED_
#define _STDAFX_H_INCLUDED_

#pragma once



#define WIN32_LEAN_AND_MEAN     
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        
#endif



#include <afx.h>
#include <afxwin.h>       
#include <afxcmn.h>       


#include "D3D8.h"
#include "D3DX8.h"

#include "Debug.h"
#include "Effect.h"



#pragma warning ( disable : 4786 )



#endif // #ifndef _STDAFX_H_INCLUDED_
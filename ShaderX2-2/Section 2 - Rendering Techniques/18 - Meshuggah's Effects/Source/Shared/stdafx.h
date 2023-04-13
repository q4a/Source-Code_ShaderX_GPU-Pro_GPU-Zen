///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Predefined header file for the \e Shared subproject.
 *
 * @file
 *  stdafx.h
 * @author
 *  Carsten Wenzel
 * @date
 *  08.11.2001
 * @brief
 *  Predefined header file for the \e Shared subproject.
 */



#ifndef _STDAFX_H_INCLUDED_
#define _STDAFX_H_INCLUDED_

#pragma once



#define WIN32_LEAN_AND_MEAN



#include "windows.h"
#include "D3D8.h"
#include "D3DX8.h"



#pragma warning ( disable : 4786 )



/** 
 * The namespace \e Shared contains code that is to be reused by the rest of the project.
 *
 * @brief
 *  The namespace \e Shared contains code that is to be reused by the rest of the project.
 */
namespace Shared
{
}



#endif // #ifndef _STDAFX_H_INCLUDED_
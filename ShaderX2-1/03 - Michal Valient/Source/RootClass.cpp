//*****************************************************************************
//* This file is part of the Dimension3 renderer library                      *
//* Copyright (C) 2003  Michal Valient (valient@host.sk)                      *
//*                                                                           *
//* Dimension3 renderer can be found at http://www.dimension3.host.sk         *
//*                                                                           *
//* This program is free software; you can redistribute it and/or modify      *
//* it under the terms of the GNU General Public License as published by      *
//* the Free Software Foundation; either version 2 of the License, or         *
//* (at your option) any later version.                                       *
//*                                                                           *
//* This program is distributed in the hope that it will be useful,           *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
//* GNU General Public License for more details.                              *
//*                                                                           *
//* You should have received a copy of the GNU General Public License         *
//* along with this program; if not, write to the Free Software               *
//* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA * 
//*****************************************************************************
//hdr_utility_remove_tag


// RootClass.cpp: implementation of the CRootClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RootClass.h"

#include "nommgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRootClass::CRootClass() {
}

CRootClass::~CRootClass() {
}

void *CRootClass::operator new(size_t s) {
    if (MemMgr==NULL) return malloc(s);
#ifndef D3_DEBUG_MEMORY_MANAGER
    else return MemMgr->xNew_Release(s);
#else
    else return MemMgr->xNew_Debug(s);
#endif
}

void CRootClass::operator delete(void *p) {
    if (MemMgr==NULL) free(p);
#ifndef D3_DEBUG_MEMORY_MANAGER
    else MemMgr->xDelete_Release(p);
#else
    else MemMgr->xDelete_Debug(p);
#endif
}



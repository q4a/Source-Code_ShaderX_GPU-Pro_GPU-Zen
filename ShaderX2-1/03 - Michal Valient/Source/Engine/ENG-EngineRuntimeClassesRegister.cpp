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


// Engine's Runtime classes initialisation
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-Engine.h"

///This method registers all classes derived from CBaseClass that engine knows for dynamic creation
void CEngine::RegisterRuntimeClasses(void) {
    /*
    int logE = _LOGB("Engine",NULL,D3_DV_GRP0_LEV0,"Registering runtime classes");
    CBaseClass *RClass;
    char RClassName[1024];
    //CDirTreeDirectory
    RClass = new CDirTreeDirectory("RuntimeClass",this);
    if (!RuntimeCreator->RegisterClass(RClass,NULL)) {
        RClass->GetClsName(RClassName,1024);
        _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"Failed to register class %s",RClassName);
    }
    _LOGE(logE,"Registered");
    */
}


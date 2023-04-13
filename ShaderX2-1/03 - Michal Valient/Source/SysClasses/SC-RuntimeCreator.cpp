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


#include "stdafx.h"
#include "sc-runtimecreator.h"
#include "sc-StrHashTable.h"

CRuntimeCreator::CRuntimeCreator(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP0_LEV0, "Creating");
    Classes = new CStrHashTable(true, 0);
}

CRuntimeCreator::~CRuntimeCreator(void) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0, "Destructing");
    delete Classes;
    _LOGE(logE, "Destructed");
}

bool CRuntimeCreator::RegisterClass(CBaseClass *Class, void *RegisterInfo) {
    int NameSize = Class->GetClsName(NULL,0);
    char *Name = (char *)MemMgr->AllocMem(NameSize+1,0);
    NameSize = Class->GetClsName(Name,NameSize+1);
    int logE = _LOGB(this,D3_DV_GRP0_LEV1, "Registering class %s",Name);
    CRuntimeInfo *Nfo = new CRuntimeInfo();
    Nfo->ExistingInstance = Class;
    Nfo->RegisterInfo = RegisterInfo;
    CMString ClassName(Name);
    Classes->Add(ClassName.c_str(), ClassName.Length(), Nfo);
    MemMgr->FreeMem(Name);
    _LOGE(logE);
    return true;
}

CBaseClass *CRuntimeCreator::CreateClass(char *ClassName, char *ObjectName, void *Parameters) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV4, "Creating object %s from class %s", ObjectName, ClassName);
    CBaseClass *result = NULL;
    CMString Name(ClassName);
    CRuntimeInfo *Origin = (CRuntimeInfo *)Classes->Get(Name.c_str(), Name.Length());
    if (Origin!=NULL) {
        result = Origin->ExistingInstance->Create(ObjectName,this->OwnerEngine, Origin->RegisterInfo, Parameters);
        if (result==NULL) _LOGE(logE,"NOT CREATED - Classes Create method failed");
        else {
            char on[128],cn[128];
            result->GetObjName(on,128);
            result->GetClsName(cn,128);
            _LOGE(logE,"Created class %s.%s",on,cn);
        }
    } else {
        _LOGE(logE,"NOT CREATED - Runtime class not found");
    }
    return result;
}

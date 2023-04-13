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


// ENG-MaterialManager.cpp: implementation of the CMaterialManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Material\Mat-MaterialManager.h"
#include "Material\Mat-BaseMaterial.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMaterialManager::CMaterialManager(char *ObjName, CEngine *OwnerEngine) : CResourceManager(ObjName,OwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Creating");
    this->CurrentMaterial = NULL;
    this->CurrentPass = 0;
    _LOGE(logE);
}

CMaterialManager::~CMaterialManager() {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Destroying");
    if (CurrentMaterial!=NULL) {
        _WARN(this,ErrMgr->TMemory,"Material %u (%s) was left as current. Possible memory leaks in Direct3DDevice", CurrentMaterial->GetResourceID(), CurrentMaterial->GetObjName());
    }
    _LOGE(logE);
}

bool CMaterialManager::SetMaterial(CBaseMaterial *Material, CBaseMaterialContext *Context, DWORD Pass, CRenderSettings &RenderSettings) {
    int logE = 0;
    bool result;
    if (Material==NULL) {   //unset material
        logE = _LOGB(this,D3_DV_GRP0_LEV4,"Setting material to NULL");
        if (CurrentMaterial) {
            CurrentMaterial->UnSet(RenderSettings);
        }
        Context = NULL;
        Pass = 0xFFFFFFFF;
        result = true;
    } else if (Material != CurrentMaterial) {   //new material
        logE = _LOGB(this,D3_DV_GRP0_LEV4,"Setting new material %u (%s)",Material->GetResourceID(), Material->GetObjName());
        if (CurrentMaterial) {
            CurrentMaterial->UnSet(RenderSettings);
        }
        result = Material->Set(RenderSettings);
        result = result && Material->SetContext(Context);
        result = result && Material->SetPass(RenderSettings, Pass);
    } else {    //the same material
        result = true;
        if ((Context != CurrentContext) && (Material->GetHasContext())) {
            _LOG(this,D3_DV_GRP0_LEV4,"Material identical but context is other");
            result = result && Material->SetContext(Context);
        }
        if (Pass!= CurrentPass) {
            _LOG(this,D3_DV_GRP0_LEV4,"Material identical but pass is other");
            result = result && Material->SetPass(RenderSettings, Pass);
        }
    }
    if (!result) {
        CurrentMaterial = NULL;
        CurrentContext = NULL;
        CurrentPass = 0xFFFFFFFF;
        _WARN(this,ErrMgr->TNoSupport,"Cannot set material");
    } else {
        CurrentMaterial = Material;
        CurrentContext = Context;
        CurrentPass = Pass;
    }
    _LOGE(logE);
    return result;
}

void CMaterialManager::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Sending device lost");
    SetMaterial(NULL,NULL,0,Settings);
    CResourceManager::DeviceLost(Settings);
    _LOGE(logE,"Sended device lost");
}


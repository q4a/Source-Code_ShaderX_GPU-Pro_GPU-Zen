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


// SHD-ShaderManager.cpp: implementation of the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shader\SHD-ShaderManager.h"
#include "shader\shd-baseshader.h"
#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShaderManager::CShaderManager(char *ObjName, CEngine *OwnerEngine) : CResourceManager(ObjName,OwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Creating");
    PixelShader = NULL;
    VertexShader = NULL;
    _LOGE(logE);
}

CShaderManager::~CShaderManager() {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Destroying");
    _LOGE(logE);
}

void CShaderManager::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Sending device lost");
    SetPixelShader(NULL, Settings);
    SetVertexShader(NULL, Settings);
    CResourceManager::DeviceLost(Settings);
    _LOGE(logE,"Sended device lost");
}

bool CShaderManager::SetPixelShader(CBaseShader *PixelShader, CRenderSettings &Settings) {
    if (this->PixelShader != PixelShader) { //not the same
        if (this->PixelShader) this->PixelShader->SetIsSet(false);
        if (PixelShader) {
            _LOG(this, D3_DV_GRP0_LEV4, "Setting new pixel shader %s (%p)", PixelShader->GetObjName(), PixelShader);
            if (PixelShader->Set(this, false, Settings)) {
                this->PixelShader = PixelShader;
            } else {
                _WARN(this, ErrMgr->TNoSupport, "Cannot set pixel shader %s (%p)", PixelShader->GetObjName(), PixelShader);
                this->PixelShader = NULL;
                return false;
            }
        } else {
            _LOG(this, D3_DV_GRP0_LEV4, "Resetting pixel to NULL");
            Settings.Direct3DDevice->SetPixelShader(NULL);
            this->PixelShader = NULL;
        }
    } else {    //the same
        if (PixelShader) {
            _LOG(this, D3_DV_GRP0_LEV4, "Setting identical pixel shader %s (%p)", PixelShader->GetObjName(), PixelShader);
        }
    }
    return true;
}

bool CShaderManager::SetVertexShader(CBaseShader *VertexShader, CRenderSettings &Settings) {
    if (this->VertexShader != VertexShader) { //not the same
        if (this->VertexShader) this->VertexShader->SetIsSet(false);
        if (VertexShader) {
            _LOG(this, D3_DV_GRP0_LEV4, "Setting new vertex shader %s (%p)", VertexShader->GetObjName(), VertexShader);
            if (VertexShader->Set(this, true, Settings)) {
                this->VertexShader = VertexShader;
            } else {
                _WARN(this, ErrMgr->TNoSupport, "Cannot set vertex shader %s (%p)", VertexShader->GetObjName(), VertexShader);
                this->VertexShader = NULL;
                return false;
            }
        } else {
            _LOG(this, D3_DV_GRP0_LEV4, "Resetting vertex to NULL");
            Settings.Direct3DDevice->SetVertexShader(NULL);
            this->VertexShader = NULL;
        }
    } else {    //the same
        if (VertexShader) {
            _LOG(this, D3_DV_GRP0_LEV4, "Setting identical vertex shader %s (%p)", VertexShader->GetObjName(), VertexShader);
        }
    }
    return true;
}

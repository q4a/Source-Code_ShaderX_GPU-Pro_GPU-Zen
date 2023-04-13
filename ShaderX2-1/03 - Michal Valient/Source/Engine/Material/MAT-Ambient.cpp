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
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "material\MAT-Ambient.h"
#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "ENG-Texture2D.h"
#include "ENG-TextureManager.h"
#include "ENG-Base3DObject.h"
#include "MGR-DirectGraphicsManager.h"
#include "WND-WindowManager.h"
#include "FIL-FileManager.h"
#include "SC-MString.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "ENG-BaseTexture.h"
#include "d3scene\eng-d3slight.h"
#include "sc-dynamicarray.h"
#include "eng-enginedefaults.h"

CMaterialAmbient::CMaterialAmbient(char *ObjName,CEngine *OwnerEngine) : CDX8Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    AmbientPS = NULL;
    AmbientVS = NULL;
    VSDeclaration = NULL;
    DefaultTexture = NULL;
    DefaultEmisive = NULL;
}

CMaterialAmbient::~CMaterialAmbient(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    DeleteShaders();
    DeleteTextures();
}

void CMaterialAmbient::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheIn(RenderSettings);
    if (DefaultEmisive!=NULL) DefaultEmisive->CacheIn(RenderSettings);
}

void CMaterialAmbient::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheOut(RenderSettings);
    if (DefaultEmisive!=NULL) DefaultEmisive->CacheOut(RenderSettings);
}

DWORD CMaterialAmbient::GetPassCount() {
    return 1;
}

void CMaterialAmbient::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CMaterialAmbient::CreateContext(CBase3DObject *Obj) {
    CMaterialAmbientContext *con = new CMaterialAmbientContext;
    con->Object = Obj;
    return con;
}

void CMaterialAmbient::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CMaterialAmbient::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CMaterialAmbientContext *)Context;
    return true;
}

bool CMaterialAmbient::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        SetCommon(RenderSettings);
        SetIsSet(true);
    }
    return true;
}

bool CMaterialAmbient::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
    return true;
}

bool CMaterialAmbient::UnSet(CRenderSettings &RenderSettings) {
    RenderSettings.Direct3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    SetIsSet(false);
    return true;
}


void CMaterialAmbient::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CMaterialAmbient::DeleteShaders() {
    if (VSDeclaration!=NULL) {
        VSDeclaration->Release();
        VSDeclaration = NULL;
    }
    if (AmbientVS!=NULL) {
        DeleteVS(AmbientVS);
        AmbientVS = NULL;
    }
    if (AmbientPS!=NULL) {
        DeletePS(AmbientPS);
        AmbientPS = NULL;
    }
}

void CMaterialAmbient::DeleteTextures() {
    if (DefaultTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultTexture->GetResourceID());
        DefaultTexture = NULL;
    }
    if (DefaultEmisive!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultEmisive->GetResourceID());
        DefaultEmisive = NULL;
    }
}

bool CMaterialAmbient::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading material");
    DeleteShaders();
    //Vertex shaders support
    if((OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) || 
        (OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)) {
            if( OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(1,1) ) {
                _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 1.1 not supported");
            }
    }
    //pixel shaders support
    if(OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DVS_VERSION(1,4)) {
        _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 1.4 not supported");
    }

    //shader input declaration
    D3DVERTEXELEMENT9  dwDecl[] = {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        D3DDECL_END()
    };
    RenderSettings.Direct3DDevice->CreateVertexDeclaration(dwDecl, &VSDeclaration);

    //Ambient
    CMString VSFile = "CMaterialAmbient.vsh";
    AmbientVS = LoadVS_Src(VSFile, RenderSettings);
    CMString PSFile = "CMaterialAmbient.psh";
    AmbientPS = LoadPS_Src(PSFile, RenderSettings);

    DeleteTextures();

    //Load default diffuse texture
    DefaultTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_Gray);
    if (DefaultTexture==NULL) {
        CMString LMFile = "D3F_DefaultTexture.bmp";
        DefaultTexture = new CTexture2D(LMFile.c_str(),OwnerEngine);
        DefaultTexture->SetResourceID(D3_GRI_Tex_Gray);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        DefaultTexture->Load(resindex,RenderSettings);
        OwnerEngine->GetTextureManager()->Add(DefaultTexture->GetResourceID(),*DefaultTexture);
    }

    //Load default normal map texture
    DefaultEmisive = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_Black);
    if (DefaultEmisive==NULL) {
        CMString LMFile = "D3F_DefaultBlack.bmp";
        CTexture2D *TempPointer = new CTexture2D(LMFile.c_str(),OwnerEngine);
        TempPointer->SetResourceID(D3_GRI_Tex_Black);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        TempPointer->Load(resindex,RenderSettings);
        DefaultEmisive = TempPointer;
        OwnerEngine->GetTextureManager()->Add(DefaultEmisive->GetResourceID(),*DefaultEmisive);
    }

    SetFilled(true);
    _LOGE(logE,"Loaded");
    return true;
}

//Specific set / update methods
bool CMaterialAmbient::SetCommon(CRenderSettings &RenderSettings) {

    RenderSettings.Direct3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);  //Flat shading is enough

    if (this->AmbientPS && this->AmbientVS) {
        HRESULT res = RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);
        res = RenderSettings.Direct3DDevice->SetVertexShader(AmbientVS);
        if (FAILED(res)) {
            _WARN(this,ErrMgr->TNoSupport, "Cannot set vertex shader - %p",AmbientVS);
        }
        res = RenderSettings.Direct3DDevice->SetPixelShader(AmbientPS);
        if (FAILED(res)) {
            _WARN(this,ErrMgr->TNoSupport, "Cannot set pixel shader - %p",AmbientPS);
        }
    }
    return true;
}

bool CMaterialAmbient::UpdateAmbient(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        CMaterialAmbientContext *Cntx = (CMaterialAmbientContext *)CurrentContext;
        if (Cntx->DiffuseTexture!=NULL) {
            this->OwnerEngine->GetTextureManager()->SetTexture(1,Cntx->DiffuseTexture);
        } else {
            this->OwnerEngine->GetTextureManager()->SetTexture(1,DefaultEmisive);
        }
        if (Cntx->AmbientTexture!=NULL) {
            this->OwnerEngine->GetTextureManager()->SetTexture(0,Cntx->AmbientTexture);
        } else {
            this->OwnerEngine->GetTextureManager()->SetTexture(0,DefaultTexture);
        }

        // world * view * proj matrix
        D3DXMATRIX tmp;
        D3DXMatrixMultiplyTranspose(&tmp, &CurrentContext->Object->WorldTransform, &RenderSettings.CameraTransformMatrix);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(0, tmp, 4);

        if (!Cntx->CacheDone) { //Cache constant values for pixel shader
            D3DXVECTOR4 AmbientLight(1,1,1,1);
            Cntx->CachedAmbMul = AmbientLight * Cntx->AmbientMultiplier;
            float fSelfIllumOn = (Cntx->SelfIllumOn)? 1.0f : 0.0f;
            Cntx->CachedDiffMul = D3DXVECTOR4(1,1,1,1) * (1.0f - fSelfIllumOn) * Cntx->SelfIllumValue * Cntx->DiffuseMultiplier;
            D3DXVECTOR4 tmpVec(Cntx->AmbientColor.x * AmbientLight.x, Cntx->AmbientColor.y * AmbientLight.y, Cntx->AmbientColor.z * AmbientLight.z, Cntx->AmbientColor.w * AmbientLight.w);
            Cntx->CachedConstant =  tmpVec * (1.0f - Cntx->AmbientMultiplier) +
                                    fSelfIllumOn * Cntx->SelfIllumColor +
                                    (1.0f - fSelfIllumOn) * Cntx->SelfIllumValue * (1.0f - Cntx->DiffuseMultiplier) * Cntx->DiffuseColor;
            Cntx->CacheDone = true;
        }
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(0,Cntx->CachedAmbMul,1);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(1,Cntx->CachedDiffMul,1);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(2,Cntx->CachedConstant,1);
    }
    return true;
}

bool CMaterialAmbient::Update(CRenderSettings &RenderSettings) {
    return UpdateAmbient(RenderSettings);
}

//***********************************************
//Material context
//***********************************************
CMaterialAmbientContext::CMaterialAmbientContext() : CDX8MaterialContext() {
    AmbientTexture = NULL;
    DiffuseTexture = NULL;
    CacheDone = false;
}

CMaterialAmbientContext::~CMaterialAmbientContext() {
    if (AmbientTexture!=NULL) {
        AmbientTexture->OwnerEngine->GetTextureManager()->Delete(AmbientTexture->GetResourceID());
    }
    if (DiffuseTexture!=NULL) {
        DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
    }
}

int CMaterialAmbientContext::QueryProperty(const char *PropertyName) {
    if (strcmp(PropertyName, "AmbientMultiplier") == 0) return PID_AmbientMultiplier;
    if (strcmp(PropertyName, "AmbientColor") == 0) return PID_AmbientColor;
    if (strcmp(PropertyName, "AmbientTexture") == 0) return PID_AmbientTexture ;
    if (strcmp(PropertyName, "SelfIllumOn") == 0) return PID_SelfIllumOn;
    if (strcmp(PropertyName, "SelfIllumColor") == 0) return PID_SelfIllumColor;
    if (strcmp(PropertyName, "SelfIllumValue") == 0) return PID_SelfIllumValue;
    if (strcmp(PropertyName, "DiffuseMultiplier") == 0) return PID_DiffuseMultiplier;
    if (strcmp(PropertyName, "DiffuseColor") == 0) return PID_DiffuseColor;
    if (strcmp(PropertyName, "DiffuseTexture") == 0) return PID_DiffuseTexture;
    return CDX8MaterialContext::QueryProperty(PropertyName);
}

bool CMaterialAmbientContext::SetProperty(int PropertyID, const void *Property) {
    switch (PropertyID) {
        case PID_AmbientMultiplier : {
            AmbientMultiplier = *(float *)Property;
            return true;
        } break;
        case PID_AmbientColor : {
            AmbientColor = *(D3DXVECTOR4 *)Property;
            return true;
        } break;
        case PID_AmbientTexture : {
            if (AmbientTexture!=NULL) {
                AmbientTexture->OwnerEngine->GetTextureManager()->Delete(AmbientTexture->GetResourceID());
            }
            AmbientTexture = (CBaseTexture *)Property;
            return true;
        } break;
        case PID_SelfIllumOn : {
            SelfIllumOn = *(bool *)Property;
            return true;
        } break;
        case PID_SelfIllumValue : {
            SelfIllumValue = *(float *)Property;
            return true;
        } break;
        case PID_SelfIllumColor : {
            SelfIllumColor = *(D3DXVECTOR4 *)Property;
            return true;
        } break;
        case PID_DiffuseMultiplier : {
            DiffuseMultiplier = *(float *)Property;
            return true;
        } break;
        case PID_DiffuseColor : {
            DiffuseColor = *(D3DXVECTOR4 *)Property;
            return true;
        } break;
        case PID_DiffuseTexture : {
            if (DiffuseTexture!=NULL) {
                DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
            }
            DiffuseTexture = (CBaseTexture *)Property;
            return true;
        } break;
        default : {
            return CDX8MaterialContext::SetProperty(PropertyID, Property);
        }
    }
}

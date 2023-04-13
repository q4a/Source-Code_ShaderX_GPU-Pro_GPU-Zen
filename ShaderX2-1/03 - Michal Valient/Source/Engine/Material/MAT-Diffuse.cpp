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

#include "material\mat-Diffuse.h"
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

CMaterialDiffuse::CMaterialDiffuse(char *ObjName,CEngine *OwnerEngine) : CDX8Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    CurrentPass = 0xFFFFFFFF;
    VSDeclaration = NULL;
    CreatedVertexShader = 0;
    CreatedPixelShader = 0;
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
}

CMaterialDiffuse::~CMaterialDiffuse(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    DeleteShaders();
    DeleteTextures();
}

void CMaterialDiffuse::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheIn(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheIn(RenderSettings);
}

void CMaterialDiffuse::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheOut(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheOut(RenderSettings);
}

DWORD CMaterialDiffuse::GetPassCount() {
    if (CurrentContext!=NULL) {
        CMaterialDiffuseContext *Cntx = (CMaterialDiffuseContext *)CurrentContext;
        return Cntx->Object->Lights->GetSize();
    } else {
        return 0;
    }
}

void CMaterialDiffuse::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CMaterialDiffuse::CreateContext(CBase3DObject *Obj) {
    CMaterialDiffuseContext *con = new CMaterialDiffuseContext;
    con->Object = Obj;
    return con;
}

void CMaterialDiffuse::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CMaterialDiffuse::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CMaterialDiffuseContext *)Context;
    return true;
}

bool CMaterialDiffuse::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        SetCommon(RenderSettings);
        SetIsSet(true);
        CurrentPass = 0xFFFFFFFF;
    }
    return true;
}

bool CMaterialDiffuse::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
    if (PassNumber != CurrentPass) {
        if ((PassNumber==0) && (CurrentPass != 0xFFFFFFFF)) {
            RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);        //Enable writing to Zbuffer
            RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);   //Default Z function
            RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending
        } else {
            if ((CurrentPass == 0) || (CurrentPass == 0xFFFFFFFF)) {
                RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);       //Disable writing to Zbuffer
                RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);     //Z only if equal
                RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);   //Enable blending
            }
        }
        CurrentPass = PassNumber;
    }
    return true;
}

bool CMaterialDiffuse::UnSet(CRenderSettings &RenderSettings) {
    if ((GetIsSet()) && (CurrentPass > 0) && (CurrentPass!=0xFFFFFFFF)) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);        //Enable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);   //Default Z function
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending
        SetIsSet(false);
    }
    return true;
}

void CMaterialDiffuse::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CMaterialDiffuse::DeleteShaders() {
    if (VSDeclaration!=NULL) {
        VSDeclaration->Release();
        VSDeclaration = NULL;
    }
    if (CreatedVertexShader!=0) {
        DeleteVS(CreatedVertexShader);
        CreatedVertexShader = 0;
    }
    if (CreatedPixelShader!=0) {
        DeletePS(CreatedPixelShader);
        CreatedPixelShader = 0;
    }
}

void CMaterialDiffuse::DeleteTextures() {
    if (DefaultTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultTexture->GetResourceID());
        DefaultTexture = NULL;
    }
    if (DefaultNormalMap!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultNormalMap->GetResourceID());
        DefaultNormalMap = NULL;
    }
}

bool CMaterialDiffuse::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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

    //PerDiffuse
    CMString VSFile = "CMaterialDiffuse.vsh";
    CreatedVertexShader = LoadVS_Src(VSFile, RenderSettings);
    CMString PSFile = "CMaterialDiffuse.psh";
    CreatedPixelShader = LoadPS_Src(PSFile, RenderSettings);

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
    DefaultNormalMap = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_DefaultNotmalMap);
    if (DefaultNormalMap==NULL) {
        CMString LMFile = "D3F_DefaultBlack.bmp";
        CTexture2D *TempPointer = new CTexture2D(LMFile.c_str(),OwnerEngine);
        TempPointer->SetResourceID(D3_GRI_Tex_DefaultNotmalMap);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        TempPointer->Load(resindex,RenderSettings);
        TempPointer->ConvertToNormalMap(RenderSettings);
        DefaultNormalMap = TempPointer;
        OwnerEngine->GetTextureManager()->Add(DefaultNormalMap->GetResourceID(),*DefaultNormalMap);
    }

    SetFilled(true);
    _LOGE(logE,"Loaded");
    return true;
}

//Specific set / update methods
bool CMaterialDiffuse::SetCommon(CRenderSettings &RenderSettings) {
    if (CreatedVertexShader && CreatedPixelShader) {
        HRESULT res = RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);
        res = RenderSettings.Direct3DDevice->SetVertexShader(CreatedVertexShader);
        if (FAILED(res)) {
            _WARN(this,ErrMgr->TNoSupport, "Cannot set vertex shader - %p",CreatedVertexShader);
        }
        res = RenderSettings.Direct3DDevice->SetPixelShader(CreatedPixelShader);
        if (FAILED(res)) {
            _WARN(this,ErrMgr->TNoSupport, "Cannot set pixel shader - %p",CreatedPixelShader);
        }
    }
    return true;
}

bool CMaterialDiffuse::SetDiffuseSpecular(CRenderSettings &RenderSettings) {
    return true;
}

bool CMaterialDiffuse::UpdateDiffuseSpecular(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        CMaterialDiffuseContext *Cntx = (CMaterialDiffuseContext *)CurrentContext;
        //SetTexture
        if (Cntx->DiffuseTexture!=NULL) this->OwnerEngine->GetTextureManager()->SetTexture(0,Cntx->DiffuseTexture);
        else this->OwnerEngine->GetTextureManager()->SetTexture(0,DefaultTexture);
        if (Cntx->NormalTexture!=NULL) this->OwnerEngine->GetTextureManager()->SetTexture(1,Cntx->NormalTexture);
        else this->OwnerEngine->GetTextureManager()->SetTexture(1,DefaultNormalMap);

        // world * view * proj matrix
        D3DXMATRIX tmp;
        D3DXMatrixMultiplyTranspose(&tmp, &CurrentContext->Object->WorldTransform, &RenderSettings.CameraTransformMatrix);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(4, tmp, 4);	

        // world matrix
        D3DXMatrixTranspose(&tmp,&CurrentContext->Object->WorldTransform);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(0, tmp, 4);

        // light position
        CD3SLight *Light = (CD3SLight *)Cntx->Object->Lights->GetAt(CurrentPass);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(8, Light->WorldObjectPos,  1 );

        //Diffuse multiplicator
        D3DXCOLOR DiffMul = Light->LightColor * (float)Light->Intensity * (float)Cntx->DiffuseMultiplier;
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(0, DiffMul, 1);

        //Diffuse color constant
        D3DXCOLOR DiffConst = Cntx->DiffuseColor * (1.0f - Cntx->DiffuseMultiplier) * (float)Light->Intensity;
        D3DXColorModulate(&DiffConst, &Light->LightColor, &DiffConst);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(1, DiffConst, 1);
    }
    return true;
}


bool CMaterialDiffuse::Update(CRenderSettings &RenderSettings) {
    UpdateDiffuseSpecular(RenderSettings);
    return true;
}

//***********************************************
//Material context
//***********************************************
CMaterialDiffuseContext::CMaterialDiffuseContext() : CDX8MaterialContext() {
    DiffuseTexture = NULL;
    NormalTexture = NULL;
}

CMaterialDiffuseContext::~CMaterialDiffuseContext() {
    if (DiffuseTexture!=NULL) {
        DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
    }
    if (NormalTexture!=NULL) {
        NormalTexture->OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
    }
}

int CMaterialDiffuseContext::QueryProperty(const char *PropertyName) {
    if (strcmp(PropertyName, "DiffuseMultiplier") == 0) return PID_DiffuseMultiplier;
    if (strcmp(PropertyName, "DiffuseColor") == 0) return PID_DiffuseColor;
    if (strcmp(PropertyName, "DiffuseTexture") == 0) return PID_DiffuseTexture;
    if (strcmp(PropertyName, "NormalTexture") == 0) return PID_NormalTexture;
    return CDX8MaterialContext::QueryProperty(PropertyName);
}

bool CMaterialDiffuseContext::SetProperty(int PropertyID, const void *Property) {
    switch (PropertyID) {
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
        case PID_NormalTexture : {
            if (NormalTexture!=NULL) {
                NormalTexture->OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
            }
            NormalTexture = (CBaseTexture *)Property;
            return true;
        } break;
        default : {
            return CDX8MaterialContext::SetProperty(PropertyID, Property);
        }
    }
}

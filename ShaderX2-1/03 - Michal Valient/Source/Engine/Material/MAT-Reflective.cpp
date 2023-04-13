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

#include "material\MAT-Reflective.h"
#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "ENG-Texture2D.h"
#include "ENG-TextureCube.h"
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

CMaterialReflective::CMaterialReflective(char *ObjName,CEngine *OwnerEngine) : CDX8Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    CurrentPass = 0xFFFFFFFF;
    VSDeclaration = NULL;
    CreatedVertexShader = 0;
    CreatedPixelShader = 0;
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
    EnvCubeTexture = NULL;
}

CMaterialReflective::~CMaterialReflective(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    DeleteShaders();
    DeleteTextures();
}

void CMaterialReflective::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheIn(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheIn(RenderSettings);
    if (EnvCubeTexture!=NULL) EnvCubeTexture->CacheIn(RenderSettings);
    if (DefaultFresnelTexture!=NULL) DefaultFresnelTexture->CacheIn(RenderSettings);
}

void CMaterialReflective::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheOut(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheOut(RenderSettings);
    if (EnvCubeTexture!=NULL) EnvCubeTexture->CacheOut(RenderSettings);
    if (DefaultFresnelTexture!=NULL) DefaultFresnelTexture->CacheOut(RenderSettings);
}

DWORD CMaterialReflective::GetPassCount() {
    return 1;
}

void CMaterialReflective::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CMaterialReflective::CreateContext(CBase3DObject *Obj) {
    CMaterialReflectiveContext *con = new CMaterialReflectiveContext;
    con->Object = Obj;
    return con;
}

void CMaterialReflective::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CMaterialReflective::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CMaterialReflectiveContext *)Context;
    return true;
}

bool CMaterialReflective::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        this->OwnerEngine->GetTextureManager()->SetTexture(2,EnvCubeTexture);               //Environmental cube texture
        this->OwnerEngine->GetTextureManager()->SetTexture(3,DefaultFresnelTexture);        //Fresnel texture

        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);     //Disable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);     //Z only if equal
        
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
        CurrentPass = 0xFFFFFFFF;
        SetIsSet(true);
    }
    return true;
}

bool CMaterialReflective::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
    if (PassNumber != CurrentPass) {
        if (PassNumber == 0) {                  //First pass
            if (CurrentPass == 0xFFFFFFFF) {    //Newly set material
            } else {
            }
        } else {                                        //Subsequent passes
            if (CurrentPass == 0xFFFFFFFF) {            //Newly set material
                RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);  //Enable blending
            } else if (CurrentPass == 0) {              //Subsequent pass after first pass
                RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);  //Enable blending
            } else {                                    //Just new subsequent pass
            }
        }
        CurrentPass = PassNumber;
    }
    return true;
}

bool CMaterialReflective::UnSet(CRenderSettings &RenderSettings) {
    if (GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);        //Enable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);   //Default Z function
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending

        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

        SetIsSet(false);
    }
    return true;
}

void CMaterialReflective::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CMaterialReflective::DeleteShaders() {
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

void CMaterialReflective::DeleteTextures() {
    if (DefaultTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultTexture->GetResourceID());
        DefaultTexture = NULL;
    }
    if (DefaultNormalMap!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultNormalMap->GetResourceID());
        DefaultNormalMap = NULL;
    }
    if (EnvCubeTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(EnvCubeTexture->GetResourceID());
        EnvCubeTexture = NULL;
    }
    if (DefaultFresnelTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultFresnelTexture->GetResourceID());
        DefaultFresnelTexture = NULL;
    }
}

bool CMaterialReflective::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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
    CMString VSFile = "CMaterialReflective.vsh";
    CreatedVertexShader = LoadVS_Src(VSFile, RenderSettings);
    CMString PSFile = "CMaterialReflective.psh";
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

    //Load environmental cube texture
    EnvCubeTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_DefaultEnvMap);
    if (EnvCubeTexture==NULL) {
        //CMString LMFile = "D3F_DefaultCubeMap2.dds";
        CMString LMFile = "msLobby.dds";
        CTextureCube *TempPointer = new CTextureCube(LMFile.c_str(),OwnerEngine);
        TempPointer->SetResourceID(D3_GRI_Tex_DefaultEnvMap);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        TempPointer->Load(resindex,RenderSettings);
        EnvCubeTexture = TempPointer;
        OwnerEngine->GetTextureManager()->Add(EnvCubeTexture->GetResourceID(),*EnvCubeTexture);
    }

    //Load specular lightmap texture
    DefaultFresnelTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_FresnelMap);
    if (DefaultFresnelTexture==NULL) {
        CMString LMFile = "D3F_Fresnel_NotInterpolated.dds";
        CTexture2D *TempPointer = new CTexture2D(LMFile.c_str(),OwnerEngine);
        TempPointer->SetResourceID(D3_GRI_Tex_FresnelMap);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);

        CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();

        TempPointer->SetLoadParams( engDef->Txt2D_Width, engDef->Txt2D_Height, 1, engDef->Txt2D_Usage,
                                    engDef->Txt2D_Format, engDef->Txt2D_Pool, engDef->Txt2D_Filter, 
                                    engDef->Txt2D_MipFilter, engDef->Txt2D_ColorKey);

        TempPointer->Load(resindex,RenderSettings);
        DefaultFresnelTexture = TempPointer;
        OwnerEngine->GetTextureManager()->Add(DefaultFresnelTexture->GetResourceID(),*DefaultFresnelTexture);
    }

    SetFilled(true);
    _LOGE(logE,"Loaded");
    return true;
}

bool CMaterialReflective::Update(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        CMaterialReflectiveContext *Cntx = (CMaterialReflectiveContext *)CurrentContext;
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
        tmp = CurrentContext->Object->WorldTransform;
        D3DXMatrixTranspose(&tmp,&tmp);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(0, tmp, 4);

        // Eye position
        //D3DXVECTOR4 EyePosition(0,0,0,0);
        D3DXVECTOR4 EyePosition = RenderSettings.CameraSource;
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(8, EyePosition,  1 );

        //Vertex addition factor for cube map
        float Size = (float)((CTextureCube *)EnvCubeTexture)->Size;
        D3DXVECTOR4 VertFactor(1.0f / Size, 1.0f, 1.0f, 1.0f);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(9, VertFactor, 1);

        //Diffuse multiplicator
        D3DXCOLOR DiffMul(Cntx->DiffuseMultiplier, Cntx->DiffuseMultiplier, Cntx->DiffuseMultiplier, 1.0f);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(0, DiffMul, 1);

        //Diffuse color constant
        D3DXCOLOR DiffConst = Cntx->DiffuseColor * (1.0f - Cntx->DiffuseMultiplier);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(1, DiffConst, 1);

/*
        Refraction indexes. Taken from Matthias Wloka paper at www.nvidia.com.

        Material                Index of Refraction
        Vacuum                  1.0 (min. index of refraction)
        Air                     1.000293
        Ice                     1.31
        Water                   1.333333
        Ethyl Alcohol           1.36
        Fluorite                1.43
        Poppy Seed Oil          1.469
        Olive Oil               1.47
        Linseed Oil             1.478
        Plexiglas               1.51
        Immersion Oil           1.515
        Crown Glass             1.52
        Quartz                  1.54
        Salt                    1.54
        Light Flint Glass       1.58
        Dense Flint Glass       1.66
        Tourmaline              1.62
        Garnet                  1.73-1.89
        Zircon                  1.923
        Cubic Zirconia          2.14-2.20
        Diamond                 2.417
        Rutile                  2.907
        Gallium Phosphide       3.5 (max. index of refraction)
*/

        //precomputed refraction index
        float refractionIndexRatio = 2.417f;
        //float refractionIndexRatio = 1.0f;
        float rIndex = powf(1.0f - refractionIndexRatio, 2.0f) / powf(1.0f + refractionIndexRatio, 2.0f);
        D3DXCOLOR RefIndex(rIndex, rIndex, rIndex, rIndex);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(2, RefIndex, 1);
    }
    return true;
}

//***********************************************
//Material context
//***********************************************
CMaterialReflectiveContext::CMaterialReflectiveContext() : CDX8MaterialContext() {
    DiffuseTexture = NULL;
    NormalTexture = NULL;
}

CMaterialReflectiveContext::~CMaterialReflectiveContext() {
    if (DiffuseTexture!=NULL) {
        DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
    }
    if (NormalTexture!=NULL) {
        NormalTexture->OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
    }
}

int CMaterialReflectiveContext::QueryProperty(const char *PropertyName) {
    if (strcmp(PropertyName, "DiffuseMultiplier") == 0) return PID_DiffuseMultiplier;
    if (strcmp(PropertyName, "DiffuseColor") == 0) return PID_DiffuseColor;
    if (strcmp(PropertyName, "DiffuseTexture") == 0) return PID_DiffuseTexture;
    if (strcmp(PropertyName, "NormalTexture") == 0) return PID_NormalTexture;
    if (strcmp(PropertyName, "SpecularColor") == 0) return PID_SpecularColor;
    return CDX8MaterialContext::QueryProperty(PropertyName);
}

bool CMaterialReflectiveContext::SetProperty(int PropertyID, const void *Property) {
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
        case PID_SpecularColor : {
            SpecularColor = *(D3DXVECTOR4 *)Property;
            return true;
        } break;
        default : {
            return CDX8MaterialContext::SetProperty(PropertyID, Property);
        }
    }
}

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

#include "material\MAT-DiffuseSpecularSpot.h"
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

CMaterialDiffuseSpecularSpot::CMaterialDiffuseSpecularSpot(char *ObjName,CEngine *OwnerEngine) : CDX8Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    CurrentPass = 0xFFFFFFFF;
    VSDeclaration = NULL;
    CreatedVertexShader = 0;
    CreatedPixelShader = 0;
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
    LightMapTexture = NULL;
    SpotLightTexture = NULL;

    ShininessTextures = new CRootClassArray(false);
}

CMaterialDiffuseSpecularSpot::~CMaterialDiffuseSpecularSpot(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    DeleteShaders();
    DeleteTextures();
    delete ShininessTextures;
}

void CMaterialDiffuseSpecularSpot::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheIn(RenderSettings);
    if (SpotLightTexture!=NULL) LightMapTexture->CacheIn(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheIn(RenderSettings);
    if (LightMapTexture!=NULL) LightMapTexture->CacheIn(RenderSettings);

    for (int i=0; i<ShininessTextures->GetSize(); i++) {
        CBaseTexture *Tex = (CBaseTexture *)ShininessTextures->GetAt(i);
        Tex->CacheIn(RenderSettings);
    }
}

void CMaterialDiffuseSpecularSpot::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheOut(RenderSettings);
    if (SpotLightTexture!=NULL) LightMapTexture->CacheOut(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheOut(RenderSettings);
    if (LightMapTexture!=NULL) LightMapTexture->CacheOut(RenderSettings);

    for (int i=0; i<ShininessTextures->GetSize(); i++) {
        CBaseTexture *Tex = (CBaseTexture *)ShininessTextures->GetAt(i);
        Tex->CacheOut(RenderSettings);
    }
}

DWORD CMaterialDiffuseSpecularSpot::GetPassCount() {
    if (CurrentContext!=NULL) {
        CMaterialDiffuseSpecularSpotContext *Cntx = (CMaterialDiffuseSpecularSpotContext *)CurrentContext;
        return Cntx->Object->Lights->GetSize();
    } else {
        return 0;
    }
}

void CMaterialDiffuseSpecularSpot::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CMaterialDiffuseSpecularSpot::CreateContext(CBase3DObject *Obj) {
    CMaterialDiffuseSpecularSpotContext *con = new CMaterialDiffuseSpecularSpotContext;
    con->Object = Obj;
    return con;
}

void CMaterialDiffuseSpecularSpot::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CMaterialDiffuseSpecularSpot::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CMaterialDiffuseSpecularSpotContext *)Context;
    return true;
}

bool CMaterialDiffuseSpecularSpot::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);     //Disable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);     //Z only if equal
        
        //Lightmap
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        //Spotlight
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        this->OwnerEngine->GetTextureManager()->SetTexture(2,LightMapTexture);
        this->OwnerEngine->GetTextureManager()->SetTexture(3,SpotLightTexture);

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

bool CMaterialDiffuseSpecularSpot::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
    if (PassNumber != CurrentPass) {
        if (PassNumber == 0) {                  //First pass
            if (CurrentPass == 0xFFFFFFFF) {    //Newly set material
            } else {
                RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending
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

bool CMaterialDiffuseSpecularSpot::UnSet(CRenderSettings &RenderSettings) {
    if (GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);        //Enable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);   //Default Z function
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending
        SetIsSet(false);
    }
	//Lightmap
    RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    //Spotlight
    RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    return true;
}

void CMaterialDiffuseSpecularSpot::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CMaterialDiffuseSpecularSpot::DeleteShaders() {
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

void CMaterialDiffuseSpecularSpot::DeleteTextures() {
    if (DefaultTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultTexture->GetResourceID());
        DefaultTexture = NULL;
    }
    if (SpotLightTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(SpotLightTexture->GetResourceID());
        SpotLightTexture = NULL;
    }
    if (DefaultNormalMap!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultNormalMap->GetResourceID());
        DefaultNormalMap = NULL;
    }
    if (LightMapTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(LightMapTexture->GetResourceID());
        LightMapTexture = NULL;
    }

    for (int i=0; i<ShininessTextures->GetSize(); i++) {
        CBaseTexture *Tex = (CBaseTexture *)ShininessTextures->GetAt(i);
        OwnerEngine->GetTextureManager()->Delete(Tex->GetResourceID());
    }
    ShininessTextures->RemoveAll();
}

bool CMaterialDiffuseSpecularSpot::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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
    CMString VSFile = "CMaterialDiffuseSpecularSpot.vsh";
    CreatedVertexShader = LoadVS_Src(VSFile, RenderSettings);
    CMString PSFile = "CMaterialDiffuseSpecularSpot.psh";
    CreatedPixelShader = LoadPS_Src(PSFile, RenderSettings);

    DeleteTextures();

    //Load specular lightmap texture
    LightMapTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_ShininessLightMap);
    if (LightMapTexture==NULL) {
        CMString LMFile = "D3F_LightMap.dds";
        CTexture2D *TempPointer = new CTexture2D(LMFile.c_str(),OwnerEngine);
        TempPointer->SetResourceID(D3_GRI_Tex_ShininessLightMap);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);

        CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();

        TempPointer->SetLoadParams( engDef->Txt2D_Width, engDef->Txt2D_Height, 1, engDef->Txt2D_Usage,
                                    engDef->Txt2D_Format, engDef->Txt2D_Pool, engDef->Txt2D_Filter, 
                                    engDef->Txt2D_MipFilter, engDef->Txt2D_ColorKey);

        TempPointer->Load(resindex,RenderSettings);
        LightMapTexture = TempPointer;
        OwnerEngine->GetTextureManager()->Add(LightMapTexture->GetResourceID(),*LightMapTexture);
    }

    //Load shininess textures
    for (int i=0; i<=100; i++) {
        CBaseTexture *Tex = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(GetShininessID(i));
        if (Tex==NULL) {
            CMString LMFile;
            LMFile.Format("D3F_LightMap_%i.dds",i);
            CTexture2D *TempPointer = new CTexture2D(LMFile.c_str(),OwnerEngine);
            TempPointer->SetResourceID(GetShininessID(i));
            int resindex = OwnerEngine->GetFileManager()->Find(LMFile);

            CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();

            TempPointer->SetLoadParams( engDef->Txt2D_Width, engDef->Txt2D_Height, 1, engDef->Txt2D_Usage,
                                        engDef->Txt2D_Format, engDef->Txt2D_Pool, engDef->Txt2D_Filter, 
                                        engDef->Txt2D_MipFilter, engDef->Txt2D_ColorKey);

            TempPointer->Load(resindex,RenderSettings);
            Tex = TempPointer;
            OwnerEngine->GetTextureManager()->Add(Tex->GetResourceID(),*Tex);
            ShininessTextures->Add(Tex);
        }
    }

    //Load default projector texture
    SpotLightTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_DefaultSpotLight);
    if (SpotLightTexture==NULL) {
        CMString LMFile = "D3F_DefaultSpotlight2.dds";
        SpotLightTexture = new CTexture2D(LMFile.c_str(),OwnerEngine);
        SpotLightTexture->SetResourceID(D3_GRI_Tex_DefaultSpotLight);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        SpotLightTexture->Load(resindex,RenderSettings);
        OwnerEngine->GetTextureManager()->Add(SpotLightTexture->GetResourceID(),*SpotLightTexture);
    }

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

bool CMaterialDiffuseSpecularSpot::Update(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        CMaterialDiffuseSpecularSpotContext *Cntx = (CMaterialDiffuseSpecularSpotContext *)CurrentContext;
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

        // light 1 position
        CD3SLight *Light = (CD3SLight *)Cntx->Object->Lights->GetAt(CurrentPass);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(8, Light->WorldObjectPos,  1 );

        // Eye position
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(9, RenderSettings.CameraSource,  1 );

        // setup texgen, y is inverted
        D3DXMATRIX TexGenMatrix(0.5f, 0.0f, 0.0f, 0.5f,
                                0.0f, -0.5f, 0.0f, 0.5f,
                                0.0f, 0.0f, 0.0, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f);

        
        D3DXMATRIX ObjectToLight;
        D3DXMatrixMultiplyTranspose(&ObjectToLight,&CurrentContext->Object->WorldTransform, &Light->SpotViewProjection);
        D3DXMatrixMultiply(&ObjectToLight, &TexGenMatrix, &ObjectToLight);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(10, ObjectToLight, 4);

        //shininess
/*
        Cntx->SpecularShininess = Cntx->SpecularShininess + Cntx->ShininessAnimation;
        if (Cntx->SpecularShininess > 1) {
            Cntx->ShininessAnimation = -0.01f;
            Cntx->SpecularShininess = 1;
        }
        if (Cntx->SpecularShininess < 0) {
            Cntx->ShininessAnimation = 0.01f;
            Cntx->SpecularShininess = 0;
        }
*/

        //Diffuse multiplicator
        D3DXCOLOR DiffMul = Light->LightColor * (float)Light->Intensity * (float)Cntx->DiffuseMultiplier;
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(0, DiffMul, 1);

        //Diffuse color constant
        D3DXCOLOR DiffConst = Cntx->DiffuseColor * (1.0f - Cntx->DiffuseMultiplier) * (float)Light->Intensity;
        D3DXColorModulate(&DiffConst, &Light->LightColor, &DiffConst);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(1, DiffConst, 1);

        //Specular color constant
        D3DXCOLOR SpecConst = Cntx->SpecularColor * (float)Light->Intensity;
        D3DXColorModulate(&SpecConst, &Light->LightColor, &SpecConst);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(2, SpecConst, 1);

        D3DXCOLOR SpecShininess(Cntx->SpecularShininess, Cntx->SpecularShininess, Cntx->SpecularShininess, 1.0f);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(3, SpecShininess, 1);

        DWORD tmpShi = (DWORD)((float)(Cntx->SpecularShininess * 100.0f));
        CBaseTexture *ShiTex = (CBaseTexture *)this->OwnerEngine->GetTextureManager()->Find(GetShininessID(tmpShi));
        if (ShiTex) {
            this->OwnerEngine->GetTextureManager()->SetTexture(2,ShiTex);
        }
    }
    return true;
}

//***********************************************
//Material context
//***********************************************
CMaterialDiffuseSpecularSpotContext::CMaterialDiffuseSpecularSpotContext() : CDX8MaterialContext() {
    DiffuseTexture = NULL;
    NormalTexture = NULL;
}

CMaterialDiffuseSpecularSpotContext::~CMaterialDiffuseSpecularSpotContext() {
    if (DiffuseTexture!=NULL) {
        DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
    }
    if (NormalTexture!=NULL) {
        NormalTexture->OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
    }
}

int CMaterialDiffuseSpecularSpotContext::QueryProperty(const char *PropertyName) {
    if (strcmp(PropertyName, "DiffuseMultiplier") == 0) return PID_DiffuseMultiplier;
    if (strcmp(PropertyName, "DiffuseColor") == 0) return PID_DiffuseColor;
    if (strcmp(PropertyName, "DiffuseTexture") == 0) return PID_DiffuseTexture;
    if (strcmp(PropertyName, "NormalTexture") == 0) return PID_NormalTexture;
    if (strcmp(PropertyName, "SpecularColor") == 0) return PID_SpecularColor;
    if (strcmp(PropertyName, "SpecularShininess") == 0) return PID_SpecularShininess;
    return CDX8MaterialContext::QueryProperty(PropertyName);
}

bool CMaterialDiffuseSpecularSpotContext::SetProperty(int PropertyID, const void *Property) {
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
        case PID_SpecularShininess : {
            SpecularShininess = *(float *)Property;
            return true;
        } break;
        default : {
            return CDX8MaterialContext::SetProperty(PropertyID, Property);
        }
    }
}

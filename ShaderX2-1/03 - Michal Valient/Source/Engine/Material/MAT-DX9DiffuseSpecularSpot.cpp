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

#include "material\MAT-DX9DiffuseSpecularSpot.h"

#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "eng-enginedefaults.h"

#include "ENG-Texture2D.h"
#include "ENG-TextureManager.h"
#include "ENG-BaseTexture.h"

#include "ENG-Base3DObject.h"
#include "d3scene\eng-d3slight.h"

#include "MGR-DirectGraphicsManager.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "WND-WindowManager.h"
#include "FIL-FileManager.h"

#include "SC-MString.h"
#include "sc-dynamicarray.h"
#include "sc-xmlparser.h"

#include "Shader\SHD-ShaderManager.h"
#include "shader\shd-vertexshader.h"
#include "shader\shd-pixelshader.h"


CDX9_MaterialDiffuseSpecularSpot::CDX9_MaterialDiffuseSpecularSpot(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    CurrentPass = 0xFFFFFFFF;
    VSDeclaration = NULL;
    VertexShader = NULL;
    PixelShader = NULL;
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
    DefaultSpotLight = NULL;
    ShaderVersion = 0;

    ShininessTextures = new CRootClassArray(false);
}

CDX9_MaterialDiffuseSpecularSpot::~CDX9_MaterialDiffuseSpecularSpot(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    ReleaseBuffers();
    delete ShininessTextures;
}

void CDX9_MaterialDiffuseSpecularSpot::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheIn(RenderSettings);
    if (DefaultSpotLight!=NULL) DefaultSpotLight->CacheIn(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheIn(RenderSettings);

    for (int i=0; i<ShininessTextures->GetSize(); i++) {
        CBaseTexture *Tex = (CBaseTexture *)ShininessTextures->GetAt(i);
        Tex->CacheIn(RenderSettings);
    }
}

void CDX9_MaterialDiffuseSpecularSpot::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheOut(RenderSettings);
    if (DefaultSpotLight!=NULL) DefaultSpotLight->CacheOut(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheOut(RenderSettings);

    for (int i=0; i<ShininessTextures->GetSize(); i++) {
        CBaseTexture *Tex = (CBaseTexture *)ShininessTextures->GetAt(i);
        Tex->CacheOut(RenderSettings);
    }
}

DWORD CDX9_MaterialDiffuseSpecularSpot::GetPassCount() {
    if (CurrentContext!=NULL) {
        CDX9_MaterialDiffuseSpecularSpotContext *Cntx = (CDX9_MaterialDiffuseSpecularSpotContext *)CurrentContext;
        if (ShaderVersion == 3) {
            float f = (float)Cntx->Object->Lights->GetSize() / 4.0f;
            DWORD retF = (DWORD)ceilf(f);
            return retF;
        } else {
            return Cntx->Object->Lights->GetSize();
            //return (Cntx->Object->Lights->GetSize()>0)?1:0;
        }
    } else {
        return 0;
    }
}

void CDX9_MaterialDiffuseSpecularSpot::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CDX9_MaterialDiffuseSpecularSpot::CreateContext(CBase3DObject *Obj) {
    CDX9_MaterialDiffuseSpecularSpotContext *con = new CDX9_MaterialDiffuseSpecularSpotContext;
    con->Object = Obj;
    return con;
}

void CDX9_MaterialDiffuseSpecularSpot::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CDX9_MaterialDiffuseSpecularSpot::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CDX9_MaterialDiffuseSpecularSpotContext *)Context;
    return true;
}

bool CDX9_MaterialDiffuseSpecularSpot::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);     //Disable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);     //Z only if equal
        
        //Lightmap
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        //Spotlight
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        if (VertexShader && PixelShader) {
            RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);
            OwnerEngine->GetShaderManager()->SetVertexShader(VertexShader, RenderSettings);
            OwnerEngine->GetShaderManager()->SetPixelShader(PixelShader, RenderSettings);
        }
        CurrentPass = 0xFFFFFFFF;
        SetIsSet(true);
    }
    return true;
}

bool CDX9_MaterialDiffuseSpecularSpot::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
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

bool CDX9_MaterialDiffuseSpecularSpot::UnSet(CRenderSettings &RenderSettings) {
    if (GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);        //Enable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);   //Default Z function
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending

	    //Lightmap
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
        //Spotlight
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

        SetIsSet(false);
    }
    return true;
}

void CDX9_MaterialDiffuseSpecularSpot::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CDX9_MaterialDiffuseSpecularSpot::ReleaseBuffers() {
    if (VSDeclaration) {
        VSDeclaration->Release();
        VSDeclaration = NULL;
    }
    if (VertexShader) {
        OwnerEngine->GetShaderManager()->Delete(VertexShader->GetResourceID());
        VertexShader = NULL;
    }
    if (PixelShader) {
        OwnerEngine->GetShaderManager()->Delete(PixelShader->GetResourceID());
        PixelShader = NULL;
    }
    if (DefaultTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultTexture->GetResourceID());
        DefaultTexture = NULL;
    }
    if (DefaultSpotLight!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultSpotLight->GetResourceID());
        DefaultSpotLight = NULL;
    }
    if (DefaultNormalMap!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultNormalMap->GetResourceID());
        DefaultNormalMap = NULL;
    }

    for (int i=0; i<ShininessTextures->GetSize(); i++) {
        CBaseTexture *Tex = (CBaseTexture *)ShininessTextures->GetAt(i);
        OwnerEngine->GetTextureManager()->Delete(Tex->GetResourceID());
    }
    ShininessTextures->RemoveAll();
}

bool CDX9_MaterialDiffuseSpecularSpot::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading material");
    ReleaseBuffers();

    //shader input declaration
    D3DVERTEXELEMENT9  dwDecl[] = {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        D3DDECL_END()
    };
    RenderSettings.Direct3DDevice->CreateVertexDeclaration(dwDecl, &VSDeclaration);

    if (ShaderVersion == 0) {
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  >= D3DPS_VERSION(3,0)) ShaderVersion = 3;
        else if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  >= D3DPS_VERSION(2,0)) ShaderVersion = 2;
        else ShaderVersion = 1;
    }

    if (ShaderVersion == 3) {
        _LOG(this,D3_DV_GRP2_LEV3,"Loading PS_3_0 version");
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(3,0)) {
            _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 3.0 not supported");
        }
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DPS_VERSION(3,0)) {
            _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 3.0 not supported");
        }
        //Load vertex shader
        //CMString ShaderFile = "CDX9_MaterialDiffuseSpecularSpot.3_0.vso";
        CMString ShaderFile = "CDX9_MaterialDiffuseSpecularSpot.3_0.vsh";
        int ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        VertexShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (VertexShader==NULL) {
            VertexShader = new CVertexShader(ShaderFile.c_str(),OwnerEngine);
            VertexShader->SetResourceID(ShaderIndex);
            //((CVertexShader*)VertexShader)->SetLoadBinaryFirst(true);
            VertexShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*VertexShader);
        }
        
        //Load pixel shader
        //ShaderFile = "CDX9_MaterialDiffuseSpecularSpot.3_0.pso";
        ShaderFile = "CDX9_MaterialDiffuseSpecularSpot.3_0.psh";
        ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        PixelShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (PixelShader==NULL) {
            PixelShader = new CPixelShader(ShaderFile.c_str(),OwnerEngine);
            PixelShader->SetResourceID(ShaderIndex);
            //((CPixelShader*)PixelShader)->SetLoadBinaryFirst(true);
            PixelShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*PixelShader);
        }
        ShaderVersion = 3;
    } else if (ShaderVersion == 2) {
        _LOG(this,D3_DV_GRP2_LEV3,"Loading PS_2_0 version");
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(2,0)) {
            _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 2.0 not supported");
        }
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DPS_VERSION(2,0)) {
            _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 2.0 not supported");
        }
        //Load vertex shader
        CMString ShaderFile = "CDX9_MaterialDiffuseSpecularSpot.2_0.vsh";
        //CMString ShaderFile = "CDX9_MaterialDiffuseSpecularSpot_HLSL.vsh";
        int ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        VertexShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (VertexShader==NULL) {
            VertexShader = new CVertexShader(ShaderFile.c_str(),OwnerEngine);
            VertexShader->SetResourceID(ShaderIndex);
            VertexShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*VertexShader);
        }
        
        //Load pixel shader
        ShaderFile = "CDX9_MaterialDiffuseSpecularSpot.2_0.psh";
        //ShaderFile = "CDX9_MaterialDiffuseSpecularSpot_HLSL.psh";
        ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        PixelShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (PixelShader==NULL) {
            PixelShader = new CPixelShader(ShaderFile.c_str(),OwnerEngine);
            PixelShader->SetResourceID(ShaderIndex);
            PixelShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*PixelShader);
        }
        ShaderVersion = 2;
    } else {
        _LOG(this,D3_DV_GRP2_LEV3,"Loading PS_1_4 version");
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(1,1)) {
            _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 1.1 not supported");
        }
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DPS_VERSION(1,4)) {
            _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 1.4 not supported");
        }
        //Load vertex shader
        CMString ShaderFile = "CDX9_MaterialDiffuseSpecularSpot.1_4.vsh";
        int ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        VertexShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (VertexShader==NULL) {
            VertexShader = new CVertexShader(ShaderFile.c_str(),OwnerEngine);
            VertexShader->SetResourceID(ShaderIndex);
            VertexShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*VertexShader);
        }
        
        //Load pixel shader
        ShaderFile = "CDX9_MaterialDiffuseSpecularSpot.1_4.psh";
        ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        PixelShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (PixelShader==NULL) {
            PixelShader = new CPixelShader(ShaderFile.c_str(),OwnerEngine);
            PixelShader->SetResourceID(ShaderIndex);
            PixelShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*PixelShader);
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

        ShaderVersion = 1;
    }

    //Load default projector texture
    DefaultSpotLight = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_DefaultSpotLight);
    if (DefaultSpotLight==NULL) {
        //CMString LMFile = "D3F_DefaultSpotlight.bmp";
        CMString LMFile = "D3F_DefaultSpotlight2.dds";
        DefaultSpotLight = new CTexture2D(LMFile.c_str(),OwnerEngine);
        DefaultSpotLight->SetResourceID(D3_GRI_Tex_DefaultSpotLight);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        DefaultSpotLight->Load(resindex,RenderSettings);
        OwnerEngine->GetTextureManager()->Add(DefaultSpotLight->GetResourceID(),*DefaultSpotLight);
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

bool CDX9_MaterialDiffuseSpecularSpot::Update(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        CDX9_MaterialDiffuseSpecularSpotContext *Cntx = (CDX9_MaterialDiffuseSpecularSpotContext *)CurrentContext;
        //SetTexture
        if (Cntx->DiffuseTexture!=NULL) {
            this->OwnerEngine->GetTextureManager()->SetTexture(0,Cntx->DiffuseTexture);
        } else {
            this->OwnerEngine->GetTextureManager()->SetTexture(0,DefaultTexture);
        }
        if (Cntx->NormalTexture!=NULL) {
            this->OwnerEngine->GetTextureManager()->SetTexture(1,Cntx->NormalTexture);
        } else {
            this->OwnerEngine->GetTextureManager()->SetTexture(1,DefaultNormalMap);
        }

        if (ShaderVersion == 3) {
            //Due to fact, that multiple spotlights will be in one pass, only one projector texture
            //can be set
            this->OwnerEngine->GetTextureManager()->SetTexture(3,DefaultSpotLight);

            //VS_3_0 constants
            D3DXMATRIX tmp;
            // world * view * proj matrix
            D3DXMatrixMultiplyTranspose(&tmp, &Cntx->Object->WorldTransform, &RenderSettings.CameraTransformMatrix);
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(4, tmp, 4);	

            // world matrix
            D3DXMatrixTranspose(&tmp,&Cntx->Object->WorldTransform);
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(0, tmp, 4);

            // Eye position
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(8, RenderSettings.CameraSource,  1 );
            
            //PS_3_0 constants
            //Diffuse color constant
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(0, Cntx->DiffuseColor, 1);

            //Specular color constant
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(1, Cntx->SpecularColor, 1);

            //Specular shininess
            D3DXCOLOR SpecShi(Cntx->SpecularShininess * 100.0f, Cntx->SpecularShininess * 100.0f, Cntx->SpecularShininess * 100.0f, 1.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(2, SpecShi, 1);

            int LightCount = 0;
            int VSStartIndex = 9;
            int VSLoopItemSize = 6;
            int PSStartIndex = 4;
            int PSLoopItemSize = 1;
            DWORD UpperLightCount = min((CurrentPass+1) * 5, (DWORD)Cntx->Object->Lights->GetSize());
            for (DWORD i=(DWORD)CurrentPass * 5; i<UpperLightCount; i++) {
                CD3SLight *Light = (CD3SLight *)Cntx->Object->Lights->GetAt(i);

                // light position
                RenderSettings.Direct3DDevice->SetVertexShaderConstantF(VSStartIndex + 0, Light->WorldObjectPos,  1 );

                // setup texgen, y is inverted
                D3DXMATRIX TexGenMatrix(0.5f, 0.0f, 0.0f, 0.5f,
                                        0.0f,-0.5f, 0.0f, 0.5f,
                                        0.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f);

                D3DXMATRIX ObjectToLight;
                D3DXMatrixMultiplyTranspose(&ObjectToLight,&CurrentContext->Object->WorldTransform, &Light->SpotViewProjection);
                D3DXMatrixMultiply(&ObjectToLight, &TexGenMatrix, &ObjectToLight);
                RenderSettings.Direct3DDevice->SetVertexShaderConstantF(VSStartIndex + 1, ObjectToLight, 4);
                
                //Light color
                D3DXCOLOR LightColor (Light->LightColor.r, Light->LightColor.g, Light->LightColor.b, Light->Intensity);
                RenderSettings.Direct3DDevice->SetPixelShaderConstantF(PSStartIndex + 0, LightColor, 1);

                LightCount++;
                VSStartIndex += VSLoopItemSize;
                PSStartIndex += PSLoopItemSize;
            }
            int LoopConst[4];
            //LoopConst[0] = 1;
            LoopConst[0] = (int)LightCount;
            LoopConst[1] = 0;           //starting from o0
            LoopConst[2] = 2;           //coord + texture = 2 slots
            RenderSettings.Direct3DDevice->SetVertexShaderConstantI(0, LoopConst, 1);
            LoopConst[0] = (int)LightCount;
            LoopConst[1] = 0;           //starting from v0
            LoopConst[2] = 2;           //coord + texture = 2 slots
            RenderSettings.Direct3DDevice->SetPixelShaderConstantI(0, LoopConst, 1);
        } else {
            D3DXMATRIX tmp;
            // world * view * proj matrix
            D3DXMatrixMultiplyTranspose(&tmp, &Cntx->Object->WorldTransform, &RenderSettings.CameraTransformMatrix);
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(4, tmp, 4);	

            // world matrix
            D3DXMatrixTranspose(&tmp,&Cntx->Object->WorldTransform);
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(0, tmp, 4);

            CD3SLight *Light = (CD3SLight *)Cntx->Object->Lights->GetAt(CurrentPass);

            if (Light->SpotlightTexture!=NULL) this->OwnerEngine->GetTextureManager()->SetTexture(3,Light->SpotlightTexture);
            else this->OwnerEngine->GetTextureManager()->SetTexture(3,DefaultSpotLight);

            // light position
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(8, Light->WorldObjectPos,  1 );

            // Eye position
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(9, RenderSettings.CameraSource,  1 );

            // setup texgen, y is inverted
            D3DXMATRIX TexGenMatrix(0.5f, 0.0f, 0.0f, 0.5f,
                                    0.0f,-0.5f, 0.0f, 0.5f,
                                    0.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f);

            D3DXMATRIX ObjectToLight;
            D3DXMatrixMultiplyTranspose(&ObjectToLight,&CurrentContext->Object->WorldTransform, &Light->SpotViewProjection);
            D3DXMatrixMultiply(&ObjectToLight, &TexGenMatrix, &ObjectToLight);
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(10, ObjectToLight, 4);

            //Diffuse color constant
            D3DXCOLOR LightColor = Light->LightColor * (float)Light->Intensity;
            D3DXCOLOR DiffMul = *D3DXColorModulate(&DiffMul, &LightColor, &Cntx->DiffuseColor);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(0, DiffMul, 1);

            //Specular color constant
            D3DXCOLOR SpecConst = *D3DXColorModulate(&SpecConst, &LightColor, &Cntx->SpecularColor);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(1, SpecConst, 1);

            //Specular shininess
            D3DXCOLOR SpecShi(Cntx->SpecularShininess * 100.0f, Cntx->SpecularShininess * 100.0f, Cntx->SpecularShininess * 100.0f, 1.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(2, SpecShi, 1);

            if (ShaderVersion == 1) {
                DWORD tmpShi = (DWORD)((float)(Cntx->SpecularShininess * 100.0f));
                CBaseTexture *ShiTex = (CBaseTexture *)this->OwnerEngine->GetTextureManager()->Find(GetShininessID(tmpShi));
                if (ShiTex) {
                    this->OwnerEngine->GetTextureManager()->SetTexture(2,ShiTex);
                }
            }
        }
    }
    return true;
}

//***********************************************
//Material context
//***********************************************
CDX9_MaterialDiffuseSpecularSpotContext::CDX9_MaterialDiffuseSpecularSpotContext() : CDX9MaterialContext() {
    DiffuseTexture = NULL;
    NormalTexture = NULL;
}

CDX9_MaterialDiffuseSpecularSpotContext::~CDX9_MaterialDiffuseSpecularSpotContext() {
    if (DiffuseTexture!=NULL) {
        DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
    }
    if (NormalTexture!=NULL) {
        NormalTexture->OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
    }
}

void CDX9_MaterialDiffuseSpecularSpotContext::FillFromXML(CXMLNode *Node, CRenderSettings &Settings) {
    SpecularColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    CXMLNode *tmpNode = Node->GetSubNode(Node->FindSubNode("SPECULARCOLOR", 0));
    if (tmpNode!=NULL) {
        tmpNode->GetAttributeValueF("X", SpecularColor.r);
        tmpNode->GetAttributeValueF("Y", SpecularColor.g);
        tmpNode->GetAttributeValueF("Z", SpecularColor.b);
    }

    SpecularShininess = 0.3f;
    Node->GetAttributeValueF("SHININESS", SpecularShininess);

    int TexID = -1;
    Node->GetAttributeValueI("BUMPMAPID", TexID);
    NormalTexture = (CBaseTexture *)Settings.Engine->GetTextureManager()->Query(TexID);

    DiffuseColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    tmpNode = Node->GetSubNode(Node->FindSubNode("DIFFUSECOLOR", 0));
    if (tmpNode!=NULL) {
        tmpNode->GetAttributeValueF("X", DiffuseColor.r);
        tmpNode->GetAttributeValueF("Y", DiffuseColor.g);
        tmpNode->GetAttributeValueF("Z", DiffuseColor.b);
    }

    TexID = -1;
    Node->GetAttributeValueI("DIFFUSEMAPID", TexID);
    DiffuseTexture = (CBaseTexture *)Settings.Engine->GetTextureManager()->Query(TexID);
}

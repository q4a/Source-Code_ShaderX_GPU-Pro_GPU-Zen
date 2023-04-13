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

#include "material\MAT-DX9ShadowPhong.h"

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


CDX9_MaterialShadowPhong::CDX9_MaterialShadowPhong(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    CurrentPass = 0xFFFFFFFF;
    VSDeclaration = NULL;
    VertexShader = NULL;
    PixelShader = NULL;
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
    DefaultSpotLight = NULL;
    ShininessTextures = new CRootClassArray(false);
}

CDX9_MaterialShadowPhong::~CDX9_MaterialShadowPhong(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    ReleaseBuffers();
    delete ShininessTextures;
}

void CDX9_MaterialShadowPhong::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheIn(RenderSettings);
    if (DefaultSpotLight!=NULL) DefaultSpotLight->CacheIn(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheIn(RenderSettings);

    for (int i=0; i<ShininessTextures->GetSize(); i++) {
        CBaseTexture *Tex = (CBaseTexture *)ShininessTextures->GetAt(i);
        Tex->CacheIn(RenderSettings);
    }
}

void CDX9_MaterialShadowPhong::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheOut(RenderSettings);
    if (DefaultSpotLight!=NULL) DefaultSpotLight->CacheOut(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheOut(RenderSettings);

    for (int i=0; i<ShininessTextures->GetSize(); i++) {
        CBaseTexture *Tex = (CBaseTexture *)ShininessTextures->GetAt(i);
        Tex->CacheOut(RenderSettings);
    }
}

DWORD CDX9_MaterialShadowPhong::GetPassCount() {
    if (CurrentContext!=NULL) {
        CDX9_MaterialShadowPhongContext *Cntx = (CDX9_MaterialShadowPhongContext *)CurrentContext;
        return Cntx->Object->Lights->GetSize();
        //return (Cntx->Object->Lights->GetSize()>0)?1:0;
    } else {
        return 0;
    }
}

void CDX9_MaterialShadowPhong::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CDX9_MaterialShadowPhong::CreateContext(CBase3DObject *Obj) {
    CDX9_MaterialShadowPhongContext *con = new CDX9_MaterialShadowPhongContext;
    con->Object = Obj;
    return con;
}

void CDX9_MaterialShadowPhong::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CDX9_MaterialShadowPhong::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CDX9_MaterialShadowPhongContext *)Context;
    return true;
}

bool CDX9_MaterialShadowPhong::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);     //Disable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);     //Z only if equal
        
        //Shadow buffer
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

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

bool CDX9_MaterialShadowPhong::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
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

bool CDX9_MaterialShadowPhong::UnSet(CRenderSettings &RenderSettings) {
    if (GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);        //Enable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);   //Default Z function
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending

        //Shadow buffer
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        //Spotlight
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

        SetIsSet(false);
    }
    return true;
}

void CDX9_MaterialShadowPhong::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CDX9_MaterialShadowPhong::ReleaseBuffers() {
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
}

bool CDX9_MaterialShadowPhong::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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

    _LOG(this,D3_DV_GRP2_LEV3,"Loading PS_2_0 version");
    if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(2,0)) {
        _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 2.0 not supported");
    }
    if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DPS_VERSION(2,0)) {
        _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 2.0 not supported");
    }
    //Load vertex shader
    CMString ShaderFile = "CDX9_MaterialShadowPhong.2_0.vsh";
    int ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
    VertexShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
    if (VertexShader==NULL) {
        VertexShader = new CVertexShader(ShaderFile.c_str(),OwnerEngine);
        VertexShader->SetResourceID(ShaderIndex);
        VertexShader->Load(ShaderIndex,RenderSettings);
        OwnerEngine->GetShaderManager()->Add(ShaderIndex,*VertexShader);
    }
    
    //Load pixel shader
    ShaderFile = "CDX9_MaterialShadowPhong.2_0.psh";
    ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
    PixelShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
    if (PixelShader==NULL) {
        PixelShader = new CPixelShader(ShaderFile.c_str(),OwnerEngine);
        PixelShader->SetResourceID(ShaderIndex);
        PixelShader->Load(ShaderIndex,RenderSettings);
        OwnerEngine->GetShaderManager()->Add(ShaderIndex,*PixelShader);
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

bool CDX9_MaterialShadowPhong::Update(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        CDX9_MaterialShadowPhongContext *Cntx = (CDX9_MaterialShadowPhongContext *)CurrentContext;
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

        if (Light->ShadowTexture!=NULL) this->OwnerEngine->GetTextureManager()->SetTexture(2,Light->ShadowTexture);
        else this->OwnerEngine->GetTextureManager()->SetTexture(3,DefaultTexture);

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

        //Light distance ranges
        float rcpZ = 1.0f / (Light->AttenuationFarEnd - Light->AttenuationNearStart);
        D3DXCOLOR Ranges(Light->AttenuationNearStart, Light->AttenuationFarEnd, rcpZ, 0.5f);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(14,Ranges,1);

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

        //Center of the texel for shadow buffer
        if (Light->ShadowTexture!=NULL) {
            CTexture2D *tmpTex = (CTexture2D *)Light->ShadowTexture;

            float shTexelX = 1.0f / ((float)tmpTex->Width * 2.0f);
            float shTexelY = 1.0f / ((float)tmpTex->Height * 2.0f);
            D3DXCOLOR shTexelAdjust(shTexelX, shTexelY, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(3, shTexelAdjust, 1);

            shTexelX = 1.0f / ((float)tmpTex->Width*1.0f);
            shTexelY = 1.0f / ((float)tmpTex->Height*1.0f);
            //Near texel lookups
            //TopLeft
            D3DXCOLOR shTexelTopLeft(-shTexelX, -shTexelY, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(4, shTexelTopLeft, 1);
            //Left
            D3DXCOLOR shTexelLeft(-shTexelX, 0.0f, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(5, shTexelLeft, 1);
            //BottomLeft
            D3DXCOLOR shTexelBottomLeft(-shTexelX, shTexelY, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(6, shTexelBottomLeft, 1);

            //Top
            D3DXCOLOR shTexelTop(0.0f, -shTexelY, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(7, shTexelTop, 1);
            //Bottom
            D3DXCOLOR shTexelBottom(0.0f, shTexelY, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(8, shTexelBottom, 1);

            //TopRight
            D3DXCOLOR shTexelTopRight(shTexelX, -shTexelY, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(9, shTexelTopRight, 1);
            //Right
            D3DXCOLOR shTexelRight(shTexelX, 0.0f, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(10, shTexelRight, 1);
            //BottomRight
            D3DXCOLOR shTexelBottomRight(shTexelX, shTexelY, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(11, shTexelBottomRight, 1);

            //Texture size
            D3DXCOLOR shTexSize((float)tmpTex->Width, (float)tmpTex->Height, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(29, shTexSize, 1);

            //Divider
            D3DXCOLOR shTexelDivider(1.0f/4.0f, 0.0f, 0.0f, 0.0f);
            RenderSettings.Direct3DDevice->SetPixelShaderConstantF(30, shTexelDivider, 1);
        }
    }
    return true;
}

//***********************************************
//Material context
//***********************************************
CDX9_MaterialShadowPhongContext::CDX9_MaterialShadowPhongContext() : CDX9MaterialContext() {
    DiffuseTexture = NULL;
    NormalTexture = NULL;
}

CDX9_MaterialShadowPhongContext::~CDX9_MaterialShadowPhongContext() {
    if (DiffuseTexture!=NULL) {
        DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
    }
    if (NormalTexture!=NULL) {
        NormalTexture->OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
    }
}

void CDX9_MaterialShadowPhongContext::FillFromXML(CXMLNode *Node, CRenderSettings &Settings) {
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

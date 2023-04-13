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

#include "material\MAT-DX9OrenNayar.h"

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


CDX9_MaterialOrenNayar::CDX9_MaterialOrenNayar(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    CurrentPass = 0xFFFFFFFF;
    VSDeclaration = NULL;
    VertexShader = NULL;
    PixelShader = NULL;
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
    DefaultSpotLight = NULL;
    SinTan_Lookup = NULL;
}

CDX9_MaterialOrenNayar::~CDX9_MaterialOrenNayar(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    ReleaseBuffers();
}

void CDX9_MaterialOrenNayar::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheIn(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheIn(RenderSettings);
    if (DefaultSpotLight!=NULL) DefaultSpotLight->CacheIn(RenderSettings);
    if (SinTan_Lookup!=NULL) SinTan_Lookup->CacheIn(RenderSettings);
}

void CDX9_MaterialOrenNayar::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheOut(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheOut(RenderSettings);
    if (DefaultSpotLight!=NULL) DefaultSpotLight->CacheOut(RenderSettings);
    if (SinTan_Lookup!=NULL) SinTan_Lookup->CacheOut(RenderSettings);
}

DWORD CDX9_MaterialOrenNayar::GetPassCount() {
    if (CurrentContext!=NULL) {
        CDX9_MaterialOrenNayarContext *Cntx = (CDX9_MaterialOrenNayarContext *)CurrentContext;
        return Cntx->Object->Lights->GetSize();
//        if (Cntx->Object->Lights->GetSize() > 0) return 1;
//        else return 0;
    } else {
        return 0;
    }
}

void CDX9_MaterialOrenNayar::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CDX9_MaterialOrenNayar::CreateContext(CBase3DObject *Obj) {
    CDX9_MaterialOrenNayarContext *con = new CDX9_MaterialOrenNayarContext;
    con->Object = Obj;
    return con;
}

void CDX9_MaterialOrenNayar::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CDX9_MaterialOrenNayar::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CDX9_MaterialOrenNayarContext *)Context;
    return true;
}

bool CDX9_MaterialOrenNayar::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);     //Disable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);     //Z only if equal

        //SinTan lookup texture
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        OwnerEngine->GetTextureManager()->SetTexture(2, SinTan_Lookup);

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

bool CDX9_MaterialOrenNayar::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
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

bool CDX9_MaterialOrenNayar::UnSet(CRenderSettings &RenderSettings) {
    if (GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);        //Enable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);   //Default Z function
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending
        
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

        //Spotlight
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

        SetIsSet(false);
    }
    return true;
}

void CDX9_MaterialOrenNayar::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CDX9_MaterialOrenNayar::ReleaseBuffers() {
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
    if (DefaultNormalMap!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultNormalMap->GetResourceID());
        DefaultNormalMap = NULL;
    }
    if (DefaultSpotLight!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultSpotLight->GetResourceID());
        DefaultSpotLight = NULL;
    }
    if (SinTan_Lookup!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(SinTan_Lookup->GetResourceID());
        SinTan_Lookup = NULL;
    }
}

bool CDX9_MaterialOrenNayar::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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
    CMString ShaderFile = "CDX9_MaterialOrenNayar.vsh";
    //CMString ShaderFile = "CDX9_MaterialOrenNayar_HLSL.vsh";
    int ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
    VertexShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
    if (VertexShader==NULL) {
        VertexShader = new CVertexShader(ShaderFile.c_str(),OwnerEngine);
        VertexShader->SetResourceID(ShaderIndex);
        VertexShader->Load(ShaderIndex,RenderSettings);
        OwnerEngine->GetShaderManager()->Add(ShaderIndex,*VertexShader);
    }
    
    //Load pixel shader
    //ShaderFile = "CDX9_MaterialOrenNayar.psh";
    ShaderFile = "CDX9_MaterialOrenNayar_Optimal.psh";
    //ShaderFile = "CDX9_MaterialOrenNayar_HLSL.psh";
    ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
    PixelShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
    if (PixelShader==NULL) {
        PixelShader = new CPixelShader(ShaderFile.c_str(),OwnerEngine);
        PixelShader->SetResourceID(ShaderIndex);
        PixelShader->Load(ShaderIndex,RenderSettings);
        OwnerEngine->GetShaderManager()->Add(ShaderIndex,*PixelShader);
    }

    //Load Reciprocal texture
    CMString LMFile = "D3F_OrenNayar512.dds";
    int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
    SinTan_Lookup = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(resindex);
    if (SinTan_Lookup==NULL) {
        SinTan_Lookup = new CTexture2D(LMFile.c_str(),OwnerEngine);
        SinTan_Lookup->SetResourceID(resindex);

        CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();
        ((CTexture2D *)SinTan_Lookup)->SetLoadParams( engDef->Txt2D_Width, engDef->Txt2D_Height, 
                                                    1, engDef->Txt2D_Usage, engDef->Txt2D_Format, 
                                                    engDef->Txt2D_Pool, engDef->Txt2D_Filter, 
                                                    engDef->Txt2D_MipFilter, engDef->Txt2D_ColorKey);
        SinTan_Lookup->Load(resindex,RenderSettings);
        OwnerEngine->GetTextureManager()->Add(SinTan_Lookup->GetResourceID(),*SinTan_Lookup);
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

bool CDX9_MaterialOrenNayar::Update(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        CDX9_MaterialOrenNayarContext *Cntx = (CDX9_MaterialOrenNayarContext *)CurrentContext;
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
        //CD3SLight *Light = (CD3SLight *)Cntx->Object->Lights->GetAt(1);

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

        //Roughness
        //float roughness = ((float)RenderSettings.RenderID - 1.0f) * 0.01f; 
        float roughness = 0.6f;
        D3DXCOLOR Roughness(roughness, 1.0f, 1.0f, 1.0f);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(0, Roughness,  1 );

        //A and B components
        float roughness2 = roughness * roughness;
        float theA = 1.0f - 0.5f*roughness2 / (roughness2 + 0.33f);
        float theB = 0.45f*roughness2 / (roughness2 + 0.09f);
        D3DXCOLOR AandB(theA, theB, 0.0f, 0.0f);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(1, AandB,  1 );
    }
    return true;
}

//***********************************************
//Material context
//***********************************************
CDX9_MaterialOrenNayarContext::CDX9_MaterialOrenNayarContext() : CDX9MaterialContext() {
    DiffuseTexture = NULL;
    NormalTexture = NULL;
}

CDX9_MaterialOrenNayarContext::~CDX9_MaterialOrenNayarContext() {
    if (DiffuseTexture!=NULL) {
        DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
    }
    if (NormalTexture!=NULL) {
        NormalTexture->OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
    }
}

void CDX9_MaterialOrenNayarContext::FillFromXML(CXMLNode *Node, CRenderSettings &Settings) {
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

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

#include "material\MAT-DX9CookTorrance.h"

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


CDX9_MaterialCookTorrance::CDX9_MaterialCookTorrance(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    CurrentPass = 0xFFFFFFFF;
    VSDeclaration = NULL;
    VertexShader = NULL;
    PixelShader = NULL;
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
    DefaultSpotLight = NULL;
    Beckman = NULL;
    Reciprocal = NULL;
    Fresnel = NULL;
    ShaderVersion = 0;
}

CDX9_MaterialCookTorrance::~CDX9_MaterialCookTorrance(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    ReleaseBuffers();
}

void CDX9_MaterialCookTorrance::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheIn(RenderSettings);
    if (Beckman!=NULL) Beckman->CacheIn(RenderSettings);
    if (Reciprocal!=NULL) Reciprocal->CacheIn(RenderSettings);
    if (Fresnel!=NULL) Fresnel->CacheIn(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheIn(RenderSettings);
    if (DefaultSpotLight!=NULL) DefaultSpotLight->CacheIn(RenderSettings);
}

void CDX9_MaterialCookTorrance::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) DefaultTexture->CacheOut(RenderSettings);
    if (Beckman!=NULL) Beckman->CacheOut(RenderSettings);
    if (Reciprocal!=NULL) Reciprocal->CacheOut(RenderSettings);
    if (Fresnel!=NULL) Fresnel->CacheOut(RenderSettings);
    if (DefaultNormalMap!=NULL) DefaultNormalMap->CacheOut(RenderSettings);
    if (DefaultSpotLight!=NULL) DefaultSpotLight->CacheOut(RenderSettings);
}

DWORD CDX9_MaterialCookTorrance::GetPassCount() {
    if (CurrentContext!=NULL) {
        CDX9_MaterialCookTorranceContext *Cntx = (CDX9_MaterialCookTorranceContext *)CurrentContext;
        return Cntx->Object->Lights->GetSize();
        //return (Cntx->Object->Lights->GetSize()>0)?1:0;
    } else {
        return 0;
    }
}

void CDX9_MaterialCookTorrance::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CDX9_MaterialCookTorrance::CreateContext(CBase3DObject *Obj) {
    CDX9_MaterialCookTorranceContext *con = new CDX9_MaterialCookTorranceContext;
    con->Object = Obj;
    return con;
}

void CDX9_MaterialCookTorrance::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CDX9_MaterialCookTorrance::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CDX9_MaterialCookTorranceContext *)Context;
    return true;
}

bool CDX9_MaterialCookTorrance::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);     //Disable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);     //Z only if equal

        if (ShaderVersion == 1) {
            //Beckman distribution
            RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
            RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
            OwnerEngine->GetTextureManager()->SetTexture(2,Beckman);

            //Reciprocal
            RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
            RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
            OwnerEngine->GetTextureManager()->SetTexture(3,Reciprocal);

            //Fresnel
            RenderSettings.Direct3DDevice->SetSamplerState( 4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
            RenderSettings.Direct3DDevice->SetSamplerState( 4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
            OwnerEngine->GetTextureManager()->SetTexture(4,Fresnel);
        }

        //Spotlight
        RenderSettings.Direct3DDevice->SetSamplerState( 5, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        RenderSettings.Direct3DDevice->SetSamplerState( 5, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

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

bool CDX9_MaterialCookTorrance::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
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

bool CDX9_MaterialCookTorrance::UnSet(CRenderSettings &RenderSettings) {
    if (GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);        //Enable writing to Zbuffer
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);   //Default Z function
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);   //Disable blending

        if (ShaderVersion == 1) {
	        //Beckman
            RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
            RenderSettings.Direct3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
            //Reciprocal
            RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
            RenderSettings.Direct3DDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

            //Fresnel
            RenderSettings.Direct3DDevice->SetSamplerState( 4, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
            RenderSettings.Direct3DDevice->SetSamplerState( 4, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
        }

        //Spotlight
        RenderSettings.Direct3DDevice->SetSamplerState( 5, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        RenderSettings.Direct3DDevice->SetSamplerState( 5, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

        SetIsSet(false);
    }
    return true;
}

void CDX9_MaterialCookTorrance::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CDX9_MaterialCookTorrance::ReleaseBuffers() {
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
    if (Beckman!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(Beckman->GetResourceID());
        Beckman = NULL;
    }
    if (Reciprocal!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(Reciprocal->GetResourceID());
        Reciprocal = NULL;
    }
    if (Fresnel!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(Fresnel->GetResourceID());
        Fresnel = NULL;
    }
    if (DefaultNormalMap!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultNormalMap->GetResourceID());
        DefaultNormalMap = NULL;
    }
    if (DefaultSpotLight!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultSpotLight->GetResourceID());
        DefaultSpotLight = NULL;
    }
}

bool CDX9_MaterialCookTorrance::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  >= D3DPS_VERSION(2,0)) ShaderVersion = 2;
        else ShaderVersion = 1;
    }

    if (ShaderVersion == 2) {
        _LOG(this,D3_DV_GRP2_LEV3,"Loading PS_2_0 version");
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(2,0)) {
            _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 2.0 not supported");
        }
        if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DPS_VERSION(2,0)) {
            _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 2.0 not supported");
        }
        //Load vertex shader
        CMString ShaderFile = "CDX9_MaterialCookTorrance.vsh";
        //CMString ShaderFile = "CDX9_MaterialCookTorrance_HLSL.vsh";
        int ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        VertexShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (VertexShader==NULL) {
            VertexShader = new CVertexShader(ShaderFile.c_str(),OwnerEngine);
            VertexShader->SetResourceID(ShaderIndex);
            VertexShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*VertexShader);
        }
        
        //Load pixel shader
        ShaderFile = "CDX9_MaterialCookTorrance.psh";
        //ShaderFile = "CDX9_MaterialCookTorrance_HLSL.psh";
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
        CMString ShaderFile = "CDX9_MaterialCookTorrance.1_4.vsh";
        int ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        VertexShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (VertexShader==NULL) {
            VertexShader = new CVertexShader(ShaderFile.c_str(),OwnerEngine);
            VertexShader->SetResourceID(ShaderIndex);
            VertexShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*VertexShader);
        }
        
        //Load pixel shader
        ShaderFile = "CDX9_MaterialCookTorrance.1_4.psh";
        ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
        PixelShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
        if (PixelShader==NULL) {
            PixelShader = new CPixelShader(ShaderFile.c_str(),OwnerEngine);
            PixelShader->SetResourceID(ShaderIndex);
            PixelShader->Load(ShaderIndex,RenderSettings);
            OwnerEngine->GetShaderManager()->Add(ShaderIndex,*PixelShader);
        }

        //Load Beckman distribution texture
        CMString LMFile = "D3F_BeckMan512.dds";
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        Beckman = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(resindex);
        if (Beckman==NULL) {
            Beckman = new CTexture2D(LMFile.c_str(),OwnerEngine);
            Beckman->SetResourceID(resindex);

            CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();
            ((CTexture2D *)Beckman)->SetLoadParams( engDef->Txt2D_Width, engDef->Txt2D_Height, 
                                                    1, engDef->Txt2D_Usage, engDef->Txt2D_Format, 
                                                    engDef->Txt2D_Pool, engDef->Txt2D_Filter, 
                                                    engDef->Txt2D_MipFilter, engDef->Txt2D_ColorKey);

            Beckman->Load(resindex,RenderSettings);
            OwnerEngine->GetTextureManager()->Add(Beckman->GetResourceID(),*Beckman);
        }

        //Load Reciprocal texture
        LMFile = "D3F_Rcp512.dds";
        resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        Reciprocal = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(resindex);
        if (Reciprocal==NULL) {
            Reciprocal = new CTexture2D(LMFile.c_str(),OwnerEngine);
            Reciprocal->SetResourceID(resindex);

            CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();
            ((CTexture2D *)Reciprocal)->SetLoadParams( engDef->Txt2D_Width, engDef->Txt2D_Height, 
                                                       1, engDef->Txt2D_Usage, engDef->Txt2D_Format, 
                                                       engDef->Txt2D_Pool, engDef->Txt2D_Filter, 
                                                       engDef->Txt2D_MipFilter, engDef->Txt2D_ColorKey);
            Reciprocal->Load(resindex,RenderSettings);
            OwnerEngine->GetTextureManager()->Add(Reciprocal->GetResourceID(),*Reciprocal);
        }

        //Load Fresnel texture
        LMFile = "D3F_Fresnel.dds";
        resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        Fresnel = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(resindex);
        if (Fresnel==NULL) {
            Fresnel = new CTexture2D(LMFile.c_str(),OwnerEngine);
            Fresnel->SetResourceID(resindex);

            CEngineDefaults *engDef = OwnerEngine->GetEngineDefaults();
            ((CTexture2D *)Fresnel)->SetLoadParams( engDef->Txt2D_Width, engDef->Txt2D_Height, 
                                                    1, engDef->Txt2D_Usage, engDef->Txt2D_Format, 
                                                    engDef->Txt2D_Pool, engDef->Txt2D_Filter, 
                                                    engDef->Txt2D_MipFilter, engDef->Txt2D_ColorKey);
            Fresnel->Load(resindex,RenderSettings);
            OwnerEngine->GetTextureManager()->Add(Fresnel->GetResourceID(),*Fresnel);
        }
        ShaderVersion = 1;
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

    SetFilled(true);
    _LOGE(logE,"Loaded");
    return true;
}

bool CDX9_MaterialCookTorrance::Update(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        CDX9_MaterialCookTorranceContext *Cntx = (CDX9_MaterialCookTorranceContext *)CurrentContext;
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

        if (Light->SpotlightTexture!=NULL) this->OwnerEngine->GetTextureManager()->SetTexture(5,Light->SpotlightTexture);
        else this->OwnerEngine->GetTextureManager()->SetTexture(5,DefaultSpotLight);

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
        D3DXCOLOR Roughness(0.15f, 1.0f, 1.0f, 1.0f);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(2, Roughness,  1 );

        //Refraction index
        D3DXCOLOR Refraction(0.85f, 1.0f, 1.0f, 1.0f);
        RenderSettings.Direct3DDevice->SetPixelShaderConstantF(3, Refraction,  1 );
    }
    return true;
}

//***********************************************
//Material context
//***********************************************
CDX9_MaterialCookTorranceContext::CDX9_MaterialCookTorranceContext() : CDX9MaterialContext() {
    DiffuseTexture = NULL;
    NormalTexture = NULL;
}

CDX9_MaterialCookTorranceContext::~CDX9_MaterialCookTorranceContext() {
    if (DiffuseTexture!=NULL) {
        DiffuseTexture->OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
    }
    if (NormalTexture!=NULL) {
        NormalTexture->OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
    }
}

void CDX9_MaterialCookTorranceContext::FillFromXML(CXMLNode *Node, CRenderSettings &Settings) {
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

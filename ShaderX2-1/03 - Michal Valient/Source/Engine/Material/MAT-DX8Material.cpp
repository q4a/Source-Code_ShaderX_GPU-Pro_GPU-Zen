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
#include "material\mat-dx8material.h"
#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "FIL-FileManager.h"
#include "SC-MString.h"
#include "sc-xmlparser.h"
#include "ENG-Texture2D.h"
#include "ENG-TextureManager.h"

CDX8Material::CDX8Material(char *ObjName,CEngine *OwnerEngine) : CBaseMaterial(ObjName,OwnerEngine) {
    this->SetHasContext(true);
}

CDX8Material::~CDX8Material(void) {
}

IDirect3DVertexShader9 *CDX8Material::LoadVS_Bin(int ResourceID, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading vertex shader from resource %i", ResourceID);
    CFileManager *Fil = OwnerEngine->GetFileManager();
    IDirect3DVertexShader9 *OutputShader = NULL;
    int ResourceSize = Fil->GetSize(ResourceID);
    char *Buffer = (char *)MemMgr->AllocMem(ResourceSize,0);
    if (Fil->Load(ResourceID, Buffer)<0) {
        _WARN(this,ErrMgr->TFile, "Cannot load vertex shader from resource - %i", ResourceID);
    }
    HRESULT rc = RenderSettings.Direct3DDevice->CreateVertexShader((DWORD*)Buffer, &OutputShader);
    if (FAILED(rc)) {
        _WARN(this,ErrMgr->TNoSupport, "Cannot create vertex shader - resource %i",ResourceID);
        OutputShader = 0;
    }
    MemMgr->FreeMem(Buffer);
    _LOGE(logE);
    return OutputShader;
}

IDirect3DVertexShader9 *CDX8Material::LoadVS_Bin(CMString &ResourceName, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading vertex shader from '%s'", ResourceName);
    CFileManager *Fil = OwnerEngine->GetFileManager();
    IDirect3DVertexShader9 *OutputShader = NULL;
    int ResourceIndex = Fil->Find(ResourceName);
    if (ResourceIndex<0) {
        _WARN(this,ErrMgr->TFile,"VS file %s not found",ResourceName.c_str());
    } else {
        OutputShader = LoadVS_Bin(ResourceIndex, RenderSettings);
    }
    _LOGE(logE);
    return OutputShader;
}

IDirect3DVertexShader9 *CDX8Material::LoadVS_Src(int ResourceID, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading vertex shader from resource %i", ResourceID);
    CFileManager *Fil = OwnerEngine->GetFileManager();
    IDirect3DVertexShader9 *OutputShader = NULL;
    int ResourceSize = Fil->GetSize(ResourceID);
    char *Buffer = (char *)MemMgr->AllocMem(ResourceSize,0);
    if (Fil->Load(ResourceID, Buffer)<0) {
        _WARN(this,ErrMgr->TFile, "Cannot load vertex shader from resource - %i", ResourceID);
    }
    ID3DXBuffer *ShaderBuffer = NULL;
    ID3DXBuffer *ErrorBuffer = NULL;
    HRESULT rc = D3DXAssembleShader(Buffer, 
                                    ResourceSize, 
                                    NULL, 
                                    NULL, 
                                    D3DXSHADER_DEBUG, 
                                    &ShaderBuffer, 
                                    &ErrorBuffer);
    if (FAILED(rc)) {
        _WARN(this,ErrMgr->TNoSupport, "Cannot assemble vertex shader - %s",ErrorBuffer->GetBufferPointer());
        OutputShader = 0;
    }
    rc = RenderSettings.Direct3DDevice->CreateVertexShader((DWORD*)ShaderBuffer->GetBufferPointer(), &OutputShader);
    if (FAILED(rc)) {
        _WARN(this,ErrMgr->TNoSupport, "Cannot create vertex shader - resource %i",ResourceID);
        OutputShader = 0;
    }
    if (ShaderBuffer) ShaderBuffer->Release();
    if (ErrorBuffer) ErrorBuffer->Release();
    MemMgr->FreeMem(Buffer);
    _LOGE(logE);
    return OutputShader;
}

IDirect3DVertexShader9 *CDX8Material::LoadVS_Src(CMString &ResourceName, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading vertex shader from '%s'", ResourceName);
    CFileManager *Fil = OwnerEngine->GetFileManager();
    IDirect3DVertexShader9 *OutputShader = NULL;
    int ResourceIndex = Fil->Find(ResourceName);
    if (ResourceIndex<0) {
        _WARN(this,ErrMgr->TFile,"VS file %s not found",ResourceName.c_str());
    } else {
        OutputShader = LoadVS_Src(ResourceIndex, RenderSettings);
    }
    _LOGE(logE);
    return OutputShader;
}

bool CDX8Material::DeleteVS(IDirect3DVertexShader9 *ShaderHandle) {
    ULONG refCount = ShaderHandle->Release();
    if (refCount>0) {
        _WARN(this, ErrMgr->TSystem, "Deleting vertex shader %p reference count is still %i", ShaderHandle, refCount);
        return false;
    } else {
        return true;
    }
}

IDirect3DPixelShader9 *CDX8Material::LoadPS_Bin(int ResourceID, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading pixel shader from resource %i", ResourceID);
    CFileManager *Fil = OwnerEngine->GetFileManager();
    IDirect3DPixelShader9 *OutputShader = 0;
    int ResourceSize = Fil->GetSize(ResourceID);
    char *Buffer = (char *)MemMgr->AllocMem(ResourceSize,0);
    if (Fil->Load(ResourceID, Buffer)<0) {
        _WARN(this,ErrMgr->TFile, "Cannot load pixel shader from resource - %i", ResourceID);
    }
    // Create the pixel shader
	HRESULT rc = RenderSettings.Direct3DDevice->CreatePixelShader((DWORD *)Buffer, &OutputShader);
	if (FAILED(rc)) {
        _WARN(this,ErrMgr->TNoSupport, "Cannot create pixel shader - resource %i",ResourceID);
        OutputShader = 0;
	}
    MemMgr->FreeMem(Buffer);
    _LOGE(logE);
    return OutputShader;
}

IDirect3DPixelShader9 *CDX8Material::LoadPS_Bin(CMString &ResourceName, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading pixel shader from '%s'", ResourceName);
    CFileManager *Fil = OwnerEngine->GetFileManager();
    IDirect3DPixelShader9 *OutputShader = 0;
    int ResourceIndex = Fil->Find(ResourceName);
    if (ResourceIndex<0) {
        _WARN(this,ErrMgr->TFile,"PS file %s not found",ResourceName.c_str());
    } else {
        OutputShader = LoadPS_Bin(ResourceIndex,RenderSettings);
    }
    _LOGE(logE);
    return OutputShader;
}

IDirect3DPixelShader9 *CDX8Material::LoadPS_Src(int ResourceID, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading pixel shader from resource %i", ResourceID);
    CFileManager *Fil = OwnerEngine->GetFileManager();
    IDirect3DPixelShader9 *OutputShader = 0;
    int ResourceSize = Fil->GetSize(ResourceID);
    char *Buffer = (char *)MemMgr->AllocMem(ResourceSize,0);
    if (Fil->Load(ResourceID, Buffer)<0) {
        _WARN(this,ErrMgr->TFile, "Cannot load pixel shader from resource - %i", ResourceID);
    }
    ID3DXBuffer *ShaderBuffer = NULL;
    ID3DXBuffer *ErrorBuffer = NULL;
    HRESULT rc = D3DXAssembleShader(Buffer, 
                                    ResourceSize, 
                                    NULL, 
                                    NULL, 
                                    D3DXSHADER_DEBUG, 
                                    &ShaderBuffer, 
                                    &ErrorBuffer);
    if (FAILED(rc)) {
        _WARN(this,ErrMgr->TNoSupport, "Cannot assemble pixel shader - %s",ErrorBuffer->GetBufferPointer());
        OutputShader = 0;
    }
    // Create the pixel shader
	rc = RenderSettings.Direct3DDevice->CreatePixelShader((DWORD *)ShaderBuffer->GetBufferPointer(), &OutputShader);
	if (FAILED(rc)) {
        _WARN(this,ErrMgr->TNoSupport, "Cannot create pixel shader - resource %i",ResourceID);
        OutputShader = 0;
	}
    if (ShaderBuffer) ShaderBuffer->Release();
    if (ErrorBuffer) ErrorBuffer->Release();
    MemMgr->FreeMem(Buffer);
    _LOGE(logE);
    return OutputShader;
}

IDirect3DPixelShader9 *CDX8Material::LoadPS_Src(CMString &ResourceName, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading pixel shader from '%s'", ResourceName);
    CFileManager *Fil = OwnerEngine->GetFileManager();
    IDirect3DPixelShader9 *OutputShader = 0;
    int ResourceIndex = Fil->Find(ResourceName);
    if (ResourceIndex<0) {
        _WARN(this,ErrMgr->TFile,"PS file %s not found",ResourceName.c_str());
    } else {
        OutputShader = LoadPS_Src(ResourceIndex,RenderSettings);
    }
    _LOGE(logE);
    return OutputShader;
}

bool CDX8Material::DeletePS(IDirect3DPixelShader9 *ShaderHandle) {
    ULONG refCount = ShaderHandle->Release();
    if (refCount>0) {
        _WARN(this, ErrMgr->TSystem, "Deleting pixel shader %p reference count is still %i", ShaderHandle, refCount);
        return false;
    } else {
        return true;
    }
}

CDX8MaterialContext *CDX8Material::CreateContextFromXML(CXMLNode *Node, CBase3DObject *Object, CRenderSettings &Settings) {
    CDX8MaterialContext *Context = (CDX8MaterialContext *)CreateContext(Object);
    int PropID;

    //Ambient part of material
    float AmbientMultiplier = 1.0f;
    Node->GetAttributeValueF("AMBIENTCOEFF", AmbientMultiplier);
    Context->SetProperty(Context->QueryProperty("AmbientMultiplier"), &AmbientMultiplier);

    D3DXVECTOR4 AmbientColor;
    CXMLNode *tmpNode = Node->GetSubNode(Node->FindSubNode("AMBIENTCOLOR", 0));
    if (tmpNode!=NULL) {
        tmpNode->GetAttributeValueF("X", AmbientColor.x);
        tmpNode->GetAttributeValueF("Y", AmbientColor.y);
        tmpNode->GetAttributeValueF("Z", AmbientColor.z);
    }
    Context->SetProperty(Context->QueryProperty("AmbientColor"), &AmbientColor);

    PropID = Context->QueryProperty("AmbientTexture");
    if (PropID>=0) {
        int TexID = -1;
        Node->GetAttributeValueI("AMBIENTMAPID", TexID);
        CBaseTexture *AmbientTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(TexID);
        Context->SetProperty(PropID, AmbientTexture);
    }

    //Self illumination (emisive part)
    bool SelfIllumOn = false;
    Node->GetAttributeValueB("SELFILLUMON", SelfIllumOn);
    Context->SetProperty(Context->QueryProperty("SelfIllumOn"), &SelfIllumOn);

    D3DXVECTOR4 SelfIllumColor;
    tmpNode = Node->GetSubNode(Node->FindSubNode("SELFILLUMCOLOR", 0));
    if (tmpNode!=NULL) {
        tmpNode->GetAttributeValueF("X", SelfIllumColor.x);
        tmpNode->GetAttributeValueF("Y", SelfIllumColor.y);
        tmpNode->GetAttributeValueF("Z", SelfIllumColor.z);
    }
    Context->SetProperty(Context->QueryProperty("SelfIllumColor"), &SelfIllumColor);

    float SelfIllumValue = 0;
    Node->GetAttributeValueF("SELFILLUMVALUE", SelfIllumValue);
    Context->SetProperty(Context->QueryProperty("SelfIllumValue"), &SelfIllumValue);

    //Specular
    float SpecularMultiplier = 1;
    Node->GetAttributeValueF("SPECULARCOEFF", SpecularMultiplier);
    Context->SetProperty(Context->QueryProperty("SpecularMultiplier"), &SpecularMultiplier);

    D3DXVECTOR4 SpecularColor;
    tmpNode = Node->GetSubNode(Node->FindSubNode("SPECULARCOLOR", 0));
    if (tmpNode!=NULL) {
        tmpNode->GetAttributeValueF("X", SpecularColor.x);
        tmpNode->GetAttributeValueF("Y", SpecularColor.y);
        tmpNode->GetAttributeValueF("Z", SpecularColor.z);
    }
    Context->SetProperty(Context->QueryProperty("SpecularColor"), &SpecularColor);

    float SpecularShininess = 0.3f;
    Node->GetAttributeValueF("SHININESS", SpecularShininess);
    Context->SetProperty(Context->QueryProperty("SpecularShininess"), &SpecularShininess);

    PropID = Context->QueryProperty("SpecularTexture");
    if (PropID>=0) {
        int TexID = -1;
        Node->GetAttributeValueI("SPECULARMAPID", TexID);
        CBaseTexture *SpecularTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(TexID);
        Context->SetProperty(PropID, SpecularTexture);
    }

    PropID = Context->QueryProperty("NormalTexture");
    if (PropID>=0) {
        int TexID = -1;
        Node->GetAttributeValueI("BUMPMAPID", TexID);
        CBaseTexture *NormalTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(TexID);
        Context->SetProperty(PropID, NormalTexture);
    }

    //Diffuse component
    float DiffuseMultiplier = 1.0f;
    Node->GetAttributeValueF("DIFFUSECOEFF", DiffuseMultiplier);
    Context->SetProperty(Context->QueryProperty("DiffuseMultiplier"), &DiffuseMultiplier);

    D3DXVECTOR4 DiffuseColor;
    tmpNode = Node->GetSubNode(Node->FindSubNode("DIFFUSECOLOR", 0));
    if (tmpNode!=NULL) {
        tmpNode->GetAttributeValueF("X", DiffuseColor.x);
        tmpNode->GetAttributeValueF("Y", DiffuseColor.y);
        tmpNode->GetAttributeValueF("Z", DiffuseColor.z);
    }
    Context->SetProperty(Context->QueryProperty("DiffuseColor"), &DiffuseColor);

    PropID = Context->QueryProperty("DiffuseTexture");
    if (PropID>=0) {
        int TexID = -1;
        Node->GetAttributeValueI("DIFFUSEMAPID", TexID);
        CBaseTexture *DiffuseTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(TexID);
        Context->SetProperty(PropID, DiffuseTexture);
    }
    return Context;
}

//***********************************************
//Material context
//***********************************************
int CDX8MaterialContext::QueryProperty(const char *PropertyName) {
    return -1;
}

bool CDX8MaterialContext::SetProperty(int PropertyID, const void *Property) {
    return false;
}


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

#include "material\mat-vertexphong.h"
#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "ENG-BaseTexture.h"
#include "ENG-TextureManager.h"
#include "ENG-Base3DObject.h"
#include "MGR-DirectGraphicsManager.h"
#include "FIL-FileManager.h"
#include "SC-MString.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "d3scene\eng-d3slight.h"
#include "sc-dynamicarray.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"


CMaterialVertexPhong::CMaterialVertexPhong(char *ObjName,CEngine *OwnerEngine) : CDX8Material(ObjName,OwnerEngine) {
    CurrentContext = NULL;
    VSDeclaration = NULL;
    CreatedShader = 0;
}

CMaterialVertexPhong::~CMaterialVertexPhong(void) {
    if (VSDeclaration!=NULL) {
        VSDeclaration->Release();
        VSDeclaration = NULL;
    }
    if (CreatedShader!=0) {
        DeleteVS(CreatedShader);
        CreatedShader = 0;
    }
}

void CMaterialVertexPhong::DoCacheIn(CRenderSettings &RenderSettings) {
}

void CMaterialVertexPhong::DoCacheOut(CRenderSettings &RenderSettings) {
}

DWORD CMaterialVertexPhong::GetPassCount() {
    return 1;
}

void CMaterialVertexPhong::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CMaterialVertexPhong::CreateContext(CBase3DObject *Obj) {
    CMaterialVertexPhongContext *con = new CMaterialVertexPhongContext;
    con->Object = Obj;
    return con;
}

void CMaterialVertexPhong::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CMaterialVertexPhong::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CMaterialVertexPhongContext *)Context;
    return true;
}

bool CMaterialVertexPhong::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        if (CreatedShader) {
            HRESULT res = RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);
            res = RenderSettings.Direct3DDevice->SetVertexShader(CreatedShader);
            if (FAILED(res)) {
                _WARN(this,ErrMgr->TNoSupport, "Cannot set vertex shader - %p",CreatedShader);
            }
            res = RenderSettings.Direct3DDevice->SetPixelShader(NULL);

            // enable specular lighting
            RenderSettings.Direct3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	        // texture settings
	        RenderSettings.Direct3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	        RenderSettings.Direct3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	        RenderSettings.Direct3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        }
        SetIsSet(true);
    }
    return true;
}

bool CMaterialVertexPhong::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
    return true;
}

bool CMaterialVertexPhong::UnSet(CRenderSettings &RenderSettings) {
    if (GetIsSet()) {
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
        SetIsSet(false);
    }
    return true;
}

void CMaterialVertexPhong::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

bool CMaterialVertexPhong::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    //*******
    //* Vertex shader loading
    //*******
    //Vertex shaders support
    if((OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) || 
        (OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)) { 
            if( OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(1,1) )
                _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 1.1 not supported");
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
    CMString VSFile = "CMaterialVertexPhong.vsh";
    CreatedShader = LoadVS_Src(VSFile, RenderSettings);

    SetFilled(true);
    return true;
}

bool CMaterialVertexPhong::Update(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        //SetTexture
        this->OwnerEngine->GetTextureManager()->SetTexture(0,CurrentContext->Diffuse);

	    // set the clip matrix
        D3DXMATRIX tmp;
        D3DXMatrixMultiplyTranspose(&tmp, &CurrentContext->Object->WorldTransform, &RenderSettings.CameraTransformMatrix);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(0, tmp, 4);

        D3DXMATRIX matWorldInverse;
        D3DXMatrixInverse(&matWorldInverse, NULL, &CurrentContext->Object->WorldTransform);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(4,matWorldInverse,4);

        // set the world matrix
	    RenderSettings.Direct3DDevice->SetVertexShaderConstantF(8,CurrentContext->Object->WorldTransform,4);

        // light 1 position
        if (CurrentContext->Object->Lights->GetSize()>0) {
            CD3SLight *Light = (CD3SLight *)CurrentContext->Object->Lights->GetAt(0);
            // light position
            RenderSettings.Direct3DDevice->SetVertexShaderConstantF(12, Light->WorldObjectPos,1);
        } else {
            // light position
	        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(12, RenderSettings.CameraSource,1);
        }

        // set eye vector
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(13, RenderSettings.CameraSource,1);

        D3DXCOLOR matDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(14, matDiffuse, 1);

        D3DXVECTOR4 LightColor = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(15, LightColor, 1);

	    // Attenuation
	    D3DXVECTOR4 Attenuation(1.0f, 0.0f, 0.0f, 0.0f);
	    RenderSettings.Direct3DDevice->SetVertexShaderConstantF(16, Attenuation, 1); 

        // specular power
        D3DXVECTOR4 SpecularPower(25,25,25,25);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(17, SpecularPower,1);
    }
    return true;
}


//***********************************************
//Material context
//***********************************************
CMaterialVertexPhongContext::CMaterialVertexPhongContext() : CDX8MaterialContext() {
    Diffuse = NULL;
}

CMaterialVertexPhongContext::~CMaterialVertexPhongContext() {
    if (Diffuse!=NULL) {
        Diffuse->OwnerEngine->GetTextureManager()->Delete(Diffuse->GetResourceID());
    }
}

int CMaterialVertexPhongContext::QueryProperty(const char *PropertyName) {
    if (strcmp(PropertyName, "DiffuseTexture") == 0) return PID_Diffuse;
    return CDX8MaterialContext::QueryProperty(PropertyName);
}

bool CMaterialVertexPhongContext::SetProperty(int PropertyID, const void *Property) {
    switch (PropertyID) {
        case PID_Diffuse : {
            if (Diffuse!=NULL) {
                Diffuse->OwnerEngine->GetTextureManager()->Delete(Diffuse->GetResourceID());
            }
            Diffuse = (CBaseTexture *)Property;
            return true;
        } break;
        default : {
            return CDX8MaterialContext::SetProperty(PropertyID, Property);
        }
    }
}

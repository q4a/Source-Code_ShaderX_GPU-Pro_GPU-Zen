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

#include "material\MAT-ShadowBuffer.h"
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

#include "Shader\SHD-ShaderManager.h"
#include "shader\shd-vertexshader.h"
#include "shader\shd-pixelshader.h"

CMaterialShadowBuffer::CMaterialShadowBuffer(char *ObjName,CEngine *OwnerEngine) : CBaseMaterial(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    CurrentContext = NULL;
    VSDeclaration = NULL;
    VertexShader = PixelShader = NULL;
}

CMaterialShadowBuffer::~CMaterialShadowBuffer(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    DeleteShaders();
}

void CMaterialShadowBuffer::DoCacheIn(CRenderSettings &RenderSettings) {
}

void CMaterialShadowBuffer::DoCacheOut(CRenderSettings &RenderSettings) {
}

DWORD CMaterialShadowBuffer::GetPassCount() {
    return 1;
}

void CMaterialShadowBuffer::SetPassCount(DWORD PassCount) {
}

CBaseMaterialContext *CMaterialShadowBuffer::CreateContext(CBase3DObject *Obj) {
    CMaterialShadowBufferContext *con = new CMaterialShadowBufferContext;
    con->Object = Obj;
    return con;
}

void CMaterialShadowBuffer::DeleteContext(CBaseMaterialContext *Context) {
    delete Context;
}

bool CMaterialShadowBuffer::SetContext(CBaseMaterialContext *Context) {
    CurrentContext = (CMaterialShadowBufferContext *)Context;
    return true;
}

bool CMaterialShadowBuffer::Set(CRenderSettings &RenderSettings) {
    if (!GetIsSet()) {
        //RenderSettings.Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        RenderSettings.Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

        if (VertexShader && PixelShader) {
            RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);
            OwnerEngine->GetShaderManager()->SetVertexShader(VertexShader, RenderSettings);
            OwnerEngine->GetShaderManager()->SetPixelShader(PixelShader, RenderSettings);
        }

        SetIsSet(true);
    }
    return true;
}

bool CMaterialShadowBuffer::SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) {
    return true;
}

bool CMaterialShadowBuffer::UnSet(CRenderSettings &RenderSettings) {
    RenderSettings.Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    SetIsSet(false);
    return true;
}


void CMaterialShadowBuffer::DeviceLost(CRenderSettings &Settings) {
    SetLost(false);
}

void CMaterialShadowBuffer::DeleteShaders() {
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
}

bool CMaterialShadowBuffer::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading material");
    DeleteShaders();
    //Vertex shaders support
    if((OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) || 
        (OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)) {
            if( OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(2,0) ) {
                _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 2.0 not supported");
            }
    }
    //pixel shaders support
    if(OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DVS_VERSION(2,0)) {
        _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 2.0 not supported");
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

    //Load vertex shader
    CMString ShaderFile = "CMaterialShadowBuffer.vsh";
    int ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
    VertexShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
    if (VertexShader==NULL) {
        VertexShader = new CVertexShader(ShaderFile.c_str(),OwnerEngine);
        VertexShader->SetResourceID(ShaderIndex);
        VertexShader->Load(ShaderIndex,RenderSettings);
        OwnerEngine->GetShaderManager()->Add(ShaderIndex,*VertexShader);
    }
    
    //Load pixel shader
    ShaderFile = "CMaterialShadowBuffer.psh";
    ShaderIndex = OwnerEngine->GetFileManager()->Find(ShaderFile);
    PixelShader = (CBaseShader *)OwnerEngine->GetShaderManager()->Query(ShaderIndex);
    if (PixelShader==NULL) {
        PixelShader = new CPixelShader(ShaderFile.c_str(),OwnerEngine);
        PixelShader->SetResourceID(ShaderIndex);
        PixelShader->Load(ShaderIndex,RenderSettings);
        OwnerEngine->GetShaderManager()->Add(ShaderIndex,*PixelShader);
    }

    SetFilled(true);
    _LOGE(logE,"Loaded");
    return true;
}

bool CMaterialShadowBuffer::Update(CRenderSettings &RenderSettings) {
    if (CurrentContext!=NULL) {
        // world * view * proj matrix
        D3DXMATRIX tmp;
        D3DXMatrixMultiplyTranspose(&tmp, &CurrentContext->Object->WorldTransform, &RenderSettings.CameraTransformMatrix);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(0, tmp, 4);

        // world matrix
        D3DXMatrixTranspose(&tmp,&CurrentContext->Object->WorldTransform);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(4, tmp, 4);

        //light position
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(8, RenderSettings.CameraSource,  1 );

        //Camera distance ranges
        float rcpZ = 1.0f / (RenderSettings.CameraZFar - RenderSettings.CameraZNear);
        //float shZAdjust = 1.0f / 100.0f;             //depth adjust
        float shZAdjust = 0.0f;             //depth adjust
        D3DXCOLOR Ranges(RenderSettings.CameraZNear, RenderSettings.CameraZFar, rcpZ, shZAdjust);
        RenderSettings.Direct3DDevice->SetVertexShaderConstantF(9,Ranges,1);
    }
    return true;
}

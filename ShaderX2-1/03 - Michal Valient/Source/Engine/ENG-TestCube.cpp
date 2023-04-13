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


// ENG-TestCube.cpp: implementation of the CTestCube class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <D3dx9.h>
#include "ENG-TestCube.h"
#include "ENG-TextureManager.h"
#include "SC-MString.h"
#include "FIL-FileManager.h"
//#include "eng-meshms3d.h"
#include "ENG-MeshManager.h"
#include "ENG-Engine.h"
#include "ENG-Texture2D.h"
#include "ENG-MeshPart.h"
#include "ENG-RenderSettings.h"
#include "eng-meshd3f.h"

CTestCube::CTestCube(char *pObjName,CEngine *pOwnerEngine) : CBase3DObject(pObjName,pOwnerEngine) {
    prevTime = 0;
    StateBlock = 0;
    Mesh = NULL;
    Texture = NULL;
}

CTestCube::~CTestCube() {
    int loge = _LOGB(this,D3_DV_GRP2_LEV0, "Destroying");
    CacheOut(*this->OwnerEngine->GetRenderSettings());
    if (Mesh!=NULL) {
        this->OwnerEngine->GetMeshManager()->Delete(Mesh->GetResourceID());
    }
    if (Texture!=NULL) {
        this->OwnerEngine->GetTextureManager()->Delete(Texture->GetResourceID());
    }
    _LOGE(loge,"Destroyed");
}


void CTestCube::DoCacheIn(CRenderSettings &RenderSettings) {
/*
    if (!GetFilled()) { //needed to be loaded
        if (!this->Load(LoadIndex, RenderSettings)) {
            _LOG(this,D3_DV_GRP2_LEV2,"Cannot load object");
        }
    }
    Mesh->CacheIn(RenderSettings);
    Texture->CacheIn(RenderSettings);

    //StateBlock Recorder
    if (StateBlock!=0) RenderSettings.Direct3DDevice->DeleteStateBlock(StateBlock);
    RenderSettings.Direct3DDevice->BeginStateBlock();
    RenderSettings.Direct3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    RenderSettings.Direct3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);
    RenderSettings.Direct3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    RenderSettings.Direct3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    RenderSettings.Direct3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    RenderSettings.Direct3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER,   D3DTEXF_LINEAR );
    RenderSettings.Direct3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER,   D3DTEXF_LINEAR );
    RenderSettings.Direct3DDevice->EndStateBlock(&StateBlock);

    this->Update(RenderSettings);
*/
}

void CTestCube::DoCacheOut(CRenderSettings &RenderSettings) {
/*    
    if (StateBlock) {
        RenderSettings.Direct3DDevice->DeleteStateBlock(StateBlock);
        StateBlock = 0;
    }
    Mesh->CacheOut(RenderSettings);
    Texture->CacheOut(RenderSettings);
*/
}

void *CTestCube::Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) {
/*    
    RenderSettings.Direct3DDevice->ApplyStateBlock(StateBlock);
    //SetTexture
    this->OwnerEngine->GetTextureManager()->SetTexture(0,*Texture);
    //Transform it into world space
    RenderSettings.Direct3DDevice->SetTransform(D3DTS_WORLD, &this->ObjectTransformation);
	// Set the vertex shader to the FVF fixed function shader
    RenderSettings.Direct3DDevice->SetVertexShader(Mesh->GetSubset(0)->GetVertexFVF());
    //render mesh
    Mesh->Render(RenderSettings,-1);
    //RenderSettings.Direct3DDevice->SetTexture(0,NULL);
*/
    return NULL;
}

void CTestCube::PrepareBoundingGeometry(CRenderSettings &RenderInfo) {
/*
    BoundSphereCenter.x = BoundSphereCenter.y = BoundSphereCenter.z = 0;
    BoundSphereRadius = 1;
*/
}

void CTestCube::PrepareGeometry(CRenderSettings &RenderInfo) {
/*
    D3DXMATRIX matWorld1;
    D3DXMATRIX matWorld2;
    D3DXMATRIX matRotation;
    D3DXMatrixRotationY(&matWorld1,(float)RenderInfo.RenderTime);
    D3DXMatrixRotationX(&matWorld2,(float)RenderInfo.RenderTime / 2);
    D3DXMatrixMultiply(&matRotation,&matWorld1,&matWorld2);
    D3DXMatrixMultiply((D3DXMATRIX *)&this->ObjectTransformation,&matRotation,(D3DXMATRIX *)&this->WorldTransformation);
*/
}

CBase3DObject &CTestCube::Clone(bool CloneGeometry) {
    CMString name = this->ObjectName;
    name+="_Cloned";
    return *(new CTestCube(name.c_str(), this->OwnerEngine));
}

void CTestCube::DeviceLost(CRenderSettings &Settings) {
/*
    Mesh->DeviceLost(Settings);
    Texture->DeviceLost(Settings);
    if (Mesh->GetLost() || Texture->GetLost()) SetLost(TRUE);
*/
}

bool CTestCube::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
/*
    if (Mesh==NULL) {
        Mesh = (CMesh *)OwnerEngine->GetMeshManager()->Query(0);
        if (Mesh==NULL) {
            CMString name = "scene.Torus Knot01.d3mesh";
//            CMString name = "scene.Box02.d3m";
            int resindex = OwnerEngine->GetFileManager()->Find(name);
            Mesh = new CMeshD3F(name.c_str(), OwnerEngine);
            Mesh->Load(resindex,RenderSettings);
            OwnerEngine->GetMeshManager()->Add(0, *Mesh);
        }
    }
    if (Texture==NULL) {    //no texture
        Texture = (CTexture2D *)OwnerEngine->GetTextureManager()->Query(0);
        if (Texture==NULL) {    //first time creator
            CMString name = "a.jpg";
            int resindex = OwnerEngine->GetFileManager()->Find(name);
            Texture = new CTexture2D("A_JPG",OwnerEngine);
            Texture->Load(resindex,RenderSettings);
            OwnerEngine->GetTextureManager()->Add(0,*Texture);
        }
    }
    this->SetFilled(TRUE);
*/
    return TRUE;
}

bool CTestCube::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}

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
#include "d3scene\eng-d3sobject.h"

#include "ENG-RenderSettings.h"
#include "ENG-BaseTexture.h"
#include "ENG-TextureManager.h"
#include "ENG-MeshManager.h"
#include "ENG-Mesh.h"
#include "ENG-Engine.h"
#include "ENG-MeshPart.h"
#include "Material\MAT-BaseMaterial.h"
#include "Material\MAT-MaterialManager.h"
#include "material\mat-vertexphong.h"
#include "SC-HashTable2.h"
#include "sc-xmlparser.h"

CD3SObject::CD3SObject(char *ObjName,CEngine *OwnerEngine) : CBase3DObject(ObjName, OwnerEngine) {
    Mesh = NULL;
    SubSetMaterials = new CHashTable2(true,0);
    Visible = true;
}

CD3SObject::~CD3SObject(void) {
    int loge = _LOGB(this,D3_DV_GRP2_LEV0, "Destroying");
    if (Mesh!=NULL) this->OwnerEngine->GetMeshManager()->Delete(Mesh->GetResourceID());
    delete SubSetMaterials;
    _LOGE(loge);
}

void CD3SObject::DoCacheIn(CRenderSettings &RenderSettings) {
    CBaseResource::DoCacheIn(RenderSettings);
    Mesh->CacheIn(RenderSettings);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if (MS->Material!=NULL) MS->Material->CacheIn(RenderSettings);
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
    this->Update(RenderSettings);
}

void CD3SObject::DoCacheOut(CRenderSettings &RenderSettings) {
    Mesh->CacheOut(RenderSettings);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if (MS->Material!=NULL) MS->Material->CacheOut(RenderSettings);
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
}

void *CD3SObject::Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) {
    CBaseMaterial *tmpMaterial = NULL;
    CBaseMaterialContext *tmpContext = NULL;
    if (ForcedMaterial!=NULL) {
        tmpMaterial = ForcedMaterial;
        tmpContext = ForcedContext;
        tmpContext->Object = this;
    }
    for (int i=0; i<Mesh->GetSubsetCount(); i++) {
        CMatSubset *MS = (CMatSubset *)SubSetMaterials->Get(i);
        if (ForcedMaterial == NULL) {
            //Set material
            tmpMaterial = MS->Material;
            tmpContext = MS->MaterialContext;
        }
        tmpMaterial->SetContext(tmpContext);
        for (DWORD j=0; j<tmpMaterial->GetPassCount(); j++) {
            OwnerEngine->GetMaterialManager()->SetMaterial(tmpMaterial, tmpContext, j, RenderSettings);
            tmpMaterial->Update(RenderSettings);
            Mesh->Render(RenderSettings,i);
        }
    }

    /*
    //Bound sphere rendering
    RenderSettings.Direct3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    ID3DXMesh *SphereMesh;
    D3DXCreateSphere(RenderSettings.Direct3DDevice, this->WorldBSRadius,10,10, &SphereMesh, NULL);
    D3DXMATRIX SphereMatrix;
    D3DXMatrixTranslation(&SphereMatrix, WorldBSCenter.x, WorldBSCenter.y, WorldBSCenter.z);
    RenderSettings.Direct3DDevice->SetTransform(D3DTS_WORLD, &SphereMatrix);
    SphereMesh->DrawSubset(0);
    SphereMesh->Release();
    RenderSettings.Direct3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    //END - Bound sphere rendering    
    */
    return NULL;
}

void CD3SObject::PrepareBoundingGeometry(CRenderSettings &RenderInfo) {
    if (RenderInfo.RenderID != TransformRenderID) {
        this->BSCenter = Mesh->BoundSphereCenter;
        this->BSRadius = Mesh->BoundSphereRadius;
        CBase3DObject::PrepareBoundingGeometry(RenderInfo);
    }
}

void CD3SObject::PrepareGeometry(CRenderSettings &RenderInfo) {
}

void CD3SObject::DeviceLost(CRenderSettings &Settings) {
    Mesh->DeviceLost(Settings);
    if (Mesh->GetLost()) SetLost(TRUE);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if (MS->Material!=NULL) {
                MS->Material->DeviceLost(Settings);
                if (MS->Material->GetLost()) SetLost(TRUE);
            }
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
}

bool CD3SObject::AssignMesh(DWORD MeshID) {
    if (this->Mesh!=NULL) {
        this->OwnerEngine->GetMeshManager()->Delete(this->Mesh->GetResourceID());
        this->Mesh = NULL;
    }
    if (MeshID<0xFFFFFFFF) {
        this->Mesh = (CMesh *)OwnerEngine->GetMeshManager()->Query(MeshID);
        if (this->Mesh == NULL) {
            _WARN(this,ErrMgr->TSystem,"Mesh with ID %u cannot be queried",MeshID);
            return false;
        }
    }
    return true;
}

bool CD3SObject::AssignMaterial(DWORD SubsetID, DWORD MaterialID, void *Context) {
    CMatSubset *MS = (CMatSubset *)SubSetMaterials->Get(SubsetID);
    if (MS==NULL) {
        MS = new CMatSubset;
        SubSetMaterials->Add(SubsetID,MS);
    }
    if (MS!=NULL && MS->Material!=NULL) {
        MS->Material->DeleteContext((CBaseMaterialContext *)MS->MaterialContext);
        this->OwnerEngine->GetMaterialManager()->Delete(MS->Material->GetResourceID());
        MS->Material = NULL;
        MS->MaterialContext = NULL;
    }
    if (MaterialID<0xFFFFFFFF) {
        MS->Material = (CBaseMaterial *)OwnerEngine->GetMaterialManager()->Query(MaterialID);
        MS->MaterialContext = (CBaseMaterialContext *)Context;
        if (MS->Material == NULL) {
            _WARN(this,ErrMgr->TSystem,"Material with ID %u cannot be queried",MaterialID);
            MS->MaterialContext = NULL;
            return false;
        }
    }
    return true;
}
 
bool CD3SObject::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    if ((Mesh!=NULL) && (!Mesh->GetFilled())) Mesh->Load(Mesh->GetLoadIndex(), RenderSettings);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if ((MS->Material!=NULL) && (!MS->Material->GetFilled())) MS->Material->Load(MS->Material->GetLoadIndex(),RenderSettings);
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
    this->SetFilled(TRUE);
    return TRUE;
}

bool CD3SObject::Update(CRenderSettings &RenderSettings) {
    if (Mesh!=NULL) Mesh->Update(RenderSettings);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if (MS->Material!=NULL) MS->Material->Update(RenderSettings);
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
    return TRUE;
}

CD3SObject::CMatSubset::~CMatSubset() {
    if (Material!=NULL) {
        if (MaterialContext!=NULL) Material->DeleteContext((CBaseMaterialContext *)MaterialContext);
        Material->OwnerEngine->GetMaterialManager()->Delete(Material->GetResourceID());
    }
}

CD3SObject::CMatSubset::CMatSubset() {
    Material = NULL;
    MaterialContext = NULL;
}

bool CD3SObject::LoadXML(CXMLNode *Node, CRenderSettings &Settings) {
    CBase3DObject::LoadXML(Node, Settings);
    MeshID = 0;
    Node->GetAttributeValueI("MESHID", MeshID);
    return true;
}

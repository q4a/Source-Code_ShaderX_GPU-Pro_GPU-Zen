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


// ENG-Mesh.cpp: implementation of the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-Mesh.h"
#include "MGR-DirectGraphicsManager.h"
#include "ENG-Engine.h"
#include "ENG-RenderSettings.h"
#include "nommgr.h"
#include <D3d9.h>
#include "mmgr.h"
#include "ENG-MeshPart.h"
#include "SC-DynamicArray.h"
#include "ENG-MeshManager.h"

//#########################################################################
//########## Construction / Destruction
//#########################################################################

CMesh::CMesh(char *ObjName, CEngine *OwnerEngine) : CBaseResource(ObjName,OwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Creating");    
    Subsets = new CRootClassArray(true);
    BoundSphereRadius = 0;
    _LOGE(logE);
}

CMesh::~CMesh() {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Destroying");
    ReleaseBuffers(FALSE);
    _LOGE(logE);
}

//#########################################################################
//########## Mesh Lost / Restore methods
//#########################################################################
void CMesh::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Sending device lost");
    if (this->GetLost()) {
        _LOG(this, D3_DV_GRP2_LEV0, "Mesh is already marked as lost");
    } else {
        bool IsLost = FALSE;
        //Vertex buffer loster :-)
        for (int i=0; i<Subsets->GetSize(); i++) {
            CMeshPart *Buf = (CMeshPart *)Subsets->GetAt(i);
            if (Buf->GetIndexBuffer()!=NULL && Buf->GetIndexPool()!=D3DPOOL_MANAGED && Buf->GetIndexPool()!=D3DPOOL_SYSTEMMEM) { //it cannot be restored, we are killing index buffer
                IsLost = TRUE;
                break;
            }
            if (Buf->GetVertexBuffer()!=NULL && Buf->GetVertexPool()!=D3DPOOL_MANAGED && Buf->GetVertexPool()!= D3DPOOL_SYSTEMMEM) { //it cannot be restored, we are killing vertex buffer
                IsLost = TRUE;
                break;
            }
        }
        if (IsLost == TRUE) {
            DoCacheOut(Settings);
            this->SetLost(TRUE);
            _LOG(this,D3_DV_GRP2_LEV0,"Marking as lost");
        } else {
            _LOG(this,D3_DV_GRP2_LEV0,"Not marked as lost");
        }
    }
    _LOGE(logE,"Sended device lost");
}

void CMesh::AddSubset(CMeshPart &Subset) {
    this->Subsets->Add(&Subset);
}

CMeshPart *CMesh::GetSubset(int SubsetIndex) {
    return (CMeshPart *)this->Subsets->GetAt(SubsetIndex);
}

void CMesh::DeleteSubset(int SubsetIndex) {
    this->Subsets->DeleteAt(SubsetIndex);
}

int CMesh::GetSubsetCount() {
    return Subsets->GetSize();
}

void CMesh::DoCacheOut(CRenderSettings &RenderSettings) {
    ReleaseBuffers(TRUE);
    SetFilled(FALSE);
}

void CMesh::DoCacheIn(CRenderSettings &RenderSettings) {
    CBaseResource::DoCacheIn(RenderSettings);
    for (int i=0; i<Subsets->GetSize(); i++) {
        CMeshPart *Buf = (CMeshPart *)Subsets->GetAt(i);
        if (Buf->GetIndexBuffer()!=NULL) Buf->GetIndexBuffer()->PreLoad();
        if (Buf->GetVertexBuffer()!=NULL) Buf->GetVertexBuffer()->PreLoad();
    }
}

//#########################################################################
//########## Render methods
//#########################################################################
void CMesh::Render(CRenderSettings &RenderSettings, int Section) {
    int logE = _LOGB(this, D3_DV_GRP2_LEV4, "Rendering mesh with section %i", Section);
    if (Section>=0) {
        CMeshPart *SubSet = (CMeshPart *)this->Subsets->GetAt(Section);
        HRESULT res = OwnerEngine->GetMeshManager()->SetStreamSource(0, SubSet->GetVertexBuffer(), 0, SubSet->GetStrideSize());
        res = OwnerEngine->GetMeshManager()->SetIndices(SubSet->GetIndexBuffer());
        res = OwnerEngine->GetGraphicsManager()->Direct3DDevice->DrawIndexedPrimitive(SubSet->GetPrimitiveType(), 
                                                  SubSet->GetBaseVertexIndex(), 
                                                  SubSet->GetMinIndex(), 
                                                  SubSet->GetNumVerticles(), SubSet->GetStartIndex(), 
                                                  SubSet->GetPrimitiveCount());
    } else {
        for (int i=0; i<this->Subsets->GetSize(); i++) {
            Render(RenderSettings, i);
        }
    }
    _LOGE(logE);
}

//#########################################################################
//########## Buffer methods
//#########################################################################
void CMesh::ReleaseBuffers(bool Recreate) {
    int logE = _LOGB(this, D3_DV_GRP2_LEV2, "Releasing mesh buffers");
    delete Subsets;
    if (Recreate) {
        Subsets = new CRootClassArray(true);
    }
    _LOGE(logE);
}

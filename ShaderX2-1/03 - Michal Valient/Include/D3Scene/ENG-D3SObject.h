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


#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "eng-base3dobject.h"

class CMesh;
class CHashTable2;
class CXMLNode;

class CD3SObject : public CBase3DObject {

    class CMatSubset : public CRootClass {
    public:
        CBaseMaterial           *Material;        ///If not null, this is material of this object
        CBaseMaterialContext    *MaterialContext; ///If not null, this is material context of this object

        CMatSubset();
        ~CMatSubset();
    };
    CMesh  *Mesh;                   ///If not null, this is mesh of this object

    CHashTable2    *SubSetMaterials; ///Materials for subsets (CMatSubset class)

    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
	virtual ~CD3SObject();
public:
    int     MeshID;                 ///ID of the mesh

    CD3SObject(char *ObjName,CEngine *OwnerEngine);

    ///Context parameter is return value of previous call for this object (null on first call)
    ///if ForcedMaterial is not null, then object _MUST_ render with this material and 
    ///   using ForcedContext. Object _MUST_ fill Object attribute of the supplied context.
    ///   Object can expect, that material and context is already set.
    virtual void *Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext);
    virtual void PrepareGeometry(CRenderSettings &RenderInfo);
    virtual void PrepareBoundingGeometry(CRenderSettings &RenderInfo);

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    virtual bool AssignMesh(DWORD MeshID);
    virtual bool AssignMaterial(DWORD SubsetID, DWORD MaterialID, void *Context);

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CD3SObject);
    MAKE_CLSNAME("CD3SObject");
    MAKE_DEFAULTGARBAGE()
};

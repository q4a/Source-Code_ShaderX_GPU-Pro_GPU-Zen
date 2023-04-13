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


// ENG-Base3DObject.h: interface for the CBase3DObject class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "ENG-BaseResource.h"

//############################################################################
//Forward declarations
class CRootClassArray;
class CXMLNode;
class CPRSLinearController;
class CBaseMaterial;
class CBaseMaterialContext;

class CBase3DObject : public CBaseResource {
protected:
    UINT            TransformRenderID;  ///In this number, render ID is stored 
                                        ///when first called prepareBounding geometry. Every other call is
                                        ///ignored if here is stored the same value as in RenderSettings.
public:
    CPRSLinearController *PRSController;///If not null, object is animated with this controller.

    CRootClassArray *Lights;            ///Here are stored lights, that affects this object.

    bool            Visible;            ///If object is visible (means if it is worth calling render method)
    D3DXVECTOR3     Translation;
    D3DXQUATERNION  Rotation;
    D3DXVECTOR3     Scale;

    ///World object position
    D3DXVECTOR3     WorldObjectPos;

    //Bounding sphere - original parameters
    D3DXVECTOR3     BSCenter;
    float           BSRadius;

    //Bounding sphere - after transform to world space (after prepareGeometry phase)
    D3DXVECTOR3     WorldBSCenter;
    float           WorldBSRadius;

    //Transformation matrixes
    D3DXMATRIX      WorldTransform;
    D3DXMATRIX      LocalTransform;

    int             ParentID;       ///ID of the parent object.
    CBase3DObject  *Parent;         ///if not null, this is parent object

    CBase3DObject(char *ObjName,CEngine *OwnerEngine);
	virtual ~CBase3DObject();

    //Renders object to screen.
    virtual void *Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) = 0;

    //This method is called in first phase of rendering - object is requested to 
    //transform its bounding objects to its final position, size and shape.
    //All *Matrix structure have to be valid after call of this method.
    //Default implementation forces parent to compute bound geometry,
    //then computes local transform and then objects world transform.
    virtual void PrepareBoundingGeometry(CRenderSettings &RenderInfo);

    //object is requested to prepare its geometry for rendering. Changes to vertex 
    //buffers are done in this phase
    virtual void PrepareGeometry(CRenderSettings &RenderInfo) = 0;

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSNAME("CBase3DObject");
};


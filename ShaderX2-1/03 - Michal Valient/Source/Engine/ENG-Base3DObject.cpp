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


// ENG-Base3DObject.cpp: implementation of the CBase3DObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-Base3DObject.h"
#include "ENG-RenderSettings.h"
#include "sc-dynamicarray.h"
#include "sc-xmlparser.h"

#include "d3scene\eng-d3s_prslinearcontroller.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBase3DObject::CBase3DObject(char *ObjName,CEngine *OwnerEngine) : CBaseResource(ObjName,OwnerEngine) {
    Visible = false;
    Parent = NULL;
    TransformRenderID = 0;
    WorldBSRadius = BSRadius = 0;
    Lights = new CRootClassArray(false);
    PRSController = NULL;
}

CBase3DObject::~CBase3DObject() {
    delete Lights;
    if (PRSController) delete PRSController;
}

void CBase3DObject::PrepareBoundingGeometry(CRenderSettings &RenderInfo) {
    if (RenderInfo.RenderID != TransformRenderID) {
        TransformRenderID = RenderInfo.RenderID;
        D3DXMATRIX  BaseTransform;
        if (Parent!=NULL) {
            Parent->PrepareBoundingGeometry(RenderInfo);
            BaseTransform = Parent->WorldTransform;
        } else {
            D3DXMatrixIdentity(&BaseTransform);
        }
        if (PRSController) PRSController->Animate(this, (float)RenderInfo.RenderTime, RenderInfo);
        D3DXMatrixTransformation(   &this->LocalTransform, NULL, NULL, 
                                    &this->Scale, NULL, &this->Rotation, &this->Translation);
        this->WorldTransform = this->LocalTransform * BaseTransform;
        D3DXVec3TransformCoord(&this->WorldObjectPos, &this->Translation, &BaseTransform);
        D3DXVECTOR3 tmp = this->BSCenter + D3DXVECTOR3(BSRadius,0,0);
        D3DXVec3TransformCoord(&this->WorldBSCenter, &this->BSCenter, &this->WorldTransform);
        D3DXVec3TransformCoord(&tmp, &tmp, &this->WorldTransform);
        tmp = this->WorldBSCenter - tmp;
        WorldBSRadius = D3DXVec3Length(&tmp);
    }
}

bool CBase3DObject::LoadXML(CXMLNode *Node, CRenderSettings &Settings) {
    CXMLNode *tmpNode = Node->GetSubNode(Node->FindSubNode("LOCALPIVOT",0));
    ParentID = 0;
    Node->GetAttributeValueI("PARENTID", ParentID);
    if (tmpNode!=NULL) {
        CXMLNode *tmpPosition = tmpNode->GetSubNode(tmpNode->FindSubNode("POSITION",0));
        if (tmpPosition) {
            Translation.x = Translation.y = Translation.z = 0;
            tmpPosition->GetAttributeValueF("X", Translation.x);
            tmpPosition->GetAttributeValueF("Y", Translation.y);
            tmpPosition->GetAttributeValueF("Z", Translation.z);
        } else {
            _WARN(this, ErrMgr->TNoSupport, "No 'POSITION' node found in object's localpivot.");
        }
        CXMLNode *tmpRotation = tmpNode->GetSubNode(tmpNode->FindSubNode("ROTATION",0));
        if (tmpRotation) {
            Rotation.x = Rotation.y = Rotation.z = Rotation.w = 0;
            tmpRotation->GetAttributeValueF("X", Rotation.x);
            tmpRotation->GetAttributeValueF("Y", Rotation.y);
            tmpRotation->GetAttributeValueF("Z", Rotation.z);
            tmpRotation->GetAttributeValueF("W", Rotation.w);
        } else {
            _WARN(this, ErrMgr->TNoSupport, "No 'ROTATION' node found in object's localpivot.");
        }
        CXMLNode *tmpScale = tmpNode->GetSubNode(tmpNode->FindSubNode("SCALE",0));
        if (tmpScale) {
            Scale.x = Scale.y = Scale.z = 0;
            tmpScale->GetAttributeValueF("X", Scale.x);
            tmpScale->GetAttributeValueF("Y", Scale.y);
            tmpScale->GetAttributeValueF("Z", Scale.z);
        } else {
            _WARN(this, ErrMgr->TNoSupport, "No 'SCALE' node found in object's localpivot.");
        }
    } else {
        _WARN(this, ErrMgr->TNoSupport, "No 'LOCALPIVOT' node found in object.");
    }
    return true;
}

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
#include "d3scene\eng-d3scamera.h"
#include "ENG-RenderSettings.h"
#include "sc-xmlparser.h"

CD3SCamera::CD3SCamera(char *ObjName,CEngine *OwnerEngine) : CBase3DObject(ObjName, OwnerEngine) {
    Ortho = false;
    FOV = 120;
    NearClip = 0;
    FarCLip = 1000;
    Target = NULL;
    Recompute();

    Visible = false;
    SetFilled(TRUE);
    SetLost(FALSE);
}

CD3SCamera::~CD3SCamera(void) {
}

void CD3SCamera::PrepareGeometry(CRenderSettings &RenderInfo) {
}

void *CD3SCamera::Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) {
    return NULL;
}

void CD3SCamera::PrepareBoundingGeometry(CRenderSettings &RenderInfo) {
    if (Target!=NULL) Target->PrepareBoundingGeometry(RenderInfo);
    CBase3DObject::PrepareBoundingGeometry(RenderInfo);
}

void CD3SCamera::DoCacheIn(CRenderSettings &RenderSettings) {
}

void CD3SCamera::DoCacheOut(CRenderSettings &RenderSettings) {
}

bool CD3SCamera::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    SetFilled(TRUE);
    return TRUE;
}

bool CD3SCamera::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}

void CD3SCamera::DeviceLost(CRenderSettings &Settings) {
    SetLost(FALSE);
}

void CD3SCamera::Apply(CRenderSettings &Settings) {
    float fFov = D3DXToRadian((float)FOV);
    fFov = (3.0f / 4.0f) * fFov;
    Settings.CameraFieldOfView = fFov;
    Settings.CameraSource = this->WorldObjectPos;
    Settings.CameraTarget = this->Target->WorldObjectPos;
    Settings.CameraZNear = (float)this->NearClip;
    Settings.CameraZFar = (float)this->FarCLip;
    Settings.CameraUpVector = Up;
}

void CD3SCamera::Recompute() {
    Up.x = 0;
    Up.y = 1;
    Up.z = 0;
}

void CD3SCamera::RotateUD(float Angle) {
    D3DXMATRIX tToCenter,tRotate, tToOriginal;
    D3DXMatrixTranslation(&tToCenter, -Target->WorldObjectPos.x,-Target->WorldObjectPos.y,-Target->WorldObjectPos.z);
    D3DXMatrixTranslation(&tToOriginal, Target->WorldObjectPos.x,Target->WorldObjectPos.y,Target->WorldObjectPos.z);

    D3DXVECTOR2 projXZb(WorldObjectPos.x - Target->WorldObjectPos.x, WorldObjectPos.z - Target->WorldObjectPos.z);
    D3DXVECTOR2 projXZa(projXZb.x, 0);
    float angleXZ = acosf(D3DXVec2Length(&projXZa) / D3DXVec2Length(&projXZb));
    RotateLR(-angleXZ);
    D3DXMatrixRotationZ(&tRotate, Angle);
    D3DXMATRIX tFinal = tToCenter * tRotate * tToOriginal;
    D3DXVec3TransformCoord(&this->WorldObjectPos, &this->WorldObjectPos, &tFinal);
    RotateLR(angleXZ);
}


void CD3SCamera::RotateLR(float Angle) {
    D3DXMATRIX tToCenter,tRotate, tToOriginal;
    D3DXMatrixTranslation(&tToCenter, -Target->WorldObjectPos.x,-Target->WorldObjectPos.y,-Target->WorldObjectPos.z);
    D3DXMatrixRotationY(&tRotate, Angle);
    D3DXMatrixTranslation(&tToOriginal, Target->WorldObjectPos.x,Target->WorldObjectPos.y,Target->WorldObjectPos.z);
    D3DXMATRIX tFinal = tToCenter * tRotate * tToOriginal;
    D3DXVec3TransformCoord(&this->WorldObjectPos, &this->WorldObjectPos, &tFinal);
}

bool CD3SCamera::LoadXML(CXMLNode *Node, CRenderSettings &Settings) {
    CBase3DObject::LoadXML(Node, Settings);
    Ortho = false;
    Node->GetAttributeValueB("ORTHOGONAL", Ortho);
    FOV = 45.0f;
    Node->GetAttributeValueF("FOV", FOV);
    NearClip = 1.0f;
    Node->GetAttributeValueF("NEARCLIP", NearClip);
    FarCLip = 1000.0f;
    Node->GetAttributeValueF("FARCLIP", FarCLip);
    TargetID = -1;
    Node->GetAttributeValueI("TARGETID", TargetID);
    return true;
}

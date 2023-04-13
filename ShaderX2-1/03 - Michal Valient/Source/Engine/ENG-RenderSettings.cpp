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


// ENG-RenderSettings.cpp: implementation of the CRenderSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-RenderSettings.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderSettings::CRenderSettings() {
    RenderID = 0;
    RenderPass = 0;
    CameraSource.x = 0.0;
    CameraSource.y = 0.0;
    CameraSource.z = -150;
    CameraTarget.x = 0;
    CameraTarget.y = 0;
    CameraTarget.z = 0;
    CameraUpVector.x = 0;
    CameraUpVector.y = 1;
    CameraUpVector.z = 0;
    CameraFieldOfView = D3DX_PI / 2.0f;
    CameraAspectRatio = 4.0f / 3.0f;
    CameraZNear = (float)0.001;
    CameraZFar = 1000;
    GeometryDetail = 1.0;
    TextureDetail = 1.0;
    RenderTime = 0.0;
    PrevRenderTime = 0.0;
    TimesDelta = 0.0;
    Flags = 0;
}

CRenderSettings::~CRenderSettings() {
}

void CRenderSettings::ComputeViewMatrix() {
    D3DXMatrixLookAtLH((D3DXMATRIX *)&ViewMatrix, (D3DXVECTOR3 *)&CameraSource, (D3DXVECTOR3 *)&CameraTarget, (D3DXVECTOR3 *)&CameraUpVector);
}

void CRenderSettings::ComputeProjectionMatrix() {
    D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)&ProjectionMatrix, CameraFieldOfView, CameraAspectRatio, CameraZNear, CameraZFar);
}

void CRenderSettings::ComputeCameraMatrix() {
    ComputeViewMatrix();
    ComputeProjectionMatrix();
    D3DXMatrixMultiply((D3DXMATRIX *)&CameraTransformMatrix, (D3DXMATRIX *)&ViewMatrix, (D3DXMATRIX *)&ProjectionMatrix);
}

void CRenderSettings::Clone(CRenderSettings &Original) {
    memcpy(this,&Original, sizeof(CRenderSettings));
}

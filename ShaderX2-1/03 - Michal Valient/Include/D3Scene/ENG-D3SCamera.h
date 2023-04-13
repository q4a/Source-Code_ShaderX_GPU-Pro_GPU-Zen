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
#include "eng-base3dobject.h"

class CXMLNode;

class CD3SCamera : public CBase3DObject {
protected:
    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
    virtual ~CD3SCamera(void);
public:
    bool            Ortho;
    float           FOV;
    float           NearClip;
    float           FarCLip;
    int             TargetID;
    CBase3DObject   *Target;
    D3DXVECTOR3     Up;     //Vector to UP

    CD3SCamera(char *ObjName,CEngine *OwnerEngine);

    virtual void *Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext);
    
    ///This implementation Calls Target's PrepareGeometry (if not null).
    ///Then prepares own Geometry by calling ancestor's PrepareGeometry.
    virtual void PrepareBoundingGeometry(CRenderSettings &RenderInfo);
    virtual void PrepareGeometry(CRenderSettings &RenderInfo);

    ///Instructs camera to apply it's settings to rendersettings.
    virtual void Apply(CRenderSettings &Settings);

    ///Camera manipulation
    ///Recomputes UP vector and other thingies.
    virtual void Recompute();
    ///Rotate Yaw Pitch and Roll
    virtual void RotateLR(float Angle);
    virtual void RotateUD(float Angle);
    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CD3SCamera);
    MAKE_CLSNAME("CD3SCamera");
    MAKE_DEFAULTGARBAGE()
};

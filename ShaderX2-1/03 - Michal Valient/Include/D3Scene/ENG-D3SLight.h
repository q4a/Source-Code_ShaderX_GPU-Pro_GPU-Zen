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

class CFrustum;
class CXMLNode;
class CBaseTexture;
class CRootClassArray;
class CBaseMaterial;

class CD3SLight : public CBase3DObject {
private:
    CFrustum        *Frustum;
    D3DXMATRIX      SpotProjection;
    D3DXMATRIX      SpotView;
    CBaseMaterial   *ShadowBuffer;  ///Material for no output
protected:
    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
    virtual ~CD3SLight(void);
public:
    ///Type of light
    static const int TTypeOmni = 1;
    static const int TTypeSpot = 2;
    static const int TTypeDirect = 3;

    ///Type of decay
    static const int TDecayNone = 1;
    static const int TDecayInverse = 2;
    static const int TDecayInverseSquare = 3;

    CBaseTexture    *SpotlightTexture;
    
    //Shadow related
    CBaseTexture    *ShadowTexture;
    ID3DXRenderToSurface *RenderToShadow;
    CRootClassArray *LitObjects;            //objects lit by this light.

    int             Type;       //convert from Flex types
    float           Hotspot;
    float           Intensity;
    float           Contrast;
    bool            UseAttenuationNear;
    bool            UseAttenuationFar;
    float           AttenuationNearStart;
    float           AttenuationNearEnd;
    float           AttenuationFarStart;
    float           AttenuationFarEnd;
    int             DecayType;  //convert from Flex types
    int             ShadowMapSize;
    int             TargetID;
    CBase3DObject  *Target;

    D3DXCOLOR       LightColor;

    D3DXMATRIX      SpotViewProjection;

    CD3SLight(char *ObjName,CEngine *OwnerEngine);

    virtual void *Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext);
    
    ///This implementation Calls Target's PrepareGeometry (if not null).
    ///Then prepares own Geometry by calling ancestor's PrepareGeometry.
    virtual void PrepareBoundingGeometry(CRenderSettings &RenderInfo);
    virtual void PrepareGeometry(CRenderSettings &RenderInfo);

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    virtual bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    //Specific light functions
    bool TestObjectLit(CBase3DObject *Obj);
    void AddLitObject(CBase3DObject *Obj);
    bool RenderShadowBuffer(CRenderSettings &Settings);

    MAKE_CLSCREATE(CD3SLight);
    MAKE_CLSNAME("CD3SLight");
    MAKE_DEFAULTGARBAGE()
};

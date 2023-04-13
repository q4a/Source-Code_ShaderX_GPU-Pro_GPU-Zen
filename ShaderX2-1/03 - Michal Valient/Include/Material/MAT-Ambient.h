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

#include "material\mat-dx8material.h"

class CBaseTexture;
class CMString;
class CMaterialAmbient;

class CMaterialAmbientContext : public CDX8MaterialContext {
    friend class CMaterialAmbient;
protected:
    static const int       PID_AmbientMultiplier = 0;   //Code = "AmbientMultiplier"
    static const int       PID_AmbientColor = 1;   //Code = "AmbientColor"
    static const int       PID_AmbientTexture = 2;      //Code = "AmbientTexture"
    static const int       PID_SelfIllumOn = 3;      //Code = "SelfIllumOn"
    static const int       PID_SelfIllumColor = 4;      //Code = "SelfIllumColor"
    static const int       PID_SelfIllumValue = 5;      //Code = "SelfIllumValue"
    static const int       PID_DiffuseMultiplier = 6;      //Code = "DiffuseMultiplier"
    static const int       PID_DiffuseColor = 7;      //Code = "DiffuseColor"
    static const int       PID_DiffuseTexture = 8;      //Code = "DiffuseTexture"

    D3DXVECTOR4     CachedAmbMul;
    D3DXVECTOR4     CachedDiffMul;
    D3DXVECTOR4     CachedConstant;
    bool            CacheDone;
public:
    float           AmbientMultiplier;
    D3DXVECTOR4     AmbientColor;
    CBaseTexture    *AmbientTexture;
    bool            SelfIllumOn;
    D3DXVECTOR4     SelfIllumColor;
    float           SelfIllumValue;
    float           DiffuseMultiplier;
    D3DXVECTOR4     DiffuseColor;
    CBaseTexture    *DiffuseTexture;

    CMaterialAmbientContext();
    virtual ~CMaterialAmbientContext();
    virtual int QueryProperty(const char *PropertyName);
    virtual bool SetProperty(int PropertyID, const void *Property);
};

class CMaterialAmbient : public CDX8Material {
protected:
    CMaterialAmbientContext *CurrentContext;

    IDirect3DVertexDeclaration9 *VSDeclaration;
    IDirect3DPixelShader9  *AmbientPS;
    IDirect3DVertexShader9 *AmbientVS;

    CBaseTexture    *DefaultTexture;
    CBaseTexture    *DefaultEmisive;

    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);

    void DeleteShaders();
    void DeleteTextures();

    virtual ~CMaterialAmbient(void);

    //Specific set / update methods
    bool SetCommon(CRenderSettings &RenderSettings);
    bool UpdateAmbient(CRenderSettings &RenderSettings);

public:
    virtual DWORD GetPassCount();
    virtual void SetPassCount(DWORD PassCount);
    virtual CBaseMaterialContext *CreateContext(CBase3DObject *Obj);
    virtual void DeleteContext(CBaseMaterialContext *Context);
    virtual bool SetContext(CBaseMaterialContext *Context);
    virtual bool Set(CRenderSettings &RenderSettings);
    virtual bool SetPass(CRenderSettings &RenderSettings, DWORD PassNumber);
    virtual bool UnSet(CRenderSettings &RenderSettings);

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    ///Const, dest & more
    CMaterialAmbient(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSCREATE(CMaterialAmbient);
    MAKE_CLSNAME("CMaterialAmbient");
    MAKE_DEFAULTGARBAGE()
};

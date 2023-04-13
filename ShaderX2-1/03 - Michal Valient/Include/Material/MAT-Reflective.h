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
class CRootClassArray;

class CMaterialReflectiveContext : public CDX8MaterialContext {
protected:
    static const int       PID_DiffuseMultiplier = 0;       //Code = "DiffuseMultiplier"
    static const int       PID_DiffuseColor = 1;            //Code = "DiffuseColor"
    static const int       PID_DiffuseTexture = 2;          //Code = "DiffuseTexture"
    static const int       PID_NormalTexture = 3;          //Code = "NormalTexture"
    static const int       PID_SpecularColor = 5;           //Code = "SpecularColor"

public:
    D3DXVECTOR4     SpecularColor;
    float           DiffuseMultiplier;
    D3DXVECTOR4     DiffuseColor;
    CBaseTexture    *DiffuseTexture;
    CBaseTexture    *NormalTexture;

    CMaterialReflectiveContext();
    virtual ~CMaterialReflectiveContext();
    virtual int QueryProperty(const char *PropertyName);
    virtual bool SetProperty(int PropertyID, const void *Property);
};

class CMaterialReflective : public CDX8Material {
protected:
    CMaterialReflectiveContext *CurrentContext;

    IDirect3DVertexDeclaration9 *VSDeclaration;
    IDirect3DVertexShader9      *CreatedVertexShader;
    IDirect3DPixelShader9       *CreatedPixelShader;

    DWORD   CurrentPass;

    CBaseTexture    *DefaultTexture;
    CBaseTexture    *DefaultNormalMap;
    CBaseTexture    *EnvCubeTexture;
    CBaseTexture    *DefaultFresnelTexture;

    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);

    void DeleteShaders();
    void DeleteTextures();

    virtual ~CMaterialReflective(void);

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
    CMaterialReflective(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSCREATE(CMaterialReflective);
    MAKE_CLSNAME("CMaterialReflective");
    MAKE_DEFAULTGARBAGE()
};

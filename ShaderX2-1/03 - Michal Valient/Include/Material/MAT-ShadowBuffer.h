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

#include "material\mat-basematerial.h"

class CBaseTexture;
class CMString;
class CMaterialShadowBuffer;
class CBaseShader;

class CMaterialShadowBufferContext : public CBaseMaterialContext {
    friend class CMaterialShadowBuffer;
};

class CMaterialShadowBuffer : public CBaseMaterial {
protected:
    CMaterialShadowBufferContext *CurrentContext;

    IDirect3DVertexDeclaration9 *VSDeclaration;
    CBaseShader     *VertexShader;
    CBaseShader     *PixelShader;

    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);

    void DeleteShaders();

    virtual ~CMaterialShadowBuffer(void);
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
    CMaterialShadowBuffer(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSCREATE(CMaterialShadowBuffer);
    MAKE_CLSNAME("CMaterialShadowBuffer");
    MAKE_DEFAULTGARBAGE()
};

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
#include "mat-basematerial.h"

class CMString;
class CRenderSettings;
class CXMLNode;

class CDX8MaterialContext : public CBaseMaterialContext {
public:
    ///QueryProperty queries for existence of a property.
    ///return value is ID of the property, used in subsequent calls
    ///return value less than zero means, that property is not supported.
    virtual int QueryProperty(const char *PropertyName);

    ///SetProperty sets property of material context.
    ///false returned means, that property is not supported
    virtual bool SetProperty(int PropertyID, const void *Property);
};

class CDX8Material : public CBaseMaterial {
protected:
    virtual ~CDX8Material(void);
public:
    CDX8Material(char *ObjName,CEngine *OwnerEngine);

    ///LoadVS loads a vertex shader.
    ///returns ID of the shader
    IDirect3DVertexShader9 *LoadVS_Bin(int ResourceID, CRenderSettings &RenderSettings);
    IDirect3DVertexShader9 *LoadVS_Bin(CMString &ResourceName, CRenderSettings &RenderSettings);

    IDirect3DVertexShader9 *LoadVS_Src(int ResourceID, CRenderSettings &RenderSettings);
    IDirect3DVertexShader9 *LoadVS_Src(CMString &ResourceName, CRenderSettings &RenderSettings);

    ///Deletes vertex shader
    ///false returned means delete error
    bool DeleteVS(IDirect3DVertexShader9 *ShaderHandle);

    ///LoadPS loads a pixel shader.
    ///returns ID of the shader
    IDirect3DPixelShader9 *LoadPS_Bin(int ResourceID, CRenderSettings &RenderSettings);
    IDirect3DPixelShader9 *LoadPS_Bin(CMString &ResourceName, CRenderSettings &RenderSettings);

    IDirect3DPixelShader9 *LoadPS_Src(int ResourceID, CRenderSettings &RenderSettings);
    IDirect3DPixelShader9 *LoadPS_Src(CMString &ResourceName, CRenderSettings &RenderSettings);

    ///Deletes pixel shader
    ///false returned means delete error
    bool DeletePS(IDirect3DPixelShader9 *ShaderHandle);

    virtual CDX8MaterialContext *CreateContextFromXML(CXMLNode *Node, CBase3DObject *Object, CRenderSettings &Settings);

    MAKE_CLSNAME("CDX8Material");
};

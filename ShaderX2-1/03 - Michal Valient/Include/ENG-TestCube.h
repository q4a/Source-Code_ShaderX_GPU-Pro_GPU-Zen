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


// ENG-TestCube.h: interface for the CTestCube class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ENG-Base3DObject.h"

//############################################################################
//Forward declarations
class CMesh;
class CTexture2D;

class CTestCube : public CBase3DObject {
    double  prevTime;
    CMesh  *Mesh;
    CTexture2D *Texture;
    DWORD   StateBlock;

    virtual void DoCacheIn(CRenderSettings &RenderSettings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);
	virtual ~CTestCube();
public:
	CTestCube(char *pObjName,CEngine *pOwnerEngine);

    virtual void *Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext);
    virtual void PrepareBoundingGeometry(CRenderSettings &RenderInfo);
    virtual void PrepareGeometry(CRenderSettings &RenderInfo);
    virtual CBase3DObject &Clone(bool CloneGeometry);

    virtual void DeviceLost(CRenderSettings &Settings);
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    MAKE_CLSCREATE(CTestCube);
    MAKE_CLSNAME("CTestCube");
    MAKE_DEFAULTGARBAGE()
};


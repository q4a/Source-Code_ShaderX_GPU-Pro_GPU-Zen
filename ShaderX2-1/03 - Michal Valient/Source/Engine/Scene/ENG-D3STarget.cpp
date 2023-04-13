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
#include "d3scene\eng-d3starget.h"

CD3STarget::CD3STarget(char *ObjName,CEngine *OwnerEngine) : CBase3DObject(ObjName, OwnerEngine) {
    Visible = false;
    SetFilled(TRUE);
    SetLost(FALSE);
}

CD3STarget::~CD3STarget(void) {
}

void CD3STarget::PrepareGeometry(CRenderSettings &RenderInfo) {
}

void *CD3STarget::Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) {
    return NULL;
}

void CD3STarget::DoCacheIn(CRenderSettings &RenderSettings) {
}

void CD3STarget::DoCacheOut(CRenderSettings &RenderSettings) {
}

bool CD3STarget::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    SetFilled(TRUE);
    return TRUE;
}

bool CD3STarget::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}

void CD3STarget::DeviceLost(CRenderSettings &Settings) {
    SetLost(FALSE);
}

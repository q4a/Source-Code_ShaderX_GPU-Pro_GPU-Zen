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
#include "eng-enginedefaults.h"

#include "nommgr.h"
#include <D3dx9.h>
#include "mmgr.h"

CEngineDefaults::CEngineDefaults(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    //Vertex buffer defaults
    VBuf_Usage = D3DUSAGE_WRITEONLY;
//    VBuf_Usage = D3DUSAGE_SOFTWAREPROCESSING;
    VBuf_Pool = D3DPOOL_MANAGED;

    //Index buffer defaults
    IBuf_Usage = D3DUSAGE_WRITEONLY;
//    IBuf_Usage = D3DUSAGE_SOFTWAREPROCESSING;
    IBuf_Format = D3DFMT_INDEX16;
    IBuf_Pool = D3DPOOL_MANAGED;

    //2D Texture creation defaults
    Txt2D_Width = D3DX_DEFAULT;
    Txt2D_Height = D3DX_DEFAULT;
    Txt2D_MipLevels = D3DX_DEFAULT;
    Txt2D_Usage = 0;
    Txt2D_Format = D3DFMT_UNKNOWN;
    Txt2D_Pool = D3DPOOL_MANAGED;
    Txt2D_Filter = D3DX_FILTER_TRIANGLE;
    Txt2D_MipFilter = D3DX_FILTER_TRIANGLE;
    Txt2D_ColorKey = 0;

    //Cube Texture creation defaults
    TxtCube_Size = D3DX_DEFAULT;
    TxtCube_MipLevels = D3DX_DEFAULT;
    TxtCube_Usage = 0;
    TxtCube_Format = D3DFMT_UNKNOWN;
    TxtCube_Pool = D3DPOOL_MANAGED;
    TxtCube_Filter = D3DX_FILTER_TRIANGLE;
    TxtCube_MipFilter = D3DX_FILTER_TRIANGLE;
    TxtCube_ColorKey = 0;
}

CEngineDefaults::~CEngineDefaults(void) {
}

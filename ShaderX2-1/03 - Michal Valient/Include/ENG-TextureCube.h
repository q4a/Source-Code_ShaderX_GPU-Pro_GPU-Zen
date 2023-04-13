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

#include "ENG-BaseTexture.h"

//############################################################################
//Forward declarations


class CTextureCube : public CBaseTexture {
protected:
    bool                OverrideLoadParams;
    UINT                OLP_Size;   //Width and height
    UINT                OLP_MipLevels;
    DWORD               OLP_Usage;
    D3DFORMAT           OLP_Format;
    D3DPOOL             OLP_Pool;
    DWORD               OLP_Filter;
    DWORD               OLP_MipFilter;
    D3DCOLOR            OLP_ColorKey;

public:
    UINT                Size;
    D3DMULTISAMPLE_TYPE MultiSampleType;

    ///Load data into resource
    ///This implementation uses standard D3DX method D3DXCreateCubeTextureFromFileInMemoryEx
    ///to load texture.
    ///@param LoadIndex - index of data in FileManager.
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);

    ///Resets loading parameters override
    virtual void ResetLoadParams();

    ///Sets loading parameters override
    ///Parameter descriptions are identical to those in D3DXCreateCubeTextureFromFileInMemoryEx function.
    virtual void SetLoadParams(UINT Size, UINT MipLevels, DWORD Usage, D3DFORMAT Format, 
        D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey);

    ///This implementation does nothing
    virtual bool Update(CRenderSettings &RenderSettings);
    
    CTextureCube(char *sObjName, CEngine *pOwnerEngine);

    MAKE_CLSCREATE(CTextureCube);
    MAKE_CLSNAME("CTextureCube");
    MAKE_DEFAULTGARBAGE()
};

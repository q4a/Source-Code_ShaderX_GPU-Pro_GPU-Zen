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

#include "ENG-BaseResource.h"
#include <D3d9.h>

//############################################################################
//Forward declarations

class CBaseTexture : public CBaseResource {
protected:
    virtual void DoCacheIn(CRenderSettings &Settings);
    virtual void DoCacheOut(CRenderSettings &Settings);
    virtual ~CBaseTexture();
public:
    D3DPOOL                 Pool;       //D3DPOOL_XXXX parameter
    DWORD                   Usage;      //D3DUSAGE_XXXX parameter
    D3DFORMAT               Format;     //Format of texture
    int                     LevelCount; //how many mip map levels we have.

    IDirect3DBaseTexture9  *Texture;
    PALETTEENTRY           *Palette;

    //D3DPOOL_XXXX parameter
    D3_INLINE D3DPOOL GetPool() { return Pool; }
    //D3DUSAGE_XXXX parameter
    D3_INLINE UINT GetUsage() { return Usage; }
    //Format of texture
    D3_INLINE D3DFORMAT GetFormat() { return Format; }
    //how many mip map levels we have.
    D3_INLINE int GetLevelCount() { return LevelCount; }

    //Sended, when device was lost. If Texture!=NULL and it needs to be killed, we do it for safe.
    //Descendants should do all other extra processing here.
    virtual void DeviceLost(CRenderSettings &Settings);

    CBaseTexture(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSNAME("CBaseTexture");
};

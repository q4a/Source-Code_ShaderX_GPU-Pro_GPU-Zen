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

#include "ENG-ResourceManager.h"
#include <D3d9.h>

//############################################################################
//Forward declarations
class CBaseTexture;

class CTextureManager : public CResourceManager {
protected:
    class CTextureInfo : public CRootClass {
    public:
        DWORD                   Stage;
        IDirect3DBaseTexture9  *Textures;

        CTextureInfo();
        ~CTextureInfo();
    };

    CHashTable2 *GuardTextures;
public:
    CTextureManager(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTextureManager();
    virtual void DeviceLost(CRenderSettings &Settings);

    MAKE_CLSCREATE(CTextureManager);
    MAKE_CLSNAME("CTextureManager");
    MAKE_DEFAULTGARBAGE();

    ///GUARDBuffer DX8::SetTexture method
    bool SetTexture(DWORD Stage, CBaseTexture *Texture);
};

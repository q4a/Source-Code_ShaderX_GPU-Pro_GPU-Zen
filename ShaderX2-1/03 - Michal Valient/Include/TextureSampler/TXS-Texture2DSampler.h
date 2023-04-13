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

#include "baseclass.h"

///Texture sampler classes are used to create textures from functions
///i.e. Fresnel term or specular power term.
class CTexture2DSampler : public CBaseClass {
protected:
    static void WINAPI TexFill(D3DXVECTOR4* pOut, CONST D3DXVECTOR2* pTexCoord, CONST D3DXVECTOR2* pTexelSize, LPVOID pData);

    UINT    Width;
    UINT    Height;
    UINT    MipLevels;

    IDirect3DTexture9 *FilledTexture;
public:
    ///Override this function in descendants to get results you want.
    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    //Fill and save texture to file in parameter (as .dds)
    virtual void FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels);
    
    //Fill texture specified as parameter
    virtual void FillTexture(IDirect3DTexture9 *Texture);

    CTexture2DSampler(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler(void);

    MAKE_CLSCREATE(CTexture2DSampler);
    MAKE_CLSNAME("CTexture2DSampler");
    MAKE_DEFAULTGARBAGE()
};

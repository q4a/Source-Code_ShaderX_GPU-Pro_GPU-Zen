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
#include "txs-texture2dsampler.h"

///Creates complete shininess power texture
///in form f(x,y) = (x,y, pow(x,sy), pow(x,sy))
///where sy = MinPower + y * (MaxPower - MinPower)
class CTexture2DSampler_SpecularPower : public CTexture2DSampler {
public:
    float   MinPower;
    float   MaxPower;

    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    CTexture2DSampler_SpecularPower(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_SpecularPower(void);

    MAKE_CLSCREATE(CTexture2DSampler_SpecularPower);
    MAKE_CLSNAME("CTexture2DSampler_SpecularPower");
    MAKE_DEFAULTGARBAGE()
};

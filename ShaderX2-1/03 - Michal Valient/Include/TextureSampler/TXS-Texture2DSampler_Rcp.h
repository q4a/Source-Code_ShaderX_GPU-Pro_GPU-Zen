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

///Creates other part of Cook-Torrance lighthing - Fresnel term divided by (n.v) and Pi
///CTF(x,y) = F(x, y) / (x * Pi)
///where x is assumed to be dp3(n,v) and y = index of refraction
///result is in form f(x,y) = (CTF1, CTF2, CTF3, CTF4) where
///
///and CTF1 is CTF in range 0..1
///    CTF2 is CTF in range 1..2
///    CTF3 is CTF in range 2..3
///    CTF4 is CTF in range 3..4
class CTexture2DSampler_Rcp : public CTexture2DSampler {
protected:
public:
    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    CTexture2DSampler_Rcp(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_Rcp(void);

    MAKE_CLSCREATE(CTexture2DSampler_Rcp);
    MAKE_CLSNAME("CTexture2DSampler_Rcp");
    MAKE_DEFAULTGARBAGE()
};

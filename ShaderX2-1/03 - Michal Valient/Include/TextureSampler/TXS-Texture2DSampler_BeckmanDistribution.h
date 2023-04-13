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

///Creates beckman distribution texture for Cook-Tortance specular model
///in form f(x,y) = (D1, D2, D3, D4).
///where x is assumed to be cosine of an angle _A_
///and y is meant as _m_ from ...
///
///D(A, m) = (e^(-(tan A / m)^2)) / (m^2 * (cos A)^4)
///
///and D1 is D in range 0..1
///    D2 is D in range 1..2
///    D3 is D in range 2..3
///    D4 is D in range 3..4
class CTexture2DSampler_BeckmanDistribution : public CTexture2DSampler {
protected:
    double MinUsedRough;
    double MinUsedCos;
public:
    float   MinRoughness;
    float   MaxRoughness;

    virtual void FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize);

    CTexture2DSampler_BeckmanDistribution(char *ObjName,CEngine *OwnerEngine);
    virtual ~CTexture2DSampler_BeckmanDistribution(void);

    MAKE_CLSCREATE(CTexture2DSampler_BeckmanDistribution);
    MAKE_CLSNAME("CTexture2DSampler_BeckmanDistribution");
    MAKE_DEFAULTGARBAGE()
};

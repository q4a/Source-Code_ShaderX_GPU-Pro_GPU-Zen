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
#include "texturesampler\TXS-Texture2DSampler_BeckmanDistribution.h"

///Override this one
void CTexture2DSampler_BeckmanDistribution::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    double Math_E = 2.718281828459045235360287471352662497757247093699959574966967628;
    double Math_Pi = 3.141592653589793238462643383279502884197169399375105820974944592;
    //double Math_Pi = 1.0;

    double PositionX = TexCoord.x + TexelSize.x;
    PositionX = min(1.0, max(0.0, PositionX));
    double PositionY = TexCoord.y + TexelSize.y;
    PositionY = min(1.0, max(0.0, PositionY));

    double Roughness = MinRoughness + (PositionY) * (MaxRoughness - MinRoughness);
    if (Roughness>0) {
        MinUsedRough = min(MinUsedRough, Roughness);
    } else {
        Roughness = MinUsedRough;
    }


    double Roughness2 = Roughness * Roughness;
    double Cos2A = PositionX * PositionX;
    double Cos4A = PositionX * PositionX * PositionX * PositionX;
    //n.h^4x = r.v^x!!!
    //double Cos2A = sqrt(PositionX);
    //double Cos4A = PositionX;

    if (Cos4A>0) {
        MinUsedCos = min(MinUsedCos, Cos4A);
    } else {
        Cos4A = MinUsedCos;
    }

    double Exponent = 0.0 - ((1.0 - Cos2A) / (Roughness2 * Cos2A));
    double Divider = Roughness2 * Cos4A * Math_Pi;
    Divider = Divider * Math_Pi;    //added to lower the intensity
    double Result = pow(Math_E, Exponent) / Divider;
    if (1) {    //if we want it expanded to 0-4 range
        float RealResult1 = min(1.0f, max(0.0f, (float)Result));
        Result = Result - 1.0;
        float RealResult2 = min(1.0f, max(0.0f, (float)Result));
        Result = Result - 1.0;
        float RealResult3 = min(1.0f, max(0.0f, (float)Result));
        Result = Result - 1.0;
        float RealResult4 = min(1.0f, max(0.0f, (float)Result));
        Output = D3DXVECTOR4(RealResult1, RealResult2, RealResult3, RealResult4);
    } else {
        Output = D3DXVECTOR4((float)PositionX, (float)PositionY, (float)Result, (float)Result);
    }
}

CTexture2DSampler_BeckmanDistribution::CTexture2DSampler_BeckmanDistribution(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
    MinRoughness = 0.0f;
    MaxRoughness = 1.0f;
    MinUsedRough = MinUsedCos = 100.0;
}

CTexture2DSampler_BeckmanDistribution::~CTexture2DSampler_BeckmanDistribution(void) {
}

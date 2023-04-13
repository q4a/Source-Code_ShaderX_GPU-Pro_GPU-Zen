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
#include "texturesampler\txs-texture2dsampler_fresnel.h"

///Override this one
void CTexture2DSampler_Fresnel::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    float FresnelForm = powf(1.0f - TexCoord.x - TexelSize.x, 5.0f);
    if (this->Height > 1) {
        Output = D3DXVECTOR4(TexCoord.x + TexelSize.x, TexCoord.y + TexelSize.y, FresnelForm, FresnelForm);
    } else {
        Output = D3DXVECTOR4(TexCoord.x + TexelSize.x, FresnelForm, FresnelForm, FresnelForm);
    }
}

CTexture2DSampler_Fresnel::CTexture2DSampler_Fresnel(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
}

CTexture2DSampler_Fresnel::~CTexture2DSampler_Fresnel(void) {
}

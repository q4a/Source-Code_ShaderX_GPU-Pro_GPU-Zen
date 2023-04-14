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
#include "texturesampler\txs-texture2dsampler_specularmaps.h"
#include "SC-MString.h"
#include "MGR-DirectGraphicsManager.h"
#include "ENG-Engine.h"

///Override this one
void CTexture2DSampler_SpecularMaps::FillTexel(D3DXVECTOR4 &Output, const D3DXVECTOR2 &TexCoord, const D3DXVECTOR2 &TexelSize) {
    if (this->Height > 1) {
        Output = D3DXVECTOR4(TexCoord.x + TexelSize.x, TexCoord.y + TexelSize.y, powf(TexCoord.x + TexelSize.x, CurrentPower), powf(TexCoord.x + TexelSize.x, CurrentPower));
    } else {
        Output = D3DXVECTOR4(TexCoord.x + TexelSize.x, powf(TexCoord.x + TexelSize.x, CurrentPower), powf(TexCoord.x + TexelSize.x, CurrentPower), powf(TexCoord.x + TexelSize.x, CurrentPower));
    }
}

CTexture2DSampler_SpecularMaps::CTexture2DSampler_SpecularMaps(char *ObjName,CEngine *OwnerEngine) 
    : CTexture2DSampler(ObjName,OwnerEngine) {
    MinPower = 0;
    MaxPower = 100;
}

CTexture2DSampler_SpecularMaps::~CTexture2DSampler_SpecularMaps(void) {
}

void CTexture2DSampler_SpecularMaps::FillTextureFile(char *TextureFile, UINT Width, UINT Height, UINT MipLevels) {
    int LogBig = _LOGB(this, D3_DV_GRP0_LEV0, "Starting big texture sampling for group %s", TextureFile);
    CMString newFileName;

    for (UINT i=MinPower; i<=MaxPower; i++) {
        CurrentPower = (float)i;
        newFileName.Format("%s_%u.dds", TextureFile, i);
        int LogE = _LOGB(this, D3_DV_GRP0_LEV4, "Starting sample for %s", newFileName.c_str());
        IDirect3DDevice9 *Device = this->OwnerEngine->GetGraphicsManager()->Direct3DDevice;
        IDirect3DTexture9 *Texture = NULL;
        HRESULT res = D3DXCreateTexture(Device, Width, Height, MipLevels, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &Texture);
        if (res!=D3D_OK) {
            _WARN(this, ErrMgr->TNoSupport, "D3DXCreateTexture failed to create texture");
        } else {
            FillTexture(Texture);
            res = D3DXSaveTextureToFile(newFileName.c_str(), D3DXIFF_DDS, Texture, NULL);
            if (res!=D3D_OK) {
                _WARN(this, ErrMgr->TNoSupport, "D3DXSaveTextureToFile failed to save texture to %s", TextureFile);
            }
        }
        Texture->Release();
        _LOGE(LogE, "Texture sampling ended");
    }
    _LOGE(LogBig, "Ending big texture sampling");
}

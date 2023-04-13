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
#include "ENG-BASETEXTURE.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBaseTexture::CBaseTexture(char *ObjName,CEngine *OwnerEngine) : CBaseResource(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating texture");
    Format = D3DFMT_A8R8G8B8;
    Texture = NULL;
    Pool = (D3DPOOL)0;
    Usage = 0;
    LevelCount = 0;
    Palette = NULL;
}

CBaseTexture::~CBaseTexture() {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Destroying texture with ResourceID %i",ResourceID);
    if (ReferenceCount>1) _LOG(this,D3_DV_GRP2_LEV0,"Warning - Texture reference count is %i",ReferenceCount);
    if (Texture!=NULL) {
        ULONG rCnt = Texture->Release();
        if (rCnt!=0) _LOG(this,D3_DV_GRP2_LEV0,"Warning - IDirect3DBaseTexture9 reference count is %i",rCnt);
    }
    if (Palette!=NULL) MemMgr->FreeMem(Palette);
    _LOGE(logE,"Destroyed texture %i",ResourceID);
}

void CBaseTexture::DoCacheIn(CRenderSettings &RenderSettings) {
    CBaseResource::DoCacheIn(RenderSettings);
    if (Texture!=NULL) Texture->PreLoad();
}

void CBaseTexture::DoCacheOut(CRenderSettings &Settings) {
    if (Texture!=NULL) {
        UINT rCnt = Texture->Release();
        if (rCnt!=0) _LOG(this,D3_DV_GRP2_LEV2,"Warning - IDirect3DBaseTexture9 reference count is %i",rCnt);
        Texture = NULL;
    }
    if (Palette!=NULL) {
        MemMgr->FreeMem(Palette);
        Palette = NULL;
    }
    SetFilled(false);
}

void CBaseTexture::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Texture %i - Sending device lost", ResourceID);
    if (Pool != D3DPOOL_MANAGED && Pool != D3DPOOL_SYSTEMMEM && Texture!=NULL) { //it cannot be restored, we are killing texture.
        _LOG(this,D3_DV_GRP2_LEV0,"Setting texture as lost");
        DoCacheOut(Settings);
        SetLost(true);
    } else {
        _LOG(this,D3_DV_GRP2_LEV0,"Not marked as lost");
    }
    _LOGE(logE,"Texture - Sended device lost");
}


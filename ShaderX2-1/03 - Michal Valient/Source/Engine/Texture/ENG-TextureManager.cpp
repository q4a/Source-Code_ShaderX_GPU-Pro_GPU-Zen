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


// ENG-TextureManager.cpp: implementation of the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-TextureManager.h"

#include "ENG-BaseTexture.h"
#include "ENG-Engine.h"
#include "MGR-DirectGraphicsManager.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "SC-HashTable2.h"

CTextureManager::CTextureManager(char *ObjName,CEngine *OwnerEngine) : CResourceManager(ObjName,OwnerEngine) {
    GuardTextures = new CHashTable2(TRUE,0);
}

CTextureManager::~CTextureManager() {
    delete GuardTextures;
}

///GUARDBuffer DX8::SetTexture method
bool CTextureManager::SetTexture(DWORD Stage, CBaseTexture *Texture) {
    if (Texture == NULL) {  //Setting texture stage to NULL
        CTextureInfo *TInfo = (CTextureInfo *)GuardTextures->Get(Stage);
        if (TInfo != NULL) {    //If found guarded texture erase it, else do nothing
            _LOG(this,D3_DV_GRP0_LEV4,"SetTexture - NULL at stage %u",TInfo->Stage);
            if (this->OwnerEngine->GetGraphicsManager()->Direct3DDevice->SetTexture(Stage,NULL) != D3D_OK) {
                _WARN(this,ErrMgr->TNoSupport,"SetTexture - FAILED to NULL texture stage %u",Stage);
                return FALSE;
            } else {
                TInfo->Textures = NULL;
                GuardTextures->Delete(Stage);
                return TRUE;
            }
        }
        return TRUE;
    } else {
        CTextureInfo *TInfo = (CTextureInfo *)GuardTextures->Get(Stage);
        if (TInfo == NULL) {    //If new stage is guarded, create it 
            TInfo = new CTextureInfo;
            TInfo->Stage = Stage;
            GuardTextures->Add(Stage, TInfo);
        }
        if (Texture->Texture != TInfo->Textures) {
            if (this->OwnerEngine->GetGraphicsManager()->Direct3DDevice->SetTexture(Stage,Texture->Texture) != D3D_OK) {
                _WARN(this,ErrMgr->TNoSupport,"SetTexture - FAILED - TextureID %u",Texture->GetResourceID());
                TInfo->Stage = Stage;
                TInfo->Textures = NULL;
                return FALSE;
            } else {
                _LOG(this,D3_DV_GRP0_LEV4,"SetTexture - NEWVALUE - TextureID %u",Texture->GetResourceID());
                TInfo->Stage = Stage;
                TInfo->Textures = Texture->Texture;
                return TRUE;
            }
        } else {
            _LOG(this,D3_DV_GRP0_LEV4,"SetTexture - IDENTICAL - TextureID %u",Texture->GetResourceID());
            return TRUE;
        }
    }
}

void CTextureManager::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Sending device lost");
    //then unset textures
    int TextureCount = OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxTextureBlendStages;
    for (int i=0; i<TextureCount; i++) {
        SetTexture(i,NULL);
    }
    CResourceManager::DeviceLost(Settings);
    _LOGE(logE,"Sended device lost");
}


CTextureManager::CTextureInfo::CTextureInfo() {
    Textures = NULL;
    Stage = 0;
}

CTextureManager::CTextureInfo::~CTextureInfo() {
    if (Textures!=NULL) {
        _WARN("Guard","CTextureInfo",ErrMgr->TMemory,"Texture %p still referenced at stage %i", Textures, Stage);
    }
}

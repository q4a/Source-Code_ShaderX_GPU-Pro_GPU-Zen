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
#include "d3scene\eng-d3slight.h"
#include "eng-frustum.h"
#include "sc-xmlparser.h"
#include "SC-DynamicArray.h"

#include "eng-texture2d.h"
#include "ENG-TextureManager.h"
#include "ENG-Engine.h"
#include "ENG-RenderSettings.h"

#include "Material\MAT-MaterialManager.h"
#include "material\MAT-ShadowBuffer.h"

CD3SLight::CD3SLight(char *ObjName,CEngine *OwnerEngine) : CBase3DObject(ObjName, OwnerEngine) {
    Target = NULL;

    SpotlightTexture = NULL;
    Type = TTypeOmni;
    Hotspot = 120;
    Intensity = 1;
    Contrast = 1;
    UseAttenuationNear = true;
    UseAttenuationFar = true;
    AttenuationNearStart = 0.0001f;
    AttenuationNearEnd = 0.0001f;
    AttenuationFarStart = 10000.0f;
    AttenuationFarEnd = 10000.0f;
    DecayType = TDecayNone;
    LightColor.r = LightColor.g = LightColor.b = LightColor.a = 1;
    ShadowMapSize = 256;
    Visible = false;

    LitObjects = new CRootClassArray(false);
    ShadowTexture = NULL;
    RenderToShadow = NULL;
    ShadowBuffer = NULL;

    Frustum = new CFrustum(ObjName,OwnerEngine);
    SetFilled(TRUE);
    SetLost(FALSE);
}

CD3SLight::~CD3SLight(void) {
    if (ShadowTexture!=NULL) OwnerEngine->GetTextureManager()->Delete(ShadowTexture->GetResourceID());
    if (RenderToShadow!=NULL) RenderToShadow->Release();
    if (ShadowBuffer!=NULL) OwnerEngine->GetMaterialManager()->Delete(ShadowBuffer->GetResourceID());

    delete Frustum;
    delete LitObjects;
}

bool CD3SLight::RenderShadowBuffer(CRenderSettings &Settings) {
    if ((Settings.Flags & D3RS_SHADOWSENABLED)==D3RS_SHADOWSENABLED) {
        //Prepare render target texture
        if (ShadowTexture==NULL) {
            ShadowTexture = new CTexture2D(this->GetObjName(),this->OwnerEngine);
            CTexture2D *tmpTex = (CTexture2D *)ShadowTexture;
    //        if (!tmpTex->CreateTexture(Settings, 1024, 1024, 1, D3DUSAGE_RENDERTARGET, 
            if (!tmpTex->CreateTexture(Settings, ShadowMapSize, ShadowMapSize, 1, D3DUSAGE_RENDERTARGET, 
                D3DFMT_R32F, D3DPOOL_DEFAULT)) {
    //            D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT)) {
                _WARN(this, ErrMgr->TNoSupport, "Failed to create shadow surface");
            }
        }
        if (RenderToShadow==NULL) {
            CTexture2D *tmpTex = (CTexture2D *)ShadowTexture;
            HRESULT res = D3DXCreateRenderToSurface(Settings.Direct3DDevice, tmpTex->Width, tmpTex->Height, 
                tmpTex->Format, true, D3DFMT_D24S8, &RenderToShadow);
            if (FAILED(res)) {
                _WARN(this, ErrMgr->TNoSupport, "Failed to create render to surface interface");
            }
        }
        if (ShadowBuffer==NULL) {
            ShadowBuffer = (CBaseMaterial *)OwnerEngine->GetMaterialManager()->Query(D3_GRI_Mat_ShadowBuffer);
            if (ShadowBuffer == NULL) {
                ShadowBuffer = new CMaterialShadowBuffer("ShadowBuffer",this->OwnerEngine);
                ShadowBuffer->SetResourceID(D3_GRI_Mat_PixelNoOutput);
                ShadowBuffer->Load(0, *OwnerEngine->GetRenderSettings());
                OwnerEngine->GetMaterialManager()->Add(ShadowBuffer->GetResourceID(),*ShadowBuffer);
            }
        }

        //Do rendering
        IDirect3DTexture9 *shTexture = (IDirect3DTexture9 *)ShadowTexture->Texture;
        IDirect3DSurface9 *shSurface = NULL;
        HRESULT res = shTexture->GetSurfaceLevel(0, &shSurface);
        if(SUCCEEDED(res)) {
            res = RenderToShadow->BeginScene(shSurface, NULL);
            if(SUCCEEDED(res)) {
                CRenderSettings OldRS;
                OldRS.Clone(Settings);

                //Prepare frustum
                D3DXVECTOR3 Up(0.0f,1.0f,0.0f);
                Settings.CameraUpVector = Up;
                Settings.CameraSource = WorldObjectPos;
                Settings.CameraTarget = Target->WorldObjectPos;
                Settings.CameraFieldOfView = D3DXToRadian((float)Hotspot);
                Settings.CameraAspectRatio = 1.0f;
                Settings.CameraZNear = (float)AttenuationNearStart;
                Settings.CameraZFar = (float)AttenuationFarEnd;
                Settings.ComputeCameraMatrix();
                Settings.Direct3DDevice->SetTransform(D3DTS_VIEW, &Settings.ViewMatrix);
	            Settings.Direct3DDevice->SetTransform(D3DTS_PROJECTION, &Settings.ProjectionMatrix);

                Settings.Direct3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

                CBaseMaterialContext *Con = (CBaseMaterialContext *)ShadowBuffer->CreateContext(NULL);
                OwnerEngine->GetMaterialManager()->SetMaterial(ShadowBuffer, Con, 0, Settings);

                for (int j=0; j<LitObjects->GetSize(); j++) {
                    CBase3DObject *OC = (CBase3DObject *)LitObjects->GetAt(j);
                    OC->PrepareGeometry(Settings);
                    OC->Render(Settings,NULL, ShadowBuffer, Con);
                }
                
                ShadowBuffer->DeleteContext(Con);

                RenderToShadow->EndScene(D3DX_FILTER_NONE);
                Settings.Clone(OldRS);

        /* screenshot *
                CMString OutFileName;
                OutFileName.Format("C:\\_Diploma\\Shadow_%s_%06i.bmp", this->GetObjName(), Settings.RenderID);
                D3DXSaveSurfaceToFile(OutFileName.c_str(), D3DXIFF_BMP, shSurface, NULL, NULL);
        /* screenshot */
            }
            shSurface->Release();
        }
    }
    return true;
}

void CD3SLight::PrepareGeometry(CRenderSettings &RenderInfo) {

    /*
    Temporary animation of hotspot
    */
    /*
    if (DecayType>0) {
        Hotspot = Hotspot + 0.1f;
        if (Hotspot > 89.0f) {
            DecayType = -1;
            Hotspot = 89.0f;
        }
    } else {
        Hotspot = Hotspot - 0.1f;
        if (Hotspot < 1.0f) {
            DecayType = 1;
            Hotspot = 1.0f;
        }
    }
    //*/
    D3DXVECTOR3 Up(0.0f,1.0f,0.0f);
    D3DXMatrixLookAtLH(&SpotView, &WorldObjectPos, &Target->WorldObjectPos, &Up);
    D3DXMatrixPerspectiveFovLH(&SpotProjection, D3DXToRadian((float)Hotspot), 1, (float)AttenuationNearStart, (float)AttenuationFarEnd);
    D3DXMatrixMultiply(&SpotViewProjection, &SpotView, &SpotProjection);
    Frustum->CalculateFrustum(SpotView, SpotProjection);
}

bool CD3SLight::TestObjectLit(CBase3DObject *Obj) {
    bool result = Frustum->SphereInFrustum(Obj->WorldBSCenter, Obj->WorldBSRadius);
    if (result) _LOG(this, D3_DV_GRP2_LEV4, "Visible obj %s", Obj->GetObjName());
    else _LOG(this, D3_DV_GRP2_LEV4, "INVISIBLE obj %s", Obj->GetObjName());
    return result;
}

void CD3SLight::AddLitObject(CBase3DObject *Obj) {
    LitObjects->Add(Obj);
}

void *CD3SLight::Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) {
    return NULL;
}

void CD3SLight::PrepareBoundingGeometry(CRenderSettings &RenderInfo) {
    if (Target!=NULL) Target->PrepareBoundingGeometry(RenderInfo);
    CBase3DObject::PrepareBoundingGeometry(RenderInfo);
}

void CD3SLight::DoCacheIn(CRenderSettings &RenderSettings) {
}

void CD3SLight::DoCacheOut(CRenderSettings &RenderSettings) {
}

bool CD3SLight::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    SetFilled(TRUE);
    return TRUE;
}

bool CD3SLight::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}

void CD3SLight::DeviceLost(CRenderSettings &Settings) {
    SetLost(FALSE);
}

bool CD3SLight::LoadXML(CXMLNode *Node, CRenderSettings &Settings) {
    CBase3DObject::LoadXML(Node, Settings);

    CMString tmpType = "TARGETED_SPOTLIGHT";
    Node->GetAttributeValue("LIGHTTYPE", tmpType);
    if (strcmp(tmpType.c_str(), "OMNI") == 0) {
        Type = TTypeOmni;
    } else if (strcmp(tmpType.c_str(), "TARGETED_SPOTLIGHT") == 0) {
        Type = TTypeSpot;
    } else if (strcmp(tmpType.c_str(), "DIRECTIONAL") == 0) {
        Type = TTypeDirect;
    } else if (strcmp(tmpType.c_str(), "FREE_SPOTLIGHT") == 0) {
        Type = TTypeSpot;
    } else {
        Type = TTypeDirect;
    }

    Node->GetAttributeValueF("HOTSPOT", Hotspot);
    Node->GetAttributeValueF("INTENSITY", Intensity);
    Node->GetAttributeValueF("CONTRAST", Contrast);
    Node->GetAttributeValueB("USEATTENNEAR", UseAttenuationNear);
    Node->GetAttributeValueB("USEATTENFAR", UseAttenuationFar);
    Node->GetAttributeValueF("NEARATTENSTART", AttenuationNearStart);
    Node->GetAttributeValueF("NEARATTENEND", AttenuationNearEnd);
    Node->GetAttributeValueF("FARATTENSTART", AttenuationFarStart);
    Node->GetAttributeValueF("FARATTENEND", AttenuationFarEnd);
    CMString tmpDecayType = "NONE";
    Node->GetAttributeValue("DECAYTYPE", tmpDecayType);
    if (strcmp(tmpDecayType.c_str(), "NONE") == 0) {
        DecayType = TDecayNone;
    } else if (strcmp(tmpDecayType.c_str(), "INVERSE") == 0) {
        DecayType = TDecayInverse;
    } else {
        DecayType = TDecayInverseSquare;
    }

    Node->GetAttributeValueI("MAPSIZE", ShadowMapSize);

    Node->GetAttributeValueI("TARGETID", TargetID);
    CXMLNode *tmpColor = Node->GetSubNode(Node->FindSubNode("COLOR",0));
    if (tmpColor) {
        LightColor.r = LightColor.g = LightColor.b = LightColor.a = 1.0f;
        tmpColor->GetAttributeValueF("X", LightColor.r);
        tmpColor->GetAttributeValueF("Y", LightColor.g);
        tmpColor->GetAttributeValueF("Z", LightColor.b);
    } else {
        _WARN(this, ErrMgr->TNoSupport, "No 'COLOR' node found in light.");
    }
    return true;
}

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
#include "eng-rendergroup.h"
#include "material\MAT-NoOutput.h"
#include "Material\MAT-MaterialManager.h"
#include "ENG-Engine.h"

#include "SC-DynamicArray.h"
#include "eng-base3dobject.h"

CRenderGroup::CRenderGroup(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    Objects = new CRootClassArray(FALSE);
    Visible = new CRootClassArray(TRUE);

    ///Create one material with ID D3_GRI_Mat_PixelPhong
    NoOutput = (CBaseMaterial *)OwnerEngine->GetMaterialManager()->Query(D3_GRI_Mat_PixelNoOutput);
    if (NoOutput == NULL) {
        NoOutput = new CMaterialNoOutput("DefaultSimple",this->OwnerEngine);
        NoOutput->SetResourceID(D3_GRI_Mat_PixelNoOutput);
        NoOutput->Load(0, *OwnerEngine->GetRenderSettings());
        OwnerEngine->GetMaterialManager()->Add(NoOutput->GetResourceID(),*NoOutput);
    }
}

CRenderGroup::~CRenderGroup(void) {
    delete Objects;
    delete Visible;
    OwnerEngine->GetMaterialManager()->Delete(D3_GRI_Mat_PixelNoOutput);
}

void CRenderGroup::AddVisible(CBase3DObject &Object) {
    CObjectContext *OC = new CObjectContext;
    OC->Object = &Object;
    OC->Context = NULL;
    Visible->Add(OC);
}

void CRenderGroup::AddObject(CBase3DObject &Object) {
    Objects->Add(&Object);
}

void CRenderGroup::ClearVisible() {
    Visible->RemoveAll();
}

DWORD CRenderGroup::GetVisibleCount() {
    return Visible->GetSize();
}

CBase3DObject *CRenderGroup::GetVisible(DWORD Index) {
    CObjectContext *OC = (CObjectContext *)Visible->GetAt(Index);
    if (OC!=NULL) {
        return OC->Object;
    } else {
        return NULL;
    }
}

void CRenderGroup::Render(CRenderSettings &Settings) {
    CBaseMaterialContext *Con = (CBaseMaterialContext *)NoOutput->CreateContext(NULL);
    OwnerEngine->GetMaterialManager()->SetMaterial(NoOutput, Con, 0, Settings);

    for (int j=0; j<Visible->GetSize(); j++) {
        CObjectContext *OC = (CObjectContext *)Visible->GetAt(j);
        OC->Object->Render(Settings,NULL, NoOutput, Con);
    }

    NoOutput->DeleteContext(Con);

    int i=0;
    while (Visible->GetSize()>0) {
        CObjectContext *OC = (CObjectContext *)Visible->GetAt(i);
        OC->Context = OC->Object->Render(Settings,OC->Context, NULL, NULL);
        if (OC->Context == NULL) {
            Visible->DeleteAt(i);
        } else {
            i++;
        }
        if (i>=Visible->GetSize()) i=0;
    }
}

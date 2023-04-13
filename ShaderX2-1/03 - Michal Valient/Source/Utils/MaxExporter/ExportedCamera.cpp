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
#include "exportedcamera.h"

CExportedCamera::CExportedCamera(void) {
}

CExportedCamera::~CExportedCamera(void) {
}

void CExportedCamera::CreateFromFlexporter(const CameraDescriptor& Obj) {
    CExportedObject::CreateFromFlexporter(Obj);
    OrthoCam = (Obj.mOrthoCam)?1:0;
    FOV = Obj.mFOV;
    NearClip = Obj.mNearClip;
    FarClip = Obj.mFarClip;
    EnvNearRange = Obj.mEnvNearRange;
    EnvFarRange = Obj.mEnvFarRange;
    ManualClip = (Obj.mManualClip)?1:0;
    if (Obj.mFOVType == FOV_HORIZONTAL) FOVType = "HORIZONTAL";
    else if (Obj.mFOVType == FOV_VERTICAL) FOVType = "VERTICAL";
    else if (Obj.mFOVType == FOV_DIAGONAL) FOVType = "DIAGONAL";
    if (Obj.mCamType == CTYPE_FREE) CamType = "FREE";
    else if (Obj.mCamType == CTYPE_TARGET) CamType = "TARGET";
    else if (Obj.mCamType == CTYPE_PARALLEL) CamType = "PARALLEL";
}

void CExportedCamera::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    CExportedObject::SaveToXML_StartTAG(File, NULL, &IndentAttribs);
    SaveAttrib(File, IndentAttribs, "Orthogonal", OrthoCam);
    SaveAttrib(File, IndentAttribs, "FOV", FOV);
    SaveAttrib(File, IndentAttribs, "NearClip", NearClip);
    SaveAttrib(File, IndentAttribs, "FarClip", FarClip);
    SaveAttrib(File, IndentAttribs, "EnvNearRange", EnvNearRange);
    SaveAttrib(File, IndentAttribs, "EnvFarRange", EnvFarRange);
    SaveAttrib(File, IndentAttribs, "ManualClip", ManualClip);
    SaveAttrib(File, IndentAttribs, "FOVType", FOVType);
    SaveAttrib(File, IndentAttribs, "CamType", CamType);
    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
}

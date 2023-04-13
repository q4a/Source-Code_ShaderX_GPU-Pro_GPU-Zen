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
#include "exportedobject.h"

CExportedObject::CExportedObject(void) {
    ID = ParentID = MasterID = TargetID = MeshID = -1;
    LocalPRS = 0;
    IsTarget = 0;
}

CExportedObject::~CExportedObject(void) {
}

void CExportedObject::CreateFromFlexporter(const ObjectDescriptor& Obj) {
    Name = Obj.mName;

    ID = Obj.mObjectID;
    ParentID = Obj.mParentID;
    MasterID = Obj.mMasterID;
    TargetID = Obj.mTargetID;

    PRS.Position[0] = Obj.mPrs.Position.x;
    PRS.Position[1] = Obj.mPrs.Position.y;
    PRS.Position[2] = Obj.mPrs.Position.z;

    PRS.Rotation[0] = Obj.mPrs.Rotation.x;
    PRS.Rotation[1] = Obj.mPrs.Rotation.y;
    PRS.Rotation[2] = Obj.mPrs.Rotation.z;
    PRS.Rotation[3] = Obj.mPrs.Rotation.w;

    PRS.Scale[0] = Obj.mPrs.Scale.x;
    PRS.Scale[1] = Obj.mPrs.Scale.y;
    PRS.Scale[2] = Obj.mPrs.Scale.z;

    LocalPRS = (Obj.mLocalPRS)?1:0;

    LocalPivot.Position[0] = Obj.mPivot.LocalPivot.Position.x;
    LocalPivot.Position[1] = Obj.mPivot.LocalPivot.Position.y;
    LocalPivot.Position[2] = Obj.mPivot.LocalPivot.Position.z;

    LocalPivot.Rotation[0] = Obj.mPivot.LocalPivot.Rotation.x;
    LocalPivot.Rotation[1] = Obj.mPivot.LocalPivot.Rotation.y;
    LocalPivot.Rotation[2] = Obj.mPivot.LocalPivot.Rotation.z;
    LocalPivot.Rotation[3] = Obj.mPivot.LocalPivot.Rotation.w;

    LocalPivot.Scale[0] = Obj.mPivot.LocalPivot.Scale.x;
    LocalPivot.Scale[1] = Obj.mPivot.LocalPivot.Scale.y;
    LocalPivot.Scale[2] = Obj.mPivot.LocalPivot.Scale.z;

    WorldPivot.Position[0] = Obj.mPivot.WorldPivot.Position.x;
    WorldPivot.Position[1] = Obj.mPivot.WorldPivot.Position.y;
    WorldPivot.Position[2] = Obj.mPivot.WorldPivot.Position.z;

    WorldPivot.Rotation[0] = Obj.mPivot.WorldPivot.Rotation.x;
    WorldPivot.Rotation[1] = Obj.mPivot.WorldPivot.Rotation.y;
    WorldPivot.Rotation[2] = Obj.mPivot.WorldPivot.Rotation.z;
    WorldPivot.Rotation[3] = Obj.mPivot.WorldPivot.Rotation.w;

    WorldPivot.Scale[0] = Obj.mPivot.WorldPivot.Scale.x;
    WorldPivot.Scale[1] = Obj.mPivot.WorldPivot.Scale.y;
    WorldPivot.Scale[2] = Obj.mPivot.WorldPivot.Scale.z;

    IsInstance = (Obj.mIsInstance)?1:0;
}

void PosRotScale::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s>\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
}

void PosRotScale::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttribXYZ(File, IndentAttribs, "Position", Position);
    SaveAttribXYZW(File, IndentAttribs, "Rotation", Rotation);
    SaveAttribXYZ(File, IndentAttribs, "Scale", Scale);
}

void CExportedObject::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttrib(File, IndentAttribs, "NAME", Name);
    SaveAttrib(File, IndentAttribs, "ID", ID);
    SaveAttrib(File, IndentAttribs, "PARENTID", ParentID);
    SaveAttrib(File, IndentAttribs, "MASTERID", MasterID);
    SaveAttrib(File, IndentAttribs, "TARGETID", TargetID);
    SaveAttrib(File, IndentAttribs, "MESHID", MeshID);
    SaveAttrib(File, IndentAttribs, "LOCALPRS", LocalPRS);
    SaveAttrib(File, IndentAttribs, "ISTARGET", IsTarget);
    SaveAttrib(File, IndentAttribs, "ISINSTANCE", IsInstance);
    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CExportedObject::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    tmp = "PRS";
    PRS.SaveToXML(File, &tmp, &IndentAttribs);
    tmp = "LOCALPIVOT";
    LocalPivot.SaveToXML(File, &tmp, &IndentAttribs);
    tmp = "WORLDPIVOT";
    WorldPivot.SaveToXML(File, &tmp, &IndentAttribs);
}

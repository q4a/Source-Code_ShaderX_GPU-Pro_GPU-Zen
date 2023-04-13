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
#include "exportedtexture.h"

CExportedTexture::CExportedTexture(void) {
}

CExportedTexture::~CExportedTexture(void) {
}

void CExportedTexture::CreateFromFlexporter(const TextureDescriptor& Obj) {
    ID = Obj.mObjectID;
    Name = Obj.mName;

    TextureCrop_OffsetUV[0] = Obj.mCValues.OffsetU;
    TextureCrop_OffsetUV[1] = Obj.mCValues.OffsetV;
    TextureCrop_ScaleUV[0] = Obj.mCValues.ScaleU;
    TextureCrop_ScaleUV[1] = Obj.mCValues.ScaleV;
    memcpy(TextureMatrix_4x3, Obj.mTMtx.m, sizeof(TextureMatrix_4x3));
}

void CExportedTexture::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttrib(File, IndentAttribs, "ID", ID);
    CString tmpTexName = Name.Mid(Name.ReverseFind('\\')+1);
    SaveAttrib(File, IndentAttribs, "Name", tmpTexName);

    SaveAttrib(File, IndentAttribs, "TextureCrop_OffsetU", TextureCrop_OffsetUV[0]);
    SaveAttrib(File, IndentAttribs, "TextureCrop_OffsetV", TextureCrop_OffsetUV[1]);
    SaveAttrib(File, IndentAttribs, "TextureCrop_ScaleU", TextureCrop_ScaleUV[0]);
    SaveAttrib(File, IndentAttribs, "TextureCrop_ScaleV", TextureCrop_ScaleUV[1]);

    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CExportedTexture::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttribMatrix_4x3(File, IndentAttribs, "TextureMatrix", TextureMatrix_4x3);
}

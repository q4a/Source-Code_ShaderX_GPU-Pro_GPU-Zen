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


#pragma once
#include "afx.h"

void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, CString &Value);
void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, int Value);
void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, float Value);
void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, UINT Value);
void SaveAttribXY(CStdioFile *File, CString &Indent, char *AttName, float *ValueXY);
void SaveAttribXYZ(CStdioFile *File, CString &Indent, char *AttName, float *ValueXYZ);
void SaveAttribXYZW(CStdioFile *File, CString &Indent, char *AttName, float *ValueXYZW);
void SaveAttribMatrix_4x3(CStdioFile *File, CString &Indent, char *AttName, float Matrix[4][3]);

class CExportedEntity : public CObject {
public:
    CExportedEntity(void);
    virtual ~CExportedEntity(void);

    //If Tag is NULL, tag name is not written to XML
    //save whole structure
    virtual void SaveToXML(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save end tag
    virtual void SaveToXML_EndTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

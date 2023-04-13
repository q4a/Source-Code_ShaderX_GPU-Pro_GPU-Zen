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
#include "exportedentity.h"

class PosRotScale : public CExportedEntity {
public:
    float Position[3];      //x,y,z position
    float Rotation[4];      //x,y,z,w rotation quarterion
    float Scale[3];         //x,y,z scale

    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

class CExportedObject : public CExportedEntity {
public:
    CString         Name;           //Name of the object
    int             ID;             //ID of this object
    int             ParentID;       //ID of the parent (or -1)
    int             MasterID;       //ID of master (if is instance of something)
    int             TargetID;       //ID of the target (if camera or spotlight)
    int             MeshID;         //ID of the mesh. Defaults to -1 and is not touched by CreateFromXXX method (must be set by exporter).
    PosRotScale     PRS;            //Position, rotation and scale
    int             LocalPRS;       //1 for local PRS, 0 for absolute PRS
    int             IsTarget;       //1 if object is target (of camera, light) 0 if not. Defaults to 0 and is not touched by CreateFromXXX method (must be set by exporter).
    int             IsInstance;     //1 if object is instance, 0 otherwise
    PosRotScale     LocalPivot;     //Local position pivot
    PosRotScale     WorldPivot;     //World position pivot

    CExportedObject(void);
    virtual ~CExportedObject(void);

    virtual void CreateFromFlexporter(const ObjectDescriptor& Obj);
    
    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

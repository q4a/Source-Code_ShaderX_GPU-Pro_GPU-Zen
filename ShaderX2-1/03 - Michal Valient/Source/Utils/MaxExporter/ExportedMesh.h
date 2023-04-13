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

#pragma pack(1)

static const char   MeshHeader[] = "Dimension3 Mesh File";
static const float  MeshVersion = 2.0f;

typedef struct {
    UINT    MaterialID;
    UINT    MinVertIndex;
    UINT    MaxVertIndex;
    UINT    StartFaceIndex;
    UINT    FaceCount;
} ExportedMeshSubmesh;

typedef struct {
    UINT    Vertex[3];
} ExportedMeshFace;

typedef struct {
    float       Position[3];
    float       Normal[3];
    float       Map[2];
} ExportedMeshVertex;

typedef struct {
    char        Header[32];             //Header magic string "Dimension3 Mesh File" padded to 32 with zeroes
    float       Version;                //Version of mesh
    float       BSphereRadius;          //Center of bounding sphere
    float       BSphereCenter[3];       //x,y,z coordinates of bounding sphere
    UINT        NameLength;             //length of zero terminated name buffer (including zero at the end! strlen(Name) = NameLength-1)
    UINT        VertexCount;            //Number of saved verticles
    UINT        FaceCount;              //Number of saved faces
    UINT        SubMeshCount;           //Number of saved submeshes
} ExportedMeshHeader;

class CExportedMesh : public CObject {
public:
    ExportedMeshHeader      Header;
    char                   *Name;
    ExportedMeshVertex     *Vertex;
    ExportedMeshFace       *Face;
    ExportedMeshSubmesh    *SubMesh;

    CString                 FileName;

    CExportedMesh(void);
    virtual ~CExportedMesh(void);

    bool CreateFromFlexporter(const MeshDescriptor& mesh);
    bool Save(CString *FileName);
};

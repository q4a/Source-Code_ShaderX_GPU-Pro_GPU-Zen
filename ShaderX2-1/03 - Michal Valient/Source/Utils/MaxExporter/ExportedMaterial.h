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
#include "exportedentity.h"

class CExportedMaterial : public CExportedEntity {
public:
    int      ID;
    CString  Name;

    int      AmbientMapID;
    int      DiffuseMapID;
    int      SpecularMapID;
    int      ShininessMapID;
    int      ShiningStrengthMapID;
    int      SelfIllumMapID;
    int      OpacityMapID;
    int      FilterMapID;
    int      BumpMapID;
    int      ReflexionMapID;
    int      RefractionMapID;
    int      DisplacementMapID;
    // Amounts
    float    AmbientCoeff;
    float    DiffuseCoeff;
    float    SpecularCoeff;
    float    ShininessCoeff;
    float    ShiningStrengthCoeff;
    float    SelfIllumCoeff;
    float    OpacityCoeff;
    float    FilterCoeff;
    float    BumpCoeff;
    float    ReflexionCoeff;
    float    RefractionCoeff;
    float    DisplacementCoeff;
    // Colors
    float    MtlAmbientColor[3];
    float    MtlDiffuseColor[3];
    float    MtlSpecularColor[3];
    float    MtlFilterColor[3];
    // Static properties
    int      Shading;
    int      Soften;
    int      FaceMap;
    int      TwoSided;
    int      Wire;
    int      WireUnits;
    int      FalloffOut;
    int      TransparencyType;
    // Dynamic properties
    float    Shininess;
    float    ShiningStrength;
    float    SelfIllum;
    float    Opacity;
    float    OpaFalloff;
    float    WireSize;
    float    IOR;
    // Dynamic properties
    float    Bounce;
    float    StaticFriction;
    float    SlidingFriction;
    // 1.15: enhanced self-illum
    int      SelfIllumOn;
    float    SelfIllumValue;
    float    SelfIllumColor[3];

    CExportedMaterial(void);
    virtual ~CExportedMaterial(void);

    virtual void CreateFromFlexporter(const MaterialDescriptor& Obj);

    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

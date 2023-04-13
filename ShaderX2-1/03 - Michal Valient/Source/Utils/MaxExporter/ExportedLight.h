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
#include "exportedobject.h"

class CExportedLight : public CExportedObject {
public:
	CString             LightType;
	float               Color[3];
	float               Intensity;
	float               Contrast;
	float               DiffuseSoft;
	int                 LightUsed;
	int                 AffectDiffuse;
	int                 AffectSpecular;
	int                 UseAttenNear;
	int                 UseAttenFar;
	float               NearAttenStart;
	float               NearAttenEnd;
	float               FarAttenStart;
	float               FarAttenEnd;
	CString             DecayType;
	float               HotSpot;
	float               Falloff;
	float               Aspect;
	CString             SpotShape;
	int                 ShadowType;
	int                 AbsMapBias;
	float               RayBias;
	float               MapBias;
	float               MapRange;
	int                 MapSize;
	int                 CastShadows;
	float               ShadowColor[3];
	int                 LightAffectsShadow;
	float               ShadowDensity;
	int                 ProjMapID;
	int                 ShadowProjMapID;

    CExportedLight(void);
    virtual ~CExportedLight(void);

    virtual void CreateFromFlexporter(const LightDescriptor& Obj);

    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

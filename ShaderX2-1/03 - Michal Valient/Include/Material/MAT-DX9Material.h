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
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "mat-basematerial.h"

class CMString;
class CRenderSettings;
class CXMLNode;

class CDX9MaterialContext : public CBaseMaterialContext {
public:
    virtual void FillFromXML(CXMLNode *Node, CRenderSettings &Settings) = 0;
};

class CDX9Material : public CBaseMaterial {
protected:
    virtual ~CDX9Material(void);
public:
    CDX9Material(char *ObjName,CEngine *OwnerEngine);

    virtual CDX9MaterialContext *CreateContextFromXML(CXMLNode *Node, CBase3DObject *Object, CRenderSettings &Settings);

    MAKE_CLSNAME("CDX9Material");
};

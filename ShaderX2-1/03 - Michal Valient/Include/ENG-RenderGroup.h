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
#include "baseclass.h"

//############################################################################
//Forward declarations
class CBase3DObject;
class CRootClassArray;
class CRenderSettings;
class CBaseMaterial;

/**
  This class groups objects with the same rendering ordering
  This implementation uses no special sorting.
  For example group for transparent objects will use inverted Z ordering.
**/

class CRenderGroup : public CBaseClass {
protected:
    class CObjectContext : public CRootClass {
    public:
        CBase3DObject *Object;
        void          *Context;
    };
    DWORD GroupID;
    CRootClassArray *Visible;   ///All objects in group
    CBaseMaterial   *NoOutput;  ///Material for no output
public:
    CRootClassArray *Objects;   ///All objects in group

    D3_INLINE void SetGroupID(DWORD GroupID) {this->GroupID = GroupID; }
    D3_INLINE DWORD GetGroupID() { return GroupID; }


    void Render(CRenderSettings &Settings);

    void AddVisible(CBase3DObject &Object);
    void ClearVisible();
    DWORD   GetVisibleCount();
    CBase3DObject *GetVisible(DWORD Index);
    void AddObject(CBase3DObject &Object);

    CRenderGroup(char *ObjName,CEngine *OwnerEngine);
    virtual ~CRenderGroup(void);

    MAKE_CLSCREATE(CRenderGroup);
    MAKE_CLSNAME("CRenderGroup");
    MAKE_DEFAULTGARBAGE()
};

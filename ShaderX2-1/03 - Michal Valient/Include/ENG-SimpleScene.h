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
class CHashTable2;
class CRenderSettings;
class CRenderGroup;
class CBase3DObject;
class CD3SCamera;
class CXMLNode;

class CSimpleScene : public CBaseClass {
protected:
    CHashTable2  *Groups;
    CHashTable2  *Objects;
    CHashTable2  *Cameras;
    CHashTable2  *Lights;
public:
    CD3SCamera  *CurrentCamera;
    //If device is lost, everything has to be set to lost state.
    virtual void DeviceLost(CRenderSettings &Settings);    

    //Restore from lost state.
    virtual void Restore(CRenderSettings &Settings);

    void AddGroupByID(DWORD GroupID, CRenderGroup &Group);
    void AddObject(DWORD GroupID, CBase3DObject &Object);
    void AddCamera(CBase3DObject &Camera);
    void AddLight(CBase3DObject &Light);

    void Render(CRenderSettings &Settings);

    CSimpleScene(char *ObjName,CEngine *OwnerEngine);
    virtual ~CSimpleScene(void);

    bool Load(int LoadIndex, CRenderSettings &Settings);
    bool LoadXML(CXMLNode *Node, CRenderSettings &Settings);

    MAKE_CLSCREATE(CSimpleScene);
    MAKE_CLSNAME("CSimpleScene");
    MAKE_DEFAULTGARBAGE()
};

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

#include "MGR-BaseManager.h"

//############################################################################
//Forward declarations
class CHashTable2;
class CBaseResource;
class CRenderSettings;

class CResourceManager : public CBaseManager {
protected:
    CHashTable2  *Resources;
public:
    ///With this method you query for specified resource. If it is managed by manager, it's
    ///reference count is increased and caller can use it.
    ///@retval NULL if resource is not manager by manager.
    virtual CBaseResource *Query(DWORD ResourceID);
    ///Decrements reference count of Resource. If that drops to zero, Release is called.
    ///After you add resource to manager, do not destroy resource directly. Use this method,
    ///because of that there can be a lot of other objects that refer to this resource.
    ///@return New reference count of resource. Zero means resource was freed.
    ///@retval -1 means error (or resource was not found)
	virtual int Delete(DWORD ResourceID);
    ///Add new resource to manager.
    ///@retval FALSE if another resource with the same ID exists in manager
	virtual bool Add(DWORD ResourceID, CBaseResource &Resource);
    ///Add new temporary resource to manager. This resource receives it's own ID
    ///@retval FALSE if another resource with the same ID exists in manager
	virtual bool AddTemporary(CBaseResource &Resource);
    ///Find specified resource
    ///@retval NULL on error
	virtual CBaseResource *Find(DWORD ResourceID);
    ///Delete all resources (in meaning of release).
    virtual void ReleaseAll();
    ///Send DeviceLost to all resources
	virtual void DeviceLost(CRenderSettings &Settings);
    ///Send Restore to all resources
	virtual bool Restore(CRenderSettings &Settings);

    CResourceManager(char *ObjName,CEngine *OwnerEngine);
	virtual ~CResourceManager();

    MAKE_CLSCREATE(CResourceManager);
    MAKE_CLSNAME("CResourceManager");
    MAKE_DEFAULTGARBAGE();
};

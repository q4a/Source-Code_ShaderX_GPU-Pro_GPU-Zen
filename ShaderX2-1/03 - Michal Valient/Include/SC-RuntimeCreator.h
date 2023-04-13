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
class CStrHashTable;

class CRuntimeCreator : public CBaseClass {
protected:
    class CRuntimeInfo : public CRootClass {
    public:
        CBaseClass *ExistingInstance;   //existing instance of this class.
                                        //This instance is entirely managed by RuntimeCreator
                                        //and cannot be used (referenced) in runtime by nothing other.
        void       *RegisterInfo;       //This pointer is passed to Create method of class.
                                        //It is optional and is defined in RegisterClass method
                                        //It has to be allocated by MemMgr->AllocMem and 
                                        //is entirely managed by RuntimeCreator and cannot be used 
                                        //(referenced) in runtime by nothing other.
        CRuntimeInfo() {
            RegisterInfo = ExistingInstance = NULL;
        }

        virtual ~CRuntimeInfo() {
            if (ExistingInstance!=NULL) delete ExistingInstance;
            if (RegisterInfo!=NULL) MemMgr->FreeMem(RegisterInfo);
        }
    };

    CStrHashTable  *Classes;
public:
    //Class is registered by system.
    bool RegisterClass(CBaseClass *Class, void *RegisterInfo);
    //Class is created by RuntimeCreator. Parameters are passed to Create method of class.
    CBaseClass *CreateClass(char *ClassName, char *ObjectName, void *Parameters);

    CRuntimeCreator(char *ObjName,CEngine *OwnerEngine);
    virtual ~CRuntimeCreator(void);

    MAKE_CLSCREATE(CRuntimeCreator);
    MAKE_CLSNAME("CRuntimeCreator");
    MAKE_DEFAULTGARBAGE();
};

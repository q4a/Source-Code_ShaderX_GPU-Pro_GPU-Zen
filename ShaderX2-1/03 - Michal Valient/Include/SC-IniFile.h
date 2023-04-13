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


// SC-IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseClass.h"

//############################################################################
//Forward declarations
class CRootClassArray;
class CMString;

class CIniFile : public CBaseClass {
protected:
    class CIniFileSection : public CRootClass {
    public:
        CMString        *Name;
        CRootClassArray *Items; 

        CIniFileSection();
        virtual ~CIniFileSection();
    };

    class CIniFileItem : public CRootClass {
    public:
        CMString        *Name;
        CMString        *Value;

        CIniFileItem();
        virtual ~CIniFileItem();
    };

    CRootClassArray     *Sections;
public:
    virtual CMString ReadString(CMString &Section, CMString &Name, CMString &Default);
    virtual CMString ReadString(char *Section, char *Name, char *Default);
    virtual bool ReadBool(CMString &Section, CMString &Name, bool Default);
    virtual bool ReadBool(char *Section, char *Name, bool Default);
    virtual int ReadInt(CMString &Section, CMString &Name, int Default);
    virtual int ReadInt(char *Section, char *Name, int Default);

    void Load(char *Source, UINT Size);
    void Unload();
    CIniFile(char *ObjName,CEngine *OwnerEngine);
	virtual ~CIniFile();

    MAKE_CLSCREATE(CIniFile);
    MAKE_CLSNAME("CIniFile");
    MAKE_DEFAULTGARBAGE()
};


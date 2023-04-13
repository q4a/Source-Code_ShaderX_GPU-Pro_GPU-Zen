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


//############################################################################
//# DirTreeDirectory manages resources as files in directory tree.
//# Note: It manages files only by names, so take care of duplicate names
//#       the second one will be ignored.
//############################################################################


#pragma once

#include "FIL-BaseDirectory.h"

//############################################################################
//Forward declarations

class CDirTreeDirectory : public CBaseDirectory {
private:
    typedef struct sResourceInfoTag {
        char   *ResourceName;
        char   *ResourceDir;
        UINT    Size;
        UINT    Pos;
        UINT    Checksum;
        FILE   *FileHandle;
    } sResourceInfo;

    int             DirCount;
    char          **Dirs;
    int             FilesCount;
    sResourceInfo  *Files;
    bool            Initialised;
    
    void GetTreeCounts(CMString &Dir, int &DirCount, int &FileCount);
    void AddTree(CMString &Dir, int &CurDir, int &CurFile);
    inline bool IsValidIndex(int Index);
public:
    virtual bool IsInitialised() { return Initialised; }
    virtual int GetResourceCount();
    virtual bool Open(int ResourceIndex);
    virtual bool Close(int ResourceIndex);
    virtual bool Seek(int ResourceIndex, int SeekOperation, int NewPos);

    //Partial Resource Loading
    virtual int Read(int ResourceIndex, void *OutBuffer, int Count);

    //Full Resource loading
    virtual int LoadResource(int ResourceIndex, void *OutBuffer);

    //RESOURCE Informations...
    virtual int GetResourceSize(int ResourceIndex);
    virtual int GetResourcePos(int ResourceIndex);
    virtual int GetResourceIndex(CMString &ResourceName);
    virtual bool IsResourceOpened(int ResourceIndex);

    virtual bool Initialise(CMString &NewInitString);
    virtual void Reset();
    
    CDirTreeDirectory(char *ObjName,CEngine *OwnerEngine);
    virtual ~CDirTreeDirectory();
    
    MAKE_CLSCREATE(CDirTreeDirectory);
    MAKE_CLSNAME("CDirTreeDirectory");
    MAKE_DEFAULTGARBAGE();
};

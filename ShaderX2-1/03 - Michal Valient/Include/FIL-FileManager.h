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
//# FileManager unifies access to all files, no matter where they are or how
//# they are stored on disk (or if they are really stored on disk)
//############################################################################


#pragma once

#include "MGR-BaseManager.h"

//############################################################################
//Forward declarations
class CRootClassArray;
class CBaseDirectory;
class CMString;

class CFileManager : public CBaseManager {
protected:
    CRootClassArray  *ArrayOfDirs;
    UINT            NextFreeBase;

private:

    class CSingleDirectoryInfo : public CRootClass {
    public:
        CBaseDirectory     *Directory;
        bool                Enabled;
        int                 BaseIndex;
        
        CSingleDirectoryInfo() {
            Directory=NULL;
            Enabled=FALSE;
            BaseIndex=0;
        }
        virtual ~CSingleDirectoryInfo();
    };

    CSingleDirectoryInfo *GetDirInfoFromIndex(int Index);
    CBaseDirectory *GetDirectoryFromIndex(int Index);
public:
	CBaseDirectory * GetDirectory(int GlobalIndex);
	bool Seek(int GlobalIndex, int SeekOperation, int NewPos);
	int GetSize(int GlobalIndex);
	int GetPos(int GlobalIndex);
	int Load(int GlobalIndex, void *OutBuffer);
	int Read(int GlobalIndex, void *OutBuffer, int Count);
	bool IsOpened(int GlobalIndex);
	bool Close(int GlobalIndex);
	bool Open(int GlobalIndex);
	CBaseDirectory * Find(int *LocalIndex, CMString &Name);
	int Find(CMString &Name);
	int AddNewDirectory(CBaseDirectory &NewDir);

	CFileManager(char *ObjName,CEngine *OwnerEngine);
	virtual ~CFileManager();

    MAKE_CLSCREATE(CFileManager);
    MAKE_CLSNAME("CFileManager");
    MAKE_DEFAULTGARBAGE();
};


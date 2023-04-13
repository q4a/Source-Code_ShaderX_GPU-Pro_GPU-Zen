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


// ENG-MeshManager.h: interface for the CMeshManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "nommgr.h"
#include "d3d9.h"
#include "mmgr.h"

#include "ENG-ResourceManager.h"



//############################################################################
//Forward declarations

class CMeshManager : public CResourceManager {
protected:
    //Guard structure for SetStreamSource
    class CStreamInfo : public CRootClass {
    public:
        UINT    StreamNumber;
        IDirect3DVertexBuffer9 *VBuf;
        UINT    OffsetInBytes;
        UINT    Stride;

        CStreamInfo();
        virtual ~CStreamInfo();
    };

    //Guard structure for SetIndices
    class CIndicesInfo : public CRootClass {
    public:
        IDirect3DIndexBuffer9 *Ibuf;

        CIndicesInfo();
        virtual ~CIndicesInfo();
    };

    CHashTable2 *GuardStream;
    CIndicesInfo *GuardIndices;

public:
    CMeshManager(char *ObjName, CEngine *OwnerEngine);
	virtual ~CMeshManager();

    //DX8 Mapped inline functions. They use guard buffers
    HRESULT SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *VBuf, UINT OffsetInBytes, UINT Stride);
    HRESULT SetIndices(IDirect3DIndexBuffer9 *IBuf);

    virtual void DeviceLost(CRenderSettings &Settings);

    MAKE_CLSCREATE(CMeshManager);
    MAKE_CLSNAME("CMeshManager");
    MAKE_DEFAULTGARBAGE()
};


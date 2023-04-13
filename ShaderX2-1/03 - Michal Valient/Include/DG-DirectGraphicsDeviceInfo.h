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


// DG-DirectGraphicsDeviceInfo.h: interface for the CDirectGraphicsDeviceInfo class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseClass.h"
#include <d3d9.h>

//############################################################################
//Forward declarations

class CDirectGraphicsDeviceInfo : public CBaseClass {
protected:
	bool GetDeviceCaps(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType);
    void GetMonitor(IDirect3D9 &Direct3D, UINT Adapter);
    bool GetDriverInfo(IDirect3D9 &Direct3D, UINT Adapter);
    bool GetModeInfo(IDirect3D9 &Direct3D, UINT Adapter);
public:
    //variables...
    //modes information
    UINT            AdapterModeCount;
    D3DDISPLAYMODE *Modes;
    //general / count information
    UINT            Adapter;
    D3DDEVTYPE      DeviceType;
    HMONITOR        AdapterMonitor;
    //device caps info
    D3DCAPS9        DevCaps;
    //driver information
    char           *Driver;
    char           *Description;
    DWORD           DriverVersion_Product;
    DWORD           DriverVersion_Version;
    DWORD           DriverVersion_SubVersion;
    DWORD           DriverVersion_Build;
    DWORD           VendorId;
    DWORD           DeviceId;
    DWORD           SubSysId;
    DWORD           Revision;
    GUID            DeviceIdentifier;
public:
	D3DFORMAT GetModeFormat(UINT Width, UINT Height, UINT Depth, UINT &Frequency);
    //methods
	CDirectGraphicsDeviceInfo(char *ObjName,CEngine *OwnerEngine);
	virtual ~CDirectGraphicsDeviceInfo();
    bool GetAdapterInfo(IDirect3D9 &Direct3D, UINT Adapter);
    bool GetDeviceInfo(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType);
    bool GetInfo(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType);

    MAKE_CLSCREATE(CDirectGraphicsDeviceInfo);
    MAKE_CLSNAME("CDirectGraphicsDeviceInfo");
    MAKE_DEFAULTGARBAGE();
};


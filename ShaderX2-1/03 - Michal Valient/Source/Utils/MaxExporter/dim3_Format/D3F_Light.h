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
#include "D3Format\d3f_Object.h"

class CD3F_Light : public CD3F_Object {
    typedef struct {
        DWORD   TAG;
        int             Type;       //convert from Flex types
        double          Hotspot;
        double          Intensity;
        double          Contrast;
        bool            UseAttenuationNear;
        bool            UseAttenuationFar;
        double          AttenuationNearStart;
        double          AttenuationNearEnd;
        double          AttenuationFarStart;
        double          AttenuationFarEnd;
        int             DecayType;  //convert from Flex types
        int             TargetID;
    } SD3F_Light;
public:
    static const int TTypeOmni = 1;
    static const int TTypeSpot = 2;
    static const int TTypeDirect = 3;

    static const int TDecayNone = 1;
    static const int TDecayInverse = 2;
    static const int TDecayInverseSquare = 3;

    int             Type;       //convert from Flex types
    double          Hotspot;
    double          Intensity;
    double          Contrast;
    bool            UseAttenuationNear;
    bool            UseAttenuationFar;
    double          AttenuationNearStart;
    double          AttenuationNearEnd;
    double          AttenuationFarStart;
    double          AttenuationFarEnd;
    int             DecayType;  //convert from Flex types
    int             TargetID;

    CD3F_Coord      LightColor;

    CD3F_Light(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Light(void);

    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);
    
    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);

    MAKE_DEFAULTGARBAGE();
    MAKE_CLSNAME("CD3F_Light");
    MAKE_CLSCREATE(CD3F_Light);
};

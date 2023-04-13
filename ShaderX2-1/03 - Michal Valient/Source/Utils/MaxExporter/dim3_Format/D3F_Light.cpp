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


#include "stdafx.h"
#include "d3format\d3f_light.h"

CD3F_Light::CD3F_Light(char *ObjName,CEngine *OwnerEngine) : CD3F_Object(ObjName,OwnerEngine) {
}

CD3F_Light::~CD3F_Light(void) {
}

void CD3F_Light::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Light s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='HGIL') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Light",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'LIGH' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    Type = s.Type;
    Hotspot = s.Hotspot;
    Intensity = s.Intensity;
    Contrast = s.Contrast;
    UseAttenuationNear = s.UseAttenuationNear;
    UseAttenuationFar = s.UseAttenuationFar;
    AttenuationNearStart = s.AttenuationNearStart;
    AttenuationNearEnd = s.AttenuationNearEnd;
    AttenuationFarStart = s.AttenuationFarStart;
    AttenuationFarEnd = s.AttenuationFarEnd;
    DecayType = s.DecayType;
    TargetID = s.TargetID;
    LightColor.LoadBIN(Buffer,Length,CurrPosition);
    CD3F_Object::LoadBIN(Buffer,Length,CurrPosition);
}

void CD3F_Light::SaveBIN(FILE *OutFile) {
    SD3F_Light s;
    s.TAG = 'HGIL';
    s.Type = Type;
    s.Hotspot = Hotspot;
    s.Intensity = Intensity;
    s.Contrast = Contrast;
    s.UseAttenuationNear = UseAttenuationNear;
    s.UseAttenuationFar = UseAttenuationFar;
    s.AttenuationNearStart = AttenuationNearStart;
    s.AttenuationNearEnd = AttenuationNearEnd;
    s.AttenuationFarStart = AttenuationFarStart;
    s.AttenuationFarEnd = AttenuationFarEnd;
    s.DecayType = DecayType;
    s.TargetID = TargetID;
    fwrite(&s,sizeof(s),1,OutFile);
    LightColor.SaveBIN(OutFile);
    CD3F_Object::SaveBIN(OutFile);
}

void CD3F_Light::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_LIGHT TYPE='%u' HOTSPOT='%f' INTENSITY='%f' CONTRAST='%f' USEATTENUATIONNEAR='%u' ", pInt.c_str(),
                Type, Hotspot, Intensity, Contrast, UseAttenuationNear);
    fprintf(OutFile,"USEATTENUATIONFAR='%u' ATTENUATIONNEARSTART='%f' ATTENUATIONNEAREND='%f' ",
                UseAttenuationFar, AttenuationNearStart, AttenuationNearEnd);
    fprintf(OutFile,"ATTENUATIONFARSTART='%f' ATTENUATIONFAREND='%f' DECAYTYPE='%u' TARGETID='%i'>\n",
                AttenuationFarStart, AttenuationFarEnd, DecayType, TargetID);
    CD3F_Object::SaveXML(OutFile, Indent);
    CMString tmp("LIGHTCOLOR");
    LightColor.SaveXML(OutFile,Indent,tmp);
    fprintf(OutFile,"%s</D3F_LIGHT>\n",pInt.c_str());
    Indent = pInt;
}

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
#include "d3format\d3f_Texture.h"

CD3F_Texture::CD3F_Texture(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
}

CD3F_Texture::~CD3F_Texture(void) {
}

void CD3F_Texture::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Texture s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='RTXT') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Texture",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'TXTR' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    FileName = s.FileName;
    ID = s.ID;
}

void CD3F_Texture::SaveBIN(FILE *OutFile) {
    SD3F_Texture s;
    s.TAG = 'RTXT';
    s.ID = ID;
    memset(s.FileName,' ',sizeof(s.FileName));
    strcpy(s.FileName,FileName.c_str());
    fwrite(&s,sizeof(s),1,OutFile);
}

void CD3F_Texture::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_Texture ID='%i' FILENAME='%s'/>\n", pInt.c_str(),
        ID, FileName.c_str());
    Indent = pInt;
}

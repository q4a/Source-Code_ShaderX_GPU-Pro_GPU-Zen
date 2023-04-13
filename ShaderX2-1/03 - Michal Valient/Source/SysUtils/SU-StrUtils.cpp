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
#include "SU-StrUtils.h"
#include "GLobal.h"
#include "MGR-MemoryManager.h"

char *suStrReallocCopy(char *sDest, char *sSrc) {
    UINT len = strlen(sSrc)+1;
    void *ret=MemMgr->ReAllocMem(sDest,len,0);
    memcpy(ret,sSrc,len);
    return (char *)ret;
}

UINT suStrCheckSum(char *sStr) {
    unsigned char c1=0,c2=0,c3=0,c4=0;
    UINT str_len = strlen(sStr);
    while (str_len) {
        c1=(char)(c1+*sStr);
        c2=(char)(c2+c1);
        c3=(char)(c3+c2);
        c4=(char)(c4+c3);
        sStr=sStr+1;
        str_len--;
    }
    return (c1 | (c2<<8) | (c3<<16) | (c4<<24) );
}

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


// SC-SyntaxScanner.h: interface for the CSyntaxScanner class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "RootClass.h"

//############################################################################
//Forward declarations
class CMString;

//Return types for found token
const int SS_FOUND_TERMINAL = 1;
const int SS_FOUND_WORD     = 2;
const int SS_FOUND_EOLN     = 3;
const int SS_FOUND_EOF      = 4;
const int SS_FOUND_REMARK   = 5;
const int SS_FOUND_INTEGER  = 6;
const int SS_FOUND_STRING   = 7;
const int SS_FOUND_SPACE    = 8;
const int SS_FOUND_TAB      = 9;
const int SS_FOUND_FLOAT    = 10;
const int SS_FOUND_UNKNOWN  = -1;

//Flags for scanner
const UINT SS_FLAG_NONE             = 0;    //default
const UINT SS_FLAG_NEWLINE          = 1;    //return new line as special token
const UINT SS_FLAG_SPACE            = 2;    //return space as special token
const UINT SS_FLAG_TAB              = 4;    //return tab as special token
const UINT SS_FLAG_REMARK           = 8;    //return remark as special token
const UINT SS_FLAG_STRINGINQUOTES   = 16;   //return strings in quotas

class CSyntaxScanner : public CRootClass {
protected:
    char *Source;
    UINT Size;
    bool IsOwner;

    //informations about last found token (this is low level information)
    int         LastToken;
    UINT        LastTokenStart;
    UINT        LastTokenEnd;
    CMString   *LastTokenString;
    char        LastTokenChar;
    __int64     LastTokenInt;
    double      LastTokenFloat;

public:
    UINT Flags;
    UINT Position;
public:
	CMString AsString();
	char AsChar();
    __int64 AsInt64();
    int AsInt() { return (int)AsInt64(); }
    double AsFloat();
	void SetSource(char *Source, UINT Size, bool IsOwner);
	int GetNextLowLevelToken();
    int GetNextToken();
    int ScanNext();
	CSyntaxScanner();
	virtual ~CSyntaxScanner();
};


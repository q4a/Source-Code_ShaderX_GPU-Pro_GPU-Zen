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


// SC-MString.h: interface for the CMString class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "rootclass.h"

class CMString : public CRootClass {
protected:
	char *m_sData;

void ConcatCopy(int iLen1, char *sString1, int iLen2, char *sString2);
void ConcatInPlace(int iLen, char *sString);
public:
// Constructors & Destructors
	// destructor
	virtual ~CMString();
    // constructs empty CString
    CMString();
    // copy constructor
    CMString(const CMString& sSource);
    // from a single character
    CMString(char ch, int iRepeat = 1);
    // from an ANSI string (converts to TCHAR)
    CMString(char *sSource);
    CMString(const char* sSource);
    // from unsigned characters
    CMString(const unsigned char* sSource);
    //from any buffer (does not need's to be null terminated
    CMString(const char* sSource, const int Length);

// debug functions...
	void printf() { 
		if (m_sData!=NULL) ::printf("%p | %s\n",m_sData,m_sData);
		else ::printf("%p | %s\n",NULL,"<EMPTY>");
	}
// Attributes & Operations
public:
    // get data length
    int Length() const;
    // TRUE if zero length
    bool IsEmpty() const;
    // clear contents to empty
    void Empty();

//character manipulation
public:
    // return single character at zero-based index
    char GetAt(int iIndex) const;
    // return single character at zero-based index
    char operator[](int iIndex) const;
    // set a single character at zero-based index, return the previous...
    void SetAt(int iIndex, char ch);

	char *c_str() const;

	// overloaded assignment
public:
    // copy from another CString
    const CMString& operator=(const CMString& sSource);
    // set string content to single character
    const CMString& operator=(char ch);
    // copy string content from ANSI string
    const CMString& operator=(char *sSource);
    // copy string content from unsigned chars
    const CMString& operator=(const unsigned char* sSource);

// string concatenation
public:
    friend CMString operator+(const CMString& string1,const CMString& string2);
    friend CMString operator+(const CMString& string, char ch);
    friend CMString operator+(char ch, const CMString& string);
    friend CMString operator+(const CMString& string, char *lpsz);
    friend CMString operator+(char *lpsz, const CMString& string);

    // concatenate from another CString
    const CMString& operator+=(const CMString& string);
    // concatenate a single character
    const CMString& operator+=(char ch);
    // concatenate a character buffer
    const CMString& operator+=(char *lpsz);

//string formating
	void Format(char *format, ...);
    void Format(char *format, va_list argPtr);

//path related methods
    CMString ExtractPath();
    CMString ExtractFile();
    CMString ExtractExt();
};


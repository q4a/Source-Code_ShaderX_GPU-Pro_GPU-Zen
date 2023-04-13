///////////////////////////////////////////////////////////////////
//
// project:     pgLib
// class:       pgIStringTool
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIStringTool.cpp,v 1.3 2003/01/03 19:02:05 daniel Exp $
///////////////////////////////////////////////////////////////////

#pragma warning(disable: 4514)

#include "pgIStringTool.h"

#include <pgLib/Math/pgVec2.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgVec4.h>

#include <stdlib.h>
#include <string.h>

const char* pgIStringTool::readLine(FILE* nFp)
{
	//char c = fgetc(nFp);

	static char line[1024];

	for(int pos=0; pos<1024; pos++)
	{
		line[pos] = static_cast<char>(fgetc(nFp));
		if(line[pos]=='\n' || feof(nFp))
		{
			line[pos] = 0;
			return line;
		}
	}

	line[pos] = 0;
	return line;
}


const char* pgIStringTool::skipSpaces(const char* nLine)
{
	while((*nLine==' ' || *nLine=='\t') && *nLine!=0)
		nLine++;

	return nLine;
}


const char* pgIStringTool::skipNonSpaces(const char* nLine)
{
	while(*nLine!=' ' && *nLine!='\t' && *nLine!=0)
		nLine++;

	return nLine;
}


const char* pgIStringTool::getPosAfter(const char* nLine, const char* nSearch)
{
	int i, sLen = strlen(nSearch);
	int	len = strlen(nLine) - sLen;

	for(i=0; i<len; i++)
		if(strncmp(nLine+i, nSearch, sLen)==0)
			return nLine+i+sLen;

	return NULL;
}


bool pgIStringTool::startsWith(const char* nStr, const char* nStart)
{
	unsigned int pos0;

	for(pos0=0; pos0<strlen(nStr); pos0++)
		if(nStr[pos0]!=' ' && nStr[pos0]!='\t')
			break;

	return strncmp(nStr+pos0, nStart, strlen(nStart))==0;
}


bool pgIStringTool::startsWithIgnoreCase(const char* nStr, const char* nStart)
{
	int		i, lenStr = strlen(nStr), lenStart = strlen(nStart);
	char	*str = new char[lenStr+1],
			*start = new char[lenStart+1];

	strcpy(str, nStr);
	strcpy(start, nStart);

	for(i=0; i<lenStr; i++)
		if(str[i]>='A' && str[i]<='Z')
			str[i] += ('a'-'A');

	for(i=0; i<lenStart; i++)
		if(start[i]>='A' && start[i]<='Z')
			start[i] += ('a'-'A');

	bool ret = startsWith(str, start);

	delete str;
	delete start;

	return ret;
}


bool pgIStringTool::isEmpty(const char* nStr)
{
	for(unsigned int pos=0; pos<strlen(nStr); pos++)
		if(nStr[pos]!=' ' && nStr[pos]!='\t' && nStr[pos]!='\n')
			return false;

	return true;
}


const char* pgIStringTool::getNextToken(const char* nStr, unsigned int& nPos, char nSeperator)
{
	static char string[1024];
	unsigned int pos, len=strlen(nStr);

	for(pos=0; nPos<len && nStr[nPos]!=nSeperator; nPos++,pos++)
		string[pos] = nStr[nPos];

	string[pos] = 0;
	nPos++;

	return string;
}


void pgIStringTool::fillTokenList(const char* nStr, char nSeperator, pgList<pgString>& nList, bool nSkipMultipleSeperators, int nNumTokens)
{
	unsigned int pos=0, len=strlen(nStr);

	if(nNumTokens)
		nList.enlargeList(nNumTokens);

	while(pos<len)
	{
		const char* token = getNextToken(nStr, pos, nSeperator);
		nList.addTail(token);

		if(nSkipMultipleSeperators)
			while(nStr[pos]==nSeperator)
				pos++;
	}
}


void pgIStringTool::removePathAndExtension(const char* nFullPath, char* nFileName)
{
	char	str[_MAX_PATH], *name;
	int		i;

	strcpy(str, nFullPath);
	name = str;

	// try to remove a path...
	//
	for(i=strlen(str)-1; i>=0; i--)
		if(str[i]=='\\' || str[i]=='/')
		{
			name = str+i+1;
			break;
		}

	// try to remove an extension...
	//
	for(i=strlen(name)-1; i>=0; i--)
		if(name[i]=='.')
		{
			name[i] = 0;
			break;
		}

	strcpy(nFileName, name);
}


void pgIStringTool::removeExtension(const char* nFullPath, char* nFileName)
{
	char	str[_MAX_PATH], *name;
	int		i;

	strcpy(str, nFullPath);
	name = str;

	// try to remove an extension...
	//
	for(i=strlen(name)-1; i>=0; i--)
		if(name[i]=='.')
		{
			name[i] = 0;
			break;
		}

	strcpy(nFileName, name);
}


void pgIStringTool::removePath(const char* nFullPath, char* nFileName)
{
	char	str[_MAX_PATH], *name;
	int		i;

	strcpy(str, nFullPath);
	name = str;

	// try to remove a path...
	//
	for(i=strlen(str)-1; i>=0; i--)
		if(str[i]=='\\' || str[i]=='/')
		{
			name = str+i+1;
			break;
		}

	strcpy(nFileName, name);
}



bool
pgIStringTool::readVec2(const char* nString, pgVec2& nVec2)
{
	int len = strlen(nString);
	char *strEnd, *str0, *str = new char[len+1];

	strcpy(str, nString);
	strEnd = str + len;

	for(int i=0; i<2; i++)
	{
		if(str>=strEnd)
			return false;

		str0 = (char*)skipSpaces(str);
		str = (char*)skipNonSpaces(str0);
		*str = 0;

		nVec2[i] = (float)atof(str0);
		str++;
	}

	return true;
}


bool
pgIStringTool::readVec3(const char* nString, pgVec3& nVec3)
{
	int len = strlen(nString);
	char *strEnd, *str0, *str = new char[len+1];

	strcpy(str, nString);
	strEnd = str + len;

	for(int i=0; i<3; i++)
	{
		if(str>=strEnd)
			return false;

		str0 = (char*)skipSpaces(str);
		str = (char*)skipNonSpaces(str0);
		*str = 0;

		nVec3[i] = (float)atof(str0);
		str++;
	}

	return true;
}


bool
pgIStringTool::readVec4(const char* nString, pgVec4& nVec4)
{
	int len = strlen(nString);
	char *strEnd, *str0, *str = new char[len+1];

	strcpy(str, nString);
	strEnd = str + len;

	for(int i=0; i<4; i++)
	{
		if(str>=strEnd)
			return false;

		str0 = (char*)skipSpaces(str);
		str = (char*)skipNonSpaces(str0);
		*str = 0;

		nVec4[i] = (float)atof(str0);
		str++;
	}

	return true;
}


void
pgIStringTool::replaceCharacter(char* nString, char nOld, char nNew)
{
	int i, len = strlen(nString);

	for(i=0; i<len; i++)
		if(nString[i]==nOld)
			nString[i] = nNew;
}

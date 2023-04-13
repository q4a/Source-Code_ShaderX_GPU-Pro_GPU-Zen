///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgOutTextFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgOutTextFile.cpp,v 1.1 2002/12/12 21:15:26 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgOutTextFile.h"


pgOutTextFile::~pgOutTextFile()
{
	close();
}


bool
pgOutTextFile::open(const pgString& nFullPath)
{
	fName = nFullPath;
	fp = fopen(nFullPath, "w");

	return fp!=NULL;
}


void
pgOutTextFile::close()
{
	if(fp)
		fclose(fp);
	fp = NULL;
}


int
pgOutTextFile::writeString(const char* nString)
{
	if(fp)
		return fwrite(nString, 1, strlen(nString), fp);
	else
		return 0;
}


int
pgOutTextFile::writeFormated(const char *nString,...)
{
	char tmpString[2048];
    va_list marker;

	va_start(marker, nString);
	vsprintf(tmpString,nString,marker);
	assert(strlen(tmpString)<2048);

	return writeString(tmpString);
}


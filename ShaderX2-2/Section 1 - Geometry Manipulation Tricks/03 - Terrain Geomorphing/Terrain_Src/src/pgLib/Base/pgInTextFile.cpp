///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgInTextFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgInTextFile.cpp,v 1.2 2002/12/12 21:15:26 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgInTextFile.h"


pgInTextFile::~pgInTextFile()
{
	close();
}

bool
pgInTextFile::open(const pgString& nFullPath)
{
	fName = nFullPath;
	fp = fopen(nFullPath, "r");

	if(fp==NULL)
	{
		fileSize = 0;
		return false;
	}

	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	return true;
}


void
pgInTextFile::close()
{
	if(fp)
		fclose(fp);
	fp = NULL;
}


int
pgInTextFile::read(unsigned char* nBuffer, int nMaxRead)
{
	return fread(nBuffer, 1, nMaxRead, fp);
}


int
pgInTextFile::getFileSize()
{
	return fileSize;
}


bool
pgInTextFile::eof()
{
	return fp ? (feof(fp)!=0) : false;
}


const char*
pgInTextFile::readLine()
{
	static char line[2049];

	for(int pos=0; pos<2048; pos++)
	{
		line[pos] = static_cast<char>(fgetc(fp));
		if(line[pos]=='\n' || feof(fp))
		{
			line[pos] = 0;
			return line;
		}
	}

	line[pos] = 0;
	return line;
}

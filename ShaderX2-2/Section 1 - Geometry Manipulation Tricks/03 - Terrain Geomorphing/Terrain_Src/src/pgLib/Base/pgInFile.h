///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgInFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgInFile.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __INFILE_HEADERFILE__
#define __INFILE_HEADERFILE__


#include <pgLib/pgLib.h>


/// Base class for file reading classes
class PGLIB_API pgInFile
{
public:
	virtual ~pgInFile()  {}

	virtual bool open(const class pgString& nFullPath) = 0;

	virtual void close() = 0;

	virtual int read(unsigned char* nBuffer, int nMaxRead) = 0;

	virtual bool eof() = 0;

	virtual int getFileSize() = 0;
};


#endif //__INFILE_HEADERFILE__


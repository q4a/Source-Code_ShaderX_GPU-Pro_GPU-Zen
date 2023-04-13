///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgOutFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgOutFile.h,v 1.1 2002/12/12 21:15:26 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __OUTFILE_HEADERFILE__
#define __OUTINFILE_HEADERFILE__


#include <pgLib/pgLib.h>


/// Base class for file writting classes
class PGLIB_API pgOutFile
{
public:
	virtual ~pgOutFile()  {}

	virtual bool open(const class pgString& nFullPath) = 0;

	virtual void close() = 0;
};


#endif //__OUTFILE_HEADERFILE__


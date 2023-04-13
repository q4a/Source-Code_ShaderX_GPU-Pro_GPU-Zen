///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgOutTextFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgOutTextFile.h,v 1.1 2002/12/12 21:15:26 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __OUTTEXTFILE_HEADERFILE__
#define __OUTTEXTFILE_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgInFile.h>
#include <pgLib/Base/pgString.h>

#include <stdio.h>


/// This class provides basic methods for writing text files
/**
 *  You should always use this class if you want to pass a text file object
 *  to a pgLib class, since standard c FILE pointers can not be passed over
 *  DLL boundaries (parts of the FILE objects data is stored as static members
 *  in the C lib and unfortunatelly not "DLL safe")
 */
class PGLIB_API pgOutTextFile
{
public:
	/// Creates a pgOutTextFile object which is not yet attached to a file
	pgOutTextFile() : fp(NULL)  {}

	virtual ~pgOutTextFile();


	/// Opens a text file in text mode. If the file could not be opened false is returned.
	virtual bool open(const pgString& nFullPath);


	/// Closes an open text file.
	/**
	 *  If the file is not open, this call is ignored.
	 */
	virtual void close();

	/// Writes a string to the file.
	/**
	 *  The number of bytes written is returned.
	 */
	virtual int writeString(const char* nString);


	/// Writes a formated string to the file.
	/**
	 *  The number of bytes written is returned.
	 */
	virtual int writeFormated(const char *nString,...);


protected:
	FILE*		fp;
	pgString	fName;
};


#endif //__OUTTEXTFILE_HEADERFILE__

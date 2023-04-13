///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgInTextFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgInTextFile.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __INTEXTFILE_HEADERFILE__
#define __INTEXTFILE_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgInFile.h>
#include <pgLib/Base/pgString.h>

#include <stdio.h>


/// This class provides basic methods for reading text files
/**
 *  You should always use this class if you want to pass a text file object
 *  to a pgLib class, since standard c FILE pointers can not be passed over
 *  DLL boundaries (parts of the FILE objects data is stored as static members
 *  in the C lib and unfortunatelly not "DLL safe")
 */
class PGLIB_API pgInTextFile
{
public:
	/// Creates a pgInTextFile object which is not yet attached to a file
	pgInTextFile() : fp(NULL)  {}

	virtual ~pgInTextFile();


	/// Opens a text file in text mode. If the file could not be opened false is returned.
	virtual bool open(const pgString& nFullPath);


	/// Closes an open text file.
	/**
	 *  If the file is not open, this call is ignored.
	 */
	virtual void close();

	/// Reads up to nMaxRead bytes into nBuffer
	/**
	 *  The return value resembles the number of actually read bytes.
	 */
	virtual int read(unsigned char* nBuffer, int nMaxRead);


	/// Returns the filesize of a file.
	/**
	 *  Before you can call this method a file must have been opened.
	 */
	virtual int getFileSize();


	/// Reads one line of a file and returns the data as a pointer.
	const char* readLine();


	/// Returns true if the end of the file was reached.
	virtual bool eof();

protected:
	FILE*		fp;
	int			fileSize;
	pgString	fName;
};


#endif //__INTEXTFILE_HEADERFILE__

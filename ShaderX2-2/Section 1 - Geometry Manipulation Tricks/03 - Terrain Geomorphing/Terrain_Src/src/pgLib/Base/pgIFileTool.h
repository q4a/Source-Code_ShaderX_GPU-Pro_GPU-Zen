///////////////////////////////////////////////////////////////////
//
// project:     Geisterschloss
// class:       pgIFileTool
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIFileTool.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __FILETOOL_HEADERFILE__
#define __FILETOOL_HEADERFILE__


#include <pgLib/pgLib.h>

#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgString.h>


/// This class provides methods for working with files
namespace pgIFileTool
{
	/// returns in nFileList all files which are in directory nPath and have the extension nExt
	/**
	 *	nExt has to be passed without dot ('.')
	 */
	PGLIB_API void getAllFiles(const char* nPath, const char* nExt, pgList<pgString>& nFileList);

};

#endif //__FILETOOL_HEADERFILE__

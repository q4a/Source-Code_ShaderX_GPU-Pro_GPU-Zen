///////////////////////////////////////////////////////////////////
//
// project:     Geisterschloss
// class:       pgIFileTool
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIFileTool.cpp,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////

#pragma warning(disable: 4514)

#include "pgIFileTool.h"

#include <io.h>


void
pgIFileTool::getAllFiles(const char* nPath, const char* nExt, pgList<pgString>& nFileList)
{
	struct _finddata_t	fileinfo;
	pgString			files = nPath + pgString("\\*.") + nExt,
						path = nPath + pgString("\\");
	long				handle;

	if((handle=_findfirst(files, &fileinfo)) != -1)
	{
		do
		{
			if(!(fileinfo.attrib & _A_SUBDIR))
				nFileList.addTail(path + fileinfo.name);
		} while(!_findnext(handle, &fileinfo));
	}
}

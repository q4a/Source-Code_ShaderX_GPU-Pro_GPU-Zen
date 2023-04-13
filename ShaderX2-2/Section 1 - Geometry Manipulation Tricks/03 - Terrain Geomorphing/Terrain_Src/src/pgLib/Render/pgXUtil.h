///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgXUtil
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgXUtil.h,v 1.3 2002/12/15 20:34:33 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __PGXUTIL_HEADERFILE__
#define __PGXUTIL_HEADERFILE__


#include <pgLib/pgLib.h>


class pgMesh;
class pgSegment;
class pgTransform;
class pgBaseMesh;
class pgMaterial;
class pgTextureStage;
class pgString;
class pgOutTextFile;


/// loads and saves PGX files
class PGLIB_API pgXUtil
{
public:
	static bool saveMesh(const pgString& nFileName, const pgMesh& nMesh);

	static pgMesh* loadMesh(const pgString& nFileName);

protected:
	static void saveSegment(pgOutTextFile& nFile, const pgSegment& nSegment);
	static void saveTransform(pgOutTextFile& nFile, const pgTransform* nTransform);
	static void saveMaterial(pgOutTextFile& nFile, const pgMaterial& nMaterial);
	static void saveTextureStage(pgOutTextFile& nFile, const pgTextureStage& nStage);
	static void saveBaseMesh(pgOutTextFile& nFile, const pgBaseMesh& nBaseMesh);
};


#endif //__PGXUTIL_HEADERFILE__

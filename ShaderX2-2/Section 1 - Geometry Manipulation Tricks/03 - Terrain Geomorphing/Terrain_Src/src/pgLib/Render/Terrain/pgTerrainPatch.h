///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrainPatch
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrainPatch.h,v 1.6 2002/11/14 22:32:53 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TERRAINPATCH_HEADERFILE__
#define __TERRAINPATCH_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Render/Terrain/pgTerrainPatchBase.h>


class PGLIB_API pgTerrainPatch : public pgTerrainPatchBase
{
public:
	pgTerrainPatch(pgTerrain* nMaster, int nX, int nY);


	virtual void init();

	virtual void deinit();

	virtual int render();

	virtual bool updateTessellation();

	virtual bool updateTessellation2();

	virtual bool updateTessellation3();

	virtual void deleteDeviceObjects();

	virtual bool restoreDeviceObjects();

protected:
	PDIRECT3DINDEXBUFFER8	iBuffer;
	PDIRECT3DVERTEXBUFFER8	vBuffer;


	bool checkBorders();
	bool fillBuffers();
	bool checkBuffers();
};


#endif //__TERRAINPATCH_HEADERFILE__

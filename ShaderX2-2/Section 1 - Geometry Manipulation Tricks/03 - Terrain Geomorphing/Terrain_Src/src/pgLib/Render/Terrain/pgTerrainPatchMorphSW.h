///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrainPatchMorphSW
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrainPatchMorphSW.h,v 1.9 2002/11/14 18:04:04 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TERRAINPATCHMORPHSW_HEADERFILE__
#define __TERRAINPATCHMORPHSW_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Render/Terrain/pgTerrainPatchBase.h>


class PGLIB_API pgTerrainPatchMorphSW : public pgTerrainPatchBase
{
public:
	pgTerrainPatchMorphSW(pgTerrain* nMaster, int nX, int nY);


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

	PATCHVERTEX				*verticesLow;
	float					*yMoveSelf, *yMoveLeft,*yMoveLeft2, *yMoveRight,*yMoveRight2, *yMoveBottom,*yMoveBottom2, *yMoveTop,*yMoveTop2;

	float					factorSelf, factorLeft,factorLeft2, factorRight,factorRight2, factorBottom,factorBottom2, factorTop,factorTop2;

	bool					newFollowsLeft,followsLeft, newFollowsRight,followsRight, newFollowsBottom,followsBottom, newFollowsTop,followsTop;
	bool					recalcBorderLeft, recalcBorderRight, recalcBorderBottom, recalcBorderTop;

	bool					forceRetessellation;

	virtual bool fillBuffers();
	bool checkBuffers();
	void fillMorphVertices();
	void checkBorderLeft();
	void checkBorderRight();
	void checkBorderBottom();
	void checkBorderTop();
};


#endif //__TERRAINPATCHMORPHSW_HEADERFILE__

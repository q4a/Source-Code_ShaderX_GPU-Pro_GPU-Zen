///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrainPatchMorphHW
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrainPatchMorphHW.h,v 1.5 2002/12/08 16:36:23 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TERRAINPATCHMORPHHW_HEADERFILE__
#define __TERRAINPATCHMORPHHW_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Render/Terrain/pgTerrainPatchMorphSW.h>


class PGLIB_API pgTerrainPatchMorphHW : public pgTerrainPatchMorphSW
{
public:
	pgTerrainPatchMorphHW(pgTerrain* nMaster, int nX, int nY);


	virtual void init();

	virtual void deinit();

	virtual int render();

	//virtual bool updateTessellation3();

	virtual void deleteDeviceObjects();

	virtual bool restoreDeviceObjects();

	static bool setupVertexShader();

	static bool setupTextureMatrix(D3DXMATRIX& nTexMatrix);

protected:
	static DWORD	morphShader;

	//PDIRECT3DVERTEXBUFFER8	mBuffer;

	virtual bool fillBuffers();
	bool checkBuffers();
	bool checkShader();
};


#endif //__TERRAINPATCHMORPHHW_HEADERFILE__

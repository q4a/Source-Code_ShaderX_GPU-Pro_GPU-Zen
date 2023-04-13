///////////////////////////////////////////////////////////////////
//
// project:     TerrainTest
// class:       TerrainTest
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: TerrainTest.h,v 1.3 2002/12/07 19:58:56 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TERRAINTEST_HEADERFILE__
#define __TERRAINTEST_HEADERFILE__

#include <pgLib/Base/pgITime.h>
#include <pgLib/Game/pgIGame.h>
#include <pgLib/Input/pgInputDX.h>
#include <pgLib/Input/pgSteering.h>
#include <pgLib/Render/Terrain/pgTerrain.h>
#include <pgLib/Render/pgLensflare.h>
#include <pgLib/Render/pgSkyBox.h>


// This class implements the pgGame interface to form
// the TerrainTest main programm. See pgGame for more
// documentation
//
class TerrainTest : public pgIGame
{
public:
	TerrainTest();

	virtual void init(HINSTANCE hInst, HWND hWnd);

	virtual void update();

	virtual void render();

	virtual void deleteDeviceObjects();

	virtual bool restoreDeviceObjects();

	virtual void invalidateDeviceObjects();

	virtual void cleanup();

protected:
	void updateInput();
	void showInfos();
	void deleteMyObjects();
	void appendPositionToFile(const char* nFileName);
	void readBenchSettings(const pgSettingsFile& nSet);
	pgString getBenchString();


	bool					initDone, firstTimeRender;
	int						initialFreeTexMem;

	bool					showInfosB, useMouseB;
	bool					writeBenchB;

	// terrain stuff
	//
	pgTerrain*				terrain;

	pgSkyBox*				skybox;
	pgLensflare*			lensflare;

	// input stuff
	//
	//pgInputDX				input;
	pgSteering				steering;

	// rendering stuff
	//
	class CD3DFont*			pFont;
	float					statTime;
	int						trisRendered, trisPerSec;

	pgList<pgVec3>			benchPos;
	pgList<pgEuler>			benchDir;
	int						benchIdx;
	pgTimeInstance			benchStartTime;
};

#endif //__TERRAINTEST_HEADERFILE__

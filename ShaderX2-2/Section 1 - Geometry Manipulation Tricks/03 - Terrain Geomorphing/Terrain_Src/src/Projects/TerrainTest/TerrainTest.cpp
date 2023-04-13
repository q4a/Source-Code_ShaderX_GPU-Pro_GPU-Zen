///////////////////////////////////////////////////////////////////
//
// project:     TerrainTest
// class:       TerrainTest
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: TerrainTest.cpp,v 1.16 2003/03/09 19:12:20 daniel Exp $
///////////////////////////////////////////////////////////////////


/******************************************************************

  TerrainTest tutorial file:

  In order to create a pgLib application a class has to be derived
  from pgIGame. In this sample this is the class TerrainTest.
  pgIGame forces the programmer to implement some pure virtual
  methods (have a look into pgIGame.h).
  At the beginning of the pgIGame implementing class file some
  preprocessor values have to be define:

    PG_GAMECLASSNAME            -  the name of the class derived
                                   from pgIGame
    PG_WINDOW_TITLE             -  title of the render window
    PG_WINDOW_WIDTH             -  width of the render window
	PG_WINDOW_HEIGHT            -  height of the render window
	PG_LOGFILE		            -  name of the log file
	PG_ALLOW_HARDWAREPROCESSING -  set to false to disable
                                   hardware vertex processing

  next pgLib/FrameWork/pgAppStub.cxx has to be implemented which
  contains the main window method (WinMain) and creates the basic
  application.

******************************************************************/

#define STRICT

#define PG_GAMECLASSNAME			TerrainTest
#define PG_WINDOW_TITLE				"  Terrain (20030419)  -   © 2003"
#define PG_WINDOW_WIDTH				800
#define PG_WINDOW_HEIGHT			600
#define PG_LOGFILE					"Terrain.log"
#define PG_CONFIG_FILENAME			"config.cfg"

#define TER_BENCHFILE				"bench.new"

#include "TerrainTest.h"

#include <pgLib/FrameWork/pgAppStub.cxx>

#include <pgLib/Base/pgIFileTool.h>
#include <pgLib/Base/pgProfiler.h>
#include <pgLib/Math/pgPathLinear.h>
#include <pgLib/Render/pgTextureStage.h>
#include <pgLib/Render/Tree/pgBlendTree.h>
#include <D3DX8.h>


TerrainTest::TerrainTest() : pFont(NULL)
{
	initDone = false;
	firstTimeRender = true;
	statTime = 0.0f;
	trisRendered = 0;
	trisPerSec = 0;

	terrain = NULL;
	skybox = NULL;
	lensflare = NULL;

	showInfosB = true;
	useMouseB = false;
	writeBenchB = false;
	benchIdx = 99999;
}


void
TerrainTest::init(HINSTANCE hInst, HWND hWnd)
{
	pgVec4	clearColor(0.0f, 0.0f, 1.0f, 1.0f);
	pgVec3	vec3;
	pgVec2	vec2;
	bool	mergePatches = false;

	//::SetWindowPos(theApp.getHWnd(), NULL, 0,0, 0,0, SWP_NOSIZE|SWP_NOZORDER);

	// we will use pgSettingsFile for
	// easy reading input settings of a cfg file
	pgSettingsFile	set;


	// create a font object which will later on
	// draw the on screen info stuff
	pFont = new CD3DFont(_T("Arial"), 10, D3DFONT_BOLD);
    if(FAILED(pFont->InitDeviceObjects(pgIDirectX::getDevice())))
		pgLog::error("creating font object failed. screen output will be disabled");


	// give an error message if no command line parameter was passed
	//
	if(programArgs.getSize()==0)
	{
		MessageBox(NULL, "\n\n                Please pass a .cfg file as parameter                \n\n", "Terrain ERROR", MB_OK);
		exit(0);
	}


	pgIDirectX::setSkyBoxHeightFactor(0.005f);

	// load a cfg file into the pgSettingsFile object
	// since the first values we read are optional we disable error logging
	set.load(programArgs[0]);
	set.setLogging(false);


	theApp.getConfigFile().getValueYes("MergeSmallestPatches", mergePatches);


	readBenchSettings(set);


	// use mouse
	theApp.getConfigFile().getValueYes("UseMouse", useMouseB);


	// init the input device. we use a directinput implementation
	// and let create a keyboard and a mouse device
	input = new pgInputDX();
	input->init(pgIInput::KEYBOARD | (useMouseB ? pgIInput::MOUSE : 0), hInst, hWnd);


	// clear color
	//
	set.getValueVec4("BackColor", clearColor);
	pgIDirectX::setClearColor(clearColor);


	// set the current camera position and direction
	//
	set.getValueVec3("CameraPos", vec3);
	steering.setNewPosition(vec3);

	//if(setupInput)
	steering.setup(theApp.getConfigFile());


	vec3 = pgVec3();							// default value: (0.0, 0.0, 0.0)
	set.getValueVec3("CameraDir", vec3);
	steering.setDirection(vec3);

	steering.setInspectModeSpeed(1500.0f, 1500.0f, 1500.0f, 40.0f);

	// inititialize the projection matrix. in this sample
	// we will not change it any more
	vec2 = pgVec2(50.0f, 10000.0f);				// default values
	set.getValueVec2("ClippingPlanes", vec2);
	pgIDirectX::setProjectionMatrix(4.0f/3.0f, pgFPI/4, vec2[0], vec2[1]);


	set.setLogging(true);

	///////////////////////////////////////////////////////////////////////////////////////////
	//
	//                                    Terrain Creation
	//

	bool					error = false;
	int						hmDX=0, hmDY=0;
	float					terError=0;
	pgString				hmFile,
							basePassTex, basePassLight,
							pass1Tex, pass1Light,
							pass2Tex, pass2Light,
							pass3Tex, pass3Light,
							pass4Tex, pass4Light,
							pass5Tex, pass5Light;
	pgVec2					basePassRepeat,
							pass1Repeat, pass2Repeat,
							pass3Repeat, pass4Repeat,
							pass5Repeat,
							fogRange;
	pgVec3					terSize;
	pgVec4					fogColor;
	pgString				cloudsTex;
	pgVec2					cloudsRepeat, cloudsSpeed;
	pgString				pvsName,
							renderModeStr,
							treeMap,
							tree0BaseName;
	int						tree0NumImgs=-1, tree0Idx=-1;
	pgVec2					tree0Size(200.0f, 300.0f);
	pgTerrain::RENDERMODE	renderMode;
	pgString				texFilterStr;
	pgTextureStage::FILTER	texFilter;


	// create a terrain object. after this we querry
	// settings from pgSettingsFile object. if the setting
	// was not loaded from the cfg file false is returned.
	terrain = new pgTerrain();

	if(!set.getValueString("HeightMap", hmFile))
		error = true;

	if(!set.getValueVec2("HeightMapSize", vec2))
		error = true;

	hmDX = (int)vec2.x;
	hmDY = (int)vec2.y;

	if(!set.getValueVec3("TerrainSize", terSize))
		error = true;

	if(!set.getValueFloat("MaxTerrainError", terError))
		error = true;

	if(!set.getValueString("BasePassTexture", basePassTex))
		error = true;

	//if(!set.getValueString("BasePassLightmap", basePassLight))
	//	error = true;
	set.getValueString("BasePassLightmap", basePassLight);

	if(!set.getValueVec2("BaseRepeat", basePassRepeat))
		error = true;


	// we will try to read up to five terrain layers
	// from the cfg file. since these are optional
	// we turn of logging in pgSettingsFile and ignore
	// the return value
	set.setLogging(false);
	set.getValueString("Pass1Texture", pass1Tex);
	set.getValueString("Pass1Lightmap", pass1Light);
	set.getValueVec2("Pass1Repeat", pass1Repeat);
	set.getValueString("Pass2Texture", pass2Tex);
	set.getValueString("Pass2Lightmap", pass2Light);
	set.getValueVec2("Pass2Repeat", pass2Repeat);
	set.getValueString("Pass3Texture", pass3Tex);
	set.getValueString("Pass3Lightmap", pass3Light);
	set.getValueVec2("Pass3Repeat", pass3Repeat);
	set.getValueString("Pass4Texture", pass4Tex);
	set.getValueString("Pass4Lightmap", pass4Light);
	set.getValueVec2("Pass4Repeat", pass4Repeat);
	set.getValueString("Pass5Texture", pass5Tex);
	set.getValueString("Pass5Lightmap", pass5Light);
	set.getValueVec2("Pass5Repeat", pass5Repeat);


	// fog settings
	//
	set.getValueVec2("FogRange", fogRange);
	set.getValueVec4("FogColor", fogColor);


	// filtering
	//
	set.getValueString("MipmapFilter", texFilterStr);
	texFilter = pgTextureStage::getFilterFromString(texFilterStr);

	// read the clouds settings from the cfg file
	//
	set.getValueString("Clouds", cloudsTex);
	set.getValueVec2("CloudsRepeat", cloudsRepeat);
	set.getValueVec2("CloudsSpeed", cloudsSpeed);


	// read a name for the treemap
	set.getValueString("TreeMap", treeMap);
	set.getValueVec2("Tree0Size", tree0Size);
	set.getValueString("TreeBaseName0", tree0BaseName);
	set.getValueInt("Tree0NumImgs", tree0NumImgs);
	set.getValueInt("TreeIdx0", tree0Idx);


	// read pvs settings
	set.getValueString("PVSName", pvsName);


	// read terrain rendermode
	set.getValueString("RenderMode", renderModeStr);
	renderMode = pgTerrain::getRenderModeFromString(renderModeStr);


	// if everything went ok until now we got all
	// the information we need and finally create
	// the terrain (have a look into the help file
	// for more information on terrain creation)
	if(!error)
	{
		int numPX = (hmDX-1)/16,
			numPY = (hmDY-1)/16;

		terrain->setRenderMode(renderMode);
		terrain->setMipmapFilter(texFilter);

		//terrain->setDesiredFPS(20);

		terrain->setPVSName(pvsName);


		// defines of how many patches the terrain consists
		//
		terrain->setNumPatches(numPX, numPY);


		// sets a heightmap. the heightmap's size must correlate
		// with the number of patches: (numPX^2 + 1) * (numPY^2 + 1)
		terrain->setHeightMap(pgIResourceManager::getRawImage(hmFile, hmDX,hmDY, pgImage::P8));


		// sets the base pass. this is the only pass which is demanded.
		// we use the resource manager to load the textures
		terrain->addPass(pgIResourceManager::getTexture(basePassTex),
						 basePassRepeat.x, basePassRepeat.y,
						 basePassLight.getLength() ? pgIResourceManager::getTexture(basePassLight) : NULL);

		// if there are settings for more passes then add them
		// 
		if(pass1Tex.getLength() && pass1Light.getLength() && pass1Repeat.x!=0.0f && pass1Repeat.y!=0.0f)
			terrain->addPass(pgIResourceManager::getTexture(pass1Tex),
							 pass1Repeat.x, pass1Repeat.y,
							 pgIResourceManager::getTexture(pass1Light));

		if(pass2Tex.getLength() && pass2Light.getLength() && pass2Repeat.x!=0.0f && pass2Repeat.y!=0.0f)
			terrain->addPass(pgIResourceManager::getTexture(pass2Tex),
							 pass2Repeat.x, pass2Repeat.y,
							 pgIResourceManager::getTexture(pass2Light));

		if(pass3Tex.getLength() && pass3Light.getLength() && pass3Repeat.x!=0.0f && pass3Repeat.y!=0.0f)
			terrain->addPass(pgIResourceManager::getTexture(pass3Tex),
							 pass3Repeat.x, pass3Repeat.y,
							 pgIResourceManager::getTexture(pass3Light));

		if(pass4Tex.getLength() && pass4Light.getLength() && pass4Repeat.x!=0.0f && pass4Repeat.y!=0.0f)
			terrain->addPass(pgIResourceManager::getTexture(pass4Tex),
							 pass4Repeat.x, pass4Repeat.y,
							 pgIResourceManager::getTexture(pass4Light));

		if(pass5Tex.getLength() && pass5Light.getLength() && pass5Repeat.x!=0.0f && pass4Repeat.y!=0.0f)
			terrain->addPass(pgIResourceManager::getTexture(pass5Tex),
							 pass5Repeat.x, pass5Repeat.y,
							 pgIResourceManager::getTexture(pass5Light));

		if(cloudsTex.getLength() && cloudsRepeat.x!=0.0f && cloudsRepeat.y!=0.0f && cloudsSpeed.x!=0.0f && cloudsSpeed.y!=0.0f)
			terrain->setSkyMap(pgIResourceManager::getTexture(cloudsTex),
							   cloudsRepeat.x, cloudsRepeat.y, cloudsSpeed.x, cloudsSpeed.y);


		terrain->setFogRange(fogRange[0], fogRange[1]);
		terrain->setFogColor(fogColor);


		if(treeMap.getLength())
  			terrain->setTreeMap(pgIResourceManager::getRawImage(treeMap, hmDX,hmDY, pgImage::P8));

		if(tree0BaseName.getLength() && tree0NumImgs!=-1 && tree0Idx!=-1)
		{
			pgBlendTree* tree = new pgBlendTree(tree0Size.x, tree0Size.y);
			for(int i=0; i<tree0NumImgs; i++)
				tree->addTexture(pgIResourceManager::getTexture(pgStringEx("%s%04d", tree0BaseName, i)));
			terrain->addTreeType(tree, tree0Idx);
		}


		// define the size (scale) of the terrain
		//
		terrain->setPatchSize(terSize.x/numPX, terSize.y, terSize.z/numPY);


		// define the maximum tesselation error before the
		// next finer tesselation is created
		terrain->setMaxError(terError);


		// sets wether the terrain will merge level-4 patches
		// and render as one buffer
		terrain->setMergePatches(mergePatches);

		// finally create the terrain
		//
		terrain->build();
	}
	else
	{
		pgLog::error("terrain setting could not be read correctly. terrain will not be rendered");
		delete terrain;
		terrain = NULL;
	}


	///////////////////////////////////////////////////////////////////////////////////////////
	//
	//                                    Skybox Creation
	//

	// next we create a sky box
	//
	bool createSkybox = false;


	// only if the value Skybox is set to yes we
	// are gonna create the skybox
	set.getValueYes("Skybox", createSkybox);
	if(createSkybox)
	{
		// set a default basename for the textures and
		// try to override it with one from the cfg file
		pgString baseName = "skybox";
		set.getValueString("SkyboxBaseName", baseName);

		skybox = new pgSkyBox(false);

		// load all the textures with the resource manager
		// and pass them to the skybox
		skybox->setTextures(pgIResourceManager::getTexture(baseName+"_ft"),
							pgIResourceManager::getTexture(baseName+"_bk"),
							pgIResourceManager::getTexture(baseName+"_lf"),
							pgIResourceManager::getTexture(baseName+"_rt"),
							pgIResourceManager::getTexture(baseName+"_up"),
							pgIResourceManager::getTexture(baseName+"_dn"));

		// finally the skybox creates internal stuff
		// (buffers, etc...)
		skybox->create();
	}


	///////////////////////////////////////////////////////////////////////////////////////////
	//
	//                                   Lens Flare Creation
	//

	// at last we create a cool lens flare
	bool createLensflare = false;


	// only if the value Lensflare is set to yes we
	// are gonna create the skybox
	set.getValueYes("Lensflare", createLensflare);

	if(createLensflare)
	{
		pgVec3	lfPos;
		float	intensity = 1.0f;


		// try to read the intensity for the flare
		//
		set.getValueFloat("LensflareIntensity", intensity);

		if(set.getValueVec3("LensflarePos", lfPos))
		{
			lensflare = new pgLensflare();
			lensflare->setPosition(lfPos);
			lensflare->setBaseAlpha(intensity);
			lensflare->loadStdFlareImages();
		}
	}


	set.getValueYes("ShowInfos", showInfosB);


	// store the amount of free texture memory. since no
	// frame was rendered yet. no texture has yet been loaded
	// into video memory...
	initialFreeTexMem = pgIDirectX::getAvailableTextureMemory();
	initDone = true;
}


// update is called by the framework
// before a frame is rendered.
void
TerrainTest::update()
{
	if(!initDone)
		return;

	if(writeBenchB)
		appendPositionToFile(TER_BENCHFILE);

	// update the global timing class
	// (must be done exactly once per frame)
	pgITime::update();


	// update audio processing
	//
	pgIAudio::update();


	// reset directx statistics
	// (triangle counting)
	pgIDirectX::resetStats();


	if(benchIdx<benchPos.getSize())
	{
		if(benchIdx==0)
			benchStartTime = pgITime::getCurrentTime();

		steering.setNewPosition(benchPos[benchIdx]);
		steering.setDirection(benchDir[benchIdx]);
		benchIdx++;

		input->update();
		if(input->isKeyDown(steering.getKeyBenchStop()))
			benchIdx = 99999;
	}
	else
	{
		if(benchIdx>0 && benchIdx==benchPos.getSize())
		{
			MessageBox(theApp.getHWnd(), getBenchString(), "Terrain", MB_OK);
			benchIdx++;
		}

		updateInput();
		steering.update(*input);
	}


	// take care, that we don't move below
	// the surface (does not work very well yet)
	pgVec3 newPos = steering.getNewPosition();
	if(terrain && terrain->moveAboveSurface(newPos, 500, 0.3f))
		steering.setNewPosition(newPos);


	// let steering create a view matrix
	// (as default it will automatically be
	// applied to directx)
	steering.createViewMatrix();


	// update the terrain (this we check if
	// any retesselation has to be done)
	if(terrain)
		terrain->update();
}


// render() is called every frame after
// update() has finished
void
TerrainTest::render()
{
	// check if we are not here to early and if
	// the render device is initialized correctly
	if(!initDone || !checkDevice("can not render scene: direct3d device invalid"))
		return;


	// calculate screen clearing flags: z-buffer will
	// always be cleared. color buffer only in wireframe,
	// because we don't need to clear because of the skybox.
	int flags = pgIDirectX::CLEAR_Z;

	if(!pgIDirectX::isFillSolid() || !skybox)
		flags |= pgIDirectX::CLEAR_COLOR;


	// renderBegin has to be called before the
	// first object can be rendered
	if(!pgIDirectX::renderBegin(flags))
		return;


	// render the skybox
	//
	if(skybox)
		skybox->render();


	// render the terrain
	//
	if(terrain)
		terrain->render();


	// render the lensflare
	//
	if(lensflare)
		lensflare->render();


	// show some interesting stuff
	// on the top of the screen
	showInfos();


	// finish rendering and let directx
	// show the result
	pgIDirectX::renderEnd();


	// if this is the first frame we calculate
	// some infos, which a writen into the log file
	if(firstTimeRender)
	{
		firstTimeRender = false;
		int freeMem = pgIDirectX::getAvailableTextureMemory();
		pgLog::info("Startup Done: %d MB free texture memory (Usage: %d MB)", freeMem, initialFreeTexMem-freeMem);
	}
}


// if the device needs to be recreated (e.g. when resizing
// or switching to fullscreen) all device dependend objects
// need to be destroyed
void
TerrainTest::deleteMyObjects()
{
	if(terrain)
		terrain->deleteDeviceObjects();

	if(skybox)
		skybox->deleteDeviceObjects();

	if(lensflare)
		lensflare->deleteDeviceObjects();
}


void
TerrainTest::deleteDeviceObjects()
{
	if(pFont)
		pFont->DeleteDeviceObjects();

	deleteMyObjects();
}


void
TerrainTest::invalidateDeviceObjects()
{
	if(pFont)
		pFont->InvalidateDeviceObjects();

	deleteMyObjects();
}


// after recreation of the device we recreate
// all our objects
bool
TerrainTest::restoreDeviceObjects()
{
	if(pFont)
		pFont->RestoreDeviceObjects();

	if(terrain)
		terrain->restoreDeviceObjects();

	if(skybox)
		skybox->restoreDeviceObjects();

	if(lensflare)
		lensflare->restoreDeviceObjects();

	return true;
}


// do some cleanup before the application
// quits...
void
TerrainTest::cleanup()
{
	input->cleanup();
	pgIAudio::cleanup();

	pgProfiler::logResult();
}


// update info from the input device
//
void
TerrainTest::updateInput()
{
	input->update();


	// switch between wireframe
	//
	if(input->isKeyNewDown(steering.getKeyToggleWireframe()))
		pgIDirectX::switchFillOperation();

	// switch between updating visibility
	//
	if(input->isKeyNewDown(steering.getKeyToggleVisFix()))
		pgIDirectX::switchUpdateVisibility();

	// switch between updating visibility
	//
	if(input->isKeyNewDown(steering.getKeyToggleSteeringMode()))
		steering.switchMode();

	// start/stop benchmark position writing/playing
	//
	if(input->isKeyDown(steering.getKeyBenchWriteStart()))
		writeBenchB = true;
	if(input->isKeyDown(steering.getKeyBenchWriteEnd()))
		writeBenchB = false;
	if(input->isKeyDown(steering.getKeyBenchStart()))
		benchIdx = 0;
}


// show some nice infos on the top of the screen
//
void
TerrainTest::showInfos()
{
	//return;
	statTime += pgITime::getLastFrameTime();
	trisRendered += pgIDirectX::getNumLevelTris();
	trisRendered += pgIDirectX::getNumTerrainTris();
	trisRendered += pgIDirectX::getNumNonLevelTris();
	trisRendered += pgIDirectX::getNumParticles();

	if(statTime>=1.0f)
	{
		trisPerSec = trisRendered;
		trisRendered = 0;
		statTime = 0.0f;
	}


	if(showInfosB)
	{
		pgStringEx	str("leveltris: %d  terrtris: %d  nonleveltris: %d  particles: %d",
						pgIDirectX::getNumLevelTris(), pgIDirectX::getNumTerrainTris(), pgIDirectX::getNumNonLevelTris(), pgIDirectX::getNumParticles()),
					str2("troughput: %.2fM triangles/sec", (float)trisPerSec/1000000.0f);

		pFont->DrawText(  2,  0, D3DCOLOR_ARGB(255,255,255,0), frameStats);
		pFont->DrawText(  2, 14, D3DCOLOR_ARGB(255,255,255,0), deviceStats);
		pFont->DrawText(  2, 28, D3DCOLOR_ARGB(255,255,255,0), str);
		pFont->DrawText(  2, 42, D3DCOLOR_ARGB(255,255,255,0), str2);

		steering.formatPositionString(str);
		pFont->DrawText(400,  0, D3DCOLOR_ARGB(255,255,255,255), str);
		steering.formatDirectionString(str);
		pFont->DrawText(400, 14, D3DCOLOR_ARGB(255,255,255,255), str);
		steering.formatModeString(str);
		pFont->DrawText(400, 28, D3DCOLOR_ARGB(255,255,255,255), str);


		pgVec3 downPos;
		if(terrain && terrain->projectDown(steering.getNewPosition(), downPos))
		{
			pgStringEx	str3("worldheight: %.2f", downPos.y);
			pFont->DrawText(  2, 60, D3DCOLOR_ARGB(255,0,255,255), str3);
		}

		if(terrain)
		{
			terrain->fillInfoString(str);
			pFont->DrawText(400, 50, D3DCOLOR_ARGB(255,0,255,255), str);

			terrain->fillInfoString2(str);
			pFont->DrawText(400, 64, D3DCOLOR_ARGB(255,255,255,255), str);
		}


		str.set("mousepos: %d %d", input->getMouseX(), input->getMouseY());
		pFont->DrawText(400, 90, D3DCOLOR_ARGB(255,255,255,255), str);
	}
}


void
TerrainTest::appendPositionToFile(const char* nFileName)
{
	FILE* fp = fopen(nFileName, "a");

	if(!fp)
		return;

	const pgVec3& pos = steering.getNewPosition();
	const pgEuler& dir = steering.getDirection();

	fprintf(fp, "%.2f %.2f %.2f | %.2f %.2f %.2f\n", pos.x,pos.y,pos.z, dir[0],dir[1],dir[2]);
	fclose(fp);
}


void
TerrainTest::readBenchSettings(const pgSettingsFile& nSet)
{
	pgString	benchFile;
	pgVec3		pos;
	pgEuler		dir;

	if(!nSet.getValueString("BenchmarkFile", benchFile))
		return;

	FILE* fp = fopen(benchFile, "r");
	if(!fp)
	{
		pgLog::error("benchmark file '%s' not found", benchFile);
		return;
	}

	while(!feof(fp))
	{
		fscanf(fp, "%f %f %f | %f %f %f\n", &pos.x,&pos.y,&pos.z, &dir.x,&dir.y,&dir.z);
		benchPos.addTail(pos);
		benchDir.addTail(dir);
	}

}


pgString
TerrainTest::getBenchString()
{
	float diff = pgITime::getTimeSince(benchStartTime);

	return pgStringEx("Benchmark rendered %d frames in %.2f seconds.\nThis results in an fps rate of\n\n                              %.2f\n", benchIdx, diff, benchIdx/diff);
}

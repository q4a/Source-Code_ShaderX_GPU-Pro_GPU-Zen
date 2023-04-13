///////////////////////////////////////////////////////////////////
//
// project:     Geisterschloss
// class:       pgISettings
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgISettings.cpp,v 1.2 2002/10/23 19:39:36 daniel Exp $
///////////////////////////////////////////////////////////////////

#pragma warning(disable: 4514)

#include "pgISettings.h"

#include <direct.h>
#include <string.h>
#include <stdlib.h>


char*		pgISettings::pathApp = NULL;
char*		pgISettings::pathAni = NULL;
char*		pgISettings::pathAudio = NULL;
char*		pgISettings::pathBSP = NULL;
char*		pgISettings::pathChar = NULL;
char*		pgISettings::pathGame = NULL;
char*		pgISettings::pathPath = NULL;
char*		pgISettings::pathLevel = NULL;
char*		pgISettings::pathMD2 = NULL;
char*		pgISettings::pathOBJ = NULL;
char*		pgISettings::pathPS = NULL;
char*		pgISettings::pathTex = NULL;
char*		pgISettings::pathTerrain = NULL;
char*		pgISettings::pathBspTex = NULL;


char* createFullPath(const char* nPath1, const char* nPath2)
{
	char* fullPath = new char[strlen(nPath1)+strlen(nPath2)+2];

	strcpy(fullPath, nPath1);
	strcat(fullPath, "\\");
	strcat(fullPath, nPath2);

	return fullPath;
}


void pgISettings::init()
{
	pathApp = new char[_MAX_PATH];

	_getcwd(pathApp, _MAX_PATH);

	pathAni = createFullPath(pathApp, "animation");
	pathAudio = createFullPath(pathApp, "audio");
	pathBSP = createFullPath(pathApp, "bsp");
	pathChar = createFullPath(pathApp, "game");
	pathGame = createFullPath(pathApp, "game");
	pathPath = createFullPath(pathApp, "path");
	pathLevel = createFullPath(pathApp, "level");
	pathMD2 = createFullPath(pathApp, "md2");
	pathOBJ = createFullPath(pathApp, "obj");
	pathPS = createFullPath(pathApp, "animation");
	pathTerrain = createFullPath(pathApp, "terrain");
	pathTex = createFullPath(pathApp, "texture");
	pathBspTex = createFullPath(pathApp, "bsp");
}


char* pgISettings::tmpFullPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	strcpy(fullPath, pathApp);
	strcat(fullPath, "\\");
	strcat(fullPath, nFileName);

	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToAniPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathAni);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullAniPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToAniPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToBSPPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathBSP);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullBSPPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToBSPPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToCharPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathChar);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullCharPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToCharPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToAudioPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathAudio);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullAudioPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToAudioPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToGamePath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathGame);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullGamePath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToGamePath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToPathPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathPath);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullPathPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToPathPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToLevelPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathLevel);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullLevelPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToLevelPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToMD2Path(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathMD2);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullMD2Path(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToMD2Path(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToOBJPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathOBJ);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullOBJPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToOBJPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToPsPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathPS);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullPsPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToAniPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToTerrainPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathTerrain);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullTerrainPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToTerrainPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToTexturePath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathTex);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullTexturePath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToTexturePath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToBspTexPath(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathBspTex);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullBspTexPath(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToBspTexPath(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}


void pgISettings::extendToTextureMD2Path(const char* nFileName, char* nFullName)
{
	strcpy(nFullName, pathMD2);
	strcat(nFullName, "\\");
	strcat(nFullName, nFileName);
}


char* pgISettings::tmpFullTextureMD2Path(const char* nFileName, const char* nExt)
{
	static char fullPath[_MAX_PATH];

	extendToTextureMD2Path(nFileName, fullPath);
	if(nExt)
		strcat(fullPath, nExt);
	return fullPath;
}

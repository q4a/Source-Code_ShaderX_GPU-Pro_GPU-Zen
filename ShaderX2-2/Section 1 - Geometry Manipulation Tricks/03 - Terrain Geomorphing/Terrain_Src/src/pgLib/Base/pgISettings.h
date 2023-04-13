///////////////////////////////////////////////////////////////////
//
// project:     pgLib
// class:       pgISettings
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgISettings.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////


///  Stores general Settings
/**
 * The class pgISettings has only static members, since
 * it is not expected and can not be instantiated.
 * Its primary work is to store global settings and do
 * path calculations.
 */
#ifndef __SETTINGS_HEADERFILE__
#define __SETTINGS_HEADERFILE__

#include <pgLib/pgLib.h>


class PGLIB_API pgISettings
{

public:
	/// init() has to be called before any path can be retrieved
	static void init();


	/// Returns the path where the exe file is located
	static const char* getAppPath()  { return pathApp; }


	/// Returns a temporary copy of an ani-extended path. nExt is added
	static char* tmpFullPath(const char* nFileName, const char* nExt=0);


	/// Returns the path where the ani files are stored
	static const char* getAniPath()  { return pathAni; }

	/// Extends a path by prefixing the bsp path
	static void extendToAniPath(const char* nFileName, char* nFullName);

	/// Returns a temporary copy of an ani-extended path. nExt is added
	static char* tmpFullAniPath(const char* nFileName, const char* nExt=0);


	/// Returns the path where the bsp files are stored
	static const char* getBSPPath()  { return pathBSP; }

	/// Extends a path by prefixing the bsp path
	static void extendToBSPPath(const char* nFileName, char* nFullName);

	/// Returns a temporary copy of an bsp-extended path. nExt is added
	static char* tmpFullBSPPath(const char* nFileName, const char* nExt=0);

	static const char* getAudioPath()  { return pathAudio; }
	static void extendToAudioPath(const char* nFileName, char* nFullName);
	static char* tmpFullAudioPath(const char* nFileName, const char* nExt=0);

	static const char* getCharPath()  { return pathChar; }
	static void extendToCharPath(const char* nFileName, char* nFullName);
	static char* tmpFullCharPath(const char* nFileName, const char* nExt=0);

	static const char* getGamePath()  { return pathGame; }
	static void extendToGamePath(const char* nFileName, char* nFullName);
	static char* tmpFullGamePath(const char* nFileName, const char* nExt=0);

	static const char* getPathPath()  { return pathPath; }
	static void extendToPathPath(const char* nFileName, char* nFullName);
	static char* tmpFullPathPath(const char* nFileName, const char* nExt=0);

	static const char* getLevelPath()  { return pathLevel; }
	static void extendToLevelPath(const char* nFileName, char* nFullName);
	static char* tmpFullLevelPath(const char* nFileName, const char* nExt=0);

	static void extendToMD2Path(const char* nFileName, char* nFullName);
	static char* tmpFullMD2Path(const char* nFileName, const char* nExt=0);

	static void extendToOBJPath(const char* nFileName, char* nFullName);
	static char* tmpFullOBJPath(const char* nFileName, const char* nExt=0);

	static void extendToPsPath(const char* nFileName, char* nFullName);			// extends a path by prefixing the bsp path
	static char* tmpFullPsPath(const char* nFileName, const char* nExt=0);		// returns a temporary copy of an bsp extended path

	static void extendToTerrainPath(const char* nFileName, char* nFullName);
	static char* tmpFullTerrainPath(const char* nFileName, const char* nExt);

	static const char* getTexturePath()  { return pathTex; }
	static void extendToTexturePath(const char* nFileName, char* nFullName);
	static char* tmpFullTexturePath(const char* nFileName, const char* nExt=0);

	static void extendToBspTexPath(const char* nFileName, char* nFullName);
	static char* tmpFullBspTexPath(const char* nFileName, const char* nExt=0);

	static void extendToTextureMD2Path(const char* nFileName, char* nFullName);
	static char* tmpFullTextureMD2Path(const char* nFileName, const char* nExt=0);


private:
	pgISettings();

	static char*		pathAni;
	static char*		pathApp;
	static char*		pathAudio;
	static char*		pathBSP;
	static char*		pathChar;
	static char*		pathGame;
	static char*		pathPath;
	static char*		pathLevel;
	static char*		pathMD2;
	static char*		pathOBJ;
	static char*		pathPS;
	static char*		pathSprite;
	static char*		pathTerrain;
	static char*		pathTex;
	static char*		pathBspTex;
};

#endif //__SETTINGS_HEADERFILE__

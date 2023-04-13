///////////////////////////////////////////////////////////////////
//
// project:     Geisterschloss
// class:       pgIGame
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIGame.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __GAME_HEADERFILE__
#define __GAME_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgString.h>
#include <pgLib/Render/pgD3DObject.h>


/// Basic class needed to be implemented to create an executable file
/**
 *  This class needs to be overridden in order to create a game.
 *  The framework calls the overridden methods as described below.
 *  See a sample project for more information.
 */

class pgIInput;


class PGLIB_API pgIGame : public pgD3DObject
{
public:
	pgIGame();


	/// Override this method and create all your objects here.
	/**
	 *  This method is called by the framework at the very beginning
	 *  of program creation. You should create all your objects here.
	 */
	virtual void init(HINSTANCE hInst, HWND hWnd) = NULL;

	/// Called before a frame is rendered
	/**
	 *  This method is called before render() is called.
	 *  Updates which are necessary before objects can
	 *  be rendered should be made here.
	 */
	virtual void update() = NULL;


	/// Called when a frame is rendered
	virtual void render() = NULL;


	/// This method is called by the framework (main class)
	/**
	 *  Two values are passed which can be used by the main game to display render information
	 */
	virtual void setStatistics(const char* nFrameStats, const char* nDeviceStats)
		{  frameStats = nFrameStats;  deviceStats = nDeviceStats;  }


	virtual void invalidateDeviceObjects() = NULL;


	/// This method is called directly before the programm quits.
	/**
	 *  Use the implementation of the method to do cleanup stuff.
	 */
	virtual void cleanup() = NULL;


protected:
	pgString	frameStats, deviceStats;
	pgIInput*	input;

private:
friend class pgAppStub;
	void callIInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


#endif //__GAME_HEADERFILE__

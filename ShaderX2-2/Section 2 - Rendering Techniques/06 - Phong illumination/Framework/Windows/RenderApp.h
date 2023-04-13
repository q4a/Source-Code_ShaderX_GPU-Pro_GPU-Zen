                  /*--.          .-"-. 
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#ifndef _RENDERAPP_H_
#define _RENDERAPP_H_

class RenderApp;

#include "RenderWindow.h"
#include "../PixelFormat.h"
#include "../Math/Matrix.h"
#include "../Renderer.h"
#include "../Util/Tokenizer.h"
#include "../Util/String.h"

struct DemoNode {
	Vertex pos;
	float Wx, Wy;
};

class RenderApp : public CWinApp {
private:
	String debugString;
	CREATE_ACTION createAction;

protected:
	Renderer *renderer;
	TextureID textTexture;

	Vertex position;
	float Wx, Wy, Wz;
	Matrix projection, modelView;
    
	int64 cpuHz;
	int64 prev, curr;
	float time, currTime, speed;
	DWORD soundVolume;

	bool keys[256];

	bool showFPS;
	bool showDebugInfo;


	// Benchmarking stuff
	bool demoRecMode;
	bool demoPlayMode;
	FILE *demoFile;
	DemoNode *demoArray;
	unsigned int demoSize;
	unsigned int demoFrameCounter;

public:
	DWORD leftKey, rightKey, upKey, downKey, forwardsKey, backwardsKey, resetKey, screenshotKey, showFpsKey, consoleKey, demoModeKey;

	virtual void selectPixelFormat(PixelFormat &pixelFormat) = 0;

	Renderer *getRenderer(){ return renderer; }
	RenderWindow *getRenderWindow(){ return (RenderWindow *) m_pMainWnd; }

	void setCreateAction(CREATE_ACTION action){	createAction = action; }

	virtual void resetCamera(){
		position = Vertex(0,0,0);
		Wx = 0;
		Wy = 0;
		Wz = 0;
	}

	virtual void setKey(UINT key, bool value);
	virtual void rotateView(float wx, float wy, float wz = 0);

	void setShowDebugInfo(const bool showDebug){ showDebugInfo = showDebug; }

	virtual String getErrorString() = 0;
	void setDebugString(const char *debugStr){ debugString = debugStr; }

	virtual void controls();
	virtual void processMovement(const Vertex &newPosition);

	virtual bool beginDemo(char *fileName);
	virtual void recordDemo();
	virtual void stopDemo();
	virtual bool loadDemo(char *fileName);


	virtual void setViewport(int cx, int cy) = 0;

    virtual BOOL InitInstance();
	virtual int Run();
	BOOL OnIdle(LONG lCount);
	int ExitInstance();

#ifndef NO_UI
	virtual bool processCommand(const char *cmd, String &response);
#endif

	virtual void processFullscreenMode(int &width, int &height, int &bpp, int &refreshRate){};

	virtual bool checkBaseCapabilities(){ return true; }
	virtual bool initAPI(HWND hwnd, int fsaaLevel, bool fullscreen, unsigned int refreshRate) = 0;
	virtual bool init(){ return true; };
	virtual bool exit(){ return true; };
	virtual bool load(){ return true; };
	virtual bool unload(){ return true; };
	virtual void beginFrame() = 0;
	virtual bool drawFrame() = 0;
	virtual void endFrame() = 0;
	virtual bool closeAPI(HWND hwnd) = 0;

	virtual void screenShot() = 0;
	virtual void drawText(char *str, float x, float y, float charWidth, float charHeight) = 0;
};

#endif // _RENDERAPP_H_

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

#ifndef _OPENGLAPP_H_
#define _OPENGLAPP_H_

#include "../Math/Matrix.h"
#include "../OpenGL/OpenGLRenderer.h"
#include "../Util/String.h"
#include "../Util/Tokenizer.h"
#include "../Util/Config.h"
#include "../Platform.h"
#include "../DisplayMode.h"
#include "../PixelFormat.h"

#include <X11/keysym.h>

class OpenGLApp {
private:
	String debugString;

    Window window;
	GLXContext ctx;
	Display *display;
	int screen;
	Cursor blankCursor;
	DisplayModeHandler *modes;

	String home, screenshotPath;
	Config config;
protected:
	Renderer *renderer;
	String m_lpCmdLine;

	TextureID textTexture;

	int mouseSpeed;
	int width, height;
	int left, right, top, bottom;
	int fsaaLevel;
	bool fullscreen;
	bool anisotropic;
	bool invertMouse;
	bool captureMouse;


	Vertex position;
	float Wx, Wy, Wz;
	Matrix projection, modelView;
	int middleX, middleY;

	uint64 cpuHz;
    float time, currTime, speed;
    uint64 prev, curr;
	//uint32 soundVolume;

	bool keys[65536];

	bool showFPS;
	bool showDebugInfo;
public:
	OpenGLApp(){}
	virtual ~OpenGLApp(){}

	uint32 leftKey, rightKey, upKey, downKey, forwardsKey, backwardsKey, resetKey, screenshotKey, showFpsKey, consoleKey, demoModeKey;

	virtual void resetCamera(){
		position = Vertex(0,0,0);
		Wx = 0;
		Wy = 0;
		Wz = 0;
	}

	virtual void setKey(UINT key, bool value);
	virtual void rotateView(float wx, float wy, float wz = 0){
		Wx += wx;
		Wy += wy;
		Wz += wz;
	}
	virtual void controls();
	virtual void processMovement(const Vertex &newPosition);

	void setShowDebugInfo(const bool showDebug){ showDebugInfo = showDebug; }
	void setDebugString(const char *debugStr){ debugString = debugStr; }

	virtual void selectPixelFormat(PixelFormat &pixelFormat) = 0;

	String getErrorString();
	bool processCommand(const char *cmd, String &response);

	bool InitInstance(const String &commandLine);
	bool OnIdle(long lCount);
	int Run();
	bool ExitInstance();

	bool initAPI(Display *disp, int scr);
	virtual bool init(){ return true; };
	virtual bool exit(){ return true; };
	virtual bool load();
	virtual bool unload();
	//virtual void beginFrame();
	virtual bool drawFrame();
	//virtual void endFrame();
	bool closeAPI();

	virtual void setViewport(int cx, int cy);
	void screenShot();
	void drawText(char *str, float x, float y, float charWidth, float charHeight);

};

#endif // _OPENGLAPP_H_

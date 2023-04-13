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

#include "RenderApp.h"

void RenderApp::setKey(UINT key, bool value){
	keys[key & 0xFF] = value;
	
	if (value){
		if (key == showFpsKey){
			showFPS = !showFPS;
		} else if (key == screenshotKey){
			screenShot();
		}
	}	
}

void RenderApp::controls(){
    float sqrLen;

	Vertex dir(0,0,0);

	Vertex dx(modelView[0], modelView[4], modelView[8]);
	Vertex dy(modelView[1], modelView[5], modelView[9]);
	Vertex dz(modelView[2], modelView[6], modelView[10]);

	if (keys[leftKey ]) dir -= dx;
	if (keys[rightKey]) dir += dx;

	if (keys[backwardsKey]) dir -= dz;
	if (keys[forwardsKey ]) dir += dz;

	if (keys[downKey]) dir -= dy;
	if (keys[upKey  ]) dir += dy;

	if (keys[resetKey]) resetCamera();

	if ((sqrLen = lengthSqr(dir)) != 0){
		dir *= rsqrtf(sqrLen);
	}

	processMovement(position + time * speed * dir);
}

void RenderApp::processMovement(const Vertex &newPosition){
	position = newPosition;
}

BOOL RenderApp::InitInstance(){
/*#ifdef _DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	flag |= _CRTDBG_CHECK_ALWAYS_DF; // Turn on CrtCheckMemory 
	_CrtSetDbgFlag(flag); // Set flag to the new value
#endif*/
    initCPU();
    cpuHz = getHz();
	speed = 1024;

	memset(keys, 0, sizeof(keys));

	leftKey       = VK_LEFT;
	rightKey      = VK_RIGHT;
	upKey         = VK_CONTROL;
	downKey       = VK_SHIFT;
	forwardsKey   = VK_UP;
	backwardsKey  = VK_DOWN;
	resetKey      = VK_ENTER;
	screenshotKey = VK_F9;
	showFpsKey    = VK_SPACE;
	consoleKey    = VK_TAB;
	demoModeKey   = 'F';

	soundVolume = 100;

	CRegKey regKey;
	if (regKey.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus") == ERROR_SUCCESS){
		regKey.QueryValue(leftKey,      "LeftKey");
		regKey.QueryValue(rightKey,     "RightKey");
		regKey.QueryValue(upKey,        "UpKey");
		regKey.QueryValue(downKey,      "DownKey");
		regKey.QueryValue(forwardsKey,  "ForwardsKey");
		regKey.QueryValue(backwardsKey, "BackwardsKey");
		regKey.QueryValue(resetKey,     "ResetKey");
		regKey.QueryValue(screenshotKey,"ScreenshotKey");
		regKey.QueryValue(showFpsKey,   "ShowFpsKey");

		regKey.QueryValue(soundVolume,  "SoundVolume");
		regKey.Close();
	}

	showFPS = true;
	showDebugInfo = false;
	textTexture = TEXTURE_NONE;

	init();

	m_pMainWnd = new RenderWindow(this, createAction = CREATE_DEFAULT);

	resetCamera();
	currTime = 0;
	curr = getCycleNumber();

	return TRUE;
}

// Ripped and slightly changed MFC WinApp::Run() method to better fit our purposes
int RenderApp::Run(){
	int idleCount = 0;
	bool idle = true;
	bool done = false;

#if (_MFC_VER < 0x700)
#define currMsg &m_msgCur
#else
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	LPMSG currMsg = &(pState->m_msgCur);
#endif
	// acquire and dispatch messages until a WM_QUIT message is received.
	do {
		// check to see if we can do idle work
		while (idle && !::PeekMessage(currMsg, NULL, NULL, NULL, PM_NOREMOVE)){
			if (!OnIdle(idleCount++)) idle = false;
		}

		do {
			if (!PumpMessage()){
				done = true;
				break;
			}

			if (IsIdleMessage(currMsg)){
				idle = true;
				idleCount = 0;
			}
		} while (::PeekMessage(currMsg, NULL, NULL, NULL, PM_NOREMOVE));

		if (createAction != TERMINATE){
			#if defined(_DEBUG) && (_MFC_VER < 0x700)
				m_nDisablePumpCount = 0;
			#endif
			done = false;

			delete m_pMainWnd;
			m_pMainWnd = new RenderWindow(this, createAction);
		}
	} while (!done || createAction != TERMINATE);

	return ExitInstance();
}

BOOL RenderApp::OnIdle(LONG lCount){
    prev = curr;
    curr = getCycleNumber();
    time = (float) (double(curr - prev) / cpuHz);
    currTime += time;

	controls();
	beginFrame();
	bool result = drawFrame();
	if (showFPS && (textTexture != TEXTURE_NONE)){
		renderer->setTextures(textTexture);
		renderer->setTexEnvs(TEXENV_REPLACE);
		renderer->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		renderer->apply();

		static float fps[15];
		static int currPos = 0;

		fps[currPos] = 1.0f / time;
		currPos++;
		if (currPos > 14) currPos = 0;

		// Apply a median filter to get rid of temporal peeks
		float min = 0, cmin;
		for (int i = 0; i < 8; i++){
			cmin = 1e30f;
			for (int j = 0; j < 15; j++){
				if (fps[j] > min && fps[j] < cmin){
					cmin = fps[j];						
				}
			}
			min = cmin;
		}

		String str;
		str.sprintf("%d", (int) (min + 0.5f));

		drawText((char *) (const char *) str, 0.02f, 0.02f, 0.035f, 0.05f);
	}
	if (showDebugInfo && (textTexture != TEXTURE_NONE)){
		renderer->setTextures(textTexture);
		renderer->setTexEnvs(TEXENV_REPLACE);
		renderer->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		renderer->apply();

		if (!debugString.isEmpty()){
			drawText((LPSTR) (LPCSTR) ("Debug:\n" + debugString), 0.02f, 0.3f, 0.025f, 0.04f);
		}
		
		String error = getErrorString();
		
		int err;
		if (err = GetLastError()){
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
			if (error.isEmpty()){
				error = (LPCSTR) lpMsgBuf;
			} else {
				error += ',';
				error += (LPCSTR) lpMsgBuf;
			}
			LocalFree(lpMsgBuf);
			SetLastError(0);
		}
		if (!error.isEmpty()){
			drawText((LPSTR) (LPCSTR) ("Errors:\n" + error), 0.02f, 0.6f, 0.025f, 0.04f);
		}
	}
	endFrame();

	return result;
}

int RenderApp::ExitInstance(){
	exit();

	CRegKey regKey;
	if (regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus") == ERROR_SUCCESS){
		regKey.SetValue(leftKey,      "LeftKey");
		regKey.SetValue(rightKey,     "RightKey");
		regKey.SetValue(upKey,        "UpKey");
		regKey.SetValue(downKey,      "DownKey");
		regKey.SetValue(forwardsKey,  "ForwardsKey");
		regKey.SetValue(backwardsKey, "BackwardsKey");
		regKey.SetValue(resetKey,     "ResetKey");
		regKey.SetValue(screenshotKey,"ScreenshotKey");
		regKey.SetValue(showFpsKey,   "ShowFpsKey");
		regKey.SetValue(consoleKey,   "ConsoleKey");

		//regKey.SetValue(soundVolume,  "SoundVolume");
		regKey.Close();
	}
	return 0;
}

bool RenderApp::processCommand(const char *cmd, String &response){
	char str[256];

	Tokenizer tok;
	tok.setString(cmd);
	tok.next(str);

	if (stricmp(str, "pos") == 0){
		response.sprintf("(%f, %f, %f)", position.x, position.y, position.z);
	} else if (stricmp(str, "setpos") == 0){
		tok.next(str);
		if (str[0] == '-'){
			tok.next(str);
			position.x = -(float) atof(str);
		} else position.x = (float) atof(str);
		tok.next(str);
		if (*str == ',') tok.next(str);
		if (str[0] == '-'){
			tok.next(str);
			position.y = -(float) atof(str);
		} else position.y = (float) atof(str);
		tok.next(str);
		if (*str == ',') tok.next(str);
		if (str[0] == '-'){
			tok.next(str);
			position.z = -(float) atof(str);
		} else position.z = (float) atof(str);
	} else if (stricmp(str, "angles") == 0){
		response.sprintf("(%f, %f, %f)", Wx, Wy, Wz);
	} else if (stricmp(str, "setangles") == 0){
		tok.next(str);
		if (str[0] == '-'){
			tok.next(str);
			Wx = -(float) atof(str);
		} else Wx = (float) atof(str);
		tok.next(str);
		if (*str == ',') tok.next(str);
		if (str[0] == '-'){
			tok.next(str);
			Wy = -(float) atof(str);
		} else Wy = (float) atof(str);
		tok.next(str);
		if (*str == ',') tok.next(str);
		if (str[0] == '-'){
			tok.next(str);
			Wz = -(float) atof(str);
		} else Wz = (float) atof(str);
	} else {
		return false;
	}

	return true;
}

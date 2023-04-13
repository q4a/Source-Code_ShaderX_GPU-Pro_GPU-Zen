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

#ifndef _RENDERWINDOW_H_
#define _RENDERWINDOW_H_

#include <afxwin.h>
#include <atlbase.h>
#include <afxcmn.h>
#include "../CPU.h"


#include "../Math/Vertex.h"
#include "../DisplayMode.h"

#define VK_ENTER 13

#define ID_FILE_EXIT                     100
#define ID_FILE_RESTART                  101
#define ID_FILE_CONSOLE                  102
#define ID_FULLSCREEN                    200
#define ID_PREFERENCES_CONFIGURECONTROLS 300
#define ID_PREFERENCES_INVERTMOUSE       301
#define ID_PREFERENCES_VSYNC             302
#define ID_PREFERENCES_ANISOTROPIC       303
#define ID_PREFERENCES_SHOWDEBUGINFO     304
#define ID_PREFERENCES_FSAA_OFF          305
#define ID_PREFERENCES_FSAA_2X           306
#define ID_PREFERENCES_FSAA_4X           307
#define ID_PREFERENCES_FSAA_6X           308
#define ID_HELP_ABOUT                    400

#define IDC_COMMANDLINE  1000
#define IDC_OUTPUT       1001

enum CREATE_ACTION {
	TERMINATE,
	CREATE_DEFAULT,
	CREATE_FULLSCREEN,
	CREATE_WINDOWED
};

class RenderWindow;

#include "RenderApp.h"


class RenderWindow : public CFrameWnd {
public:
    RenderWindow(RenderApp *app, CREATE_ACTION createAction);

	void setMouseSpeed(const DWORD speed){ mouseSpeed = speed; }
	DWORD getMouseSpeed() const { return mouseSpeed; }
protected:
	RenderApp *renderApp;

	DisplayModeHandler *modes;
	UINT prevCheckedMode;

	CRichEditCtrl commandLine, output;


	CRect rect;
	int width, height, bpp, refreshRate;
	int middleX, middleY;
	bool fullScreen;
	bool captureMouse;

	bool leftButton, rightButton;
	DWORD mouseSpeed;
	DWORD fsaaLevel;
	bool colorMipMaps;
	bool invertMouse;

	bool vSync, anisotropic, showDebugInfo;
	bool loaded;


	BOOL PreCreateWindow(CREATESTRUCT &cs);
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
	void OnWindowPosChanged(WINDOWPOS *lpwndpos);
	void OnClose();

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp  (UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp  (UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnRButtonUp  (UINT nFlags, CPoint point);
	void OnMouseMove  (UINT nFlags, CPoint point);

	void OnKillFocus(CWnd *pNewWnd);

	void OnCommandLineNotify(NMHDR *pNMHDR, LRESULT *pResult);

	void OnFileExit();
	void OnFileRestart();
	void OnFileConsole();
	void OnFullscreen();
	void OnFullscreenMode(UINT nID);
	void OnPreferencesConfigureControls();
	void OnPreferencesInvertMouse();
	void OnPreferencesVSync();
	void OnPreferencesAnisotropic();
	void OnPreferencesFsaa(UINT nID);
	void OnShowDebugInfo();
	void OnHelpAbout();

	DECLARE_MESSAGE_MAP ()
};

#endif // _RENDERWINDOW_H_

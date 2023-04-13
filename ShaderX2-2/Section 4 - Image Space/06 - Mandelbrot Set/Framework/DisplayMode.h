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

#ifndef _DISPLAYMODE_H_
#define _DISPLAYMODE_H_

#if defined(WIN32)
#include <windows.h>
#elif defined(LINUX)
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#endif

#include "Util/Set.h"

class DisplayMode {
public:
	DisplayMode(int i, int w, int h, int b, int r){
		index  = i;
		width  = w;
		height = h;
		bpp    = b;
		refreshRate = r;
	}

	int width, height;
	int bpp;
	int refreshRate;
	int index;

	unsigned int dist(int w, int h, int b, int r);
};


class DisplayModeHandler {
protected:
	Set <DisplayMode> modes;
	int nModes;

#if defined(WIN32)
	Set <DEVMODE> dmodes;
public:
	DisplayModeHandler();
#elif defined(LINUX)
    XF86VidModeModeInfo **dmodes;
	Display *display;
	int screen;
public:
	DisplayModeHandler(Display *disp, int scr);

#endif

	~DisplayModeHandler();

	DisplayMode &operator [] (const unsigned int index) const {
		return modes[index];
	}
	int getNumberOfDisplayModes() const {
		return modes.getSize();
	}
	void filterModes(int widthLow, int heightLow, int bppLow, int widthHigh = 0x7fffffff, int heightHigh = 0x7fffffff, int bppHigh = 0x7fffffff, int refreshRateLow = 0, int refreshRateHigh = 0x7fffffff);
	void filterRefreshes(int frequency);

	bool setDisplayMode(unsigned int index) const;
	bool setDisplayMode(int width, int height, int bpp, int refreshRate) const;
	bool resetDisplayMode();
};

#endif // _DISPLAYMODE_H_

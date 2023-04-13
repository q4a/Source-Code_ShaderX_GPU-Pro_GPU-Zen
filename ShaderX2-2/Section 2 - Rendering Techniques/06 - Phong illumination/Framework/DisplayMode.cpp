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

#include "DisplayMode.h"

unsigned int DisplayMode::dist(int w, int h, int b, int r){
	return (abs(bpp - b) << 16) + (abs(width - w) << 8) + (abs(height - h) << 8) + abs(refreshRate - r);
}


#if defined(WIN32)
DisplayModeHandler::DisplayModeHandler(){
	DEVMODE mode;
	int i = 0;
	while (EnumDisplaySettings(NULL, i, &mode)){
		modes.add(DisplayMode(i, mode.dmPelsWidth, mode.dmPelsHeight, mode.dmBitsPerPel, mode.dmDisplayFrequency));
		dmodes.add(mode);
		i++;
	}
	nModes = dmodes.getSize();
#elif defined(LINUX)
DisplayModeHandler::DisplayModeHandler(Display *disp, int scr){
	int i;

    display = disp;
    screen = scr;
    XF86VidModeGetAllModeLines(display, screen, &nModes, &dmodes);
	for (i = 0; i < nModes; i++){
		modes.add(DisplayMode(i, dmodes[i]->hdisplay, dmodes[i]->vdisplay, 32, (int) (0.5f + (dmodes[i]->dotclock * 1000.0f) / (dmodes[i]->htotal * dmodes[i]->vtotal))));
	}
#endif

	// Sort
	bool changed;
	int len = modes.getSize() - 1;
	do {
		changed = false;
		for (i = 0; i < len; i++){
			if ((modes[i].width * modes[i].height) > (modes[i+1].width * modes[i+1].height)){
				DisplayMode tmp = modes[i];
				modes[i] = modes[i+1];
				modes[i+1] = tmp;
				changed = true;
			}
		}
	} while (changed);
}

DisplayModeHandler::~DisplayModeHandler(){
#ifdef LINUX
    XFree(dmodes);
#endif
}


void DisplayModeHandler::filterModes(int widthLow, int heightLow, int bppLow, int widthHigh, int heightHigh, int bppHigh, int refreshRateLow, int refreshRateHigh){
	unsigned int i = 0;

	while (i < modes.getSize()){
		if (modes[i].width  < widthLow  || modes[i].width  > widthHigh  ||
			modes[i].height < heightLow || modes[i].height > heightHigh ||
			modes[i].bpp    < bppLow    || modes[i].bpp    > bppHigh    ||
			modes[i].refreshRate < refreshRateLow || modes[i].refreshRate > refreshRateHigh){
			modes.orderedRemove(i);
		} else i++;
	}
}

void DisplayModeHandler::filterRefreshes(int frequency){
	unsigned int i, j;
	int closest, currWidth, currHeight;

	i = 1;
	currWidth  = modes[0].width;
	currHeight = modes[0].height;
	closest    = modes[0].refreshRate;

	do {
		if (i == modes.getSize() || modes[i].width != currWidth || modes[i].height != currHeight){
			j = i - 1;
			while (j >= 0 && modes[j].width == currWidth && modes[j].height == currHeight){
				if (modes[j].refreshRate != closest){
					modes.orderedRemove(j);
					i--;
				}
				j--;
			}

			if (i == modes.getSize()) return;

			currWidth  = modes[i].width;
			currHeight = modes[i].height;
			closest    = modes[i].refreshRate;
		}
		
		if (abs(modes[i].refreshRate - frequency) < abs(closest - frequency)){
			closest = modes[i].refreshRate;
		}
		i++;
	} while (true);
}

bool DisplayModeHandler::setDisplayMode(unsigned int index) const {
	if (index >= 0 && index < modes.getSize()){

#if defined(WIN32)
		return (ChangeDisplaySettings(&dmodes[modes[index].index], CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL);
#elif defined(LINUX)
        if (XF86VidModeSwitchToMode(display, screen, dmodes[modes[index].index])){
			XF86VidModeSetViewPort(display, screen, 0, 0);
			return true;
		}
#endif
	}
	return false;
}

bool DisplayModeHandler::setDisplayMode(int width, int height, int bpp, int refreshRate) const {
	int len = modes.getSize();

	if (len > 0){
		int index = -1;
		unsigned int minDist = 0x7FFFFFFF;

		for (int i = 0; i < len; i++){
			unsigned int dist = modes[i].dist(width, height, bpp, refreshRate);
			if (dist < minDist){
				minDist = dist;
				index = i;
			}
		}
		return setDisplayMode(index);
	}
	return false;
}

#if defined(WIN32)

bool DisplayModeHandler::resetDisplayMode(){
	return (ChangeDisplaySettings(NULL, 0) == DISP_CHANGE_SUCCESSFUL);
}

#elif defined(LINUX)

bool DisplayModeHandler::resetDisplayMode(){
	if (XF86VidModeSwitchToMode(display, screen, dmodes[0])){
		XF86VidModeSetViewPort(display, screen, 0, 0);
		return true;
	}
	return false;
}

#endif

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

#include "ControlsDialog.h"
#include "Resource.h"

struct KeyDef {
	UINT charCode;
	char *string;
};

static const KeyDef keyDefs[] = {
	VK_TAB,     "Tab",
	VK_SHIFT,   "Shift",
	VK_CONTROL, "Ctrl",
	VK_RETURN,  "Enter",
	VK_BACK,    "Backspace",
	VK_SPACE,   "Space",
	VK_HOME,    "Home",
	VK_END,     "End",
	VK_INSERT,  "Insert",
	VK_DELETE,  "Delete",
	VK_LEFT,    "Left",
	VK_RIGHT,   "Right",
	VK_UP,      "Up",
	VK_DOWN,    "Down",

	'0', "0", '1', "1", '2', "2", '3', "3", '4', "4", '5', "5", '6', "6", '7', "7", '8', "8", '9', "9",
	'A', "A", 'B', "B", 'C', "C", 'D', "D", 'E', "E", 'F', "F", 'G', "G", 'H', "H", 'I', "I", 'J', "J",
	'K', "K", 'L', "L", 'M', "M", 'N', "N", 'O', "O", 'P', "P", 'Q', "Q", 'R', "R", 'S', "S", 'T', "T",
	'U', "U", 'V', "V", 'W', "W", 'X', "X", 'Y', "Y", 'Z', "Z",

	VK_NUMPAD0,  "Numpad 0", VK_NUMPAD1,  "Numpad 1", VK_NUMPAD2,  "Numpad 2", VK_NUMPAD3,  "Numpad 3", VK_NUMPAD4,  "Numpad 4", 
	VK_NUMPAD5,  "Numpad 5", VK_NUMPAD6,  "Numpad 6", VK_NUMPAD7,  "Numpad 7", VK_NUMPAD8,  "Numpad 8", VK_NUMPAD9,  "Numpad 9", 

	VK_ADD,      "+",
	VK_SUBTRACT, "-",
	VK_MULTIPLY, "*",
	VK_DIVIDE,   "/",
	VK_SEPARATOR,"Separator",
	VK_DECIMAL,  "Decimal",

	VK_F1,  "F1", VK_F2,  "F2", VK_F3,  "F3", VK_F4,  "F4",  VK_F5,  "F5",  VK_F6,  "F6",
	VK_F7,  "F7", VK_F8,  "F8", VK_F9,  "F9", VK_F10, "F10", VK_F11, "F11", VK_F12, "F12",

	VK_PAUSE,   "Pause",
	VK_CAPITAL, "Caps Lock",
	VK_NUMLOCK, "Num Lock",
	VK_SCROLL,  "Scroll Lock",
};




ControlsDialog::ControlsDialog(RenderApp *renderapp) : CDialog(IDD_CONTROLSDIALOG){
	renderApp = renderapp;
}

BOOL ControlsDialog::OnInitDialog(){
	for (int i = 0; i < sizeof(keyDefs) / sizeof(KeyDef); i++){
		for (int idc = IDC_LEFT; idc <= IDC_DEMOMODE; idc++){
			SendDlgItemMessage(idc, CB_ADDSTRING, 0, (LPARAM) keyDefs[i].string);
		}

		if (keyDefs[i].charCode == renderApp->leftKey)       SendDlgItemMessage(IDC_LEFT,      CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->rightKey)      SendDlgItemMessage(IDC_RIGHT,     CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->upKey)         SendDlgItemMessage(IDC_UP,        CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->downKey)       SendDlgItemMessage(IDC_DOWN,      CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->forwardsKey)   SendDlgItemMessage(IDC_FORWARDS,  CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->backwardsKey)  SendDlgItemMessage(IDC_BACKWARDS, CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->resetKey)      SendDlgItemMessage(IDC_RESET,     CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->screenshotKey) SendDlgItemMessage(IDC_SCREENSHOT,CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->showFpsKey)    SendDlgItemMessage(IDC_SHOWFPS,   CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->consoleKey)    SendDlgItemMessage(IDC_CONSOLE,   CB_SETCURSEL, i);
		if (keyDefs[i].charCode == renderApp->demoModeKey)   SendDlgItemMessage(IDC_DEMOMODE,  CB_SETCURSEL, i);
	}

	SendDlgItemMessage(IDC_MOUSESPEED, TBM_SETRANGE, FALSE, MAKELONG(1,10));
	SendDlgItemMessage(IDC_MOUSESPEED, TBM_SETPOS,   TRUE,  renderApp->getRenderWindow()->getMouseSpeed());


	return TRUE;
}

void ControlsDialog::OnOK(){
	renderApp->leftKey       = keyDefs[SendDlgItemMessage(IDC_LEFT,      CB_GETCURSEL)].charCode;
	renderApp->rightKey      = keyDefs[SendDlgItemMessage(IDC_RIGHT,     CB_GETCURSEL)].charCode;
	renderApp->upKey         = keyDefs[SendDlgItemMessage(IDC_UP,        CB_GETCURSEL)].charCode;
	renderApp->downKey       = keyDefs[SendDlgItemMessage(IDC_DOWN,      CB_GETCURSEL)].charCode;
	renderApp->forwardsKey   = keyDefs[SendDlgItemMessage(IDC_FORWARDS,  CB_GETCURSEL)].charCode;
	renderApp->backwardsKey  = keyDefs[SendDlgItemMessage(IDC_BACKWARDS, CB_GETCURSEL)].charCode;
	renderApp->resetKey      = keyDefs[SendDlgItemMessage(IDC_RESET,     CB_GETCURSEL)].charCode;
	renderApp->screenshotKey = keyDefs[SendDlgItemMessage(IDC_SCREENSHOT,CB_GETCURSEL)].charCode;
	renderApp->showFpsKey    = keyDefs[SendDlgItemMessage(IDC_SHOWFPS,   CB_GETCURSEL)].charCode;
	renderApp->consoleKey    = keyDefs[SendDlgItemMessage(IDC_CONSOLE,   CB_GETCURSEL)].charCode;
	renderApp->demoModeKey   = keyDefs[SendDlgItemMessage(IDC_DEMOMODE,  CB_GETCURSEL)].charCode;

	renderApp->getRenderWindow()->setMouseSpeed(SendDlgItemMessage(IDC_MOUSESPEED, TBM_GETPOS));

	EndDialog(0);
}


UINT controlsDialogThread(void *param){
	ControlsDialog dialog((RenderApp *) param);
	dialog.DoModal();

	return 0;
}

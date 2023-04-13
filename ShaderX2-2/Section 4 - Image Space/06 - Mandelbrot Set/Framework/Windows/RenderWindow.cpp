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

#include "RenderWindow.h"
#include "ControlsDialog.h"
#include "../Util/Tokenizer.h"

RenderWindow::RenderWindow(RenderApp *app, CREATE_ACTION createAction){
	renderApp = app;

	fullScreen = (createAction == CREATE_FULLSCREEN);

	width  = 1024;
	height = 768;
	bpp    = 32;
	refreshRate = 85;

	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	rect.left   = max(0, (w - 800) / 2);
	rect.right  = min(w, (w + 800) / 2);
	rect.top    = max(0, (h - 600) / 2);
	rect.bottom = min(h, (h + 600) / 2);


	CRegKey regKey;
	regKey.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
	if (createAction == CREATE_DEFAULT){
		DWORD fs = 0;
		regKey.QueryValue(fs, "FullScreen");
		fullScreen = (fs != 0);
	}
	DWORD vs = 0;
	regKey.QueryValue(vs, "VSync");
	vSync = (vs != 0);

	DWORD aniso = 0;
	regKey.QueryValue(aniso, "Anisotropic");
	anisotropic = (aniso != 0);

	fsaaLevel = 0;
	regKey.QueryValue(fsaaLevel, "FSAALevel");

	DWORD im = 0;
	regKey.QueryValue(im, "InvertMouse");
	invertMouse = (im != 0);

	DWORD cm = 0;
	regKey.QueryValue(cm, "ColorMipMaps");
	colorMipMaps = (cm != 0);

	DWORD vdi = 0;
	regKey.QueryValue(vdi, "ShowDebugInfo");
	showDebugInfo = (vdi != 0);
	
	regKey.QueryValue((DWORD &) width,       "Width");
	regKey.QueryValue((DWORD &) height,      "Height");
	regKey.QueryValue((DWORD &) refreshRate, "RefreshRate");
	if (!fullScreen){
		regKey.QueryValue((DWORD &) rect.left,   "WindowedLeft");
		regKey.QueryValue((DWORD &) rect.right,  "WindowedRight");
		regKey.QueryValue((DWORD &) rect.top,    "WindowedTop");
		regKey.QueryValue((DWORD &) rect.bottom, "WindowedBottom");
	}
	mouseSpeed = 5;
	regKey.QueryValue(mouseSpeed, "MouseSpeed");
	
	regKey.Close();

	modes = new DisplayModeHandler();
	modes->filterModes(640,480,32);
	modes->filterRefreshes(85);


	if (fullScreen){
		captureMouse = true;
		if (modes->setDisplayMode(width, height, bpp, refreshRate)){
			rect = CRect(0, 0, width, height);
		} else {
			MessageBox("Could not set display mode", "Error", MB_OK | MB_ICONERROR);
			fullScreen = false;
		}
	} else {
		captureMouse = false;
	}

	middleX = (rect.left + rect.right)  / 2;
	middleY = (rect.top  + rect.bottom) / 2;

	if (captureMouse) SetCursorPos(middleX, middleY);
	
	// Reset some control variables
	leftButton  = false;
	rightButton = false;

	Create(NULL, "Render", (fullScreen? WS_POPUP : WS_OVERLAPPEDWINDOW) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect);
}

BOOL RenderWindow::PreCreateWindow(CREATESTRUCT &cs){
	CFrameWnd::PreCreateWindow(cs);

	if (fullScreen) cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}


int RenderWindow::OnCreate(LPCREATESTRUCT lpCreateStruct){
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

	if (!fullScreen){
		CMenu mainMenu, popupMenu;
		mainMenu.CreateMenu();
		
		popupMenu.CreatePopupMenu();
		popupMenu.AppendMenu(MF_STRING, ID_FILE_RESTART, "&Restart graphics");
		popupMenu.AppendMenu(MF_SEPARATOR);
		popupMenu.AppendMenu(MF_STRING, ID_FILE_CONSOLE, "Toggle &console");
		popupMenu.AppendMenu(MF_SEPARATOR);
		popupMenu.AppendMenu(MF_STRING, ID_FILE_EXIT, "E&xit");
		mainMenu.AppendMenu(MF_POPUP, (UINT) popupMenu.Detach(), "&File");
		
		int i, len = modes->getNumberOfDisplayModes();

		CString str;
		DWORD flags;
		popupMenu.CreatePopupMenu();
		popupMenu.AppendMenu(MF_STRING, ID_FULLSCREEN, "Fullscreen mode\tAlt-Enter");
		popupMenu.AppendMenu(MF_SEPARATOR);
		for (i = 0; i < len; i++){
			str.Format("%d x %d", (*modes)[i].width, (*modes)[i].height);
			flags = MF_STRING;
			if ((*modes)[i].width == width && (*modes)[i].height == height){
				flags |= MF_CHECKED;
				prevCheckedMode = ID_FULLSCREEN + i + 1;
			}
			popupMenu.AppendMenu(flags, ID_FULLSCREEN + i + 1, str);
		}
		mainMenu.AppendMenu(MF_POPUP, (UINT) popupMenu.Detach(), "Fullscreen");
		
		popupMenu.CreatePopupMenu();
		popupMenu.AppendMenu(MF_STRING, ID_PREFERENCES_CONFIGURECONTROLS, "Configure controls ...");
		popupMenu.AppendMenu(MF_STRING | (invertMouse? MF_CHECKED : MF_UNCHECKED), ID_PREFERENCES_INVERTMOUSE, "Invert mouse");
		popupMenu.AppendMenu(MF_SEPARATOR);
		popupMenu.AppendMenu(MF_STRING | (vSync? MF_CHECKED : MF_UNCHECKED), ID_PREFERENCES_VSYNC, "VSync");
		popupMenu.AppendMenu(MF_STRING | (anisotropic? MF_CHECKED : MF_UNCHECKED), ID_PREFERENCES_ANISOTROPIC, "Anisotropic");

		CMenu fsaaMenu;
		fsaaMenu.CreatePopupMenu();
		fsaaMenu.AppendMenu(MF_STRING | (fsaaLevel == 0? MF_CHECKED : MF_UNCHECKED), ID_PREFERENCES_FSAA_OFF, "Disabled");
		fsaaMenu.AppendMenu(MF_SEPARATOR);
		fsaaMenu.AppendMenu(MF_STRING | (fsaaLevel == 2? MF_CHECKED : MF_UNCHECKED), ID_PREFERENCES_FSAA_2X, "2x");
		fsaaMenu.AppendMenu(MF_STRING | (fsaaLevel == 4? MF_CHECKED : MF_UNCHECKED), ID_PREFERENCES_FSAA_4X, "4x");
		fsaaMenu.AppendMenu(MF_STRING | (fsaaLevel == 6? MF_CHECKED : MF_UNCHECKED), ID_PREFERENCES_FSAA_6X, "6x");
		popupMenu.AppendMenu(MF_POPUP, (UINT) fsaaMenu.Detach(), "Multisample FSAA");


		popupMenu.AppendMenu(MF_SEPARATOR);
		popupMenu.AppendMenu(MF_STRING | (showDebugInfo? MF_CHECKED : MF_UNCHECKED), ID_PREFERENCES_SHOWDEBUGINFO, "Show debug info");
		mainMenu.AppendMenu(MF_POPUP, (UINT) popupMenu.Detach(), "&Preferences");

		popupMenu.CreatePopupMenu();
		popupMenu.AppendMenu(MF_STRING, ID_HELP_ABOUT, "&About");
		mainMenu.AppendMenu(MF_POPUP, (UINT) popupMenu.Detach(), "&Help");

		SetMenu(&mainMenu);
		mainMenu.Detach();



		CRect rect;
		GetClientRect(&rect);
		rect.top = rect.bottom - 24;
		commandLine.Create(WS_CHILD | WS_BORDER, rect, this, IDC_COMMANDLINE);
		commandLine.SetEventMask(ENM_KEYEVENTS);

		rect.bottom = rect.top;
		rect.top = rect.bottom - 202;
		output.Create(WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL, rect, this, IDC_OUTPUT);
		output.SetEventMask(ENM_KEYEVENTS);

		CHARFORMAT cf;
	
		memset(&cf, 0, sizeof(cf));
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_BOLD | CFM_FACE | CFM_ITALIC;
		strcpy(cf.szFaceName, "Courier");

		commandLine.SetDefaultCharFormat(cf);
		output.SetDefaultCharFormat(cf);
	}

	renderApp->setCreateAction(TERMINATE);

	ShowWindow(SW_SHOW);

	if (!(loaded = renderApp->initAPI(m_hWnd, fsaaLevel, fullScreen, refreshRate))){
		PostMessage(WM_CLOSE);
		return 0;
	}

	SetForegroundWindow();
	SetFocus();

	Renderer *renderer = renderApp->getRenderer();
	renderer->changeVSync(vSync);
	renderer->setAnisotropic(anisotropic);
	renderer->setMipMapColoring(colorMipMaps);

	renderApp->setShowDebugInfo(showDebugInfo);

	if (!(loaded = renderApp->load())){
		PostMessage(WM_CLOSE);
		return 0;
	}

	if (captureMouse) ShowCursor(FALSE);

	return 0;
}

void RenderWindow::OnSize(UINT nType, int cx, int cy){
	renderApp->setViewport(cx, cy);
}

void RenderWindow::OnWindowPosChanged(WINDOWPOS *lpwndpos){
	middleX = lpwndpos->x + lpwndpos->cx / 2;
	middleY = lpwndpos->y + lpwndpos->cy / 2;
	if ((lpwndpos->flags & SWP_NOSIZE) == 0){
		CRect rect;
		GetClientRect(&rect);
		renderApp->setViewport(rect.right - rect.left, rect.bottom - rect.top);

		if (!fullScreen){
			rect.top = rect.bottom - 24;
			commandLine.MoveWindow(rect);

			rect.bottom = rect.top;
			rect.top = rect.bottom - 202;
			output.MoveWindow(rect);
		}
	}
}


void RenderWindow::OnClose(){
	if (loaded) renderApp->unload();
	renderApp->closeAPI(m_hWnd);

	CRegKey regKey;
	regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
	if (fullScreen){
		modes->resetDisplayMode();
	} else {
		GetWindowRect(&rect);
		regKey.SetValue(rect.left,   "WindowedLeft");
		regKey.SetValue(rect.right,  "WindowedRight");
		regKey.SetValue(rect.top,    "WindowedTop");
		regKey.SetValue(rect.bottom, "WindowedBottom");
	}
	regKey.SetValue(fullScreen, "FullScreen");
	regKey.SetValue(mouseSpeed, "MouseSpeed");
	regKey.Close();

	delete modes;

	if (captureMouse) ShowCursor(TRUE);
	DestroyWindow();
}


void RenderWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	renderApp->setKey(nChar, true);

	if (nChar == VK_ESCAPE){
		if (!fullScreen && captureMouse){
			ShowCursor(TRUE);
			captureMouse = false;
		} else {
			PostMessage(WM_CLOSE, 0,0);
		}
	} else if (nChar == renderApp->consoleKey){
		OnFileConsole();
	}
}

void RenderWindow::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	renderApp->setKey(nChar, false);
}

void RenderWindow::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	if (((nFlags >> 13) & 0x3) == 1 && nChar == VK_ENTER){
		if (fullScreen){
			modes->resetDisplayMode();
			renderApp->setCreateAction(CREATE_WINDOWED);
		} else {
			renderApp->setCreateAction(CREATE_FULLSCREEN);
		}
		PostMessage(WM_CLOSE);
	}
}

void RenderWindow::OnLButtonDown(UINT nFlags, CPoint point){
	if (!captureMouse){
		SetCursorPos(middleX, middleY);
		captureMouse = true;
		ShowCursor(FALSE);
	}

	leftButton = true;
}

void RenderWindow::OnLButtonUp(UINT nFlags, CPoint point){
	leftButton = false;
}

void RenderWindow::OnRButtonDown(UINT nFlags, CPoint point){
	rightButton = true;
}

void RenderWindow::OnRButtonUp(UINT nFlags, CPoint point){
	rightButton = false;
}

void RenderWindow::OnMouseMove(UINT nFlags, CPoint point){
	static bool changeMouse = false;

	if (captureMouse){
		ClientToScreen(&point);
		float mouseSensibility = 0.0005f * mouseSpeed;

		renderApp->rotateView(mouseSensibility * (middleY - point.y) * (invertMouse? 1 : -1),
							  mouseSensibility * (middleX - point.x));
		
		if (changeMouse = !changeMouse) SetCursorPos(middleX, middleY);
	}
}

void RenderWindow::OnKillFocus(CWnd *pNewWnd){
	/*if (fullScreen){
		if (captureMouse) ShowCursor(TRUE);

		modes.resetDisplayMode();
		middleX = (rect.left + rect.right)  / 2;
		middleY = (rect.top  + rect.bottom) / 2;
		MoveWindow(rect, TRUE);
		fullScreen = false;
		captureMouse = false;
	}*/
}

void RenderWindow::OnCommandLineNotify(NMHDR *pNMHDR, LRESULT *pResult){
	MSGFILTER *msg = (MSGFILTER *) pNMHDR;

	if (msg->msg == WM_KEYDOWN){
		if (msg->wParam == VK_ESCAPE) OnFileConsole();

		if (pNMHDR->idFrom == IDC_COMMANDLINE && msg->wParam == VK_ENTER){
			CString str, cmd;
			String response;

			commandLine.GetWindowText(cmd);
			if (cmd.GetLength() > 0){
				commandLine.SetWindowText("");
				output.GetWindowText(str);

				char command[256];
				Tokenizer tok;
				tok.setString(cmd);
				tok.next(command);

				if (stricmp(command, "clear") == 0){
					str = "";
				} else if (stricmp(command, "help") == 0){
					response =
						"clear           - clear the output screen\r\n"
						"help            - display this text\r\n"
						"list/ls         - list all variables\r\n"
						"set a b         - set the variable a to the integer value b\r\n"
						"set a \"b\"       - set the variable a to the string value b\r\n"
						"pos             - shows position\r\n"
						"setpos a,b,c    - set position to (a,b,c)\r\n"
						"angles          - shows viewing angles\r\n"
						"setangles a,b,c - set viewing angles to (a,b,c)\r\n"
						"rm/del a        - delete variable a\r\n"
						"exit            - close the console\r\n"
						"quit            - close the application\r\n"
						"cpu             - show cpu info\r\n"
						"vendor          - show graphic card vendor\r\n"
						"renderer        - show renderer\r\n"
						"version         - show driver version\r\n"
						"\r\n"
						"OpenGL only:\r\n"
						"extensions      - list supported OpenGL extensions\r\n";
				} else if (stricmp(command, "set") == 0){
					if (tok.next(command)){
						CString variable = command;
						if (tok.next(NULL) && tok.getAllRight(command)){
							CRegKey regKey;
							regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
							if (command[0] == '"'){
								char *ch = strchr(command + 1, '"');
								if (ch != NULL) *ch = '\0';
								regKey.SetValue(command + 1, variable);
							} else {
								regKey.SetValue(atoi(command), variable);
							}
							regKey.Close();
						}
					}
				} else if (stricmp(command, "rm") == 0 || stricmp(command, "del") == 0){
					if (tok.next(command)){
						CRegKey regKey;
						regKey.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
						regKey.DeleteValue(command);
						regKey.Close();
					}
				} else if (stricmp(command, "exit") == 0){
					OnFileConsole();
				} else if (stricmp(command, "quit") == 0){
					PostMessage(WM_CLOSE);
				} else if (stricmp(command, "cpu") == 0){
					CString mhz;
					mhz.Format(", %dMHz", (int) (getHz() + 500000) / 1000000);
					response = cpuVendor;
					if (cpuBrandName[0] != '\0'){
						response += ", ";
						response += cpuBrandName;
					}
					response += mhz;
				} else if (stricmp(command, "list") == 0 || stricmp(command, "ls") == 0){
					CRegKey regKey;
					if (regKey.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus") == ERROR_SUCCESS){
						DWORD maxNameSize, maxDataSize, nValues;
						DWORD nameSize, dataSize, type, i;
						
						RegQueryInfoKey(regKey, NULL, NULL, NULL, NULL, NULL, NULL, &nValues, &maxNameSize, &maxDataSize, NULL, NULL);
						maxNameSize += sizeof(TCHAR);
						maxDataSize += sizeof(unsigned char);

						TCHAR *name = new TCHAR[maxNameSize];
						unsigned char *data = new unsigned char[maxDataSize];

						CString *strings = new CString[nValues];
						CString *values  = new CString[nValues];

						for (i = 0; i < nValues; i++){
							nameSize = maxNameSize;
							dataSize = maxDataSize;

							RegEnumValue(regKey, i, name, &nameSize, NULL, &type, data, &dataSize);

							strings[i] = name;
							switch(type){
							case REG_DWORD:
								values[i].Format("%d", *(int *) data);
								break;
							case REG_SZ:
								values[i] = "\"";
								values[i] += (char *) data;
								values[i] += "\"";
								break;
							}
						}
						
						int *indices = new int[nValues];
						int m = 0;
						for (i = 0; i < nValues; i++){
							m = max(m, strings[i].GetLength());
							indices[i] = i;
						}
						memset(name, ' ', ++m);

						bool changed;
						do {
							changed = false;
							for (i = 1; i < nValues; i++){
								if (strings[indices[i - 1]].CompareNoCase(strings[indices[i]]) > 0){
									int temp = indices[i - 1];
									indices[i - 1] = indices[i];
									indices[i] = temp;

									changed = true;
								}
							}
						} while (changed);


						for (i = 0; i < nValues; i++){
							response += strings[indices[i]];
							int index = m - strings[indices[i]].GetLength();
							name[index] = '\0';
							response += name;
							name[index] = ' ';
							response += values[indices[i]];
							response += "\r\n";
						}
						
						delete indices;
						delete [] values;
						delete [] strings;
						delete data;
						delete name;
						
						regKey.Close();
					}

				} else if (renderApp->processCommand(cmd, response)){
				} else {
					response = "Invalid command. Type help for a list of valid commands.";
				}

				if (response.getLength() == 0) response = "Command OK";

				response.trimRight(" \r\n\t");
				str += "\r\n> " + cmd;
				str += "\r\n" + response;
				output.SetWindowText(str);

				output.LineScroll(output.GetLineCount() - output.GetFirstVisibleLine() - 15);
			}
		}
	} 
   
	*pResult = 0;
}


void RenderWindow::OnFileExit(){
	PostMessage(WM_CLOSE);
}

void RenderWindow::OnFileRestart(){
	renderApp->setCreateAction(fullScreen? CREATE_FULLSCREEN : CREATE_WINDOWED);
	PostMessage(WM_CLOSE);
}

void RenderWindow::OnFileConsole(){
	if (!fullScreen){
		static int visible = false;

		if (visible){
			output.ShowWindow(SW_HIDE);
			commandLine.ShowWindow(SW_HIDE);
		} else {
			output.ShowWindow(SW_SHOW);
			commandLine.ShowWindow(SW_SHOW);
			commandLine.SetFocus();
		}

		visible = !visible;
	}
}

void RenderWindow::OnFullscreen(){
	renderApp->setCreateAction(CREATE_FULLSCREEN);
	PostMessage(WM_CLOSE);
}

void RenderWindow::OnFullscreenMode(UINT nID){
	CRegKey regKey;
	DWORD refresh;

	regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
	regKey.SetValue((*modes)[nID - ID_FULLSCREEN - 1].width,  "Width");
	regKey.SetValue((*modes)[nID - ID_FULLSCREEN - 1].height, "Height");

	CString str;
	str.Format("RefreshRate_%dx%d", (*modes)[nID - ID_FULLSCREEN - 1].width, (*modes)[nID - ID_FULLSCREEN - 1].height);
	if (regKey.QueryValue(refresh, str) == ERROR_SUCCESS){
		regKey.SetValue(refresh, "RefreshRate");
	} else {
		regKey.SetValue((*modes)[nID - ID_FULLSCREEN - 1].refreshRate, "RefreshRate");
	}
	regKey.Close();

	GetMenu()->CheckMenuItem(nID, MF_CHECKED);
	GetMenu()->CheckMenuItem(prevCheckedMode, MF_UNCHECKED);
	prevCheckedMode = nID;
}

void RenderWindow::OnPreferencesConfigureControls(){
	// Create a thread for the dialog so that it doesn't halt window redrawing
	AfxBeginThread(controlsDialogThread, renderApp);
}

void RenderWindow::OnPreferencesInvertMouse(){
	invertMouse = !invertMouse;

	CRegKey regKey;
	regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
	regKey.SetValue(invertMouse, "InvertMouse");
	regKey.Close();

	GetMenu()->CheckMenuItem(ID_PREFERENCES_INVERTMOUSE, invertMouse? MF_CHECKED : MF_UNCHECKED);
}

void RenderWindow::OnPreferencesVSync(){
	vSync = !vSync;
	
	CRegKey regKey;
	regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
	regKey.SetValue(vSync, "VSync");
	regKey.Close();

	GetMenu()->CheckMenuItem(ID_PREFERENCES_VSYNC, vSync? MF_CHECKED : MF_UNCHECKED);
	renderApp->getRenderer()->changeVSync(vSync);
}

void RenderWindow::OnPreferencesAnisotropic(){
	anisotropic = !anisotropic;

	CRegKey regKey;
	regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
	regKey.SetValue(anisotropic, "Anisotropic");
	regKey.Close();

	GetMenu()->CheckMenuItem(ID_PREFERENCES_ANISOTROPIC, anisotropic? MF_CHECKED : MF_UNCHECKED);

	Renderer *renderer = renderApp->getRenderer();
	TextureID texID = TEXTURE_NONE;
	while ((texID = renderer->getNextTexture(texID)) != TEXTURE_NONE){
		renderer->changeAnisotropic(texID, anisotropic);
	}
}

void RenderWindow::OnPreferencesFsaa(UINT nID){
	fsaaLevel = 2 * (nID - ID_PREFERENCES_FSAA_OFF);

	CRegKey regKey;
	regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
	regKey.SetValue(fsaaLevel, "FSAALevel");
	regKey.Close();

	renderApp->setCreateAction(fullScreen? CREATE_FULLSCREEN : CREATE_WINDOWED);
	PostMessage(WM_CLOSE);
}

void RenderWindow::OnShowDebugInfo(){
	showDebugInfo = !showDebugInfo;

	CRegKey regKey;
	regKey.Create(HKEY_LOCAL_MACHINE, "SOFTWARE\\Humus");
	regKey.SetValue(showDebugInfo, "ShowDebugInfo");
	regKey.Close();

	GetMenu()->CheckMenuItem(ID_PREFERENCES_SHOWDEBUGINFO, showDebugInfo? MF_CHECKED : MF_UNCHECKED);

	renderApp->setShowDebugInfo(showDebugInfo);
}


UINT aboutThread(void *param){
	MessageBox((HWND) param, 
		"This application was written by Humus.\n\n"
		"emiper-8@student.luth.se\n"
		"ICQ 47010716\n"
		"http://esprit.campus.luth.se/~humus/", "About", MB_ICONINFORMATION);

	return 0;
}

void RenderWindow::OnHelpAbout(){
	// Create a thread for the messagebox so that it doesn't halt window redrawing
	AfxBeginThread(aboutThread, GetSafeHwnd());
}


/* -------------------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(RenderWindow, CFrameWnd)
	
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_CLOSE()

	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()

	ON_WM_KILLFOCUS()

	ON_NOTIFY(EN_MSGFILTER, IDC_COMMANDLINE, OnCommandLineNotify)
	ON_NOTIFY(EN_MSGFILTER, IDC_OUTPUT,      OnCommandLineNotify)

	ON_COMMAND      (ID_FILE_EXIT,    OnFileExit)
	ON_COMMAND      (ID_FILE_RESTART, OnFileRestart)
	ON_COMMAND      (ID_FILE_CONSOLE, OnFileConsole)
	ON_COMMAND      (ID_FULLSCREEN, OnFullscreen)
	ON_COMMAND_RANGE(ID_FULLSCREEN + 1, ID_FULLSCREEN + 99, OnFullscreenMode)
	ON_COMMAND      (ID_PREFERENCES_CONFIGURECONTROLS, OnPreferencesConfigureControls)
	ON_COMMAND      (ID_PREFERENCES_INVERTMOUSE,       OnPreferencesInvertMouse)
	ON_COMMAND      (ID_PREFERENCES_VSYNC,             OnPreferencesVSync)
	ON_COMMAND      (ID_PREFERENCES_ANISOTROPIC,       OnPreferencesAnisotropic)
	ON_COMMAND_RANGE(ID_PREFERENCES_FSAA_OFF, ID_PREFERENCES_FSAA_6X, OnPreferencesFsaa)
	ON_COMMAND      (ID_PREFERENCES_SHOWDEBUGINFO,     OnShowDebugInfo)
	ON_COMMAND      (ID_HELP_ABOUT, OnHelpAbout)

END_MESSAGE_MAP()


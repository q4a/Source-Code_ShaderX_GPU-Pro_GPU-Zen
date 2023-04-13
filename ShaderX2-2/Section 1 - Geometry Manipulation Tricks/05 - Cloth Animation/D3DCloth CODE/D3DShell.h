/*****************************************************************************
  Name : D3DShell.h v9.0.5
  Date : March 2003
  Platform : ANSI compatible

  Header file to be used with D3DShell.cpp

  Copyright : 2003 by PowerVR Technologies Ltd. All rights reserved.
******************************************************************************/

#ifndef _D3DSHELL_H_
#define _D3DSHELL_H_


/* The 3D menus of D3DShell requires linking with the D3DTools library (D3DTools.h and D3DTools.lib).
   You may comment out the following line to disable the compilation of 3D menus-related functions. In that 
   case D3DTools.h and D3DTools.lib are not required to the build process. */
//#define D3DSHELL_USE_3D_MENUS

#ifdef D3DSHELL_USE_3D_MENUS
#include "D3DX9.h"
#include "D3DTools.h"		/* Header file for D3DTools (contains Print3D functions used in 3D menus) */
#else
typedef IDirect3DDevice9	IDirect3DDevice9Wrapped, *PDIRECT3DDEVICE9WRAPPED, *LPDIRECT3DDEVICE9WRAPPED;
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Enum */
typedef enum _D3DShellPrefs
{
	DEFAULT_USE_REF_DRIVER  =0x00000001,	// Use Ref. Driver by default (if available).
	DEFAULT_ZBUFFER_OFF		=0x00000002,	// Application will start with no Z-Buffer as default
	DEFAULT_TRIPLE_BUFFER	=0x00000004,	// Start with triple buffer by default
	DEFAULT_CLEAR_ON		=0x00000008,	// Viewport clear is On by default
	DEFAULT_FULLSCREEN		=0x00000010,	// Application will start FullScreen by default
	DEFAULT_NO_VSYNC		=0x00000020,	// Vertical retrace synchronisation will be off by default. This flag will have no effect if the DirectDraw driver does not support No VSync.
	DEFAULT_USE_HAL			=0x00000040,	// The HAL 3D device will be selected by default, even if a T&L HAL is present
	FORCE_ZBUFFER_ON		=0x00000080,	// Z-Buffer will be forced On and will not appear in the menu
	FORCE_ZBUFFER_OFF		=0x00000100,	// Z-Buffer will be forced Off and will not appear in the menu
	FORCE_DOUBLE_BUFFER		=0x00000200,	// Double Buffer will be forced On and will not appear in the menu
	FORCE_TRIPLE_BUFFER		=0x00000400,	// Triple Buffer will be forced On and will not appear in the menu
	FORCE_CLEAR_ON			=0x00000800,	// Viewport clear will be forced On and will not appear in the menu
	FORCE_CLEAR_OFF			=0x00001000,	// Viewport clear will be forced Off and will not appear in the menu
	FORCE_FULLSCREEN		=0x00002000,	// FullScreen mode will be used all the time (Unable to get to Window mode)
	FORCE_MULTISAMPLING_OFF =0x00004000,	// Multisampling will be disabled and will not appear in the menu
	FORCE_VSYNC_ON			=0x00008000,	// VSync will be turned ON and will not appear in the menu
	FORCE_VSYNC_OFF			=0x00010000,	// VSync will be turned OFF and will not appear in the menu
	DISABLE_TRIPLE_BUFFER	=0x00020000,	// Triple Buffer choice will not appear in the menu
	DISABLE_VERY_HIGH_RES	=0x00040000,	// Disable resolution above 1024x768
	DISABLE_RESIZING		=0x00080000,	// Prevent the application to be resized by mouse drag
	DISABLE_Z_CLEAR			=0x00100000,	// Z Clear will not be performed.
	DISABLE_PURE_DEVICE		=0x00200000,	// PURE DEVICE will not appear in the menu, even if it's available
	USE_INSTANTANEOUS_FPS	=0x00400000,	// Frame rate will be computed on an instantaneous basis (each frame) rather then average (default)
	USE_STENCIL_BUFFER		=0x00800000,	// D3DShell will use a depth buffer containing a stencil if available
	USE_3D_MENUS			=0x01000000,	// 3D scaling menus will be used
	USE_LOCKABLE_BACK_BUFFER=0x02000000,	// Render surface will be created with the D3DPRESENTFLAG_LOCKABLE_BACKBUFFER flag
	START_WINDOW_320X240	=0x04000000,	// Starting window resolution will be 320x240 (Cannot be used with FULLSCREEN)
	DISPLAY_PRINT3D_ICONS	=0x08000000,	// The menu icons for page management will be displayed. 
	ENABLE_CLEAN_EXIT		=0x10000000,	// No error message will be displayed after either Initview() or Renderscene() return FALSE
	NO_REFRESH_RATES		=0x20000000,	// No refresh rates will be presented (this is always true if the Windows version is Win98SE or below)
} D3DShellPrefs;


/* Enum */
typedef enum _D3DShellVariables
{
	APPLICATION_INSTANCE_HANDLE	=1,	// Handle of application (HINSTANCE type)
	PREVIOUS_INSTANCE_HANDLE,		// Handle of previous application (HINSTANCE type)
	WINDOW_HANDLE,					// Application window handle (HWND type)
	FULLSCREEN_MODE,				// Is application in FullScreen mode (BOOL type)
	Z_BUFFER,						// Z Buffer surface (LPDIRECT3DSURFACE9 type)
	D3DDEVICETYPE,					// 3D device currently selected (D3DDEVTYPE type)
	FRAME_BUFFER_BIT_DEPTH,			// Rendering surface bit depth (DWORD type)
	NUM_BACK_BUFFER,				// Number of back buffer(s). 1 means double buffer and 2 means triple buffer (DWORD type)
	FRAME_RATE,						// Current frame rate (float type)
	SCREEN_SAVER_SETTING			// Current screen saver setting (INT type)
} D3DShellVariables;


/*******************************************
** D3DShell helper functions declarations **
*******************************************/

/* This function is used to pass preferences to the D3DShell
   If used, it must be called from InitApplication() only. */
void D3DShellSetPreferences(const char *pszApplicationName, 
							const HMENU	hUserMenuID, 
							const HACCEL hUserAccel, 
							const HICON	hUserIcon, 
							const DWORD	dwFlags);

/* This function enables the user to retrieve rendering variables if needed */
void D3DShellGetVariable(D3DShellVariables nVariableName, 
						 void *pVariablePointer);

/* This function enables to user to reinitialise D3D with the submitted rendering variables. */
/*********************************** USE WITH CAUTION. ***************************************/
BOOL D3DShellReinitialise(int nAdapter, D3DDEVTYPE DeviceType,
						  BOOL bFullScreen, int nBufferMode,
						  D3DMULTISAMPLE_TYPE MultiSamplingType,
						  DWORD dwRenderWidth, DWORD dwRenderHeight, D3DFORMAT FrameBufferFormat,
						  BOOL bUseHWTnL,
						  BOOL bZBufferEnabled, D3DFORMAT ZBufferFormat);


/*******************************
** User interaction functions **
*******************************/

/* These functions have to exist in the scene file (e.g. scene.c).
   They are used to interact with the D3DShell */

void InitApplication(HINSTANCE hInstance, HWND hWindow, char *pszCommandLine);
/*
This function will be called by D3DShell ONCE at the beginning of the D3DShell 
WinMain() function. This function enables the user to perform any initialisation 
before any 3D is rendered.
From this function the user can call D3DShellSetPreferences() to set default
values or submit a menu to D3DShell. See above for the prototype of this function.
*/


void QuitApplication();
/*
This function will be called by D3DShell just before finishing the program.
It enables the user to release any memory allocated in InitApplication().
*/


void UserWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
/*
This function is the user Window Procedure function. It enables the user to retrieve
menu choices, keystrokes or other messages (WM_TIMER for instance).
If you don't want to use this function, put nothing in it :
    
void UserWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Nothing !
}

D3DShell processes many window messages. If the user needs to 
process these messages in their application, they should make sure
NOT to return DefWindowProc() as it will prevent the D3DShell WindowProc
to do its own processing for these messages.
The window messages processed by D3DShell are :

WM_ENTERMENULOOP, WM_EXITMENULOOP,
WM_ACTIVATEAPP,
WM_SYSCOMMAND,
WM_SIZE, WM_MOVE, WM_SIZING, WM_MOVING,
WM_PAINT,
WM_DESTROY, WM_QUIT

Note : do NOT process the VK_ESCAPE key, as it is already used by  
  	   D3DShell to quit the application.
*/


BOOL RenderScene(LPDIRECT3DDEVICE9WRAPPED lpDev9);
/*
This is the main user function in which you have to do your own rendering.
A pointer to the D3D device (LPDIRECT3DDEVICE9) is passed to this function 
so you can use it to control your rendering (Renderstates, lights, etc...)
This function is called every frame by D3DShell and enable the user to
create the scene for this frame.
*/


BOOL InitView(LPDIRECT3D9 lpD3D9, LPDIRECT3DDEVICE9WRAPPED lpDev9, 
			  DWORD dwWidth, DWORD dwHeight);
/*
This function enables the user to create vertex buffers, materials, set 
rendering values, load textures, etc... This function will be called each 
time the surfaces and objects will be recreated (i.e. when a rendering
variable is changed).
dwWidth and dwHeight are the current dimensions of the rendering surface.
*/

void ReleaseView();
/*
This function enables the user to release any devices they created 
in the InitView function.
The function will be called each time a rendering variable is changed.
*/


#ifdef D3DSHELL_USE_3D_MENUS

BOOL D3DShellDisplay3DMenus();
/*
If 3D menus are to be used, a single call to this function must be made by 
the user within the BeginScene to EndScene pair. It returns true if successful, 
and false otherwise. 
*/

BOOL D3DShellTick3DMenus(UINT identifier, BOOL tick);
/*
When updating menus this function allows you to tick a menu item by passing its
identifier and a boolean to determine whether to tick or untick this item.
*/

BOOL D3DShellGhost3DMenus(UINT identifier, BOOL tick);
/*
When updating menus this function allows you to ghost a menu item by passing its
identifier and a boolean to determine whether to ghost or unghost this item.
*/

#endif

#ifdef __cplusplus
}
#endif

#endif

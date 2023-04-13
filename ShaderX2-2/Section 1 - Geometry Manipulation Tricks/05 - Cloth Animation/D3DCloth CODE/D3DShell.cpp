/*****************************************************************************
  Name : D3DShell.cpp v9.0.5
  Date : March 2003
  Platform : ANSI compatible

  Build Info : D3DShell.cpp + D3DShell.h + D3DTools.h + D3DTools.lib + D3D9.LIB + D3DX9.LIB + Scene file

  D3DShell is a program used to make D3D programming easier and less 
  time-consuming. D3DShell takes care of all Direct3D initialisation for the 
  user : handling adapters, 3D devices, Fullscreen mode, resolution mode, window 
  modes, buffering modes, Z-Buffer use, viewport creation & clearing, etc...

  D3DShell consists of only 2 files (D3DShell.c and D3DShell.h). However it relies on the
  D3DTools library to handle 3D menus. Thus, when creating a D3D application one has to link your files 
  with D3DShell.cpp and D3DShell.h, D3DTools.h and D3DTools.lib, plus the Direct3D library (D3D9.LIB).
  Note that a small modification may be performed to D3DShell.h in order to avoid linking with 
  D3DTools (and as such giving up the 3D menus). See D3DShell.h for more details.

  The user is only responsible for creating all D3D variables that will be
  needed at rendering time (vertex buffers, lights, materials, etc..).
  
  The user can choose preferences thanks to the D3DShellSetPreferences() function.
  A list of flags that can be passed to this function can be found in the
  D3DShell.h file.
  
  For a list of functions that the user has to use to interact with D3DShell, 
  again see the D3DShell.h file.

  
  * Screen capture

  During execution, a screen capture can be performed by pressing F12.
  The current contents of the back buffer will be saved as a .BMP file
  named ScreenXX.bmp. Up to 10000 different images can be saved.
  
  
  *	Helper Functions :

  void D3DShellSetPreferences(const char	*pszApplicationName, 
							  const HMENU	hUserMenuID, 
							  const HACCEL	hUserAccel, 
							  const HICON	hUserIcon, 
							  const DWORD	dwFlags);
  -> This function is used to pass preferences to D3DShell. If used, it must be 
     called from InitApplication() only. 

  void D3DShellGetVariable(D3DShellVariables nVariableName, void *pVariablePointer);
  -> This function should only be used if the user needs to retrieve some rendering 
     variables that are not already provided in InitView(). Using flags defined 
	 in D3DShell.h, one can ask a pointer to a certain type of variable. The list of 
	 these variables can be found in D3DShell.h.

  The user doesn't have to use these helper functions, they are provided to make 
  common rendering tasks easier.

  The scene file can either be a C++ or C file. If for any reason you need to compile
  D3DShell in C this should be easily supported by simply renaming it to D3DShell.c
  
    
  Please report any bugs or comments to me. If, for any reason, you need to
  modify D3DShell, please report to me so I can do the modifications myself.

  
  * LATEST CHANGES :
    28/03/03:  - Spring cleaning. Updated D3DShell.txt file.
			   Versioning to 9.0.5.
    11/03/03:  - Added command line option: -CAPTUREEVERYFRAME.
			   Versioning to 9.0.4.
    24/09/02:  - Added support for SW, HW and MIXED vertex processing.
			   Added some check cases when multisampling is used.
			   Versioning to 9.0.3.
    16/09/02:  - Display modes enumeration now use dynamic buffers.
			   Versioning to 9.0.2.
    13/08/02:  - Converted to DX9 Beta 2. Fixed bug with viewport clears not occuring
			   if a depth buffer wasn't used. Now includes D3DX9.h if D3DTools is linked.
    07/05/02:  - Fixed bug with Multisampling tick boxes and DISABLE_PURE_DEVICE flag.
			   - cs: fixed a bug related to first clicking on a 3D menu.
			   Versioning to 8.1.5 for Web release.
    05/03/02:  - D3DFMT_A8R8G8B8 display format will now be selected by default if 
			   available.
			   - Increased maximum number of screenshots to 10000.
    06/02/02:  - Fixed a bug where keyboard input was not passed to the scene file if
			   3D menus are not used.
			   - D3DShell no longer modifies the original command line.
			   - Added flags: DISABLE_PURE_DEVICE and ENABLE_CLEAN_EXIT.
			   - Removed flags concerning secondary adapters.
    10/12/01:  -cs: 3D Menus fixed and improved.
			   -cs: New flag added to D3DShellSetPreferences : DISPLAY_PRINT3D_ICONS
    16/11/01:  - Screen captures are now performed from the front buffer.
			   - New flag: USE_LOCKABLE_BACK_BUFFER.
			   Versioning to 8.1.4
    05/11/01:  - Added support for MultiSampling.
			   Versioning to 8.1.3.
    10/10/01:  - D3DShell now requires linking with D3DTools however it can be
			   avoided by commenting out a line in D3DShell.h. Versioning to 8.1.1.
    13/09/01:  - D3DShell is now in C++ by default. D3DShell.cpp can safely be
			   renamed to D3DShell.c and recompiled in 'C' if needs be.
    30/08/01:  - 3D Menus added:
					- Add USE_3D_MENUS to D3DShellSetPreferences to enable them.
					- Add D3DShellDisplay3DMenus() inside your BeginScene/EndScene to display the menus
					- Currently F9 toggles the menu on and off.
					- Currently 3D menus only support one level of popups (sub menus).
    12/01/01:  - Changed screen capture function so that top bits are now
			   replicated into lower bits when doing a screen capture from 
			   a 16bpp surface.
			   Versioning to 8.0.1.
    18/07/00:  - First version from D3DShell for DirectX7.0.
	
	
  ------------------------------------------------------------
  | READ D3DSHELL.TXT FOR FURTHER INFORMATION ABOUT D3DSHELL |
  ------------------------------------------------------------

  Copyright : 2002 by PowerVR Technologies. All rights reserved.
******************************************************************************/
/*************************
**		  Includes	    **
*************************/
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>			/* For sprintf(...) */
#include <d3d9.h>			/* D3D header */

/* Version checker */
#if DIRECT3D_VERSION < 0x0900
#error DIRECTX 9.0 MINIMUM IS REQUIRED TO COMPILE D3DSHELL
#endif

#include "D3DShell.h"		/* Header file for D3DShell */


/* Direct3D errors 
   This header can be included (and linked with the 
   D3DError.c file) to report Direct3D errors */
//#include "D3DError.h"
#ifndef DDERROR_H
#define DisplayHRESULTInDebug(x)
#define ASSERT(x)
#endif


/************************* 
**		Menu defines    **
*************************/
/*  These values are "weird" numbers on purpose, as we don't want
    to have duplicate menu items IDs from the Application own menu */
#define MAINMENU                        19101
#define ID_FILE	                        19801
#define ID_FILE_EXIT					19802
#define ID_FILE_SCREENCAPTURE			19803
#define ID_FILE_DISPLAYINFO				19804
#define ID_FILE_HIDEMENU				19805
#define ID_DEVICES                      29801
#define ID_DEVICES_ADAPTER0				29810
#define ID_DEVICES_ADAPTER1             29811
#define ID_DEVICES_ADAPTER2             29812
#define ID_DEVICES_ADAPTER3             29813
#define ID_DEVICES_ADAPTER4             29814
#define ID_DEVICES_D3DDEVICE_HAL        29820
#define ID_DEVICES_D3DDEVICE_HALPURE    29821
#define ID_DEVICES_D3DDEVICE_SW			29822
#define ID_DEVICES_D3DDEVICE_REF		29823
#define ID_DEVICES_HWVERTEXPROCESSING	29830
#define ID_DEVICES_SWVERTEXPROCESSING	29831
#define ID_DEVICES_MIXEDVERTEXPROCESSING	29832
#define	ID_BUFFERING					39800
#define ID_BUFFERING_VSYNC				39805
#define ID_BUFFERING_MS_NONE			39810
#define ID_BUFFERING_MS_2_SAMPLES		39812
#define ID_BUFFERING_MS_3_SAMPLES		39813
#define ID_BUFFERING_MS_4_SAMPLES		39814
#define ID_BUFFERING_MS_5_SAMPLES		39815
#define ID_BUFFERING_MS_6_SAMPLES		39816
#define ID_BUFFERING_MS_7_SAMPLES		39817
#define ID_BUFFERING_MS_8_SAMPLES		39818
#define ID_BUFFERING_MS_9_SAMPLES		39819
#define ID_BUFFERING_MS_10_SAMPLES		39820
#define ID_BUFFERING_MS_11_SAMPLES		39821
#define ID_BUFFERING_MS_12_SAMPLES		39822
#define ID_BUFFERING_MS_13_SAMPLES		39823
#define ID_BUFFERING_MS_14_SAMPLES		39824
#define ID_BUFFERING_MS_15_SAMPLES		39825
#define ID_BUFFERING_MS_16_SAMPLES		39826
#define	ID_BUFFERING_ZBUFFER			39860
#define	ID_BUFFERING_ZBUFFER_D16L		39861
#define	ID_BUFFERING_ZBUFFER_D32		39862
#define	ID_BUFFERING_ZBUFFER_D15S1		39863
#define	ID_BUFFERING_ZBUFFER_D24S8		39864
#define	ID_BUFFERING_ZBUFFER_D16		39865
#define	ID_BUFFERING_ZBUFFER_D24X8		39866
#define	ID_BUFFERING_ZBUFFER_D24X4S4	39867
#define ID_BUFFERING_DOUBLEBUFFER		39870
#define ID_BUFFERING_TRIPLEBUFFER		39871
#define ID_BUFFERING_CLEAR				39872
#define ID_MODES						49801
#define ID_MODES_FULLSCREEN				49802
#define ID_MODES_FORMAT_X8R8G8B8		49810
#define ID_MODES_FORMAT_A8R8G8B8		49811
#define ID_MODES_FORMAT_A2R10G10B10		49812
#define ID_MODES_FORMAT_X1R5G5B5		49813
#define ID_MODES_FORMAT_A1R5G5B5		49814
#define ID_MODES_FORMAT_R5G6B5			49815
#define ID_MODES_FREQUENCY_60			49820
#define ID_MODES_FREQUENCY_70			49821
#define ID_MODES_FREQUENCY_72			49822
#define ID_MODES_FREQUENCY_75			49823
#define ID_MODES_FREQUENCY_85			49824
#define ID_MODES_FREQUENCY_90			49825
#define ID_MODES_FREQUENCY_100			49826
#define ID_MODES_FREQUENCY_120			49827
#define ID_MODES_FREQUENCY_150			49828
#define ID_MODES_RES0                   49850
#define ID_MODES_RES1                   49851
#define ID_MODES_RES2                   49852
#define ID_MODES_RES3                   49853
#define ID_MODES_RES4                   49854
#define ID_MODES_RES5                   49855
#define ID_MODES_RES6                   49856
#define ID_MODES_RES7                   49857
#define ID_MODES_RES8                   49858
#define ID_MODES_RES9                   49859
#define ID_MODES_RES10                  49860
#define ID_MODES_RES11                  49861
#define ID_MODES_RES12                  49862
#define ID_MODES_RES13                  49863
#define ID_MODES_RES14                  49864
#define ID_MODES_RES15                  49865
#define ID_MODES_RES16                  49866
#define ID_MODES_RES17                  49867
#define ID_MODES_RES18                  49868
#define ID_MODES_RES19                  49869
#define ID_MODES_RES20                  49870
#define ID_MODES_RES21                  49871
#define ID_MODES_RES22                  49872
#define ID_MODES_RES23                  49873
#define ID_MODES_RES24                  49874
#define ID_MODES_RES25                  49875
#define ID_MODES_RES26                  49876
#define ID_MODES_RES27                  49877
#define ID_MODES_RES28                  49878
#define ID_MODES_RES29                  49879
#define ID_MODES_RES30                  49880
#define ID_MODES_RES31                  49881
#define ID_MODES_RES32                  49882
#define ID_MODES_RES33                  49883
#define ID_MODES_RES34                  49884
#define ID_MODES_RES35                  49885
#define ID_MODES_RES36                  49886
#define ID_MODES_RES37                  49887
#define ID_MODES_RES38                  49888
#define ID_MODES_RES39                  49889
#define ID_MODES_RES40                  49890
#define ID_MODES_RES41                  49891
#define ID_MODES_RES42                  49892
#define ID_MODES_RES43                  49893
#define ID_MODES_RES44                  49894
#define ID_MODES_RES45                  49895
#define ID_MODES_RES46                  49896
#define ID_MODES_RES47                  49897
#define ID_MODES_RES48                  49898
#define ID_MODES_RES49                  49899
#define ID_MODES_RES50                  49900
#define ID_MODES_RES51                  49901
#define ID_MODES_RES52                  49902
#define ID_MODES_RES53                  49903
#define ID_MODES_RES54                  49904
#define ID_MODES_RES55                  49905
#define ID_MODES_RES56                  49906
#define ID_MODES_RES57                  49907
#define ID_MODES_RES58                  49908
#define ID_MODES_RES59                  49909


/*************************
**		  Defines	    **
*************************/
#define	FRAMESBEFOREKILLING		20	/* Application will auto terminate after 20 frames */

#define	MAXDADAPTERS			5	/* Maximum number of adapters we ever expect to find */
#define MAXDISPLAYMODES			60	/* Maximum number of display modes we ever expect to find */
#define MAXTOTALDISPLAYMODES	300	/* Maximum of total display modes that can be enumerated */
#define MAXD3DDRIVERS			10	/* Maximum number of D3D drivers we ever expect to find */
#define MAXZBUFFERS				8	/* Maximum number of ZBuffer formats we ever expect to find */

#define SW		0
#define HW		1
#define MIXED	2

#define TIMER_ID			99	/* We don't want an obvious 1 for an ID in order to avoid any 
								   clash with the scene file's own counters */
#define TIMER_PERIOD		2000	/* Frame rate will be computed every 2 s */

#define SWAP(x)				(x)=!(x)

#undef RELEASE
//#define RELEASE(x)	if (x) { IDirect3D9_Release(x); (x)=NULL; }
#define RELEASE(x)		if (x) { sprintf(pszTmp, "%s released. Ref count=%d\n", #x, IDirect3D9_Release(x)); OutputDebugString(pszTmp); (x)=NULL; }

#define DEBUG(x)	OutputDebugString(x);

#define DISPLAYREFCOUNT(x)		{ if (x) { IDirect3D9_AddRef(x); sprintf(pszTmp, "\n\n%s ref count = %d\n", #x, IDirect3D9_Release(x)); OutputDebugString(pszTmp); } }

/* Uncomment the lines below to display debug messages on screen in a message box */

/*
#define DEBUG(x)	DEBUG(x); MessageBox(D3DFE.hwnd, x, "Debug message", MB_OK | MB_ICONERROR)
char *ErrorToString(HRESULT hres);
#undef DisplayHRESULTInDebug
#define DisplayHRESULTInDebug(x)	OutputDebugString(ErrorToString(x))
*/

#define MAX_NUMBER_OF_ACCELERATORS		50		/* Maximum number of accelerators */
#define MAX_NUMBER_OF_STRINGS			50		/* Maximum number of strings */
#define MAX_NUMBER_OF_TEXTURE_FORMAT	30		/* Maximum number of texture formats */
#define MAX_NUMBER_OF_TEXTURES			256		/* Maximum number of textures */

/************************************************
* Menu Properties - size units in % screen size *
************************************************/
#ifdef D3DSHELL_USE_3D_MENUS
#define MENU3D_FADE_SPEED			750.0f

#define MENU3D_COL_BG_SELECTED		0xFFE0E0FF	/* light blue */
#define MENU3D_COL_BG_NOTSELECTED	0xFF202080	/* dark blue */

#define MENU3D_COL_FONT_GHOST		0xFF606080	/* grey */
#define MENU3D_COL_FONT_NORMAL		0xFFFFFFFF	/* white */
#define MENU3D_COL_FONT_HEADING		0xFFFFFFFF	/* white */
#define MENU3D_COL_FONT_SELECTED	0xFF202080	/* dark blue */

#define MENU3D_COL_BG_HEADING		0xFFE0E0FF	/* light blue */
#define MENU3D_COL_BG_HEADING_NS	0xFF202080	/* dark blue */

#define MENU3D_FONT_SIZE_HEADING	0.50f	/* size of font used for headings */
#define MENU3D_FONT_SIZE_ELEMENT	0.50f	/* size of font used for menu items */
#define MENU3D_X_START_OFFSET		0.0f	/* position menu starts from left of screen */
#define MENU3D_Y_START_OFFSET		0.0f	/* position menu starts from top of screen */
#define MENU3D_HEADING_OFFSET		0.3f	/* gap between headings and elements */
#define MENU3D_POPUP_OFFSET			0.2f	/* gap between elements and popups */
#define MENU3D_HEADING_HEIGHT		5.0f	/* height of each heading cell */
#define MENU3D_ELEMENT_HEIGHT		5.0f	/* height of each element cell */


#define MENU3D_HEADING_MAX_X	20		/* max number of chars in element before crop */
#define MENU3D_ELEMENT_MAX_X	150		/* max number of chars in heading before crop */
#endif

/*************************
**		  Typedefs	    **
*************************/
typedef struct _AdapterInfo
{
	D3DADAPTER_IDENTIFIER9	Id;							/* Adapter identifier */
	
	D3DDISPLAYMODE			*pAllMode;					/* All display modes as listed by the enumeration function */
	int						nTotalNumberOfDisplayModes;	/* Number of all enumerated display modes */

	int						nNumberOfDisplayModes;		/* Number of display modes, not taking into account different refresh rates */
	D3DDISPLAYMODE			Mode[MAXDISPLAYMODES];		/* Array of display modes available for this adapter */
	int						nSafeDisplayModeNumber;		/* Number of "safe" display mode (usually 640x480x16) */

	BOOL					bCanDo60Hz;					/* Does any resolution support 60Hz? */
	BOOL					bCanDo70Hz;					/* Does any resolution support 70Hz? */
	BOOL					bCanDo72Hz;					/* Does any resolution support 72Hz? */
	BOOL					bCanDo75Hz;					/* Does any resolution support 75Hz? */
	BOOL					bCanDo85Hz;					/* Does any resolution support 85Hz? */
	BOOL					bCanDo90Hz;					/* Does any resolution support 90Hz? */
	BOOL					bCanDo100Hz;				/* Does any resolution support 100Hz? */
	BOOL					bCanDo120Hz;				/* Does any resolution support 120Hz? */
	BOOL					bCanDo150Hz;				/* Does any resolution support 150Hz? */
	
	D3DCAPS9				HALCaps;					/* D3DCAPS of HAL device on this adapter */
	BOOL					bCanRenderWindowed;			/* Indicates that this DD driver can render windowed */
	BOOL					bSharedVideoMemory;			/* Indicate if Frame buffer and Z-Buffer memory are shared */
	DWORD					dwPhysicalVideoMemory;		/* Amount of physical video memory for a DirectDraw driver */
	DWORD					dwTotalMemoryForZBuffer;	/* Total video memory to create Z-Buffer (variable) */
	DWORD					dwTotalMemoryForWindow;		/* Total video memory to create window (variable) */
} AdapterInfo;


typedef struct _D3DFEVariables
{
	/* Surface variables */
	LPDIRECT3DSURFACE9	 lpDDSPrimary;			/* Primary surface */
	LPDIRECT3DSURFACE9   lpDDSBack;				/* Back buffer surface */
	LPDIRECT3DSURFACE9   lpDDSZBuffer;			/* Z-Buffer surface */
	
	/* Direct3D variables */
	LPDIRECT3D9			 lpD3D9;				/* Direct3D9 object */
	LPDIRECT3DDEVICE9WRAPPED	lpDev9;			/* Direct3D device object */

	/* Window management variables */
	MSG					 msg;					/* WindowProc messages */
	HWND				 hwnd;					/* window handle */
	WNDCLASS			 wndclass;				/* window class */
	HINSTANCE			 hPreviousInstance;		/* Previous instance */
	HICON				 hIcon;					/* Application icon */
	DWORD				 dwGDIWidth;			/* GDI screen width */
	DWORD				 dwGDIHeight;			/* GDI screen height */
	DWORD				 dwGDIBPP;				/* GDI screen colour depth */
	BOOL				 bUseRefreshRates;		/* Indicate whether refresh rates should be used or not */
	
	/* Window information variables */
	DWORD				 dwWindowStyle;			/* Style of window */
	DWORD				 dwWindowPositionX;		/* Window position (X) */
	DWORD				 dwWindowPositionY;		/* Window position (Y) */
	DWORD				 dwWindowWidth;			/* Window width */
	DWORD				 dwWindowHeight;		/* Window height */
	DWORD				 dwWindowFramePositionX;/* Window frame position (X) */
	DWORD				 dwWindowFramePositionY;/* Window frame position (Y) */
	DWORD				 dwWindowFrameWidth;	/* Window frame width */
	DWORD				 dwWindowFrameHeight;	/* Window frame height */
	DWORD				 dwWindowSpaceX;		/* Total space between window frame width and client area width */
	DWORD				 dwWindowSpaceY;		/* Total space between window frame height and client area height */
	DWORD				 dwMaxWindowWidth;		/* Max window width */
	DWORD				 dwMaxWindowHeight;		/* Max window height */
	DWORD				 dwMinWindowWidth;		/* Min Window width */
	DWORD				 dwMinWindowHeight;		/* Min window height */
	BOOL				 bMenuTakesMoreThanOneLine;	/* Needed to display information text at correct location */
	
	/* Menu variables */
	HMENU				 hMenu;					/* Main menu handle */
	HMENU				 hFileMenu;				/* File menu handle */
	HMENU				 hDevicesMenu;			/* Devices menu handle */
	HMENU				 hBufferingMenu;		/* Buffering menu handle */
	HMENU				 hModesMenu;			/* Modes menu handle */
	HMENU				 hUserMenu;				/* User menu handle */
	ACCEL				 Accel[MAX_NUMBER_OF_ACCELERATORS];	/* Accelerator table */
	HACCEL				 hAccel;					/* Accelerator table handle */
	
	/* Booleans */
	BOOL				 bHasFocus;				/* Is the window currently in focus ? */
	BOOL				 bMinimized;			/* D3DShell minimized? */
	BOOL				 bRenderingReady;		/* Are all the variables set up and ready to render? */
	BOOL				 bIgnoreWM_SIZE;		/* Ignore the WM_SIZE window message? */
	BOOL				 bWindowResized;		/* Had window been resized? */
	BOOL				 bFullScreen;			/* Are we in FullScreen mode? */
	BOOL				 bZBufferNeeded;		/* Do we need a Z-Buffer? */
	BOOL				 bClearViewport;		/* Will the viewport be cleared before each frame? */
	BOOL				 bDisplayHasChanged;	/* Needed to recreate window mode after restore from minimise state */
	BOOL				 bInitViewWasCalled;	/* Needed to avoid calling ReleaseView() more than once */
	BOOL				 bAutoKill;				/* Application will auto-finish after FRAMESBEFOREKILLING frames */
	BOOL				 bAutoKillInProgress;	/* Application is autokilling. Used for screen saver. */
	BOOL				 bDisableZClear;		/* Did the user specify DISABLE_Z_CLEAR? */
	BOOL				 bUse3DMenus;			/* Does user want 3D menus? */
	BOOL				 bDisplay3DMenuIcons;   /* Flag set by user. Icons for info-pages on menu bar */
	BOOL				 bUseLockableBackBuffer;/* Does the user want a lockable back buffer? */
	BOOL				 bCaptureEachFrame;		/* Special mode where each frame is captured to a .BMP */
	
	/* Rendering variables */
	int					 nNumBackBuffer;		/* Number of back buffers (can only be 1 or 2) */
	int					 nCurrentAdapter;		/* Number of current adapter */
	int					 nCurrentResolution;	/* Number of current display mode */
	D3DMULTISAMPLE_TYPE	 MultiSamplingType;		/* Number of samples used in Multisampling AA */
	DWORD				 dwMultiSamplingQuality;/* Current multisampling quality setting */
	D3DDEVTYPE			 CurrentDeviceType;		/* HAL, REF or SW */
	D3DFORMAT			 CurrentZBufferFormat;	/* Current depth buffer format */
	D3DFORMAT			 CurrentBackBufferFormat;	/* Current back buffer format */
	DWORD				 dwCurrentRefreshRate;	/* Current refresh rate */
	int					 nVPType;				/* Type of Vertex Processing. 0 = SW, 1 = HW, 2 = MIXED */
	BOOL				 bUsePureDevice;		/* Use Pure device (if supported) */
	BOOL				 bVSyncOn;				/* VSync ON or OFF */
	float				 fFrameRate;			/* Current frame rate */
	DWORD				 dwFramesElapsed;		/* Number of frames elapsed in TIMER_PERIOD ms */
	int					 nScreenCaptureFrame;	/* Frame number to capture */
	int					 nAutokillTime;			/* Frame number to terminate on */
	DWORD				 dwClearColor;			/* Colour to clear the viewport to */
	DWORD				 dwNumberOfPresent;		/* Number of times Present() was called */
	int					 nScreenSaverSetting;	/* Screen saver setting from command line */
	
	/* User variables */
	DWORD				 dwUserPreferences;			/* Flag to retrieve user preferences */
	char				 pszFrontEndTitle[128];		/* Title of application */
	char				 pszCaptureDirectory[256];	/* Directory to write captured bitmaps to */
	char				 pszPausedMessage[64];		/* Default paused message */
} D3DFEVariables;


/* 3D Menu Structs */
typedef struct _MENU_ITEM				/* the structure used by all items in the menu */
{
	char				pszText[128];	/* the text to appear in this menu item */
	DWORD 				dwNumber;		/* the 3d window handle of this item */
	BOOL				bGhosted;		/* whether this item is ghosted (greyed) or not */
	BOOL				bHighlighted;	/* whether this item is highlighted or not */
	BOOL				bTicked;		/* whether this item is ticked or not */
	BOOL				bSeparator;		/* whether this item has a separator below it */
	int					nSeparator_num;	/* separator 3d window handle of this separator */
	BOOL				bSub_menu;		/* whether this item has a sub menu */
	int					nSub_menu_num;	/* the location of this sub menu in pM_Sub_Columns */
	UINT_PTR			uIdentifier;	/* the command associated with this item, passed to D3DFEWindowProc */
} MENU_ITEM;

typedef struct _MENU_COLUMN				/* the structure used by all columns in the menu */
{
	char				pszTitle[128];	/* the text to appear at the top of this column */
	DWORD 				dwNumber;		/* the 3d window handle of this column */
	float				fWidth;			/* width of this column */
	float				fItem_width;	/* max item width used for all items in this column */
	BOOL				bHighlighted;	/* whether this column is highlighted or not */
	MENU_ITEM			*pElement;		/* pointer to all items in this column */
} MENU_COLUMN;

/* Enum */
typedef enum _RENDERING_VARIABLE
{
	ADAPTER					=0x001,
	D3DDEVICE				=0x002,
	VERTEXPROCESSING		=0x004,
	DEPTHBUFFER				=0x008,
	MULTISAMPLINGTYPE		=0x010,
	MULTISAMPLINGQUALITY	=0x020,
	NUMBACKBUFFER			=0x040,
	FULLSCREEN				=0x080,
	BACKBUFFERFORMAT		=0x100,
	REFRESHRATE				=0x200,
	RESOLUTION				=0x400	
} RENDERING_VARIABLE;


/*************************
**		  Globals	    **
*************************/

/* D3DShell variables structure */
static D3DFEVariables	D3DFE;

/* Enumeration callback variables */
static AdapterInfo		Adapter[MAXDADAPTERS];		/* Array to store all adapters */
static int				nNumAdapters=0;				/* Total number of Adapters */

/* General string used to display text */
static char				pszTmp[512];				


/* 3D Menu Globals */
static BOOL				bM_Menu_On			= TRUE;	/* are we showing the menu? (F9 to hide) */
static int				nM_Num_Columns		= 0;	/* total number of columns */
static int				nM_Num_Sub_Columns	= 0;	/* total number of sub columns */
static int				*pM_Num_Items=NULL;			/* array: number of items in each column */
static int				*pM_Num_Sub_Items=NULL;		/* array: number of items in each sub column */
static int				nM_Current_Heading	= -1;	/* current heading highlighted	(-1 if none) */
static int				nM_Current_Item		= -1;	/* current item highlighted		(-1 if none) */	
static int				nM_Current_Sub_Item	= -1;	/* current sub item highlighted	(-1 if none) */	
static MENU_COLUMN		*pM_Columns=NULL;			/* stores all columns */
static MENU_COLUMN		*pM_Sub_Columns=NULL;		/* stores all sub columns */
static float			fM_Mouse_X			= 0.0f;	/* cursor x position 0-100 */
static float			fM_Mouse_Y			= 0.0f;	/* cursor y position 0-100 */
static BOOL				bM_In_Menu			= FALSE;/* are we in a menu? */
static BOOL				bM_In_Pop_Up		= FALSE;/* are we in a popup menu? */
static DWORD			dwM_title_bar		= 0;	/* 3d window id of the title bar that runs to right edge */
static float			fM_title_bar_size   = 0.0f;	/* Used to control the right hand menus */

static DWORD dwM_col_bg_trans_menu		= 0x00000000;/* transparent dark blue */
static DWORD dwM_col_bg_trans_popup		= 0x20000000;/* transparent dark blue */


/************************
******* Prototypes ******
************************/
static void				D3DFEInitFrontEnd();
static void				D3DFEFinish();
static void				D3DFEReleaseAll();	//A
static BOOL				D3DFECheckForLostSurfaces();
static BOOL				D3DFECheckRenderingVariable(DWORD dwFlag, void *pRenderingVariable);
static BOOL				D3DFEInitialiseDirect3D(int nAdapter, BOOL bFullScreen, int nResolution, D3DMULTISAMPLE_TYPE MultiSamplingType, BOOL bZBufferNeeded, int nNumBackBuffer,  D3DDEVTYPE DeviceType, int nVPType, D3DFORMAT ZBufferFormat, D3DFORMAT BackBufferFormat, DWORD dwRefreshRate);
static BOOL				D3DFERender();		//A
static void				D3DFEDisplayToGDI();
int	WINAPI				WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
static long CALLBACK	D3DFEWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);		//A
static void				D3DFEHandleWM_PAINT(HWND hWindow);
static BOOL				D3DFEHandleWM_SIZE(LRESULT *lresult, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void				D3DFEHandleWM_SIZING(LRESULT *lresult, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL				D3DFEBuildMenus();
static void				D3DFECreateAccelerators();
static void				D3DFEUpdateMenus();
static void				D3DFEMouseVisible(BOOL bVisible);
static void CALLBACK	D3DFETimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
static BOOL				D3DFEScreenCapture(char *pszDirectory);
static void				D3DFESetWindowProperties();
static void				D3DFERemoveString(char *pszString, char *pszStringToRemove);
static float			D3DFEInstantaneousFPS();
static DWORD			D3DFEBPPToDDBD(int bpp);
static WORD				D3DFEGetNumberOfBits(DWORD dwMask);
static BOOL				D3DFEIsWindowsVersionOK(DWORD dwMajor, DWORD dwMinor, DWORD dwSPMajor);
static void				D3DFEGetUserCommandLine(char *pszOriginalCommandLine, char *pszRemainingCommandLine);
static void				D3DFEProcessCommandLine(char *pszOriginalCommandLine);
static BOOL				D3DFEFileExists(char *n);
static int				D3DFEGetResolution(int nAdapter, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);
static int				D3DFEGetBPP(D3DFORMAT Format);
static BOOL				D3DFEGetColorChannels(D3DFORMAT Format, BYTE *pR, BYTE *pG, BYTE *pB, BYTE *pA);
static D3DDEVTYPE		D3DFEPick3DDevice(int nAdapter, D3DDEVTYPE DevType, BOOL bFullScreen);
static D3DMULTISAMPLE_TYPE D3DFEPickMultiSampleType(int nAdapter, D3DDEVTYPE DevType, BOOL bFullScreen, D3DFORMAT BackBufferFormat, D3DFORMAT DepthStencilFormat, D3DMULTISAMPLE_TYPE DesiredMultiSampleType);
static D3DFORMAT		D3DFEPickDepthBufferFormat(int nAdapter, D3DDEVTYPE DevType, D3DFORMAT DisplayModeFormat, BOOL bUseStencil, BOOL bUseMultiSampling, D3DFORMAT DesiredFormat);
static D3DFORMAT		D3DFEPickBackBufferFormat(int nAdapter, D3DDEVTYPE DevType, D3DFORMAT DisplayModeFormat, BOOL bFullScreen, D3DFORMAT DesiredFormat);
static DWORD			D3DFEPickRefreshRate(int nAdapter, DWORD dwWidth, DWORD dwHeight, D3DFORMAT DisplayModeFormat, DWORD dwDesiredRefreshRate);
static void				D3DFEWaitForVSync();


/* 3D Menu Functions Start Here */
#ifdef D3DSHELL_USE_3D_MENUS
static BOOL				D3DFEConvertTo3DMenus();
static BOOL				D3DFECreate3DMenus();
static void				D3DFECountItems3DMenus();
static int				D3DFECountSubColumns3DMenus();
static void				D3DFECountSubItems3DMenus();
static void				D3DFEUpdate3DMenus();
static BOOL				D3DFEReset3DMenus();
static void				D3DFEMouse3DMenus(UINT message);
static char				*D3DFEFormatString3DMenus(char *input);
static void				D3DFERelease3DMenus();
static void				D3DFERelease3DMenusWindows();
#else
static void				D3DFERelease3DMenus() {}
static void				D3DFERelease3DMenusWindows() {}
static void				D3DFEUpdate3DMenus() {}
static void				D3DFEMouse3DMenus(UINT message) {}
#endif


/************************
******* Functions *******
************************/


/*******************************************************************************
 * Function Name  : D3DFEInitFrontEnd
 * Returns        : Nothing
 * Global Used    : D3DFE
 * Description    : Perform variables initialisation
 *******************************************************************************/
static void D3DFEInitFrontEnd()
{
	/* Init surface variables */
	D3DFE.lpDDSPrimary =		NULL;
	D3DFE.lpDDSBack =			NULL;		
	D3DFE.lpDDSZBuffer =		NULL;			
		
	/* Init Direct3D variables */
	D3DFE.lpD3D9 =				NULL;					
	D3DFE.lpDev9 =				NULL;				
	
	/* Window management variables */
	D3DFE.hIcon =				NULL;

	/* Init window variables */
	D3DFE.dwWindowStyle =		0;
	D3DFE.dwWindowWidth =		640;
	D3DFE.dwWindowHeight =		480;
	D3DFE.dwMaxWindowWidth =	1024;
	D3DFE.dwMaxWindowHeight =	768;
	D3DFE.dwMinWindowWidth =	128;
	D3DFE.dwMinWindowHeight =	128;
	D3DFE.bUseRefreshRates =	FALSE;

	/* Init Booleans */
	D3DFE.bHasFocus =			TRUE;
	D3DFE.bIgnoreWM_SIZE =		TRUE;
	D3DFE.bWindowResized =		FALSE;
	D3DFE.bMinimized =			FALSE;
	D3DFE.bZBufferNeeded =		FALSE;
	D3DFE.bRenderingReady =		FALSE;
	D3DFE.bFullScreen =			FALSE;
	D3DFE.CurrentBackBufferFormat = D3DFMT_R5G6B5;
	D3DFE.dwCurrentRefreshRate= D3DPRESENT_RATE_DEFAULT;
	D3DFE.bClearViewport =		FALSE;
	D3DFE.bDisplayHasChanged =	FALSE;
	D3DFE.bInitViewWasCalled =	FALSE;
	D3DFE.bAutoKill =			FALSE;
	D3DFE.bAutoKillInProgress =	FALSE;
	D3DFE.bDisableZClear =		FALSE;
	D3DFE.bUse3DMenus =			FALSE;
	D3DFE.bDisplay3DMenuIcons =	FALSE;
	D3DFE.bUseLockableBackBuffer=FALSE;
	D3DFE.bCaptureEachFrame=	FALSE;
		
	/* Init menu variables */
	D3DFE.hMenu =				NULL;
	D3DFE.hFileMenu =			NULL;
	D3DFE.hDevicesMenu =		NULL;
	D3DFE.hBufferingMenu =		NULL;
	D3DFE.hModesMenu =			NULL;
	D3DFE.hUserMenu =			NULL;
	D3DFE.hAccel =				NULL;

	/* Init rendering variables */
	D3DFE.nNumBackBuffer =		1;
	D3DFE.nCurrentAdapter =		-1;
	D3DFE.nCurrentResolution =	-1;
	D3DFE.MultiSamplingType =	D3DMULTISAMPLE_NONE;
	D3DFE.dwMultiSamplingQuality=0;
	D3DFE.CurrentDeviceType =	D3DDEVTYPE_REF;
	D3DFE.CurrentZBufferFormat= D3DFMT_D16;
	D3DFE.nVPType =				HW;
	D3DFE.bUsePureDevice =		FALSE;
	D3DFE.bVSyncOn =			TRUE;
	D3DFE.fFrameRate =			0.0f;
	D3DFE.dwFramesElapsed =		0;
	D3DFE.nScreenCaptureFrame = -1;
	D3DFE.nAutokillTime =		FRAMESBEFOREKILLING;
	D3DFE.dwClearColor =		D3DCOLOR_RGBA(150, 150, 150, 0);
	D3DFE.dwNumberOfPresent =	0;
	D3DFE.nScreenSaverSetting =	-1;
	
	/* User rendering variables */
	D3DFE.dwUserPreferences =	0;
	strcpy(D3DFE.pszFrontEndTitle, "D3DShell");
	strcpy(D3DFE.pszCaptureDirectory, "");
	strcpy(D3DFE.pszPausedMessage, "Loading");
}


/*******************************************************************************
 * Function Name  : D3DFEFinish
 * Returns        : Nothing
 * Global Used    : D3DFE
 * Description    : Quit program
 *******************************************************************************/
static void D3DFEFinish()
{
	/* Set states */
	D3DFE.bRenderingReady=FALSE;

	/* Destroy timer */
	KillTimer(D3DFE.hwnd, TIMER_ID);
	
	/* Release all variables */
	D3DFEReleaseAll();

	/* Send a WM_DESTROY message to the event loop */
	DestroyWindow(D3DFE.hwnd);

	/* Free 3D menus */
	//D3DFERelease3DMenusWindows();
	//D3DFERelease3DMenus();
}


/*******************************************************************************
 * Function Name  : D3DFEReleaseAll
 * Returns        : Nothing
 * Global Used    : D3DFE
 * Description    : Release all memory used by Direct3D variables
 *******************************************************************************/
static void D3DFEReleaseAll()
{
    int		i, j;
		
	/* Set state */
	D3DFE.bRenderingReady=FALSE;
	
	/* Call ReleaseView (not more than once thanks to bInitViewWasCalled) */
	if (D3DFE.bInitViewWasCalled)
	{
		ReleaseView();
		D3DFE.bInitViewWasCalled=FALSE;
	}

#ifdef D3DSHELL_USE_3D_MENUS
	if (D3DFE.bUse3DMenus)
	{
		/* Release text textures */
		D3DTPrint3DReleaseTextures();

		//tmp
		D3DFERelease3DMenusWindows();
		D3DFERelease3DMenus();
	}
#endif


	/* Release depth buffer surface (if any) */
	if (D3DFE.lpDDSZBuffer) 
	{
		RELEASE(D3DFE.lpDDSZBuffer);
	}

	/* Release 3D device */
#ifdef USE_3D_DEVICE_WRAPPER
	delete D3DFE.lpDev9;
	D3DFE.lpDev9 = 0;
#else
	RELEASE(D3DFE.lpDev9);
#endif
	
	/* Release menu handles */
	/* Important! The user menu must be detached from the main menu, otherwise it
	   will be destroyed as the same time the main menu is destroyed */
	if (D3DFE.hUserMenu)
	{
		char	pszString[64];
		HMENU	hUserFileMenu=NULL, hUserFileSubMenu=NULL;
		
		/* Look if there was a user File menu attached to the main one */
		for (i=0; i<GetMenuItemCount(D3DFE.hUserMenu); i++)
		{
			/* Get string for this menu item number i */
			GetMenuString(D3DFE.hUserMenu, i, pszString, 100, MF_BYPOSITION);

			/* Is this menu item called File or &File ? */
			if ( (lstrcmpi(pszString, "File")==0) ||
				 (lstrcmpi(pszString, "&File")==0))
			{
				/* Get user File menu handle */
				hUserFileMenu=GetSubMenu(D3DFE.hUserMenu, i);
				
				/* Parse through all menu item in user File menu */
				for (j=0; j<GetMenuItemCount(D3DFE.hFileMenu); j++)
				{
					/* Check if there are other submenus */
					hUserFileSubMenu=GetSubMenu(D3DFE.hFileMenu, j);

					/* There is, detach it */
					if (hUserFileSubMenu)
					{
						RemoveMenu(D3DFE.hFileMenu, (UINT)hUserFileSubMenu, MF_BYCOMMAND);
					}
				}
			}
			else
			{
				/* Detach each user's submenu from main menu */
				RemoveMenu(D3DFE.hMenu, (UINT)GetSubMenu(D3DFE.hUserMenu, i), MF_BYCOMMAND);
			}
		}
	}

	/* If main menu exists, then destroy it */
	if (D3DFE.hMenu)
	{
		DestroyMenu(D3DFE.hMenu);
	}
} 


/*******************************************************************************
 * Function Name  : D3DFECheckRenderingVariable
 * Returns        : TRUE if no error occured
 * Global Used    : D3DFE
 * Description    : Check if selected rendering variables can actually be used.
 *					All rendering variables will be tested on the DirectX
 *					configuration specified and modified if necessary.
 *******************************************************************************/
static BOOL	D3DFECheckRenderingVariable(DWORD dwFlag, void *pRenderingVariable)
{
	int						nAdapter;
	BOOL					bFullScreen;
	int						nResolution;
	D3DMULTISAMPLE_TYPE		MultiSamplingType;
	DWORD					dwMultiSamplingQuality;
	D3DFORMAT				DepthBufferFormat;
	D3DFORMAT				BackBufferFormat;
	int						nNumBackBuffer;
	D3DDEVTYPE				DeviceType;
	int						nVPType;
	DWORD					dwRefreshRate;
	D3DFORMAT				DisplayModeFormat;
	HDC						hdc;
	int						nNewColorDepth;


	//TMP: is this really required?
	/* If we are in window mode, take note of GDI resolution and color depth */
	if (!D3DFE.bFullScreen)
	{
		/* Get Screen default dimensions and bpp */
		D3DFE.dwGDIWidth =	GetSystemMetrics(SM_CXSCREEN);
		D3DFE.dwGDIHeight =	GetSystemMetrics(SM_CYSCREEN);
		hdc =				GetDC(NULL);
		D3DFE.dwGDIBPP =	GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC(NULL, hdc);
	}


	/************************
	** Adapter has changed **
	************************/
	if (dwFlag & ADAPTER)
	{
		nAdapter = *(DWORD *)pRenderingVariable;

		/* If adapter has changed, then use safe resolution and 3D device */
		if (nAdapter!=D3DFE.nCurrentAdapter)
		{
			/* Use safe resolution */
			nResolution = Adapter[nAdapter].nSafeDisplayModeNumber;
			
			/* Try to pick use 3D device */
			DeviceType = D3DFEPick3DDevice(nAdapter, D3DFE.CurrentDeviceType, D3DFE.bFullScreen);
		
			/* Check whether hardware can do HW T&L or not */
			if (DeviceType==D3DDEVTYPE_HAL)
			{
				if (Adapter[nAdapter].HALCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) nVPType=HW;
					else nVPType=SW;
			}
			else
			{
				nVPType = SW;
			}

			/* Update rendering variables and return */
			D3DFE.nCurrentAdapter =		nAdapter;
			D3DFE.nCurrentResolution =	nResolution;
			D3DFE.CurrentDeviceType =	DeviceType;
			D3DFE.nVPType =				nVPType;

			return TRUE;
		}
	}


	/**************************
	** 3D device has changed **
	**************************/
	if (dwFlag & D3DDEVICE)
	{
		DeviceType = *(D3DDEVTYPE *)pRenderingVariable;
		nVPType = D3DFE.nVPType;

		/* Get current display mode format */
		if (D3DFE.bFullScreen) 
		{
			DisplayModeFormat = Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format;
		}
		else 
		{
			DisplayModeFormat = D3DFE.dwGDIBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		}

		/* Try to keep same vertex processing type */
		if (DeviceType==D3DDEVTYPE_HAL && 
			!(Adapter[D3DFE.nCurrentAdapter].HALCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
		{
			nVPType = SW;
		}

		/* Try to keep same back buffer format */
		BackBufferFormat = D3DFEPickBackBufferFormat(D3DFE.nCurrentAdapter, DeviceType, 
													 DisplayModeFormat, D3DFE.bFullScreen, D3DFE.CurrentBackBufferFormat);

		/* Try to keep same depth buffer format */
		DepthBufferFormat = D3DFEPickDepthBufferFormat(D3DFE.nCurrentAdapter, DeviceType, DisplayModeFormat,
													   D3DFE.dwUserPreferences & USE_STENCIL_BUFFER, D3DFE.MultiSamplingType!=D3DMULTISAMPLE_NONE, 
													   D3DFE.CurrentZBufferFormat);
		
		/* Try to keep current multisampling type */
		MultiSamplingType = D3DFEPickMultiSampleType(D3DFE.nCurrentAdapter, DeviceType,
													 D3DFE.bFullScreen, BackBufferFormat,
													 D3DFE.CurrentZBufferFormat, D3DFE.MultiSamplingType);

		/* Update rendering variables and return */
		D3DFE.CurrentDeviceType =		DeviceType;	
		D3DFE.nVPType =					nVPType;
		D3DFE.CurrentBackBufferFormat=	BackBufferFormat;
		D3DFE.CurrentZBufferFormat =	DepthBufferFormat;
		D3DFE.MultiSamplingType =		MultiSamplingType;

		return TRUE;
	}


	/**********************************
	** Vertex processing has changed **
	**********************************/
	if (dwFlag & VERTEXPROCESSING)
	{
		nVPType = *(int *)pRenderingVariable;

		/* Update rendering variables and return */
		D3DFE.nVPType =					nVPType;
	}

	
	/************************************
	** Depth buffer format has changed **
	************************************/
	if (dwFlag & DEPTHBUFFER)
	{
		DepthBufferFormat = *(D3DFORMAT *)pRenderingVariable;

		/* Update rendering variable and returns */
		if (DepthBufferFormat==(D3DFORMAT)0)
		{
			D3DFE.bZBufferNeeded = FALSE;
		}
		else
		{
			D3DFE.bZBufferNeeded = TRUE;
			D3DFE.CurrentZBufferFormat = DepthBufferFormat;
		}
		
		return TRUE;
	}


	/******************************
	** Multisampling has changed **
	******************************/
	if (dwFlag & (MULTISAMPLINGTYPE | MULTISAMPLINGQUALITY))
	{
		if (dwFlag & MULTISAMPLINGTYPE) 
		{
			MultiSamplingType =	*(D3DMULTISAMPLE_TYPE *)pRenderingVariable;
			dwMultiSamplingQuality = D3DFE.dwMultiSamplingQuality;
		}
		else 
		{
			MultiSamplingType = D3DFE.MultiSamplingType;
			dwMultiSamplingQuality = *(DWORD *)pRenderingVariable;
		}
		DepthBufferFormat = D3DFE.CurrentZBufferFormat;
		BackBufferFormat = D3DFE.CurrentBackBufferFormat;

		/* Get current display mode format */
		if (D3DFE.bFullScreen) 
		{
			DisplayModeFormat = Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format;
		}
		else 
		{
			DisplayModeFormat = D3DFE.dwGDIBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		}

		/* Is this multisampling mode supported with current back buffer? */
		if (IDirect3D9_CheckDeviceMultiSampleType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
												  D3DFE.CurrentBackBufferFormat,
												  !D3DFE.bFullScreen, MultiSamplingType,
												  &dwMultiSamplingQuality)!=D3D_OK)
			 
		{
			/* Make mouse visible */
			D3DFEMouseVisible(TRUE);

			/* This multisample type is not supported with current back buffer */
			if (MessageBox(D3DFE.hwnd, "This MultiSampling setting is not supported by the current BACK BUFFER format. \nPress OK to switch to a compatible back buffer or CANCEL to keep existing multisampling mode", D3DFE.pszFrontEndTitle, MB_OKCANCEL | MB_ICONWARNING)==IDOK)
			{
				/* OK: Set back buffer to compatible format */

				/* Find compatible back buffer format */
				if (D3DFEGetBPP(DisplayModeFormat)==32) BackBufferFormat = D3DFMT_X8R8G8B8; 
					else BackBufferFormat = D3DFMT_R5G6B5; 
			}
			else
			{
				/* Mouse is invisible in fullscreen */
				if (D3DFE.bFullScreen) D3DFEMouseVisible(FALSE);

				/* No: Ignore request */
				return FALSE;
			}
		}

		/* Is this multisampling mode supported with current depth buffer? */
		if (D3DFE.bZBufferNeeded)
		{
			if ( (IDirect3D9_CheckDeviceMultiSampleType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													    D3DFE.CurrentZBufferFormat,
													    !D3DFE.bFullScreen, MultiSamplingType,
													    &dwMultiSamplingQuality)!=D3D_OK) ||
				  (D3DFE.CurrentZBufferFormat == D3DFMT_D16_LOCKABLE) )
			{
				/* This multisample type is not supported with current depth buffer */
				if (MessageBox(D3DFE.hwnd, "This MultiSampling setting is not supported by the current DEPTH BUFFER format. \nPress OK to switch to a compatible depth buffer or CANCEL to keep existing multisampling mode", D3DFE.pszFrontEndTitle, MB_OKCANCEL | MB_ICONWARNING)==IDOK)
				{
					/* OK: Set depth buffer to compatible format */

					/* Find compatible depth buffer format */
					DepthBufferFormat = D3DFEPickDepthBufferFormat(D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, DisplayModeFormat,
																   D3DFE.dwUserPreferences & USE_STENCIL_BUFFER, TRUE, (D3DFORMAT)0);
				}
				else
				{
					/* Mouse is invisible in fullscreen */
					if (D3DFE.bFullScreen) D3DFEMouseVisible(FALSE);

					/* No: Ignore request */
					return FALSE;
				}
			}
		}

		/* Update rendering parameters and return */
		D3DFE.MultiSamplingType =		MultiSamplingType;
		D3DFE.dwMultiSamplingQuality=	dwMultiSamplingQuality;
		D3DFE.CurrentZBufferFormat =	DepthBufferFormat;
		D3DFE.CurrentBackBufferFormat = BackBufferFormat;
		
		return TRUE;
	}


	/**************************************
	** Number of back buffer has changed **
	**************************************/
	if (dwFlag & NUMBACKBUFFER)
	{
		nNumBackBuffer = *(int *)pRenderingVariable;

		/* Update rendering variable and returns */
		D3DFE.nNumBackBuffer = nNumBackBuffer;

		return TRUE;
	}


	/********************************
	** Fullscreen mode has changed **
	********************************/
	if (dwFlag & FULLSCREEN)
	{
		bFullScreen = *(BOOL *)pRenderingVariable;

		/* Fullscreen->Window */
		if (!bFullScreen & D3DFE.bFullScreen)
		{
			// To do: try to match fullscreen resolution in desktop mode */

			/* Update rendering variable and return */
			D3DFE.bFullScreen = bFullScreen;

			return TRUE;
		}
		else
		{
			/* Window->Fullscreen */

			/* Try to select same resolution as window mode */
			nResolution = D3DFEGetResolution(D3DFE.nCurrentAdapter, D3DFE.dwWindowWidth, D3DFE.dwWindowHeight, D3DFE.dwGDIBPP);
			if (nResolution==-1)
			{
				/* Use safe resolution */
				nResolution = Adapter[D3DFE.nCurrentAdapter].nSafeDisplayModeNumber;
			}

			/* Try to use same back buffer format */
			BackBufferFormat = D3DFEPickBackBufferFormat(D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
				D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format)==32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5, 
				TRUE, D3DFE.CurrentBackBufferFormat);

			/* Try to use 75Hz refresh rate */
			dwRefreshRate = D3DFEPickRefreshRate(D3DFE.nCurrentAdapter, 
												 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Width, 
												 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Height, 
												 D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format)==32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
												 75);
			
			/* Update rendering parameters and return */
			D3DFE.bFullScreen =				TRUE;
			D3DFE.nCurrentResolution =		nResolution;
			D3DFE.CurrentBackBufferFormat =	BackBufferFormat;
			D3DFE.dwCurrentRefreshRate =	dwRefreshRate;

			return TRUE;
		}
	}


	/***********************************
	** Back buffer format has changed **
	***********************************/
	if (dwFlag & BACKBUFFERFORMAT)
	{
		BackBufferFormat = *(D3DFORMAT *)pRenderingVariable;

		/* Get current display mode format */
		if (D3DFE.bFullScreen) 
		{
			DisplayModeFormat = Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format;
		}
		else 
		{
			DisplayModeFormat = D3DFE.dwGDIBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		}

		/* Check whether the new back buffer is supported */
		if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
									   DisplayModeFormat,
									   BackBufferFormat, !D3DFE.bFullScreen)!=D3D_OK)
		{
			/* Make mouse reappear */
			D3DFEMouseVisible(TRUE);

			/* Display info message */
			sprintf(pszTmp, "This back buffer format is not supported\nPress OK to default to a supported format (%s)",
				D3DFEGetBPP(DisplayModeFormat)==32 ? "X8R8G8B8" : "R5G6B5");
			if (MessageBox(D3DFE.hwnd, pszTmp, D3DFE.pszFrontEndTitle, MB_OKCANCEL | MB_ICONWARNING)==IDOK)
			{
				/* OK: set a supported back buffer format */
				if (D3DFEGetBPP(DisplayModeFormat)==32)
				{
					BackBufferFormat=D3DFMT_X8R8G8B8;
				}
				else
				{
					BackBufferFormat=D3DFMT_R5G6B5;
				}
			}
			else
			{
				/* No: Ignore request */

				/* Mouse is invisible in fullscreen */
				if (D3DFE.bFullScreen) D3DFEMouseVisible(FALSE);

				return FALSE;
			}
		}

		/* Check if new back buffer format is supported with current multisampling mode */
		if (IDirect3D9_CheckDeviceMultiSampleType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
												  BackBufferFormat,
												  !D3DFE.bFullScreen, D3DFE.MultiSamplingType,
												  &D3DFE.dwMultiSamplingQuality)!=D3D_OK)
		{
			/* Make mouse visible */
			D3DFEMouseVisible(TRUE);

			/* This multisample type is not supported */
			if (MessageBox(D3DFE.hwnd, "This new back buffer format is not supported with current MultiSampling settings. \nPress OK to switch to NO MULTISAMPLE or CANCEL to keep current format", D3DFE.pszFrontEndTitle, MB_OKCANCEL | MB_ICONWARNING)==IDOK)
			{
				/* OK: Set no multisampling */
				D3DFE.MultiSamplingType =		D3DMULTISAMPLE_NONE;
				D3DFE.dwMultiSamplingQuality =	0;
			}
			else
			{
				/* No: Ignore request */

				/* Mouse is invisible in fullscreen */
				if (D3DFE.bFullScreen) D3DFEMouseVisible(FALSE);

				return FALSE;
			}
		}

		/* Update rendering variable and return */
		D3DFE.CurrentBackBufferFormat = BackBufferFormat;

		return TRUE;
	}


	/*****************************
	** Refresh rate has changed **
	*****************************/
	if (dwFlag & REFRESHRATE)
	{
		dwRefreshRate = *(DWORD *)pRenderingVariable;

		/* No need to check if refresh rate is supported as this is already done 
		   in UpdateMenus() (unsupported refresh rates are grayed and thus cannot be selected) */

		/* Update rendering variable and return */
		D3DFE.dwCurrentRefreshRate = dwRefreshRate;

		return TRUE;
	}


	/***************************
	** Resolution has changed **
	***************************/
	if (dwFlag & RESOLUTION)
	{
		nResolution = *(int *)pRenderingVariable;

		/* Get color depth of desired resolution */
		nNewColorDepth = D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format);

		if (!D3DFE.bFullScreen)
		{
			/* Window mode */

			/* Check if it is the same as GDI color depth */
			if (nNewColorDepth!=(int)D3DFE.dwGDIBPP)
			{
				sprintf(pszTmp, "Your current GDI colour depth is %u bpp\nand does not correspond to the selected resolution.\nTherefore you can not set this resolution in Window Mode.\nSelect OK to set this resolution in FullScreen mode.", D3DFE.dwGDIBPP);
				if (MessageBox(D3DFE.hwnd, pszTmp, D3DFE.pszFrontEndTitle, MB_OKCANCEL | MB_ICONWARNING)==IDOK)
				{
					/* Pick best back buffer format corresponding to new color depth */
					BackBufferFormat = D3DFEPickBackBufferFormat(D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
																 nNewColorDepth==32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5, FALSE,
																 nNewColorDepth==32 ? D3DFMT_A8R8G8B8 : D3DFMT_R5G6B5);

					/* Update rendering variables and return */
					D3DFE.nCurrentResolution = nResolution;
					D3DFE.bFullScreen = TRUE;
					D3DFE.CurrentBackBufferFormat = BackBufferFormat;

					return TRUE;
				}
				else
				{
					/* No: ignore request */
					return FALSE;
				}
			}
			else
			/* Check if new window size exceeds maximum */
			if (Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Width > D3DFE.dwMaxWindowWidth || 
				Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Height > D3DFE.dwMaxWindowHeight)
			{
				sprintf(pszTmp, "This new window size is too large for your desktop.\nSelect OK to go to FullScreen mode.");
				if (MessageBox(D3DFE.hwnd, pszTmp, D3DFE.pszFrontEndTitle, MB_OKCANCEL | MB_ICONWARNING)==IDOK)
				{
					/* Update rendering variables and return */
					D3DFE.nCurrentResolution = nResolution;
					D3DFE.bFullScreen = TRUE;

					return TRUE;
				}
				else
				{
					/* No: ignore request */
					return FALSE;
				}
			}
			else
			{
				/* New window dimensions OK */
				D3DFE.dwWindowWidth =  Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Width;
				D3DFE.dwWindowHeight = Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Height;
				
				/* Reset window properties with new size */
				D3DFESetWindowProperties();

				return TRUE;
			}
		}
		else
		{
			/* Fullscreen mode */

			/* Retrieve display mode format of new resolution */
			DisplayModeFormat = D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format)==32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;

			/* Try to keep same back buffer format */
			BackBufferFormat = D3DFEPickBackBufferFormat(D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, DisplayModeFormat,
														 TRUE, D3DFE.CurrentBackBufferFormat);

			/* Try to keep same depth buffer format */
			DepthBufferFormat = D3DFEPickDepthBufferFormat(D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, DisplayModeFormat,
														   D3DFE.dwUserPreferences & USE_STENCIL_BUFFER, D3DFE.MultiSamplingType!=D3DMULTISAMPLE_NONE, D3DFE.CurrentZBufferFormat);

			/* Try to keep same refresh rate */
			dwRefreshRate = D3DFEPickRefreshRate(D3DFE.nCurrentAdapter, 
												 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Width, 
												 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Height, 
												 D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format)==32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
												 D3DFE.dwCurrentRefreshRate);

			/* Update rendering variables and return */
			D3DFE.nCurrentResolution =		nResolution;
			D3DFE.bFullScreen =				TRUE;
			D3DFE.CurrentBackBufferFormat = BackBufferFormat;
			D3DFE.CurrentZBufferFormat =	DepthBufferFormat;
			D3DFE.dwCurrentRefreshRate =	dwRefreshRate;

			return TRUE;
		}
	}

	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFEInitialiseDirect3D
 * Returns        : TRUE if no error occured
 * Global Used    : D3DFE, Adapter[]
 * Description    : Initialise everything for D3D use.
 *					Attach a menu to window for window mode, but not for 
 *					fullscreen mode.
 *******************************************************************************/
static BOOL	D3DFEInitialiseDirect3D(int nAdapter, BOOL bFullScreen, int nResolution, 
									D3DMULTISAMPLE_TYPE MultiSamplingType,
									BOOL bZBufferNeeded, int nNumBackBuffer,  
									D3DDEVTYPE DeviceType, int nVPType, 
									D3DFORMAT ZBufferFormat, D3DFORMAT BackBufferFormat,
									DWORD dwRefreshRate)
{
	HRESULT					hres;
	D3DPRESENT_PARAMETERS	pp;
	D3DVIEWPORT9			ViewportData;
	DWORD					dwCurrentWidth, dwCurrentHeight;
	DWORD					dwBehaviorFlag;
	LPDIRECT3DDEVICE9		lpDev9;


	/************
	** Asserts **
	************/
	ASSERT(D3DFE.lpD3D9);
		
	/* Debug info */
	DEBUG("*******************************\n");
	DEBUG("** D3DShell Initialising D3D **\n");
	DEBUG("*******************************\n");

	/* Set states */
	D3DFE.bRenderingReady=FALSE;

	/* Set globals */
	D3DFE.nCurrentAdapter=		nAdapter;
	D3DFE.bFullScreen=			bFullScreen;
	D3DFE.nCurrentResolution=	nResolution;
	D3DFE.MultiSamplingType=	MultiSamplingType;
	D3DFE.bZBufferNeeded=		bZBufferNeeded;
	D3DFE.nNumBackBuffer=		nNumBackBuffer;
	D3DFE.CurrentDeviceType=	DeviceType;
	D3DFE.nVPType=				nVPType;
	D3DFE.CurrentZBufferFormat= ZBufferFormat;
	D3DFE.CurrentBackBufferFormat= BackBufferFormat;
	D3DFE.dwCurrentRefreshRate= dwRefreshRate;


	/****************
	** Build menus **
	****************/
	if (!D3DFEBuildMenus())
	{
		D3DFEMouseVisible(TRUE);
		DEBUG("D3DShell *ERROR* : D3DFEBuildMenus() failed in D3DFEInitialiseDirect3D\n");
		/* No MessageBox here, as the only problem that could happen in D3DFEBuildMenus
		   already displays a MessageBox */
		return FALSE;
	}
	
	
	/*********************************************
	** Set Behavior flag for 3D device creation **
	*********************************************/
	switch(D3DFE.nVPType)
	{
		case SW:	dwBehaviorFlag = D3DCREATE_SOFTWARE_VERTEXPROCESSING;	break;
		case HW:	dwBehaviorFlag = D3DCREATE_HARDWARE_VERTEXPROCESSING;	break;
		case MIXED: dwBehaviorFlag = D3DCREATE_MIXED_VERTEXPROCESSING;		break;
	}

	/* Only display PURE DEVICE if not disabled */
	if (!(D3DFE.dwUserPreferences & DISABLE_PURE_DEVICE))
	{
		/* If pure device if supported and pure device is requested then use it */
		if ((Adapter[D3DFE.nCurrentAdapter].HALCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) && D3DFE.bUsePureDevice)
		{
			dwBehaviorFlag |= D3DCREATE_PUREDEVICE;
		}
	}


	/*******************************************************
	** Set presentation parameters for 3D device creation **
	*******************************************************/
	
	/* Reset structure */
	ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
	
	if (D3DFE.bFullScreen)
	{
		/* Fullscreen case */
		pp.BackBufferWidth =				Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
		pp.BackBufferHeight =				Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height;
		pp.BackBufferCount =				D3DFE.nNumBackBuffer;
		pp.SwapEffect =						(D3DFE.MultiSamplingType!=D3DMULTISAMPLE_NONE ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_FLIP);
		pp.Windowed =						FALSE;
		pp.FullScreen_RefreshRateInHz =		D3DFE.dwCurrentRefreshRate;;
		pp.PresentationInterval =			D3DFE.bVSyncOn ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	else
	{
		/* Windowed case */
		pp.BackBufferWidth =	D3DFE.dwWindowWidth;
		pp.BackBufferHeight =	D3DFE.dwWindowHeight;
		pp.BackBufferCount =	1;					/* Always double-buffered in Window mode */
		pp.SwapEffect =			(D3DFE.MultiSamplingType>1 ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_COPY);
		pp.Windowed =			TRUE;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// New for DX9
	}

	/* Common values for both fullscreen and window mode */
	pp.BackBufferFormat =		D3DFE.CurrentBackBufferFormat;
	pp.MultiSampleType =		D3DFE.MultiSamplingType; 
	pp.hDeviceWindow =			D3DFE.hwnd;
	pp.EnableAutoDepthStencil =	D3DFE.bZBufferNeeded;
	pp.AutoDepthStencilFormat =	D3DFE.bZBufferNeeded ? D3DFE.CurrentZBufferFormat : (D3DFORMAT)0;
	pp.Flags =					(D3DFE.bUseLockableBackBuffer ? D3DPRESENTFLAG_LOCKABLE_BACKBUFFER : 0) |
								(D3DFE.bZBufferNeeded ? (D3DFE.CurrentZBufferFormat==D3DFMT_D16_LOCKABLE ? 0 : D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL) : 0);
	

	/***********************
	** Create device call **
	***********************/
	hres=IDirect3D9_CreateDevice(D3DFE.lpD3D9, 
								 D3DFE.nCurrentAdapter, 
								 D3DFE.CurrentDeviceType, 
								 D3DFE.hwnd,
								 dwBehaviorFlag,
								 &pp,
								 &lpDev9);
	if (hres!=D3D_OK)
	{
		DEBUG("D3DShell *ERROR* : CreateDevice() failed.\n");
		DisplayHRESULTInDebug(hres);
		D3DFEMouseVisible(TRUE);
		MessageBox(D3DFE.hwnd, "Failed to create Direct3D device\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		return FALSE;
	}
#ifdef USE_3D_DEVICE_WRAPPER
	D3DFE.lpDev9 = new CDevWrap(lpDev9);
#else
	D3DFE.lpDev9 = lpDev9;
#endif

	/* Enable Z-Buffering */
	IDirect3DDevice9_SetRenderState(D3DFE.lpDev9, D3DRS_ZENABLE, D3DFE.bZBufferNeeded);


	/**************************************
	** Some special case for Window mode **
	**************************************/
	if (!D3DFE.bFullScreen)
	{
		/* Resize window to its new size */
		D3DFE.bIgnoreWM_SIZE=TRUE;
		SetWindowPos(D3DFE.hwnd, NULL, 
					 D3DFE.dwWindowFramePositionX, D3DFE.dwWindowFramePositionY, 
					 D3DFE.dwWindowFrameWidth, D3DFE.dwWindowFrameHeight, 
					 SWP_NOZORDER | SWP_SHOWWINDOW);
		D3DFE.bIgnoreWM_SIZE=FALSE;

		/* Show Cursor in window mode */
		D3DFEMouseVisible(TRUE);
	}
	else
	{
		/* Mouse visibility in fullscreen mode depends on whether 3D menus are visible */
		D3DFEMouseVisible(D3DFE.bUse3DMenus && bM_Menu_On);
	}


	/********************
	** Create Viewport **
	********************/
	/* Get dimensions of rendering surface */
	if (D3DFE.bFullScreen)
	{
		dwCurrentWidth =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
		dwCurrentHeight =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height;
	}
	else
	{
		dwCurrentWidth =	D3DFE.dwWindowWidth;
		dwCurrentHeight =	D3DFE.dwWindowHeight;
	}

	/* Setup the viewport structure for the viewing area */
	ZeroMemory(&ViewportData, sizeof(D3DVIEWPORT9));
	ViewportData.X =		0;
	ViewportData.Y =		0;
    ViewportData.Width =	dwCurrentWidth;
    ViewportData.Height =	dwCurrentHeight;
    ViewportData.MinZ =		0.0f;
    ViewportData.MaxZ =		1.0f;

	/* Set viewport */
	IDirect3DDevice9_SetViewport(D3DFE.lpDev9, &ViewportData);
	if (hres!=D3D_OK)
	{
		D3DFEMouseVisible(TRUE);
		DEBUG("D3DShell *ERROR* : SetViewport() failed.\n");
		DisplayHRESULTInDebug(hres);
		MessageBox(D3DFE.hwnd, "Failed to set viewport\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		RELEASE(D3DFE.lpDDSZBuffer);
#ifdef USE_3D_DEVICE_WRAPPER
		delete D3DFE.lpDev9;
		D3DFE.lpDev9 = 0;
#else
		RELEASE(D3DFE.lpDev9);
#endif
		return FALSE;
	}


#ifdef D3DSHELL_USE_3D_MENUS
	/*************
	** 3D menus **
	*************/
	if (D3DFE.bUse3DMenus)
	{
		/* Initialise text textures used by D3DTPrint3D */
		if (!D3DTPrint3DSetTextures(D3DFE.lpDev9, dwCurrentWidth, dwCurrentHeight))
		{
			OutputDebugString("Failed to initialise text textures.\n");
			return FALSE;
		}

		/* Convert menus to 3D menus */
		if (!D3DFEConvertTo3DMenus())
		{
			DEBUG("D3DShell *ERROR* : D3DFEConvertTo3DMenus failed in D3DFEInitialiseDirect3D\n");
			return FALSE;
		}

		/* Create menus */
		if (!D3DFECreate3DMenus())
		{
			DEBUG("D3DShell *ERROR* : D3DFECreate3DMenus failed in D3DFEInitialiseDirect3D\n");
			return FALSE;
		}
	}
#endif

	if (1)
	{
		COLORREF	crOldTextColor;
		HBRUSH		hOldBrush;
		HPEN		hOldPen;
		HDC			hdc;
		INT			oldMode, x, y, nStrLen;
		RECT		rect;
		SIZE		size;

		/* Get DC */
		hdc = GetDC(D3DFE.hwnd);

		/* Black background */
		hOldPen =	(HPEN)SelectObject(hdc, GetStockObject (NULL_PEN));
		hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject (BLACK_BRUSH));

		/* White text */
		oldMode =		 SetBkMode(hdc, TRANSPARENT);
		crOldTextColor = SetTextColor(hdc, RGB(255, 255, 255));

		/* Get client area */
		GetClientRect(D3DFE.hwnd, &rect);

		/* Clear the client area */
		Rectangle(hdc, rect.left, rect.top, rect.right + 1, rect.bottom + 1);

		/* Draw the string centered in the client area */
		nStrLen = strlen(D3DFE.pszPausedMessage);
		GetTextExtentPoint32(hdc, D3DFE.pszPausedMessage, nStrLen, &size);
		x = (rect.right  - size.cx) / 2;
		y = (rect.bottom - size.cy) / 2;
		TextOut(hdc, x, y, D3DFE.pszPausedMessage, nStrLen);

		/* Cleanup */
		SetTextColor(hdc, crOldTextColor);
		SetBkMode(hdc, oldMode);
		SelectObject(hdc, hOldBrush);
		SelectObject(hdc, hOldPen);

		/* Release DC */
		ReleaseDC(D3DFE.hwnd, hdc);
	}


	/********************
	** Call InitView() **
	********************/
	/* Call user function InitView to initialise the user rendering */
	if (!(InitView(D3DFE.lpD3D9, D3DFE.lpDev9, dwCurrentWidth, dwCurrentHeight)))
	{
		D3DFEMouseVisible(TRUE);
		DEBUG("D3DShell *ERROR* : InitView() failed.\n");
		if (!(D3DFE.dwUserPreferences & ENABLE_CLEAN_EXIT))
		{
			MessageBox(D3DFE.hwnd, "The InitView() function has failed\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		}
		RELEASE(D3DFE.lpDDSZBuffer);
#ifdef USE_3D_DEVICE_WRAPPER
		delete D3DFE.lpDev9;
		D3DFE.lpDev9 = 0;
#else
		RELEASE(D3DFE.lpDev9);
#endif
		return FALSE;
	}

#if 0
	/* In fullscreen mode, restrict mouse movement to client area as we don't 
	   want to click on the edges of the screen */
	if (D3DFE.bFullScreen)
	{
		RECT	rect;
		POINT	pt = { 0, 0 };
		
		/* Get screen coordinates rectangle of client area */
		GetClientRect(D3DFE.hwnd, &rect); 
		ClientToScreen(D3DFE.hwnd, &pt);
		rect.left +=   pt.x;
		rect.right +=  pt.x;
		rect.top +=    pt.y;
		rect.bottom += pt.y;
		
		/* Clip mouse cursor to this area */
		ClipCursor(&rect);
	}
	else
	{
		/* Reset any mouse cursor clipping */
		ClipCursor(NULL);
	}
#endif

	/* Update status */
	D3DFE.bInitViewWasCalled=TRUE;

	/* If we're not using 3D menus */
	if (!D3DFE.bUse3DMenus)
	{
		/* Update 2D menu choices */
		D3DFEUpdateMenus();
	}
	


/* DEBUG Info */
#ifdef _DEBUG
	OutputDebugString("\n-----------------------------------------------------------------------------------------\n");
	OutputDebugString("                  3D DEVICE CREATED:\n");
	sprintf(pszTmp, "                  Adapter: %s\n", Adapter[D3DFE.nCurrentAdapter].Id.Description);
	OutputDebugString(pszTmp);
	sprintf(pszTmp, "                  Driver version: %s\n", Adapter[D3DFE.nCurrentAdapter].Id.Driver);
	OutputDebugString(pszTmp);
	sprintf(pszTmp, "                  Vendor ID: 0x%04X\n                  Device ID: 0x%04X\n                  Sub-system ID: 0x%08X\n                  Revision: %d\n",
		Adapter[D3DFE.nCurrentAdapter].Id.VendorId, Adapter[D3DFE.nCurrentAdapter].Id.DeviceId,
		Adapter[D3DFE.nCurrentAdapter].Id.SubSysId, Adapter[D3DFE.nCurrentAdapter].Id.Revision);
	OutputDebugString(pszTmp);
	sprintf(pszTmp, "                  WHQL Level: %s\n", Adapter[D3DFE.nCurrentAdapter].Id.WHQLLevel ? "Certified" : "NOT certified");
	OutputDebugString(pszTmp);
	switch(D3DFE.CurrentDeviceType)
	{
		case D3DDEVTYPE_HAL:	if (D3DFE.bUsePureDevice)
								{
									sprintf(pszTmp, "                  Device type: HAL (PURE)\n");
								}
								else
								{
									sprintf(pszTmp, "                  Device type: HAL\n");
								}

		case D3DDEVTYPE_REF:	sprintf(pszTmp, "                  Device type: REF\n"); break;
		case D3DDEVTYPE_SW:		sprintf(pszTmp, "                  Device type: SW\n"); break;
	}
	OutputDebugString(pszTmp);
	switch(D3DFE.nVPType)
	{
		case SW: sprintf(pszTmp, "                  Vertex Processing: SW\n"); break;
		case HW: sprintf(pszTmp, "                  Vertex Processing: HW\n"); break;
		case MIXED: sprintf(pszTmp, "                  Vertex Processing: MIXED\n"); break;
	}
	OutputDebugString(pszTmp);
	sprintf(pszTmp, "                  %d Back buffer (%d*%d*%d %s)  %s %s %s\n", 
		pp.BackBufferCount, pp.BackBufferWidth, pp.BackBufferHeight, D3DFEGetBPP(BackBufferFormat), BackBufferFormat==D3DFMT_X1R5G5B5 ? "555" : (BackBufferFormat==D3DFMT_R5G6B5 ? "565" : (BackBufferFormat==D3DFMT_X8R8G8B8 ? "X888" : (BackBufferFormat==D3DFMT_A8R8G8B8 ? "8888" : "XXXX"))),
		D3DFE.bZBufferNeeded ? "Z-Buffer" : "", D3DFE.bFullScreen ? "FullScreen" : "Windowed", 
		D3DFE.bVSyncOn ? "VSync ON" : "VSync OFF");
	OutputDebugString(pszTmp);
	if (D3DFE.MultiSamplingType==D3DMULTISAMPLE_NONE)
	{
		sprintf(pszTmp, "                  Multisample: NONE\n");
	}
	else
	{
		sprintf(pszTmp, "                  Multisample: %d Samples\n", (int)D3DFE.MultiSamplingType);
	}
	OutputDebugString(pszTmp);
	sprintf(pszTmp, "                  Swap type: %s\n", pp.SwapEffect == D3DSWAPEFFECT_FLIP ? "FLIP" : "COPY");
	OutputDebugString(pszTmp);
	OutputDebugString("-----------------------------------------------------------------------------------------\n");
#endif
	
	/* Set status */
	D3DFE.bDisplayHasChanged=FALSE;
	D3DFE.dwNumberOfPresent = 0;

	/* New rendering variables have been set up correctly */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFEGetDirect3DConfig
 * Inputs		  : None
 * Global Used    : D3DFE
 * Description    : Get Direct3D configuration of all adapters present in the 
 *					system.
 *******************************************************************************/
static BOOL D3DFEGetDirect3DConfig()
{
	HRESULT				hres;
	int					i, j, k, nBackCount, nOffset, nTotalNumberOfDisplayModes;
	BOOL				bSwap, bAlreadyStored;
	D3DDISPLAYMODE		CurrentDisplayMode;
	
	/* Check that lpD3D9 is created */
	ASSERT(D3DFE.lpD3D9);


	/***************************
	** Enumerate all adapters **
	***************************/
	/* Get number of adapters */
	nNumAdapters = IDirect3D9_GetAdapterCount(D3DFE.lpD3D9);

	/* Reset adapters array */
	for (i=0; i<nNumAdapters; i++)
	{
		ZeroMemory(&Adapter[i], sizeof(Adapter[i]));
	}

	/* Loop through number of adapters and get details from them */
	for (i=0; i<nNumAdapters; i++)
	{
		/* Get adapter identifier */
		hres=IDirect3D9_GetAdapterIdentifier(D3DFE.lpD3D9, i, 0, &Adapter[i].Id);
		if (hres!=D3D_OK)
		{
			sprintf(pszTmp, "D3DShell *ERROR* : GetAdapterIdentifier() failed for adapter #%d.\n", i);
			DEBUG(pszTmp);
			DisplayHRESULTInDebug(hres);
			return FALSE;
		}
	}

	
	/*********************************************
	** Enumerate display modes for each adapter **
	*********************************************/
	for (i=0; i<nNumAdapters; i++)
	{
		/* Find out how many display modes we need to allocate */
		nTotalNumberOfDisplayModes = IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_X8R8G8B8) +
									 IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_A8R8G8B8) +
									 IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_A2R10G10B10) +
									 IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_X1R5G5B5) +
									 IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_A1R5G5B5) +
									 IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_R5G6B5);

		/* Allocate memory for all display modes */
		Adapter[i].pAllMode = (D3DDISPLAYMODE *)malloc(nTotalNumberOfDisplayModes * sizeof(D3DDISPLAYMODE));

		/* Reset booleans in structure */
		Adapter[i].bCanDo60Hz = FALSE;
		Adapter[i].bCanDo70Hz = FALSE;
		Adapter[i].bCanDo72Hz = FALSE;
		Adapter[i].bCanDo75Hz = FALSE;
		Adapter[i].bCanDo85Hz = FALSE;
		Adapter[i].bCanDo90Hz = FALSE;
		Adapter[i].bCanDo100Hz = FALSE; 
		Adapter[i].bCanDo120Hz = FALSE;
		Adapter[i].bCanDo150Hz = FALSE;

		/* Reset total number of display modes */
		nTotalNumberOfDisplayModes = 0;

		/* First enumerate all D3DFMT_X8R8G8B8 formats */
		nOffset = IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_X8R8G8B8);
		for (j=0; j<nOffset; j++)
		{
			hres=IDirect3D9_EnumAdapterModes(D3DFE.lpD3D9, i, D3DFMT_X8R8G8B8, j, &Adapter[i].pAllMode[nTotalNumberOfDisplayModes+j]);
		}
		nTotalNumberOfDisplayModes += nOffset;

		/* Then enumerate all D3DFMT_A8R8G8B8 formats */
		nOffset = IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_A8R8G8B8);
		for (j=0; j<nOffset; j++)
		{
			hres=IDirect3D9_EnumAdapterModes(D3DFE.lpD3D9, i, D3DFMT_A8R8G8B8, j, &Adapter[i].pAllMode[nTotalNumberOfDisplayModes+j]);
		}
		nTotalNumberOfDisplayModes += nOffset;

		/* Then enumerate all D3DFMT_A2R10G10B10 formats */
		nOffset = IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_A2R10G10B10);
		for (j=0; j<nOffset; j++)
		{
			hres=IDirect3D9_EnumAdapterModes(D3DFE.lpD3D9, i, D3DFMT_A2R10G10B10, j, &Adapter[i].pAllMode[nTotalNumberOfDisplayModes+j]);
		}
		nTotalNumberOfDisplayModes += nOffset;

		/* Then enumerate all D3DFMT_X1R5G5B5 formats */
		nOffset = IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_X1R5G5B5);
		for (j=0; j<nOffset; j++)
		{
			hres=IDirect3D9_EnumAdapterModes(D3DFE.lpD3D9, i, D3DFMT_X1R5G5B5, j, &Adapter[i].pAllMode[nTotalNumberOfDisplayModes+j]);
		}
		nTotalNumberOfDisplayModes += nOffset;

		/* Then enumerate all D3DFMT_A1R5G5B5 formats */
		nOffset = IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_A1R5G5B5);
		for (j=0; j<nOffset; j++)
		{
			hres=IDirect3D9_EnumAdapterModes(D3DFE.lpD3D9, i, D3DFMT_A1R5G5B5, j, &Adapter[i].pAllMode[nTotalNumberOfDisplayModes+j]);
		}
		nTotalNumberOfDisplayModes += nOffset;

		/* Then enumerate all D3DFMT_R5G6B5 formats */
		nOffset = IDirect3D9_GetAdapterModeCount(D3DFE.lpD3D9, i, D3DFMT_R5G6B5);
		for (j=0; j<nOffset; j++)
		{
			hres=IDirect3D9_EnumAdapterModes(D3DFE.lpD3D9, i, D3DFMT_R5G6B5, j, &Adapter[i].pAllMode[nTotalNumberOfDisplayModes+j]);
		}
		nTotalNumberOfDisplayModes += nOffset;

		/* Store total number of display modes */
		Adapter[i].nTotalNumberOfDisplayModes = nTotalNumberOfDisplayModes;
	
		/* Now store them in the list, making sure each display mode is stored only ONCE 
		   (only looking at width, height and BPP) */
		for (j=0; j<nTotalNumberOfDisplayModes; j++)
		{
			/* Update booleans in adapter structure */
			switch (Adapter[i].pAllMode[j].RefreshRate)
			{
				case 60:	Adapter[i].bCanDo60Hz = TRUE; break;
				case 70:	Adapter[i].bCanDo70Hz = TRUE; break;
				case 72:	Adapter[i].bCanDo72Hz = TRUE; break;
				case 75:	Adapter[i].bCanDo75Hz = TRUE; break;
				case 85:	Adapter[i].bCanDo85Hz = TRUE; break;
				case 90:	Adapter[i].bCanDo90Hz = TRUE; break;
				case 100:	Adapter[i].bCanDo100Hz = TRUE; break;
				case 120:	Adapter[i].bCanDo120Hz = TRUE; break;
				case 150:	Adapter[i].bCanDo150Hz = TRUE; break;
			}

			bAlreadyStored = FALSE;
			for (k=0; k<Adapter[i].nNumberOfDisplayModes; k++)
			{
				if ( (Adapter[i].pAllMode[j].Width == Adapter[i].Mode[k].Width) &&
					 (Adapter[i].pAllMode[j].Height == Adapter[i].Mode[k].Height) &&
					 (D3DFEGetBPP(Adapter[i].pAllMode[j].Format)==D3DFEGetBPP(Adapter[i].Mode[k].Format) ) )
				{
					bAlreadyStored=TRUE;
					break;
				}
			}

			/* Store display mode in list (only show display modes<640x480 in debug mode) */
			if (Adapter[i].nNumberOfDisplayModes<MAXDISPLAYMODES && !bAlreadyStored)
			{
#ifndef _DEBUG
				if (Adapter[i].pAllMode[j].Width>=640 && Adapter[i].pAllMode[j].Height>=480)
				{
#endif
				Adapter[i].Mode[Adapter[i].nNumberOfDisplayModes] = Adapter[i].pAllMode[j];
				Adapter[i].Mode[Adapter[i].nNumberOfDisplayModes].RefreshRate = 0;
				Adapter[i].nNumberOfDisplayModes++;
#ifndef _DEBUG
				}
#endif
			}
		}


		/* Once all modes have been enumerated, sort them in the list */
		nBackCount=0;
		do
		{
			bSwap=FALSE;
			nBackCount++;
			for (j=0; j<Adapter[i].nNumberOfDisplayModes-nBackCount; j++)
			{
				if (1000*D3DFEGetBPP(Adapter[i].Mode[j].Format)+Adapter[i].Mode[j].Width+Adapter[i].Mode[j].Height<
					1000*D3DFEGetBPP(Adapter[i].Mode[j+1].Format)+Adapter[i].Mode[j+1].Width+Adapter[i].Mode[j+1].Height)
				{
					/* Swap required */
					CurrentDisplayMode =	Adapter[i].Mode[j+1];
					Adapter[i].Mode[j+1] =	Adapter[i].Mode[j];
					Adapter[i].Mode[j] =	CurrentDisplayMode;
					bSwap=TRUE;
				}
			}
		}
		while (bSwap && nBackCount<Adapter[i].nNumberOfDisplayModes);

		/* Now find the "safe" display mode number (corresponding to 640x480x16) */
		Adapter[i].nSafeDisplayModeNumber=-1;
		for (j=0; j<Adapter[i].nNumberOfDisplayModes; j++)
		{
			if (Adapter[i].Mode[j].Width==640 && Adapter[i].Mode[j].Height==480 && 
				D3DFEGetBPP(Adapter[i].Mode[j].Format)==16)
			{
				Adapter[i].nSafeDisplayModeNumber=j;
			}
		}

		/* If 640x480x16 is not supported (you never know...) then choose first enumerated display mode as safe mode */
		if (Adapter[i].nSafeDisplayModeNumber==-1) Adapter[i].nSafeDisplayModeNumber=0;
	}



	/******************************
	/* Get caps for each adapter **
	******************************/
	for (i=0; i<nNumAdapters; i++)
	{
		hres=IDirect3D9_GetDeviceCaps(D3DFE.lpD3D9, i, D3DDEVTYPE_HAL, &Adapter[i].HALCaps);
		if (hres!=D3D_OK)
		{
			/* If no HAL just reset structure to zero */
			ZeroMemory(&Adapter[i].HALCaps, sizeof(D3DCAPS9));
		}
	}

	/* No problem occured */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFERender
 * Returns        : TRUE if no error occured
 * Global Used    : D3DFE
 * Description    : Render 3D scene.
 *					Call Present() to update the scene.
 *******************************************************************************/
static BOOL D3DFERender()
{
	HRESULT		hres;
	D3DRECT		dummy;
	RECT		rect;
	DWORD		dwClearFlags=0;
	static int	nFrameNumber=0;

	/* Assert */
	ASSERT(D3DFE.lpDev9);


	/****************************
	** Check cooperative level **
	****************************/
	/* Test the cooperative level to see if it's okay to render */
    hres = IDirect3DDevice9_TestCooperativeLevel(D3DFE.lpDev9);
    if (hres!=D3D_OK)
	{
        /* If the device was lost, do not render until we get it back */
        if (hres==D3DERR_DEVICELOST)
		{
			DEBUG("D3DShell *WARNING* : Device Lost - waiting for cooperative level\n");
			return TRUE;
		}

		/* Check if the device needs to be reset */
        if (hres==D3DERR_DEVICENOTRESET)
        {
			D3DFEReleaseAll();
			if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
										 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
										 D3DFE.MultiSamplingType,
										 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
										 D3DFE.CurrentDeviceType, D3DFE.nVPType,
										 D3DFE.CurrentZBufferFormat, D3DFE.CurrentBackBufferFormat,
										 D3DFE.dwCurrentRefreshRate))
			{
				D3DFEFinish();
				return FALSE;
			}
        }
		else
		{
			return FALSE;
		}
        
		/* Dont render until device is reset */
		return TRUE;
    }


	/* Has Autokill been specified in the command line ? */
	if (D3DFE.bAutoKill)
	{
		if (nFrameNumber>D3DFE.nAutokillTime)
		{
			/* Auto-terminate */
			sprintf(pszTmp, "D3DShell AUTOKILL : %d frames elapsed, quitting application\n", nFrameNumber);
			DEBUG(pszTmp);
			D3DFE.bAutoKillInProgress = TRUE; /* We are autokilling so let screen saver know */
			D3DFEFinish();
			return TRUE;
		}
	}

	/* Compute instantaneous frame rate if requested */
	if (D3DFE.dwUserPreferences & USE_INSTANTANEOUS_FPS)
	{
		D3DFE.fFrameRate = D3DFEInstantaneousFPS();
	}

	/* Clear values for rectangles */
	dummy.x1 =  0;
	dummy.y1 =  0;
	rect.left = 0;
	rect.top =  0;
	
	/* Retrieve blitting rectangle information*/ 
	if (D3DFE.bFullScreen)
	{
		dummy.x2 =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
		dummy.y2 =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height;
		rect.right= dummy.x2;
		rect.bottom=dummy.y2;
	}
	else
	{
		dummy.x2 =	D3DFE.dwWindowWidth;
		dummy.y2 =	D3DFE.dwWindowHeight;
		rect.right= D3DFE.dwWindowWidth;
		rect.bottom=D3DFE.dwWindowHeight;
	}

	/* Compute clear flags */
	if (D3DFE.bClearViewport)							dwClearFlags|=D3DCLEAR_TARGET;
	if (!D3DFE.bDisableZClear && D3DFE.bZBufferNeeded)	dwClearFlags|=D3DCLEAR_ZBUFFER;

	/* Clear frame buffer and/or Z-Buffer */
	if (dwClearFlags)
	{
		hres=IDirect3DDevice9_Clear(D3DFE.lpDev9, 0, NULL, dwClearFlags, D3DFE.dwClearColor, 1.0f, 0);
		if (hres!=D3D_OK)
		{
			DEBUG("D3DShell *WARNING* : Viewport clear failed in D3DFERender()\n");
			DisplayHRESULTInDebug(hres);
		}
	}

	/* Update 3D Menus */
	if (D3DFE.bUse3DMenus)
	{
		D3DFEUpdate3DMenus();
	}

	/* Call user rendering routine */
	if (!RenderScene(D3DFE.lpDev9))
	{
		DEBUG("D3DShell *ERROR* : RenderScene failed in D3DFERender()\n");
		return FALSE;
	}

	/* Present next frame */
	hres=IDirect3DDevice9_Present(D3DFE.lpDev9, NULL, NULL, D3DFE.hwnd, NULL);
	if (hres!=D3D_OK)
	{
		DEBUG("D3DShell *ERROR* : Present() failed in D3DFERender()\n");
		DisplayHRESULTInDebug(hres);
		return FALSE;
	}
	D3DFE.dwNumberOfPresent++;

	/* Capture each frame mode? */
	if (D3DFE.bCaptureEachFrame)
	{
		D3DFEScreenCapture(D3DFE.pszCaptureDirectory);
	}
	else
	{
		/* If screen capture has been selected and we've reached the frame number, capture the screen */
		if (D3DFE.nScreenCaptureFrame!=-1 && nFrameNumber==D3DFE.nScreenCaptureFrame)
		{
			D3DFEScreenCapture(D3DFE.pszCaptureDirectory);
		}
	}

	/* Increase frame number */
	nFrameNumber++;

	/* Increase the number of frames since the last frame rate computation */
	D3DFE.dwFramesElapsed++;

	/* The first render worked, so we are good */
	D3DFE.bRenderingReady=TRUE;

	/* No problem occured */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFEDisplayToGDI
 * Global Used    : D3DFE
 * Description    : Make the GDI surface the primary surface
 *******************************************************************************/
static void D3DFEDisplayToGDI()
{
	/* Asserts */
	ASSERT(D3DFE.hwnd);

	/* Number of presents */
	if (D3DFE.bFullScreen)
	{
		if (D3DFE.nNumBackBuffer==1)
		{
			/* Double buffer case */
			if (D3DFE.dwNumberOfPresent & 0x01)
			{
				IDirect3DDevice9_Present(D3DFE.lpDev9, NULL, NULL, NULL, NULL);
				D3DFE.dwNumberOfPresent++;
			}
		}
		else
		{
			/* The following code is to make sure the window is displayed on the front buffer */

			/* Triple buffer case */
			if (!(D3DFE.dwNumberOfPresent % 0x03))
			{
			}
			else
			if (!((D3DFE.dwNumberOfPresent+1) % 0x03))
			{
				IDirect3DDevice9_Present(D3DFE.lpDev9, NULL, NULL, NULL, NULL);
				D3DFE.dwNumberOfPresent++;
			}
			else
			{
				IDirect3DDevice9_Present(D3DFE.lpDev9, NULL, NULL, NULL, NULL);
				D3DFE.dwNumberOfPresent++;
				IDirect3DDevice9_Present(D3DFE.lpDev9, NULL, NULL, NULL, NULL);
				D3DFE.dwNumberOfPresent++;
			}
		}
	}


	/* Draw menu bar and window */
	D3DFE.bIgnoreWM_SIZE=TRUE;
	DrawMenuBar(D3DFE.hwnd);
	ShowWindow(D3DFE.hwnd, SW_SHOW);
	D3DFE.bIgnoreWM_SIZE=FALSE;
}


/*******************************************************************************
 * Function Name  : WinMain 
 * Description    : Main function of the program
 *******************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HDC				hdc;
	int				idle;
	int				done = FALSE;
	int				i;
	char			pszOrgCommandLine[512];
	DWORD			dwBPP;

	
	/**********************************
	** INITIALISE D3DSHELL VARIABLES **
	**********************************/
	D3DFEInitFrontEnd();


	/**************************
	** CHECK WINDOWS VERSION **
	**************************/

	/* Win98+ is REQUIRED to build DirectX  apps */
	if (!D3DFEIsWindowsVersionOK(4, 0, 0))
	{
		DEBUG("D3DShell *ERROR* : Windows 98 minimum is required to run D3DShell for DirectX8.\n");
		sprintf(pszTmp, "Windows 98 minimum is required to run D3DShell for DirectX8.", D3DFE.dwGDIBPP);
		MessageBox(NULL, pszTmp, D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		DEBUG("D3DShell : End of program\n");
		return 0;
	}

	/* Enumeration of refresh rates are only available on Win2K+ */
	D3DFE.bUseRefreshRates = D3DFEIsWindowsVersionOK(5, 0, 0);

	/* Set up and register window class */
	if (!hPrevInstance)	
	{
		D3DFE.wndclass.style=CS_HREDRAW | CS_VREDRAW;
		D3DFE.wndclass.lpfnWndProc=(WNDPROC)D3DFEWindowProc;
		D3DFE.wndclass.cbClsExtra=0;
		D3DFE.wndclass.cbWndExtra=0;
		D3DFE.wndclass.hInstance=hInstance;
		D3DFE.wndclass.hIcon=NULL;
		D3DFE.wndclass.hCursor=LoadCursor(NULL, IDC_ARROW);
		D3DFE.wndclass.hbrBackground=NULL; 
		D3DFE.wndclass.lpszMenuName=NULL;
		D3DFE.wndclass.lpszClassName="D3DShellClass";
		RegisterClass(&D3DFE.wndclass);
	}

	/* Get previous instance handle */
	D3DFE.hPreviousInstance=hPrevInstance;


	/*****************
	** WINDOW STUFF **
	*****************/

	/* Compute Window style */
	D3DFE.dwWindowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME;
	
	/* Get Screen default dimensions and bpp */
	D3DFE.dwGDIWidth =	GetSystemMetrics(SM_CXSCREEN);
	D3DFE.dwGDIHeight =	GetSystemMetrics(SM_CYSCREEN);
	hdc =				GetDC(NULL);
	D3DFE.dwGDIBPP =	GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(NULL, hdc);

	/* Check if colour depth is at least 16 bpp */
	if (D3DFE.dwGDIBPP<16)
	{
		DEBUG("D3DShell *ERROR* : Color depth is lower than 16 bpp.\n");
		sprintf(pszTmp, "Can not run program with a color depth of %u\nColor depth must be at least 16 bpp", D3DFE.dwGDIBPP);
		MessageBox(NULL, pszTmp, D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		DEBUG("D3DShell : End of program\n");
		return 0;
	}

	/* Update Max window width and height */
	D3DFE.dwMaxWindowWidth =  D3DFE.dwGDIWidth;
	D3DFE.dwMaxWindowHeight = (DWORD)(0.8f*D3DFE.dwGDIHeight);
	
	/* Change window size depending on GDI resolution */
	if (D3DFE.dwGDIWidth<800)
	{
		D3DFE.dwWindowWidth=512;
		D3DFE.dwWindowHeight=384;
	}
	if (D3DFE.dwGDIWidth<640)
	{
		D3DFE.dwWindowWidth=320;
		D3DFE.dwWindowHeight=240;
	}

	/* Check that window width and height do not exceed maximas */
	if (D3DFE.dwWindowWidth>D3DFE.dwMaxWindowWidth)
	{
		D3DFE.dwWindowWidth=D3DFE.dwMaxWindowWidth;
	}
	if (D3DFE.dwWindowHeight>D3DFE.dwMaxWindowHeight)
	{
		D3DFE.dwWindowHeight=D3DFE.dwMaxWindowHeight;
	}
	
	/* Set window properties */
	D3DFESetWindowProperties();
	
	/* create a window */
    D3DFE.hwnd = CreateWindowEx(0,
								"D3DShellClass",
								D3DFE.pszFrontEndTitle,
								D3DFE.dwWindowStyle,
								D3DFE.dwWindowFramePositionX,
								D3DFE.dwWindowFramePositionY,
								D3DFE.dwWindowFrameWidth,
								D3DFE.dwWindowFrameHeight,
								NULL,
								NULL,
								hInstance,
								NULL);

	/* If window could not be created */
    if (D3DFE.hwnd==NULL)
    {
        DEBUG("D3DShell *ERROR* : Window could not be created\n");
		MessageBox(NULL, "Unable to create Window", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		DEBUG("D3DShell : End of program\n");
		return 0;
    }

	
	/****************************************
	** At this point the window is created **
	****************************************/


	/*****************************
	** Create Direct3D 9 object **
	*****************************/
	D3DFE.lpD3D9=Direct3DCreate9(D3D_SDK_VERSION);
	if (!D3DFE.lpD3D9)
	{
		DEBUG("D3DShell *ERROR* : Direct3DCreate9() failed.\n");
		MessageBox(NULL, "Unable to create Direct3D9 object", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		DEBUG("D3DShell : End of program\n");
		return 0;
	}


	/*****************************
	** Get Direct3D information **
	*****************************/
	if (!D3DFEGetDirect3DConfig())
	{
		DEBUG("D3DShell *ERROR* : D3DFEGetDirect3DConfig() failed.\n");
		MessageBox(NULL, "Direct3D configuration does not match minimal requirements", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		RELEASE(D3DFE.lpD3D9);
		DEBUG("D3DShell : End of program\n");
		return 0;
	}

	
	/*********************************
	** Set default rendering values **
	*********************************/
	/* Use default adapter */
	D3DFE.nCurrentAdapter = 0;
									  
	/* Start in a window */
	D3DFE.bFullScreen =		FALSE;

	/* Use safe resolution for fullscreen mode */
	D3DFE.nCurrentResolution = Adapter[D3DFE.nCurrentAdapter].nSafeDisplayModeNumber;

	/* VSync */
	D3DFE.bVSyncOn =		TRUE;

	/* Use HAL 3D device with T&L by default */
	D3DFE.CurrentDeviceType =	D3DDEVTYPE_HAL;
	D3DFE.nVPType =				HW;
	
	/* Use a Z-Buffer by default */
	D3DFE.bZBufferNeeded =	TRUE;

	/* ZBuffer choice is 16 bits by default */
	D3DFE.CurrentZBufferFormat=D3DFMT_D16;

	/* Backbuffer choice is 565 if GDI is 16 bits, or X888 if GDI is in 32 bits */
	D3DFE.CurrentBackBufferFormat = (D3DFE.dwGDIBPP==16 ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8);

	/* Use default refresh rate */
	D3DFE.dwCurrentRefreshRate = D3DPRESENT_RATE_DEFAULT;

	/* Default buffering mode (for FullScreen) is double buffer */
	D3DFE.nNumBackBuffer =	1;
	

	/* Get user command line and exclude D3DShell-specific commands */
	D3DFEGetUserCommandLine(lpCmdLine, pszOrgCommandLine);


	/**************************************************
	** Call InitApplication to retrieve user choices **
	**************************************************/
	InitApplication(D3DFE.wndclass.hInstance, D3DFE.hwnd, pszOrgCommandLine);


	/*************************
	** PROCESS COMMAND LINE **
	*************************/
	D3DFEProcessCommandLine(lpCmdLine);


	/************************************************
	** Update rendering variables with user choice **
	************************************************/

	/* Fullscreen choice */
	if (D3DFE.dwUserPreferences & (DEFAULT_FULLSCREEN | FORCE_FULLSCREEN))
	{
		D3DFE.bFullScreen=TRUE;
	}

	/* VSync choice */
	if (D3DFE.dwUserPreferences & DEFAULT_NO_VSYNC)
	{
		D3DFE.bVSyncOn = FALSE;
	}

	/* ZBuffer choice */
	if (D3DFE.dwUserPreferences & (FORCE_ZBUFFER_ON))
	{
		D3DFE.bZBufferNeeded=TRUE;
	}

	if (D3DFE.dwUserPreferences & FORCE_DOUBLE_BUFFER)
	{
		D3DFE.nNumBackBuffer=1;
	}
	if (D3DFE.dwUserPreferences & (FORCE_TRIPLE_BUFFER | DEFAULT_TRIPLE_BUFFER))
	{
		D3DFE.nNumBackBuffer=2;
	}

	/* Ref. 3D device selected ? */
	if (D3DFE.dwUserPreferences & DEFAULT_USE_REF_DRIVER)
	{
		D3DFE.CurrentDeviceType = D3DDEVTYPE_REF;
	}

	/* Has a Lockable back buffer been requested? */
	if (D3DFE.dwUserPreferences & USE_LOCKABLE_BACK_BUFFER)
	{
		D3DFE.bUseLockableBackBuffer= TRUE;
	}

	/* 3D device choice */
	D3DFE.CurrentDeviceType = D3DFEPick3DDevice(D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, D3DFE.bFullScreen);

	/* Check whether hardware can do HW T&L or not */
	if (D3DFE.CurrentDeviceType==D3DDEVTYPE_HAL)
	{
		if (!(Adapter[D3DFE.nCurrentAdapter].HALCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
		{
			D3DFE.nVPType = SW;
		}
	}

	/* Back buffer format choice */
	if (D3DFE.bFullScreen) 
	{
		dwBPP = D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format);
	}
	else 
	{
		dwBPP = D3DFE.dwGDIBPP;
	}
	D3DFE.CurrentBackBufferFormat = D3DFEPickBackBufferFormat(D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
		dwBPP==32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5, D3DFE.bFullScreen, dwBPP==32 ? D3DFMT_A8R8G8B8 : D3DFMT_R5G6B5);

	/* Refresh rate choice: try to always use 75Hz */
	if (D3DFE.bFullScreen)
	{
		D3DFE.dwCurrentRefreshRate = D3DFEPickRefreshRate(D3DFE.nCurrentAdapter, 
												Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width, 
												Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height, 
												dwBPP==32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5, 75);
	}

	/* ZBuffer choice */
	D3DFE.CurrentZBufferFormat=D3DFEPickDepthBufferFormat(D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
														  dwBPP==32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
														  D3DFE.dwUserPreferences & USE_STENCIL_BUFFER, D3DFE.MultiSamplingType!=D3DMULTISAMPLE_NONE,
														  (D3DFORMAT)0);


	/*************************************
	** Adapt window to user preferences **
	*************************************/
	/* Application title */
	SetWindowText(D3DFE.hwnd, D3DFE.pszFrontEndTitle);

	/* Application icon */
	SetClassLong(D3DFE.hwnd, GCL_HICON, (LONG)D3DFE.hIcon);
	
	/* If disable resizing has been selected, change window style (remove thickframe) */
	if (D3DFE.dwUserPreferences & DISABLE_RESIZING)
	{
		D3DFE.dwWindowStyle^=WS_THICKFRAME;
		SetWindowLong(D3DFE.hwnd, GWL_STYLE, D3DFE.dwWindowStyle);
	}

	/* If window has been resized, then display the updated window */
	if (D3DFE.bWindowResized)
	{
		/* Set window size */
		D3DFESetWindowProperties();

		/* Resize window to its new size */
		D3DFE.bIgnoreWM_SIZE=TRUE;
		SetWindowPos(D3DFE.hwnd, NULL, 
					 D3DFE.dwWindowFramePositionX, D3DFE.dwWindowFramePositionY, 
					 D3DFE.dwWindowFrameWidth, D3DFE.dwWindowFrameHeight, 
					 SWP_NOZORDER | SWP_SHOWWINDOW);
		D3DFE.bIgnoreWM_SIZE=FALSE;
	}

	/* Set up cursor */
	SetCursor(D3DFE.wndclass.hCursor);
	D3DFEMouseVisible(TRUE);

	/* Create accelerator table */
	D3DFECreateAccelerators();

	/* Enable 3D Menus? */
	if (D3DFE.dwUserPreferences & USE_3D_MENUS)
	{
		D3DFE.bUse3DMenus=TRUE;
	}

	/* Display Print3D icons? */
	if (D3DFE.dwUserPreferences & DISPLAY_PRINT3D_ICONS)
	{
		D3DFE.bDisplay3DMenuIcons=TRUE;
	}

	/* Show and update window */
    ShowWindow(D3DFE.hwnd, nCmdShow);
    UpdateWindow(D3DFE.hwnd);

	
	/****************************************
	** Here starts Direct3D initialisation **
	****************************************/
	/* Initialise D3D with these defaults */
	if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter,
								 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
								 D3DFE.MultiSamplingType,
								 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
								 D3DFE.CurrentDeviceType, D3DFE.nVPType,
								 D3DFE.CurrentZBufferFormat, D3DFE.CurrentBackBufferFormat,
								 D3DFE.dwCurrentRefreshRate))
	{
		DEBUG("D3DShell *ERROR* : D3DFEInitialiseDirect3D() failed in Winmain().\n");
		MessageBox(D3DFE.hwnd, "Failed to initialise Direct3D\n",
				   D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
		QuitApplication();
		D3DFEReleaseAll();
		RELEASE(D3DFE.lpD3D9);
		DEBUG("D3DShell : End of program\n");
		return 0;
	}



	/* D3D Initialisation complete! */



	/* Create a timer used to calculate average frame rate */
	if (!(D3DFE.dwUserPreferences & USE_INSTANTANEOUS_FPS))
	{
		SetTimer(D3DFE.hwnd, TIMER_ID, TIMER_PERIOD, (TIMERPROC)D3DFETimerProc);
	}
		
			
	/**********************
	** Main message loop **
	**********************/
	while (!done)
    {   
		idle = TRUE;
		while (PeekMessage(&D3DFE.msg, NULL, 0, 0, PM_REMOVE))
		{
			idle=FALSE;
			if (D3DFE.msg.message==WM_QUIT)
			{	
				done=TRUE;
				break;
			}
			if (!TranslateAccelerator(D3DFE.hwnd, D3DFE.hAccel, &D3DFE.msg))
			{
					TranslateMessage(&D3DFE.msg);
					DispatchMessage(&D3DFE.msg);
			}
		}

			/* If we are not minimised, window has focus then render */
		if (!done && !D3DFE.bMinimized && D3DFE.bHasFocus)
		{
			/* Call the rendering routine */
			if (!D3DFERender())
			{
				/* If render fails, stop execution straight away */
				DEBUG("D3DShell : D3DFERender() failed\n");
				D3DFE.bRenderingReady=FALSE;
				D3DFEMouseVisible(TRUE);
				DEBUG("D3DShell : D3DFERender() has failed! Aborting program\n");
				if (!(D3DFE.dwUserPreferences & ENABLE_CLEAN_EXIT))
				{
					MessageBox(D3DFE.hwnd, "Rendering has failed\nAborting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
				}
				D3DFEFinish();
			}
		}
    }

	/* Give time to free all variables and erase windows */
	Sleep(100);
	
	/* Cleaning up */
	ClipCursor(NULL);
	DestroyAcceleratorTable(D3DFE.hAccel);
	RELEASE(D3DFE.lpD3D9);
	QuitApplication();

	/* Release display mode array for each adapter */
	for (i=0; i<nNumAdapters; i++)
	{
		free(Adapter[i].pAllMode);
	}

	/* Make sure mouse is visible before quitting */
	D3DFEMouseVisible(TRUE);

	/* Delete all windows created by 3DMenus and any Print3D Window left by the user */
#ifdef D3DSHELL_USE_3D_MENUS
	D3DTPrint3DDeleteAllWindows();
#endif

	/* End of program */
	DEBUG("D3DShell : End of program\n");
    return D3DFE.msg.wParam;
}


/*******************************************************************************
 * Function Name  : D3DFEWindowProc
 * Description    : Messages handling function
 *******************************************************************************/
long CALLBACK D3DFEWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT				lresult;
	RECT				Rectangle;
	DWORD				MenuMessage;
	DWORD				dwH, dwW, dwBPP, dwRefreshRate;
	D3DDEVTYPE			NewDevType;
	D3DMULTISAMPLE_TYPE	NewMultiSampleType;
	D3DFORMAT			DepthBufferFormat, BackBufferFormat;
	int					nNumber;
	int					nVPType;
	BOOL				bLocalFullScreen, bLocalZBufferNeeded;
	BOOL				bUsePureDevice=FALSE;
	RECT				rect;
	char				pszString[64];
	static BOOL			bInMenu=FALSE;
	int					dwCurrentWidth, dwCurrentHeight; 


	/* Get current render surface dimensions */
	if (D3DFE.bFullScreen)
	{
		dwCurrentWidth =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
		dwCurrentHeight =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height;
	}
	else
	{
		dwCurrentWidth =	D3DFE.dwWindowWidth;
		dwCurrentHeight =	D3DFE.dwWindowHeight;
	}

	/* Let the user process window message (menus and keystrokes) but 
	   only if menus are not currently used */
	if (!D3DFE.bUse3DMenus || (D3DFE.bUse3DMenus && !bM_In_Menu))
	{
		UserWindowProc(hWnd, message, wParam, lParam);
	}

	//sprintf(pszTmp, "bHasFocus = %s\nbRenderingReady = %s\n", D3DFE.bHasFocus ? "TRUE" : "FALSE", D3DFE.bRenderingReady ? "TRUE" : "FALSE");
	//DEBUG(pszTmp);
	
	/* FrontEnd window messages */
	switch(message)
    {
#ifdef D3DSHELL_USE_3D_MENUS	
	case WM_KEYDOWN:	
						if (D3DFE.bUse3DMenus)
						{
							switch(wParam)
							{
								case VK_UP:
									/* If we're in a popup */
									if (bM_In_Pop_Up)
									{
										/* If we're not at the top of the sub menu, move up one */
										if (nM_Current_Sub_Item > 0)
										{
											pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
											nM_Current_Sub_Item--;
											pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = TRUE;
										}
									}
									else
									{
										/* If we're not at the top of the menu, move up one */
										if (nM_Current_Item > 0)
										{
											pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
											nM_Current_Item--;
											pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = TRUE;
											dwM_col_bg_trans_popup = 0x00000000; 
										}
									}
									break;

								case VK_DOWN:
									if (bM_Menu_On==FALSE || bM_In_Menu== FALSE) 
									{ 
										break;
									}
									/* If we're in a popup */
									if (bM_In_Pop_Up)
									{
										/* If we're not at the bottom of the sub menu, move down one */
										if (nM_Current_Sub_Item < pM_Num_Sub_Items[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num] - 1)
										{
											pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
											nM_Current_Sub_Item++;
											pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = TRUE;
										}
									}
									else
									{
										/* If we're not at the bottom of the menu, move down one */
										if (nM_Current_Item < pM_Num_Items[nM_Current_Heading] - 1)
										{
											pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
											nM_Current_Item++;
											pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = TRUE;
											dwM_col_bg_trans_popup = 0x00000000; 
										}
									}
									break;

								case VK_LEFT:
									if (bM_Menu_On==FALSE || bM_In_Menu== FALSE) 
									{ 
										break;
									}
									/* If we're in a popup, move out of it */
									if (bM_In_Pop_Up)
									{
										pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
										bM_In_Pop_Up = FALSE;
									}
									else
									{
										/* If we're not at the left of menu, move left one */
										if (nM_Current_Heading > 0)
										{
											/* If this column isn't empty unhighlight current element */
											if (pM_Num_Items[nM_Current_Heading] > 0)
												pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
											pM_Columns[nM_Current_Heading].bHighlighted = FALSE;
											nM_Current_Heading--;
											pM_Columns[nM_Current_Heading].bHighlighted = TRUE;
											nM_Current_Item = 0;		//reset highlight to top of new menu
											//if previous column isn't empty highlight top element
											if (pM_Num_Items[nM_Current_Heading] > 0)
												pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = TRUE;
											dwM_col_bg_trans_menu		= 0x00000000;
										}
									}
									break;


								case VK_RIGHT:
									if (bM_Menu_On==FALSE || bM_In_Menu== FALSE) 
									{ 
										break;
									}

									/* If we're in a popup move to next column */
									if (bM_In_Pop_Up)
									{
										/* If there is a next column */
										if (nM_Current_Heading < nM_Num_Columns - 1)
										{
											//unhighlight this element
											pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
											//unhighlight this popup element
											pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
											pM_Columns[nM_Current_Heading].bHighlighted = FALSE;
											nM_Current_Heading++;
											pM_Columns[nM_Current_Heading].bHighlighted = TRUE;
											nM_Current_Item = 0;		//reset highlight to top of new menu
											nM_Current_Sub_Item = -1;
											bM_In_Pop_Up = FALSE;
											//highlight top of new menu, if there is one
											if (pM_Num_Items[nM_Current_Heading] > 0)
												pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = TRUE;
										}
										break;
									}
									//if this column isn't empty
									if ((pM_Num_Items[nM_Current_Heading] > 0) && (nM_Current_Item != -1))
									{
										//if this element has a sub menu
										if (pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bSub_menu)
										{
											//highlight first element in sub menu 
											pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[0].bHighlighted = TRUE;
											nM_Current_Sub_Item = 0;
											bM_In_Pop_Up = TRUE;
										}
										else
										{
											//if there is a next column
											if (nM_Current_Heading < nM_Num_Columns - 1)
											{
												pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
												pM_Columns[nM_Current_Heading].bHighlighted = FALSE;
												nM_Current_Heading++;
												pM_Columns[nM_Current_Heading].bHighlighted = TRUE;
												nM_Current_Item = 0;		//reset highlight to top of new menu
												nM_Current_Sub_Item = -1;
												//highlight top of new menu, if there is one
												if (pM_Num_Items[nM_Current_Heading] > 0)
													pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = TRUE;
												dwM_col_bg_trans_menu		= 0x00000000;
											}
										}
										break;
									}
									//this column empty so move to next
									else 
									{
										//if there is a next column
										if (nM_Current_Heading < nM_Num_Columns - 1)
										{
											//if this column had any items, unhighlight current
											if (pM_Num_Items[nM_Current_Heading] > 0)
												pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
											pM_Columns[nM_Current_Heading].bHighlighted = FALSE;
											nM_Current_Heading++;
											pM_Columns[nM_Current_Heading].bHighlighted = TRUE;
											nM_Current_Item = 0;		//reset highlight to top of new menu
											nM_Current_Sub_Item = -1;
											//highlight top of new menu, if there is one
											if (pM_Num_Items[nM_Current_Heading] > 0)
												pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = TRUE;
										}
										break;
									}

								case VK_RETURN:	//menu option selected
									if (bM_Menu_On==FALSE || bM_In_Menu== FALSE) 
									{ 
										break;
									}
									if (bM_In_Pop_Up)
									{
										//only activate if not ghosted
										if (pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bGhosted == FALSE)
										{
											//call handler with this ID
											//D3DFEWindowProc(D3DFE.hwnd, WM_COMMAND, pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].uIdentifier, 0);
											PostMessage(D3DFE.hwnd, WM_COMMAND, pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].uIdentifier, 0);
											//reset menus
											D3DFEReset3DMenus();
										}
									}
									else
									{
										//if this column isn't empty
										if (pM_Num_Items[nM_Current_Heading] > 0)
										{
											if (pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bGhosted == FALSE)
											{
												//call handler with this ID
												//D3DFEWindowProc(D3DFE.hwnd, WM_COMMAND, pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].uIdentifier, 0);
												PostMessage(D3DFE.hwnd, WM_COMMAND, pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].uIdentifier, 0);
												//reset menus
												D3DFEReset3DMenus();
											}
										}
									}
									break;
								}
							}
							break;
#endif
	
	case	WM_MOUSEMOVE :
	case	WM_LBUTTONUP :
	case	WM_LBUTTONDOWN :	if (D3DFE.bUse3DMenus)
								{
									/* Find out mouse position */
									GetClientRect(hWnd, &rect);

									fM_Mouse_X = (float)GET_X_LPARAM(lParam)*(dwCurrentWidth * (100.0f/640.0f))/rect.right;
									fM_Mouse_Y = (float)GET_Y_LPARAM(lParam)*(dwCurrentHeight * (100.0f/480.0f))/rect.bottom;
									
									/* Process mouse message */
									D3DFEMouse3DMenus(message);
								}
								break;

	case	WM_ENTERMENULOOP :	if (D3DFE.bFullScreen && !D3DFE.bUse3DMenus)
								{
									/* Make sure the window is visible */
									D3DFEDisplayToGDI();

									/* Make mouse cursor visible */
									D3DFEMouseVisible(TRUE);

									/* We are in menu mode */
									bInMenu=TRUE;
								}
								break; 

	case	WM_EXITMENULOOP :	if (D3DFE.bFullScreen && !D3DFE.bUse3DMenus)
								{
									/* Hide mouse cursor */
									D3DFEMouseVisible(FALSE);

						   			/* We are no longer in menu mode */
									bInMenu=FALSE;
								}
								break;


	/* Used to disable rendering if window is not activated */
	case	WM_ACTIVATEAPP :    if (!(BOOL)wParam)
								{
									D3DFE.bHasFocus=FALSE;
								}
								else
								{
									D3DFE.bHasFocus=TRUE;
								}
								break;

	case	WM_SYSCOMMAND :	    switch (wParam & 0xFFF0)
								{
									/* Close Window */
									case SC_CLOSE:	D3DFEFinish();
													break;
									
									/* Disable Screen saver */	
									case SC_SCREENSAVE:

									/* Disable monitor power down */
									case SC_MONITORPOWER:	return 1; break;

									/* Minimize has been selected */
									case SC_MINIMIZE:		D3DFE.bMinimized =	TRUE; 
															D3DFE.bHasFocus =	FALSE;
															break;

#ifdef D3DSHELL_USE_3D_MENUS
									/* Use menu keys to enable or disable menu when 3D menus are used */
									case SC_KEYMENU:		
															if (D3DFE.bUse3DMenus)
															{
																/* Are menus currently invisible? */
																if (!bM_Menu_On)
																{
																	/* Yes, so make them appear */

																	/* Change menu state */
																	SWAP(bM_Menu_On);

																	/* Display or hide the mouse in fullscreen mode ONLY */
																	if (D3DFE.bFullScreen) D3DFEMouseVisible(bM_Menu_On);

																	/* Reset 3D menus */
																	D3DFEReset3DMenus();
																}

																/* Are the menus currently highlighted? */
																if (!bM_In_Menu)
																{
																	/* No, highlight them now */
																	bM_In_Menu = TRUE;
																	nM_Current_Heading	= 0;
																	nM_Current_Item		= 0;
																	nM_Current_Item		= 0;		
																	nM_Current_Sub_Item = -1;
																	pM_Columns[nM_Current_Heading].bHighlighted = TRUE;
																	
																	if (pM_Num_Items[nM_Current_Heading] > 0)
																	{
																		pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = TRUE;
																	}
																}
																else
																{
																	/* Yes, so un-highlight them now */
																	bM_In_Menu = FALSE;
																	pM_Columns[nM_Current_Heading].bHighlighted = FALSE;
																	if (pM_Num_Items[nM_Current_Heading] > 0)
																	{
																		pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
																	}
																}

																/* Prevents Windows to process this message */
																return 1;
															}
															break;
#endif
									default : break;
								}
								return (DefWindowProc(hWnd, message, wParam, lParam));
								break;
								

	case	WM_SIZE :			/* Window resize (minimize or restore) */
								if (!D3DFE.bIgnoreWM_SIZE) 
								{
									D3DFEHandleWM_SIZE(&lresult, hWnd, message, wParam, lParam);
									return 0;
								}
								break;

	case	WM_MOVE :			/* Update window position information after a move */
								if (!D3DFE.bFullScreen && !D3DFE.bMinimized && !D3DFE.bIgnoreWM_SIZE && D3DFE.bHasFocus)
								{
									GetWindowRect(hWnd, &Rectangle);
									D3DFE.dwWindowFramePositionX=Rectangle.left; 
									D3DFE.dwWindowFramePositionY=Rectangle.top;
								}
								break;
	
	case	WM_SIZING :			/* Prevent the window to reach unwanted size */
								/* No resize allowed if DISABLE_RESIZING is set or if in Fullscreen mode */
								if ( (D3DFE.dwUserPreferences & DISABLE_RESIZING) || (D3DFE.bFullScreen) )
								{
									GetWindowRect(hWnd, (LPRECT)lParam);
									return 0;							
								}
								else
								{
									D3DFEHandleWM_SIZING(&lresult, hWnd, message, wParam, lParam);
								}
								break;
								
	case	WM_MOVING	:		/* Prevent the window from moving in fullscreen */
								if (D3DFE.bFullScreen) 
								{
									GetWindowRect(hWnd, (LPRECT)lParam);
									return 0;								
								}
								break;

	case	WM_PAINT :			/* Only handle WM_PAINT message if we're not in 3D menus */
								if (!bInMenu) 
								{
									D3DFEHandleWM_PAINT(hWnd);
								}
								return (DefWindowProc(hWnd, message, wParam, lParam));
								break;

	case	WM_DISPLAYCHANGE :	/* If in window mode, take note of the new GDI resolution */
								DEBUG("WM_DISPLAYCHANGE\n");
								return (DefWindowProc(hWnd, message, wParam, lParam));

								break;

	case	WM_DESTROY :		/* Handle window destroying */
								/* If we're not autokilling, exit normally */
								if (D3DFE.bAutoKillInProgress == FALSE)
								{
									PostQuitMessage(0);
								}
								else
								{
									/* Autokilling so exit with code 555 to let ssaver know to run next saver. */
									PostQuitMessage(555);
								}
								break;

	case	WM_QUIT :			/* Quit program */
								DEBUG("D3DShell : Exiting...\n");
								break;

	/* Handle FrontEnd menu messages */
	case	WM_COMMAND : 
		
		/* Get menu message ID */
		MenuMessage=GET_WM_COMMAND_ID(wParam, lParam);
		
		switch(MenuMessage)
		{
#ifdef D3DSHELL_USE_3D_MENUS
			/* Handle FILE menu message */
			case	ID_FILE_HIDEMENU :			/* Hide/Show 3D menus (F9) */
												if (D3DFE.bUse3DMenus)
												{
													/* Change menu state */
													SWAP(bM_Menu_On);

													/* Display or hide the mouse in fullscreen mode ONLY */
													if (D3DFE.bFullScreen) D3DFEMouseVisible(bM_Menu_On);

													/* Reset 3D menus */
													D3DFEReset3DMenus();
												}
												break;
#endif

			case	ID_FILE_SCREENCAPTURE :		/* Handle screen capture */
												DEBUG("D3DShell : Screen capture occuring\n"); 
												D3DFEScreenCapture(D3DFE.pszCaptureDirectory);
												break;
		
			case	ID_FILE_DISPLAYINFO	:		/* Get render surface dimensions */
												if (D3DFE.bFullScreen)
												{
													dwW =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
													dwH =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height;
													dwBPP =	D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format);
												}
												else
												{
													dwW =	D3DFE.dwWindowWidth;
													dwH =	D3DFE.dwWindowHeight;
													dwBPP =	D3DFE.dwGDIBPP;
												}
												
												/* Create display string */
												sprintf(pszString, "%ux%ux%u  %s  %s   %.1f fps\n", dwW, dwH, dwBPP, D3DFE.bZBufferNeeded ? "Z" : " ",
														(!D3DFE.bFullScreen) ? "" : D3DFE.nNumBackBuffer==1 ? "DB" : "TB", D3DFE.fFrameRate);
												
												/* Display string */
												DEBUG(pszString);
												break;
			
			case	ID_FILE_EXIT : 				D3DFE.bRenderingReady=FALSE;
												D3DFEFinish();
												break;

			/* Handle adapters menu messages */
			case	ID_DEVICES_ADAPTER0	:	
			case	ID_DEVICES_ADAPTER1	:	
			case	ID_DEVICES_ADAPTER2	:	
			case	ID_DEVICES_ADAPTER3	:	
			case	ID_DEVICES_ADAPTER4	:	nNumber=MenuMessage - ID_DEVICES_ADAPTER0;
											if (nNumber!=D3DFE.nCurrentAdapter)
											{
												/* Check rendering variables */
												if (D3DFECheckRenderingVariable(ADAPTER, &nNumber))
												{
													D3DFEReleaseAll();
													if (!D3DFEInitialiseDirect3D(nNumber, 
																				 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																				 D3DFE.MultiSamplingType,
																				 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																				 D3DFE.CurrentDeviceType, D3DFE.nVPType,
																				 D3DFE.CurrentZBufferFormat, D3DFE.CurrentBackBufferFormat,
																				 D3DFE.dwCurrentRefreshRate))
													{
														D3DFEFinish();
													}
												}
											}
											break;

			/* Handle 3D Devices menu messages */
			case	ID_DEVICES_D3DDEVICE_HAL :	
			case	ID_DEVICES_D3DDEVICE_HALPURE :
			case	ID_DEVICES_D3DDEVICE_SW :	
			case	ID_DEVICES_D3DDEVICE_REF :	/* Get device type corresponding to menu choice */
												switch (MenuMessage)
												{
													case ID_DEVICES_D3DDEVICE_HAL:			NewDevType = D3DDEVTYPE_HAL;	bUsePureDevice=FALSE;	break;
													case ID_DEVICES_D3DDEVICE_HALPURE:		NewDevType = D3DDEVTYPE_HAL;	bUsePureDevice=TRUE;	break;
													case ID_DEVICES_D3DDEVICE_SW:			NewDevType = D3DDEVTYPE_SW;		bUsePureDevice=FALSE;	break;
													case ID_DEVICES_D3DDEVICE_REF:			NewDevType = D3DDEVTYPE_REF;	bUsePureDevice=FALSE;	break;
												}
				
												/* Only change 3D device if a new choice is being made */
												if (NewDevType!=D3DFE.CurrentDeviceType || bUsePureDevice!=D3DFE.bUsePureDevice)
												{
													/* Set new state */
													D3DFE.bUsePureDevice = bUsePureDevice;

													/* Check rendering variables */
													if (D3DFECheckRenderingVariable(D3DDEVICE, &NewDevType))
													{
														D3DFEReleaseAll();
														if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																					 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																					 D3DFE.MultiSamplingType,
																					 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																					 NewDevType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat, 
																					 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
														{
															D3DFEFinish();
														}
													}
												}
												break;

			case	ID_DEVICES_SWVERTEXPROCESSING :	
			case	ID_DEVICES_HWVERTEXPROCESSING :	
			case	ID_DEVICES_MIXEDVERTEXPROCESSING :	/* Get vertex type corresponding to menu choice */
														switch (MenuMessage)
														{
															case ID_DEVICES_HWVERTEXPROCESSING:		nVPType = HW;		break;
															case ID_DEVICES_SWVERTEXPROCESSING:		nVPType = SW;		break;
															case ID_DEVICES_MIXEDVERTEXPROCESSING:	nVPType = MIXED;	break;
														}

														/* Only change vertex type if a new choice is being made */
														if (nVPType!=D3DFE.nVPType)
														{
															/* Check rendering variables */
															if (D3DFECheckRenderingVariable(VERTEXPROCESSING, &nVPType))
															{
																D3DFEReleaseAll();
																if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																							 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																							 D3DFE.MultiSamplingType,
																							 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																							 D3DFE.CurrentDeviceType, nVPType, D3DFE.CurrentZBufferFormat, 
																							 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
																{
																	D3DFEFinish();
																}
															}
														}
														break;

			/* Handle VSync menu message */
			case	ID_BUFFERING_VSYNC :	/* Change VSync state */
											if (D3DFE.bVSyncOn)
											{
												D3DFE.bVSyncOn=FALSE;		
											}
											else 
											{
												D3DFE.bVSyncOn=TRUE;
											}

											/* Reinitialise D3D with VSync ON */
											D3DFEReleaseAll();
											if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																		 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																		 D3DFE.MultiSamplingType,
																		 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																		 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
																		 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
											{
												D3DFEFinish();
											}
											break;

			/* Handle MultiSampling menu message */
			case	ID_BUFFERING_MS_NONE :
			case	ID_BUFFERING_MS_2_SAMPLES :
			case	ID_BUFFERING_MS_3_SAMPLES :
			case	ID_BUFFERING_MS_4_SAMPLES :
			case	ID_BUFFERING_MS_5_SAMPLES :
			case	ID_BUFFERING_MS_6_SAMPLES :
			case	ID_BUFFERING_MS_7_SAMPLES :
			case	ID_BUFFERING_MS_8_SAMPLES :
			case	ID_BUFFERING_MS_9_SAMPLES :
			case	ID_BUFFERING_MS_10_SAMPLES :
			case	ID_BUFFERING_MS_11_SAMPLES :
			case	ID_BUFFERING_MS_12_SAMPLES :
			case	ID_BUFFERING_MS_13_SAMPLES :
			case	ID_BUFFERING_MS_14_SAMPLES :
			case	ID_BUFFERING_MS_15_SAMPLES :
			case	ID_BUFFERING_MS_16_SAMPLES :	/* Get MultiSample choice corresponding to menu choice */
													NewMultiSampleType = (D3DMULTISAMPLE_TYPE)(MenuMessage - ID_BUFFERING_MS_NONE);
				
													/* Check rendering variables */
													if (D3DFECheckRenderingVariable(MULTISAMPLINGTYPE, &NewMultiSampleType))
													{
														D3DFEReleaseAll();
														if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																					 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																					 NewMultiSampleType,
																					 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																					 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
																					 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
														{
															D3DFEFinish();
														}
													}
													break;
											
			/* Handle Z-Buffer menu message */
			case	ID_BUFFERING_ZBUFFER :	/* Only handle depth buffer choice if not forced in preferences */
											if (!(D3DFE.dwUserPreferences & (FORCE_ZBUFFER_ON | FORCE_ZBUFFER_OFF)))
											{
												if (D3DFE.bZBufferNeeded)
												{
													bLocalZBufferNeeded = FALSE;
													DepthBufferFormat = (D3DFORMAT)0;
												}
												else
												{
													bLocalZBufferNeeded = TRUE;
													DepthBufferFormat = D3DFE.CurrentZBufferFormat;
												}
												
												/* Check rendering variables */
												if (D3DFECheckRenderingVariable(DEPTHBUFFER, &DepthBufferFormat))
												{
													D3DFEReleaseAll();
													if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																				 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																				 D3DFE.MultiSamplingType,
																				 bLocalZBufferNeeded, D3DFE.nNumBackBuffer,
																				 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
																				 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
													{
														D3DFEFinish();
													}
												}
											}
											break;

			/* Handle type of Z-Buffer menu message */
			case	ID_BUFFERING_ZBUFFER_D16L:
			case	ID_BUFFERING_ZBUFFER_D32:
			case	ID_BUFFERING_ZBUFFER_D15S1:
			case	ID_BUFFERING_ZBUFFER_D24S8:
			case	ID_BUFFERING_ZBUFFER_D16:	
			case	ID_BUFFERING_ZBUFFER_D24X8:
			case	ID_BUFFERING_ZBUFFER_D24X4S4:	/* Get depth buffer format corresponding to menu choice */
													switch (MenuMessage)
													{
														case ID_BUFFERING_ZBUFFER_D16L:		DepthBufferFormat = D3DFMT_D16_LOCKABLE;	break;
														case ID_BUFFERING_ZBUFFER_D32:		DepthBufferFormat = D3DFMT_D32;				break;
														case ID_BUFFERING_ZBUFFER_D15S1:	DepthBufferFormat = D3DFMT_D15S1;			break;
														case ID_BUFFERING_ZBUFFER_D24S8:	DepthBufferFormat = D3DFMT_D24S8;			break;
														case ID_BUFFERING_ZBUFFER_D16:		DepthBufferFormat = D3DFMT_D16;				break;
														case ID_BUFFERING_ZBUFFER_D24X8:	DepthBufferFormat = D3DFMT_D24X8;			break;
														case ID_BUFFERING_ZBUFFER_D24X4S4:	DepthBufferFormat = D3DFMT_D24X4S4;			break;
													}

													/* Only process new choice if different from current one */
													if (DepthBufferFormat!=D3DFE.CurrentZBufferFormat)
													{
														/* Check rendering variables */
														if (D3DFECheckRenderingVariable(DEPTHBUFFER, &DepthBufferFormat))
														{
															D3DFEReleaseAll();
															if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																						 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																						 D3DFE.MultiSamplingType,
																						 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																						 D3DFE.CurrentDeviceType, D3DFE.nVPType, DepthBufferFormat,
																						 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
															{
																D3DFEFinish();
															}
														}
													}
													break;

			/* Handle Buffering menu message */
			case	ID_BUFFERING_DOUBLEBUFFER :	
			case	ID_BUFFERING_TRIPLEBUFFER :	/* Get new number of back buffer required */			
												nNumber=(MenuMessage - ID_BUFFERING_DOUBLEBUFFER) + 1;
												
												/* Only process new choice if different from current one */
												if (nNumber!=D3DFE.nNumBackBuffer)
												{
													/* Check rendering variables */
													if (D3DFECheckRenderingVariable(NUMBACKBUFFER, &nNumber))
													{
														D3DFEReleaseAll();
														if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																					 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																					 D3DFE.MultiSamplingType,
																					 D3DFE.bZBufferNeeded, nNumber,
																					 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
																					 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
														{
															D3DFEFinish();
														}
													}
												}
												break;

			case	ID_BUFFERING_CLEAR :		/* Only process this menu choice if clear are not forced */
												if ( !(D3DFE.dwUserPreferences & (FORCE_CLEAR_ON | FORCE_CLEAR_OFF)) )
												{
													/* Swap viewport status */
													SWAP(D3DFE.bClearViewport);
													
													/* If we're not using 3D menus */
													if (!D3DFE.bUse3DMenus)
													{
														/* Don't need to recreate variables for this choice */
														D3DFEUpdateMenus();
													}
												}
												break;

			/* Handle MODES menu messages */
			case	ID_MODES_FULLSCREEN	:	if (!(D3DFE.dwUserPreferences & FORCE_FULLSCREEN))
											{
												/* Swap fullscreen status */
												if (D3DFE.bFullScreen==TRUE) 
												{
													bLocalFullScreen=FALSE;
												}
												else 
												{
													bLocalFullScreen=TRUE;
												}

												/* Check rendering variables */
												if (D3DFECheckRenderingVariable(FULLSCREEN, &bLocalFullScreen)) 
												{
													D3DFEReleaseAll();
													if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																				 bLocalFullScreen, D3DFE.nCurrentResolution,
																				 D3DFE.MultiSamplingType,
																				 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																				 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
																				 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
													{
														D3DFEFinish();
													}
												}
											}
											break;

			/* Handle back buffer format menu messages */
			case	ID_MODES_FORMAT_X8R8G8B8:
			case	ID_MODES_FORMAT_A8R8G8B8:
			case	ID_MODES_FORMAT_A2R10G10B10:
			case	ID_MODES_FORMAT_X1R5G5B5:
			case	ID_MODES_FORMAT_A1R5G5B5:
			case	ID_MODES_FORMAT_R5G6B5:		switch(MenuMessage)
												{
													case ID_MODES_FORMAT_X8R8G8B8:		BackBufferFormat =	D3DFMT_X8R8G8B8;	break;
													case ID_MODES_FORMAT_A8R8G8B8:		BackBufferFormat =	D3DFMT_A8R8G8B8;	break;
													case ID_MODES_FORMAT_A2R10G10B10:	BackBufferFormat =	D3DFMT_A2R10G10B10; break;
													case ID_MODES_FORMAT_X1R5G5B5:		BackBufferFormat =	D3DFMT_X1R5G5B5;	break;
													case ID_MODES_FORMAT_A1R5G5B5:		BackBufferFormat =	D3DFMT_A1R5G5B5;	break;
													case ID_MODES_FORMAT_R5G6B5:		BackBufferFormat =	D3DFMT_R5G6B5;		break;
												}

												/* Only process new choice if different from current one */
												if (BackBufferFormat!=D3DFE.CurrentBackBufferFormat)
												{
													/* Check rendering variables */
													if (D3DFECheckRenderingVariable(BACKBUFFERFORMAT, &BackBufferFormat))
													{
														D3DFEReleaseAll();
														if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																					 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																					 D3DFE.MultiSamplingType,
																					 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																					 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
																					 BackBufferFormat, D3DFE.dwCurrentRefreshRate))
														{
															D3DFEFinish();
														}
													}
												}
												break;

			/* Handle refresh rates menu messages */
			case	ID_MODES_FREQUENCY_60:
			case	ID_MODES_FREQUENCY_70:
			case	ID_MODES_FREQUENCY_72:
			case	ID_MODES_FREQUENCY_75:
			case	ID_MODES_FREQUENCY_85:
			case	ID_MODES_FREQUENCY_90:
			case	ID_MODES_FREQUENCY_100:
			case	ID_MODES_FREQUENCY_120:
			case	ID_MODES_FREQUENCY_150:		switch(MenuMessage)
												{
													case ID_MODES_FREQUENCY_60:		dwRefreshRate = 60; break;
													case ID_MODES_FREQUENCY_70:		dwRefreshRate = 70; break;
													case ID_MODES_FREQUENCY_72:		dwRefreshRate = 72; break;
													case ID_MODES_FREQUENCY_75:		dwRefreshRate = 75; break;
													case ID_MODES_FREQUENCY_85:		dwRefreshRate = 85; break;
													case ID_MODES_FREQUENCY_90:		dwRefreshRate = 90; break;
													case ID_MODES_FREQUENCY_100:	dwRefreshRate = 100; break;
													case ID_MODES_FREQUENCY_120:	dwRefreshRate = 120; break;
													case ID_MODES_FREQUENCY_150:	dwRefreshRate = 150; break;
												}

												/* Only process new choice if different from current one */
												if (dwRefreshRate!=D3DFE.dwCurrentRefreshRate && D3DFE.bUseRefreshRates)
												{
													/* Check rendering variables */
													if (D3DFECheckRenderingVariable(REFRESHRATE, &dwRefreshRate))
													{
														D3DFEReleaseAll();
														if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																					 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
																					 D3DFE.MultiSamplingType,
																					 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																					 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
																					 D3DFE.CurrentBackBufferFormat, dwRefreshRate))
														{
															D3DFEFinish();
														}
													}
												}
												break;

			/* Handle resolutions menu message */
			case	ID_MODES_RES0 :			
			case	ID_MODES_RES1 :			
			case	ID_MODES_RES2 :			
			case	ID_MODES_RES3 :			
			case	ID_MODES_RES4 :			
			case	ID_MODES_RES5 :			
			case	ID_MODES_RES6 :			
			case	ID_MODES_RES7 :			
			case	ID_MODES_RES8 :			
			case	ID_MODES_RES9 :			
			case	ID_MODES_RES10 :			
			case	ID_MODES_RES11 :			
			case	ID_MODES_RES12 :			
			case	ID_MODES_RES13 :			
			case	ID_MODES_RES14 :			
			case	ID_MODES_RES15 :			
			case	ID_MODES_RES16 :			
			case	ID_MODES_RES17 :			
			case	ID_MODES_RES18 :			
			case	ID_MODES_RES19 :			
			case	ID_MODES_RES20 :			
			case	ID_MODES_RES21 :			
			case	ID_MODES_RES22 :			
			case	ID_MODES_RES23 :			
			case	ID_MODES_RES24 :			
			case	ID_MODES_RES25 :			
			case	ID_MODES_RES26 :			
			case	ID_MODES_RES27 :			
			case	ID_MODES_RES28 :			
			case	ID_MODES_RES29 :			
			case	ID_MODES_RES30 :			
			case	ID_MODES_RES31 :			
			case	ID_MODES_RES32 :			
			case	ID_MODES_RES33 :			
			case	ID_MODES_RES34 :			
			case	ID_MODES_RES35 :			
			case	ID_MODES_RES36 :			
			case	ID_MODES_RES37 :			
			case	ID_MODES_RES38 :			
			case	ID_MODES_RES39 :		
			case	ID_MODES_RES40 :		
			case	ID_MODES_RES41 :		
			case	ID_MODES_RES42 :		
			case	ID_MODES_RES43 :		
			case	ID_MODES_RES44 :		
			case	ID_MODES_RES45 :		
			case	ID_MODES_RES46 :		
			case	ID_MODES_RES47 :		
			case	ID_MODES_RES48 :		
			case	ID_MODES_RES49 :		
			case	ID_MODES_RES50 :		
			case	ID_MODES_RES51 :		
			case	ID_MODES_RES52 :		
			case	ID_MODES_RES53 :		
			case	ID_MODES_RES54 :		
			case	ID_MODES_RES55 :		
			case	ID_MODES_RES56 :		
			case	ID_MODES_RES57 :		
			case	ID_MODES_RES58 :		
			case	ID_MODES_RES59 :		/* Get resolution number */
											nNumber=MenuMessage - ID_MODES_RES0;

											/* Check rendering variables */
											if (D3DFECheckRenderingVariable(RESOLUTION, &nNumber))
											{
												D3DFEReleaseAll();
												if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter, 
																			 D3DFE.bFullScreen, nNumber,
																			 D3DFE.MultiSamplingType,
																			 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer,
																			 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
																			 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
												{
													D3DFEFinish();
												}
											}
											break;
						
			default : return DefWindowProc(hWnd, message, wParam, lParam);
		 }
		 break;

    default:	return DefWindowProc(hWnd, message, wParam, lParam);
    }

    /* Message handled OK */
	return 0;
}


/*******************************************************************************
 * Function Name  : D3DFEHandleWM_PAINT
 * Global Used    : D3DFE
 * Description    : Paint the window when paused
 *******************************************************************************/
static void D3DFEHandleWM_PAINT(HWND hWindow)
{
	HRESULT		hres;
    COLORREF	crOldTextColor;
	HBRUSH		hOldBrush;
	HPEN		hOldPen;
    HDC			hdc;
	INT			oldMode, x, y, nStrLen;
	RECT		rect;
	SIZE		size;

	/* If rendering is ready just present scene onto front buffer */
	if (D3DFE.bRenderingReady && !D3DFE.bFullScreen)
	{
		/* Present scene only required once in window mode */
		hres=IDirect3DDevice9_Present(D3DFE.lpDev9, NULL, NULL, hWindow, NULL);
		
		/* Return from function if Present() call worked */
		if (hres==D3D_OK)
		{
			D3DFE.dwNumberOfPresent++;
			return;
		}
	}
	
	/* Rendering is not ready, display the paused string */

	/* Get DC */
	hdc = GetDC(hWindow);

	/* Black background */
	hOldPen =	(HPEN)SelectObject(hdc, GetStockObject (NULL_PEN));
	hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject (BLACK_BRUSH));

	/* White text */
	oldMode =		 SetBkMode(hdc, TRANSPARENT);
	crOldTextColor = SetTextColor(hdc, RGB(255, 255, 255));

	/* Get client area */
	GetClientRect(hWindow, &rect);

	/* Clear the client area */
	Rectangle(hdc, rect.left, rect.top, rect.right + 1, rect.bottom + 1);

	/* Draw the string centered in the client area */
	nStrLen = strlen(D3DFE.pszPausedMessage);
	GetTextExtentPoint32(hdc, D3DFE.pszPausedMessage, nStrLen, &size);
	x = (rect.right  - size.cx) / 2;
	y = (rect.bottom - size.cy) / 2;
	TextOut(hdc, x, y, D3DFE.pszPausedMessage, nStrLen);

	/* Cleanup */
	SetTextColor(hdc, crOldTextColor);
	SetBkMode(hdc, oldMode);
	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);

	/* Release DC */
	ReleaseDC(hWindow, hdc);
}


/*******************************************************************************
 * Function Name  : D3DFEHandleWM_SIZE
 * Global Used    : D3DFE
 * Description    : Handle the WM_SIZE message from the window loop.
 *					Deal with minimising and unminimising initialisations in both 
 *					window and FullScreen mode, and resizing in window mode.
 *******************************************************************************/
static BOOL D3DFEHandleWM_SIZE(LRESULT *lresult, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT	Rect;

	/* If we have minimized, take note and call the default window proc */
    if (wParam==SIZE_MINIMIZED) 
	{
        D3DFE.bMinimized=TRUE;
		D3DFE.bRenderingReady=FALSE;
        *lresult=DefWindowProc(hWnd, message, wParam, lParam);
        return TRUE;
    }

    /* If we are minimized, this is the un-minimized size message */
    if (D3DFE.bMinimized)									 
	{
		/* Return default message */
        *lresult=DefWindowProc(hWnd, message, wParam, lParam);

		/* Window has been restored */
		D3DFE.bMinimized=FALSE;
		D3DFE.bHasFocus=TRUE;

		/* Recreate everything on restore except in window 16+ bpp modes */
		if (D3DFE.bDisplayHasChanged || D3DFE.bFullScreen || (!D3DFE.bFullScreen && D3DFE.dwGDIBPP==8))
		{
			/* Release all Direct3D variables */
			D3DFEReleaseAll();

			/* Recreate Direct3D */
			if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter,
										 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
										 D3DFE.MultiSamplingType,
										 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer, 
										 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
										 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
			{
				DEBUG("D3DShell *ERROR* : D3DFEInitialiseDirect3D() failed in D3DFEHandleWM_SIZE().\n");
				MessageBox(D3DFE.hwnd, "Failed to recreate Direct3D variables\n", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
				D3DFEFinish();
			}
		}
	
		/* No problem occured */
		return TRUE;
    }

    /* At this point we should only be handling a simple window resizing message */

    /* We should never be FullScreen at this point but let's be sure */
	if (!D3DFE.bFullScreen) 
	{
		/* Retrieve client area dimensions */
		D3DFE.dwWindowWidth =  LOWORD(lParam);
		D3DFE.dwWindowHeight = HIWORD(lParam);

		/* Retrieve frame window coordinates and dimensions */
		GetWindowRect(hWnd, &Rect);
		D3DFE.dwWindowFramePositionX = Rect.left;
		D3DFE.dwWindowFramePositionY = Rect.top;
		D3DFE.dwWindowFrameWidth =     Rect.right-Rect.left;
		D3DFE.dwWindowFrameHeight =	   Rect.bottom-Rect.top;
		
		/* Test if the menu takes more than one line height */
		if ((D3DFE.dwWindowHeight+D3DFE.dwWindowSpaceY)<D3DFE.dwWindowFrameHeight)
		{
			D3DFE.bMenuTakesMoreThanOneLine=TRUE;
		}
		else
		{
			D3DFE.bMenuTakesMoreThanOneLine=FALSE;
		}

		/* We are about to change all variables so set rendering state to FALSE */
		D3DFE.bRenderingReady=FALSE; 

		/* Release all DirectDraw objects */
		D3DFEReleaseAll();

		/* Recreate Direct3D */
		if (!D3DFEInitialiseDirect3D(D3DFE.nCurrentAdapter,
									 D3DFE.bFullScreen, D3DFE.nCurrentResolution,
									 D3DFE.MultiSamplingType,
									 D3DFE.bZBufferNeeded, D3DFE.nNumBackBuffer, 
									 D3DFE.CurrentDeviceType, D3DFE.nVPType, D3DFE.CurrentZBufferFormat,
									 D3DFE.CurrentBackBufferFormat, D3DFE.dwCurrentRefreshRate))
		{
			DEBUG("D3DShell *ERROR* : D3DFEInitialiseDirect3D() failed in D3DFEHandleWM_SIZE().\n");
			MessageBox(D3DFE.hwnd, "Failed to recreate Direct3D variables\n", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
			D3DFEFinish();
		}
	}

	/* No problem encountered */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFEHandleWM_SIZING
 * Global Used    : D3DFE
 * Description    : Handle the WM_SIZING message from the window loop.
 *					Prevent the user from selecting a window size too large
 *******************************************************************************/
static void D3DFEHandleWM_SIZING(LRESULT *lresult, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD	dwClientAreaResizedWidth, dwClientAreaResizedHeight;

	/* Keep trace of the current size of the window to prevent user from 
	   creating a window too large */
	
	/* Get width and height of current window size */
	dwClientAreaResizedWidth = ((LPRECT)lParam)->right-((LPRECT)lParam)->left - D3DFE.dwWindowSpaceX + 1;
	dwClientAreaResizedHeight =((LPRECT)lParam)->bottom-((LPRECT)lParam)->top - D3DFE.dwWindowSpaceY + 1;

	/* Get Window position */
	D3DFE.dwWindowPositionX = ((LPRECT)lParam)->left;
	D3DFE.dwWindowPositionY = ((LPRECT)lParam)->top;
	
	/* First check minimums and maximums */
	if (dwClientAreaResizedWidth<D3DFE.dwMinWindowWidth)
	{
		dwClientAreaResizedWidth=D3DFE.dwMinWindowWidth;
	}
	if (dwClientAreaResizedWidth>D3DFE.dwMaxWindowWidth) 
	{
		dwClientAreaResizedWidth=D3DFE.dwMaxWindowWidth;
	}
	if (dwClientAreaResizedHeight<D3DFE.dwMinWindowHeight) 
	{
		dwClientAreaResizedHeight=D3DFE.dwMinWindowHeight;
	}
	if (dwClientAreaResizedHeight>D3DFE.dwMaxWindowHeight) 
	{
		dwClientAreaResizedHeight=D3DFE.dwMaxWindowHeight;
	}
	
	/* Send processed Window rectangle information */
	((LPRECT)lParam)->left =   D3DFE.dwWindowPositionX;
	((LPRECT)lParam)->top =	   D3DFE.dwWindowPositionY;
	((LPRECT)lParam)->right =  D3DFE.dwWindowPositionX+dwClientAreaResizedWidth-1+D3DFE.dwWindowSpaceX;
	((LPRECT)lParam)->bottom = D3DFE.dwWindowPositionY+dwClientAreaResizedHeight-1+D3DFE.dwWindowSpaceY;
}



/*******************************************************************************
 * Function Name  : D3DFEBuildMenus
 * Returns		  : TRUE if no problem occurs.
 * Global Used    : D3DFE
 * Description    : Build all menus to be used in D3DShell.
 *******************************************************************************/
static BOOL D3DFEBuildMenus()
{
	HRESULT	hres;
	HMENU	hUserFileMenu, hUserFileSubMenu;
	int		i, j, nResolution;
	DWORD	dwQualityLevels;	//tmp
	DWORD	dwBPP;
	char	pszString[128];


	/*********************
	** Create main menu **
	*********************/
	D3DFE.hMenu=CreateMenu();
	if (!D3DFE.hMenu)
	{
		DEBUG("D3DShell *WARNING* : Failed to create main menu in BuildMenus().\n");
	}


	/*********************
	** Create File menu **
	*********************/
	D3DFE.hFileMenu=CreatePopupMenu();
	if (!D3DFE.hFileMenu)
	{
		DEBUG("D3DShell *WARNING* : Failed to create System menu in BuildMenus().\n");
	}

	/* Attach File menu to main menu */
	AppendMenu(D3DFE.hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT)D3DFE.hFileMenu, "&File");

	/* If user menu contains a File menu, then append it to the existing File menu */
	if (D3DFE.hUserMenu)
	{
		/* Parse through all menu item strings */
		for (i=0; i<GetMenuItemCount(D3DFE.hUserMenu); i++)
		{
			/* Get string for this menu item number i */
			GetMenuString(D3DFE.hUserMenu, i, pszString, 100, MF_BYPOSITION);
			
			/* Is this menu item called File or &File ? */
			if ( (lstrcmpi(pszString, "File")==0) ||
				 (lstrcmpi(pszString, "&File")==0))
			{
				/* Get user File menu handle */
				hUserFileMenu=GetSubMenu(D3DFE.hUserMenu, i);
				
				/* Parse through all menu item in user file menu */
				for (j=0; j<GetMenuItemCount(hUserFileMenu); j++)
				{
					/* Get string for this menu item number j */
					GetMenuString(hUserFileMenu, j, pszString, 100, MF_BYPOSITION);
					
					/* Check if there are other submenus */
					hUserFileSubMenu=GetSubMenu(hUserFileMenu, j);

					if (hUserFileSubMenu)
					{
						/* Append popup menu to FILE menu */
						AppendMenu(D3DFE.hFileMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT)hUserFileSubMenu, pszString);
					}
					else
					{
						/* Append menu item to FILE menu */
						(D3DFE.hFileMenu, GetMenuState(hUserFileMenu, j, MF_BYPOSITION),
													GetMenuItemID(hUserFileMenu, j), pszString);
					}
				}

				/* Finally append a separator */
				AppendMenu(D3DFE.hFileMenu, MF_SEPARATOR, 0, 0);
			}
		}
	}

	if (D3DFE.bUse3DMenus)
	{
		/* Add Show menu menu item */
		AppendMenu(D3DFE.hFileMenu, MF_STRING | MF_ENABLED, ID_FILE_HIDEMENU, "&Hide/Show menu\tF9");

		/* Then append a separator */
		AppendMenu(D3DFE.hFileMenu, MF_SEPARATOR, 0, 0);
	}

	/* Add Screen capture menu item */
	AppendMenu(D3DFE.hFileMenu, MF_STRING | MF_ENABLED, ID_FILE_SCREENCAPTURE, "&Screen capture\tF12");

	/* Then append a separator */
	AppendMenu(D3DFE.hFileMenu, MF_SEPARATOR, 0, 0);
	
	/* Add Exit menu item */
	AppendMenu(D3DFE.hFileMenu, MF_STRING | MF_ENABLED, ID_FILE_EXIT, "&Exit\tAlt+F4");


	/************************
	** Create Devices menu **
	************************/
	D3DFE.hDevicesMenu=CreatePopupMenu();
	if (!D3DFE.hDevicesMenu)
	{
		DEBUG("D3DShell *WARNING* : Failed to create Devices menu in D3DFEBuildMenus().\n");
	}

	/* Attach Devices menu to main menu */
	AppendMenu(D3DFE.hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT)D3DFE.hDevicesMenu, "&Devices");
	
	/* Insert all available adapters in menu */
	for (i=0; i<nNumAdapters; i++)
	{
		AppendMenu(D3DFE.hDevicesMenu, MF_ENABLED, ID_DEVICES_ADAPTER0+i, Adapter[i].Id.Description);
	}

	/* Then append a separator */
	AppendMenu(D3DFE.hDevicesMenu, MF_SEPARATOR, 0, 0);

	/* Create menu items corresponding to device available on the system */

	/* Get actual screen resolution we're presently in (independant of window or fullscreen mode) */
	if (!D3DFE.bFullScreen)	nResolution = D3DFEGetResolution(D3DFE.nCurrentAdapter, D3DFE.dwGDIWidth, D3DFE.dwGDIHeight, D3DFE.dwGDIBPP);
		else nResolution = D3DFE.nCurrentResolution;

	/* Check whether a HAL device is supported in that particular display mode */
	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DDEVTYPE_HAL, 
											   Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
											   Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
											   D3DFE.bFullScreen);
	if (hres==D3D_OK)
	{
		/* Add HAL to menu */
		AppendMenu(D3DFE.hDevicesMenu, MF_ENABLED, ID_DEVICES_D3DDEVICE_HAL, "HAL");

		/* HAL T&L and Pure device */
		if ((Adapter[D3DFE.nCurrentAdapter].HALCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) && (!(D3DFE.dwUserPreferences & DISABLE_PURE_DEVICE)))
		{
			/* Add HAL T&L (PURE) to menu */
			AppendMenu(D3DFE.hDevicesMenu, MF_ENABLED, ID_DEVICES_D3DDEVICE_HALPURE, "HAL (PURE)");
		}
	}

	/* Check whether a SW device is supported in that particular display mode */
	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DDEVTYPE_SW, 
											   Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
											   Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
											   D3DFE.bFullScreen);
	if (hres==D3D_OK)
	{
		/* Add SW to menu */
		AppendMenu(D3DFE.hDevicesMenu, MF_ENABLED, ID_DEVICES_D3DDEVICE_SW, "SW");
	}

	/* Check whether a REF device is supported in that particular display mode */
	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DDEVTYPE_REF, 
											   Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
											   Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
											   D3DFE.bFullScreen);
	if (hres==D3D_OK)
	{
		/* Add REF to menu */
		AppendMenu(D3DFE.hDevicesMenu, MF_ENABLED, ID_DEVICES_D3DDEVICE_REF, "REF");
	}

	/* Then append a separator */
	AppendMenu(D3DFE.hDevicesMenu, MF_SEPARATOR, 0, 0);

	/* Hardware or vertex processing */
	AppendMenu(D3DFE.hDevicesMenu, MF_ENABLED, ID_DEVICES_HWVERTEXPROCESSING, "HW Vertex Processing");
	AppendMenu(D3DFE.hDevicesMenu, MF_ENABLED, ID_DEVICES_SWVERTEXPROCESSING, "SW Vertex Processing");
	AppendMenu(D3DFE.hDevicesMenu, MF_ENABLED, ID_DEVICES_MIXEDVERTEXPROCESSING, "MIXED Vertex Processing");


	/**************************
	** Create Buffering menu **
	**************************/
	D3DFE.hBufferingMenu=CreatePopupMenu();
	if (!D3DFE.hBufferingMenu)
	{
		DEBUG("D3DShell *WARNING* : Failed to create Buffering menu in D3DFEBuildMenus().\n");
	}

	/* Attach Buffering menu to main menu */
	AppendMenu(D3DFE.hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT)D3DFE.hBufferingMenu, "&Buffering");
	
	/* Add VSync choice */
	AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_VSYNC, "&VSync\tF4");

	/* Multisampling */
	if (!(D3DFE.dwUserPreferences & FORCE_MULTISAMPLING_OFF))
	{
		/* Draw separator */
		AppendMenu(D3DFE.hBufferingMenu, MF_SEPARATOR, 0, 0);

		/* Add multisampling choice */
		AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_MS_NONE, "No MultiSample");
		for (i=2; i<=16; i++)
		{
			// tmp: quality level
			hres=IDirect3D9_CheckDeviceMultiSampleType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													   Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format, !D3DFE.bFullScreen,
													   (D3DMULTISAMPLE_TYPE)i, &dwQualityLevels);
			if (hres==D3D_OK) 
			{
				sprintf(pszString, "MultiSample: %d Samples", i);
				AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_MS_NONE + i, pszString);
			}
		}
	}


	/* Add ZBuffer choice */
	if (!(D3DFE.dwUserPreferences & (FORCE_ZBUFFER_ON | FORCE_ZBUFFER_OFF)))
	{
		/* No ZBuffer mode has been chosen by default -> display menu item */

		/* Draw separator */
		AppendMenu(D3DFE.hBufferingMenu, MF_SEPARATOR, 0, 0);

		/* Add the normal ZBuffer choice */
		AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_ZBUFFER, "&Z Buffer\tF5");

		/* Check whether the following formats are supported and display them if they are */
		hres=IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
													 D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16_LOCKABLE);
		if (hres==D3D_OK) AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_ZBUFFER_D16L, "Z-Buffer Z:16 (L)");

		hres=IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
													 D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16);
		if (hres==D3D_OK) AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_ZBUFFER_D16, "Z-Buffer Z:16");

		hres=IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
													 D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1);
		if (hres==D3D_OK) AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_ZBUFFER_D15S1, "Z-Buffer Z:15 S:1");

		hres=IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
													 D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8);
		if (hres==D3D_OK) AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_ZBUFFER_D24X8, "Z-Buffer Z:24");

		hres=IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
													 D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4);
		if (hres==D3D_OK) AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_ZBUFFER_D24X4S4, "Z-Buffer Z:24 S:4");

		hres=IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
													 D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8);
		if (hres==D3D_OK) AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_ZBUFFER_D24S8, "Z-Buffer Z:24 S:8");

		hres=IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
													 Adapter[D3DFE.nCurrentAdapter].Mode[nResolution].Format,
													 D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32);
		if (hres==D3D_OK) AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_ZBUFFER_D32, "Z-Buffer Z:32");
	}

	/* Add buffering modes */
	if (!(D3DFE.dwUserPreferences & (FORCE_DOUBLE_BUFFER | FORCE_TRIPLE_BUFFER)))
	{
		/* Draw separator */	
		AppendMenu(D3DFE.hBufferingMenu, MF_SEPARATOR, 0, 0);

		/* Always display Double Buffer menu item (no option to disable it) */
		AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_DOUBLEBUFFER, "&Double Buffer");
		/* Display Triple Buffer menu item if not disabled */
		if (!(D3DFE.dwUserPreferences & DISABLE_TRIPLE_BUFFER))
		{
			AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_TRIPLEBUFFER, "&Triple Buffer");
		}
	}

	/* Clear choice */
	if ( !(D3DFE.dwUserPreferences & (FORCE_CLEAR_ON | FORCE_CLEAR_OFF)) )
	{
		/* Draw separator */	
		AppendMenu(D3DFE.hBufferingMenu, MF_SEPARATOR, 0, 0);

		/* Add Clear menu choice */
		AppendMenu(D3DFE.hBufferingMenu, MF_ENABLED, ID_BUFFERING_CLEAR, "&Clear\tF6");
	}
	
	/* If buffering modes are already forced On or Off, no point using this menu */
	if ( (D3DFE.dwUserPreferences & (FORCE_DOUBLE_BUFFER | FORCE_TRIPLE_BUFFER)) &&
		 (D3DFE.dwUserPreferences & (FORCE_ZBUFFER_ON | FORCE_ZBUFFER_OFF)) &&
		 (D3DFE.dwUserPreferences & (FORCE_CLEAR_ON | FORCE_CLEAR_OFF)) )
	{
		DeleteMenu(D3DFE.hMenu, (UINT)D3DFE.hBufferingMenu, MF_BYCOMMAND);
		D3DFE.hBufferingMenu=NULL;
	}


	/**********************
	** Create Modes menu **
	**********************/
	D3DFE.hModesMenu=CreatePopupMenu();
	if (!D3DFE.hModesMenu)
	{
		DEBUG("D3DShell *WARNING* : Failed to create Modes menu in D3DFEBuildMenus().\n");
	}

	/* Attach Modes menu to main menu */
	AppendMenu(D3DFE.hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT)D3DFE.hModesMenu, "&Modes");

	/* Create FullScreen choice if permitted */
	if (!(D3DFE.dwUserPreferences & FORCE_FULLSCREEN))
	{
		/* Create FullScreen choice */
		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FULLSCREEN, "&Full Screen\tAlt+Enter");
	
		/* Draw separator */
		AppendMenu(D3DFE.hModesMenu, MF_SEPARATOR, 0, 0);
	}

	/* Create menu items corresponding to back buffer formats */
	
	/* Retrieve current display mode format */
	if (D3DFE.bFullScreen)	dwBPP = D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format);
		else dwBPP = D3DFE.dwGDIBPP;

	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
									dwBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
									D3DFMT_X8R8G8B8, !D3DFE.bFullScreen);
	if (hres==D3D_OK)	AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FORMAT_X8R8G8B8,		dwBPP==32 ? "X8R8G8B8\t32bits" : "(!) X8R8G8B8\t32bits");
	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
									dwBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
									D3DFMT_A8R8G8B8, !D3DFE.bFullScreen);
	if (hres==D3D_OK)	AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FORMAT_A8R8G8B8,		dwBPP==32 ? "A8R8G8B8\t32bits" : "(!) A8R8G8B8\t32bits");
	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
									dwBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
									D3DFMT_A2R10G10B10, !D3DFE.bFullScreen);
	if (hres==D3D_OK)	AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FORMAT_A2R10G10B10,	dwBPP==32 ? "A2R10G10B10\t32bits" : "(!) A2R10G10B10\t32bits");
	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
									dwBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
									D3DFMT_X1R5G5B5, !D3DFE.bFullScreen);
	if (hres==D3D_OK)	AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FORMAT_X1R5G5B5,		dwBPP==16 ? "X1R5G5B5\t16bits" : "(!) X1R5G5B5\t16bits");
	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
									dwBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
									D3DFMT_A1R5G5B5, !D3DFE.bFullScreen);
	if (hres==D3D_OK)	AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FORMAT_A1R5G5B5,		dwBPP==16 ? "A1R5G5B5\t16bits" : "(!) A1R5G5B5\t16bits");
	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, D3DFE.nCurrentAdapter, D3DFE.CurrentDeviceType, 
									dwBPP == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5,
									D3DFMT_R5G6B5, !D3DFE.bFullScreen);
	if (hres==D3D_OK)	AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FORMAT_R5G6B5,		dwBPP==16 ? "R5G6B5\t16bits" : "(!) R5G6B5\t16bits");

	/* Draw separator */
	AppendMenu(D3DFE.hModesMenu, MF_SEPARATOR, 0, 0);

	/* Create menu items corresponding to refresh rates formats (fullscreen mode only) */
	if (D3DFE.bFullScreen && D3DFE.bUseRefreshRates)
	{
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo60Hz) 		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_60, "60Hz");
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo70Hz) 		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_70, "70Hz");
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo72Hz) 		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_72, "72Hz");
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo75Hz) 		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_75, "75Hz");
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo85Hz) 		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_85, "85Hz");
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo90Hz) 		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_90, "90Hz");
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo100Hz)		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_100, "100Hz");
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo120Hz) 	AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_120, "120Hz");
		if (Adapter[D3DFE.nCurrentAdapter].bCanDo150Hz) 	AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_FREQUENCY_150, "150Hz");

		/* Draw separator */
		AppendMenu(D3DFE.hModesMenu, MF_SEPARATOR, 0, 0);
	}

	/* Create menu items corresponding to screen resolutions */
	for (i=0; i<Adapter[D3DFE.nCurrentAdapter].nNumberOfDisplayModes; i++)
	{
		/* Disable resolutions above 1024x768 if required */
		if ( (D3DFE.dwUserPreferences & DISABLE_VERY_HIGH_RES) && 
			 (Adapter[D3DFE.nCurrentAdapter].Mode[i].Width>1024) ) continue;

		/* Add new menu item corresponding to resolution */
		sprintf(pszString, "%ux%ux%u", 
			Adapter[D3DFE.nCurrentAdapter].Mode[i].Width,
			Adapter[D3DFE.nCurrentAdapter].Mode[i].Height,
			D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[i].Format));
		AppendMenu(D3DFE.hModesMenu, MF_ENABLED, ID_MODES_RES0+i, pszString);
	}


	/*********************
	** Create User menu **
	*********************/
	/* If user menu has been submitted, then append it to the D3D Shell menu */
	if (D3DFE.hUserMenu)
	{
		for (i=0; i<GetMenuItemCount(D3DFE.hUserMenu); i++)
		{
			GetMenuString(D3DFE.hUserMenu, i, pszString, 100, MF_BYPOSITION);
			if ( lstrcmpi(pszString, "File") && lstrcmpi(pszString, "&File") )
			{
				AppendMenu(D3DFE.hMenu, MF_STRING | MF_ENABLED | MF_POPUP, (UINT)GetSubMenu(D3DFE.hUserMenu, i), pszString);
			}
		}
	}

	/* If we're not using 3D menus */
	if (!D3DFE.bUse3DMenus)
	{
		/* Attach menu to window */
		D3DFE.bIgnoreWM_SIZE=TRUE;
		SetMenu(D3DFE.hwnd, D3DFE.hMenu);
		DrawMenuBar(D3DFE.hwnd);
		D3DFE.bIgnoreWM_SIZE=FALSE;
	}

	/* No problem occured */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFECreateAccelerators
 * Global Used    : D3DFE
 * Description    : Create menus and accelerators
 *******************************************************************************/
static void D3DFECreateAccelerators()
{
	int	nNumberOfAccel=0;
	int nNumberOfUserAccel=0;

	
	/* Create D3DShell accelator table */
		
	/* Assign ALT+RETURN for FullScreen */
	D3DFE.Accel[nNumberOfAccel].fVirt =	FALT | FVIRTKEY;
	D3DFE.Accel[nNumberOfAccel].key =	VK_RETURN;
	D3DFE.Accel[nNumberOfAccel].cmd =	ID_MODES_FULLSCREEN;

	/* Increase count */
	nNumberOfAccel++;

	/* Assign ESCAPE for quit */
	D3DFE.Accel[nNumberOfAccel].fVirt =	FVIRTKEY;
	D3DFE.Accel[nNumberOfAccel].key =	VK_ESCAPE;
	D3DFE.Accel[nNumberOfAccel].cmd =	ID_FILE_EXIT;

	/* Increase count */
	nNumberOfAccel++;

	/* Assign TAB for DisplayInfo */
	D3DFE.Accel[nNumberOfAccel].fVirt =	FVIRTKEY;
	D3DFE.Accel[nNumberOfAccel].key =	VK_TAB;
	D3DFE.Accel[nNumberOfAccel].cmd =	ID_FILE_DISPLAYINFO;

	/* Increase count */
	nNumberOfAccel++;

#ifdef D3DSHELL_USE_3D_MENUS
	/* Assign F9 for Show menu */
	D3DFE.Accel[nNumberOfAccel].fVirt =	FVIRTKEY;
	D3DFE.Accel[nNumberOfAccel].key =	VK_F9;
	D3DFE.Accel[nNumberOfAccel].cmd =	ID_FILE_HIDEMENU;

	/* Increase count */
	nNumberOfAccel++;
#endif

	/* Assign F4 for VSync */
	D3DFE.Accel[nNumberOfAccel].fVirt =	FVIRTKEY;
	D3DFE.Accel[nNumberOfAccel].key =	VK_F4;
	D3DFE.Accel[nNumberOfAccel].cmd =	ID_BUFFERING_VSYNC;

	/* Increase count */
	nNumberOfAccel++;

	/* Assign F5 for Z-Buffer */
	D3DFE.Accel[nNumberOfAccel].fVirt =	FVIRTKEY;
	D3DFE.Accel[nNumberOfAccel].key =	VK_F5;
	D3DFE.Accel[nNumberOfAccel].cmd =	ID_BUFFERING_ZBUFFER;

	/* Increase count */
	nNumberOfAccel++;

	/* Assign F6 for Viewport Clear */
	D3DFE.Accel[nNumberOfAccel].fVirt =	FVIRTKEY;
	D3DFE.Accel[nNumberOfAccel].key =	VK_F6;
	D3DFE.Accel[nNumberOfAccel].cmd =	ID_BUFFERING_CLEAR;

	/* Increase count */
	nNumberOfAccel++;

	/* Assign F12 for Screen Capture */
	D3DFE.Accel[nNumberOfAccel].fVirt =	FVIRTKEY;
	D3DFE.Accel[nNumberOfAccel].key =	VK_F12;
	D3DFE.Accel[nNumberOfAccel].cmd =	ID_FILE_SCREENCAPTURE;

	/* Increase count */
	nNumberOfAccel++;

	/* If a user accelerator table has been submitted */
	if (D3DFE.hAccel)
	{
		/* Retrieve number of accelerators from user table */
		nNumberOfUserAccel=CopyAcceleratorTable(D3DFE.hAccel, NULL, 0);

		/* Test if number of accelerators exceeds maximum */
		if (nNumberOfAccel+nNumberOfUserAccel>MAX_NUMBER_OF_ACCELERATORS)
		{
			DEBUG("D3DShell *WARNING* : Number of accelerators is too high : disabling user accelerators\n");
			nNumberOfUserAccel=0;
		}
		else
		{
			/* Copy user table into D3DFE.Accel */
			CopyAcceleratorTable(D3DFE.hAccel, &D3DFE.Accel[nNumberOfAccel], nNumberOfUserAccel);
		}

		/* Compute total number of accelerators */
		nNumberOfAccel+=nNumberOfUserAccel;
	}

	/* Create Accelerator Table */
	D3DFE.hAccel=CreateAcceleratorTable(D3DFE.Accel, nNumberOfAccel);
}


/*******************************************************************************
 * Function Name  : D3DFEUpdateMenus
 * Global Used    : D3DFE, nNumAdapters
 * Description    : Update check boxes on menu items.
 *					Check or uncheck items depending on current selection.
 *					Gray items if not supported.
 *******************************************************************************/
static void D3DFEUpdateMenus()
{
	int		i;

	/* Asserts */
	ASSERT(nNumAdapters>0);
	ASSERT(D3DFE.hDevicesMenu);
	ASSERT(D3DFE.hModesMenu);


	/*****************
	** Devices menu **
	*****************/
	/* Adapters */
	for (i=0; i<nNumAdapters; i++)
	{
		CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_ADAPTER0+i, MF_UNCHECKED);
	}
	CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_ADAPTER0+D3DFE.nCurrentAdapter, MF_CHECKED);

	/* 3D devices */
	CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_D3DDEVICE_HAL, 
		(D3DFE.CurrentDeviceType==D3DDEVTYPE_HAL && !D3DFE.bUsePureDevice) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_D3DDEVICE_HALPURE, 
		(D3DFE.CurrentDeviceType==D3DDEVTYPE_HAL && D3DFE.bUsePureDevice) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_D3DDEVICE_REF, D3DFE.CurrentDeviceType==D3DDEVTYPE_REF ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_D3DDEVICE_SW, D3DFE.CurrentDeviceType==D3DDEVTYPE_SW ? MF_CHECKED : MF_UNCHECKED);

	if (D3DFE.bUsePureDevice && D3DFE.CurrentDeviceType==D3DDEVTYPE_HAL && D3DFE.nVPType==HW)
	{
		EnableMenuItem(D3DFE.hBufferingMenu, ID_DEVICES_SWVERTEXPROCESSING, MF_GRAYED);
	}
	else
	{
		EnableMenuItem(D3DFE.hBufferingMenu, ID_DEVICES_SWVERTEXPROCESSING, MF_ENABLED);
	}

	/* Gray HW and MIXED vertex processing if 3D device doesn't support T&L */
	EnableMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_HWVERTEXPROCESSING, MF_ENABLED);
	EnableMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_MIXEDVERTEXPROCESSING, MF_ENABLED);
	if ( (D3DFE.CurrentDeviceType==D3DDEVTYPE_HAL) &&
		!(Adapter[D3DFE.nCurrentAdapter].HALCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
	{
		EnableMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_HWVERTEXPROCESSING, MF_GRAYED);
		EnableMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_MIXEDVERTEXPROCESSING, MF_GRAYED);
	}

	/* Tick current vertex processing mode */
	CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_HWVERTEXPROCESSING, D3DFE.nVPType==HW ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_SWVERTEXPROCESSING, D3DFE.nVPType==SW ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(D3DFE.hDevicesMenu, ID_DEVICES_MIXEDVERTEXPROCESSING, D3DFE.nVPType==MIXED ? MF_CHECKED : MF_UNCHECKED);


	/*******************
	** Buffering menu **
	*******************/
	if (D3DFE.hBufferingMenu)
	{
		/* VSync */
		if (D3DFE.bFullScreen)
		{
			EnableMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_VSYNC, MF_ENABLED);
			CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_VSYNC, D3DFE.bVSyncOn ? MF_CHECKED : MF_UNCHECKED);
		}
		else
		{
			/* Gray VSync option in window mode */
			CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_VSYNC, MF_UNCHECKED);
			EnableMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_VSYNC, MF_GRAYED);
		}

		/* Multisampling */
		CheckMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_MS_NONE, D3DFE.MultiSamplingType == D3DMULTISAMPLE_NONE ? MF_CHECKED : MF_UNCHECKED);
		for (i=2; i<=16; i++)
		{
			CheckMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_MS_NONE + i, i == (int)D3DFE.MultiSamplingType ? MF_CHECKED : MF_UNCHECKED);
		}

		/* Z-Buffer */
		EnableMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER, MF_ENABLED);
		CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER, D3DFE.bZBufferNeeded ? MF_CHECKED : MF_UNCHECKED);

		/* Is ZBuffer OFF ? */
		if (!D3DFE.bZBufferNeeded)
		{
			/* No ZBuffer is selected so gray all ZBuffer choices */
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D16L,		MF_UNCHECKED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D16L,		MF_GRAYED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D32,		MF_UNCHECKED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D32,		MF_GRAYED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D15S1,		MF_UNCHECKED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D15S1,		MF_GRAYED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D24S8,		MF_UNCHECKED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D24S8,		MF_GRAYED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D16,		MF_UNCHECKED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D16,		MF_GRAYED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D24X8,		MF_UNCHECKED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D24X8,		MF_GRAYED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D24X4S4,	MF_UNCHECKED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D24X4S4,	MF_GRAYED);
		}
		else
		{
			/* Be sure all ZBuffer choices are enabled and unchecked */
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D16L,		MF_ENABLED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D16L,		MF_UNCHECKED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D32,		MF_ENABLED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D32,		MF_UNCHECKED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D15S1,		MF_ENABLED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D15S1,		MF_UNCHECKED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D24S8,		MF_ENABLED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D24S8,		MF_UNCHECKED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D16,		MF_ENABLED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D16,		MF_UNCHECKED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D24X8,		MF_ENABLED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D24X8,		MF_UNCHECKED);
			CheckMenuItem(D3DFE.hBufferingMenu,		ID_BUFFERING_ZBUFFER_D24X4S4,	MF_ENABLED);
			EnableMenuItem(D3DFE.hBufferingMenu,	ID_BUFFERING_ZBUFFER_D24X4S4,	MF_UNCHECKED);
			
			/* Tick current ZBuffer format */
			switch (D3DFE.CurrentZBufferFormat)
			{
				case D3DFMT_D16_LOCKABLE:	CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER_D16L,		MF_CHECKED); break;
				case D3DFMT_D32:			CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER_D32,		MF_CHECKED); break;
				case D3DFMT_D15S1:			CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER_D15S1,		MF_CHECKED); break;
				case D3DFMT_D24S8:			CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER_D24S8,		MF_CHECKED); break;
				case D3DFMT_D16:			CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER_D16,		MF_CHECKED); break;
				case D3DFMT_D24X8:			CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER_D24X8,		MF_CHECKED); break;
				case D3DFMT_D24X4S4:		CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER_D24X4S4,	MF_CHECKED); break;
			}

			/* Gray lockable depth buffer format if multisampling is anything other than none */
			if (D3DFE.MultiSamplingType!=D3DMULTISAMPLE_NONE)
			{
				EnableMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_ZBUFFER_D16L, MF_GRAYED);
			}
		}

		/* Buffering modes */
		/* First uncheck them all */
		CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_DOUBLEBUFFER, MF_UNCHECKED);
		CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_TRIPLEBUFFER, MF_UNCHECKED);
		
		/* Buffering modes are only available in FullScreen */
		if (D3DFE.bFullScreen)
		{
			EnableMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_DOUBLEBUFFER, MF_ENABLED);
			EnableMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_TRIPLEBUFFER, MF_ENABLED);
			
			/* Tick current choice */
			switch (D3DFE.nNumBackBuffer)
			{
				case	1 :	CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_DOUBLEBUFFER, MF_CHECKED); break;
				case	2 :	CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_TRIPLEBUFFER, MF_CHECKED); break;
			}
		}
		else
		{
			/* Disable buffering menu in window mode */
			EnableMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_DOUBLEBUFFER, MF_GRAYED);
			EnableMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_TRIPLEBUFFER, MF_GRAYED);
		}
			
		/* Clear viewport mode */
		CheckMenuItem(D3DFE.hBufferingMenu, ID_BUFFERING_CLEAR, D3DFE.bClearViewport ? MF_CHECKED : MF_UNCHECKED);
	}


	/***************
	** Modes menu **
	***************/
	/* Uncheck all resolutions in Modes menu */
	for (i=0; i<Adapter[D3DFE.nCurrentAdapter].nNumberOfDisplayModes; i++)
	{
		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_RES0+i, MF_UNCHECKED);
	}

	/* FullScreen or not */
	if (D3DFE.bFullScreen)
	{
		/* Tick Fullscreen box */
		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FULLSCREEN, MF_CHECKED);
		
		/* Check current resolution */
		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_RES0 + D3DFE.nCurrentResolution, MF_CHECKED);
	}
	else
	{
		/* Not in FullScreen mode -> Uncheck item */
		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FULLSCREEN, MF_UNCHECKED);
	}

	/* Uncheck all formats in Modes menu */
	for (i=0; i<6; i++)
	{
		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FORMAT_X8R8G8B8+i,	MF_UNCHECKED);
	}
	/* Tick current back buffer format */
	switch(D3DFE.CurrentBackBufferFormat)
	{
		case D3DFMT_X8R8G8B8 :		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FORMAT_X8R8G8B8,		MF_CHECKED); break;
		case D3DFMT_A8R8G8B8 :		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FORMAT_A8R8G8B8,		MF_CHECKED); break;
		case D3DFMT_A2R10G10B10 :	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FORMAT_A2R10G10B10,	MF_CHECKED); break;
		case D3DFMT_X1R5G5B5 :		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FORMAT_X1R5G5B5,		MF_CHECKED); break;
		case D3DFMT_A1R5G5B5 :		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FORMAT_A1R5G5B5,		MF_CHECKED); break;
		case D3DFMT_R5G6B5 :		CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FORMAT_R5G6B5,			MF_CHECKED); break;
	}

	/* Only perform the following if refresh rates menu is used */
	if (D3DFE.bUseRefreshRates)
	{
		for (i=0; i<9; i++)
		{
			/* Uncheck all refresh rates in Modes menu */
			CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_60 + i, MF_UNCHECKED);
		}

		/* Refresh rates are only available in fullscreen mode */
		if (D3DFE.bFullScreen)
		{
			D3DFORMAT	DisplayModeFormat;
			DWORD		RRTable[9] = { 60, 70, 72, 75, 85, 90, 100, 120, 150 };

			/* Enable all refresh rates */
			for (i=0; i<9; i++) 
			{
				EnableMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_60+i, MF_ENABLED);
			}

			/* Get current display mode format */
			if (D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format)==32) 
			{
				DisplayModeFormat=D3DFMT_X8R8G8B8;
			}
			else 
			{
				DisplayModeFormat=D3DFMT_R5G6B5;
			}

			/* Gray refresh rates not supported at current resolution */
			for (i=0; i<9; i++)
			{
				if (D3DFEPickRefreshRate(D3DFE.nCurrentAdapter, Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width, 
									 Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height, DisplayModeFormat, RRTable[i])!=RRTable[i])
				{
					EnableMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_60+i, MF_GRAYED);
				}
			}

			/* Tick current refresh rate */
			switch(D3DFE.dwCurrentRefreshRate)
			{
				case 60:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_60,	MF_CHECKED); break;
				case 70:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_70,	MF_CHECKED); break;
				case 72:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_72,	MF_CHECKED); break;
				case 75:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_75,	MF_CHECKED); break;
				case 85:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_85,	MF_CHECKED); break;
				case 90:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_90,	MF_CHECKED); break;
				case 100:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_100, MF_CHECKED); break;
				case 120:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_120, MF_CHECKED); break;
				case 150:	CheckMenuItem(D3DFE.hModesMenu, ID_MODES_FREQUENCY_150, MF_CHECKED); break;
			}
		}
	}
}
	

/*******************************************************************************
 * Function Name  : D3DFEMouseVisible
 * Global Used    : None
 * Description    : Simple function which makes the mouse cursor visible or not
 *******************************************************************************/
static void D3DFEMouseVisible(BOOL bVisible)
{
	static BOOL	bMouseVisible=TRUE;

	if (bVisible)
	{
		/* We want the mouse visible */
		/* We call ShowCursor(TRUE) only if mouse was invisible before */
		if (!bMouseVisible)
		{
			ShowCursor(TRUE);
			bMouseVisible=TRUE;
		}
	}
	else
	{	
		/* We want the mouse invisible */
		/* We call ShowCursor(FALSE) only if mouse was visible before */
		if (bMouseVisible)
		{
			ShowCursor(FALSE);
			bMouseVisible=FALSE;
		}
	}
}


/*******************************************************************************
 * Function Name  : D3DShellGetVariable
 * Input		  : nVariableName
 * Output		  : pVariablePointer
 * Global Used    : D3DFE
 * Description    : Function called by the user to retrieve a rendering variable.
 *					A list of variables that can be retrieved is in D3DShell.h
 *******************************************************************************/
void D3DShellGetVariable(D3DShellVariables nVariableName, void *pVariablePointer)
{
	/* Check that the variable name is valid */
	if (nVariableName>SCREEN_SAVER_SETTING)
	{
		DEBUG("D3DShell *WARNING* : Invalid variable name.\n");
		return;
	}
	
	/* Fill pointer's content with appropriate value */
	switch(nVariableName)
	{
		/* pVariablePointer points to a HINSTANCE variable */
		case APPLICATION_INSTANCE_HANDLE :	*(HINSTANCE *)pVariablePointer=D3DFE.wndclass.hInstance; break;

		/* pVariablePointer points to a HINSTANCE variable */
		case PREVIOUS_INSTANCE_HANDLE :		*(HINSTANCE *)pVariablePointer=D3DFE.hPreviousInstance; break;

		/* pVariablePointer points to a HWND variable */
		case WINDOW_HANDLE :				*(HWND *)pVariablePointer=D3DFE.hwnd; break;

		/* pVariablePointer points to a BOOL variable */
		case FULLSCREEN_MODE :				*(BOOL *)pVariablePointer=D3DFE.bFullScreen; break;

		/* pVariablePointer points to a LPDIRECT3DSURFACE9 variable */
		case Z_BUFFER :						*(LPDIRECT3DSURFACE9 *)pVariablePointer=D3DFE.lpDDSZBuffer; break;

		/* pVariablePointer points to a D3DDEVTYPE variable */
		case D3DDEVICETYPE:					*(D3DDEVTYPE *)pVariablePointer=D3DFE.CurrentDeviceType; break;

		/* pVariablePointer points to a DWORD variable */
		case FRAME_BUFFER_BIT_DEPTH :		*(DWORD *)pVariablePointer=(D3DFE.bFullScreen ? D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format) : D3DFE.dwGDIBPP); break;

		/* pVariablePointer points to a DWORD variable */
		case NUM_BACK_BUFFER :				*(DWORD *)pVariablePointer=(D3DFE.bFullScreen ? D3DFE.nNumBackBuffer : 1); break;

		/* pVariablePointer points to a float variable */
		case FRAME_RATE :					*(float *)pVariablePointer=D3DFE.fFrameRate; break;
		
		/* pVariablePointer points to an int variable */
		case SCREEN_SAVER_SETTING :			*(int *)pVariablePointer=D3DFE.nScreenSaverSetting; break;
		
	}
}


/*******************************************************************************
 * Function Name  : D3DShellSetPreferences
 * Global Used    : D3DFE
 * Description    : Function called by the user to submit a menu and set
 *					preferences.
 *******************************************************************************/
void D3DShellSetPreferences(const char *pszApplicationName, 
							const HMENU	hUserMenuID, 
							const HACCEL hUserAccel, 
							const HICON	hUserIcon, 
							const DWORD	dwFlags)
{
	/* BACKDOOR settings : 
	   - if pszApplicationName is equal to "_DefaultPausedMessage", then dwFlags 
	     corresponds to a pointer to a string containing the new message to use. 
	   - if pszApplicationName is equal to "_DefautlClearColor", then dwFlags
	     corresponds to the Clear color that should be used when clearing viewport.
	   - if pszApplicationName is equal to "_DefaultWindowWidth", then dwFlags 
	     corresponds to the new default window width. Other parameters are ignored.
	   - if pszApplicationName is equal to "_DefaultWindowHeight", then dwFlags 
	     corresponds to the new default window height. Other parameters are ignored.
	   - if pszApplicationName is equal to "_DefaultScreenWidth", then dwFlags 
	     corresponds to the new default screen width. Other parameters are ignored.
	   - if pszApplicationName is equal to "_DefaultScreenHeight", then dwFlags 
	     corresponds to the new default screen height. Other parameters are ignored. 
	   - if pszApplicationName is equal to "_DefaultScreenBPP", then dwFlags 
	     corresponds to the new default screen bit depth. Other parameters are ignored. */
	if (!strcmp(pszApplicationName, "_DefaultPausedMessage"))
	{
		strncpy(D3DFE.pszPausedMessage, (char *)dwFlags, min(strlen((char *)dwFlags), 62));
		return;
	}
	if (!strcmp(pszApplicationName, "_DefaultClearColor"))
	{
		D3DFE.dwClearColor = dwFlags;
		return;
	}
	if (!strcmp(pszApplicationName, "_DefaultWindowWidth"))
	{
		D3DFE.dwWindowWidth=dwFlags;
		if (D3DFE.dwWindowWidth>D3DFE.dwMaxWindowWidth) D3DFE.dwWindowWidth=D3DFE.dwMaxWindowWidth;
		if (D3DFE.dwWindowWidth<D3DFE.dwMinWindowWidth) D3DFE.dwWindowWidth=D3DFE.dwMinWindowWidth;
		D3DFE.bWindowResized=TRUE;
		return;
	}
	if (!strcmp(pszApplicationName, "_DefaultWindowHeight"))
	{
		D3DFE.dwWindowHeight=dwFlags;
		if (D3DFE.dwWindowHeight>D3DFE.dwMaxWindowHeight) D3DFE.dwWindowHeight=D3DFE.dwMaxWindowHeight;
		if (D3DFE.dwWindowHeight<D3DFE.dwMinWindowHeight) D3DFE.dwWindowHeight=D3DFE.dwMinWindowHeight;
		D3DFE.bWindowResized=TRUE;
		return;
	}
	if ( (!strcmp(pszApplicationName, "_DefaultScreenWidth")) ||
		 (!strcmp(pszApplicationName, "_DefaultScreenHeight")) ||
		 (!strcmp(pszApplicationName, "_DefaultScreenBPP")) )
	{
		static DWORD	dwNewScreenWidth=0, dwNewScreenHeight=0, dwNewScreenBPP=0;

		/* New screen color depth */
		if (!strcmp(pszApplicationName, "_DefaultScreenBPP"))
		{
			if (dwFlags==16 || dwFlags==24 || dwFlags==32)
			{
				dwNewScreenBPP=dwFlags;
			}
		}

		/* New screen width */
		if (!strcmp(pszApplicationName, "_DefaultScreenWidth"))
		{
			dwNewScreenWidth = dwFlags;
		}

		/* New screen height */
		if (!strcmp(pszApplicationName, "_DefaultScreenHeight"))
		{
			dwNewScreenHeight= dwFlags;
		}
		
		/* If both screen width and height have been selected, choose corresponding resolution */
		if (dwNewScreenWidth && dwNewScreenHeight)
		{
			int nScreenMode;

			nScreenMode=D3DFEGetResolution(D3DFE.nCurrentAdapter, dwNewScreenWidth, dwNewScreenHeight, dwNewScreenBPP ? dwNewScreenBPP : D3DFE.dwGDIBPP);
			if (nScreenMode!=-1)
			{
				D3DFE.nCurrentResolution = nScreenMode;
			}
		}
		return;
	}

	/* If the user has submitted a name for his/her application */
	if (pszApplicationName)
	{
		strcpy(D3DFE.pszFrontEndTitle, pszApplicationName);
	}
	
	/* If the user has submitted a menu */
	if (hUserMenuID)
	{
		D3DFE.hUserMenu=hUserMenuID;
	}

	/* If the user has submitted an accelerator table */
	if (hUserAccel)
	{
		D3DFE.hAccel=hUserAccel;
	}

	/* If the user has submitted an icon */
	if (hUserIcon)
	{
		D3DFE.hIcon=hUserIcon;
	}

	/* Retrieve user's preferences */
	D3DFE.dwUserPreferences|=dwFlags;

	/* If Z-Buffer has been chosen as a default, enable it */
	if (D3DFE.dwUserPreferences & FORCE_ZBUFFER_ON)
	{
		D3DFE.bZBufferNeeded=TRUE;
	}

	/* If no Z-Buffer is selected by default then disable it */
	if (D3DFE.dwUserPreferences & DEFAULT_ZBUFFER_OFF)
	{
		D3DFE.bZBufferNeeded=FALSE;
	}

	/* If Clear viewport On as been chosen as a default, enable it */
	if (D3DFE.dwUserPreferences & DEFAULT_CLEAR_ON)
	{
		D3DFE.bClearViewport=TRUE;
	}

	/* If FullScreen as been chosen as a default, enable it */
	if (D3DFE.dwUserPreferences & DEFAULT_FULLSCREEN)
	{
		D3DFE.bFullScreen=TRUE;
	}
	
	/* Set the fullscreen flag if required */
	if (D3DFE.dwUserPreferences & FORCE_FULLSCREEN)
	{
		D3DFE.bFullScreen=TRUE;
	}

	/* If clear viewport is forced On or Off, then set
	   the state of the corresponding BOOL variable */
	if (D3DFE.dwUserPreferences & FORCE_CLEAR_ON)
	{
		D3DFE.bClearViewport=TRUE;
	}
	if (D3DFE.dwUserPreferences & FORCE_CLEAR_OFF)
	{
		D3DFE.bClearViewport=FALSE;
	}

	/* Check that window size and fullscreen are not both selected */
	if ( (D3DFE.dwUserPreferences & START_WINDOW_320X240) &&
		 (D3DFE.dwUserPreferences & (DEFAULT_FULLSCREEN | FORCE_FULLSCREEN)) )
	{
		/* Remove window size */
		D3DFE.dwUserPreferences^=START_WINDOW_320X240;
	}
	
	/* Starting window size */
	if (D3DFE.dwUserPreferences & START_WINDOW_320X240)
	{
		D3DFE.dwWindowWidth=320;
		D3DFE.dwWindowHeight=240;
		D3DFE.bWindowResized=TRUE;
	}

	/* Disable Z Clear or not */
	if (D3DFE.dwUserPreferences & DISABLE_Z_CLEAR)
	{
		D3DFE.bDisableZClear=TRUE;
	}

	/* Enable 3D Menus or not */
	if (D3DFE.dwUserPreferences & USE_3D_MENUS)
	{
		D3DFE.bUse3DMenus=TRUE;
	}

	/* Refresh rates */
	if (D3DFE.dwUserPreferences & NO_REFRESH_RATES)
	{
		D3DFE.bUseRefreshRates=FALSE;
	}
}


/*******************************************************************************
 * Function Name  : D3DShellReinitialise
 * Returns		  : TRUE or FALSE
 * Inputs		  : nAdapter, DeviceType, bFullScreen, nBufferMode, dwRenderWidth,
 *					dwRenderHeight, FrameBufferFormat, bZBufferEnabled, ZBufferFormat
 *					bZBufferEnabled, *pddpfZBufferFormat
 * Global Used    : D3DFE
 * Description    : Reinitialise rendering variables with selected parameters.
 *					This function will return FALSE if D3D can not be initialised
 *					with these settings.
 *******************************************************************************/
BOOL D3DShellReinitialise(int nAdapter, D3DDEVTYPE DeviceType,
						  BOOL bFullScreen, int nBufferMode,
						  D3DMULTISAMPLE_TYPE MultiSamplingType,
						  DWORD dwRenderWidth, DWORD dwRenderHeight, D3DFORMAT FrameBufferFormat,
						  int nVPType,
						  BOOL bZBufferEnabled, D3DFORMAT ZBufferFormat, 
						  D3DFORMAT BackBufferFormat, DWORD dwRefreshRate)
{
	HRESULT		hres;
	int			nNumBackBuffer;
	int			nDisplayMode;
	int			nGDIResolution;
	int			i;
	DWORD		dwWidth, dwHeight;
	BOOL		bScreenModeSelected=FALSE;
	BOOL		bZBuffer;
	BOOL		bFound;
	

	/* Check that this function was not called from InitApplication() */
	if (!D3DFE.lpD3D9)
	{
		OutputDebugString("D3DShell *WARNING* : 3D has not been initialised yet\n");
		return FALSE;
	}
	
	/* Check that this function was not called from InitView() */
	if (!D3DFE.bInitViewWasCalled)
	{
		OutputDebugString("D3DShell *WARNING* : D3DShellReinitialise() cannot be called from InitView()\n");
		return FALSE;
	}

	
	/* Get variables */
	bZBuffer =		bZBufferEnabled;
	dwWidth =		dwRenderWidth;
	dwHeight =		dwRenderHeight;
	nNumBackBuffer=	nBufferMode;
	

	/*****************************
	** Check for bad parameters **
	*****************************/
	if ((dwWidth<D3DFE.dwMinWindowWidth && dwWidth!=0) || (dwHeight<D3DFE.dwMinWindowHeight && dwHeight!=0) ||
		(nNumBackBuffer>2) || (nNumBackBuffer<0) ||
		(!bFullScreen && (dwWidth>D3DFE.dwMaxWindowWidth || dwHeight>D3DFE.dwMaxWindowHeight)) )
	{
		OutputDebugString("D3DShell *WARNING* : Bad parameters in D3DShellReinitialise()\n");
		return FALSE;
	}

	/* Set up current parameters if not specified (i.e. 0) */
	if (!DeviceType)		DeviceType =		D3DFE.CurrentDeviceType;
	if (!ZBufferFormat)		ZBufferFormat =		D3DFE.CurrentZBufferFormat;
	if (!nNumBackBuffer)	nNumBackBuffer =	D3DFE.nNumBackBuffer;


	/********************************************
	** Check whether these variables are valid **
	********************************************/
	nGDIResolution = D3DFEGetResolution(nAdapter, D3DFE.dwGDIWidth, D3DFE.dwGDIHeight, D3DFE.dwGDIBPP);
	if (!FrameBufferFormat)
	{
		FrameBufferFormat = Adapter[nAdapter].Mode[nGDIResolution].Format;
	}

	hres=IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, DeviceType, 
		bFullScreen ? FrameBufferFormat : Adapter[nAdapter].Mode[nGDIResolution].Format, FrameBufferFormat, 
		!bFullScreen);
	if (hres!=D3D_OK)
	{
		OutputDebugString("D3DShell *WARNING* : Invalid combination of parameters.\n");
		return FALSE;
	}

	/* Resolution in fullscreen mode */
	if (bFullScreen)
	{
		if (!dwWidth)	dwWidth = Adapter[nAdapter].Mode[D3DFE.nCurrentResolution].Width;
		if (!dwHeight)	dwHeight = Adapter[nAdapter].Mode[D3DFE.nCurrentResolution].Height;
		if (!FrameBufferFormat) FrameBufferFormat = Adapter[nAdapter].Mode[D3DFE.nCurrentResolution].Format;
		bScreenModeSelected=TRUE;

		/* Loop through all display modes to see if it exists */
		bFound=FALSE;
		for (i=0; i<Adapter[nAdapter].nNumberOfDisplayModes; i++)
		{
			if (Adapter[nAdapter].Mode[i].Width == dwWidth && 
				Adapter[nAdapter].Mode[i].Height == dwHeight &&
				Adapter[nAdapter].Mode[i].Format == FrameBufferFormat)
			{
				nDisplayMode=i;
				bFound=TRUE;
			}
		}
		if (!bFound)
		{
			OutputDebugString("D3DShell *WARNING* : Resolution not supported\n");
			return FALSE;
		}
	}
	else
	{
		/* Window size in window mode */
		
		/* Default values */
		if (!dwWidth)	dwWidth =	D3DFE.dwWindowWidth;
		if (!dwHeight)	dwHeight =	D3DFE.dwWindowHeight;
		if (!FrameBufferFormat)		FrameBufferFormat =	Adapter[nAdapter].Mode[nGDIResolution].Format;

		/* Set display mode */
		nDisplayMode = nGDIResolution;

		/* Update window size */
		D3DFE.dwWindowWidth =	dwWidth;
		D3DFE.dwWindowHeight =	dwHeight;

		/* Reset window properties */
		D3DFESetWindowProperties();
	}


	/* At this point it seems it is pretty safe to reinitialise */
	OutputDebugString("\nD3DShell : Reinitialising Direct3D...\n\n\n");

	/* Release all our variables (including ReleaseView()) */
	D3DFEReleaseAll();


	/* Reinitialise Direct3D */
	if (!D3DFEInitialiseDirect3D(nAdapter, 
								 bFullScreen, nDisplayMode,
								 MultiSamplingType,
								 bZBuffer, nNumBackBuffer, 
								 DeviceType, nVPType, ZBufferFormat,
								 BackBufferFormat, dwRefreshRate))
	{
		OutputDebugString("D3DShell *ERROR* : Failed to reinitialise D3D\n");
		return FALSE;
	}

	/* Seems that everything is fine */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFEScreenCapture
 * Global Used    : D3DFE
 * Description    : Take a screenshot of the current scene.
 *******************************************************************************/
static BOOL D3DFEScreenCapture(char *pszDirectory)
{
	HRESULT					hres;
	LPDIRECT3DSURFACE9		lpD3DSImageSurface=NULL;
	LPDIRECT3DSURFACE9		lpD3DSTmp=NULL;
	D3DLOCKED_RECT			LockedRect;
	BITMAPFILEHEADER		BitmapFileHeader;
	BITMAPINFO				BitmapInfo;
	FILE					*f;
	RGBTRIPLE				Triplet;
	RECT					SrcRect = { 0, 0, 0, 0 };
	POINT					pt = { 0, 0 };
	DWORD					dwWidth, dwHeight, BMPHeaderSize=54;
	WORD					*pWord;
	BYTE					*pByte, Blank=0;
	BYTE					*pSrc, *pDest;
	D3DLOCKED_RECT			LockedSrcRect, LockedDstRect;
	int						i, j;
	int						nScreenshotCount=0;
	char					pszString[128];
	char					pszScreenshotName[32];
	
	/* Asserts */
	ASSERT(D3DFE.lpDev9);

	/****************************************
	** Find out primary surface dimensions **
	****************************************/
	if (D3DFE.bFullScreen)
	{
		dwWidth = Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
		dwHeight = Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height;
	}
	else
	{
		dwWidth = D3DFE.dwGDIWidth;
		dwHeight = D3DFE.dwGDIHeight;
	}


	/************************************************************************************
	** Create SYSTEM MEMORY surface that will receive the contents of the front buffer **
	************************************************************************************/
	hres=IDirect3DDevice9_CreateOffscreenPlainSurface(D3DFE.lpDev9, dwWidth, dwHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, 
													  &lpD3DSImageSurface, NULL);
	if (hres!=D3D_OK)
	{
		DEBUG("D3DShell *ERROR* : CreateImageSurface() failed in D3DFEScreenCapture\n");
		return FALSE;
	}


	/*********************************
	** Copy front buffer to surface **
	*********************************/
	hres=IDirect3DDevice9_GetFrontBufferData(D3DFE.lpDev9, 0, lpD3DSImageSurface);
	if (hres!=D3D_OK)
	{
		DEBUG("D3DShell *ERROR* : GetFrontBuffer() failed in D3DFEScreenCapture\n");
		RELEASE(lpD3DSImageSurface);
		return FALSE;
	}


	/**************************************************************
	** Additional step for window mode: only capture client area **
	**************************************************************/
	if (!D3DFE.bFullScreen)
	{
		/* New width and height are render dimensions */
		dwWidth = D3DFE.dwWindowWidth;
		dwHeight = D3DFE.dwWindowHeight;

		/* Find out where the window's client area is */
		GetClientRect(D3DFE.hwnd, &SrcRect);
		ClientToScreen(D3DFE.hwnd, &pt);
		SrcRect.left +=   pt.x;
		SrcRect.right +=  pt.x;
		SrcRect.top +=    pt.y;
		SrcRect.bottom += pt.y;

		/* Create temporary surface */
		hres=IDirect3DDevice9_CreateOffscreenPlainSurface(D3DFE.lpDev9, dwWidth, dwHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM,
														  &lpD3DSTmp, NULL);
		if (hres!=D3D_OK)
		{
			DEBUG("D3DShell *ERROR* : CreateImageSurface() failed in D3DFEScreenCapture\n");
			RELEASE(lpD3DSImageSurface);
			return FALSE;
		}

		/* Lock source surface */
		IDirect3DSurface9_LockRect(lpD3DSImageSurface, &LockedSrcRect, &SrcRect, 0);
		
		/* Lock destination surface */
		IDirect3DSurface9_LockRect(lpD3DSTmp, &LockedDstRect, NULL, 0);

		/* Copy data */
		for (i=0; i<(int)dwHeight; i++)
		{
			pSrc = (BYTE *)LockedSrcRect.pBits + i*LockedSrcRect.Pitch;
			pDest = (BYTE *)LockedDstRect.pBits + i*LockedDstRect.Pitch;
			memcpy(pDest, pSrc, LockedDstRect.Pitch);
		}

		/* Unlock destination surface */
		IDirect3DSurface9_UnlockRect(lpD3DSTmp);

		/* Lock source surface */
		IDirect3DSurface9_UnlockRect(lpD3DSImageSurface);
		

		/* Release primary surface */
		RELEASE(lpD3DSImageSurface);

		/* New image surface is our client area surface */
		lpD3DSImageSurface = lpD3DSTmp;
	}


	/**************************************
	** Prepare BMP file to write to disk **
	**************************************/

	/* Fill BITMAPFILEHEADER structure */
	BitmapFileHeader.bfType =				'MB';
	BitmapFileHeader.bfSize =				BMPHeaderSize+(dwWidth*dwHeight*3);
	BitmapFileHeader.bfReserved1 =			0;
	BitmapFileHeader.bfReserved2 =			0;
	BitmapFileHeader.bfOffBits =			BMPHeaderSize;

	/* Fill BITMAPINFO structure */
	/* Fill BITMAPINFOHEADER structure inside BITMAPINFO structure */
	BitmapInfo.bmiHeader.biSize =			sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth =			dwWidth;
	BitmapInfo.bmiHeader.biHeight =			dwHeight;
	BitmapInfo.bmiHeader.biPlanes =			1;
	BitmapInfo.bmiHeader.biBitCount =		24;		/* Image will always be saved as 24-bit BMP */
	BitmapInfo.bmiHeader.biCompression =	BI_RGB;
	BitmapInfo.bmiHeader.biSizeImage =		dwWidth*dwHeight*3;
	BitmapInfo.bmiHeader.biXPelsPerMeter =	0;
	BitmapInfo.bmiHeader.biYPelsPerMeter =	0;
	BitmapInfo.bmiHeader.biClrUsed =		0;
	BitmapInfo.bmiHeader.biClrImportant =	0;

	/* Look for the first file name that doesn't already exist */
	do
	{
		sprintf(pszScreenshotName, "%sScreen%04d.bmp", pszDirectory, nScreenshotCount);
	}
	while (++nScreenshotCount<10000 && D3DFEFileExists(pszScreenshotName));

	/* If all files already exist, replace the first one */
	if (nScreenshotCount==10000)
	{
		sprintf(pszScreenshotName, "%sScreen0000.bmp", pszDirectory);
		OutputDebugString("D3DShell *WARNING* : Overwriting Screen0000.bmp\n");
	}

	/* Debug info */
	sprintf(pszString, "D3DShell : Saving %s... ", pszScreenshotName);
	OutputDebugString(pszString);

	/* Open file for writing */
	if ( !(f=fopen(pszScreenshotName, "wb")) )
	{
		sprintf(pszString, "\nD3DShell *WARNING* : Unable to write %s.\n", pszScreenshotName);
		DEBUG(pszString);
		RELEASE(lpD3DSImageSurface);
		return FALSE;
	}

	/* Write BitmapFileheader */
	if (fwrite(&BitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, f)<1)
	{
		sprintf(pszString, "\nD3DShell *WARNING* : Error in writing BITMAPFILEHEADER in %s.\n", pszScreenshotName);
		DEBUG(pszString);
		fclose(f);
		DeleteFile(pszScreenshotName);
		RELEASE(lpD3DSImageSurface);
		return FALSE;
	}

	/* Write BitmapInfoHeader */
	if (fwrite(&BitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, f)<1)
	{
		sprintf(pszString, "\nD3DShell *WARNING* : Error in writing BITMAPINFOHEADER in %s.\n", pszScreenshotName);
		DEBUG(pszString);
		fclose(f);
		DeleteFile(pszScreenshotName);
		RELEASE(lpD3DSImageSurface);
		return FALSE;
	}


	/*****************
	** Lock surface **
	*****************/
	hres=IDirect3DSurface9_LockRect(lpD3DSImageSurface, &LockedRect, NULL, D3DLOCK_READONLY);
	if (hres!=D3D_OK)
	{
		DEBUG("D3DShell *WARNING* : LockRect() failed in D3DFEScreenCapture().\n");
		fclose(f);
		DeleteFile(pszScreenshotName);
		RELEASE(lpD3DSImageSurface);
		return FALSE;
	}


	/***************
	** Write data **
	***************/
	/* Read all lines from bottom to top */
	for (i=dwHeight-1; i>=0; i--)
	{
		/* Position source pointers at the correct offset */
		pByte = (BYTE *)LockedRect.pBits + i*LockedRect.Pitch;
		pWord = (WORD *)pByte;

		/* For each pixel of each line */
		for (j=0; j<(int)dwWidth; j++)
		{
			/* Read RGB values */
			Triplet.rgbtBlue =	*pByte++;
			Triplet.rgbtGreen = *pByte++;
			Triplet.rgbtRed =	*pByte++;
			pByte++;
			
			/* Write colour channels */
			if (fwrite(&Triplet, sizeof(Triplet), 1, f)<1)
			{
				sprintf(pszString, "\nD3DShell *WARNING* : Error in writing %s.\n", pszScreenshotName);
				DEBUG(pszString);
				IDirect3DSurface9_UnlockRect(lpD3DSImageSurface);
				fclose(f);
				DeleteFile(pszScreenshotName);
				RELEASE(lpD3DSImageSurface);
				return FALSE;
			}
		}

		/* Align line in BMP on a DWORD boundary */
		if ((3*dwWidth)%4)
		{
			/* Padd with '0's */
			if (fwrite(&Blank, sizeof(unsigned char), 4-((3*dwWidth)%4), f)<1)
			{
				sprintf(pszString, "\nD3DShell *WARNING* : Error in writing %s.\n", pszScreenshotName);
				DEBUG(pszString);
				IDirect3DSurface9_UnlockRect(lpD3DSImageSurface);
				fclose(f);
				DeleteFile(pszScreenshotName);
				RELEASE(lpD3DSImageSurface);
				return FALSE;
			}
		}
	}

	/*******************
	** Unlock surface **
	*******************/
	hres=IDirect3DSurface9_UnlockRect(lpD3DSImageSurface);
	if (hres!=D3D_OK)
	{
		DEBUG("\nD3DShell *WARNING* : UnlockRect() failed in D3DFEScreenCapture\n");
	}

	/* Close file */
	if (fclose(f)!=0)
	{
		sprintf(pszString, "\nD3DShell *WARNING* : Error in closing %s.\n", pszScreenshotName);
		DEBUG(pszString);
	}

	/* Debug output */
	DEBUG("Done!\n");

	/* Release surface as we don't need it anymore */
	RELEASE(lpD3DSImageSurface);

	/* Beep = screen capture OK (only enabled for screenshots) */
	if (!D3DFE.bCaptureEachFrame)
	{
		MessageBeep(MB_OK);
	}

	/* Debug info */
	DEBUG("D3DShell : Screen capture successful\n");

	/* No problem occured */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFETimerProc
 * Inputs		  : hwnd, uMsg, idEvent, dwTime
 * Global Used    : D3DFE
 * Description    : Timer callback procedure
 *******************************************************************************/
static void CALLBACK D3DFETimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	/* Compute frame rate if ready */
	if (D3DFE.bRenderingReady && D3DFE.bHasFocus && !D3DFE.bMinimized)
	{
		D3DFE.fFrameRate=(D3DFE.dwFramesElapsed * 1000.0f) / TIMER_PERIOD;
		D3DFE.dwFramesElapsed=0;
	}
	else
	{
		D3DFE.fFrameRate=0.0f;
		D3DFE.dwFramesElapsed=0;
	}
}


/*******************************************************************************
 * Function Name  : D3DFESetWindowProperties
 * Inputs		  : None
 * Global Used    : D3DFE
 * Description    : Reset window properties depending on new window size
 *******************************************************************************/
static void D3DFESetWindowProperties()
{
	RECT Rect;

	/* Compute window position (screen centre) */
	D3DFE.dwWindowPositionX=(D3DFE.dwGDIWidth-D3DFE.dwWindowWidth)/2;
	D3DFE.dwWindowPositionY=(D3DFE.dwGDIHeight-D3DFE.dwWindowHeight)/2;
	
	/* That's to prevent the window menu to be above the upper limit of the screen */
	if (D3DFE.dwWindowPositionY<38) D3DFE.dwWindowPositionY=38;
	
	/* Compute window size corresponding to client area size */
	SetRect(&Rect,  D3DFE.dwWindowPositionX, D3DFE.dwWindowPositionY, 
					D3DFE.dwWindowWidth+D3DFE.dwWindowPositionX-1, D3DFE.dwWindowHeight+D3DFE.dwWindowPositionY-1);
	AdjustWindowRect(&Rect, D3DFE.dwWindowStyle, D3DFE.bUse3DMenus ? FALSE : TRUE);

	/* Set window frame values */
	D3DFE.dwWindowFramePositionX=Rect.left;
	D3DFE.dwWindowFramePositionY=Rect.top;

	D3DFE.dwWindowFrameWidth=(Rect.right-Rect.left)+1;
	D3DFE.dwWindowFrameHeight=(Rect.bottom-Rect.top)+1;
	
	/* Compute window space between client and window */
	D3DFE.dwWindowSpaceX=D3DFE.dwWindowFrameWidth-D3DFE.dwWindowWidth;
	D3DFE.dwWindowSpaceY=D3DFE.dwWindowFrameHeight-D3DFE.dwWindowHeight;
}


/*******************************************************************************
 * Function Name  : D3DFERemoveString
 * Inputs		  : pszStringToRemove
 * Input/Output	  : pszString
 * Global Used    : None
 * Description    : Remove string pszStringToRemove from pszString
 *******************************************************************************/
static void D3DFERemoveString(char *pszString, char *pszStringToRemove)
{
	char	pszTmpString[300];
	char	*c;
	int		nPos;
	int		nStringLength=strlen(pszString);
	int		nStringTRLength=strlen(pszStringToRemove);

	/* Copy string */
	strcpy(pszTmpString, pszString);

	/* Find occurence of string to remove */
	c=strstr(pszTmpString, pszStringToRemove);

	/* If found, then remove it (actually copy the contents of the string before
	   and after the string to look for) */
	if (c)
	{
		/* Compute position of pszStringToRemove in pszString */
		nPos=c-pszTmpString;
		
		/* If a string is before pszStringToRemove, copy it to the new buffer */
		if (nPos)
		{
			strncpy(pszString, pszTmpString, nPos);
		}
		
		/* Copy the rest of the string */
		strncpy(&pszString[nPos], &pszTmpString[nPos+nStringTRLength+1], nStringLength-nStringTRLength);
		
		/* Append a zero to specify end of string */
		pszString[nStringLength-nStringTRLength]='\0';
	}
}


/*******************************************************************************
 * Function Name  : D3DFEInstantaneousFPS
 * Inputs		  : None
 * Output		  : None
 * Returns		  : DWORD
 * Description    : Compute the instantaneous frame rate.
 *******************************************************************************/
static float D3DFEInstantaneousFPS()
{
	LONGLONG		llTime; 
	DWORD			dwCountElapsed;
	float			fFrameRate;
	static int		nNumberOfCountsInOneSecond=-1;
	static LONGLONG	llTimeCounter;
	
	/* If first time called, then initialise counter */
	if (nNumberOfCountsInOneSecond==-1)
	{
		/* Get counter frequency and convert it to ms/count */
		QueryPerformanceFrequency((LARGE_INTEGER *)&llTime);
		if (llTime==0) 
		{
			DEBUG("D3DShell *WARNING* : High resolution performance counter not supported.\n");
		}
		nNumberOfCountsInOneSecond=(int)llTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&llTimeCounter);
	}
	
	/* How much counts since previous frame ? */
	QueryPerformanceCounter((LARGE_INTEGER *)&llTime);
	dwCountElapsed=(DWORD)(llTime-llTimeCounter);
	
	/* The following line is to avoid a nasty division by zero */
	if (!dwCountElapsed) dwCountElapsed=1;

	/* Immediate frame rate calculation */
	fFrameRate = (float)nNumberOfCountsInOneSecond/dwCountElapsed;
	
	/* Set last timer count value */
	llTimeCounter=llTime;

	/* Return frame rate */
	return fFrameRate;
}


/*******************************************************************************
 * Function Name  : D3DFEGetNumberOfBits
 * Inputs		  : dwMask
 * Returns		  : Number of bits in mask
 * Global Used    : None
 * Description    : Returns the number of bits a logical mask has.
 *******************************************************************************/
static WORD D3DFEGetNumberOfBits(DWORD dwMask)
{
    WORD wBits = 0;

    while (dwMask)
    {
        dwMask = dwMask & ( dwMask - 1 );  
        wBits++;
    }
    return wBits;
}


/*******************************************************************************
 * Function Name  : D3DFEIsWindowsVersionOK
 * Inputs		  : dwMajor, dwMinor, dwSPMajor
 * Returns		  : TRUE or FALSE
 * Global Used    : None
 * Description    : Check Windows version and return OK if equal or later of the
 *					version submitted.
 *******************************************************************************/
static BOOL D3DFEIsWindowsVersionOK(DWORD dwMajor, DWORD dwMinor, DWORD dwSPMajor)
{
	OSVERSIONINFO	osvi;

	/* Initialise structure */
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);

	/* Get information */
	GetVersionEx(&osvi);

	/* First compare major version */
	if (osvi.dwMajorVersion>dwMajor) 
	{
		return TRUE;
	}
	else 
	if (osvi.dwMajorVersion==dwMajor)
	{
		/* Same major version number, check minor version */
		if (osvi.dwMinorVersion>dwMinor)
		{
			return TRUE;
		}
		else
		if (osvi.dwMinorVersion==dwMinor)
		{
			/* Major and minor versions are equal to the ones desired, we now need
			   to check the service pack version */
			if (dwSPMajor && osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
			{
				HKEY	hKey;
				DWORD	dwCSDVersion;
				DWORD	dwSize;
				BOOL	bMeetsSPRequirement=FALSE;

				/* Open registry key */
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Windows", 0,
					KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
				{
					/* Size of DWORD */
					dwSize=sizeof(dwCSDVersion);
					
					/* Query key value */
					if (RegQueryValueEx(hKey, "CSDVersion", NULL, NULL, (unsigned char *)&dwCSDVersion, &dwSize) == ERROR_SUCCESS)
					{
						bMeetsSPRequirement = (LOWORD(dwCSDVersion) >= dwSPMajor);
					}

					/* Close key */
					RegCloseKey(hKey);
				}

				/* Return status */
				return bMeetsSPRequirement;
			}

			/* Version is OK */
			return TRUE;
		}
	}

	/* If we get to that point it means that Windows version is lower what we expect */
	return FALSE;
}


/*******************************************************************************
 * Function Name  : D3DFEGetUserCommandLine
 * Inputs		  : *pszCommandLine
 * Output		  : *pszRemainingCommandLine
 * Returns		  : Nothing
 * Global Used    : D3DFE
 * Description    : Return user command line. D3DShell-specific options will be
 *					excluded.
 *******************************************************************************/
static void D3DFEGetUserCommandLine(char *pszOriginalCommandLine, char *pszRemainingCommandLine)
{
	char			*pszOption;
	char			pszCommandLine[512];
	char			pszValue[32], pszString[512];
	DWORD			dwNewBufferingMode;
	static DWORD	dwNewWidth=0, dwNewHeight=0, dwNewBPP=0;

	/* Check parameters */
	if (!pszCommandLine || !pszRemainingCommandLine)
	{
		return;
	}

	/* Copy command line so that we don't alter the original */
	strcpy(pszCommandLine, pszOriginalCommandLine);

	/* Copy command line */
	strcpy(pszRemainingCommandLine, pszCommandLine);

	/* Put it in uppercase */
	_strupr(pszRemainingCommandLine);
	
	/* Delimitor */
	pszOption=strtok(pszCommandLine, " -");
    while(pszOption!=NULL)
	{
		/* Put argument in uppercase */
		_strupr(pszOption);
		
		/* Autokill application after FRAMESBEFOREKILLING frames */
		if (!lstrcmpi(pszOption, "AUTOKILL"))
		{
			D3DFERemoveString(pszRemainingCommandLine, "-AUTOKILL");
        }

		/* Application will start FullScreen by default */
		if (!lstrcmpi(pszOption, "FULLSCREEN")) 
		{
			D3DFERemoveString(pszRemainingCommandLine, "-FULLSCREEN");
        }
		
		/* Select Ref. Rasterizer device by default */
		if (!lstrcmpi(pszOption, "REFDEVICE"))
		{
			D3DFERemoveString(pszRemainingCommandLine, "-REFDEVICE");
        } 

		/* Specify buffering mode */
		if (!lstrcmpi(pszOption, "BUFFER="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[7]);

			/* Convert this string value to a proper int */
			sscanf(pszValue, "%d", &dwNewBufferingMode);

			/* Remove string from command line */
			sprintf(pszString, "-BUFFER=%s", pszValue);
			D3DFERemoveString(pszRemainingCommandLine, pszString);
		}

		/* New width */
		if (strstr(pszOption, "WIDTH="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[6]);

			/* Convert this string value to a proper int */
			sscanf(pszValue, "%d", &dwNewWidth);

			/* Remove string from command line */
			sprintf(pszString, "-WIDTH=%s", pszValue);
			D3DFERemoveString(pszRemainingCommandLine, pszString);
		}

		/* New height */
		if (strstr(pszOption, "HEIGHT="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[7]);

			/* Convert this string value to a proper int */
			sscanf(pszValue, "%d", &dwNewHeight);

			/* Remove string from command line */
			sprintf(pszString, "-HEIGHT=%s", pszValue);
			D3DFERemoveString(pszRemainingCommandLine, pszString);
		}

		/* New color depth */
		if (strstr(pszOption, "BPP="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[4]);

			/* Remove string from command line */
			sprintf(pszString, "-BPP=%s", pszValue);
			D3DFERemoveString(pszRemainingCommandLine, pszString);
		}

		/* Specify frame number at which a screen capture will occur */
		if (strstr(pszOption, "CAPTURE="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[8]);

			/* Remove string from command line */
			sprintf(pszString, "-CAPTURE=%s", pszValue);
			D3DFERemoveString(pszRemainingCommandLine, pszString);
        }

		/* Set directory to write captured images to */
		if (strstr(pszOption, "CAPTURE_DIR="))
		{
			int		i=0;
			char	pszTemp[256];

			/* Path is specified using speech marks */
			if (pszOption[i+12]=='"')
			{
				while (pszOption[i + 12 + 1] != '"') i++;

				/* Copy string */
				strncpy(pszTemp, &pszOption[13], i);

				/* Prepare string to remove from command line */
				sprintf(pszString, "-CAPTURE_DIR=%c%s%c", '"', pszTemp, '"');
			}
			else
			{
				/* No speech mark was specified */
				while (pszOption[i+12]!=' ' && pszOption[i+12]!='\0') i++;

				/* Copy string */
				strncpy(pszTemp, &pszOption[12], i);

				/* Prepare string to remove from command line */
				sprintf(pszString, "-CAPTURE_DIR=%s", pszTemp);
			}

			/* Terminate string correctly */
			pszTemp[i] = '\\';
			pszTemp[i + 1] = '\0';

			/* Remove string from command line */
			D3DFERemoveString(pszRemainingCommandLine, pszString);
        }

		/* Let the user specify the number of frames to render before terminating the application */
		if (strstr(pszOption, "AUTOKILL_TIME="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[14]);

			/* Remove string from command line */
			sprintf(pszString, "-AUTOKILL_TIME=%s", pszValue);
			D3DFERemoveString(pszRemainingCommandLine, pszString);
        }

		/* Launched with screen saver setting */
		if (strstr(pszOption, "SETTING="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[8]);

			/* Remove string from command line */
			sprintf(pszString, "-SETTING=%s", pszValue);
			D3DFERemoveString(pszRemainingCommandLine, pszString);
        }

		/* Launched with screen saver setting */
		if (strstr(pszOption, "CAPTUREEVERYFRAME"))
		{
			D3DFERemoveString(pszRemainingCommandLine, "-CAPTUREEVERYFRAME");
        }

		pszOption=strtok(NULL, " -");
    }
}


/*******************************************************************************
 * Function Name  : D3DFEProcessCommandLine
 * Inputs		  : *pszOriginalCommandLine
 * Output		  : None
 * Returns		  : Nothing
 * Global Used    : D3DFE
 * Description    : Parse the command line in seach of one of supported options.
 *					String comparison is NOT case sensitive.
 *******************************************************************************/
static void D3DFEProcessCommandLine(char *pszOriginalCommandLine)
{
	char			*pszOption;
	char			pszValue[32], pszCommandLine[512];
	DWORD			dwNewBufferingMode;
	static DWORD	dwNewWidth=0, dwNewHeight=0, dwNewBPP=0;

	/* Check parameter */
	if (!pszOriginalCommandLine)
	{
		return;
	}

	/* Copy string so that we don't alter the original */
	strcpy(pszCommandLine, pszOriginalCommandLine);

	/* Delimitor */
	pszOption=strtok(pszCommandLine, " -");
    while(pszOption!=NULL)   
	{
		/* Put argument in uppercase */
		_strupr(pszOption);
		
		/* Autokill application after FRAMESBEFOREKILLING frames */
		if (!lstrcmpi(pszOption, "AUTOKILL"))
		{
			D3DFE.bAutoKill=TRUE;
        }

		/* Application will start FullScreen by default */
		if (!lstrcmpi(pszOption, "FULLSCREEN")) 
		{
            D3DFE.dwUserPreferences|=DEFAULT_FULLSCREEN;
        }
		
		/* Select Ref. Rasterizer device by default */
		if (!lstrcmpi(pszOption, "REFDEVICE"))
		{
			D3DFE.dwUserPreferences|=DEFAULT_USE_REF_DRIVER;
        }

		/* Specify buffering mode */
		if (strstr(pszOption, "BUFFER="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[7]);

			/* Convert this string value to a proper int */
			sscanf(pszValue, "%d", &dwNewBufferingMode);

			/* Only set new buffering mode if value if valid */
			if (dwNewBufferingMode>=1 && dwNewBufferingMode<=3)	
			{
				/* Remove any buffering mode-dependant flags */
				D3DFE.dwUserPreferences &= ~(FORCE_DOUBLE_BUFFER | FORCE_TRIPLE_BUFFER | DISABLE_TRIPLE_BUFFER);

				/* Set new buffering mode */
				D3DFE.nNumBackBuffer = dwNewBufferingMode;
			}
		}

		/* Specify render width, height and bits-per-pixel */
		if (strstr(pszOption, "WIDTH=") || strstr(pszOption, "HEIGHT=") || strstr(pszOption, "BPP="))
		{
			/* New color depth */
			if (strstr(pszOption, "BPP="))
			{
				/* Get value */
				strcpy(pszValue, &pszOption[4]);

				/* Convert this string value to a proper int */
				sscanf(pszValue, "%d", &dwNewBPP);

				/* Check that value is correct, reset it if not */
				if (dwNewBPP!=16 && dwNewBPP!=24 && dwNewBPP!=32) dwNewBPP=0;
			}

			/* New width */
			if (strstr(pszOption, "WIDTH="))
			{
				/* Get value */
				strcpy(pszValue, &pszOption[6]);

				/* Convert this string value to a proper int */
				sscanf(pszValue, "%d", &dwNewWidth);
			}

			/* New height */
			if (strstr(pszOption, "HEIGHT="))
			{
				/* Get value */
				strcpy(pszValue, &pszOption[7]);

				/* Convert this string value to a proper int */
				sscanf(pszValue, "%d", &dwNewHeight);
			}
        }

		/* Specify frame number at which a screen capture will occur */
		if (strstr(pszOption, "CAPTURE="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[8]);

			/* Convert this string value to a proper int */
			sscanf(pszValue, "%d", &D3DFE.nScreenCaptureFrame);
        }

		/* Set directory to write captured images to */
		if (strstr(pszOption, "CAPTURE_DIR="))
		{
			int		i=0;

			/* Path is specified using speech marks */
			if (pszOption[i+12]=='"')
			{
				while (pszOption[i + 12 + 1] != '"') i++;

				/* Copy string */
				strncpy(D3DFE.pszCaptureDirectory, &pszOption[13], i);
			}
			else
			{
				/* No speech mark was specified */
				while (pszOption[i+12]!=' ' && pszOption[i+12]!='\0') i++;

				/* Copy string */
				strncpy(D3DFE.pszCaptureDirectory, &pszOption[12], i);
			}

			/* Terminate string correctly */
			D3DFE.pszCaptureDirectory[i] = '\\';
			D3DFE.pszCaptureDirectory[i + 1] = '\0';
        }

		/* Let the user specify the number of frames to render before terminating the application */
		if (strstr(pszOption, "AUTOKILL_TIME="))
		{
			/* Set state */
			D3DFE.bAutoKill=TRUE;

			/* Get value */
			strcpy(pszValue, &pszOption[14]);

			/* Convert this string value to a proper int */
			sscanf(pszValue, "%d", &D3DFE.nAutokillTime);
        }

		/* Screen Saver Setting */
		if (strstr(pszOption, "SETTING="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[8]);

			/* Convert this string value to a proper int */
			sscanf(pszValue, "%d", &D3DFE.nScreenSaverSetting);
        }

		/* Vertex Processing */
		if (strstr(pszOption, "VP="))
		{
			if (strstr(&pszOption[3], "SW"))	D3DFE.nVPType = SW;
			if (strstr(&pszOption[3], "HW"))	D3DFE.nVPType = HW;
			if (strstr(&pszOption[3], "MIXED")) D3DFE.nVPType = MIXED;
		}

		/* Capture every frame */
		if (strstr(pszOption, "CAPTUREEVERYFRAME"))
		{
			D3DFE.bCaptureEachFrame=TRUE;
		}

		/* Next command */
		pszOption=strtok(NULL, " -");
    }

	/* Fullscreen case */
	if (D3DFE.dwUserPreferences & (DEFAULT_FULLSCREEN | FORCE_FULLSCREEN))
	{
		/* If both screen width and height have been selected, choose corresponding resolution */
		if (dwNewWidth && dwNewHeight)
		{
			int nScreenMode;

			nScreenMode=D3DFEGetResolution(D3DFE.nCurrentAdapter, dwNewWidth, dwNewHeight, dwNewBPP ? dwNewBPP : 16);
			if (nScreenMode!=-1)
			{
				D3DFE.nCurrentResolution = nScreenMode;
			}
		}
	}
	else
	{
		/* Window case */
		if (dwNewWidth)		D3DFE.dwWindowWidth=dwNewWidth;
		if (dwNewHeight)	D3DFE.dwWindowHeight=dwNewHeight;
		D3DFE.bWindowResized=TRUE;
	}
}


/*******************************************************************************
 * Function Name  : D3DFEFileExists
 * Returns        : TRUE or FALSE
 * Global Used    : None
 * Description    : Return TRUE if the specified file exists, FALSE otherwise.
 *******************************************************************************/
static BOOL D3DFEFileExists(char *n)
{
	FILE *fp=fopen(n, "rb"); 

	return (fp ? (fclose(fp)==0) : FALSE);
}


/*******************************************************************************
 * Function Name  : D3DFEGetResolution
 * Inputs		  : nAdapter, dwWidth, dwHeight, dwBPP
 * Returns        : Resolution number
 * Global Used    : D3DFE
 * Description    : Return resolution number corresponding to specified parameters.
 *					If no corresponding resolution was found, return -1
 *******************************************************************************/
static int D3DFEGetResolution(int nAdapter, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
	int i;

	/* Loop through all available resolutions and try to find a match */
	for (i=0; i<Adapter[nAdapter].nNumberOfDisplayModes; i++)
	{
		if (Adapter[nAdapter].Mode[i].Width == dwWidth &&
			Adapter[nAdapter].Mode[i].Height == dwHeight &&
			D3DFEGetBPP(Adapter[nAdapter].Mode[i].Format) == (int)dwBPP)
		{
			return i;
		}
	}
	
	/* No match was found */
	return -1;
}


/*******************************************************************************
 * Function Name  : D3DFEGetBPP
 * Inputs		  : Format
 * Returns        : Number of bits per pixel corresponding to that format.
 * Global Used    : D3DFE, Adapter[]
 * Description    : Return color depth corresponding to the D3DFORMAT value
 *					submitted. Possible return values are:
 *					0: Unknown.
 *					4: Only used for DXT1 format.
 *					8: 8 bpp, 8-bit palettised or DXT2-5 formats.
 *					16: 16 bpp
 *					24: 24 bpp
 *					32: 32 bpp
 *******************************************************************************/
static int D3DFEGetBPP(D3DFORMAT Format)
{
	/* Which format are we dealing with ? */
	switch (Format)
	{
		/* Return value is 0 */
		case D3DFMT_UNKNOWN:
		case D3DFMT_VERTEXDATA:			return 0; break;

		/* Return value is 4 (only used for DXT1) */
		case D3DFMT_DXT1:				return 4; break;

		/* Return value is 8 */
		case D3DFMT_R3G3B2:
		case D3DFMT_A8:	
		case D3DFMT_P8:
		case D3DFMT_L8:
		case D3DFMT_A4L4:				
		case D3DFMT_DXT2:
		case D3DFMT_DXT3:
		case D3DFMT_DXT4:
		case D3DFMT_DXT5:				return 8; break;

		/* Return value is 16 */
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:			
		case D3DFMT_A8R3G3B2:
		case D3DFMT_X4R4G4B4:
		case D3DFMT_A8P8:
		case D3DFMT_A8L8:
		case D3DFMT_V8U8:
		case D3DFMT_L6V5U5:				
		case D3DFMT_UYVY:
		case D3DFMT_YUY2:				
		case D3DFMT_D16:				
		case D3DFMT_D16_LOCKABLE:
		case D3DFMT_D15S1:
		case D3DFMT_INDEX16:			return 16; break;

		/* Return value is 24 */
		case D3DFMT_R8G8B8:				return 24; break;

		/* Return value is 32 */
		case D3DFMT_D32:
		case D3DFMT_D24S8:
		case D3DFMT_D24X8:
		case D3DFMT_D24X4S4:
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
		case D3DFMT_X8L8V8U8:
		case D3DFMT_Q8W8V8U8:
		case D3DFMT_V16U16:
		case D3DFMT_A2B10G10R10:
		case D3DFMT_A2R10G10B10:
		case D3DFMT_G16R16:
		case D3DFMT_INDEX32:			return 32; break;

		/* Return value is 64 */
		case D3DFMT_Q16W16V16U16:
		case D3DFMT_A16B16G16R16:		return 64; break;

		/* Unknown value */
		default:						return 0;
	}
}


/*******************************************************************************
 * Function Name  : D3DFEGetColorChannels
 * Inputs		  : Format
 * Outputs		  : *pR, *pG, *pB, *pA
 * Returns        : Number of bits per color channel corresponding to that format.
 * Global Used    : D3DFE, Adapter[]
 * Description    : Return number of bits per color channel in submitted format.
 *******************************************************************************/
static BOOL D3DFEGetColorChannels(D3DFORMAT Format, BYTE *pR, BYTE *pG, BYTE *pB, BYTE *pA)
{
	/* Which format are we dealing with ? */
	switch (Format)
	{
		/* Look for known formats */
		case D3DFMT_R3G3B2:			*pR=3; *pG=3; *pB=2; *pA=0; return TRUE; break;
		case D3DFMT_A8:				*pR=0; *pG=0; *pB=0; *pA=8; return TRUE; break;
		case D3DFMT_R5G6B5:			*pR=5; *pG=6; *pB=5; *pA=0; return TRUE; break;
		case D3DFMT_X1R5G5B5:		*pR=5; *pG=5; *pB=5; *pA=0; return TRUE; break;
		case D3DFMT_A1R5G5B5:		*pR=5; *pG=5; *pB=5; *pA=1; return TRUE; break;
		case D3DFMT_A4R4G4B4:		
		case D3DFMT_X4R4G4B4:		*pR=4; *pG=4; *pB=4; *pA=4; return TRUE; break;
		case D3DFMT_A8R3G3B2:		*pR=3; *pG=3; *pB=2; *pA=8; return TRUE; break;
		case D3DFMT_R8G8B8:			*pR=8; *pG=8; *pB=8; *pA=0; return TRUE; break;
		case D3DFMT_X8R8G8B8:		
		case D3DFMT_A8R8G8B8:		*pR=8; *pG=8; *pB=8; *pA=8; return TRUE; break;

		/* Undetermined formats */
		default:					*pR=0; *pG=0; *pB=0; *pA=0; return FALSE;
	}
}


/*******************************************************************************
 * Function Name  : D3DFEPick3DDevice
 * Inputs		  : nAdapter, DevType, bFullScreen
 * Returns        : 3D device
 * Global Used    : D3DFE, Adapter[]
 * Description    : Check whether submitted 3D device is supported, if not return
 *					best device.
 *******************************************************************************/
static D3DDEVTYPE D3DFEPick3DDevice(int nAdapter, D3DDEVTYPE DevType, BOOL bFullScreen)
{
	int nResolution;

	/* Get actual screen resolution we're presently in (independent of window or fullscreen mode) */
	if (!bFullScreen)	nResolution = D3DFEGetResolution(nAdapter, D3DFE.dwGDIWidth, D3DFE.dwGDIHeight, D3DFE.dwGDIBPP);
		else nResolution = D3DFE.nCurrentResolution;

	switch(DevType)
	{
		case D3DDEVTYPE_HAL:	/* Check that 3D device is supported at this resolution */
								if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, D3DDEVTYPE_HAL, 
															   Adapter[nAdapter].Mode[nResolution].Format,
															   Adapter[nAdapter].Mode[nResolution].Format,
															   bFullScreen)==D3D_OK)
								{
									return D3DDEVTYPE_HAL;
								}

		case D3DDEVTYPE_REF:	/* Check that 3D device is supported at this resolution */
								if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, D3DDEVTYPE_REF, 
															   Adapter[nAdapter].Mode[nResolution].Format,
															   Adapter[nAdapter].Mode[nResolution].Format,
															   bFullScreen)==D3D_OK)
								{
									return D3DDEVTYPE_REF;
								}

		case D3DDEVTYPE_SW:		/* Check that 3D device is supported at this resolution */
								if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, D3DDEVTYPE_SW, 
															   Adapter[nAdapter].Mode[nResolution].Format,
															   Adapter[nAdapter].Mode[nResolution].Format,
															   bFullScreen)==D3D_OK)
								{
									return D3DDEVTYPE_SW;
								}
	}

	/* Unknown 3D device!! */
	DEBUG("D3DShell *ERROR* : Unknown 3D device!\n");
	return D3DDEVTYPE_REF;
}


/*******************************************************************************
 * Function Name  : D3DFEPickMultiSampleType
 * Inputs		  : nAdapter, DevType, bFullScreen, BackBufferFormat, 
 *					DepthStencilFormat, DesiredMultiSampleType
 * Returns        : Back buffer format
 * Global Used    : D3DFE, Adapter[]
 * Description    : Check whether submitted multisample type is supported, 
 *					or returns D3DMULTISAMPLE_NONE if not.
 *******************************************************************************/
static D3DMULTISAMPLE_TYPE D3DFEPickMultiSampleType(int nAdapter, D3DDEVTYPE DevType, 
													BOOL bFullScreen,
													D3DFORMAT BackBufferFormat, D3DFORMAT DepthStencilFormat,
													D3DMULTISAMPLE_TYPE DesiredMultiSampleType)
{
	DWORD	dwQuality=0;

	/* Check if back buffer can be multisampled */
	if (IDirect3D9_CheckDeviceMultiSampleType(D3DFE.lpD3D9, nAdapter, DevType, BackBufferFormat, !bFullScreen, 
											  DesiredMultiSampleType, &dwQuality)!=D3D_OK)
	{
		/* No, return D3DMULTISAMPLE_NONE */
		return D3DMULTISAMPLE_NONE;
	}

	/* Check if depth buffer can be multisampled */
	if (IDirect3D9_CheckDeviceMultiSampleType(D3DFE.lpD3D9, nAdapter, DevType, DepthStencilFormat, !bFullScreen, 
											  DesiredMultiSampleType, &dwQuality)!=D3D_OK)
	{
		/* No, return D3DMULTISAMPLE_NONE */
		return D3DMULTISAMPLE_NONE;
	}

	/* Both calls returned OK, desired multisampletype is supported */
	return DesiredMultiSampleType;
}


/*******************************************************************************
 * Function Name  : D3DFEPickBackBufferFormat
 * Inputs		  : nAdapter, DevType, DisplayModeFormat, bFullScreen, DesiredFormat
 * Returns        : Back buffer format
 * Global Used    : D3DFE, Adapter[]
 * Description    : Check whether submitted back buffer format is supported, 
 *					or returns closest format if not.
 *******************************************************************************/
static D3DFORMAT D3DFEPickBackBufferFormat(int nAdapter, D3DDEVTYPE DevType, D3DFORMAT DisplayModeFormat, BOOL bFullScreen, 
										   D3DFORMAT DesiredFormat)
{
	/* Check whether submitted back buffer format can be used with this configuration */
	if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, DesiredFormat, !bFullScreen)==D3D_OK)
	{
		return DesiredFormat;
	}

	/* Desired format not supported, try other alternatives */
	if (DisplayModeFormat==D3DFMT_X8R8G8B8)
	{
		/* Try D3DFMT_A8R8G8B8 */
		if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DFMT_A8R8G8B8, !bFullScreen)==D3D_OK)
		{
			return D3DFMT_A8R8G8B8;
		}

		/* Try D3DFMT_X8R8G8B8 */
		if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DFMT_X8R8G8B8, !bFullScreen)==D3D_OK)
		{
			return D3DFMT_X8R8G8B8;
		}
	}
	else
	{
		/* Try D3DFMT_R5G6B5 */
		if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DFMT_R5G6B5, !bFullScreen)==D3D_OK)
		{
			return D3DFMT_R5G6B5;
		}

		/* Try D3DFMT_A1R5G5B5 (highly unlikely that this one is supported if 565 isn't! */
		// TO DO: could have an option to prefer 1555 16 bits format over 565 */
		if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DFMT_A1R5G5B5, !bFullScreen)==D3D_OK)
		{
			return D3DFMT_A1R5G5B5;
		}

		/* Try D3DFMT_X1R5G5B5 (highly unlikely that this one is supported if 565 isn't! */
		if (IDirect3D9_CheckDeviceType(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DFMT_X1R5G5B5, !bFullScreen)==D3D_OK)
		{
			return D3DFMT_X1R5G5B5;
		}
	}

	/* If we get there, this means we couldn't find any back buffer format */
	OutputDebugString("D3DShell *WARNING* : No back buffer was found in D3DFEPickBackBufferFormat()\n");

	/* Return default value anyway */
	return D3DFMT_R5G6B5;
}


/*******************************************************************************
 * Function Name  : D3DFEPickRefreshRate
 * Inputs		  : nAdapter, dwWidth, dwHeight, DisplayModeFormat, dwDesiredRefreshRate
 * Returns        : Back buffer format
 * Global Used    : D3DFE, Adapter[]
 * Description    : Check whether submitted refresh rate is supported with 
 *					specified resolution. Returns closest refresh rate if not.
 *******************************************************************************/
static DWORD D3DFEPickRefreshRate(int nAdapter, DWORD dwWidth, DWORD dwHeight, D3DFORMAT DisplayModeFormat, 
								  DWORD dwDesiredRefreshRate)
{
	int i;

	/* Look for desired refrehs rate */
	for (i=0; i<Adapter[nAdapter].nTotalNumberOfDisplayModes; i++)
	{
		if (dwWidth == Adapter[nAdapter].pAllMode[i].Width &&
			dwHeight == Adapter[nAdapter].pAllMode[i].Height &&
			DisplayModeFormat == Adapter[nAdapter].pAllMode[i].Format &&
			dwDesiredRefreshRate == Adapter[nAdapter].pAllMode[i].RefreshRate)
		{
			/* Specified resolution supports desired refresh rate */
			return dwDesiredRefreshRate;
		}
	}

	/* Try 75Hz */
	for (i=0; i<Adapter[nAdapter].nTotalNumberOfDisplayModes; i++)
	{
		if (dwWidth == Adapter[nAdapter].pAllMode[i].Width &&
			dwHeight == Adapter[nAdapter].pAllMode[i].Height &&
			DisplayModeFormat == Adapter[nAdapter].pAllMode[i].Format &&
			75 == Adapter[nAdapter].pAllMode[i].RefreshRate)
		{
			/* 75Hz found */
			return 75;
		}
	}

	/* Try 60Hz */
	for (i=0; i<Adapter[nAdapter].nTotalNumberOfDisplayModes; i++)
	{
		if (dwWidth == Adapter[nAdapter].pAllMode[i].Width &&
			dwHeight == Adapter[nAdapter].pAllMode[i].Height &&
			DisplayModeFormat == Adapter[nAdapter].pAllMode[i].Format &&
			60 == Adapter[nAdapter].pAllMode[i].RefreshRate)
		{
			/* 60Hz found */
			return 60;
		}
	}

	/* Couldn't find proper refresh rate, return default (0) */
	return 0;
}


/*******************************************************************************
 * Function Name  : D3DFEPickDepthBufferFormat
 * Inputs		  : nAdapter, DevType, DisplayModeFormat, bUseStencil, DesiredFormat
 * Returns        : Depth buffer format
 * Global Used    : D3DFE, Adapter[]
 * Description    : Check whether submitted depth buffer format is supported, 
 *					or returns closest format if not.
 *					If DesiredFormat is 0, parameter is ignored.
 *******************************************************************************/
static D3DFORMAT D3DFEPickDepthBufferFormat(int nAdapter, D3DDEVTYPE DevType, D3DFORMAT DisplayModeFormat, BOOL bUseStencil, 
											BOOL bUseMultiSampling, D3DFORMAT DesiredFormat)
{
	/* Check whether submitted Z buffer format can be used with this configuration */
	if (DesiredFormat!=0)
	{
		if (IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, DesiredFormat)==D3D_OK)
		{
			return DesiredFormat;
		}
	}

	/* Stencil case */
	if (bUseStencil)
	{
		/* Try D3DFMT_D24S8 first */
		if (IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)==D3D_OK)
		{
			return D3DFMT_D24S8;
		}

		/* Then try D3DFMT_D24X4S4 */
		if (IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4)==D3D_OK)
		{
			return D3DFMT_D24X4S4;
			
		}

		/* Then try D3DFMT_D15S1 */
		if (IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1)==D3D_OK)
		{
			return D3DFMT_D15S1;
			
		}

		/* No Depth buffer are supported with stencil, continue */
	}

	if (!bUseMultiSampling)
	{
		/* Try D3DFMT_D16_LOCKABLE */
		if (IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16_LOCKABLE)==D3D_OK)
		{
			return D3DFMT_D16_LOCKABLE;
		}
	}

	/* Then try D3DFMT_D16 */
	if (IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16)==D3D_OK)
	{
		return D3DFMT_D16;
	}

	/* Then try D3DFMT_D24X8 */
	if (IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8)==D3D_OK)
	{
		return D3DFMT_D24X8;
	}

	/* And finally try D3DFMT_D32 */
	if (IDirect3D9_CheckDeviceFormat(D3DFE.lpD3D9, nAdapter, DevType, DisplayModeFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32)==D3D_OK)
	{
		return D3DFMT_D32;
	}

	/* If we get there, this means we couldn't find any Z-Buffer format */
	OutputDebugString("D3DShell *WARNING* : No depth buffer was found in D3DFEPickDepthBufferFormat()\n");

	/* Return default value anyway */
	return D3DFMT_D16;
}


/*******************************************************************************
 * Function Name  : D3DFEWaitForVSync
 * Returns        : Nothing
 * Global Used    : None
 * Description    : Wait for next vertical retrace
 *******************************************************************************/
static void D3DFEWaitForVSync()
{
#define 	VGA_INPUT_STATUS_1    0x03DA

	__asm
	{
		mov     dx, VGA_INPUT_STATUS_1
L101:
		in      al, dx
		test    al, 8
		jz      L101
	}
}



/************************** 3D MENUS FUNCTIONS START HERE **************************/


#ifdef D3DSHELL_USE_3D_MENUS


/*******************************************************************************
 * Function Name  : D3DShellDisplay3DMenus
 * Global Used    : 
 * Description    : Called from scene file between BeginScene and EndScene
 *				  :	Displays 3D menu system in current state
 *******************************************************************************/
BOOL D3DShellDisplay3DMenus()
{
	int i = 0;
	int j = 0;
	int k = 0;
	DWORD dwCurrentWidth, dwCurrentHeight; 
	DWORD dwUserWidth, dwUserHeight;

	/* Make sure 3D menus are being used */
	if (!D3DFE.bUse3DMenus || !bM_Menu_On)
	{
		return FALSE;
	}

	if (D3DFE.bFullScreen)
	{
		dwCurrentWidth =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
		dwCurrentHeight =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height;
	}
	else
	{
		dwCurrentWidth =	D3DFE.dwWindowWidth;
		dwCurrentHeight =	D3DFE.dwWindowHeight;
	}

	/* Get the user aspect ratio used for Print3D */
	D3DTPrint3DGetAspectRatio(&dwUserWidth, &dwUserHeight);

	/* Keep the same aspect ration when changing resolution */
	D3DTPrint3DSetTextures(D3DFE.lpDev9, 640,480);

	//for all columns
	for (i=0; i<nM_Num_Columns; i++)
	{
		//display column heading
		D3DTPrint3DDisplayWindow (pM_Columns[i].dwNumber);
		//only display column elements if this column selected
		if (pM_Columns[i].bHighlighted == TRUE && bM_In_Menu == TRUE) 
		{
			//for all items in the selected column
			for (j=0; j<pM_Num_Items[i]; j++)
			{
				//display this item
				D3DTPrint3DDisplayWindow (pM_Columns[i].pElement[j].dwNumber);
				//if it has a separator, display it
				if (pM_Columns[i].pElement[j].bSeparator)
					D3DTPrint3DDisplayWindow (pM_Columns[i].pElement[j].nSeparator_num);
				//if this item is highlighted and has popup, display popup
				if (pM_Columns[i].pElement[j].bSub_menu && pM_Columns[i].pElement[j].bHighlighted)
				{
					//for all items in this popup
					for (k=0; k<pM_Num_Sub_Items[pM_Columns[i].pElement[j].nSub_menu_num]; k++)
					{
						//display this popup item
						D3DTPrint3DDisplayWindow(pM_Sub_Columns[pM_Columns[i].pElement[j].nSub_menu_num].pElement[k].dwNumber);
						//if it has a separator, display it
						if (pM_Sub_Columns[pM_Columns[i].pElement[j].nSub_menu_num].pElement[k].bSeparator)
							D3DTPrint3DDisplayWindow (pM_Sub_Columns[pM_Columns[i].pElement[j].nSub_menu_num].pElement[k].nSeparator_num);
					}		
				}
			}
		}
	}
	//display title bar that extends to right edge of screen
	D3DTPrint3DDisplayWindow (dwM_title_bar);

	/* Draw right hand menu */
	if(dwCurrentWidth>(fM_title_bar_size+160) && D3DFE.bDisplay3DMenuIcons)
	{
		D3DTPrint3D ((float)(dwCurrentWidth-155) * (100.0f/640.0f), 1, 0.5, 0xFFD0D0FF,"F1\x1 F2\x2 F3\x3  ESC@");
	}

	else if (dwCurrentWidth>(fM_title_bar_size+54))
	{
		D3DTPrint3D ((float)(dwCurrentWidth-48) * (100.0f/640.0f),1,0.5,0xFFD0D0FF,"ESC@");
	}

	/* Restore user aspect ratio */
	D3DTPrint3DSetTextures(D3DFE.lpDev9, dwUserWidth, dwUserHeight);

	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFECountItems3DMenus
 * Global Used    : 
 * Description    : count number of items in each menu (column)
 *					excludes separators
 *******************************************************************************/
static void D3DFECountItems3DMenus()
{
	int		i = 0;
	int		j = 0;
	int		num_items = 0;
	HMENU	hTempMenu; 

	//for every column
	for (i=0; i<GetMenuItemCount(D3DFE.hMenu); i++)	
	{
		hTempMenu=GetSubMenu(D3DFE.hMenu, i);
		//for every item in this column
		for (j=0; j<GetMenuItemCount(hTempMenu); j++)	
		{
			//don't allocate mem for a separator - ignore
			if (! ((GetMenuState(hTempMenu, j, MF_BYPOSITION) & MF_SEPARATOR) && (!GetSubMenu(hTempMenu, j))) )
			{
				num_items++;
			}
		}
		pM_Num_Items[i] = num_items;
		num_items = 0;
	}
}


/*******************************************************************************
 * Function Name  : D3DFECountSubColumns3DMenus
 * Global Used    : 
 * Description    : count total number of sub columns (popups) in menu
 *******************************************************************************/
static int D3DFECountSubColumns3DMenus()
{
	int sub_count   = 0;
	int i, j = 0;
	HMENU	hTempMenu;

	//for every column
	for (i=0; i<GetMenuItemCount(D3DFE.hMenu); i++)	
	{
		hTempMenu=GetSubMenu(D3DFE.hMenu, i);
		//for every item in this column
		for (j=0; j<GetMenuItemCount(hTempMenu); j++)	
		{
			//if it has a submenu
			if (GetSubMenu(hTempMenu, j))
			{
				sub_count++;
			}
		}
	}
	return sub_count;
}


/*******************************************************************************
 * Function Name  : D3DFECountSubItems3DMenus
 * Global Used    : 
 * Description    : sets how many items are in each user sub column (popup)
 *					ignores separators
 *******************************************************************************/
static void D3DFECountSubItems3DMenus()	
{
	int		this_col = 0;
	int		i,j,k;
	int		item_count = 0;
	HMENU	hTempMenu;
	HMENU	hTempSubMenu;

	/* Parse through all menu item strings */
	for (i=0; i<GetMenuItemCount(D3DFE.hMenu); i++)
	{
		//pick out 1st column
		hTempMenu=GetSubMenu(D3DFE.hMenu, i);
		//for every item in the column
		for (j=0; j<GetMenuItemCount(hTempMenu); j++)	
		{
			//pick out 1st item
			hTempSubMenu=GetSubMenu(hTempMenu, j);

			if (hTempSubMenu)
			{
				for (k=0; k<GetMenuItemCount(hTempSubMenu); k++)
				{
					if (! ( (GetMenuState(hTempSubMenu, k, MF_BYPOSITION) & MF_SEPARATOR) && (!GetSubMenu(hTempSubMenu, k))))
					{
						item_count++;
					}
				}
				pM_Num_Sub_Items[this_col] = item_count;
				item_count = 0;
				this_col++;
			}
		}
	}
}


/*******************************************************************************
 * Function Name  : D3DFEFormatString3DMenus
 * Global Used    : 
 * Description    : Format menu string to remove '&' and accelerator
 *******************************************************************************/
static char *D3DFEFormatString3DMenus(char *input)
{
	static char	 new_string[128];
	char		*p = new_string;

	for (; *input != '\0'; ++input)
	{
		//ignore &
		if (*input == '&')
			continue;
		//if accelerator found, put brackets around it (and finish string)
		if (*input == '\t')
		{
			input++;

			*p++ = ' ';
			*p++ = ' ';
			*p++ = '(';
				while(*input != ' ' && *input != '\0') *p++ = *input++;
			*p++ = ')';
			break;
		}
		//else copy character to new string
		else
		{
			*p = *input;
			p++;
		}
	}
	//at end of string so terminate it and return
	*p = '\0';
	return new_string;
}


/********************************************************************************
 * Function Name  : D3DFEConvertTo3DMenus
 * Returns		  : 
 * Global Used    : 
 * Description    : Parses the menu created in BuildMenus() and generates 3D Menu.
 *					Only run once.
 ********************************************************************************/
static BOOL D3DFEConvertTo3DMenus()
{
	int		i,j,k;	
	int		col_count			= 0;	//the column we're currently adding to
	int		item_count			= 0;	//the item we're currently adding
	int		col_sub_count		= 0;	//the sub column we're currently adding to
	int		item_sub_count		= 0;	//the sub item we're currently adding
	HMENU	hTempMenu;
	HMENU	hTempSubMenu;
	char	pszString[128];
	char	pszString2[128];
	float	fMin_item_width		= 8.0f;	//the minimum item width of any column (units in % screen x)
	MENU_ITEM	*psMenuItem;

	//Find out how many columns we need including user ones 
	nM_Num_Columns		= GetMenuItemCount(D3DFE.hMenu);
	//Find out how many user sub-columns (popups) there are
	nM_Num_Sub_Columns	= D3DFECountSubColumns3DMenus();

	/* allocate memory for this many columns and popups */

	//enough space for all item counts
	if (nM_Num_Columns)
	{
		pM_Num_Items	=	(int *)calloc(nM_Num_Columns+1,	sizeof(*pM_Num_Items));			
		if (pM_Num_Items==NULL)
		{
			DEBUG("D3DShell *ERROR* : ConvertTo3DMenus() failed.\n");
			MessageBox(D3DFE.hwnd, "Failed to allocate 3D menu memory\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
			return FALSE;
		}
	}

	//enough space for all sub item counts
	if (nM_Num_Sub_Columns)
	{
		pM_Num_Sub_Items=	(int *)calloc(nM_Num_Sub_Columns+1,	sizeof(*pM_Num_Sub_Items));			
		if (pM_Num_Sub_Items==NULL)
		{
			DEBUG("D3DShell *ERROR* : ConvertTo3DMenus() failed.\n");
			MessageBox(D3DFE.hwnd, "Failed to allocate 3D menu memory\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
			return FALSE;
		}
	}

	//enough space for all columns.
	if (nM_Num_Columns)
	{
		pM_Columns		=	(MENU_COLUMN *)calloc(nM_Num_Columns+1,	sizeof(*pM_Columns));	
		if (pM_Columns==NULL)
		{
			DEBUG("D3DShell *ERROR* : ConvertTo3DMenus() failed.\n");
			MessageBox(D3DFE.hwnd, "Failed to allocate 3D menu memory\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
			return FALSE;
		}
	}
	
	//enough space for all sub columns.
	if (nM_Num_Sub_Columns)
	{
		pM_Sub_Columns	=	(MENU_COLUMN *)calloc(nM_Num_Sub_Columns+1,	sizeof(*pM_Sub_Columns));	
		if (pM_Sub_Columns==NULL)
		{
			DEBUG("D3DShell *ERROR* : ConvertTo3DMenus() failed.\n");
			MessageBox(D3DFE.hwnd, "Failed to allocate 3D menu memory\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
			return FALSE;
		}
	}
	
	//Find out how many items in each column (pM_Num_Items[0] to pM_Num_Items[x])
	D3DFECountItems3DMenus();	
		
	//Find out how many items in each popup	(pM_Num_Sub_Items[0] to pM_Num_Sub_Items[x])
	D3DFECountSubItems3DMenus(); 


	/* Parse through all menu item strings */
	for (i=0; i<GetMenuItemCount(D3DFE.hMenu); i++)
	{
		/* Get string for this menu item number i, eg."File" */
		GetMenuString(D3DFE.hMenu, i, pszString, 100, MF_BYPOSITION);
	
		/* Allocate enough space for all items in this column */
		psMenuItem = (MENU_ITEM *)calloc(pM_Num_Items[col_count]+1, sizeof(*psMenuItem));
		if (psMenuItem==NULL)
		{
			DEBUG("D3DShell *ERROR* : ConvertTo3DMenus() failed.\n");
			MessageBox(D3DFE.hwnd, "Failed to allocate 3D menu memory\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
			return FALSE;
		}

		/* Get menu handle */
		hTempMenu=GetSubMenu(D3DFE.hMenu, i);
		
		/* Parse through all menu items in this column */
		for (j=0; j<GetMenuItemCount(hTempMenu); j++)
		{
			/* Check if there are other submenus (popups) */
			hTempSubMenu=GetSubMenu(hTempMenu, j);

			/* start of popup */
			if (hTempSubMenu)
			{
				//set this item to have a sub menu
				psMenuItem[item_count].bSub_menu = TRUE;
				
				//assign this sub menu its number
				psMenuItem[item_count].nSub_menu_num = col_sub_count;
				//allocate enough mem for all items in this user sub column
				pM_Sub_Columns[col_sub_count].pElement = (MENU_ITEM *)calloc(pM_Num_Sub_Items[col_sub_count], sizeof(*pM_Sub_Columns->pElement));
				if (pM_Sub_Columns[col_sub_count].pElement==NULL)
				{
					DEBUG("D3DShell *ERROR* : ConvertTo3DMenus() failed.\n");
					MessageBox(D3DFE.hwnd, "Failed to allocate 3D menu memory\nExiting program", D3DFE.pszFrontEndTitle, MB_OK | MB_ICONERROR);
					return FALSE;
				}
				// Parse through all menu items in sub menu 
				for (k=0; k<GetMenuItemCount(hTempSubMenu); k++)
				{
					//if there is a separator and it's not 1st item, add it
					if ((GetMenuState(hTempSubMenu, k, MF_BYPOSITION) & MF_SEPARATOR) && (item_sub_count > 0) && (!GetSubMenu(hTempSubMenu, k)))
						pM_Sub_Columns[col_sub_count].pElement[item_sub_count-1].bSeparator = TRUE;
					else
					{
						// Get string for this menu item number k 
						GetMenuString(hTempSubMenu, k, pszString2, 100, MF_BYPOSITION);
						//Format string to remove '&' and accelerator then store it
						strcpy(pszString2, D3DFEFormatString3DMenus(pszString2));
						strcpy(pM_Sub_Columns[col_sub_count].pElement[item_sub_count].pszText, pszString2);

						/* If this string is biggest so far, set sub-column width to its length */
						if ((D3DTPrint3DGetLength (MENU3D_FONT_SIZE_ELEMENT, (BYTE *)pszString2)+5) > pM_Sub_Columns[col_sub_count].fItem_width)
							pM_Sub_Columns[col_sub_count].fItem_width = (D3DTPrint3DGetLength (MENU3D_FONT_SIZE_ELEMENT, (BYTE *)pszString2))+5;

						pM_Sub_Columns[col_sub_count].pElement[item_sub_count].uIdentifier = GetMenuItemID(hTempSubMenu, k);
						//check and set if this item is ghosted or ticked
						if ((GetMenuState(hTempSubMenu, k, MF_BYPOSITION) & MF_GRAYED))
							pM_Sub_Columns[col_sub_count].pElement[item_sub_count].bGhosted = TRUE;
						if ((GetMenuState(hTempSubMenu, k, MF_BYPOSITION) & MF_CHECKED))
							pM_Sub_Columns[col_sub_count].pElement[item_sub_count].bTicked = TRUE;
						//done with this sub item, so increment count
						item_sub_count++;
					}
				}
				//impose the minimum item width value if required
				if (fMin_item_width > pM_Sub_Columns[col_sub_count].fItem_width)
					pM_Sub_Columns[col_sub_count].fItem_width = fMin_item_width;
				//added another sub column (popup), so increment count
				col_sub_count++;
				//reset item count ready for next sub column
				item_sub_count = 0;
			}
			/* end of popup */


			//if this item is a separator and isn't 1st item, add it
			if ((GetMenuState(hTempMenu, j, MF_BYPOSITION) & MF_SEPARATOR) && (item_count > 0) 
				&& (psMenuItem[item_count].bSub_menu == FALSE))
			{
				psMenuItem[item_count-1].bSeparator = TRUE;
			}
			else
			{
				/* Get string for this menu item number j */
				GetMenuString(hTempMenu, j, pszString2, 100, MF_BYPOSITION);
				//Format string to remove '&' and accelerator then store it
				strcpy(pszString2, D3DFEFormatString3DMenus(pszString2));
				strcpy(psMenuItem[item_count].pszText, pszString2);

				/* If this string is biggest so far, set column width to its length */
				if ((D3DTPrint3DGetLength (MENU3D_FONT_SIZE_ELEMENT, (BYTE *)pszString2)+5) > pM_Columns[col_count].fItem_width)
					pM_Columns[col_count].fItem_width = (D3DTPrint3DGetLength (MENU3D_FONT_SIZE_ELEMENT, (BYTE *)pszString2))+5;

				psMenuItem[item_count].uIdentifier = GetMenuItemID(hTempMenu, j); 
				//check if this item is ghosted or ticked
				if ((GetMenuState(hTempMenu, j, MF_BYPOSITION) & MF_GRAYED))
					psMenuItem[item_count].bGhosted = TRUE;
				if ((GetMenuState(hTempMenu, j, MF_BYPOSITION) & MF_CHECKED))
					psMenuItem[item_count].bTicked = TRUE;
				item_count++;
			}
		}	
		//Format string to remove '&' and accelerator if they exist, then store
		strcpy(pszString, D3DFEFormatString3DMenus(pszString));
		strcpy(pM_Columns[col_count].pszTitle, pszString);
		//impose the minimum item width value if required
		if (fMin_item_width > pM_Columns[col_count].fItem_width)
			pM_Columns[col_count].fItem_width = fMin_item_width;

		pM_Columns[col_count].pElement = psMenuItem;
		//column finished so increment count
		col_count++;
		//reset item count for next column
		item_count = 0;
	}

	return TRUE;
}


/********************************************************************************
 * Function Name  : D3DFECreate3DMenus
 * Returns		  : 
 * Global Used    : 
 * Description    : Creates the 3D Menu boxes. Must been done again if res change.
 ********************************************************************************/
static BOOL D3DFECreate3DMenus()
{
	int		i, j, m;
	int		nNumLines = 0;
	int		dwCurrentWidth;

	/* create all windows */
	for (i=0; i<nM_Num_Columns; i++)		//create each column heading
	{
		//create a heading window with max characters MENU3D_HEADING_MAX_X and max rows 1
		pM_Columns[i].dwNumber = D3DTPrint3DCreateWindow (MENU3D_HEADING_MAX_X, 1);
		//set its output to be the title of the column
		D3DTPrint3DWindowOutput (pM_Columns[i].dwNumber, pM_Columns[i].pszTitle);
		//set window to be deactivated at the start, turn off window title - ie.just have one cell, turn off the bevelled border for a simple, clean look
		D3DTPrint3DSetWindowFlags (pM_Columns[i].dwNumber, PRINT3D_DEACTIVATE_WIN | PRINT3D_DEACTIVATE_TITLE | PRINT3D_NO_BORDER);
		
		//create each item in column
		for (j=0; j<pM_Num_Items[i]; j++)	
		{
			pM_Columns[i].pElement[j].dwNumber = D3DTPrint3DCreateWindow (MENU3D_ELEMENT_MAX_X, 1);
			//output offset to line up with ticked columns
			D3DTPrint3DWindowOutput (pM_Columns[i].pElement[j].dwNumber, "   %s", pM_Columns[i].pElement[j].pszText);	
			D3DTPrint3DSetWindowFlags (pM_Columns[i].pElement[j].dwNumber, PRINT3D_DEACTIVATE_WIN | PRINT3D_DEACTIVATE_TITLE | PRINT3D_NO_BORDER);
				
			//if there is a separator, create it
			if (pM_Columns[i].pElement[j].bSeparator)
			{
				pM_Columns[i].pElement[j].nSeparator_num = D3DTPrint3DCreateWindow (MENU3D_ELEMENT_MAX_X, 1);
			
				//scale separator to width of column
				nNumLines = (int)((pM_Columns[i].fItem_width - 2) * 1.29f); 
				for (m=0; m<nNumLines; m++)
					D3DTPrint3DWindowOutput (pM_Columns[i].pElement[j].nSeparator_num, "-"); //output separator

				D3DTPrint3DSetWindowFlags (pM_Columns[i].pElement[j].nSeparator_num, PRINT3D_DEACTIVATE_WIN | PRINT3D_DEACTIVATE_TITLE | PRINT3D_NO_BORDER);
			}
			//if there is a sub menu, add popup arrow to item text
			if (pM_Columns[i].pElement[j].bSub_menu)
			{
				D3DTPrint3DClearWindowBuffer(pM_Columns[i].pElement[j].dwNumber);
				D3DTPrint3DWindowOutput(pM_Columns[i].pElement[j].dwNumber, "   %s $", pM_Columns[i].pElement[j].pszText);
			}
		}
	}
	//create sub menus
	for (i=0; i<nM_Num_Sub_Columns; i++)		
	{
		for (j=0; j<pM_Num_Sub_Items[i]; j++)//create each item in column
		{
			pM_Sub_Columns[i].pElement[j].dwNumber = D3DTPrint3DCreateWindow (MENU3D_ELEMENT_MAX_X, 1);
			D3DTPrint3DWindowOutput (pM_Sub_Columns[i].pElement[j].dwNumber, "   %s", pM_Sub_Columns[i].pElement[j].pszText);
			D3DTPrint3DSetWindowFlags (pM_Sub_Columns[i].pElement[j].dwNumber, PRINT3D_DEACTIVATE_WIN | PRINT3D_DEACTIVATE_TITLE | PRINT3D_NO_BORDER);
						
			if (pM_Sub_Columns[i].pElement[j].bSeparator)//if there is a separator, create it
			{
				pM_Sub_Columns[i].pElement[j].nSeparator_num = D3DTPrint3DCreateWindow (MENU3D_ELEMENT_MAX_X, 1);
		
				nNumLines = (int)((float)(pM_Columns[i].fItem_width-2) * 1.29); 
				for (m=0; m<nNumLines; m++)
					D3DTPrint3DWindowOutput (pM_Sub_Columns[i].pElement[j].nSeparator_num, "-");
				D3DTPrint3DSetWindowFlags (pM_Sub_Columns[i].pElement[j].nSeparator_num, PRINT3D_DEACTIVATE_WIN | PRINT3D_DEACTIVATE_TITLE | PRINT3D_NO_BORDER );	
			}
		}
	}

	if (D3DFE.bFullScreen)
	{
		dwCurrentWidth =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
	}
	else
	{
		dwCurrentWidth =	D3DFE.dwWindowWidth;
	}

	dwM_title_bar = D3DTPrint3DCreateWindow (500, 1);
	D3DTPrint3DWindowOutput (dwM_title_bar, "");
	D3DTPrint3DSetWindowFlags (dwM_title_bar, PRINT3D_DEACTIVATE_WIN | PRINT3D_DEACTIVATE_TITLE | PRINT3D_NO_BORDER);	

	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFEReset3DMenus
 * Returns		  : True if successful
 * Description    : Clears all selections and resets menu
 *******************************************************************************/
static BOOL D3DFEReset3DMenus()
{
	if (nM_Current_Heading>nM_Num_Columns || nM_Current_Heading<0) 
	{
		return FALSE; 
	}

	//if we have a heading
	if (nM_Current_Heading != -1)
	{
		//unhighlight heading
		pM_Columns[nM_Current_Heading].bHighlighted = FALSE;

		//if we have an item
		if(nM_Current_Item != -1)
		{
			//unhighlight this item
			pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;

			//if we have a sub item (popup)
			if (nM_Current_Sub_Item != -1)
			{
				//unhighlight this popup
				pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
			}
		}
	}
	
	//set no heading
	nM_Current_Heading = -1;
	//set no item
	nM_Current_Item = -1;
	//set no popup
	nM_Current_Sub_Item = -1;

	//set not in menu
	bM_In_Menu = FALSE;
	//set not in popup
	bM_In_Pop_Up = FALSE;

	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DShellTick3DMenus
 * Returns		  : 
 * Global Used    : 
 * Description    : Pass it an identifier and a boolean and it will
 *				  : tick or untick the element with that identifier.
 *******************************************************************************/
BOOL D3DShellTick3DMenus(UINT identifier, BOOL tick)
{
	int i, j = 0;

	//for all columns
	for (i=0; i<nM_Num_Columns; i++)
	{
		//for every item in this column
		for (j=0; j<pM_Num_Items[i]; j++)
		{
			//if this items identifier is the one passed
			if (identifier == pM_Columns[i].pElement[j].uIdentifier)
			{	
				//if we told to tick
				if (tick) 
				{
					//set this item ticked
					pM_Columns[i].pElement[j].bTicked = TRUE;
					//clear the window buffer of this item
					D3DTPrint3DClearWindowBuffer(pM_Columns[i].pElement[j].dwNumber);
					//output a tick symbol & this items text
					D3DTPrint3DWindowOutput(pM_Columns[i].pElement[j].dwNumber, "%%%s", pM_Columns[i].pElement[j].pszText);				
				}
				//if we were told to untick
				else 
				{
					pM_Columns[i].pElement[j].bTicked = FALSE;
					D3DTPrint3DClearWindowBuffer(pM_Columns[i].pElement[j].dwNumber);
					D3DTPrint3DWindowOutput(pM_Columns[i].pElement[j].dwNumber, "   %s", pM_Columns[i].pElement[j].pszText);
				}
				break;
			}
		}
	}
	//for all sub-columns...
	for (i=0; i<nM_Num_Sub_Columns; i++)
	{
		for (j=0; j<pM_Num_Sub_Items[i]; j++)
		{
			if (identifier == pM_Sub_Columns[i].pElement[j].uIdentifier)
			{	
				if (tick) 
				{
					pM_Sub_Columns[i].pElement[j].bTicked = TRUE;
					D3DTPrint3DClearWindowBuffer(pM_Sub_Columns[i].pElement[j].dwNumber);
					D3DTPrint3DWindowOutput(pM_Sub_Columns[i].pElement[j].dwNumber, "%%%s", pM_Sub_Columns[i].pElement[j].pszText);
				}
				else 
				{
					pM_Sub_Columns[i].pElement[j].bTicked = FALSE;
					D3DTPrint3DClearWindowBuffer(pM_Sub_Columns[i].pElement[j].dwNumber);
					D3DTPrint3DWindowOutput(pM_Sub_Columns[i].pElement[j].dwNumber, "   %s", pM_Sub_Columns[i].pElement[j].pszText);
				}
				break;
			}
		}
	}
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DShellGhost3DMenus
 * Returns		  : 
 * Global Used    : 
 * Description    : Pass it an identifier and a boolean and it will
 *				  : ghost or unghost the element with that identifier.
 *******************************************************************************/
BOOL D3DShellGhost3DMenus(UINT identifier, BOOL ghost)
{
	int i, j = 0;

	//for all columns
	for (i=0; i<nM_Num_Columns; i++)
	{
		//for every item in this column
		for (j=0; j<pM_Num_Items[i]; j++)
		{
			//if this items identifier is the one passed
			if (identifier == pM_Columns[i].pElement[j].uIdentifier)
			{
				//if we were told to ghost
				if (ghost) 
				{
					pM_Columns[i].pElement[j].bGhosted = TRUE;	
				}
				//if we were told to unghost
				else 
				{
					pM_Columns[i].pElement[j].bGhosted = FALSE;
				}
				break;
			}
		}
	}
	//for every sub column
	for (i=0; i<nM_Num_Sub_Columns; i++)
	{
		for (j=0; j<pM_Num_Sub_Items[i]; j++)
		{
			if (identifier == pM_Sub_Columns[i].pElement[j].uIdentifier)
			{
				if (ghost) 
				{
					pM_Sub_Columns[i].pElement[j].bGhosted = TRUE;	
				}
				else 
				{
					pM_Sub_Columns[i].pElement[j].bGhosted = FALSE;
				}
				break;
			}
		}
	}
	return TRUE;
}


/*******************************************************************************
 * Function Name  : D3DFEMouse3DMenus
 * Global Used    : 
 * Description    : Updates the 3D Menus on input from the mouse 
 *					(movement/left click)
 *******************************************************************************/
static void D3DFEMouse3DMenus(UINT message)
{
	int			i, j, k, s;
	float		separator_offset		= 0.0f;	//increased by element height each time separator found
	float		popup_separator_offset	= 0.0f;	//increased by element height each time popup separator found
	float		heading_width_count		= 0.0f;	//increased by width of each column as it's added to determine how far we are from left
	int			nAbsoluteMouseX;
	int			dwCurrentWidth;
	MENU_COLUMN	*psMenuCol;

	/* Get current render surface width */
	if (D3DFE.bFullScreen)
		dwCurrentWidth =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
	else
		dwCurrentWidth =	D3DFE.dwWindowWidth;


	//MENU OFF, MOUSE MOVE	(1/4)
	/**********************************
	** Highlight/Unhighlight heading **
	**********************************/
	if ((bM_In_Menu == FALSE)			//no menu selected yet
		&& (message == WM_MOUSEMOVE))	//mouse movement
	{
		//for all columns
		for (i=0; i<nM_Num_Columns; i++)
		{
			psMenuCol = &pM_Columns[i];

			//highlight heading if...
			//if adding this column has taken us over fM_Mouse_X, we know this column is selected
			if ((MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fWidth > fM_Mouse_X)
				&&	(fM_Mouse_X > MENU3D_X_START_OFFSET)						//cursor not left of menu
				&& (fM_Mouse_Y > MENU3D_Y_START_OFFSET)							//cursor not above menu
				&& (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT))	//cursor not below menu
			{
				//if a previous heading was highlighted, unhighlight it
				if (nM_Current_Heading != -1)
					pM_Columns[nM_Current_Heading].bHighlighted = FALSE;
				//set the current heading to the one the cursor is over
				nM_Current_Heading = i;
				//highlight the new heading
				pM_Columns[nM_Current_Heading].bHighlighted = TRUE;
				return;
			}
			//cursor is not over this heading, so increment the width count with this one
			heading_width_count += psMenuCol->fWidth;
		}
		//unhighlight heading if...
		if ((fM_Mouse_Y < MENU3D_Y_START_OFFSET)							//cursor above menu
			|| (fM_Mouse_Y > MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT)		//cursor below menu
			|| (fM_Mouse_X < MENU3D_X_START_OFFSET)							//cursor left of menu
			|| (fM_Mouse_X > MENU3D_X_START_OFFSET + heading_width_count))	//cursor right of menu
		{
			//if a previous heading was highlighted, unhighlight it
			if (nM_Current_Heading != -1)
				pM_Columns[nM_Current_Heading].bHighlighted = FALSE;
			//set no heading selected
			nM_Current_Heading = -1;
		}
		return;
	}

	// RIGHT MENU
	if ((message == WM_LBUTTONDOWN) && bM_Menu_On)
	{
		nAbsoluteMouseX  = (int)((fM_Mouse_X  * 6.4f));
		
		/* Look if cursor is on right hand menu and send a message to the user if so */
		if(D3DFE.bDisplay3DMenuIcons && dwCurrentWidth>(fM_title_bar_size+160))
		{
			/* F1 */
			if ((dwCurrentWidth-123 > nAbsoluteMouseX && dwCurrentWidth-158 < nAbsoluteMouseX)
					&& (fM_Mouse_Y > MENU3D_Y_START_OFFSET)
					&& (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT))
			{
				PostMessage(D3DFE.hwnd, WM_KEYDOWN, VK_F1, 0);
				
			}

			/* F2 */
			if ((dwCurrentWidth-89 > nAbsoluteMouseX && dwCurrentWidth-123 < nAbsoluteMouseX)
					&& (fM_Mouse_Y > MENU3D_Y_START_OFFSET)
					&& (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT))
			{
				PostMessage(D3DFE.hwnd, WM_KEYDOWN, VK_F2, 0);
				
			}

			/* F3 */
			if ((dwCurrentWidth-52 > nAbsoluteMouseX && dwCurrentWidth-89 < nAbsoluteMouseX)
					&& (fM_Mouse_Y > MENU3D_Y_START_OFFSET)
					&& (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT))
			{
				PostMessage(D3DFE.hwnd, WM_KEYDOWN, VK_F3, 0);
			}
		}

		/* ESC */
		if (dwCurrentWidth>(fM_title_bar_size+54))
		{
			if ((dwCurrentWidth > nAbsoluteMouseX && dwCurrentWidth-52 < nAbsoluteMouseX)
					&& (fM_Mouse_Y > MENU3D_Y_START_OFFSET)
					&& (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT))
			{
				PostMessage(D3DFE.hwnd, WM_COMMAND, ID_FILE_EXIT, 0);
			}
		}
	}

	
	//MENU OFF, MOUSE BUTTON	(2/4)
	/****************
	** Open column **
	****************/
	if ((bM_In_Menu == FALSE)			//no menu selected yet
		&& (message == WM_LBUTTONDOWN))	//left button click
	{
		for (i=0; i<nM_Num_Columns; i++)
		{
			psMenuCol = &pM_Columns[i];

			//if adding this column has taken us over fM_Mouse_X, we know this column is selected
			if ((MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fWidth > fM_Mouse_X)
				&& (fM_Mouse_Y > MENU3D_Y_START_OFFSET)
				&& (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT))
			{
				//we have now clicked a heading so we're in a menu
				bM_In_Menu = TRUE;
				dwM_col_bg_trans_menu		= 0x00000000;
				break;
			}
			else
			{
				//cursor is not over this heading, so increment the width count with this one
				heading_width_count += psMenuCol->fWidth;
			}
		}
		//either we've clicked a heading, or mouse wasn't over one. Either way, return.
		return;
	}


	//MENU ON, MOUSE BUTTON	(3/4)
	/*******************************
	** Close column/Activate Item **
	*******************************/
	if ((bM_In_Menu == TRUE)				//menu selected 
		&& (message == WM_LBUTTONDOWN  ||  	//left button click
		(message == WM_LBUTTONUP && dwM_col_bg_trans_menu==0xFF000000)))	 // to be able to select holding down and un-holding.
	{
		//for all columns
		for (i=0; i<nM_Num_Columns; i++)
		{
			psMenuCol = &pM_Columns[i];

			//if this column is highlighted
			if (psMenuCol->bHighlighted == TRUE)
			{
				//activate item if...
				//cursor within this columns item X
				if ((fM_Mouse_X > MENU3D_X_START_OFFSET + heading_width_count) 
					&& (fM_Mouse_X < MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fItem_width))
				{
					//if this column isn't empty
					if (pM_Num_Items[nM_Current_Heading] > 0)
					{
						//if this item isn't ghosted
						if (pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bGhosted == FALSE)
						{
							//call handler with this ID
							PostMessage(D3DFE.hwnd, WM_COMMAND, pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].uIdentifier, 0);
						}
					}
					//reset menus
					D3DFEReset3DMenus();
					return;
				}

				//activate popup if...
				//we have a current item
				if (nM_Current_Item != -1)
				//if this item has a popup
				{
					if (pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bSub_menu)
					{
						//if cursor in X-bound of sub-item
						if ((fM_Mouse_X > MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fItem_width) 
						&& (fM_Mouse_X < MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fItem_width + pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].fItem_width))
						{
							//only activate if not ghosted
							if (pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bGhosted == FALSE)
							{
								//call handler with this ID
								PostMessage(D3DFE.hwnd, WM_COMMAND, pM_Sub_Columns[pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].uIdentifier, 0);
							}
							//reset menus
							D3DFEReset3DMenus();
							return;
						}
					}
				}

				//close column if...
				if ((fM_Mouse_X < MENU3D_X_START_OFFSET + heading_width_count)		//cursor left of item
					|| (fM_Mouse_X > MENU3D_X_START_OFFSET + heading_width_count	//cursor right of item
					+ psMenuCol->fItem_width)
					|| (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT		//cursor above item menu
					+ MENU3D_HEADING_OFFSET)
					|| (fM_Mouse_Y > MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT		//cursor below item menu
					+ MENU3D_HEADING_OFFSET + separator_offset + (MENU3D_ELEMENT_HEIGHT * pM_Num_Items[nM_Current_Heading])))
				{
					D3DFEReset3DMenus();
					return;
				}

			}
			//increment heading width count with this column
			heading_width_count += psMenuCol->fWidth;
		}
		return;
	}


	//MENU ON, MOUSE MOVE	(4/4)
	/*******************************************
	** Highlight/Unhighlight item/Open Column **
	*******************************************/
	if ((bM_In_Menu == TRUE)			//menu selected
		&& (message == WM_MOUSEMOVE))	//mouse movement
	{
		//for all columns
		for (i=0; i<nM_Num_Columns; i++)
		{
			psMenuCol = &pM_Columns[i];

			//open column if...
			//if adding this column has taken us over fM_Mouse_X, we know this column is selected
			if ((MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fWidth > fM_Mouse_X)
				&& (fM_Mouse_Y > MENU3D_Y_START_OFFSET)
				&& (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT))
			{
				//changing column so unhighlight current column
				if (nM_Current_Heading != -1)
				{
					if(psMenuCol->bHighlighted == FALSE)	dwM_col_bg_trans_menu = 0x00000000;
					pM_Columns[nM_Current_Heading].bHighlighted = FALSE;
				}
				//unhighlight current item
				if (nM_Current_Item != -1)
					pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
				//set new column
				nM_Current_Heading = i;
				//highlight new column
				pM_Columns[nM_Current_Heading].bHighlighted = TRUE;

				return;
			}
			//cursor is not over this heading, so increment the width count with this one
			heading_width_count += psMenuCol->fWidth;
		}


		//we're not over a heading so reset heading width count
		heading_width_count = 0;
		//highlight item if...
		//for all columns
		for (i=0; i<nM_Num_Columns; i++)
		{
			psMenuCol = &pM_Columns[i];

			//if this column is highlighted
			if (psMenuCol->bHighlighted == TRUE)
			{
				//if cursor in X-bound of item
				if ((fM_Mouse_X > MENU3D_X_START_OFFSET + heading_width_count) 
					&& (fM_Mouse_X < MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fItem_width))
				{
					//for all items in this column
					for (j=0; j<pM_Num_Items[i]; j++)
					{
						//if adding this item has taken us over cursor position, we know this item is selected
						if ((MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*(j+1) + separator_offset > fM_Mouse_Y)
							&& (fM_Mouse_Y > MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET))
						{
							if(pM_Columns[nM_Current_Heading].pElement[j].bHighlighted==FALSE)
								dwM_col_bg_trans_popup = 0x00000000; 
						
							//if we have a current item, unhighlight it
							if (nM_Current_Item != -1)
								pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;

							//if we have a current sub item, unhighlight it and set it null
							if ((nM_Current_Sub_Item != -1) && (nM_Current_Item != -1))
							{
								pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
								nM_Current_Sub_Item = -1;
								bM_In_Pop_Up = FALSE;
							}
							//set the new current item
							nM_Current_Item = j;
							//highlight the new current item
							pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = TRUE;
							//set not in popup
							bM_In_Pop_Up = FALSE;
							return;
						}
						//we're not over cursor yet, so lets include this item's separator
						if (psMenuCol->pElement[j].bSeparator)
						{
							separator_offset += MENU3D_ELEMENT_HEIGHT;
						}
						//if we're over the cursor it's because of the separator, so highlight nothing
						if (MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*(j+1) + separator_offset > fM_Mouse_Y)
						{
							if(nM_Current_Item != -1)
								pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
							nM_Current_Item = -1;
							nM_Current_Sub_Item = -1;
							bM_In_Pop_Up = FALSE;
							return;
						}
					}
				}

				//highlight popup if...
				//if there is a current item
				if (nM_Current_Item != -1)
				{
					//if this item has a sub-menu
					if (pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bSub_menu)
					{
						//if cursor in X-bound of sub-item
						if ((fM_Mouse_X > MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fItem_width) 
						&& (fM_Mouse_X < MENU3D_X_START_OFFSET + heading_width_count + psMenuCol->fItem_width + pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].fItem_width))
						{
							//work out separator offset
							for (s=0; s<nM_Current_Item; s++)
							{
								if (psMenuCol->pElement[s].bSeparator)
								{
									separator_offset += MENU3D_ELEMENT_HEIGHT;
								}
							}

							//unhighlight popup item if above popup menu
							if ((fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*nM_Current_Item + separator_offset) && bM_In_Pop_Up)
							{
								pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
								return;
							}

							//for all items in the sub menu
							for (k=0; k<pM_Num_Sub_Items[psMenuCol->pElement[nM_Current_Item].nSub_menu_num]; k++)
							{
								//if adding this sub-item has taken us over cursor position, we know this sub-item is selected
								if (MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*(nM_Current_Item+1) + separator_offset + popup_separator_offset + MENU3D_ELEMENT_HEIGHT*(k) > fM_Mouse_Y)
								{
									//if we have a current sub item, unhighlight it
									if (nM_Current_Sub_Item != -1)
										pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
									//set the new current item
									nM_Current_Sub_Item = k;
									//highlight the new current item
									pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].pElement[k].bHighlighted = TRUE;
									//set that we're in a popup
									bM_In_Pop_Up = TRUE;
									return;
								}
								//we're not over cursor yet, so lets include this popup item's separator (if it has one)
								if (pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].pElement[k].bSeparator)
								{
									popup_separator_offset += MENU3D_ELEMENT_HEIGHT;
								}
								//if we're over the cursor it's because of the separator, so highlight nothing
								if (MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*(nM_Current_Item+1) + separator_offset + popup_separator_offset + MENU3D_ELEMENT_HEIGHT*(k) > fM_Mouse_Y)
								{
									//if we have a current sub item, unhighlight it
									if (nM_Current_Sub_Item != -1)
										pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
									//set that we're in a popup
									bM_In_Pop_Up = TRUE;
									return;
								}
							}
						}
					}
				}

				
				//unhighlight item if...
				//if we have a current item
				if (nM_Current_Item != -1)
				{
					if ((fM_Mouse_X < MENU3D_X_START_OFFSET + heading_width_count)		//cursor left of item
						|| (fM_Mouse_X > MENU3D_X_START_OFFSET + heading_width_count	//cursor right of item
						+ psMenuCol->fItem_width)
						|| (fM_Mouse_Y < MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT		//cursor above item menu
						+ MENU3D_HEADING_OFFSET)
						|| (fM_Mouse_Y > MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT		//cursor below item menu
						+ MENU3D_HEADING_OFFSET + (MENU3D_ELEMENT_HEIGHT * pM_Num_Items[nM_Current_Heading]) + separator_offset))
					{
						//if we're in a popup
						if (bM_In_Pop_Up)
						{
							//unhighlight it
							pM_Sub_Columns[psMenuCol->pElement[nM_Current_Item].nSub_menu_num].pElement[nM_Current_Sub_Item].bHighlighted = FALSE;
							//set no sub item
							nM_Current_Sub_Item = -1;
							//set not in popup
							//bM_In_Pop_Up = FALSE;
						}
						//not in popup so...
						else
						{
							//unhighlight current item
							pM_Columns[nM_Current_Heading].pElement[nM_Current_Item].bHighlighted = FALSE;
							//set no current item
							nM_Current_Item		= -1;
						}
						return;
					}


				}
			}	

			heading_width_count += psMenuCol->fWidth;
		}
		return;
	}
}


 /*******************************************************************************
 * Function Name  : D3DFEUpdate3DMenus
 * Global Used    : D3DFE, nNumAdapters
 * Description    : Tick required items.
 *					Ghost items if not supported.
 *					Set window positions and colours
 *					Activate required windows
 *******************************************************************************/
static void D3DFEUpdate3DMenus()
{
	int			i, j, k;
	float		separator_offset		= 0.0f;	//increased by element height each time separator found
	float		popup_separator_offset= 0.0f;	//increased by element height each time popup separator found
	float		heading_width_count	= 0.0f;		//increased by width of each column as it's added
	int			dwCurrentWidth, dwCurrentHeight; 
	int			nFadeIncr, nCurrentFadeSpeed;
	int			nCurrentMenuTrans, nCurrentPopupTrans;
	DWORD		dwColBgSel, dwColFontNormal, dwColBgNotSel, dwColFontGhost;
	MENU_COLUMN	*psMenuCol, *psMenuColPopup;
	MENU_ITEM	*psMenuItem, *psMenuItemPopup;

	if (!bM_Menu_On)	return;


	/*******************************************/
	/* Use time-based fading out for the menus */
	/*******************************************/
	/* Get the current value (between (0 and 255) */
	nCurrentMenuTrans  = dwM_col_bg_trans_menu >> 24;
	nCurrentPopupTrans = dwM_col_bg_trans_popup >> 24;

	/* Get the current performance and caculate the increment */
	if(D3DFE.fFrameRate>0) nCurrentFadeSpeed = (int)(MENU3D_FADE_SPEED/(D3DFE.fFrameRate+0.001f));
		else nCurrentFadeSpeed = (int)(MENU3D_FADE_SPEED/60.0f);
	if(nCurrentFadeSpeed > 255) nCurrentFadeSpeed = 255;
	nFadeIncr = nCurrentFadeSpeed & 0x000000FF;

	/* Add it to the current translucency value */
	if(nCurrentMenuTrans!=0xFF)		nCurrentMenuTrans  += nFadeIncr;
	if(nCurrentPopupTrans!=0xFF)	nCurrentPopupTrans += nFadeIncr;

	/* If it is already 0xFF do nothing otherwise update trans variable */
	if(nCurrentMenuTrans>0xFF)		dwM_col_bg_trans_menu = 0xFF000000;
	else							dwM_col_bg_trans_menu = (nCurrentMenuTrans<<24);

	if(nCurrentPopupTrans>0xFF)		dwM_col_bg_trans_popup = 0xFF000000;
	else							dwM_col_bg_trans_popup = (nCurrentPopupTrans<<24);


	if (D3DFE.bFullScreen)
	{
		dwCurrentWidth =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width;
		dwCurrentHeight =	Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height;
	}
	else
	{
		dwCurrentWidth =	D3DFE.dwWindowWidth;
		dwCurrentHeight =	D3DFE.dwWindowHeight;
	}

	/*****************
	** Devices menu **
	*****************/
	/* Adapters */
	for (i=0; i<nNumAdapters; i++)
	{
		D3DShellTick3DMenus(ID_DEVICES_ADAPTER0+i, i == D3DFE.nCurrentAdapter ? TRUE: FALSE);
	}

	/* 3D devices */
	D3DShellTick3DMenus(ID_DEVICES_D3DDEVICE_HAL, (D3DFE.CurrentDeviceType==D3DDEVTYPE_HAL && !D3DFE.bUsePureDevice) ? TRUE : FALSE);
	D3DShellTick3DMenus(ID_DEVICES_D3DDEVICE_HALPURE, (D3DFE.CurrentDeviceType==D3DDEVTYPE_HAL && D3DFE.bUsePureDevice) ? TRUE : FALSE);
	D3DShellTick3DMenus(ID_DEVICES_D3DDEVICE_SW, D3DFE.CurrentDeviceType==D3DDEVTYPE_SW ? TRUE : FALSE);
	D3DShellTick3DMenus(ID_DEVICES_D3DDEVICE_REF, D3DFE.CurrentDeviceType==D3DDEVTYPE_REF ? TRUE : FALSE);
	D3DShellTick3DMenus(ID_DEVICES_SWVERTEXPROCESSING, D3DFE.nVPType==SW ? TRUE : FALSE);
	D3DShellTick3DMenus(ID_DEVICES_HWVERTEXPROCESSING, D3DFE.nVPType==HW ? TRUE : FALSE);
	D3DShellTick3DMenus(ID_DEVICES_MIXEDVERTEXPROCESSING, D3DFE.nVPType==MIXED ? TRUE : FALSE);

	/*******************
	** Buffering menu **
	*******************/
	/* VSync */
	D3DShellTick3DMenus(ID_BUFFERING_VSYNC, D3DFE.bVSyncOn ? TRUE : FALSE);

	/* Multisampling */
	D3DShellTick3DMenus(ID_BUFFERING_MS_NONE, D3DFE.MultiSamplingType == D3DMULTISAMPLE_NONE ? TRUE : FALSE);
	for (i=2; i<=16; i++)
	{
		D3DShellTick3DMenus(ID_BUFFERING_MS_NONE + i, i == D3DFE.MultiSamplingType ? TRUE : FALSE);
	}


	/* Z-Buffer */
	D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER, FALSE);
	D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER, D3DFE.bZBufferNeeded ? TRUE : FALSE);

	/* Is ZBuffer OFF ? */
	if (!D3DFE.bZBufferNeeded)
	{
		/* No ZBuffer is selected so gray all ZBuffer choices */
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D16L,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D16L,		TRUE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D32,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D32,		TRUE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D15S1,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D15S1,	TRUE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24S8,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D24S8,	TRUE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D16,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D16,		TRUE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24X8,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D24X8,	TRUE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24X4S4,	FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D24X4S4,	TRUE);
	}
	else
	{
		/* Be sure all ZBuffer choices are enabled and unchecked */
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D16L,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D16L,		FALSE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D32,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D32,		FALSE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D15S1,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D15S1,	FALSE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24S8,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D24S8,	FALSE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D16,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D16,		FALSE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24X8,		FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D24X8,	FALSE);
		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24X4S4,	FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_ZBUFFER_D24X4S4,	FALSE);
		
		/* Tick current ZBuffer format */
		switch (D3DFE.CurrentZBufferFormat)
		{
			case D3DFMT_D16_LOCKABLE:	D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D16L,		TRUE); break;
			case D3DFMT_D32:			D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D32,		TRUE); break;
			case D3DFMT_D15S1:			D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D15S1,		TRUE); break;
			case D3DFMT_D24S8:			D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24S8,		TRUE); break;
			case D3DFMT_D16:			D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D16,		TRUE); break;
			case D3DFMT_D24X8:			D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24X8,		TRUE); break;
			case D3DFMT_D24X4S4:		D3DShellTick3DMenus(ID_BUFFERING_ZBUFFER_D24X4S4,	TRUE); break;
		}
	}

	/* Buffering modes */
	/* First uncheck them all */
	D3DShellTick3DMenus(ID_BUFFERING_DOUBLEBUFFER, FALSE);
	D3DShellTick3DMenus(ID_BUFFERING_TRIPLEBUFFER, FALSE);
	
	/* Buffering modes are only available in FullScreen */
	if (D3DFE.bFullScreen)
	{
		D3DShellGhost3DMenus(ID_BUFFERING_DOUBLEBUFFER, FALSE);
		D3DShellGhost3DMenus(ID_BUFFERING_TRIPLEBUFFER, FALSE);
		
		/* Tick current choice */
		switch (D3DFE.nNumBackBuffer)
		{
			case	1 :	D3DShellTick3DMenus(ID_BUFFERING_DOUBLEBUFFER, TRUE); break;
			case	2 :	D3DShellTick3DMenus(ID_BUFFERING_TRIPLEBUFFER, TRUE); break;
		}
	}
	else
	{
		/* Disable buffering menu in window mode */
		D3DShellGhost3DMenus(ID_BUFFERING_DOUBLEBUFFER, TRUE);
		D3DShellGhost3DMenus(ID_BUFFERING_TRIPLEBUFFER, TRUE);
	}
		
	/* Clear viewport mode */
	D3DShellTick3DMenus(ID_BUFFERING_CLEAR, D3DFE.bClearViewport ? TRUE : FALSE);


	/***************
	** Modes menu **
	***************/
	/* Uncheck all resolutions in Modes menu */
	for (i=0; i<Adapter[D3DFE.nCurrentAdapter].nNumberOfDisplayModes; i++)
	{
		D3DShellTick3DMenus(ID_MODES_RES0+i, FALSE);
	}

	/* FullScreen or not */
	if (D3DFE.bFullScreen)
	{
		/* Tick Fullscreen box */
		D3DShellTick3DMenus(ID_MODES_FULLSCREEN, TRUE);
		
		/* Check current resolution */
		D3DShellTick3DMenus(ID_MODES_RES0+D3DFE.nCurrentResolution, TRUE);
	}
	else
	{
		/* Not in FullScreen mode -> Uncheck item */
		D3DShellTick3DMenus(ID_MODES_FULLSCREEN, FALSE);
	}

	/* Uncheck all back buffer formats in Modes menu */
	for (i=0; i<6; i++)
	{
		D3DShellTick3DMenus(ID_MODES_FORMAT_X8R8G8B8+i,	FALSE);
	}
	/* Tick current back buffer format */
	switch(D3DFE.CurrentBackBufferFormat)
	{
		case D3DFMT_X8R8G8B8 :		D3DShellTick3DMenus(ID_MODES_FORMAT_X8R8G8B8, TRUE); break;
		case D3DFMT_A8R8G8B8 :		D3DShellTick3DMenus(ID_MODES_FORMAT_A8R8G8B8, TRUE); break;
		case D3DFMT_A2R10G10B10 :	D3DShellTick3DMenus(ID_MODES_FORMAT_A2R10G10B10, TRUE); break;
		case D3DFMT_X1R5G5B5 :		D3DShellTick3DMenus(ID_MODES_FORMAT_X1R5G5B5, TRUE); break;
		case D3DFMT_A1R5G5B5 :		D3DShellTick3DMenus(ID_MODES_FORMAT_A1R5G5B5, TRUE); break;
		case D3DFMT_R5G6B5 :		D3DShellTick3DMenus(ID_MODES_FORMAT_R5G6B5, TRUE); break;
	}

	/* Only perform the following if refresh rates menu is used */
	if (D3DFE.bUseRefreshRates)
	{
		/* Uncheck all refresh rates in Modes menu */
		for (i=0; i<9; i++) 
		{
			D3DShellTick3DMenus(ID_MODES_FREQUENCY_60 + i , FALSE);
		}

		/* Refresh rates are only available in fullscreen mode */
		if (D3DFE.bFullScreen)
		{
			D3DFORMAT	DisplayModeFormat;
			DWORD		RRTable[9] = { 60, 70, 72, 75, 85, 90, 100, 120, 150 };

			/* Enable all refresh rates */
			for (i=0; i<9; i++) 
			{
				D3DShellGhost3DMenus(ID_MODES_FREQUENCY_60 + i, FALSE);
			}

			/* Get current display mode format */
			if (D3DFEGetBPP(Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Format)==32) 
			{
				DisplayModeFormat=D3DFMT_X8R8G8B8;
			}
			else 
			{
				DisplayModeFormat=D3DFMT_R5G6B5;
			}

			/* Gray refresh rates not supported at current resolution */
			for (i=0; i<9; i++)
			{
				if (D3DFEPickRefreshRate(D3DFE.nCurrentAdapter, Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Width, 
									 Adapter[D3DFE.nCurrentAdapter].Mode[D3DFE.nCurrentResolution].Height, DisplayModeFormat, RRTable[i])!=RRTable[i])
				{
					D3DShellGhost3DMenus(ID_MODES_FREQUENCY_60 + i, TRUE);
				}
			}

			/* Tick current refresh rate */
			switch(D3DFE.dwCurrentRefreshRate)
			{
				case 60:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_60 , TRUE);	break;
				case 70:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_70 , TRUE);	break;
				case 72:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_72 , TRUE);	break;
				case 75:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_75 , TRUE);	break;
				case 85:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_85 , TRUE);	break;
				case 90:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_90 , TRUE);	break;
				case 100:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_100 , TRUE);	break;
				case 120:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_120 , TRUE);	break;
				case 150:	D3DShellTick3DMenus(ID_MODES_FREQUENCY_150 , TRUE);	break;
			}
		}
	}


	/**********************
	** Update 3D Windows **
	**********************/

	for (i=0; i<nM_Num_Columns; i++)
	{
		psMenuCol = &pM_Columns[i];

		//set the width of this heading
		psMenuCol->fWidth = D3DTPrint3DGetLength (MENU3D_FONT_SIZE_HEADING, (BYTE *)psMenuCol->pszTitle) + 2.0f;

		if (psMenuCol->bHighlighted == TRUE)
			D3DTPrint3DSetWindow(psMenuCol->dwNumber, MENU3D_COL_BG_HEADING, MENU3D_COL_FONT_SELECTED,
				MENU3D_FONT_SIZE_HEADING, MENU3D_X_START_OFFSET + heading_width_count,
				MENU3D_Y_START_OFFSET, psMenuCol->fWidth, MENU3D_HEADING_HEIGHT);
		else
			D3DTPrint3DSetWindow(psMenuCol->dwNumber, MENU3D_COL_BG_HEADING_NS, MENU3D_COL_FONT_HEADING,
				MENU3D_FONT_SIZE_HEADING, MENU3D_X_START_OFFSET + heading_width_count,
				MENU3D_Y_START_OFFSET, psMenuCol->fWidth, MENU3D_HEADING_HEIGHT);

		//if this column highlighted, set its items
		if (psMenuCol->bHighlighted == TRUE && bM_In_Menu == TRUE)
		{
			//set each item in column 
			for (j=0; j<pM_Num_Items[i]; j++)
			{
				psMenuItem = &psMenuCol->pElement[j];

				dwColBgSel		= (MENU3D_COL_BG_SELECTED & 0x00FFFFFF) | dwM_col_bg_trans_menu;
				dwColFontNormal	= (MENU3D_COL_FONT_NORMAL & 0x00FFFFFF) | dwM_col_bg_trans_menu;
				dwColBgNotSel	= (MENU3D_COL_BG_NOTSELECTED & 0x00FFFFFF) | dwM_col_bg_trans_menu;
				dwColFontGhost	= (MENU3D_COL_FONT_GHOST & 0x00FFFFFF) | dwM_col_bg_trans_menu;

				if (psMenuItem->bHighlighted)
				{
					if(psMenuItem->bGhosted)
					{
						//highlighted, ghosted
						D3DTPrint3DSetWindow (psMenuItem->dwNumber, dwColBgSel, dwColFontGhost, MENU3D_FONT_SIZE_ELEMENT,
							MENU3D_X_START_OFFSET+heading_width_count, 
							(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset, 
							psMenuCol->fItem_width, MENU3D_ELEMENT_HEIGHT);
					}
					else
					{
						//highlighted, not ghosted
						D3DTPrint3DSetWindow (psMenuItem->dwNumber, dwColBgSel, MENU3D_COL_FONT_SELECTED, MENU3D_FONT_SIZE_ELEMENT, 
							MENU3D_X_START_OFFSET+heading_width_count,
							(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset, 
							psMenuCol->fItem_width, MENU3D_ELEMENT_HEIGHT);
					}

					//update popup menu if there is one on this item
					if (psMenuItem->bSub_menu)
					{
						psMenuColPopup = &pM_Sub_Columns[psMenuItem->nSub_menu_num];

						dwColBgSel		= (MENU3D_COL_BG_SELECTED & 0x00FFFFFF) | dwM_col_bg_trans_popup;
						dwColFontNormal	= (MENU3D_COL_FONT_NORMAL & 0x00FFFFFF) | dwM_col_bg_trans_popup;
						dwColBgNotSel	= (MENU3D_COL_BG_NOTSELECTED & 0x00FFFFFF) | dwM_col_bg_trans_popup;
						dwColFontGhost	= (MENU3D_COL_FONT_GHOST & 0x00FFFFFF) | dwM_col_bg_trans_popup;

						//run through each pop-up menu item
						for (k=0; k<pM_Num_Sub_Items[psMenuItem->nSub_menu_num]; k++)
						{
							psMenuItemPopup = &psMenuColPopup->pElement[k];

							if(psMenuItemPopup->bHighlighted)
							{
								if(psMenuItemPopup->bGhosted)
								{
									//highlighted, ghosted
									D3DTPrint3DSetWindow (psMenuItemPopup->dwNumber, 
										dwColBgSel, dwColFontGhost, MENU3D_FONT_SIZE_ELEMENT, MENU3D_X_START_OFFSET+heading_width_count + psMenuCol->fItem_width + MENU3D_POPUP_OFFSET, 
										(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset + popup_separator_offset + (MENU3D_ELEMENT_HEIGHT*k), 
										psMenuColPopup->fItem_width, MENU3D_ELEMENT_HEIGHT);
								}
								else
								{
									//highlighted, not ghosted
									D3DTPrint3DSetWindow (psMenuItemPopup->dwNumber, 
										dwColBgSel, MENU3D_COL_FONT_SELECTED, MENU3D_FONT_SIZE_ELEMENT, MENU3D_X_START_OFFSET+heading_width_count + psMenuCol->fItem_width + MENU3D_POPUP_OFFSET, 
										(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset + popup_separator_offset + (MENU3D_ELEMENT_HEIGHT*k), 
										psMenuColPopup->fItem_width, MENU3D_ELEMENT_HEIGHT);
								}
							}
							else
							{
								if(psMenuItemPopup->bGhosted)
								{
									//not highlighted, ghosted
									D3DTPrint3DSetWindow (psMenuItemPopup->dwNumber, 
										dwColBgNotSel, dwColFontGhost, MENU3D_FONT_SIZE_ELEMENT, MENU3D_X_START_OFFSET+heading_width_count + psMenuCol->fItem_width + MENU3D_POPUP_OFFSET, 
										(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset + popup_separator_offset + (MENU3D_ELEMENT_HEIGHT*k), 
										psMenuColPopup->fItem_width, MENU3D_ELEMENT_HEIGHT);
								}
								else
								{
									//not highlighted, not ghosted
									D3DTPrint3DSetWindow (psMenuItemPopup->dwNumber, 
										dwColBgNotSel, dwColFontNormal, MENU3D_FONT_SIZE_ELEMENT, MENU3D_X_START_OFFSET+heading_width_count + psMenuCol->fItem_width + MENU3D_POPUP_OFFSET, 
										(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset + popup_separator_offset + (MENU3D_ELEMENT_HEIGHT*k), 
										psMenuColPopup->fItem_width, MENU3D_ELEMENT_HEIGHT);
								}
							}

							//add separator to popup if needed
							if (psMenuItemPopup->bSeparator)
							{
								D3DTPrint3DSetWindow (psMenuItemPopup->nSeparator_num, 
									dwColBgNotSel, dwColFontNormal, MENU3D_FONT_SIZE_ELEMENT, MENU3D_X_START_OFFSET+heading_width_count + psMenuCol->fItem_width + MENU3D_POPUP_OFFSET, 
									(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset + popup_separator_offset + MENU3D_ELEMENT_HEIGHT + (MENU3D_ELEMENT_HEIGHT*k), 
									psMenuColPopup->fItem_width, MENU3D_ELEMENT_HEIGHT);
								popup_separator_offset += MENU3D_ELEMENT_HEIGHT;
							}
						}
					}
				}
				else
				{
					if(psMenuItem->bGhosted)
					{
						//not highlighted, not ghosted
						D3DTPrint3DSetWindow (psMenuItem->dwNumber, 
							dwColBgNotSel, dwColFontGhost, MENU3D_FONT_SIZE_ELEMENT, MENU3D_X_START_OFFSET+heading_width_count, 
							(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset, 
							psMenuCol->fItem_width, MENU3D_ELEMENT_HEIGHT);
					}
					else
					{
						//not highlighted, not ghosted
						D3DTPrint3DSetWindow (psMenuItem->dwNumber, 
							dwColBgNotSel, dwColFontNormal, MENU3D_FONT_SIZE_ELEMENT, MENU3D_X_START_OFFSET+heading_width_count, 
							(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset, 
							psMenuCol->fItem_width, MENU3D_ELEMENT_HEIGHT);
					}
				}

				//add separator if needed
				if (psMenuItem->bSeparator)
				{
					D3DTPrint3DSetWindow (psMenuItem->nSeparator_num, 
						dwColBgNotSel, dwColFontNormal, MENU3D_FONT_SIZE_ELEMENT, MENU3D_X_START_OFFSET+heading_width_count, 
						(MENU3D_Y_START_OFFSET + MENU3D_HEADING_HEIGHT + MENU3D_HEADING_OFFSET + MENU3D_ELEMENT_HEIGHT*j) + separator_offset + MENU3D_ELEMENT_HEIGHT, 
						psMenuCol->fItem_width, MENU3D_ELEMENT_HEIGHT);
					separator_offset += MENU3D_ELEMENT_HEIGHT;
				}
			}
		}
		//add the width of the current column to running total
		heading_width_count += psMenuCol->fWidth;
	}
	

	//update title bar
	fM_title_bar_size = (MENU3D_X_START_OFFSET + heading_width_count) * (640.0f/100.0f);

	D3DTPrint3DSetWindow(dwM_title_bar, MENU3D_COL_BG_HEADING_NS, 0xFFD0D0FF,//MENU3D_COL_FONT_HEADING, 
		MENU3D_FONT_SIZE_HEADING, MENU3D_X_START_OFFSET + heading_width_count,
		MENU3D_Y_START_OFFSET,
		dwCurrentWidth * (100.0f/640.0f),
		MENU3D_HEADING_HEIGHT);

	D3DTPrint3DSetWindowFlags (dwM_title_bar,  PRINT3D_ACTIVATE_WIN);

	/* turn on all windows */
	//for every column
	for (i=0; i<nM_Num_Columns; i++)
	{
		psMenuCol = &pM_Columns[i];

		//activate heading
		D3DTPrint3DSetWindowFlags(psMenuCol->dwNumber,  PRINT3D_ACTIVATE_WIN);

		//for every item
		for (j=0; j<pM_Num_Items[i]; j++)
		{
			psMenuItem = &psMenuCol->pElement[j];

			//activate item
			D3DTPrint3DSetWindowFlags(psMenuItem->dwNumber,  PRINT3D_ACTIVATE_WIN);

			//if it has a separator, activate it
			if (psMenuItem->bSeparator)
				D3DTPrint3DSetWindowFlags(psMenuItem->nSeparator_num,  PRINT3D_ACTIVATE_WIN);

			//if it's highlighted, and it has a sub menu
			if (psMenuItem->bSub_menu && psMenuItem->bHighlighted)
			{
				psMenuColPopup = &pM_Sub_Columns[psMenuItem->nSub_menu_num];

				//run through each pop-up menu item
				for (k=0; k<pM_Num_Sub_Items[psMenuItem->nSub_menu_num]; k++)
				{
					psMenuItemPopup = &psMenuColPopup->pElement[k];

					//activate it
					D3DTPrint3DSetWindowFlags(psMenuItemPopup->dwNumber, PRINT3D_ACTIVATE_WIN);
					//if it has a separator, activate it
					if (psMenuItemPopup->bSeparator)
						D3DTPrint3DSetWindowFlags(psMenuItemPopup->nSeparator_num,  PRINT3D_ACTIVATE_WIN);
				}		
			}
		}
	}
}

/*******************************************************************************
 * Function Name  : D3DFERelease3DMenus
 * Returns        : Nothing
 * Global Used    : D3DFE
 * Description    : Release all memory used by 3D menus (done once only) 
 *******************************************************************************/
static void D3DFERelease3DMenus()
{
	int i, j;

	/* Free 3D Menu Variables */
	free(pM_Num_Sub_Items);
	free(pM_Num_Items);

	for (j=0; j<nM_Num_Sub_Columns; j++)
		free(pM_Sub_Columns[j].pElement);

	for (i=0; i<nM_Num_Columns; i++)
		free(pM_Columns[i].pElement);

	free(pM_Sub_Columns);
	free(pM_Columns);

}

/*******************************************************************************
 * Function Name  : D3DFERelease3DMenusWindows
 * Returns        : Nothing
 * Global Used    : D3DFE
 * Description    : Release all memory used by 3D windows (done once only) 
 *******************************************************************************/
static void D3DFERelease3DMenusWindows()
{
	int i=0, j=0;

	
	/* Delete all windows created */
	for (i=0; i<nM_Num_Columns; i++)		
	{
		D3DTPrint3DDeleteWindow(pM_Columns[i].dwNumber);

		for (j=0; j<pM_Num_Items[i]; j++)	
		{
			D3DTPrint3DDeleteWindow(pM_Columns[i].pElement[j].dwNumber);

			if (pM_Columns[i].pElement[j].bSeparator)
			{
				D3DTPrint3DDeleteWindow(pM_Columns[i].pElement[j].nSeparator_num);
			}
		}
	}

	for (i=0; i<nM_Num_Sub_Columns; i++)		
	{
		for (j=0; j<pM_Num_Sub_Items[i]; j++)//create each item in column
		{

			D3DTPrint3DDeleteWindow(pM_Sub_Columns[i].pElement[j].dwNumber);

			if (pM_Sub_Columns[i].pElement[j].bSeparator)//if there is a separator, create it
			{
				D3DTPrint3DDeleteWindow(pM_Sub_Columns[i].pElement[j].nSeparator_num);
			}
		}
	
	}

} 

#endif
/*****************************************************************************
  ShaderX2 : Cloth Animation Demo Program
  Name : D3DCloth.cpp
  Date : 28/03/03
  
  D3DCloth.cpp is an example of a scene to be used with D3DShell. 
  Make sure your application include D3DShell.h to have access to D3DShell 
  functions.

  D3DShell is a program used to make D3D programming easier and less 
  time-consuming. D3DShell takes care of all Direct3D initialisation for the 
  user : handling D3D devices, Fullscreen mode, resolution mode, window modes, 
  buffering modes, Z-Buffer use, viewport creation, viewport clearing, etc...

  Six basic functions have to exist in the scene file to interact correctly with 
  D3DShell. A list of these functions is :

  
  **********************************************************************************
  * void InitApplication(HINSTANCE hInstance, HWND hWindow, char *pszCommandLine); *
  **********************************************************************************

  This function will be called by D3DShell ONCE at the beginning of the D3DShell 
  WinMain() function. This function enables the user to perform any initialisation 
  before any 3D is rendered.
  The application instance and window handle are passed to this function so that the 
  programmer can use them should they needs those.
  From this function the user may call D3DShellSetPreferences() to :
  - Specify the application name.
  - Submit a menu and/or an accelerator table.
  - Specify the icon used for the application.
  - Adjust the behaviour of the application by specifying some flags.

  A prototype of the function is :
	
	void D3DShellSetPreferences(const char		*pszApplicationName, 
								const HMENU		hUserMenuID, 
								const HACCEL	hUserAccel,
								const HICON		hUserIcon, 
								const DWORD		dwFlags);

  A list of flags and their functions can be found in the D3DShell.h file.


  **************************
  * void QuitApplication() *
  **************************

  This function will be called by the D3D Shell just before finishing the program.
  It enables the user to release any memory allocated before.
  

  ******************************************************************************
  * void UserWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) *
  ******************************************************************************

  This function is the user Window Procedure function. It enables the user to retrieve
  menu choices, keystrokes or other messages (WM_TIMER for instance).
  
  D3DShell processes many window messages. If the user needs to 
  process these messages in their application, they should make sure
  NOT to return DefWindowProc() as it will prevent D3DShell WindowProc
  to do its own processing for these messages.
  The window messages processed by D3DShell are :

  WM_ENTERMENULOOP, WM_EXITMENULOOP,
  WM_ACTIVATEAPP,
  WM_SYSCOMMAND,
  WM_SIZE, WM_MOVE, WM_SIZING, WM_MOVING,
  WM_PAINT,
  WM_DESTROY, WM_QUIT

  Note : There are some keys already used by D3DShell as accelerators. You should not
		 remap these keys, as the related functions will not work anymore in that case.
		 The list of keys used by D3DShell is : F4, F5, F6, F12, Alt+ENTER.


  **********************************************
  * BOOL RenderScene(LPDIRECT3DDEVICE9 lpDev9) *
  **********************************************

  This is the main user function in which you have to do your own rendering.
  A pointer to the D3D device (LPDIRECT3DDEVICE9) is passed to this function 
  so you can use it to control your rendering (Renderstates, lights, etc...)
  This function is called every frame by D3DShell and enable the user to
  create the scene for this frame.
  

  ************************************************************************************
  * BOOL InitView(LPDIRECT3D9 lpD3D9, LPDIRECT3DDEVICE9 lpDev9,						 *
  *				  DWORD dwWidth, DWORD dwHeight)									 *
  ************************************************************************************

  This function enables the user to create vertex buffers, materials, set 
  rendering values, load textures, etc... This function will be called each 
  time the surfaces and objects will be recreated (i.e. when a rendering
  variable is changed).
  dwWidth and dwHeight are the current dimensions of the rendering surface.


  **********************
  * void ReleaseView() *
  **********************

  This function enables the user to release any devices they created 
  in the InitView function.
  The function will be called each time a rendering variable is changed.

  
  BUILD INFORMATION:
  ------------------

  Source files :	D3DShell.cpp + D3DShell.h + "Scene file".cpp
  Libraries :		D3D9.LIB + D3DX9.LIB

  
  For more information concerning D3DShell, please read D3DShell.txt.

******************************************************************************/

/*************
** Includes **
*************/
#include <stdio.h>
#include <crtdbg.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "D3DShell.h"	/* D3DShell include file */
#include "D3DTools.h"   /* D3DTools include file */
#include "Scene.h"		/* Scene Geometry Data */
#include "Resource.h"

/* Set to 1 to use GDI menus */
#define USE3DMENUS 0


/****************************************************************************
** Defines
****************************************************************************/

/* Information window Title*/
#define WND_TITLE	"D3DCloth - Cloth Animation Demo"

/****************************************************************************
** Macros
****************************************************************************/
#define FREE(X) { if(X) { free(X); (X) = 0; } }
#define SWAP(x)			(x)=!(x)

#ifndef D3D_RGBA
#define D3D_RGBA(r, g, b, a)   ((DWORD) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))
#endif


/****************************************************************************
** Structures
****************************************************************************/
/* Vertex structures */

// Stream 0 structure - Position
typedef struct D3DPOSVERTEX_TAG
{
	float		x, y, z;
} D3DPOSVERTEX;

// Stream 1 structure - Tex Coord
typedef struct TSUVVERTEX_TAG
{
	float		tu, tv;				
} TSUVVERTEX;

//Stream 2 structure - Normal
typedef struct NORMALVERTEX_TAG
{
	float		nx, ny, nz;				
} NORMALVERTEX;

// Stream Quad Structure - Position and Tex Coord
typedef struct XYZUVVERTEX_TAG
{
	float		x, y, z;
	float		tu, tv;
} XYZUVVERTEX;

// Structure to create geometry
typedef struct 
{
	int						nNumVertex;
	int						nNumFaces;
	D3DMATRIX				mWorld;			/* World-transform for object */
	D3DVECTOR				vPos, vRot;		/* Position of object */
	
	LPDIRECT3DVERTEXBUFFER9	lpVertexBufferXYZ;
	LPDIRECT3DVERTEXBUFFER9	lpVertexBufferTSUV;
	LPDIRECT3DVERTEXBUFFER9	lpVertexBufferNormal;
	LPDIRECT3DINDEXBUFFER9	lpIndexBuffer;

} SModel;

/****************************************************************************
** Constants
****************************************************************************/
const static D3DXVECTOR3 g_UpVector1( 0.0f, 1.0f, 0.0f );  // Up Vector Camera 1
const static D3DXVECTOR3 g_UpVector0( 0.0f, 0.0f, 1.0f );  // Up Vector Camera 0


/****************************************************************************
** Prototypes
****************************************************************************/

// Model Functions
static bool CreateModels(LPDIRECT3DDEVICE9 lpDev9);
static bool RenderModel(LPDIRECT3DDEVICE9 lpDev9, SModel *psMdl, int nStream, bool bCloth);
static bool DestroyModels();

// Texture Functions
static bool LoadTextures(LPDIRECT3DDEVICE9 lpDev9);
static bool CreateTextures(LPDIRECT3DDEVICE9 lpDev9);
static void DestroyTextures();

// Create and Destroy Shaders
static bool CreatePixelShaders(LPDIRECT3DDEVICE9 lpDev9);
static void DestroyPixelShaders();
static bool CreateVertexShaders(LPDIRECT3DDEVICE9 lpDev9);
static void DestroyVertexShaders();

// Init Constants
static bool SetShaderConstants(LPDIRECT3DDEVICE9 lpDev9);

// Render Loop Phases
static bool InitPhase(LPDIRECT3DDEVICE9 lpDev9);
static bool DepthPhase(LPDIRECT3DDEVICE9 lpDev9);
static bool ClothPhase(LPDIRECT3DDEVICE9 lpDev9);
static bool ConstraintPhase(LPDIRECT3DDEVICE9 lpDev9);
static bool NormalMapPhase(LPDIRECT3DDEVICE9 lpDev9);
static bool DisplayPhase(LPDIRECT3DDEVICE9 lpDev9);
static bool DebugView(LPDIRECT3DDEVICE9 lpDev9, int nScene);

// Handle MRTs
static bool SetMRT0TexMRT1RT(LPDIRECT3DDEVICE9 lpDev9);
static bool SetMRT1TexMRT0RT(LPDIRECT3DDEVICE9 lpDev9);
static bool SetMRT0TexMRT2RT(LPDIRECT3DDEVICE9 lpDev9);

// Helper Functions
static bool CheckCaps(LPDIRECT3DDEVICE9 lpDev9);
static void ClGetValue(const char * const szSrc, const char * const szTag, void * const pDest);
static bool LoadState();
static bool RenderScreenAlignedQuad(LPDIRECT3DDEVICE9 lpDev9, float fRenderWidth, float fRenderHeight, float fTextureWidth, float fTextureHeight);
static bool RenderLogo(LPDIRECT3DDEVICE9 lpDev9,float fRenderWidth, float fRenderHeight,float fTextureWidth, float fTextureHeight);

/****************************************************************************
** Globals
****************************************************************************/

/* Object information */
static SModel	g_psMdl[NUM_MESHES];

/* viewport resolution */
static DWORD	dwCurrentWidth, dwCurrentHeight;

/* Initial Camera */
static Struct_Camera *g_psCamera = &Camera[0];
static float fCameraStartX, fCameraStartY, fCameraStartZ;

/* Control Variables */

static bool			g_bPause		= false;
static bool			bFirstTimeRun	= true;
static bool			bDepthUpdate	= false;
static bool			bSupportPS30	= false;
static bool			bSupportVS30	= false;
static bool			bSupportMRTs	= false;
static bool			bWireframe		= false;

static int			nShowScene      = 3; // Which Scene View ? 3 = cloth

static float		fAngle			= 0.0f;
static int			iCount			= 0;
static int			iIterationLoops = 10;

static char			pszFileName[256], pszPSStr[30];
static int			nObjSelected=1, nIterPerFrame=10;
static float		fClothSize, fGravity, fSVA[4], fSVB[4], fSVC[4], fSVD[4], fSVE[4], fSVF[4], fConstraintRange;

/* All-purpose string */
static char			pszTmp[512];		

/* Matrices */
static D3DMATRIX	ProjectionMatrix, ViewMatrix;

// Variables for VS 
static LPDIRECT3DVERTEXBUFFER9		lpVSVB;
static LPDIRECT3DVERTEXDECLARATION9 pVertexDeclarationOneStream;	// Pos Only (depth phase)
static LPDIRECT3DVERTEXDECLARATION9 pVertexDeclarationTwoStreams;	// Pos and UV (cloth)
static LPDIRECT3DVERTEXDECLARATION9 pVertexDeclarationThreeStreams;	// Pos, UV and Normal (objects)

static LPDIRECT3DVERTEXSHADER9		pVSBasic;
static LPDIRECT3DVERTEXSHADER9		pVSInitPhase;
static LPDIRECT3DVERTEXSHADER9		pVSDepthPhase;
static LPDIRECT3DVERTEXSHADER9		pVSClothPhase;
static LPDIRECT3DVERTEXSHADER9		pVSConstraintPhase;
static LPDIRECT3DVERTEXSHADER9		pVSNormalMapPhase;
static LPDIRECT3DVERTEXSHADER9		pVSDisplayPhaseA;
static LPDIRECT3DVERTEXSHADER9		pVSDisplayPhaseB;
static LPDIRECT3DVERTEXSHADER9		pVSDebug;

// Variables for PS 
static LPDIRECT3DPIXELSHADER9		pPSBasic;
static LPDIRECT3DPIXELSHADER9		pPSInitPhase;
static LPDIRECT3DPIXELSHADER9		pPSDepthPhase;
static LPDIRECT3DPIXELSHADER9		pPSClothPhase;
static LPDIRECT3DPIXELSHADER9		pPSConstraintPhase;
static LPDIRECT3DPIXELSHADER9		pPSNormalMapPhase;
static LPDIRECT3DPIXELSHADER9		pPSDisplayPhaseA;
static LPDIRECT3DPIXELSHADER9		pPSDisplayPhaseB;
static LPDIRECT3DPIXELSHADER9		pPSDebugGEO;
static LPDIRECT3DPIXELSHADER9		pPSDebugNRM;
static LPDIRECT3DPIXELSHADER9		pPSDebugDEPTH;

// Textures
static LPDIRECT3DTEXTURE9	lpDepthTexture;
static LPDIRECT3DTEXTURE9	lpMRTTexture[3][3];
static LPDIRECT3DTEXTURE9	lpClothTexture;
static LPDIRECT3DTEXTURE9	lpSphereTexture;
static LPDIRECT3DTEXTURE9	lpFloorTexture;
static LPDIRECT3DTEXTURE9	lpPVRLogoTexture;

// Surfaces
static LPDIRECT3DSURFACE9	lpSurfaceDepth;
static LPDIRECT3DSURFACE9	lpSurfaceMRT[3][3];
static LPDIRECT3DSURFACE9	lpTexDepthBuffer;
static LPDIRECT3DSURFACE9	lpBackBuffer;
static LPDIRECT3DSURFACE9	lpDepthBackBuffer;

// Render Target Sizes
static int		DepthWidth	=	1024;
static int		DepthHeight	=	1024;
static int		ClothWidth	=	64;
static int		ClothHeight	=	64;

static HMENU	MyMenu;

/***********************************************************************
************************************************************************
*********************** D3DSHELL FUNCTIONS *****************************
************************************************************************
***********************************************************************/


/****************************************************************************
** InitApplication() is called by D3DShell to enable user to initialise	   **
** his/her application													   **
****************************************************************************/
void InitApplication(HINSTANCE hInstance, HWND hWindow, char *pszCommandLine)
{
	unsigned int	nCurr, nVert;
	char			pszOrgCommandLine[256];
	char			szTmp[256];


#if 0
	/* Check for memory leaks */
	_CrtSetDbgFlag(_CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif

	/* You can use D3DShellSetPreferences(...) to set preferences for your application. 
	   You can specify a name, pass a menu, an accelerator table, an application icon 
	   and specify default rendering variables that will be used with your program : */

	MyMenu=LoadMenu(hInstance, "MENU");

	D3DShellSetPreferences(WND_TITLE,
						   MyMenu,
						   LoadAccelerators(hInstance, "ACCEL"),
						   LoadIcon(hInstance, "ICO_PVR"),
						   DEFAULT_NO_VSYNC |
						   USE_INSTANTANEOUS_FPS|
						   FORCE_MULTISAMPLING_OFF |  
						   /*DEFAULT_ZBUFFER_OFF |*/
#if USE3DMENUS == 1
						   USE_3D_MENUS | DISPLAY_PRINT3D_ICONS |
#endif
							0);

	/* Read command line parameters */
	strcpy(pszOrgCommandLine, pszCommandLine);
	_strupr(pszOrgCommandLine);

	ClGetValue(pszOrgCommandLine, "-OBJECT", szTmp);
	strcpy(pszFileName,szTmp);

	/* Shift objects back to origin for all meshes */
	for (nCurr=0; nCurr<NUM_MESHES; nCurr++) 
	{
		/* Shift objects back to origin */
		for (nVert = 0; nVert < Mesh[nCurr].nNumVertex; nVert++) 
		{
			Mesh[nCurr].pVertex[3 * nVert + 0] -= Mesh[nCurr].fCenter[0];
			Mesh[nCurr].pVertex[3 * nVert + 1] -= Mesh[nCurr].fCenter[1];
			Mesh[nCurr].pVertex[3 * nVert + 2] -= Mesh[nCurr].fCenter[2];
		}
	}

	/* Store Initial Camera Position */
	fCameraStartX=Camera[1].fPosition[0];
	fCameraStartY=Camera[1].fPosition[1];
	fCameraStartZ=Camera[1].fPosition[2];

}


/****************************************************************************
** QuitApplication() is called by D3DShell to enable user to release      **
** any memory before quitting the program.								   **
****************************************************************************/
void QuitApplication()
{



}


/*******************************************************************************
** UserWindowProc(...) is the application's window messages handler.
** From that function you can process menu inputs, keystrokes, timers, etc...
** When processing keystrokes, DO NOT process ESCAPE key 
** (VK_ESCAPE), as it is already used by D3DShell.
********************************************************************************/
void UserWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/* You can process normal window messages from that function (WM_COMMAND
	   to retrieve menu inputs, WM_KEYDOWN to retrieve keystrokes, etc...) */
	switch(message) 
	{
		case WM_KEYDOWN:	break;

		case WM_COMMAND:	switch(LOWORD(wParam)) 
							{
								case MENU_PAUSE_ON:		g_bPause = TRUE;  CheckMenuItem(MyMenu, MENU_PAUSE_ON, MF_CHECKED); break;
									
								case MENU_PAUSE_OFF:	g_bPause = FALSE; CheckMenuItem(MyMenu, MENU_PAUSE_ON, MF_UNCHECKED); break;

								case MENU_WIREFRAME:	SWAP(bWireframe); break;
								case MENU_POSITION:		nShowScene=1; break;
								case MENU_NORMAL:		nShowScene=2; break;
								case MENU_DEPTH:		nShowScene=0; break;
								case MENU_CLOTH:		nShowScene=3; break;
								case MENU_RESET:		bFirstTimeRun=true; break;


							}

							// Update Menu Ticks
							CheckMenuItem(MyMenu, MENU_POSITION, (nShowScene==1) ? MF_CHECKED : MF_UNCHECKED);
							CheckMenuItem(MyMenu, MENU_NORMAL, (nShowScene==2) ? MF_CHECKED : MF_UNCHECKED);
							CheckMenuItem(MyMenu, MENU_DEPTH, (nShowScene==0) ? MF_CHECKED : MF_UNCHECKED);
							CheckMenuItem(MyMenu, MENU_CLOTH, (nShowScene==3) ? MF_CHECKED : MF_UNCHECKED);
							CheckMenuItem(MyMenu, MENU_WIREFRAME,(bWireframe) ? MF_CHECKED : MF_UNCHECKED);
							break;
		}

	
}


/*******************************************************************************
** RenderScene(...) is the main rendering function. It is called by D3DShell
** for every frame.
** This function should contain a single BeginScene()/EndScene() pair containing
** the rendering functions to call (DrawPrimitive, SetRenderState, etc...)
*******************************************************************************/
BOOL RenderScene(LPDIRECT3DDEVICE9 lpDev9)
{
	bool	bResult;
	float	fFPS;

	lpDev9->BeginScene();

	if (bFirstTimeRun)
	{
		// If first frame then kick off initialisation - also when restarting cloth simulation		
		bResult=InitPhase(lpDev9);
		if (!bResult)
		{
			OutputDebugString("Something went wrong in InitPhase()\n");
			return false;
		}

		bFirstTimeRun=false;
		bDepthUpdate=true;
	}

	if (bDepthUpdate)
	{
		// If needed update the depth render surface, e.g. animation or new object or reset
		bResult=DepthPhase(lpDev9);
		if (!bResult)
		{
			OutputDebugString("Something went wrong in DepthPhase()\n");
			return false;
		}

		bDepthUpdate=false;
	}

	// Update Cloth - multiple iterations per frame

	if(!g_bPause) // only update if not paused
	{

		for (int loop=0; loop<iIterationLoops; loop++)
		{
			bResult=ClothPhase(lpDev9);
			if (!bResult)
			{
				OutputDebugString("Something went wrong in ClothPhase()\n");
				return false;
			}

			// Enforce constraints on Cloth

			bResult=ConstraintPhase(lpDev9);
			if (!bResult)
			{
				OutputDebugString("Something went wrong in ConstraintPhase()\n");
				return false;
			}
		}


		// Create Normal Map from Cloth Data

		bResult=NormalMapPhase(lpDev9);
		if (!bResult)
		{
			OutputDebugString("Something went wrong in NormalMapPhase()\n");
			return false;
		}

		iCount=iCount+1;

	}


	// Display the final result on the screen

	switch(nShowScene)
	{
		case 0:			DebugView(lpDev9,0); break;  // Show Depth Texture Contents
		case 1:			DebugView(lpDev9,1);  break; // Show Position MRT Contents
		case 2:			DebugView(lpDev9,2); break;  // Show NormalMap MRT Contents
		case 3:			{
							bResult=DisplayPhase(lpDev9);  // Show actual Cloth Scene
							if (!bResult)
							{
								OutputDebugString("Something went wrong in DisplayPhase()\n");
								return false;
							}
							break;
						}								
	}

	// Show Logo

	lpDev9->SetTexture(0,lpPVRLogoTexture);
	lpDev9->SetVertexShader(pVSBasic);
	lpDev9->SetPixelShader(pPSBasic);

	RenderLogo(lpDev9, (float)dwCurrentWidth, (float)dwCurrentHeight, 89.0f, 60.0f);

	// Reset to make sure Print3D works

	lpDev9->SetTexture(0,NULL);
	lpDev9->SetVertexShader(NULL);
	lpDev9->SetPixelShader(NULL);

	// Update Camera Position - Simple rotation and height wobble
	// Frame Based Animation

	if (!g_bPause)
	{
		Camera[1].fPosition[0]=((float)cos(fAngle))*fCameraStartX;
		Camera[1].fPosition[1]=fCameraStartY+(-25.0f+(((float)cos(fAngle*0.5f))*50.0f));
		Camera[1].fPosition[2]=-1.0f*((float)sin(fAngle))*fCameraStartX;
		fAngle=fAngle+0.05f;
	}

	D3DShellGetVariable((enum _D3DShellVariables)FRAME_RATE, &fFPS);

	if (fFPS<2000.0)  // Avoid huge initial number due to INSTANTANEOUS framerate being used.
	{
		if (fFPS>1.0f)
		{
			D3DTPrint3D(1.0f, 1.0f, 0.5f, 0xFFFF0000, "%.2f fps - %i iterations", fFPS,iCount);
		}
		else
		{
			fFPS=1.0f/fFPS;
			D3DTPrint3D(1.0f, 1.0f, 0.5f, 0xFFFF0000, "%.2f spf - %i iterations", fFPS,iCount);
		}
	}
	
	/* The function should ALWAYS return TRUE if the rendering was successful.
	   If D3DShell receives FALSE from this function, then rendering will be
	   stopped and the application will be terminated */

	lpDev9->EndScene();

	return TRUE;
}
	

/*******************************************************************************
** InitView() is called by D3DShell each time a rendering variable is changed
** in the Shell menu (Z-Buffer On/Off, resolution change, buffering mode...)
** In this function one should initialise all variables that are dependant on
** general rendering variables (screen mode, 3D device, etc...)
*******************************************************************************/
BOOL InitView(LPDIRECT3D9 lpD3D9, LPDIRECT3DDEVICE9 lpDev9, DWORD dwWidth, DWORD dwHeight)
{
	int nCurr;
	SModel		*psMdl;
	D3DMATRIX	WorldMatrix, TranslationMatrix;

	if (!LoadState())
	{
		OutputDebugString("Failed to Load State\n");
		return false;
	}

	bFirstTimeRun=TRUE;

	/* Store frame resolution */
	dwCurrentWidth = dwWidth;
	dwCurrentHeight = dwHeight;

	/* Initialise Print3D textures */
	D3DTPrint3DSetTextures(lpDev9, dwWidth, dwHeight);

	/*************
	* Check Caps *
	*************/

	if (!CheckCaps(lpDev9))
	{
		OutputDebugString("Failed in Caps Check in InitView()\n");
		return false;
	}


	/******************
	**    textures   **
	******************/

	if (!LoadTextures(lpDev9))
	{
		OutputDebugString("Failed to Load Textures in InitView()\n");
		return false;
	}

	if (!CreateTextures(lpDev9))
	{
		OutputDebugString("Failed to Create RT Textures in InitView()\n");
		return false;
	}

	/********************
	** Create  shaders **
	********************/

	if (!CreatePixelShaders(lpDev9))
	{
		OutputDebugString("Failed to create Pixel Shaders in InitView()\n");
		return false;
	}

	if (!CreateVertexShaders(lpDev9))
	{
		OutputDebugString("Failed to create Vertex Shaders in InitView()\n");
		return false;
	}

	/*********************
	** Shader Constants **
	*********************/

	if (!SetShaderConstants(lpDev9))
	{
		OutputDebugString("Failed to set Vertex Shader Constants in InitView()\n");
		return false;
	}

	/********************
	** Create Geometry **
	********************/

	if (!CreateModels(lpDev9))
	{
		OutputDebugString("Failed to create 3D Models in InitView()\n");
		return false;
	}

	/* Setup World Matrices */

	nCurr = NUM_MESHES;
	while(nCurr) 
	{
		nCurr--;

		/* Get pointer to current model */
		psMdl = &g_psMdl[nCurr];

		/* Reset world matrix to identity */
		D3DXMatrixIdentity((D3DXMATRIX *)&WorldMatrix);

		/* Object translation */
		D3DXMatrixTranslation((D3DXMATRIX *)&TranslationMatrix, psMdl->vPos.x, psMdl->vPos.y, psMdl->vPos.z);
		D3DXMatrixMultiply((D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&TranslationMatrix);
					   
		/* Store WORLD matrix in mesh */
		psMdl->mWorld = WorldMatrix;
	}

	
	/*******************************
	** Set default sampler states **
	*******************************/
	
	/* Clamp UVs  */

	lpDev9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	lpDev9->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(D3DVERTEXTEXTURESAMPLER1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(D3DVERTEXTEXTURESAMPLER1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(D3DVERTEXTEXTURESAMPLER2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(D3DVERTEXTEXTURESAMPLER2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	

	/* Set PROJECTION matrix */
	D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)&ProjectionMatrix, g_psCamera->fFOV, (float)dwCurrentWidth / (float)dwCurrentHeight, 
							   g_psCamera->fNearClip, g_psCamera->fFarClip);	
	lpDev9->SetTransform(D3DTS_PROJECTION, &ProjectionMatrix);

	/* Z Compare mode */
	lpDev9->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	/* Backface culling */
	lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	/* Enables Depth Testing */
	lpDev9->SetRenderState(D3DRS_ZENABLE, TRUE);
	
	/* Disable lighting completely */
	lpDev9->SetRenderState(D3DRS_LIGHTING, FALSE);
	lpDev9->SetRenderState(D3DRS_DITHERENABLE, FALSE);	

	/* Set Camera for Initial Phase*/
	g_psCamera = &Camera[0];

	/* The function should ALWAYS return TRUE if the initialisation was successful.
	   If D3DShell receives FALSE from this function, then rendering will be
	   stopped and the application will be terminated */
	return TRUE;
}


/*******************************************************************************
** ReleaseView(...) is called by D3DShell each time a rendering variable is
** changed. It is the direct companion to InitView(...), as its purpose is to
** release or destroy any variables or processes allocated in InitView.
** NOTE : Textures created with the D3DShellLoadBMP(...) function do *NOT* need
** to be released, as it is already taken care of by D3DShell.
*******************************************************************************/
void ReleaseView()
{
	/**************************
	** Release pixel shaders **
	**************************/
	
	DestroyPixelShaders();


	/***************************
	** Release vertex shaders **
	***************************/
	
	DestroyVertexShaders();

	/***********************
	** Release Model Data **
	***********************/

	DestroyModels();


	/*********************
	** Release textures **
	*********************/

	DestroyTextures();

	/*****************************
	** Release Print3D textures **
	*****************************/

	D3DTPrint3DReleaseTextures();
}


/***********************************************************************
************************************************************************
********************** APPLICATION FUNCTIONS ***************************
************************************************************************
***********************************************************************/
/* You can code your application functions (non-Shell functions) there */

/*******************************************************************************
 * Function Name  : RenderModel
 * Inputs		  : lpDev9, *psMdl, Number of Streams, Cloth Modifier
 * Returns        : Error
 * Global Used    : 
 * Description    : Render model
 *******************************************************************************/
bool RenderModel(LPDIRECT3DDEVICE9 lpDev9, SModel *psMdl, int nStream, bool bCloth)
{
	HRESULT		hres;
	D3DMATRIX	GlobalTransformationMatrix, MyWorld, Identity, MyScale;
	
	D3DMATRIX RotationMatrix;

	if (g_psCamera==&Camera[0])
	{
		// Depth Render uses Ortho Projection - detect from camera enabled

		D3DXMatrixOrthoLH((D3DXMATRIX *)&ProjectionMatrix,256,256,g_psCamera->fNearClip, g_psCamera->fFarClip);

		lpDev9->SetTransform(D3DTS_PROJECTION, &ProjectionMatrix);

		D3DXMatrixLookAtLH((D3DXMATRIX *)&ViewMatrix, 
					   (D3DXVECTOR3 *)g_psCamera->fPosition, 
					   (D3DXVECTOR3 *)g_psCamera->fTarget, 
					   &g_UpVector0);
	
	}
	else
	{
		D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)&ProjectionMatrix, g_psCamera->fFOV, (float)dwCurrentWidth / (float)dwCurrentHeight, 
							   g_psCamera->fNearClip, g_psCamera->fFarClip);

		lpDev9->SetTransform(D3DTS_PROJECTION, &ProjectionMatrix);

		D3DXMatrixLookAtLH((D3DXMATRIX *)&ViewMatrix, 
					   (D3DXVECTOR3 *)g_psCamera->fPosition, 
					   (D3DXVECTOR3 *)g_psCamera->fTarget, 
					   &g_UpVector1);
	}

	lpDev9->SetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixIdentity((D3DXMATRIX *)&Identity);

	/* Concatenate world, view and projection matrix */
	if (!bCloth)
	{		
		D3DXMatrixScaling((D3DXMATRIX *)&MyScale,1.0f,1.0f,1.0f);
		D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&psMdl->mWorld, (D3DXMATRIX *)&MyScale);
		
		D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&ViewMatrix);
		D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&ProjectionMatrix);
		D3DXMatrixRotationZ ((D3DXMATRIX *)&RotationMatrix,0.0f);
		D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&RotationMatrix);
	}
	else
	{
		D3DXMatrixScaling((D3DXMATRIX *)&MyScale,1.08f,1.0f,1.08f); // Scale the cloth so it sits around object - avoids ugly Z fighting
		D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&MyScale/*Identity*/, (D3DXMATRIX *)&ViewMatrix);
		D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&ProjectionMatrix);
		D3DXMatrixRotationY ((D3DXMATRIX *)&RotationMatrix,0.0f);
		D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&RotationMatrix);
	}
	

	D3DXMatrixTranspose((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix);

	/* Set global transformation matrix to VS */
	hres=lpDev9->SetVertexShaderConstantF(0, (float *)&GlobalTransformationMatrix,  4);

	D3DXMatrixMultiply((D3DXMATRIX *)&MyWorld, (D3DXMATRIX *)&psMdl->mWorld, (D3DXMATRIX *)&ViewMatrix);
	D3DXMatrixTranspose((D3DXMATRIX *)&MyWorld, (D3DXMATRIX *)&MyWorld);

	/* Set world transformation matrix to VS for depth render */
	hres=lpDev9->SetVertexShaderConstantF(4, (float *)&MyWorld,  4);
	
	/* Set indices */
	hres|=lpDev9->SetIndices(psMdl->lpIndexBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetIndices() failed in RenderModel()\n");
	}

	/* Set Stream(s) depending on what we are drawing*/

	if (nStream==1)  // Only Position - e.g. for Depth Render
	{
		hres=lpDev9->SetVertexDeclaration(pVertexDeclarationOneStream);
		if (hres!=D3D_OK)
		{
			OutputDebugString("SetVertexDeclaration() failed\n");
		}

		hres=lpDev9->SetStreamSource(0, psMdl->lpVertexBufferXYZ, 0, sizeof(D3DPOSVERTEX));
		if (hres!=D3D_OK)
		{	
			OutputDebugString("SetStreamSource() failed in RenderScene() - 1 Stream\n");
			return false;
		}
	}
	else if (nStream==2)  // Only Position and TexCoord - e.g. for Cloth
	{
		hres=lpDev9->SetVertexDeclaration(pVertexDeclarationTwoStreams);
		if (hres!=D3D_OK)
		{
			OutputDebugString("SetVertexDeclaration() failed\n");
		}

		hres =lpDev9->SetStreamSource(0, psMdl->lpVertexBufferXYZ , 0, sizeof(D3DPOSVERTEX));
		hres|=lpDev9->SetStreamSource(1, psMdl->lpVertexBufferTSUV, 0, sizeof(TSUVVERTEX));
		if (hres!=D3D_OK)
		{	
			OutputDebugString("SetStreamSource() failed in RenderScene() - 2 Streams\n");
			return false;
		}
	}
	else	// Position, Normal and TexCoord - e.g. for floor
	{	hres=lpDev9->SetVertexDeclaration(pVertexDeclarationThreeStreams);
		if (hres!=D3D_OK)
		{
			OutputDebugString("SetVertexDeclaration() failed\n");
		}

		hres =lpDev9->SetStreamSource(0, psMdl->lpVertexBufferXYZ   , 0, sizeof(D3DPOSVERTEX));
		hres|=lpDev9->SetStreamSource(1, psMdl->lpVertexBufferTSUV  , 0, sizeof(TSUVVERTEX));
		hres|=lpDev9->SetStreamSource(2, psMdl->lpVertexBufferNormal, 0, sizeof(NORMALVERTEX));
		if (hres!=D3D_OK)
		{	
			OutputDebugString("SetStreamSource() failed in RenderScene() - 3 Streams\n");
			return false;
		}
	}		
	
	/* Draw model */
	hres=lpDev9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, psMdl->nNumVertex, 0, psMdl->nNumFaces);
	if (hres!=D3D_OK)
	{
		OutputDebugString("DrawIndexedPrimitive() failed in RenderModel()\n");
		return false;
	}

	return true;
}

/*******************************************************************************
 * Function Name  : CreateModels
 * Inputs		  : lpDev9
 * Returns        : Error
 * Global Used    : 
 * Description    : Create Model from .H Data
 *******************************************************************************/
bool CreateModels(LPDIRECT3DDEVICE9 lpDev9)
{	
	/*************************
	** Create geometry data **
	*************************/

	int				nCurr, i;
	D3DPOSVERTEX		*pVertexXYZ;
	TSUVVERTEX		*pVertexTSUV;
	NORMALVERTEX	*pVertexNormal;
	WORD			*pIndex;
	DWORD			dwVert;

	HRESULT		hres;

	for (nCurr=0; nCurr<NUM_MESHES; nCurr++) 
	{
		_ASSERT(Mesh[nCurr].pVertex);
		_ASSERT(Mesh[nCurr].pNormals);
		_ASSERT(Mesh[nCurr].pUV);
		_ASSERT(Mesh[nCurr].fCenter);

		/* Object position */
		g_psMdl[nCurr].vPos = *(D3DVECTOR*)Mesh[nCurr].fCenter;
		g_psMdl[nCurr].vRot.x = 0;
		g_psMdl[nCurr].vRot.y = 0;
		g_psMdl[nCurr].vRot.z = 0;

		// Save info
		g_psMdl[nCurr].nNumVertex =		Mesh[nCurr].nNumVertex;
		g_psMdl[nCurr].nNumFaces =		Mesh[nCurr].nNumFaces;

		/* Create vertex buffers */
		hres=lpDev9->CreateVertexBuffer(g_psMdl[nCurr].nNumVertex*sizeof(D3DPOSVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &g_psMdl[nCurr].lpVertexBufferXYZ, NULL);
		hres|=lpDev9->CreateVertexBuffer(g_psMdl[nCurr].nNumVertex*sizeof(TSUVVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &g_psMdl[nCurr].lpVertexBufferTSUV, NULL);
		hres|=lpDev9->CreateVertexBuffer(g_psMdl[nCurr].nNumVertex*sizeof(NORMALVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &g_psMdl[nCurr].lpVertexBufferNormal, NULL);
		if (hres!=D3D_OK)
		{
			OutputDebugString("CreateVertexBuffer() failed in InitView()\n");
			return FALSE;
		}

		/* Lock vertex buffers and retrieve pointers to the locked data */
		hres=g_psMdl[nCurr].lpVertexBufferXYZ->Lock(0, 0, (void **)&pVertexXYZ, 0);
		hres|=g_psMdl[nCurr].lpVertexBufferTSUV->Lock(0, 0, (void **)&pVertexTSUV, 0);
		hres|=g_psMdl[nCurr].lpVertexBufferNormal->Lock(0, 0, (void **)&pVertexNormal, 0);
		if (hres!=D3D_OK)
		{
			OutputDebugString("Lock() failed\n");
			RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
			RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
			RELEASE(g_psMdl[nCurr].lpVertexBufferNormal);
			return FALSE;
		}

		/* Copy vertex position and UVs */
		for (dwVert=0; dwVert<(DWORD)g_psMdl[nCurr].nNumVertex; dwVert++) 
		{
			pVertexXYZ->x = Mesh[nCurr].pVertex[3 * dwVert + 0];
			pVertexXYZ->y = Mesh[nCurr].pVertex[3 * dwVert + 1];
			pVertexXYZ->z = Mesh[nCurr].pVertex[3 * dwVert + 2];
			pVertexXYZ++;

			pVertexTSUV->tu = Mesh[nCurr].pUV[2 * dwVert + 0];
			pVertexTSUV->tv = Mesh[nCurr].pUV[2 * dwVert + 1];
			pVertexTSUV++;

			pVertexNormal->nx = Mesh[nCurr].pNormals[3 * dwVert + 0];
			pVertexNormal->ny = Mesh[nCurr].pNormals[3 * dwVert + 1];
			pVertexNormal->nz = Mesh[nCurr].pNormals[3 * dwVert + 2];
			pVertexNormal++;
		}

		/* Unlock vertex buffers */
		g_psMdl[nCurr].lpVertexBufferTSUV->Unlock();
		g_psMdl[nCurr].lpVertexBufferXYZ->Unlock();
		g_psMdl[nCurr].lpVertexBufferNormal->Unlock();


		/* Create index buffer */
		hres = lpDev9->CreateIndexBuffer(g_psMdl[nCurr].nNumFaces * 3 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
                                         D3DPOOL_MANAGED, &g_psMdl[nCurr].lpIndexBuffer, NULL); 
		if (hres!=D3D_OK)
		{
			OutputDebugString("CreateIndexBuffer() failed\n");
			RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
			RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
			return FALSE;
		}

		/* Lock index buffer and retrieve a pointer to the locked data */
		hres=g_psMdl[nCurr].lpIndexBuffer->Lock(0, 0, (void **)&pIndex, 0);
		if (hres!=D3D_OK)
		{
			OutputDebugString("Lock() failed\n");
			RELEASE(g_psMdl[nCurr].lpIndexBuffer);
			RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
			RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
			RELEASE(g_psMdl[nCurr].lpVertexBufferNormal);
			return FALSE;
		}

		/* Copy indices */
		for (i=0; i<(int)Mesh[nCurr].nNumFaces; i++)
		{
			*pIndex++ = Mesh[nCurr].pFaces[i*3+0];
			*pIndex++ = Mesh[nCurr].pFaces[i*3+1];
			*pIndex++ = Mesh[nCurr].pFaces[i*3+2];
		}

		/* Unlock index buffer */
		g_psMdl[nCurr].lpIndexBuffer->Unlock();

	}	

	return true;
}
/*******************************************************************************
 * Function Name  : DestroyModel
 * Inputs		  : lpDev9
 * Returns        : Error
 * Global Used    : 
 * Description    : Clean-up models created from .H Data
 *******************************************************************************/
bool DestroyModels()
{
	int nCurr;

	for (nCurr = 0; nCurr <NUM_MESHES; nCurr++) 
	{
		RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
		RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
		RELEASE(g_psMdl[nCurr].lpVertexBufferNormal);
		RELEASE(g_psMdl[nCurr].lpIndexBuffer);
	}

	return true;
}

/*****************************************************************************
 * Function Name  : LoadTextures
 * Inputs		  : lpDev9
 * Outputs		  : 
 * Globals used   : Yes
 * Returns		  : Error
 * Description    : Load textures
 *****************************************************************************/
bool LoadTextures(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT		hres;


	// Load a texture 
	hres = D3DXCreateTextureFromFileEx(lpDev9, /*"GhostClothTestmap.bmp"*/"./Textures/defaultCloth.bmp", 
								   D3DX_DEFAULT, D3DX_DEFAULT,
								   D3DX_DEFAULT, 
								   0, 
								   D3DFMT_DXT1, 
								   D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
								   D3DX_FILTER_BOX, 0, NULL, NULL, &lpClothTexture);
	
	hres = D3DXCreateTextureFromFileEx(lpDev9, "./Textures/tableMap.bmp", 
								   D3DX_DEFAULT, D3DX_DEFAULT,
								   D3DX_DEFAULT, 
								   0, 
								   D3DFMT_DXT1, 
								   D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
								   D3DX_FILTER_BOX, 0, NULL, NULL, &lpSphereTexture);

	hres = D3DXCreateTextureFromFileEx(lpDev9, "./Textures/floorKristof.bmp", 
								   D3DX_DEFAULT, D3DX_DEFAULT,
								   D3DX_DEFAULT, 
								   0, 
								   D3DFMT_DXT1, 
								   D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
								   D3DX_FILTER_BOX, 0, NULL, NULL, &lpFloorTexture);

	hres = D3DXCreateTextureFromFileEx(lpDev9, "./Textures/LogoPVR.bmp", 
								   D3DX_DEFAULT, D3DX_DEFAULT,
								   1, 
								   0, 
								   D3DFMT_A8R8G8B8, 
								   D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
								   D3DX_FILTER_BOX, 0, NULL, NULL, &lpPVRLogoTexture);

	

	/* No problem occured */
	return TRUE;
}


/*****************************************************************************
 * Function Name  : DestroyTextures
 * Inputs		  : 
 * Outputs		  : 
 * Globals used   : Yes
 * Returns		  : Nothing
 * Description    : Destroy textures
 *****************************************************************************/
void DestroyTextures()
{
	// Match LoadTextures and Create Textures
	
	// Textures and surfaces
	RELEASE(lpDepthTexture);
	RELEASE(lpSurfaceDepth);
	RELEASE(lpTexDepthBuffer);

	RELEASE(lpClothTexture);
	RELEASE(lpSphereTexture);
	RELEASE(lpFloorTexture);
	RELEASE(lpPVRLogoTexture);

	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			RELEASE(lpMRTTexture[i][j]);
			RELEASE(lpSurfaceMRT[i][j]);
		}
	}	
}

/*******************************************************************************
 * Function Name  : CreatePixelShaders
 * Returns        : Error
 * Global Used    : 
 * Description    : Render shadows
 *******************************************************************************/
bool CreatePixelShaders(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT					hres;
	LPD3DXBUFFER			pCode;
	LPDIRECT3DPIXELSHADER9	*PS[11];
	char					*pszPSFileName[] = { "./Shaders/PSBasic.txt", "./Shaders/PSInitPhase.txt", "./Shaders/PSDepthPhase.txt", pszPSStr, "./Shaders/PSConstraintPhase.txt", "./Shaders/PSNormalMapPhase.txt", "./Shaders/PSDisplayPhaseA.txt", "./Shaders/PSDisplayPhaseB.txt", "./Shaders/PSDebugGEO.txt", "./Shaders/PSDebugNRM.txt", "./Shaders/PSDebugDEPTH.txt" };
	int						i;
	char					pszTmp[512];

	/* Fill array of PS pointers */
	PS[0] = &pPSBasic;
	PS[1] = &pPSInitPhase;
	PS[2] = &pPSDepthPhase;
	PS[3] = &pPSClothPhase;
	PS[4] = &pPSConstraintPhase;
	PS[5] = &pPSNormalMapPhase;
	PS[6] = &pPSDisplayPhaseA;
	PS[7] = &pPSDisplayPhaseB;
	PS[8] = &pPSDebugGEO;
	PS[9] = &pPSDebugNRM;
	PS[10]= &pPSDebugDEPTH;

	/* Loop through all pixel shaders */
	for (i=0; i<11; i++)
	{
		/* Assemble pixel shader */
		hres = D3DXAssembleShaderFromFile(pszPSFileName[i], NULL, NULL, 0, &pCode, NULL);
		if (hres!=D3D_OK)	
		{
			sprintf(pszTmp, "Failed to assemble %s Pixel Shader\n", pszPSFileName[i]);
			OutputDebugString(pszTmp);
			return false;
		}

		/* Create the pixel shader */
		hres = lpDev9->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), PS[i]);
		if (hres!=D3D_OK)	
		{
			sprintf(pszTmp, "Failed to create Pixel Shader %s\n", pszPSFileName[i]);
			OutputDebugString(pszTmp);
			return false;
		}

		/* Release pointer */
		pCode->Release();
	}
	return true;
}


/*******************************************************************************
 * Function Name  : DestroyPixelShaders
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Destroy pixel shaders
 *******************************************************************************/
void DestroyPixelShaders()
{
	/* Release all pixel shaders */
	RELEASE(pPSBasic);
	RELEASE(pPSInitPhase);
	RELEASE(pPSDepthPhase);
	RELEASE(pPSClothPhase);
	RELEASE(pPSConstraintPhase);
	RELEASE(pPSNormalMapPhase);
	RELEASE(pPSDisplayPhaseA);
	RELEASE(pPSDisplayPhaseB);
	RELEASE(pPSDebugGEO);
	RELEASE(pPSDebugNRM);
	RELEASE(pPSDebugDEPTH);

}

/*******************************************************************************
 * Function Name  : CreateVertexShaders
 * Returns        : Error
 * Global Used    : 
 * Description    : Create Vertex Shaders from Text Files
 *******************************************************************************/
bool CreateVertexShaders(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT					hres;
	LPD3DXBUFFER			pCode;
	LPDIRECT3DVERTEXSHADER9	*VS[9];
	char					*pszVSFileName[] = { "./Shaders/VSBasic.txt", "./Shaders/VSInitPhase.txt", "./Shaders/VSDepthPhase.txt", "./Shaders/VSClothPhase.txt", "./Shaders/VSConstraintPhase.txt", "./Shaders/VSNormalMapPhase.txt", "./Shaders/VSDisplayPhaseA.txt", "./Shaders/VSDisplayPhaseB.txt", "./Shaders/VSDebug.txt" };
	int						i;
	char					pszTmp[512];

	// Create the shader declarations

	/* Single stream (used by depth phase) */
    D3DVERTEXELEMENT9 declOneStream[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
        D3DDECL_END()
    };
	
	D3DVERTEXELEMENT9 declTwoStreams[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
        { 1,  0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
        D3DDECL_END()
    };

	D3DVERTEXELEMENT9 declThreeStreams[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
        { 1,  0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
		{ 2,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
        D3DDECL_END()
	};

	D3DVERTEXELEMENT9 declPositionUVSingleStream[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
        D3DDECL_END()
    };

	hres =	lpDev9->CreateVertexDeclaration(declOneStream, &pVertexDeclarationOneStream);
	hres |= lpDev9->CreateVertexDeclaration(declTwoStreams, &pVertexDeclarationTwoStreams);
	hres |= lpDev9->CreateVertexDeclaration(declThreeStreams, &pVertexDeclarationThreeStreams);
	if (hres!=D3D_OK)
	{
		OutputDebugString("CreateVertexDeclaration() failed in InitView()\n");
		return false;
	}

	/* Fill array of VS pointers */
	VS[0] = &pVSBasic;
	VS[1] = &pVSInitPhase;
	VS[2] = &pVSDepthPhase;
	VS[3] = &pVSClothPhase;
	VS[4] = &pVSConstraintPhase;
	VS[5] = &pVSNormalMapPhase;
	VS[6] = &pVSDisplayPhaseA;
	VS[7] = &pVSDisplayPhaseB;
	VS[8] = &pVSDebug;
	

	/* Loop through all vertex shaders */
	for (i=0; i<9; i++)
	{
		/* Assemble vertex shader */
		hres = D3DXAssembleShaderFromFile(pszVSFileName[i], NULL, NULL, 0, &pCode, NULL);
		if (hres!=D3D_OK)	
		{
			sprintf(pszTmp, "Failed to assemble %s Vertex Shader\n", pszVSFileName[i]);
			OutputDebugString(pszTmp);
			return false;
		}

		/* Create the vertex shader */
		hres = lpDev9->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), VS[i]);
		if (hres!=D3D_OK)	
		{
			sprintf(pszTmp, "Failed to create Vertex Shader %s\n", pszVSFileName[i]);
			OutputDebugString(pszTmp);
			return false;
		}

		/* Release pointer */
		pCode->Release();
	}
	return true;
}


/*******************************************************************************
 * Function Name  : DestroyVertexShaders
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Destroy vertex shaders
 *******************************************************************************/
void DestroyVertexShaders()
{
	/* Release all pixel shaders */
	RELEASE(pVSBasic);
	RELEASE(pVSInitPhase);
	RELEASE(pVSDepthPhase);
	RELEASE(pVSClothPhase);
	RELEASE(pVSConstraintPhase);
	RELEASE(pVSNormalMapPhase);
	RELEASE(pVSDisplayPhaseA);
	RELEASE(pVSDisplayPhaseB);
	RELEASE(pVSDebug);

	/* Release Declarations */

	RELEASE(pVertexDeclarationOneStream);
	RELEASE(pVertexDeclarationTwoStreams);
	RELEASE(pVertexDeclarationThreeStreams);
}

/*******************************************************************************
 * Function Name  : SetShaderConstants
 * Returns        : Error
 * Global Used    : 
 * Description    : Set Constants as used by the Pixel and Vertex Shaders
 *******************************************************************************/
bool SetShaderConstants(LPDIRECT3DDEVICE9 lpDev9)
{
	// Only set some general usage constants here
	// RenderModel sets up the transformation constants (C0-3 and C4-7)

	//Vertex Shader Constant Map
	//C0-3		= World/View/Proj
	//C4-7		= World/View
	//C8		= Cloth Size -> [0->1] => [0.x->0.x] => Size = X then .x=X and .y=(1-X)/2  .x is multiply and .y is add (init phase)
	//C9		= View to World for Z and Z scale to [0->1] .x is factor (400) and .y is scale (1/240) (Depth phase)
	//C10-C17	= 8 Neighbor Deltas for Vertex Shader (Cloth Phase)
	//C18		= Light Direction (Display Phase)
	//C19		= Ambient Light (Display Phase)
	//C20		= Scaling factors for normalise to world .x = add (120) and .y = mul (240) (Display Phase)
	//C21		= 0.0f, 0.0f, 0.0f, 1.0f (init value) (Almost all phases)

	HRESULT		hres;
	float		fFloat[4];
	float		fDU, fDV, fDU2, fDV2, fDW;

	fFloat[0]=fClothSize; fFloat[1]=(1.0f-fClothSize)*0.5f; fFloat[2]=0.0f; fFloat[3]=0.0f;
	hres =lpDev9->SetVertexShaderConstantF(8, fFloat,  1);

	fFloat[0]=400.0f; fFloat[1]=(1.0f/240.0f); fFloat[2]=0.0f; fFloat[3]=0.0f;
	hres |=lpDev9->SetVertexShaderConstantF(9, fFloat,  1);

	fDU = 1.0f/ClothWidth;
	fDV = 1.0f/ClothHeight;
	fDU2 = (2.0f)/ClothWidth;
	fDV2 = (2.0f)/ClothHeight;

	fFloat[0] = fDU;	fFloat[1] = 0.0f; fFloat[2] = fDU2;	 fFloat[3] = 0.0f;
	hres |=lpDev9->SetVertexShaderConstantF(10, fFloat,  1);
	fFloat[0] = fDU;	fFloat[1] = fDV;  fFloat[2] = fDU2;	 fFloat[3] = fDV2;
	hres|=lpDev9->SetVertexShaderConstantF(14, fFloat,  1);
	fFloat[0] = 0.0f;	fFloat[1] = fDV;  fFloat[2] = 0.0f;	 fFloat[3] = fDV2;
	hres|=lpDev9->SetVertexShaderConstantF(11, fFloat,  1);
	fFloat[0] = -fDU;	fFloat[1] = fDV;  fFloat[2] = -fDU2; fFloat[3] = fDV2;
	hres|=lpDev9->SetVertexShaderConstantF(15, fFloat,  1);
	fFloat[0] = -fDU;	fFloat[1] = 0.0f; fFloat[2] = -fDU2; fFloat[3] = 0.0f;
	hres|=lpDev9->SetVertexShaderConstantF(12, fFloat,  1);
	fFloat[0] = -fDU;	fFloat[1] = -fDV; fFloat[2] = -fDU2; fFloat[3] = -fDV2;
	hres|=lpDev9->SetVertexShaderConstantF(16, fFloat,  1);
	fFloat[0] = 0.0;	fFloat[1] = -fDV; fFloat[2] = 0.0;	 fFloat[3] = -fDV2;
	hres|=lpDev9->SetVertexShaderConstantF(13, fFloat,  1);
	fFloat[0] = fDU;	fFloat[1] = -fDV; fFloat[2] = fDU2;	 fFloat[3] = -fDV2;
	hres|=lpDev9->SetVertexShaderConstantF(17, fFloat,  1);

	fFloat[0]=0.4267f; fFloat[1]=-0.853f; fFloat[2]=0.298f; fFloat[3]=0.0f;
	hres |=lpDev9->SetVertexShaderConstantF(18, fFloat,  1);

	fFloat[0]=0.5f; fFloat[1]=0.5f; fFloat[2]=0.5f; fFloat[3]=0.5f;
	hres |=lpDev9->SetVertexShaderConstantF(19, fFloat,  1);

	fFloat[0]=120.0f; fFloat[1]=240.0f; fFloat[2]=0.5f; fFloat[3]=0.0f;
	hres |=lpDev9->SetVertexShaderConstantF(20, fFloat,  1);

	fFloat[0]=0.0f; fFloat[1]=0.0f; fFloat[2]=0.0f; fFloat[3]=1.0f;
	hres |=lpDev9->SetVertexShaderConstantF(21, fFloat,  1);

	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShaderConstantF() failed in SetShaderConstants()\n");
		return false;
	}

	// Set PS constants for the Cloth Phase

	//Pixel Shader Constant Map

	//C0   = Gravity							(cloth phase)
	//C1   = Small Value A						(cloth phase)
	//C2   = Small Value B						(cloth phase)
	//C3   = Small Value C						(cloth phase)
	//C4   = Small Value D						(cloth phase)
	//C5   = Small Value E						(cloth phase)
	//C6   = Small Value F						(cloth phase)
	//C7   = Initial Length A = 1.0				(cloth phase)
	//C8   = Initial Length B = 1.4 (Diag)		(cloth phase)
	//C9   = Initial Length C = 2.0				(cloth phase)
	//C10  = Initial Length D = 2.8 (Diag)		(cloth phase)
	//C11  = Initial Length E+F = 2.2 (Gaps)	(cloth phase)
	//C12  = Initial Height of Cloth (scalar)	(init phase)
	//C13  = Constraint Range					(constraint phase)

	fFloat[0]=0.0f; fFloat[1]=0.0f; fFloat[2]=fGravity; fFloat[3]=0.0f;
	hres =lpDev9->SetPixelShaderConstantF(0, fFloat,  1);

	fFloat[0]=fSVA[0]; fFloat[1]=fSVA[1]; fFloat[2]=fSVA[2]; fFloat[3]=fSVA[3];
	hres |=lpDev9->SetPixelShaderConstantF(1, fFloat,  1);
	fFloat[0]=fSVB[0]; fFloat[1]=fSVB[1]; fFloat[2]=fSVB[2]; fFloat[3]=fSVB[3];
	hres |=lpDev9->SetPixelShaderConstantF(2, fFloat,  1);
	fFloat[0]=fSVC[0]; fFloat[1]=fSVC[1]; fFloat[2]=fSVC[2]; fFloat[3]=fSVC[3];
	hres |=lpDev9->SetPixelShaderConstantF(3, fFloat,  1);
	fFloat[0]=fSVD[0]; fFloat[1]=fSVD[1]; fFloat[2]=fSVD[2]; fFloat[3]=fSVD[3];
	hres |=lpDev9->SetPixelShaderConstantF(4, fFloat,  1);
	fFloat[0]=fSVE[0]; fFloat[1]=fSVE[1]; fFloat[2]=fSVE[2]; fFloat[3]=fSVE[3];
	hres |=lpDev9->SetPixelShaderConstantF(5, fFloat,  1);
	fFloat[0]=fSVF[0]; fFloat[1]=fSVF[1]; fFloat[2]=fSVF[2]; fFloat[3]=fSVF[3];
	hres |=lpDev9->SetPixelShaderConstantF(6, fFloat,  1);

	fDU = fClothSize/(ClothWidth-1.0f);						// default length
	fDV = (fClothSize*1.414214f)/(ClothHeight-1.0f);		// diagonal length
	fDW = (fClothSize*2.236068f)/(ClothHeight-1.0f);		// gap length

	fFloat[0]=0.0f; fFloat[1]=0.0f; fFloat[2]=0.0f; fFloat[3]=0.0f;

	fFloat[0] = 1.0f;	 fFloat[1] = 1.0f/fDU;				// Case A  : Not Diagonal Sample
	hres|=lpDev9->SetPixelShaderConstantF(7, fFloat,  1);
	fFloat[0] = 1.0f;	 fFloat[1] = 1.0f/fDV;				// Case B  : Diagonal Sample
	hres|=lpDev9->SetPixelShaderConstantF(8, fFloat,  1);
	fFloat[0] = 1.0f;	 fFloat[1] = 1.0f/(2.0f*fDV);		// Case C  : Distant Sample ND
	hres|=lpDev9->SetPixelShaderConstantF(9, fFloat,  1);
	fFloat[0] = 1.0f;	 fFloat[1] = 1.0f/(2.0f*fDV);		// Case D  : Distant Sample D
	hres|=lpDev9->SetPixelShaderConstantF(10, fFloat,  1);
	fFloat[0] = 1.0f;	 fFloat[1] = 1.0f/(fDW);			// Case E+F: Distant Sample G
	hres|=lpDev9->SetPixelShaderConstantF(11, fFloat,  1);

	fFloat[0]=0.0f; fFloat[1]=0.0f; fFloat[2]=0.0f; fFloat[3]=0.0f;
	switch (nObjSelected)
	{
		case 1:		fFloat[0]=150.0f/240.0f; break; /*Sphere*/
		case 7:		fFloat[0]=110.0f/240.0f; break; /*Square Table*/
		case 8:		fFloat[0]=109.0f/240.0f; break; /*Round Table*/
		case 9:		fFloat[0]=158.0f/240.0f; break; /*Ghost*/
		case 10:	fFloat[0]= 61.0f/240.0f; break; /*Text*/
		case 11:	fFloat[0]= 87.0f/240.0f; break; /*Tent Framework*/
		case 12:	fFloat[0]=118.0f/240.0f; break; /*Woman*/
	}
	hres |=lpDev9->SetPixelShaderConstantF(12, fFloat,  1);

	fFloat[0]=fConstraintRange; fFloat[1]=0.0f; fFloat[2]=0.0f; fFloat[3]=0.0f;
	hres |=lpDev9->SetPixelShaderConstantF(13, fFloat,  1);

	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShaderConstantF() failed in SetShaderConstants()\n");
		return false;
	}

	return true;
}

/*******************************************************************************
 * Function Name  : CreateTextures
 * Returns        : Error
 * Global Used    : 
 * Description    : Sets up the various textures used as  Render Targets 
 *******************************************************************************/
bool CreateTextures(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT	hres;

	// Create Depth Render Texture, Target Surface and Depth Buffer
	// Format = F16 to match MRT

	hres =  lpDev9->CreateTexture(DepthWidth, DepthHeight, 0, D3DUSAGE_RENDERTARGET , D3DFMT_R16F, D3DPOOL_DEFAULT, &lpDepthTexture, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to create Depth Texture Render Target\n");
		return FALSE;
	}
	hres =  lpDepthTexture->GetSurfaceLevel(0, &lpSurfaceDepth);
	if (hres!=D3D_OK)
	{
		OutputDebugString("GetSurfaceLevel() failed on Depth Texture\n");
		return FALSE;
	}

	hres = lpDev9->CreateDepthStencilSurface(DepthWidth, DepthHeight, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &lpTexDepthBuffer, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to create Texture Render Target Depth Buffer\n");
		return FALSE;
	}

	// Create MRT (pos+normal) Render Textures and Target Surfaces
	// Total = F16-16 + F16-16 + F16-16 = PosX, PosY, PosZ, NormX, NormY, NormZ

	for (int j=0;j<3;j++)
	{
		for (int i=0;i<3;i++)
		{
			hres =  lpDev9->CreateTexture(ClothWidth, ClothHeight, 0, D3DUSAGE_RENDERTARGET , D3DFMT_G16R16F, D3DPOOL_DEFAULT, &lpMRTTexture[j][i], NULL);
			if (hres!=D3D_OK)
			{
				OutputDebugString("Failed to create MRT\n");
				return FALSE;
			}
			hres =  lpMRTTexture[j][i]->GetSurfaceLevel(0, &lpSurfaceMRT[j][i]);
			if (hres!=D3D_OK)
			{
				OutputDebugString("GetSurfaceLevel() failed on MRT\n");
				return FALSE;
			}
		}
	}

	return true;
}


/*******************************************************************************
 * Function Name  : InitPhase
 * Returns        : Error
 * Global Used    : 
 * Description    : Handles the Initialisation Phase of the RenderLoop 
 *******************************************************************************/
bool InitPhase(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT hres;

	/**********************************
	** First Pass : Clear everything **
	**********************************/

	// Clear Everything related to BackBuffer

	lpDev9->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xFF000000, 1, 0);

	// Retrieve and Store pointer to BackBufferRT and DepthBackBuffer

	hres = lpDev9->GetRenderTarget(0, &lpBackBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to retrieve back buffer surface\n");
		return false;
	}
	lpBackBuffer->Release();

	hres = lpDev9->GetDepthStencilSurface(&lpDepthBackBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to retrieve back buffer depth surface\n");
		return false;
	}
	lpDepthBackBuffer->Release();

	// Clear Render Targets
	for (int i=0; i<2;i++)
	{
		for (int j=0;j<3;j++)
		{
			hres = lpDev9->SetRenderTarget(0, lpSurfaceMRT[i][j]);
			if (hres!=D3D_OK)
			{
				OutputDebugString("SetRenderTarget() failed in InitPhase\n");
				return false;
			}
			lpDev9->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF00FF00, 1, 0);
		}
	}

	// Set and Clear RT BackBuffer

	/* Set depth buffer */
	hres = lpDev9->SetDepthStencilSurface(lpTexDepthBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to set depth buffer\n");
		return false;
	}

	lpDev9->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFF000000, 1.0f, 128);

	/****************************************************
	** Second Pass : Setup start-up cloth grid in MRT0 **
	****************************************************/

	/* Disable Z Buffer */

	lpDev9->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);

	hres = lpDev9->SetDepthStencilSurface(NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to release depth buffer\n");
		return false;
	}

	SetMRT1TexMRT0RT(lpDev9);	// Select MRT0 as render target

	/* Set Vertex Shader */
	hres=lpDev9->SetVertexShader(pVSInitPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed in InitPhase()\n");
		return false;
	}

	/* Set Pixel Shader */
	hres=lpDev9->SetPixelShader(pPSInitPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed in InitPhase()\n");
		return false;
	}

	RenderScreenAlignedQuad(lpDev9, (float)ClothWidth, (float)ClothHeight, (float)ClothWidth, (float)ClothHeight);

	return true;
}


/*******************************************************************************
 * Function Name  : DepthPhase
 * Returns        : Error
 * Global Used    : 
 * Description    : Handles the Depth Phase of the RenderLoop
 *******************************************************************************/
bool DepthPhase(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT hres;

	// Select Camera

	g_psCamera = &Camera[0];

	// Set Depth Render Target and Texture Z Buffer and enable Z

	hres = lpDev9->SetRenderTarget(0, lpSurfaceDepth);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetRenderTarget() failed in DepthPhase()\n");
	}

	// Disable other render targets

	hres  = lpDev9->SetRenderTarget(1, NULL);
	hres |= lpDev9->SetRenderTarget(2, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetRenderTarget() failed to release rendertargets in DepthPhase()\n");
	}

	hres = lpDev9->SetDepthStencilSurface(lpTexDepthBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to set depth buffer in DepthPhase()\n");
		return false;
	}

	lpDev9->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

	lpDev9->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFF000000, 1.0f, 128);

	// Render depth to surface

	/* Set Vertex Shader */
	hres=lpDev9->SetVertexShader(pVSDepthPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed in DepthPhase()\n");
		return false;
	}

	/* Set Pixel Shader */
	hres=lpDev9->SetPixelShader(pPSDepthPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed in DepthPhase()\n");
		return false;
	}

	// Render Object(s) and Floor

	RenderModel(lpDev9, &g_psMdl[nObjSelected], 1, false);  // Sphere with single stream (pos only) 6/1
	
	/* Disable Z Buffer for following phases*/

	
	lpDev9->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);

	hres = lpDev9->SetDepthStencilSurface(NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to release depth buffer\n");
		return false;
	}

	return true;
}


/*******************************************************************************
 * Function Name  : ClothPhase
 * Returns        : Error
 * Global Used    : 
 * Description    : Handles the Cloth Phase of the RenderLoop
 *******************************************************************************/
bool ClothPhase(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT hres;

	SetMRT0TexMRT1RT(lpDev9);	// Select MRT1 as render target and input is MRT0 from init phase

	/* Set Vertex Shader */
	hres=lpDev9->SetVertexShader(pVSClothPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed in ClothPhase()\n");
		return false;
	}

	/* Set Pixel Shader */
	hres=lpDev9->SetPixelShader(pPSClothPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed in ClothPhase()\n");
		return false;
	}

	RenderScreenAlignedQuad(lpDev9, (float)ClothWidth, (float)ClothHeight, (float)ClothWidth, (float)ClothHeight);

	return true;
}


/*******************************************************************************
 * Function Name  : ConstraintPhase
 * Returns        : Error
 * Global Used    : 
 * Description    : Handles the Constraint Phase of the RenderLoop 
 *******************************************************************************/
bool ConstraintPhase(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT hres;


	SetMRT1TexMRT0RT(lpDev9);	// Select MRT0 as render target and input is MRT1 from cloth phase

	lpDev9->SetTexture(3, lpDepthTexture);

	/* Set Vertex Shader */
	hres=lpDev9->SetVertexShader(pVSConstraintPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed in ClothPhase()\n");
		return false;
	}

	/* Set Pixel Shader */
	hres=lpDev9->SetPixelShader(pPSConstraintPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed in ClothPhase()\n");
		return false;
	}

	RenderScreenAlignedQuad(lpDev9, (float)ClothWidth, (float)ClothHeight, (float)ClothWidth, (float)ClothHeight);

	return true;
}


/*******************************************************************************
 * Function Name  : NormalMapPhase
 * Returns        : Error
 * Global Used    : 
 * Description    : Handles the Normal Map Generation Phase of the RenderLoop
 *******************************************************************************/
bool NormalMapPhase(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT hres;

	SetMRT0TexMRT2RT(lpDev9);	

	lpDev9->SetTexture(3, NULL);

	/* Set Vertex Shader */
	hres=lpDev9->SetVertexShader(pVSNormalMapPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed in ClothPhase()\n");
		return false;
	}

	/* Set Pixel Shader */
	hres=lpDev9->SetPixelShader(pPSNormalMapPhase);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed in ClothPhase()\n");
		return false;
	}

	RenderScreenAlignedQuad(lpDev9, (float)ClothWidth, (float)ClothHeight, (float)ClothWidth, (float)ClothHeight);

	return true;

}

/*******************************************************************************
 * Function Name  : DebugView
 * Returns        : Error
 * Inputs		  : lpDev9 and Scene ID
 * Global Used    : 
 * Description    : Handles the Normal Map Generation Phase of the RenderLoop
 *******************************************************************************/
bool DebugView(LPDIRECT3DDEVICE9 lpDev9, int nScene)
{
	HRESULT hres;

	lpDev9->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

	// Set Back Buffer as Render Target

	hres = lpDev9->SetRenderTarget(0, lpBackBuffer);
	hres = lpDev9->SetRenderTarget(1, NULL);
	hres = lpDev9->SetRenderTarget(2, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetRenderTarget() failed in DisplayPhase()\n");
	}

	// Set the correct Depth/Stencil Buffer

	hres = lpDev9->SetDepthStencilSurface(lpDepthBackBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to set Depth Buffer in DisplayPhase\n");
		return false;
	}

	lpDev9->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xFF000000, 1, 0);

	if (nScene!=0)
	{
		hres =lpDev9->SetTexture(0,lpMRTTexture[2][0]);
		hres|=lpDev9->SetTexture(1,lpMRTTexture[2][1]);
		hres|=lpDev9->SetTexture(2,lpMRTTexture[2][2]);	
	}
	else
	{
		hres =lpDev9->SetTexture(0,lpDepthTexture);
		hres =lpDev9->SetTexture(1,NULL);
		hres =lpDev9->SetTexture(2,NULL);
	}

	if (hres!=D3D_OK)
	{
		OutputDebugString("SetTexture failed in DebugView()\n");
		return false;
	}


	/* Set Vertex Shader */
	hres=lpDev9->SetVertexShader(pVSDebug);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed in DebugView()\n");
		return false;
	}

	/* Set Pixel Shader */
	if (nScene==1)
	{
		hres=lpDev9->SetPixelShader(pPSDebugGEO);
	}
	else if (nScene==2)
	{
		hres=lpDev9->SetPixelShader(pPSDebugNRM);
	}
	else
	{
		hres=lpDev9->SetPixelShader(pPSDebugDEPTH);
	}


	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed in DebugView()\n");
		return false;
	}

	RenderScreenAlignedQuad(lpDev9, (float)dwCurrentWidth, (float)dwCurrentHeight, (float)dwCurrentWidth, (float)dwCurrentHeight);

	return true;

}


/*******************************************************************************
 * Function Name  : DisplayPhase
 * Returns        : Error
 * Global Used    : 
 * Description    : Handles the final Display Phase of the RenderLoop
 *******************************************************************************/
bool DisplayPhase(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT hres;

	// Enable Z Buffer
	lpDev9->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

	// Change Camera
	g_psCamera = &Camera[1];

	// Set Back Buffer as Render Target

	hres = lpDev9->SetRenderTarget(0, lpBackBuffer);
	hres = lpDev9->SetRenderTarget(1, NULL);
	hres = lpDev9->SetRenderTarget(2, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetRenderTarget() failed in DisplayPhase()\n");
	}

	// Set the correct Depth/Stencil Buffer

	hres = lpDev9->SetDepthStencilSurface(lpDepthBackBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to set Depth Buffer in DisplayPhase\n");
		return false;
	}

	lpDev9->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xFF000000, 1, 0);

	// Render Default Object and Floor

	/* Set Vertex Shader */
	hres=lpDev9->SetVertexShader(pVSDisplayPhaseA);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed in ClothPhase()\n");
		return false;
	}

	/* Set Pixel Shader */
	hres=lpDev9->SetPixelShader(pPSDisplayPhaseA);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed in ClothPhase()\n");
		return false;
	}

	// Render Object(s) and Floor

	lpDev9->SetTexture(0,lpSphereTexture);

	lpDev9->SetSamplerState(0,D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	lpDev9->SetSamplerState(0,D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	lpDev9->SetSamplerState(0,D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	lpDev9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	lpDev9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	if ((nObjSelected!=12) && (nObjSelected!=9))  // Don't draw woman and don't draw ghost obj
	{
		RenderModel(lpDev9, &g_psMdl[nObjSelected], 3, false);
	}

	lpDev9->SetTexture(0,lpFloorTexture);

	RenderModel(lpDev9, &g_psMdl[0], 3, false);  // Floor 

	lpDev9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	lpDev9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	

	// Render Special Cloth Object

	if (bWireframe)
	{
		lpDev9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	// Set Vertex Textures

	hres =lpDev9->SetTexture(D3DVERTEXTEXTURESAMPLER0,lpMRTTexture[2][0]);
	hres|=lpDev9->SetTexture(D3DVERTEXTEXTURESAMPLER1,lpMRTTexture[2][1]);
	hres|=lpDev9->SetTexture(D3DVERTEXTEXTURESAMPLER2,lpMRTTexture[2][2]);	
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexTexture failed in DisplayPhase()\n");
		return false;
	}

	/* Set Vertex Shader */
	hres=lpDev9->SetVertexShader(pVSDisplayPhaseB);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed in DisplayPhase()\n");
		return false;
	}

	/* Set Pixel Shader */
	hres=lpDev9->SetPixelShader(pPSDisplayPhaseB);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed in DisplayPhase()\n");
		return false;
	}

	// Render Object(s) and Floor

	lpDev9->SetTexture(0,lpClothTexture);

	lpDev9->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	RenderModel(lpDev9, &g_psMdl[5], 2, true);  // Cloth with dual stream (pos+uv) 4

	lpDev9->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);

	if (bWireframe)
	{
		lpDev9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	// Disable Vertex Textures

	hres =lpDev9->SetTexture(D3DVERTEXTEXTURESAMPLER0,NULL);
	hres|=lpDev9->SetTexture(D3DVERTEXTEXTURESAMPLER1,NULL);
	hres|=lpDev9->SetTexture(D3DVERTEXTEXTURESAMPLER2,NULL);	
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexTexture() failed in DisplayPhase()\n");
		return false;
	}

	lpDev9->SetSamplerState(0,D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	lpDev9->SetSamplerState(0,D3DSAMP_MINFILTER, D3DTEXF_POINT);
	lpDev9->SetSamplerState(0,D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	// Disable Z Buffer
	lpDev9->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	
	return true;
}

/*******************************************************************************
 * Function Name  : SetMRT0TexMRT1RT
 * Returns        : Error
 * Global Used    : 
 * Description    : Set MRT 0 as Tex (Input) and MRT 1 as Render Tgt (Output)
 *******************************************************************************/
bool SetMRT0TexMRT1RT(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT		hres;

	/* Set Textures */

	hres =  lpDev9->SetTexture(0, lpMRTTexture[0][0]);
	hres |= lpDev9->SetTexture(1, lpMRTTexture[0][1]);
	hres |= lpDev9->SetTexture(2, lpMRTTexture[0][2]);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetTexture() failed in SetMRT0TexMRT1RT()\n");
	}

	/* Set new Render Targets */

	hres =  lpDev9->SetRenderTarget(0, lpSurfaceMRT[1][0]);
	hres |= lpDev9->SetRenderTarget(1, lpSurfaceMRT[1][1]);
	hres |= lpDev9->SetRenderTarget(2, lpSurfaceMRT[1][2]);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetRenderTarget() failed in SetMRT0TexMRT1RT()\n");
	}

	return true;
}


/*******************************************************************************
 * Function Name  : SetMRT0TexMRT2RT
 * Returns        : Error
 * Global Used    : 
 * Description    : Set MRT 0 as Tex (Input) and MRT 2 as Render Tgt (Output)
 *******************************************************************************/
bool SetMRT0TexMRT2RT(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT		hres;

	/* Set Textures */

	hres =  lpDev9->SetTexture(0, lpMRTTexture[0][0]);
	hres |= lpDev9->SetTexture(1, lpMRTTexture[0][1]);
	hres |= lpDev9->SetTexture(2, lpMRTTexture[0][2]);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetTexture() failed in SetMRT0TexMRT2RT()\n");
	}

	/* Set new Render Targets */

	hres =  lpDev9->SetRenderTarget(0, lpSurfaceMRT[2][0]);
	hres |= lpDev9->SetRenderTarget(1, lpSurfaceMRT[2][1]);
	hres |= lpDev9->SetRenderTarget(2, lpSurfaceMRT[2][2]);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetRenderTarget() failed in SetMRT0TexMRT2RT()\n");
	}

	return true;
}

/*******************************************************************************
 * Function Name  : SetMRT1TexMRT0RT
 * Returns        : Error
 * Global Used    : 
 * Description    : Set MRT 1 as Tex (Input) and MRT 0 as Render Tgt (Output)
 *******************************************************************************/
bool SetMRT1TexMRT0RT(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT		hres;

	/* Set Textures */

	hres =  lpDev9->SetTexture(0, lpMRTTexture[1][0]);
	hres |= lpDev9->SetTexture(1, lpMRTTexture[1][1]);
	hres |= lpDev9->SetTexture(2, lpMRTTexture[1][2]);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetTexture() failed in SetMRT1TexMRT0RT()\n");
	}

	/* Set new Render Targets */

	hres =  lpDev9->SetRenderTarget(0, lpSurfaceMRT[0][0]);
	hres |= lpDev9->SetRenderTarget(1, lpSurfaceMRT[0][1]);
	hres |= lpDev9->SetRenderTarget(2, lpSurfaceMRT[0][2]);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetRenderTarget() failed in SetMRT1TexMRT0RT()\n");
	}

	return true;
}

/*******************************************************************************
 * Function Name  : CheckCaps
 * Returns        : Error
 * Global Used    : 
 * Description    : Make sure the current device can run this app - if not QUIT
 *******************************************************************************/
bool CheckCaps(LPDIRECT3DDEVICE9 lpDev9)
{

	D3DCAPS9		D3DCaps;

	/*******************
	** Check for caps **
	*******************/
	lpDev9->GetDeviceCaps(&D3DCaps);


	// Check MRT Support
	bSupportMRTs = FALSE;
	if (D3DCaps.NumSimultaneousRTs>=3)
	{
		bSupportMRTs=TRUE;
	}
	else
	{	
		OutputDebugString("Insufficient MRT support, exiting\n");
		MessageBox(NULL, "Insufficient MRT support\n\nPlease run with -RefDevice command line option.", "Error", MB_OK);
		return FALSE;
	}
	
	// Check Vertex Shader Support
	bSupportVS30=FALSE;
	if (D3DCaps.VertexShaderVersion>=D3DSHADER_VERSION_MAJOR(3))
	{
		bSupportVS30 = TRUE;
	}

	// Check Pixel Shader Support
	bSupportPS30 = FALSE;

	if (D3DCaps.PixelShaderVersion>=D3DPS_VERSION(3,0))		
	{
		bSupportPS30 = TRUE;
	}

	if (!bSupportVS30 || !bSupportPS30)
	{
		OutputDebugString("Application requires VS/PS3.0, exiting\n");
		MessageBox(NULL, "Application requires VS/PS3.0\n\nPlease run with -RefDevice command line option", "Error", MB_OK);
		return FALSE;
	}

	return true;
}

/*******************************************************************************
 * Function Name  : RenderScreenAlignedQuad
 * Returns        : Error
 * Global Used    : 
 * Description    : Render a screen-aligned quad
 *******************************************************************************/
bool RenderScreenAlignedQuad(LPDIRECT3DDEVICE9 lpDev9, 
									float fRenderWidth, float fRenderHeight,
									float fTextureWidth, float fTextureHeight)
{
	HRESULT		hres;
	XYZUVVERTEX	QuadVertices[4];
	float		fu0, fv0;
	float		fu1, fv1;

	/* Set FVF */
	lpDev9->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

	/* Compute UVs to use */
	fu0 = 0.0;
	fv0 = 0.0;
	fu1 = 1.0;
	fv1 = 1.0;

	/* Set quad vertices */
	QuadVertices[0].x = -1.0f;
	QuadVertices[0].y =  1.0f;
	QuadVertices[0].z =  0.0f;
	QuadVertices[0].tu = fu0;
	QuadVertices[0].tv = fv0;
	QuadVertices[1].x =  1.0f;
	QuadVertices[1].y =  1.0f;
	QuadVertices[1].z =  0.0f;
	QuadVertices[1].tu = fu1;
	QuadVertices[1].tv = fv0;
	QuadVertices[2].x = -1.0f;
	QuadVertices[2].y = -1.0f;
	QuadVertices[2].z =  0.0f;
	QuadVertices[2].tu = fu0;
	QuadVertices[2].tv = fv1;
	QuadVertices[3].x =  1.0f;
	QuadVertices[3].y = -1.0f;
	QuadVertices[3].z =  0.0f;
	QuadVertices[3].tu = fu1;
	QuadVertices[3].tv = fv1;

	/* Draw Quad */
	hres = lpDev9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, QuadVertices, sizeof(XYZUVVERTEX));
	if (hres!=D3D_OK)
	{
		OutputDebugString("DrawPrimitiveUP() failed in RenderScreenAlignedQuad()\n");
		return false;
	}

	return true;
}

/*****************************************************************************
 * Function Name  : LoadState
 * Inputs		  : None
 * Globals used   : Yes
 * Returns		  : Error
 * Description    : Loads current viewing state.
 *****************************************************************************/
bool LoadState()
{
	FILE					*f;
	char					pszString[128];
	int						nGrid=0, nNeighbors=0, nObj=0;

	if ( !(f=fopen(pszFileName, "rb")) )
	{
		sprintf(pszString, "*ERROR* : Unable to open object data.\n", pszFileName);
		MessageBox(NULL, "Failed to open Obj File, please use command line :\n\n-Object=<ObjName>.txt", "Error", MB_OK);
		OutputDebugString(pszString);
		return false;
	}

	fscanf(f, "Object: %d\n", &nObj);

	if ((nObj<1)||(nObj>7))
	{
		nObjSelected=1;
		sprintf(pszString, "Error in input file. Invalid Object using default.");
		OutputDebugString(pszString);
	}

	switch (nObj)
	{
		case 1: nObjSelected=1; break; 
		case 2: nObjSelected=7; break;
		case 3: nObjSelected=8; break;
		case 4: nObjSelected=9; break;
		case 5: nObjSelected=10; break;
		case 6: nObjSelected=11; break;
		case 7: nObjSelected=12; break;
	}
	

	fscanf(f, "Nodes in Grid: %d\n", &nGrid);


	switch (nGrid)
	{
		case 8 :  ClothWidth=8; ClothHeight=8; break;
		case 16 : ClothWidth=16; ClothHeight=16; break;
		case 32 : ClothWidth=32; ClothHeight=32; break;
		case 64 : ClothWidth=64; ClothHeight=64; break;
		case 128: ClothWidth=128; ClothHeight=128; break;
		default : ClothWidth=64; ClothHeight=64; 
				  sprintf(pszString, "Error in input file. Using Default Grid Size.");
			      OutputDebugString(pszString);
	}

	fscanf(f, "Cloth Size: %f\n", &fClothSize);
	fscanf(f, "Gravity: %f\n", &fGravity);
	fscanf(f, "Small Value A: %f, %f, %f, %f\n", &fSVA[0],&fSVA[1],&fSVA[2],&fSVA[3]);
	fscanf(f, "Small Value B: %f, %f, %f, %f\n", &fSVB[0],&fSVB[1],&fSVB[2],&fSVB[3]);
	fscanf(f, "Small Value C: %f, %f, %f, %f\n", &fSVC[0],&fSVC[1],&fSVC[2],&fSVC[3]);
	fscanf(f, "Small Value D: %f, %f, %f, %f\n", &fSVD[0],&fSVD[1],&fSVD[2],&fSVD[3]);
	fscanf(f, "Small Value E: %f, %f, %f, %f\n", &fSVE[0],&fSVE[1],&fSVE[2],&fSVE[3]);
	fscanf(f, "Small Value F: %f, %f, %f, %f\n", &fSVF[0],&fSVF[1],&fSVF[2],&fSVF[3]);
	fscanf(f, "Constraint Range: %f\n", &fConstraintRange);
	fscanf(f, "Neighbors: %d\n", &nNeighbors);

	switch (nNeighbors)
	{
		case 4 :  strcpy(pszPSStr,"./Shaders/PSClothPhase-04OPT.txt"); break;
		case 8 :  strcpy(pszPSStr,"./Shaders/PSClothPhase-08OPT.txt"); break;
		case 12 : strcpy(pszPSStr,"./Shaders/PSClothPhase-12OPT.txt"); break;
		case 16 : strcpy(pszPSStr,"./Shaders/PSClothPhase-16OPT.txt"); break;
		case 20 : strcpy(pszPSStr,"./Shaders/PSClothPhase-20OPT.txt"); break;
		case 24 : strcpy(pszPSStr,"./Shaders/PSClothPhase-24OPT.txt"); break;
		default : strcpy(pszPSStr,"./Shaders/PSClothPhase-16OPT.txt"); 
			sprintf(pszString, "Error in input file. Using Default Cloth Shader.");
			OutputDebugString(pszString);
	}

	fscanf(f, "Iterations per Frame: %d\n", &nIterPerFrame);

	/* Close file */
	if (fclose(f)!=0)
	{
		sprintf(pszString, "*WARNING* : Error in closing data.txt.\n");
		OutputDebugString(pszString);
	}

	return true;
}

/*******************************************************************************
 * Function Name  : ClGetValue
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Grab a command line string parameter
 *******************************************************************************/

void ClGetValue(const char * const szSrc, const char * const szTag, void * const pDest)
{
	const char	*pSrc, *pTag;
	BOOL		bIn;

	pSrc	= szSrc;
	pTag	= szTag;
	bIn		= FALSE;

	while(*pSrc) 
	{
		if(*pSrc == ' ' || *pSrc == '\t') 
		{
			pSrc++;
			bIn = FALSE;
			continue;
		}

		if(bIn == FALSE && *pSrc == *pTag) 
		{
			while(*pTag && *pSrc == *pTag) 
			{
				pSrc++;
				pTag++;
			}

			// Have we found our variable?
			if(*pTag == 0 && *pSrc == '=') 
			{
				pSrc++;
				char *pszDest = (char*)pDest;
				while(*pSrc && *pSrc != ' ' && *pSrc != '\t')
					*pszDest++ = *pSrc++;
				*pszDest = 0;
				return;
			}

			pTag	= szTag;
			bIn		= TRUE;
			continue;
		}

		bIn = TRUE;
		pSrc++;
	}
}

/*******************************************************************************
 * Function Name  : RenderLogo
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Render our PowerVR(tm) Logo to screen
 *******************************************************************************/
bool RenderLogo(LPDIRECT3DDEVICE9 lpDev9, 
									float fRenderWidth, float fRenderHeight,
									float fTextureWidth, float fTextureHeight)
{
	HRESULT		hres;
	XYZUVVERTEX	QuadVertices[4];
	float		fu0, fv0;
	float		fu1, fv1;

	/* Set FVF */
	lpDev9->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

	/* Compute UVs to use */
	fu0 = 1.0f/(2.0f*fRenderWidth);
	fv0 = 1.0f/(2.0f*fRenderHeight);
	fu1 = fTextureWidth/128.0f + fu0;
	fv1 = fTextureHeight/64.0f + fv0;

	/* Set quad vertices */
	QuadVertices[0].x = ((fRenderWidth*0.5f)-fTextureWidth)/(fRenderWidth*0.5f);
	QuadVertices[0].y = -1.0f*(((fRenderHeight*0.5f)-fTextureHeight)/(fRenderHeight*0.5f));
	QuadVertices[0].z =  0.0f;
	QuadVertices[0].tu = fu0;
	QuadVertices[0].tv = fv0;
	QuadVertices[1].x =  1.0f;
	QuadVertices[1].y =  -1.0f*(((fRenderHeight*0.5f)-fTextureHeight)/(fRenderHeight*0.5f));
	QuadVertices[1].z =  0.0f;
	QuadVertices[1].tu = fu1;
	QuadVertices[1].tv = fv0;
	QuadVertices[2].x = ((fRenderWidth*0.5f)-fTextureWidth)/(fRenderWidth*0.5f);
	QuadVertices[2].y = -1.0f;
	QuadVertices[2].z =  0.0f;
	QuadVertices[2].tu = fu0;
	QuadVertices[2].tv = fv1;
	QuadVertices[3].x =  1.0f;
	QuadVertices[3].y = -1.0f;
	QuadVertices[3].z =  0.0f;
	QuadVertices[3].tu = fu1;
	QuadVertices[3].tv = fv1;

	/* Draw Logo */
	hres = lpDev9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, QuadVertices, sizeof(XYZUVVERTEX));
	if (hres!=D3D_OK)
	{
		OutputDebugString("DrawPrimitiveUP() failed in RenderScreenAlignedQuad()\n");
		return false;
	}

	return true;
}


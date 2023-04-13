/*****************************************************************************
  Name : ShaderX2MRT.cpp
  Date : 12/03/03
  
  ShaderX2MRT.cpp is an example of a scene to be used with D3DShell. 
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
#include <d3d9.h>		/* D3D library */
#include <d3dx9.h>		/* D3DX library */
#include "Shadow.h"		/* Stencil shadow functions */
#include "D3DShell.h"	/* D3DShell header file */
#include "D3DTools.h"	/* D3DTools header file */
#include "resource.h"	/* Resource header file */
#include "mrts.h"		/* Scene data */

/* Set to 0 to use GDI menus */
#define USE3DMENUS 0

/* Redefine total number of lights */
#define TOTAL_NUM_LIGHTS	NUM_LIGHTS



/****************************************************************************
** Defines
****************************************************************************/
#define PI				(3.1415927f)

#define WND_TITLE		"Deferred Shading with Multiple Render Targets"

#define STENCIL_REF		128



/****************************************************************************
** Macros
****************************************************************************/
#define SWAP(x)					(x)=!(x)

#define CLAMP(x, min, max)		( (x)<min ? min : ( (x)>max ? (max) : (x) ) )

#define FREE(X)					{ if(X) { free(X); (X) = 0; } }

#ifndef D3D_RGBA
#define D3D_RGBA(r, g, b, a)   ((DWORD) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))
#endif


/****************************************************************************
** Structures
****************************************************************************/
/* Vertex structures */
typedef struct XYZVERTEX_TAG
{
	float		x, y, z;
} XYZVERTEX;

typedef struct XYZDIFFUSEVERTEX_TAG
{
	float		x, y, z;
	DWORD		diffuse;
} XYZDIFFUSEVERTEX;

typedef struct XYZUVVERTEX_TAG
{
	float		x, y, z;
	float		tu, tv;
} XYZUVVERTEX;

typedef struct XYZWUVVERTEX_TAG
{
	float		x, y, z, w;
	float		tu, tv;
} XYZWUVVERTEX;

typedef struct TSUVVERTEX_TAG
{
	float		Bx, By, Bz;			// Binormal vector
	float		Tx, Ty, Tz;			// Tangent vector
	float		Nx, Ny, Nz;			// Normal vector
	float		tu, tv;				// Mesh texture coordinates
} TSUVVERTEX;

typedef struct TEMPVERTEX_TAG
{
	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
	float bx, by, bz;
	float tx, ty, tz;
} TEMPVERTEX; 

typedef enum _LIGHTSHAPE
{
	FULLSCREEN, SPHERE, CONE
} LIGHTSHAPE;

typedef struct LIGHTINFO_TAG
{
	LIGHTSHAPE	eShape;										/* Light shape (FULLSCREEN, SPHERE or CONE) */

	float		fIntensity;									/* Light intensity */

	int			nNumVertex;									/* Number of vertices in vertex buffer */
	WORD		wNumIndices;								/* Number of indices in index buffer */

	D3DPRIMITIVETYPE		ePrimitiveType;					/* Primitive type */
	LPDIRECT3DVERTEXBUFFER9	lpVBLightShape;					/* Vertex buffer */
	LPDIRECT3DINDEXBUFFER9	lpIBLightShape;					/* Index buffer */

	/* Cone lights only */
	float		fConeBottomRadius;							/* Cone bottom radius */
	float		fConeTopRadius;								/* Cone top radius */
	float		fConeHeight;								/* Cone height */
	
	D3DVECTOR	InitialPosition;							/* Initial position of light in world space */
	D3DVECTOR	InitialTarget;								/* Initial target of light in world space */
	
	D3DVECTOR	TPosition;									/* Transformed position of light in world space */
	D3DVECTOR	TTarget;									/* Transformed target of light in world space */

	float		fMaxRange;									/* Max range of light */
} LIGHTINFO;


typedef struct 
{
	SShadowMesh				sShadowMesh;					/* Model definition suitable for shadow volumes */
	SShadowVol				psShadowVol[TOTAL_NUM_LIGHTS];	/* Geometry of actual shadow volume per light */

	int						nNumVertex;						/* Number of vertices */
	int						nNumFaces;						/* Number of triangles */

	D3DMATRIX				mWorld;							/* World-transform for object */
	D3DVECTOR				vPos, vRot;						/* Position of object */

	LPDIRECT3DVERTEXBUFFER9	lpVertexBufferXYZ;				/* Vertex buffer for vertex position */
	LPDIRECT3DVERTEXBUFFER9	lpVertexBufferTSUV;				/* Vertex buffer for vertex tangent space and texture coordinates */
	LPDIRECT3DINDEXBUFFER9	lpIndexBuffer;					/* Index buffer */
} SModel;



/****************************************************************************
** Constants
****************************************************************************/
const static D3DXVECTOR3 g_UpVector( 0.0f, 1.0f, 0.0f );


/****************************************************************************
** Prototypes
****************************************************************************/
static void RenderModel(LPDIRECT3DDEVICE9 lpDev9, SModel *psMdl, int nLight);
static void LoadState();
static void SaveState();
static BOOL LoadTextures(LPDIRECT3DDEVICE9 lpDev9);
static void DestroyTextures();
static BOOL CreateFallOffTexture(LPDIRECT3DDEVICE9 lpDev9, int x, float fOrder, LPDIRECT3DTEXTURE9 *lplpTextureName);
static BOOL CreatePowerLookupTexture(LPDIRECT3DDEVICE9 lpDev9, int nSize, float fMaxPower, LPDIRECT3DTEXTURE9 *lplpTextureName);
static BOOL CreateNormalCubeMap(LPDIRECT3DDEVICE9 lpDev9, int nSize, LPDIRECT3DCUBETEXTURE9 *lplpTextureName);
static BOOL CreateNormalMapFromHeightMap(LPDIRECT3DDEVICE9 lpDev9, char *pszFileName, float fHeightScale, BOOL bMipmap, LPDIRECT3DTEXTURE9 *lplpTextureName);
static void GetUserInput();
static void DoAnimation();
static void BackTransform(D3DVECTOR *pvTrans, D3DVECTOR *pvOrig, int nCount, D3DMATRIX *pmWorld);
static void DrawParticle(LPDIRECT3DDEVICE9 lpDev9, float fX, float fY, float fZ, float fRadius);
static void RenderShadows(LPDIRECT3DDEVICE9 lpDev9, const int nLight);
static void CreateVertexShaders(LPDIRECT3DDEVICE9 lpDev9);
static void DestroyVertexShaders();
static void CreatePixelShaders(LPDIRECT3DDEVICE9 lpDev9);
static void DestroyPixelShaders();
static BOOL RenderMRT(LPDIRECT3DDEVICE9 lpDev9);
static void RenderScreenAlignedQuad(LPDIRECT3DDEVICE9 lpDev9, float fRenderWidth, float fRenderHeight, float fTextureWidth, float fTextureHeight);
static void RenderLightSource(LPDIRECT3DDEVICE9 lpDev9, int nLight, BOOL bDrawLightDirection);
static void DrawTangentSpace(LPDIRECT3DDEVICE9 lpDev9, int nMesh);
static void RenderLogo(LPDIRECT3DDEVICE9 lpDev9);
static void RenderInfoText(LPDIRECT3DDEVICE9 lpDev9);
static void CreateSphereVertexBuffer(LPDIRECT3DDEVICE9 lpDev9, float fRadius, LPDIRECT3DVERTEXBUFFER9 *lplpVBSphere, int *pnNumVertex);
static void CreateConeVertexAndIndexBuffers(LPDIRECT3DDEVICE9 lpDev9, float r, float R, float H,
											LPDIRECT3DVERTEXBUFFER9 *lplpVBCone, int *pnNumVertex,
											LPDIRECT3DINDEXBUFFER9 *lplpIBCone, WORD *pwNumIndices);
static void ComputeRotationMatrixFromTwoVectors(D3DXVECTOR3 *pV1, D3DXVECTOR3 *pV2, D3DXMATRIX *pMatrix);
static BOOL IsMeshShadowCaster(int nMesh);
static BOOL IsMeshShadowedByHeadLight(int nMesh);
static void UpdateMenu();


/****************************************************************************
** Globals
****************************************************************************/

/* Vertex declaration for temporary buffer (used for tangent space conversion) */
D3DVERTEXELEMENT9 declTemp[] =
{
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
		{ 0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL, 0}, 
		{ 0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
		{ 0,  32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BINORMAL, 0},
		{ 0,  44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TANGENT, 0},
        D3DDECL_END()
};

/* Object information */
static SModel						g_psMdl[NUM_MESHES];

/* Light-related variables */
static LIGHTINFO					LightInfo[TOTAL_NUM_LIGHTS];
static DWORD						g_dwLightNum = TOTAL_NUM_LIGHTS;
static float						FullscreenLightBorderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
static float						CurrentLightIntensity[TOTAL_NUM_LIGHTS];
static int							nSelectedLight=0;
static int							nCurrentActiveLight=8;
static BOOL							bModifyTarget=FALSE;
static BOOL							bDrawLight[9] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };
static BOOL							bDrawLightDirection=FALSE;
static BOOL							bDrawLightZones = FALSE;
static BOOL							bCyclingLights=TRUE;
static BOOL							bVisualiseVolumes = FALSE;
static BOOL							bDrawShadow=FALSE;

/* Textures-related variables */
static LPDIRECT3DCUBETEXTURE9		lpNormalizationCubeMap;
static LPDIRECT3DTEXTURE9			lpFallOffSharp;
static LPDIRECT3DTEXTURE9			lpDiffuseMap[NUM_MATERIALS];
static LPDIRECT3DTEXTURE9			lpNormalMap[NUM_MATERIALS];
static LPDIRECT3DTEXTURE9			lpLightMap;
static LPDIRECT3DTEXTURE9			lpLightTexture;
static LPDIRECT3DTEXTURE9			lpPowerLookUpTexture;
static LPDIRECT3DTEXTURE9			lpPowerVRLogoTexture;
static int							nTextureFilter = D3DTEXF_LINEAR;
static int							nMipFilter = D3DTEXF_LINEAR;
static BOOL							bUseCompressedTextures=FALSE;
static BOOL							bUse16BitsTextures=FALSE;
static BOOL							bSupportA2R10G10B10=FALSE;

/* Vertex shader-related variables */
static LPDIRECT3DVERTEXBUFFER9		lpVSVB;
static LPDIRECT3DVERTEXDECLARATION9 pVSDeclPosition;
static LPDIRECT3DVERTEXDECLARATION9 pVSDeclPositionTSUVTwoStreams;
static LPDIRECT3DVERTEXDECLARATION9 pVSDeclTPositionUV;
static LPDIRECT3DVERTEXSHADER9		pVSTransformOnly;
static LPDIRECT3DVERTEXSHADER9		pVSInitialPass;
static LPDIRECT3DVERTEXSHADER9		pVSLightPassShape;
static BOOL							bSupportVS20;

/* Pixel shader-related variables */
static LPDIRECT3DPIXELSHADER9		pPSInitialPass;
static LPDIRECT3DPIXELSHADER9		pPSLightPassFullScreen;
static LPDIRECT3DPIXELSHADER9		pPSLightPassSphereShape;
static LPDIRECT3DPIXELSHADER9		pPSLightPassConeShape;
static LPDIRECT3DPIXELSHADER9		pPSViewMRTPosition;
static LPDIRECT3DPIXELSHADER9		pPSViewMRTNormal;
static BOOL							bSupportPS20;

/* MRT-related variables */
static LPDIRECT3DTEXTURE9			pMRT1616XY;
static LPDIRECT3DTEXTURE9			pMRT1616Z;
static LPDIRECT3DTEXTURE9			pMRT8888Normal;
static LPDIRECT3DTEXTURE9			pMRTDiffuse;
static LPDIRECT3DSURFACE9			lpMRT0, lpMRT1, lpMRT2, lpMRT3;		
static DWORD						dwMRTWidth, dwMRTHeight;

/* Camera-related variables */
static int							nCurrentCamera = 0;
static float						fFreeCamX, fFreeCamY;
static float						fCameraSpeed=20.0f;
static BOOL							bFreeCamera=FALSE;

/* User interface-related variables and booleans */
static HMENU						hUserMenu;
static int							nViewMRTMode=0;
static int							nTextOn=1;
static char							pszTmp[512];
static BOOL							bPause = FALSE;
static BOOL							bMenuHasChanged=TRUE;
static BOOL							bSupportTwoSidedStencil=FALSE;
static BOOL							bDrawTangentSpace=FALSE;
static BOOL							bStep=FALSE;

/* Other rendering variables */
static LPDIRECT3DDEVICE9			lpGlobalDev9=NULL;
static D3DMATRIX					ProjectionMatrix, ViewMatrix, TextureProjectionMatrix, ProjectionMatrixForLightShape;
static DWORD						dwCurrentWidth, dwCurrentHeight;




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
	char			*pszOption, pszValue[32];
	char			pszOrgCommandLine[256];

	/* You can use D3DShellSetPreferences(...) to set preferences for your application. 
	   You can specify a name, pass a menu, an accelerator table, an application icon 
	   and specify default rendering variables that will be used with your program : */

	/* Load user menu */
	hUserMenu = LoadMenu(hInstance, "MENU");

	/* Set application preferences */
	D3DShellSetPreferences(WND_TITLE,
						   hUserMenu,
						   LoadAccelerators(hInstance, "ACCEL"),
						   LoadIcon(hInstance, "ICO_PVR"),
						   DEFAULT_NO_VSYNC |
						   DEFAULT_CLEAR_ON |
				  		   USE_STENCIL_BUFFER |
						   FORCE_MULTISAMPLING_OFF |
#if USE3DMENUS == 1
						   USE_3D_MENUS | DISPLAY_PRINT3D_ICONS |
#endif
						   0);

	/* Default window size is 1024x768 */
	D3DShellSetPreferences("_DefaultWindowWidth", 0, 0, 0, 800);
	D3DShellSetPreferences("_DefaultWindowHeight", 0, 0, 0, 600);

	/* Default clear color is black */
	D3DShellSetPreferences("_DefaultClearColor", 0, 0, 0, 0xFF000000);

	/* Read command line parameters */
	strcpy(pszOrgCommandLine, pszCommandLine);
	_strupr(pszOrgCommandLine);
	pszOption=strtok(pszCommandLine, " -");
    while(pszOption!=NULL)   
	{
		_strupr(pszOption);
		if (strstr(pszOption, "TEXTUREFORMAT="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[14]);
	
			if (!strcmp(pszValue, "DXT"))
			{
				bUseCompressedTextures = TRUE;
			}
			else if (!strcmp(pszValue, "16BITS"))
			{
				bUseCompressedTextures = FALSE;
				bUse16BitsTextures = TRUE;
			}
			else if (!strcmp(pszValue, "32BITS"))
			{
				bUseCompressedTextures = FALSE;
				bUse16BitsTextures = FALSE;
			}
        }

		if (strstr(pszOption, "FILTERING="))
		{
			/* Get value */
			strcpy(pszValue, &pszOption[10]);

			if (!strcmp(pszValue, "TRILINEAR"))
			{
				nTextureFilter = D3DTEXF_LINEAR;
				nMipFilter = D3DTEXF_LINEAR;
			} 
			else if (!strcmp(pszValue, "BILINEAR"))
			{
				nTextureFilter = D3DTEXF_LINEAR;
				nMipFilter = D3DTEXF_NONE;
			}
			else if (!strcmp(pszValue, "NEAREST"))
			{
				nTextureFilter = D3DTEXF_POINT;
				nMipFilter = D3DTEXF_NONE;
			}
        }

		pszOption=strtok(NULL, " -");
    }

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


	/* Create shadow volumes */
	for (nCurr=0; nCurr<NUM_MESHES; nCurr++) 
	{
		/* Create the shadow-mesh for each shadow-caster object */
		if (IsMeshShadowCaster(nCurr))
		{
			ShadowMeshCreate(Mesh[nCurr].pVertex,	Mesh[nCurr].nNumVertex,
							 Mesh[nCurr].pFaces,	Mesh[nCurr].nNumFaces,
							 &g_psMdl[nCurr].sShadowMesh,
							 g_psMdl[nCurr].psShadowVol,
							 sizeof(g_psMdl[nCurr].psShadowVol) / sizeof(*g_psMdl[nCurr].psShadowVol));
		}
	}
	ShadowMeshCreateComplete();

	
	/* Adjust camera FOVs */
	for (nCurr=0; nCurr<NUM_CAMERAS; nCurr++)
	{
		/* Convert exported horizontal FOV to vertical, assuming 4:3 aspect ratio. */
		Camera[nCurr].fFOV *= 0.75f;
	}

	/* Initialise mouse */
	D3DTMouseInit(hInstance, hWindow);
}


/****************************************************************************
** QuitApplication() is called by D3DShell to enable user to release      **
** any memory before quitting the program.								   **
****************************************************************************/
void QuitApplication()
{
	int	nCurr;

	/* Release mouse */
	D3DTMouseShutdown();

	/* Destroy shadows */
	for (nCurr = 0; nCurr < NUM_MESHES; nCurr++) 
	{
		if (IsMeshShadowCaster(nCurr))
		{
			ShadowMeshDestroy(&g_psMdl[nCurr].sShadowMesh, g_psMdl[nCurr].psShadowVol, TOTAL_NUM_LIGHTS);
		}
	}
	ShadowMeshDestroyComplete();
}


/*******************************************************************************
** UserWindowProc(...) is the application's window messages handler.
** From that function you can process menu inputs, keystrokes, timers, etc...
** When processing keystrokes, DO NOT process ESCAPE key 
** (VK_ESCAPE), as it is already used by D3DShell.
********************************************************************************/
void UserWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	D3DXVECTOR3 v;
	int			i;

	/* You can process normal window messages from that function (WM_COMMAND
	   to retrieve menu inputs, WM_KEYDOWN to retrieve keystrokes, etc...) */
	switch(message) 
	{
		case WM_KEYDOWN:	switch(wParam)
							{
								case 'E':		SWAP(bFreeCamera); break;

#if 0
								case 'I' :		SWAP(bDrawLightDirection); break;
								case 'O':		SaveState(); break;
								
								case 'P':		BOOL bLocalUseCompressedTextures;
												BOOL bLocalUse16BitsTextures;

												bLocalUseCompressedTextures = bUseCompressedTextures;
												bLocalUse16BitsTextures = bUse16BitsTextures;
												LoadState(); 
												if (bUseCompressedTextures!=bLocalUseCompressedTextures ||
													bUse16BitsTextures!=bLocalUse16BitsTextures)
												{
													/* Reload textures with new format */
													DestroyTextures();
													LoadTextures(lpGlobalDev9);
												}

												/* Free Camera */
												v.x = Camera[nCurrentCamera].fTarget[0] - Camera[nCurrentCamera].fPosition[0];
												v.y = Camera[nCurrentCamera].fTarget[1] - Camera[nCurrentCamera].fPosition[1];
												v.z = Camera[nCurrentCamera].fTarget[2] - Camera[nCurrentCamera].fPosition[2];
												fFreeCamX = -(float)atan2(v.y, sqrt(v.x*v.x + v.z*v.z));
												fFreeCamY = (float)atan2(v.x, v.z);

												break;
							
								case 'L' :		SWAP(bModifyTarget);
												break;

								case VK_ADD :	/* Toggle Light */
												if (nSelectedLight<TOTAL_NUM_LIGHTS-1) nSelectedLight++;
												break;

								case VK_SUBTRACT:	/* Toggle Light */
													if (nSelectedLight>0) nSelectedLight--;
													break;
#endif
							}
							break;

		case WM_COMMAND:	switch(LOWORD(wParam)) 
							{
								case ID_OPTIONS_INFO:						nTextOn=(++nTextOn)%2; 
																			break;

								case ID_OPTIONS_PAUSESTEP:					if (bPause)
																			{
																				bStep=TRUE;
																			}
																			bPause=TRUE;
																			bMenuHasChanged=TRUE; 
																			break;

								case ID_OPTIONS_RESUME:						bPause=FALSE; 
																			bMenuHasChanged=TRUE; 
																			break;

								case ID_OPTIONS_SHADOWS:					bMenuHasChanged=TRUE;
																			SWAP(bDrawShadow); break;

								case ID_OPTIONS_VISUALISEVOLUMES:			bMenuHasChanged=TRUE;
																			SWAP(bVisualiseVolumes); break;

								case ID_OPTIONS_TEXTUREFORMAT_32BITS:		bMenuHasChanged=TRUE;
																			bUse16BitsTextures=FALSE;
																			bUseCompressedTextures=FALSE;
																			DestroyTextures();
																			LoadTextures(lpGlobalDev9);
																			break;

								case ID_OPTIONS_TEXTUREFORMAT_16BITS:		bMenuHasChanged=TRUE;
																			bUse16BitsTextures=TRUE;
																			bUseCompressedTextures=FALSE;
																			DestroyTextures();
																			LoadTextures(lpGlobalDev9);
																			break;

								case ID_OPTIONS_TEXTUREFORMAT_DXT:			bMenuHasChanged=TRUE;
																			bUse16BitsTextures=FALSE;
																			bUseCompressedTextures=TRUE;
																			DestroyTextures();
																			LoadTextures(lpGlobalDev9);
																			break;

								case ID_OPTIONS_FILTERING_POINTSAMPLING:	bMenuHasChanged=TRUE;
																			nTextureFilter=D3DTEXF_POINT;
																			nMipFilter=D3DTEXF_NONE; 
																			break;

								case ID_OPTIONS_FILTERING_BILINEAR:			bMenuHasChanged=TRUE;	
																			nTextureFilter=D3DTEXF_LINEAR;
																			nMipFilter=D3DTEXF_NONE; 
																			break;

								case ID_OPTIONS_FILTERING_TRILINEAR:		bMenuHasChanged=TRUE;
																			nTextureFilter=D3DTEXF_LINEAR;
																			nMipFilter=D3DTEXF_LINEAR; 
																			break;

								case ID_OPTIONS_FILTERING_ANISOTROPIC:		bMenuHasChanged=TRUE;
																			nTextureFilter=D3DTEXF_ANISOTROPIC;
																			nMipFilter=D3DTEXF_LINEAR; 
																			break;

								case ID_OPTIONS_TOGGLECAMERA:				/* Toggle Camera */
																			nCurrentCamera = (++nCurrentCamera) % NUM_CAMERAS;
																			
																			v.x = Camera[nCurrentCamera].fTarget[0] - Camera[nCurrentCamera].fPosition[0];
																			v.y = Camera[nCurrentCamera].fTarget[1] - Camera[nCurrentCamera].fPosition[1];
																			v.z = Camera[nCurrentCamera].fTarget[2] - Camera[nCurrentCamera].fPosition[2];
																			
																			fFreeCamX = -(float)atan2(v.y, sqrt(v.x*v.x + v.z*v.z));
																			fFreeCamY = (float)atan2(v.x, v.z);
																			break;
																			
																			
								case ID_OPTIONS_NORMAL :					bMenuHasChanged=TRUE; nViewMRTMode = 0; break;
								case ID_OPTIONS_VIEWPOSITIONMRT:			bMenuHasChanged=TRUE; nViewMRTMode = 1; break;
								case ID_OPTIONS_VIEWNORMALMRT:				bMenuHasChanged=TRUE; nViewMRTMode = 2; break;
								case ID_OPTIONS_VIEWDIFFUSEMRT:				bMenuHasChanged=TRUE; nViewMRTMode = 3; break;
																			
								case ID_OPTIONS_VIEWTANGENTSPACE:			bMenuHasChanged=TRUE;  SWAP(bDrawTangentSpace); break;
																			
								case ID_LIGHTS_VIEWLIGHTZONES:				bMenuHasChanged=TRUE;  SWAP(bDrawLightZones); break;
																			
								case ID_LIGHTS_CYCLINGLIGHTS:				bMenuHasChanged=TRUE;  SWAP(bCyclingLights); break;
																			
								case ID_LIGHTS_ENABLEALLLIGHTS:				bMenuHasChanged=TRUE; bCyclingLights=FALSE;
																			for (i=0; i<9; i++) bDrawLight[i]=TRUE;  
																			break;
																			
								case ID_LIGHTS_FULLSCREENLIGHT:				bMenuHasChanged=TRUE; SWAP(bDrawLight[0]); break;
								case ID_LIGHTS_SPHERELIGHT1:				bMenuHasChanged=TRUE; SWAP(bDrawLight[1]); break;
								case ID_LIGHTS_SPHERELIGHT2:				bMenuHasChanged=TRUE; SWAP(bDrawLight[2]); break;
								case ID_LIGHTS_CONELIGHT1:					bMenuHasChanged=TRUE; SWAP(bDrawLight[3]); break;
								case ID_LIGHTS_CONELIGHT2:					bMenuHasChanged=TRUE; SWAP(bDrawLight[4]); break;
								case ID_LIGHTS_CONELIGHT3:					bMenuHasChanged=TRUE; SWAP(bDrawLight[5]); break;
								case ID_LIGHTS_CONELIGHT4:					bMenuHasChanged=TRUE; SWAP(bDrawLight[6]); break;
								case ID_LIGHTS_CONELIGHT5:					bMenuHasChanged=TRUE; SWAP(bDrawLight[7]); break;
								case ID_LIGHTS_CONELIGHT6:					bMenuHasChanged=TRUE; SWAP(bDrawLight[8]); break;
							}
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
	/* Update menu if required */
	if (bMenuHasChanged)
	{
		UpdateMenu();
		bMenuHasChanged=FALSE;
	}


	/***************
	** USER INPUT **
	***************/
	if (bFreeCamera)
	{
		/* Only retrieve user input if free camera is enabled */
		GetUserInput();
	}
	

	/********************
	** SET VIEW MATRIX **
	********************/
	D3DXMatrixLookAtLH((D3DXMATRIX *)&ViewMatrix, 
					   (D3DXVECTOR3 *)&Camera[nCurrentCamera].fPosition, 
					   (D3DXVECTOR3 *)&Camera[nCurrentCamera].fTarget, 
					   &g_UpVector);
	lpDev9->SetTransform(D3DTS_VIEW, &ViewMatrix);


	/**************
	** ANIMATION **
	**************/
	DoAnimation();


	/****************
	** Begin Scene **
	****************/
	lpDev9->BeginScene();


	/* Render MRT */
	RenderMRT(lpDev9);


	/* Render logo */
	RenderLogo(lpDev9);


	/* Render info text */
	RenderInfoText(lpDev9);



#if USE3DMENUS == 1
	/* Display 3D menus */
	D3DShellDisplay3DMenus();
#endif


	/**************
	** End Scene **
	**************/
    lpDev9->EndScene();

	
	/* The function should ALWAYS return TRUE if the rendering was successful.
	   If D3DShell receives FALSE from this function, then rendering will be
	   stopped and the application will be terminated */
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
	HRESULT							hres;
	D3DDEVICE_CREATION_PARAMETERS	CreationParameters;
	D3DCAPS9						D3DCaps;
	int								nCurr, i;
	XYZVERTEX						*pVertexXYZ;
	TSUVVERTEX						*pVertexTSUV;
	WORD							*pIndex;
	DWORD							dwVert;
	int								nOutputVertexCount;
	TEMPVERTEX						*pTempVertexBuffer=NULL, *pOutputVertex=NULL;
	D3DDISPLAYMODE					CurrentDisplayMode;
	WORD							*pTempIndexBuffer;


	/****************************
	** Update global variables **
	****************************/
	lpGlobalDev9 = lpDev9;
	dwCurrentWidth = dwWidth;
	dwCurrentHeight = dwHeight;
	bMenuHasChanged=TRUE;


	/*******************
	** Check for caps **
	*******************/
	lpDev9->GetDeviceCaps(&D3DCaps);

	/* Does this device support cube maps? */
	if (!(D3DCaps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		OutputDebugString("Cube mapping not supported, exiting\n");
		MessageBox(NULL, "Cube mapping not supported, exiting\n", "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	/* Vertex and pixel shader support */
	bSupportVS20 = (D3DCaps.VertexShaderVersion>=D3DVS_VERSION(2,0));
	bSupportPS20 = (D3DCaps.PixelShaderVersion>=D3DPS_VERSION(2,0));


	/* Check for minimum Vertex and Pixel Shader support */
	if (!bSupportVS20 || !bSupportPS20)
	{
		OutputDebugString("Vertex and Pixel Shader 2.0 or above are required to run this demo\n");
		MessageBox(NULL, "Vertex and Pixel Shader 2.0 or above are required to run this demo", "ERROR", MB_OK);
		return FALSE;
	}

	/* Two-sided stencil suppprt */
	bSupportTwoSidedStencil = (D3DCaps.StencilCaps & D3DSTENCILCAPS_TWOSIDED);
	if (bSupportTwoSidedStencil)
	{
		OutputDebugString("3D device supports two-sided stencil\n");
	}


	/* MRT support */
	if (D3DCaps.NumSimultaneousRTs<4)
	{
		OutputDebugString("Four Multiple Render Targets are required to run this demo.\n");
		MessageBox(NULL, "Four Multiple Render Targets are required to run this demo.", "ERROR", MB_OK);
		return FALSE;
	}

	/* NON-POW2 CONDITIONAL support */
	if (!(D3DCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL))
	{
		OutputDebugString("Non-POW2 conditional textures are required to run this demo.\n");
		MessageBox(NULL, "Non-POW2 conditional textures are required to run this demo", "ERROR", MB_OK);
		return FALSE;
	}


	/* D3DFMT_A2R10G10B10 support (used to store normals) */
	hres = lpDev9->GetCreationParameters(&CreationParameters);
	hres = lpD3D9->GetAdapterDisplayMode(CreationParameters.AdapterOrdinal, &CurrentDisplayMode);
	if ( (lpD3D9->CheckDeviceFormat(CreationParameters.AdapterOrdinal, CreationParameters.DeviceType, CurrentDisplayMode.Format,
								  D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A2R10G10B10)==D3D_OK) && 
								  (lpD3D9->CheckDeviceFormat(CreationParameters.AdapterOrdinal, CreationParameters.DeviceType, CurrentDisplayMode.Format,
								  D3DUSAGE_RENDERTARGET, D3DRTYPE_CUBETEXTURE, D3DFMT_A2R10G10B10)==D3D_OK) )
	{
		bSupportA2R10G10B10=TRUE;
	}
	else
	{
		bSupportA2R10G10B10=FALSE;
	}

			

	/********************************
	** Initialise Print3D textures **
	********************************/
	D3DTPrint3DSetTextures(lpDev9, dwWidth, dwHeight);
	
	
	/****************
	** Create MRTs **
	****************/
	/* Use non-POW2 size for MRTs */
	dwMRTWidth = dwCurrentWidth;
	dwMRTHeight = dwCurrentHeight;

	/* Create MRTs */
	hres = lpDev9->CreateTexture(dwMRTWidth, dwMRTHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G16R16F, D3DPOOL_DEFAULT, &pMRT1616XY, NULL);
	hres |= lpDev9->CreateTexture(dwMRTWidth, dwMRTHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G16R16F, D3DPOOL_DEFAULT, &pMRT1616Z, NULL);
	hres |= lpDev9->CreateTexture(dwMRTWidth, dwMRTHeight, 1, D3DUSAGE_RENDERTARGET, bSupportA2R10G10B10 ? D3DFMT_A2R10G10B10 : D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pMRT8888Normal, NULL);
	hres |= lpDev9->CreateTexture(dwMRTWidth, dwMRTHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pMRTDiffuse, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to create MRT textures\n");
		MessageBox(NULL, "Failed to create MRT textures", "ERROR", MB_OK);
		D3DTPrint3DReleaseTextures();
		return FALSE;
	}

	/* Retrieve 1st level surfaces of MRTs */
	hres =  pMRT1616XY->GetSurfaceLevel(0, &lpMRT0);
	hres |= pMRT1616Z->GetSurfaceLevel(0, &lpMRT1);
	hres |= pMRT8888Normal->GetSurfaceLevel(0, &lpMRT2);
	hres |= pMRTDiffuse->GetSurfaceLevel(0, &lpMRT3);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to retrieve top level surfaces of MRTs\n");
		MessageBox(NULL, "Failed to retrieve top level surfaces of MRTs", "ERROR", MB_OK);
		D3DTPrint3DReleaseTextures();
		return FALSE;
	}


	/*************************
	** Create geometry data **
	*************************/
	for (nCurr=0; nCurr<NUM_MESHES; nCurr++) 
	{
		/* Object position */
		g_psMdl[nCurr].vPos = *(D3DVECTOR*)Mesh[nCurr].fCenter;
		g_psMdl[nCurr].vRot.x = 0;
		g_psMdl[nCurr].vRot.y = 0;
		g_psMdl[nCurr].vRot.z = 0;

		/* Save info */
		g_psMdl[nCurr].nNumVertex =		Mesh[nCurr].nNumVertex;
		g_psMdl[nCurr].nNumFaces =		Mesh[nCurr].nNumFaces;

		/* Allocate temporary buffers to prepare vertex data for input to tangent space function */
		pTempVertexBuffer = (TEMPVERTEX *)malloc(Mesh[nCurr].nNumVertex * sizeof(TEMPVERTEX));
		pTempIndexBuffer = (WORD *)malloc(3 * Mesh[nCurr].nNumFaces * sizeof(WORD));
		if (!pTempVertexBuffer || !pTempIndexBuffer)
		{
			OutputDebugString("Not enough memory to allocate temp buffers\n");
			MessageBox(NULL, "Not enough memory to allocate temp buffers", "ERROR", MB_OK);
			FREE(pTempIndexBuffer);
			FREE(pTempVertexBuffer);
			D3DTPrint3DReleaseTextures();
			return FALSE;
		}

		/* Copy vertex data into temporary buffer */
		for (i=0; i<(int)Mesh[nCurr].nNumVertex; i++)
		{
			pTempVertexBuffer[i].x = Mesh[nCurr].pVertex[3*i+0];
			pTempVertexBuffer[i].y = Mesh[nCurr].pVertex[3*i+1];
			pTempVertexBuffer[i].z = Mesh[nCurr].pVertex[3*i+2];

			pTempVertexBuffer[i].nx = Mesh[nCurr].pNormals[3*i+0];
			pTempVertexBuffer[i].ny = Mesh[nCurr].pNormals[3*i+1];
			pTempVertexBuffer[i].nz = Mesh[nCurr].pNormals[3*i+2];

			pTempVertexBuffer[i].tu = Mesh[nCurr].pUV[2*i+0];
			pTempVertexBuffer[i].tv = Mesh[nCurr].pUV[2*i+1];
		}

		/* Copy index data into temporary buffer */
		for (i=0; i<(int)Mesh[nCurr].nNumFaces; i++)
		{
			pTempIndexBuffer[3*i+0] = Mesh[nCurr].pFaces[3*i+0];
			pTempIndexBuffer[3*i+1] = Mesh[nCurr].pFaces[3*i+1];
			pTempIndexBuffer[3*i+2] = Mesh[nCurr].pFaces[3*i+2];
		}

		/* Generate tangent space data */
		if (!D3DTVertexGenerateTangentSpace(&nOutputVertexCount, (char **)&pOutputVertex, pTempIndexBuffer,
											Mesh[nCurr].nNumVertex, (char *)pTempVertexBuffer, (D3DVERTEXELEMENT9 *)&declTemp,
											sizeof(TEMPVERTEX), 
											0, 0, 0, 
											Mesh[nCurr].nNumFaces, 0.0f))
		{
			OutputDebugString("D3DTVertexGenerateTangentSpace() failed\n");
			MessageBox(NULL, "D3DTVertexGenerateTangentSpace() failed", "ERROR", MB_OK);
			free(pTempIndexBuffer);
			free(pTempVertexBuffer);
			D3DTPrint3DReleaseTextures();
			return FALSE;
		}

		/* Release temporary vertex buffer */
		free(pTempVertexBuffer);

		/* Update number of vertices */
		g_psMdl[nCurr].nNumVertex = nOutputVertexCount;


		/* Create vertex buffers */
		hres=lpDev9->CreateVertexBuffer(g_psMdl[nCurr].nNumVertex*sizeof(XYZVERTEX), 0, 0, D3DPOOL_MANAGED, &g_psMdl[nCurr].lpVertexBufferXYZ, NULL);
		hres|=lpDev9->CreateVertexBuffer(g_psMdl[nCurr].nNumVertex*sizeof(TSUVVERTEX), 0, 0, D3DPOOL_MANAGED, &g_psMdl[nCurr].lpVertexBufferTSUV, NULL);
		if (hres!=D3D_OK)
		{
			OutputDebugString("Failed to create vertex buffer\n");
			MessageBox(NULL, "Failed to create vertex buffer", "ERROR", MB_OK);
			free(pTempIndexBuffer);
			D3DTPrint3DReleaseTextures();
			return FALSE;
		}

		/* Lock vertex buffers and retrieve pointers to the locked data */
		hres=g_psMdl[nCurr].lpVertexBufferXYZ->Lock(0, 0, (void **)&pVertexXYZ, 0);
		hres|=g_psMdl[nCurr].lpVertexBufferTSUV->Lock(0, 0, (void **)&pVertexTSUV, 0);
		if (hres!=D3D_OK)
		{
			OutputDebugString("Lock() failed\n");
			MessageBox(NULL, "Lock() failed", "ERROR", MB_OK);
			RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
			RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
			free(pTempIndexBuffer);
			D3DTPrint3DReleaseTextures();
			return FALSE;
		}

		/* Copy vertex position and UVs */
		for (dwVert=0; dwVert<(DWORD)g_psMdl[nCurr].nNumVertex; dwVert++) 
		{
			/* Copy vertex position */
			pVertexXYZ->x = pOutputVertex[dwVert].x;
			pVertexXYZ->y = pOutputVertex[dwVert].y;
			pVertexXYZ->z = pOutputVertex[dwVert].z;

			/* Increase vertex pointer */
			pVertexXYZ++;

			/* Copy *transpose* tangent space matrix data into vertex buffer */
			pVertexTSUV->Bx = pOutputVertex[dwVert].tx;
			pVertexTSUV->By = pOutputVertex[dwVert].bx;
			pVertexTSUV->Bz = pOutputVertex[dwVert].nx;

			pVertexTSUV->Tx = pOutputVertex[dwVert].ty;
			pVertexTSUV->Ty = pOutputVertex[dwVert].by;
			pVertexTSUV->Tz = pOutputVertex[dwVert].ny;

			pVertexTSUV->Nx = pOutputVertex[dwVert].tz;
			pVertexTSUV->Ny = pOutputVertex[dwVert].bz;
			pVertexTSUV->Nz = pOutputVertex[dwVert].nz;

			/* Copy UVs */
			pVertexTSUV->tu = pOutputVertex[dwVert].tu;
			pVertexTSUV->tv = pOutputVertex[dwVert].tv;

			/* Increase vertex pointer */
			pVertexTSUV++;
		}

		/* Unlock vertex buffers */
		g_psMdl[nCurr].lpVertexBufferTSUV->Unlock();
		g_psMdl[nCurr].lpVertexBufferXYZ->Unlock();

		/* Data has been copied into vertex buffer, release output buffer */
		free(pOutputVertex);


		/* Create index buffer */
		hres = lpDev9->CreateIndexBuffer(g_psMdl[nCurr].nNumFaces * 3 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
                                         D3DPOOL_MANAGED, &g_psMdl[nCurr].lpIndexBuffer, NULL); 
		if (hres!=D3D_OK)
		{
			OutputDebugString("Failed to create index buffer\n");
			MessageBox(NULL, "Failed to create index buffer", "ERROR", MB_OK);
			RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
			RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
			free(pTempIndexBuffer);
			D3DTPrint3DReleaseTextures();
			return FALSE;
		}

		/* Lock index buffer and retrieve a pointer to the locked data */
		hres=g_psMdl[nCurr].lpIndexBuffer->Lock(0, 0, (void **)&pIndex, 0);
		if (hres!=D3D_OK)
		{
			OutputDebugString("Lock() failed\n");
			MessageBox(NULL, "Lock() failed", "ERROR", MB_OK);
			RELEASE(g_psMdl[nCurr].lpIndexBuffer);
			RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
			RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
			free(pTempIndexBuffer);
			D3DTPrint3DReleaseTextures();
			return FALSE;
		}

		/* Copy indices */
		for (i=0; i<(int)Mesh[nCurr].nNumFaces; i++)
		{
			*pIndex++ = pTempIndexBuffer[i*3+0];
			*pIndex++ = pTempIndexBuffer[i*3+1];
			*pIndex++ = pTempIndexBuffer[i*3+2];
		}

		/* Unlock index buffer */
		g_psMdl[nCurr].lpIndexBuffer->Unlock();

		/* Release temporary index buffer */
		free(pTempIndexBuffer);


		/* Create vertex buffers for shadow volumes */

		/* Skip unshadowed objects */
		if (!IsMeshShadowCaster(nCurr)) continue;
		
		/* Create vertex buffers for shadows */
		if (!ShadowMeshInit(&(g_psMdl[nCurr].sShadowMesh), g_psMdl[nCurr].psShadowVol, 
							IsMeshShadowedByHeadLight(nCurr) ? 9 : 3, lpDev9))
		{
			OutputDebugString("Failed to create shadow volume vertex buffers\n");
			MessageBox(NULL, "Failed to create shadow volume vertex buffers", "ERROR", MB_OK);
			RELEASE(g_psMdl[nCurr].lpIndexBuffer);
			RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
			RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
			D3DTPrint3DReleaseTextures();
			return FALSE;
		}
	}


	/**********************
	** Initialise lights **
	**********************/
	/* Light 0 is fullscreen light */
	LightInfo[0].InitialPosition = *(D3DVECTOR *)Light[0].fPosition;
	LightInfo[0].InitialTarget = *(D3DVECTOR *)Light[0].fTarget;
	LightInfo[0].eShape = FULLSCREEN;
	LightInfo[0].fMaxRange = 15000.0f;
	LightInfo[0].fIntensity = Light[0].fIntensity;

	/* Light 1 and 2 are sphere lights */
	for (i=1; i<3; i++)
	{
		LightInfo[i].InitialPosition = *(D3DVECTOR *)Light[i].fPosition;
		LightInfo[i].InitialTarget = *(D3DVECTOR *)Light[i].fTarget;
		LightInfo[i].eShape = SPHERE;
		LightInfo[i].fMaxRange = 2500.0f;
		LightInfo[i].fIntensity = Light[i].fIntensity;

		/* Create vertex buffers for sphere lights */
		CreateSphereVertexBuffer(lpDev9, LightInfo[i].fMaxRange, &LightInfo[i].lpVBLightShape, &LightInfo[i].nNumVertex);
	}

	/* Light 3 to 8 are cone lights */
	for (i=3; i<=8; i++)
	{
		LightInfo[i].InitialPosition = *(D3DVECTOR *)Light[i].fPosition;
		LightInfo[i].InitialTarget = *(D3DVECTOR *)Light[i].fTarget;
		LightInfo[i].eShape = CONE;
		LightInfo[i].fConeBottomRadius = 20.0f;
		LightInfo[i].fConeTopRadius = 600.0f;
		LightInfo[i].fConeHeight = 1500.0f;
		LightInfo[i].fMaxRange = 1500.0f;
		LightInfo[i].fIntensity = Light[i].fIntensity;

		/* Create vertex buffers for cone light */
		CreateConeVertexAndIndexBuffers(lpDev9, LightInfo[i].fConeBottomRadius, LightInfo[i].fConeTopRadius, LightInfo[i].fConeHeight, 
									&LightInfo[i].lpVBLightShape, &LightInfo[i].nNumVertex,
									&LightInfo[i].lpIBLightShape, &LightInfo[i].wNumIndices);
	}
	

	/* All world space light position and targets are their initial values by default */
	for (i=0; i<9; i++)
	{
		LightInfo[i].TPosition = LightInfo[i].InitialPosition;
		LightInfo[i].TTarget = LightInfo[i].InitialTarget;
		CurrentLightIntensity[i] = LightInfo[i].fIntensity;
	}


	/* Load default state from text file */
	LoadState();


	/******************
	** Load textures **
	******************/
	if (!LoadTextures(lpDev9))
	{
		OutputDebugString("Failed to load textures\n");
		MessageBox(NULL, "Failed to load textures", "ERROR", MB_OK);
		D3DTPrint3DReleaseTextures();
		return FALSE;
	}


	
	/**************************
	** Create vertex shaders **
	**************************/
	CreateVertexShaders(lpDev9);


	/*************************
	** Create pixel shaders **
	*************************/
	CreatePixelShaders(lpDev9);

	
	/*************
	** Matrices **
	*************/
	
	/* Set PROJECTION matrix */
	D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)&ProjectionMatrix, Camera[nCurrentCamera].fFOV, (float)dwCurrentWidth / (float)dwCurrentHeight, 
							   Camera[nCurrentCamera].fNearClip, Camera[nCurrentCamera].fFarClip);	
	lpDev9->SetTransform(D3DTS_PROJECTION, &ProjectionMatrix);

	/* Set PROJECTION matrix for light shapes */
	D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)&ProjectionMatrixForLightShape, Camera[nCurrentCamera].fFOV, (float)dwCurrentWidth / (float)dwCurrentHeight, 
							   Camera[nCurrentCamera].fNearClip, Camera[nCurrentCamera].fFarClip);	

	/* Set TEXTURE PROJECTION matrix */
	D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)&TextureProjectionMatrix, PI/4.0f, 1.0f, 10.0f, 100000.0f);


	/* Set some variables for Free Camera */
	D3DXVECTOR3 v;
	v.x = Camera[nCurrentCamera].fTarget[0] - Camera[nCurrentCamera].fPosition[0];
	v.y = Camera[nCurrentCamera].fTarget[1] - Camera[nCurrentCamera].fPosition[1];
	v.z = Camera[nCurrentCamera].fTarget[2] - Camera[nCurrentCamera].fPosition[2];
	fFreeCamX = -(float)atan2(v.y, sqrt(v.x*v.x + v.z*v.z));
	fFreeCamY = (float)atan2(v.x, v.z);

	
	/*****************************
	** Set default renderstates **
	*****************************/

	/* Z Compare mode */
	lpDev9->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	/* Backface culling */
	lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	/* Enables Depth Testing */
	lpDev9->SetRenderState(D3DRS_ZENABLE, TRUE);
	
	/* Disable lighting completely */
	lpDev9->SetRenderState(D3DRS_LIGHTING, FALSE);

	/* Max anisotry level */
	for (i=0; i<8; i++)
	{
		lpDev9->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 8);
	}


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
	int nCurr;

	
	/**************************
	** Release pixel shaders **
	**************************/
	DestroyPixelShaders();


	/***************************
	** Release vertex shaders **
	***************************/
	DestroyVertexShaders();


	/********************************
	** Release other geometry data **
	********************************/
	for (nCurr=0; nCurr<TOTAL_NUM_LIGHTS; nCurr++)
	{
		/* Destroy VBs for light shapes */
		if (LightInfo[nCurr].eShape==CONE || LightInfo[nCurr].eShape==SPHERE)
		{
			RELEASE(LightInfo[nCurr].lpVBLightShape);
		}

		/* Destroy IBs for light shapes */
		if (LightInfo[nCurr].eShape==CONE)
		{
			RELEASE(LightInfo[nCurr].lpIBLightShape);
		}
	}



	/************************
	** Release meshes data **
	************************/
	/* Destroy mesh data for each mesh (reverse order) */
	for (nCurr = 0; nCurr <NUM_MESHES; nCurr++) 
	{
		RELEASE(g_psMdl[nCurr].lpVertexBufferXYZ);
		RELEASE(g_psMdl[nCurr].lpVertexBufferTSUV);
		RELEASE(g_psMdl[nCurr].lpIndexBuffer);

		/* Shadow vertex buffer data */
		if (IsMeshShadowCaster(nCurr))
		{
			ShadowMeshRelease(g_psMdl[nCurr].psShadowVol, IsMeshShadowedByHeadLight(nCurr) ? 9 : 3);
		}
	}


	/*********************************************
	** Release MRT render surfaces and textures **
	*********************************************/
	RELEASE(lpMRT3);
	RELEASE(lpMRT2);
	RELEASE(lpMRT1);
	RELEASE(lpMRT0);

	RELEASE(pMRTDiffuse);
	RELEASE(pMRT8888Normal);
	RELEASE(pMRT1616Z);
	RELEASE(pMRT1616XY);


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
 * Inputs		  : lpDev9, *psMdl, nLight
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Render model
 *******************************************************************************/
static void RenderModel(LPDIRECT3DDEVICE9 lpDev9, SModel *psMdl, int nLight)
{
	HRESULT		hres;
	D3DMATRIX	GlobalTransformationMatrix, WorldMatrix;

	/* Concatenate world, view and projection matrix */
	D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&psMdl->mWorld, (D3DXMATRIX *)&ViewMatrix);
	D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&ProjectionMatrix);
	D3DXMatrixTranspose((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix);

	/* Compute transpose of world matrix */
	D3DXMatrixTranspose((D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&psMdl->mWorld);

	/* Set VS constants */
	lpDev9->SetVertexShaderConstantF(0, (float *)&GlobalTransformationMatrix,  4);
	lpDev9->SetVertexShaderConstantF(4, (float *)&WorldMatrix,  4);

	/* Set PS constants */
	lpDev9->SetPixelShaderConstantF(0, (float *)&WorldMatrix,  4);

	/* Set indices */
	lpDev9->SetIndices(psMdl->lpIndexBuffer);
	
	/* Draw model */
	hres=lpDev9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, psMdl->nNumVertex, 0, psMdl->nNumFaces);
	if (hres!=D3D_OK)
	{
		OutputDebugString("ERROR: DrawIndexedPrimitive() failed in RenderModel()\n");
	}
}


/*****************************************************************************
 * Function Name  : SaveState
 * Inputs		  : None
 * Globals used   : Yes
 * Returns		  : Nothing
 * Description    : Saves current viewing state.
 *****************************************************************************/
static void SaveState()
{
	FILE	*f;
	int		i=0;
	char	pszScreenshotName[] = "data.txt";
	char	pszString[128];
	int		nint;
	
	if ( !(f=fopen(pszScreenshotName, "wb")) )
	{
		sprintf(pszString, "D3DShell *WARNING* : Unable to write data.txt.\n", pszScreenshotName);
		OutputDebugString(pszString);
	}

	for(i=0; i<TOTAL_NUM_LIGHTS; i++)
	{
		fprintf(f, "Lightpos %d:%f %f %f\nLighttarget %d:%f %f %f\n", 
				i, LightInfo[i].TPosition.x, LightInfo[i].TPosition.y, LightInfo[i].TPosition.z,
				i, LightInfo[i].TTarget.x, LightInfo[i].TTarget.y, LightInfo[i].TTarget.z);
	}

	

	for(i=0; i<NUM_CAMERAS; i++)
	{
		fprintf(f, "Camerapos %d:%f %f %f\nCameratarget %d:%f %f %f\n",
			i, Camera[i].fPosition[0], Camera[i].fPosition[1], Camera[i].fPosition[2],
			i, Camera[i].fTarget[0], Camera[i].fTarget[1], Camera[i].fTarget[2]);
	}

	for(i=0;i<TOTAL_NUM_LIGHTS; i++)
	{
		fprintf(f, "DrawLight %d:%d\n", i, bDrawLight[i]);
	}

	fprintf(f, "DrawShadow %d\n", bDrawShadow);
	fprintf(f, "Text On %d\n", nTextOn);
	fprintf(f, "Use compressed textures %d\n", bUseCompressedTextures);
	fprintf(f, "Use 16 bits textures %d\n", bUse16BitsTextures);
	if (nTextureFilter==D3DTEXF_POINT && nMipFilter==D3DTEXF_NONE)		nint=0;
	if (nTextureFilter==D3DTEXF_LINEAR && nMipFilter==D3DTEXF_NONE)		nint=1;
	if (nTextureFilter==D3DTEXF_LINEAR && nMipFilter==D3DTEXF_LINEAR)	nint=2;
	fprintf(f, "Filtering mode %d\n", nint);
	fprintf(f, "Default Camera %d\n", nCurrentCamera);

	/* Close file */
	if (fclose(f)!=0)
	{
		sprintf(pszString, ": Error in closing data.txt.\n", pszScreenshotName);
		OutputDebugString(pszString);
	}
}


/*****************************************************************************
 * Function Name  : LoadState
 * Inputs		  : None
 * Globals used   : Yes
 * Returns		  : Nothing
 * Description    : Loads current viewing state.
 *****************************************************************************/
static void LoadState()
{
	FILE					*f;
	char					pszString[128];
	char					pszScreenshotName[] = "data.txt";
	int						i=0,j,nint;

	if ( !(f=fopen(pszScreenshotName, "rb")) )
	{
		sprintf(pszString, "D3DShell *WARNING* : Unable to open data.txt.\n", pszScreenshotName);
		OutputDebugString(pszString);
	}

	for(i=0; i<TOTAL_NUM_LIGHTS; i++)
	{
		fscanf(f, "Lightpos %d:%f %f %f\nLighttarget %d:%f %f %f\n", 
				&j, &LightInfo[i].TPosition.x, &LightInfo[i].TPosition.y, &LightInfo[i].TPosition.z,
				&j, &LightInfo[i].TTarget.x, &LightInfo[i].TTarget.y, &LightInfo[i].TTarget.z);
	}

	for(i=0; i<NUM_CAMERAS; i++)
	{
		fscanf(f, "Camerapos %d:%f %f %f\nCameratarget %d:%f %f %f\n",
			&j, &Camera[i].fPosition[0], &Camera[i].fPosition[1], &Camera[i].fPosition[2],
			&j, &Camera[i].fTarget[0], &Camera[i].fTarget[1], &Camera[i].fTarget[2]);
	}

	for(i=0;i<TOTAL_NUM_LIGHTS; i++)
	{
		fscanf(f, "DrawLight %d:%d\n", &j,&bDrawLight[i]);
	}

	fscanf(f, "DrawShadow %d\n", &bDrawShadow);
	fscanf(f, "Text On %d\n", &nTextOn);
	fscanf(f, "Use compressed textures %d\n", &bUseCompressedTextures);
	fscanf(f, "Use 16 bits textures %d\n", &bUse16BitsTextures);
	fscanf(f, "Filtering mode %d\n", &nint);
	switch (nint)
	{
		case 0 : nTextureFilter=D3DTEXF_POINT;	nMipFilter=D3DTEXF_NONE;	break;
		case 1 : nTextureFilter=D3DTEXF_LINEAR; nMipFilter=D3DTEXF_NONE;	break;
		case 2 : nTextureFilter=D3DTEXF_LINEAR; nMipFilter=D3DTEXF_LINEAR;	break;
	}
	fscanf(f, "Default Camera %d\n", &nCurrentCamera);

	/* Close file */
	if (fclose(f)!=0)
	{
		sprintf(pszString, "Error in closing data.txt.\n", pszScreenshotName);
		OutputDebugString(pszString);
	}
}


/*****************************************************************************
 * Function Name  : LoadTextures
 * Inputs		  : 
 * Outputs		  : 
 * Globals used   : Yes
 * Returns		  : Nothing
 * Description    : Load textures
 *****************************************************************************/
static BOOL LoadTextures(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT		hres;
	D3DFORMAT	OpaqueTextureFormat, TranslucentTextureFormat;
	BOOL		bRet;
	int			i;
	char		pszFileName[128];
	char		*pszPointer;


	/* What texture format are we using? */
	if (bUseCompressedTextures)	
	{
		OpaqueTextureFormat =		D3DFMT_DXT1;
		TranslucentTextureFormat =	D3DFMT_DXT3;
	}
	else if (bUse16BitsTextures) 
	{
		OpaqueTextureFormat =		D3DFMT_R5G6B5;
		TranslucentTextureFormat =	D3DFMT_A4R4G4B4;
	}
	else 
	{
		OpaqueTextureFormat =		D3DFMT_A8R8G8B8;
		TranslucentTextureFormat =	D3DFMT_A8R8G8B8;
	}

	/* Create falloff textures */
	bRet = CreateFallOffTexture(lpDev9, 512, 0.8f, &lpFallOffSharp);
	if (!bRet)
	{
		OutputDebugString("Failed to create falloff texture\n");
		return FALSE;
	}


	/* Create power lookup texture */
	bRet = CreatePowerLookupTexture(lpDev9, 512, 60.0f, &lpPowerLookUpTexture);
	if (!bRet)
	{
		OutputDebugString("Failed to create power lookup texture\n");
		return FALSE;
	}


	/* Load light texture (used to display light sources in debug mode) */
	hres = D3DXCreateTextureFromFileEx(lpDev9, "LIGHT.BMP", 
								   D3DX_DEFAULT, D3DX_DEFAULT,
								   D3DX_DEFAULT, 
								   0, 
								   OpaqueTextureFormat, 
								   D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
								   D3DX_FILTER_BOX, 0, NULL, NULL, &lpLightTexture);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to create light texture\n");
		return FALSE;
	}


	/* Load lightmap texture for main light */
	hres = D3DXCreateTextureFromFileEx(lpDev9, "PROJECTIONMAP.BMP", 
									   D3DX_DEFAULT, D3DX_DEFAULT,
									   1,		// No mipmapping for lightmap
									   0, 
									   OpaqueTextureFormat, 
									   D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
									   D3DX_FILTER_BOX, 0, NULL, NULL, &lpLightMap);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to create lightmap texture\n");
		return FALSE;
	}


	/* Load normalization cube map */
	if (!CreateNormalCubeMap(lpDev9, 256, &lpNormalizationCubeMap))
	{
		OutputDebugString("Failed to create cube normalization map texture().\n");
		return FALSE;
	}


	/* Load PowerVR logo texture */
	D3DXCreateTextureFromFileEx(lpDev9, "LogoPVR.bmp", 
								D3DX_DEFAULT, D3DX_DEFAULT,
								1,
								0, 
								OpaqueTextureFormat, 
								D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
								D3DX_FILTER_BOX, 0, NULL, NULL, &lpPowerVRLogoTexture);

	/* Load mesh textures */
	for (i=0; i<NUM_MATERIALS; i++)
	{
		/* Replace diffuse map .bmp extension by .dds */
		strcpy(pszFileName, Material[i].sDifusseFile);
		pszPointer = strstr(pszFileName, ".bmp");
		if (pszPointer)
		{
			pszPointer[0]='.';
			pszPointer[1]='d';
			pszPointer[2]='d';
			pszPointer[3]='s';
		}

		/* Load diffuse maps (alpha contains power level) */
		hres = D3DXCreateTextureFromFileEx(lpDev9, pszFileName, 
								   D3DX_DEFAULT, D3DX_DEFAULT,
								   D3DX_DEFAULT, 
								   0, 
								   TranslucentTextureFormat, 
								   D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
								   D3DX_FILTER_BOX, 0, NULL, NULL, &lpDiffuseMap[i]);
		if (hres!=D3D_OK)
		{
			sprintf(pszTmp, "Failed to load %s texture\n", pszFileName);
			OutputDebugString(pszTmp);
			return FALSE;
		}

		/* Load normal maps */
		if (Material[i].sBumpFile) sprintf(pszFileName, Material[i].sBumpFile); 
			else sprintf(pszFileName, "GREY.BMP"); 

		/* Convert height map ot normal map */
		bRet = CreateNormalMapFromHeightMap(lpDev9, pszFileName, 2.0f, TRUE, &lpNormalMap[i]);
		if (hres!=D3D_OK)
		{
			sprintf(pszTmp, "Failed to load %s texture\n", pszFileName);
			OutputDebugString(pszTmp);
			return FALSE;
		}
	}


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
static void DestroyTextures()
{
	int i;

	/* Release diffuse, specular and normal textures */
	for (i=0; i<NUM_MATERIALS; i++)
	{
		RELEASE(lpDiffuseMap[i]);
		RELEASE(lpNormalMap[i]);
	}

	/* Release PowerVR logo texture */
	RELEASE(lpPowerVRLogoTexture);

	/* Release cube map texture */
	RELEASE(lpNormalizationCubeMap);

	/* Release lightmap texture */
	RELEASE(lpLightMap);

	/* Release light texture */
	RELEASE(lpLightTexture);

	/* Release power lookup texture */
	RELEASE(lpPowerLookUpTexture);

	/* Release falloff textures */
	RELEASE(lpFallOffSharp);
}


/*******************************************************************************
 * Function Name  : CreateFallOffTexture
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Create a 1D falloff texture of width x.
 *******************************************************************************/
static BOOL CreateFallOffTexture(LPDIRECT3DDEVICE9 lpDev9, int x, float fOrder, LPDIRECT3DTEXTURE9 *lplpTextureName)
{
#define TEXTURE_HEIGHT 4	// Use a minimum texture height of 4 to be on the safe side
	HRESULT				hres;
	LPDIRECT3DSURFACE9	lpD3DSurface;
	BYTE				*pImageData;
	RECT				SrcRect;
	int					i, j;

	/* Create empty texture */
	hres = D3DXCreateTexture(lpDev9, x, TEXTURE_HEIGHT, 1, 0, D3DFMT_L8, D3DPOOL_MANAGED, lplpTextureName);
	if (hres!=D3D_OK)
	{
		OutputDebugString("D3DXCreateTexture() failed in CreateFallOffTexture().\n");
		*lplpTextureName = NULL;
		return FALSE;
	}

	/* Get surface */
	hres = (*lplpTextureName)->GetSurfaceLevel(0, &lpD3DSurface);
	if (hres!=D3D_OK)
	{
		OutputDebugString("GetSurfaceLevel() failed in CreateFallOffTexture().\n");
		RELEASE(*lplpTextureName);
		*lplpTextureName = NULL;
		return FALSE;
	}

	/* Prepare image data */
	pImageData = (BYTE *)malloc(x*TEXTURE_HEIGHT*sizeof(BYTE));

	/* Write falloff data */
	for (j=0; j<TEXTURE_HEIGHT; j++)
	{
		for (i=0; i<x; i++)
		{
			pImageData[j*x + i] = (BYTE)(255.0f * pow( (x-i-1.0f)/(x-1.0f) , fOrder) );
		}
	}

	/* Upload data into surface */
	SetRect(&SrcRect, 0, 0, x, TEXTURE_HEIGHT );
	hres = D3DXLoadSurfaceFromMemory(lpD3DSurface, NULL, NULL, pImageData, 
									 D3DFMT_L8, x*sizeof(BYTE), NULL, &SrcRect,
									 D3DX_FILTER_NONE, 0);
	if (hres!=D3D_OK)
	{
		OutputDebugString("D3DXLoadSurfaceFromMemory() failed in CreateFallOffTexture().\n");
		free(pImageData);
		RELEASE(lpD3DSurface);
		RELEASE(*lplpTextureName);
		*lplpTextureName = NULL;
		return FALSE;
	}

	/* Free image data */
	free(pImageData);

	/* Release surface */
	RELEASE(lpD3DSurface);
	
	/* No problem occured */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : CreatePowerLookupTexture
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Create a 1D falloff texture of width x.
 *******************************************************************************/
static BOOL CreatePowerLookupTexture(LPDIRECT3DDEVICE9 lpDev9, int nSize, float fMaxPower, LPDIRECT3DTEXTURE9 *lplpTextureName)
{
	HRESULT				hres;
	LPDIRECT3DSURFACE9	lpD3DSurface;
	BYTE				*pImageData;
	int					i, j, nMIPLevel;

	/* Create empty texture */
	hres = D3DXCreateTexture(lpDev9, nSize, nSize, D3DX_DEFAULT, 0, D3DFMT_L8, D3DPOOL_MANAGED, lplpTextureName);
	if (hres!=D3D_OK)
	{
		OutputDebugString("D3DXCreateTexture() failed in CreateFallOffTexture().\n");
		*lplpTextureName = NULL;
		return FALSE;
	}

	/* Allocate memory for image data */
	pImageData = (BYTE *)malloc(nSize*nSize*sizeof(BYTE));

	for (nMIPLevel=0; nSize>0; nSize/=2, nMIPLevel++)
	{
		/* Get surface */
		hres = (*lplpTextureName)->GetSurfaceLevel(nMIPLevel, &lpD3DSurface);
		if (hres!=D3D_OK)
		{
			OutputDebugString("GetSurfaceLevel() failed in CreateFallOffTexture().\n");
			RELEASE(*lplpTextureName);
			*lplpTextureName = NULL;
			return FALSE;
		}

		/* Write falloff data */
		for (j=0; j<nSize; j++)
		{
			for (i=0; i<nSize; i++)
			{
				pImageData[j*nSize + i] = (BYTE)(255.0f*((float)pow((float)i/(float)(nSize-1), (float)(j*fMaxPower)/(float)(nSize-1))));
			}
		}

		/* Upload data into surface */
		RECT SrcRect = { 0, 0, nSize, nSize };
		hres = D3DXLoadSurfaceFromMemory(lpD3DSurface, NULL, NULL, pImageData, 
										 D3DFMT_L8, nSize*sizeof(BYTE), NULL, &SrcRect,
										 D3DX_DEFAULT, 0);
		if (hres!=D3D_OK)
		{
			OutputDebugString("D3DXLoadSurfaceFromMemory() failed in CreateFallOffTexture().\n");
			free(pImageData);
			RELEASE(lpD3DSurface);
			RELEASE(*lplpTextureName);
			*lplpTextureName = NULL;
			return FALSE;
		}

		/* Release surface */
		RELEASE(lpD3DSurface);
	}

	/* Free image data */
	free(pImageData);
	
	/* No problem occured */
	return TRUE;
}


/*****************************************************************************
 * Function Name  : GetCubeVector
 * Inputs		  : i, cubesize, x, y
 * Outputs		  : *vector
 * Returns		  : Nothing
 * Description    : Converts an input U,V texture coordinate and face number to
 *					a resulting cube map vector.
 *****************************************************************************/
static void GetCubeVector(int i, int cubesize, int x, int y, float *vector)
{
  float s, t, sc, tc, mag;

  s = ((float)x + 0.5f) / (float)cubesize;
  t = ((float)y + 0.5f) / (float)cubesize;
  sc = s*2.0f - 1.0f;
  tc = t*2.0f - 1.0f;

  switch (i) 
  {
	  case 0:	vector[0] = 1.0f;
				vector[1] = -tc;
				vector[2] = -sc;
				break;
	  case 1:	vector[0] = -1.0;
				vector[1] = -tc;
				vector[2] = sc;
				break;
	  case 2:	vector[0] = sc;
				vector[1] = 1.0;
				vector[2] = tc;
				break;
	  case 3:	vector[0] = sc;
				vector[1] = -1.0;
				vector[2] = -tc;
				break;
	  case 4:	vector[0] = sc;
				vector[1] = -tc;
				vector[2] = 1.0;
				break;
	  case 5:	vector[0] = -sc;
				vector[1] = -tc;
				vector[2] = -1.0;
				break;
  }

  /* Normalize vector */
  mag = 1.0f/sqrtf(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
  vector[0] *= mag;
  vector[1] *= mag;
  vector[2] *= mag;
}


/*****************************************************************************
 * Function Name  : CreateNormalCubeMap
 * Inputs		  : lpDev9, nSize
 * Outputs		  : *lplpTextureName
 * Returns		  : Nothing
 * Description    : Create a normalization cube map
 *****************************************************************************/
static BOOL CreateNormalCubeMap(LPDIRECT3DDEVICE9 lpDev9, int nSize, LPDIRECT3DCUBETEXTURE9 *lplpTextureName)
{
	HRESULT				hres;
	LPDIRECT3DSURFACE9	lpD3DSurface;
	float				*fFloatPixels;
	RECT				SrcRect;
	float				vector[3];
	int					i, x, y, nMIPLevel;


	/* Create empty cube texture */
	hres = D3DXCreateCubeTexture(lpDev9, nSize, D3DX_DEFAULT, 0, bSupportA2R10G10B10 ? D3DFMT_A2R10G10B10 : D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, lplpTextureName);
	if (hres!=D3D_OK)
	{
		OutputDebugString("D3DXCreateCubeTexture() failed in CreateNormalCubeMap().\n");
		*lplpTextureName = NULL;
		return FALSE;
	}

	/* Allocate memory for image data */
	fFloatPixels = (float *)malloc(nSize*nSize*4*sizeof(float));

	/* Main loop */
	for (nMIPLevel=0; nSize>0; nSize/=2, nMIPLevel++)
	{
		/* Main loop */
		for (i=0; i<6; i++) 
		{
			for (y=0; y<nSize; y++) 
			{
				for (x=0; x<nSize; x++) 
				{
					/* Get vector for pixel (x, y) in face i */
					GetCubeVector(i, nSize, x, y, vector);

					/* Write resulting vector in cube map */
					fFloatPixels[4*(y*nSize+x) + 0] = 0.5f + 0.5f*vector[0];
					fFloatPixels[4*(y*nSize+x) + 1] = 0.5f + 0.5f*vector[1];
					fFloatPixels[4*(y*nSize+x) + 2] = 0.5f + 0.5f*vector[2];
				}
			}

			/* Get current face */
			hres = (*lplpTextureName)->GetCubeMapSurface((D3DCUBEMAP_FACES)(D3DCUBEMAP_FACE_POSITIVE_X + i), nMIPLevel, &lpD3DSurface);
			if (hres!=D3D_OK)
			{
				OutputDebugString("GetCubeMapSurface() failed in CreateNormalCubeMap().\n");
				RELEASE(*lplpTextureName);
				free(fFloatPixels); 
				return FALSE;
			}

			/* Fill texture with data */
			SetRect(&SrcRect, 0, 0, nSize, nSize);
			hres = D3DXLoadSurfaceFromMemory(lpD3DSurface, NULL, NULL, fFloatPixels, 
											 D3DFMT_A32B32G32R32F, nSize*4*sizeof(float), NULL, &SrcRect,
											 D3DX_FILTER_NONE, 0);
			if (hres!=D3D_OK)
			{	
				OutputDebugString("D3DXLoadSurfaceFromMemory() failed in CreateNormalCubeMap().\n");
				RELEASE(lpD3DSurface);
				RELEASE(*lplpTextureName);
				free(fFloatPixels); 
				return FALSE;
			}

			/* Release surface */
			RELEASE(lpD3DSurface);
		}
	}

	/* Release memory */
	free(fFloatPixels); 

	/* No problem occured */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : CreateNormalMapFromHeightMap
 * Input		  : lpDev9, *pszFileName, fHeightScale, bMipmap
 * Output		  : *lplpTextureName
 * Global Used    : None
 * Description    : Create a normal map texture from a height map stored in a 
 *					.BMP file. The height map must be a 24-bits grayscale BMP.
 *					The source BMP file can either be stored on file or resource.
 *					
 *******************************************************************************/
static BOOL CreateNormalMapFromHeightMap(LPDIRECT3DDEVICE9 lpDev9, char *pszFileName, 
										 float fHeightScale, BOOL bMipmap, LPDIRECT3DTEXTURE9 *lplpTextureName)
{
	LPDIRECT3DSURFACE9	lpD3DSurface;
	LPDIRECT3DTEXTURE9	lpHeightMap;
	D3DSURFACE_DESC		Desc;
	HRESULT				hres;
	
	/* Set default height scale */
	if (fHeightScale==0.0f) fHeightScale=1.0f;

	/* Load height map */
	hres = D3DXCreateTextureFromFileEx(lpDev9, pszFileName, 
								   D3DX_DEFAULT, D3DX_DEFAULT,
								   bMipmap ? D3DX_DEFAULT : 1, 
								   0, 
								   D3DFMT_L8, 
								   D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE, 
								   D3DX_FILTER_BOX, 0, NULL, NULL, &lpHeightMap);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to load height map\n");
		return FALSE;
	}

	/* Retrieve information about texture */
	lpHeightMap->GetSurfaceLevel(0, &lpD3DSurface);
	lpD3DSurface->GetDesc(&Desc);
	RELEASE(lpD3DSurface);


	/* Create destination texture */
	hres = D3DXCreateTexture(lpDev9, Desc.Width, Desc.Height, bMipmap ? D3DX_DEFAULT : 1, 0, D3DFMT_A2R10G10B10, 
							 D3DPOOL_MANAGED, lplpTextureName);
	if (hres!=D3D_OK)
	{
		OutputDebugString("D3DXCreateTexture() failed in CreateNormalMapFromHeightMap().\n");
		RELEASE(lpHeightMap);
		return FALSE;
	}
	

	/* Convert texture into normal map */
	hres = D3DXComputeNormalMap(*lplpTextureName, lpHeightMap, NULL, 0, D3DX_CHANNEL_LUMINANCE, fHeightScale);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to compute normal map\n");
		RELEASE(*lplpTextureName);
		RELEASE(lpHeightMap);
		return FALSE;
	}


	/* Release height map */
	RELEASE(lpHeightMap);

	/* No problem occured */
	return TRUE;
}


/*****************************************************************************
 * Function Name  : GetUserInput
 * Inputs		  : 
 * Outputs		  : 
 * Globals used   : Yes
 * Returns		  : Nothing
 * Description    : Get user input 
 *****************************************************************************/
static void GetUserInput()
{
	float fSpeed;
	D3DXMATRIX mTrans, mTmp;
	D3DXVECTOR3 v, s;
	int nMouseX, nMouseY, nMouseButtons;
	D3DXVECTOR3	CameraDelta(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3	LightDelta(0.0f, 0.0f, 0.0f);
	

	/*******************
	** Camera control **
	*******************/
	// Check mouse input
	D3DTMouseQuery(&nMouseX, &nMouseY, &nMouseButtons);
	if (nMouseButtons & 0x02) 
	{
		fFreeCamX += 0.01f * nMouseY;
		fFreeCamY += 0.01f * nMouseX;
	}

	/* Determine speed to use */
	fSpeed = (GetKeyState(VK_SHIFT)>>8 || nMouseButtons & 0x04) ? fCameraSpeed * 4 : fCameraSpeed;

	/* Update camera position */
	D3DXMatrixRotationX(&mTrans, fFreeCamX);
	D3DXMatrixRotationY(&mTmp, fFreeCamY);
	D3DXMatrixMultiply(&mTrans, &mTrans, &mTmp);

	/* Transform (0,0,1) */
	v.x = mTrans._31;	
	v.y = mTrans._32;
	v.z = mTrans._33;

	/* Do a cross-product */
	D3DXVec3Cross(&s, &g_UpVector, &v);

	/* Keyboard control */
	if (GetKeyState('W')>>8)
	{
	    CameraDelta.x = v.x * fSpeed;
	    CameraDelta.y = v.y * fSpeed;
	    CameraDelta.z = v.z * fSpeed;
	}

	if (GetKeyState('S')>>8)
	{
	    CameraDelta.x = -v.x * fSpeed;
	    CameraDelta.y = -v.y * fSpeed;
	    CameraDelta.z = -v.z * fSpeed;
	}

	if (GetKeyState('A')>>8)
	{
	    CameraDelta.x = -s.x * fSpeed;
	    CameraDelta.y = -s.y * fSpeed;
	    CameraDelta.z = -s.z * fSpeed;
	}

	if (GetKeyState('D')>>8)
	{
	    CameraDelta.x = s.x * fSpeed;
	    CameraDelta.y = s.y * fSpeed;
	    CameraDelta.z = s.z * fSpeed;
	}

	if (GetKeyState('R')>>8)
	{
	    CameraDelta.x = g_UpVector.x * fSpeed;
	    CameraDelta.y = g_UpVector.y * fSpeed;
	    CameraDelta.z = g_UpVector.z * fSpeed;
	}

	if (GetKeyState('F')>>8)
	{
	    CameraDelta.x = -g_UpVector.x * fSpeed;
	    CameraDelta.y = -g_UpVector.y * fSpeed;
	    CameraDelta.z = -g_UpVector.z * fSpeed;
	}

	if (GetKeyState(VK_UP)>>8)
	{
		LightDelta.z = fSpeed;
	}

	if (GetKeyState(VK_DOWN)>>8)
	{
		LightDelta.z = -fSpeed;
	}

	if (GetKeyState(VK_RIGHT)>>8)
	{
		LightDelta.x = fSpeed;
	}

	if (GetKeyState(VK_LEFT)>>8)
	{
		LightDelta.x = -fSpeed;
	}

	if (GetKeyState(VK_PRIOR)>>8)
	{
		LightDelta.y = fSpeed;
	}

	if (GetKeyState(VK_NEXT)>>8)
	{
		LightDelta.y = -fSpeed;
	}

	/* Update light position or target */
	if (bModifyTarget)
	{
		LightInfo[nSelectedLight].TTarget.x += LightDelta.x;
		LightInfo[nSelectedLight].TTarget.y += LightDelta.y;
		LightInfo[nSelectedLight].TTarget.z += LightDelta.z;
	}
	else
	{
		LightInfo[nSelectedLight].TPosition.x += LightDelta.x;
		LightInfo[nSelectedLight].TPosition.y += LightDelta.y;
		LightInfo[nSelectedLight].TPosition.z += LightDelta.z;
	}
	

	/* Update camera position and targets */
	Camera[nCurrentCamera].fPosition[0] += CameraDelta.x;
	Camera[nCurrentCamera].fPosition[1] += CameraDelta.y;
	Camera[nCurrentCamera].fPosition[2] += CameraDelta.z;
	Camera[nCurrentCamera].fTarget[0] = Camera[nCurrentCamera].fPosition[0] + v.x;
	Camera[nCurrentCamera].fTarget[1] = Camera[nCurrentCamera].fPosition[1] + v.y;
	Camera[nCurrentCamera].fTarget[2] = Camera[nCurrentCamera].fPosition[2] + v.z;
}



/*****************************************************************************
 * Function Name  : DoAnimation
 * Inputs		  : 
 * Outputs		  : 
 * Globals used   : Yes
 * Returns		  : Nothing
 * Description    : 
 *****************************************************************************/
static void DoAnimation()
{
	
	D3DMATRIX		WorldMatrix, TranslationMatrix;
	D3DVECTOR		vModel, vWorld;
	SModel			*psMdl;
	DWORD			dwTimeDelta, dwTime;
	float			fNormalise;
	int				nCurr, nLight, i;
	float			R1=1000.0f, R2=2000.0f;
	static float	fAlpha = 0.0f;
	static DWORD	dwPreviousTime=0, dwTimeElapsed=0;


	/* Loop through all meshes */
	nCurr = NUM_MESHES;
	while (nCurr) 
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

		/* Skip shadow mesh step for non-shadow casting objects */
		if (!IsMeshShadowCaster(nCurr)) continue;

		/* Only go further if shadows are enabled */
		if (!bDrawShadow) continue;

		/* Transform shadow meshes into world space */
		for (i=0; i<(int)psMdl->sShadowMesh.nV; i++)
		{
			psMdl->sShadowMesh.pVw[i].x = WorldMatrix._11*psMdl->sShadowMesh.pVo[i].x +
										  WorldMatrix._21*psMdl->sShadowMesh.pVo[i].y +
										  WorldMatrix._31*psMdl->sShadowMesh.pVo[i].z +
										  WorldMatrix._41;
			psMdl->sShadowMesh.pVw[i].y = WorldMatrix._12*psMdl->sShadowMesh.pVo[i].x +
										  WorldMatrix._22*psMdl->sShadowMesh.pVo[i].y +
										  WorldMatrix._32*psMdl->sShadowMesh.pVo[i].z +
										  WorldMatrix._42;
			psMdl->sShadowMesh.pVw[i].z = WorldMatrix._13*psMdl->sShadowMesh.pVo[i].x +
										  WorldMatrix._23*psMdl->sShadowMesh.pVo[i].y +
										  WorldMatrix._33*psMdl->sShadowMesh.pVo[i].z +
										  WorldMatrix._43;
		}
	}

	/* Time */
	dwTime = timeGetTime();
	if (dwPreviousTime==0) dwPreviousTime=dwTime;
	if (bPause)
	{
		if (bStep) 
		{
 			dwTimeDelta = 50;
			bStep=FALSE;
		}
		else dwTimeDelta = 0;
	}
	else
	{
		dwTimeDelta = CLAMP(dwTime - dwPreviousTime, 1, 500);
	}
	dwPreviousTime = dwTime;


	/* Animate sphere lights */
	LightInfo[1].TPosition.x = 0.707f * R1 * (float)cos(fAlpha);
	LightInfo[1].TPosition.y = (float)fabs(R1 * (float)sin(fAlpha)) + 620.0f;
	LightInfo[1].TPosition.z = 0.707f * R1 * (float)cos(fAlpha);

	LightInfo[2].TPosition.x = R2 * (float)cos(fAlpha+PI);
	LightInfo[2].TPosition.y = 800.0f;
	LightInfo[2].TPosition.z = R2 * (float)sin(fAlpha+PI);

	fAlpha += (float)dwTimeDelta/20000.0f;

	
	/* Cycle through cone lights if enabled */
	if (bCyclingLights)
	{
		/* Compute time elapsed since the last time a light was swapped */
		dwTimeElapsed += dwTimeDelta;

		/* Swap light every 3 sec */
		if ( dwTimeElapsed > 3000 ) 
		{
			nCurrentActiveLight++;
			if (nCurrentActiveLight>8) nCurrentActiveLight=3;
			for (i=3; i<9; i++) 
			{
				bDrawLight[i]=FALSE;
				CurrentLightIntensity[i] = LightInfo[i].fIntensity;
			}
			bDrawLight[nCurrentActiveLight]=TRUE;
			bMenuHasChanged=TRUE;
			dwTimeElapsed=0;
		}

		/* Change light intensity to do the fade-in effect */
		CurrentLightIntensity[nCurrentActiveLight] = (float)sin(((float)(dwTimeElapsed)/3000.0f)*PI) * LightInfo[nCurrentActiveLight].fIntensity;
	}
	else
	{
		/* Reset time variable */
		dwTimeElapsed = 0;

		/* Restore original intensity */
		for (i=3; i<9; i++) 
		{
			CurrentLightIntensity[i] = LightInfo[i].fIntensity;
		}
	}


	/* If shadows are enabled, calculate the shadow volumes */
	if (bDrawShadow) 
	{
		nCurr = NUM_MESHES;
		while(nCurr) 
		{
			nCurr--;

			/* No shadows for these meshes */
			if (!IsMeshShadowCaster(nCurr)) continue;

			/* Set mesh pointer */
			psMdl = &g_psMdl[nCurr];

			nLight = g_dwLightNum;
			while (nLight) 
			{
				nLight--;

				vWorld.x = LightInfo[nLight].TPosition.x - Mesh[nCurr].fCenter[0];
				vWorld.y = LightInfo[nLight].TPosition.y - Mesh[nCurr].fCenter[1];
				vWorld.z = LightInfo[nLight].TPosition.z - Mesh[nCurr].fCenter[2];

				fNormalise = 1.0f / (float)sqrt(vWorld.x*vWorld.x + vWorld.y*vWorld.y + vWorld.z*vWorld.z);

				vWorld.x = LightInfo[nLight].TPosition.x - vWorld.x * fNormalise;
				vWorld.y = LightInfo[nLight].TPosition.y - vWorld.y * fNormalise;
				vWorld.z = LightInfo[nLight].TPosition.z - vWorld.z * fNormalise;

				/* Calculate the model-space light position */
				BackTransform(&vModel, &vWorld, 1, &psMdl->mWorld);

				/* Skip calculations of meshes that are not shadow casters with minor lights */
				if (nLight>=L_MINORLIGHT01 && nLight<=L_MINORLIGHT06)
				{
					if (!IsMeshShadowedByHeadLight(nCurr)) continue;
				}

				/* Calculate shadow silhouettes */
				ShadowSilhouetteProjectedBuild(&psMdl->sShadowMesh, &psMdl->psShadowVol[nLight], &vWorld, &vModel);
			}
		}
	}
}


/*****************************************************************************
 * Function Name  : BackTransform
 * Inputs		  : 
 * Outputs		  : 
 * Globals used   : Yes
 * Returns		  : Nothing
 * Description    : 
 *****************************************************************************/
static void BackTransform(D3DVECTOR *pvTrans, D3DVECTOR *pvOrig, int nCount, D3DMATRIX *pmWorld)
{
	D3DMATRIX	mBack;
	
	/* Get inverse matrix */
	D3DXMatrixInverse((D3DXMATRIX *)&mBack, NULL, (D3DXMATRIX *)pmWorld);

	/* Back transform vertex */
	pvTrans->x =	mBack._11*pvOrig->x + mBack._21*pvOrig->y + mBack._31*pvOrig->z + mBack._41;
	pvTrans->y =	mBack._12*pvOrig->x + mBack._22*pvOrig->y + mBack._32*pvOrig->z + mBack._42;
	pvTrans->z =	mBack._13*pvOrig->x + mBack._23*pvOrig->y + mBack._33*pvOrig->z + mBack._43;
}


/*******************************************************************************
 * Function Name  : DrawParticle
 * Inputs		  : lpDev9, fX, fY, fZ, fRadius
 * Output		  : None
 * Returns		  : 
 * Globals used	  : None
 * Description    : Display a forward-facing particle quad on the screen
 *******************************************************************************/
static void DrawParticle(LPDIRECT3DDEVICE9 lpDev9, float fX, float fY, float fZ, float fRadius)
{
	HRESULT		hres;
	D3DVECTOR	RightVector, UpVector;
	XYZUVVERTEX	ParticleVertices[4];
	D3DMATRIX	WorldMatrix;
	
	/* Get right and up vectors */
	RightVector.x = ViewMatrix._11;
	RightVector.y = ViewMatrix._21;
	RightVector.z = ViewMatrix._31;
	UpVector.x =	ViewMatrix._12;
	UpVector.y =	ViewMatrix._22;
	UpVector.z =	ViewMatrix._32;

	/* Normalize them */
	D3DXVec3Normalize((D3DXVECTOR3 *)&RightVector, (D3DXVECTOR3 *)&RightVector);
	D3DXVec3Normalize((D3DXVECTOR3 *)&UpVector, (D3DXVECTOR3 *)&UpVector);
	RightVector.x *=	fRadius;
	RightVector.y *=	fRadius;
	RightVector.z *=	fRadius;
	UpVector.x *=		fRadius;
	UpVector.y *=		fRadius;
	UpVector.z *=		fRadius;

	/* Set all 4 particles vertices so that it always face the camera */
	ParticleVertices[0].x = -RightVector.x - UpVector.x;
	ParticleVertices[0].y = -RightVector.y - UpVector.y;
	ParticleVertices[0].z = -RightVector.z - UpVector.z;
	ParticleVertices[0].tu = 0.0f;
	ParticleVertices[0].tv = 0.0f;
	ParticleVertices[1].x = -RightVector.x + UpVector.x;
	ParticleVertices[1].y = -RightVector.y + UpVector.y;
	ParticleVertices[1].z = -RightVector.z + UpVector.z;
	ParticleVertices[1].tu = 0.0f;
	ParticleVertices[1].tv = 1.0f;
	ParticleVertices[2].x =  RightVector.x - UpVector.x;
	ParticleVertices[2].y =  RightVector.y - UpVector.y;
	ParticleVertices[2].z =  RightVector.z - UpVector.z;
	ParticleVertices[2].tu = 1.0f;
	ParticleVertices[2].tv = 0.0f;
	ParticleVertices[3].x =  RightVector.x + UpVector.x;
	ParticleVertices[3].y =  RightVector.y + UpVector.y;
	ParticleVertices[3].z =  RightVector.z + UpVector.z;
	ParticleVertices[3].tu = 1.0f;
	ParticleVertices[3].tv = 1.0f;

	/* Set world matrix */
	D3DXMatrixTranslation((D3DXMATRIX *)&WorldMatrix, fX, fY, fZ);
	lpDev9->SetTransform(D3DTS_WORLD, &WorldMatrix);

	/* Set FVF */
	lpDev9->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

	/* Draw particle */
	hres = lpDev9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, ParticleVertices, sizeof(XYZUVVERTEX));
	if (hres!=D3D_OK)
	{
		OutputDebugString("DrawPrimitiveUP() failed in DrawParticle()\n");
	}
}


/*******************************************************************************
 * Function Name  : RenderShadows
 * Inputs		  : lpDev9, nLight
 * Returns        : Nothing
 * Global Used    : Yes
 * Description    : Render stencilshadows
 *					This uses the Z-pass stencil shadow algorithm which doesn't
 *					require volume capping. However, depending on the camera
 *					position (e.g. if the camera is in shadow) visual errors
 *					can occur. The Z-fail method is more robust but requires
 *					shadow volume capping.
 *******************************************************************************/
static void RenderShadows(LPDIRECT3DDEVICE9 lpDev9, const int nLight) 
{
	D3DMATRIX		GlobalTransformationMatrix;
	HRESULT			hres;
	int				nMesh;

	/* Z writes OFF */
	lpDev9->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	
	/* Set VS declaration */
	lpDev9->SetVertexDeclaration(pVSDeclPosition);
	
	/* Set vertex shader to transformations only */
	lpDev9->SetVertexShader(pVSTransformOnly);

	/* Send global transformation matrix to VS */
	D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&ViewMatrix, (D3DXMATRIX *)&ProjectionMatrix);
	D3DXMatrixTranspose((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix);
	lpDev9->SetVertexShaderConstantF(0, (float *)&GlobalTransformationMatrix,  4);

	/* No pixel shader */
	hres=lpDev9->SetPixelShader(NULL);

	/* Do we want visible volumes? */
	if (bVisualiseVolumes) 
	{
		/* Render shadow volumes in a visible manner */

		/* Disable stencil test */
		lpDev9->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		/* Set additive blending */
		lpDev9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		lpDev9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		lpDev9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		/* Set multitexturing properties */
		lpDev9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		lpDev9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		lpDev9->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(10, 10, 30));
	}
	else
	{
		/* Set renderstates for volume rendering onto stencil buffer */

		/* Set color mask to disable writing to color buffer */
		lpDev9->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		/* Enable stencil test */
		lpDev9->SetRenderState(D3DRS_STENCILENABLE, TRUE);

		/* Stencil function always passes */
		lpDev9->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		lpDev9->SetRenderState(D3DRS_STENCILREF, STENCIL_REF);
		lpDev9->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	}


	/******************************
	** Two-sided stencil version **
	******************************/
	if (bSupportTwoSidedStencil && !bVisualiseVolumes)
	{
		/* Set up two-sided stencil */
		lpDev9->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
		lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		/* Increment stencil if stencil test passes for back faces */
		lpDev9->SetRenderState(D3DRS_CCW_STENCILFAIL,  D3DSTENCILOP_KEEP);
		lpDev9->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
		lpDev9->SetRenderState(D3DRS_CCW_STENCILPASS,  D3DSTENCILOP_INCR);

		/* Decrement stencil if stencil test passes for front faces */
		lpDev9->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
		lpDev9->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		lpDev9->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_DECR);


		/* Render shadow volumes */
		nMesh = NUM_MESHES;
		while(nMesh) 
		{
			nMesh--;

			/* Only cast shadows on shader caster meshes */
			if (IsMeshShadowCaster(nMesh))
			{
				/* Scene-dependent optimizations: only render volumes where the shadows are visible */
				if (nLight>=L_MINORLIGHT01 && nLight<=L_MINORLIGHT06 && !IsMeshShadowedByHeadLight(nMesh)) continue;
			
				/* Render shadows */
				ShadowSilhouetteProjectedRender(&g_psMdl[nMesh].psShadowVol[nLight], lpDev9);
			}
		}
	}
	else
	{
		/***************************************************
		** Normal version (two-sided stencil unsupported) **
		***************************************************/

		/* Two-sided stencil mode not supported, perform two passes */
		lpDev9->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);

		/* 1ST PASS */

		/* Increment stencil if test succeeds */
		lpDev9->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
		lpDev9->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		lpDev9->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_INCR);

		/* Cull back faces: only render front faces */
		lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		
		/* Render shadow volumes */
		nMesh = NUM_MESHES;
		while(nMesh) 
		{
			nMesh--;

			/* Only cast shadows on shader caster meshes */
			if (IsMeshShadowCaster(nMesh))
			{
				/* Scene-dependent optimizations: only render volumes where the shadows are visible */
				if (nLight>=L_MINORLIGHT01 && nLight<=L_MINORLIGHT06 && !IsMeshShadowedByHeadLight(nMesh)) continue;
			
				/* Render shadows */
				ShadowSilhouetteProjectedRender(&g_psMdl[nMesh].psShadowVol[nLight], lpDev9);
			}
		}

		/* 2ND PASS */

		/* Update texture factor */
		lpDev9->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(30, 10, 10));

		/* Decrement stencil if test succeeds */
		lpDev9->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
		lpDev9->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		lpDev9->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_DECR);

		/* Cull front faces: only render back faces */
		lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		/* Render shadow volumes */
		nMesh = NUM_MESHES;
		while(nMesh) 
		{
			nMesh--;

			/* Only cast shadows on shader caster meshes */
			if (IsMeshShadowCaster(nMesh))
			{
				/* Scene-dependent optimizations: only render volumes where the shadows are visible */
				if (nLight>=L_MINORLIGHT01 && nLight<=L_MINORLIGHT06 && !IsMeshShadowedByHeadLight(nMesh)) continue;
			
				/* Render shadows */
				ShadowSilhouetteProjectedRender(&g_psMdl[nMesh].psShadowVol[nLight], lpDev9);
			}
		}
	}

	/* Restore stencil:only pass further tests if stencil value is less or equal STENCIL_REF */
	lpDev9->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
	lpDev9->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
	lpDev9->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	lpDev9->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP);
	lpDev9->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_LESSEQUAL);

	/* Enable writes to color buffer */
	lpDev9->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

	/* Back to normal culling order */
	lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


/*******************************************************************************
 * Function Name  : CreateVertexShaders
 * Inputs		  : lpDev9
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Create the vertex shaders used in this program
 *******************************************************************************/
static void CreateVertexShaders(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT			hres;
	LPD3DXBUFFER	pCode;

	/* Set-up the vertex shader declarations */

	/* Position */
    D3DVERTEXELEMENT9 declPosition[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
        D3DDECL_END()
    };

	/* Position, tangent space and one set of texture coordinates */
	D3DVERTEXELEMENT9 declPositionTSUVTwoStreams[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
		{ 1,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BINORMAL, 0}, 
		{ 1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TANGENT,  0}, 
		{ 1, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
        { 1, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
        D3DDECL_END()
    };

	/* Transformed Position and one set of texture coordinates */
	D3DVERTEXELEMENT9 declTPositionUV[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITIONT, 0}, 
        { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
        D3DDECL_END()
    };


	/* Create vertex shader declarations */
	hres =	lpDev9->CreateVertexDeclaration(declPosition, &pVSDeclPosition);
	hres |= lpDev9->CreateVertexDeclaration(declPositionTSUVTwoStreams, &pVSDeclPositionTSUVTwoStreams);
	hres |= lpDev9->CreateVertexDeclaration(declTPositionUV, &pVSDeclTPositionUV);
	if (hres!=D3D_OK)
	{
		OutputDebugString("CreateVertexDeclaration() failed in InitView()\n");
	}

	/* Assemble vertex shader code (VSInitialPass.txt) */
	if (D3DXAssembleShaderFromFile("VSInitialPass.txt", NULL, NULL, 0, &pCode, NULL)==D3D_OK)
	{
		/* Create vertex shader */
		hres = lpDev9->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &pVSInitialPass);
		if (hres!=D3D_OK)
		{
			OutputDebugString("Failed to create vertex shader\n");
		}
		pCode->Release();
	}

	/* Assemble vertex shader code (VSLightPassShape.txt) */
	if (D3DXAssembleShaderFromFile("VSLightPassShape.txt", NULL, NULL, 0, &pCode, NULL)==D3D_OK)
	{
		/* Create vertex shader */
		hres = lpDev9->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &pVSLightPassShape);
		if (hres!=D3D_OK)
		{
			OutputDebugString("Failed to create vertex shader\n");
		}
		pCode->Release();
	}

	/* Assemble vertex shader code (VSTransformOnly.txt) */
	if (D3DXAssembleShaderFromFile("VSTransformOnly.txt", NULL, NULL, 0, &pCode, NULL)==D3D_OK)
	{
		/* Create vertex shader */
		hres = lpDev9->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &pVSTransformOnly);
		if (hres!=D3D_OK)
		{
			OutputDebugString("Failed to create vertex shader\n");
		}
		pCode->Release();
	}
}


/*******************************************************************************
 * Function Name  : DestroyVertexShaders
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Destroy pixel shaders
 *******************************************************************************/
static void DestroyVertexShaders()
{
	/* Release vertex shaders */
	RELEASE(pVSInitialPass);
	RELEASE(pVSLightPassShape);
	RELEASE(pVSTransformOnly);

	/* Release vertex shader declarations */
	RELEASE(pVSDeclTPositionUV);
	RELEASE(pVSDeclPosition);
	RELEASE(pVSDeclPositionTSUVTwoStreams);
}


/*******************************************************************************
 * Function Name  : CreatePixelShaders
 * Inputs		  : lpDev9
 * Returns        : Nothing
 * Global Used    : Yes
 * Description    : Render shadows
 *******************************************************************************/
static void CreatePixelShaders(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT					hres;
	LPD3DXBUFFER			pCode;
	LPDIRECT3DPIXELSHADER9	*PS[7];
	char					*pszPSFileName[] = { "", "PSInitialPass.txt", "PSLightPassFullScreenWithLightmap.txt", 
													 "PSLightPassSphereShape.txt", "PSLightPassConeShape.txt",
													 "PSViewMRTPosition.txt", "PSViewMRTNormal.txt"
												};
	int						i;

	/* Fill array of PS pointers */
	PS[1] = &pPSInitialPass;
	PS[2] = &pPSLightPassFullScreen;
	PS[3] = &pPSLightPassSphereShape;
	PS[4] = &pPSLightPassConeShape;
	PS[5] = &pPSViewMRTPosition;
	PS[6] = &pPSViewMRTNormal;

	/* Loop through all pixel shaders */
	for (i=1; i<7; i++)
	{
		/* Assemble pixel shader */
		hres = D3DXAssembleShaderFromFile(pszPSFileName[i], NULL, NULL, 0, &pCode, NULL);
		if (hres!=D3D_OK)	
		{
			sprintf(pszTmp, "Failed to assemble %s shader\n", pszPSFileName[i]);
			OutputDebugString(pszTmp);
		}

		/* Create the pixel shader */
		hres = lpDev9->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), PS[i]);
		if (hres!=D3D_OK)	
		{
			sprintf(pszTmp, "Failed to create pixel shader %s\n", pszPSFileName[i]);
			OutputDebugString(pszTmp);
		}

		/* Release pointer */
		pCode->Release();
	}
}


/*******************************************************************************
 * Function Name  : DestroyPixelShaders
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Destroy pixel shaders
 *******************************************************************************/
static void DestroyPixelShaders()
{
	/* Release all pixel shaders */
	RELEASE(pPSViewMRTNormal)
	RELEASE(pPSViewMRTPosition);
	RELEASE(pPSLightPassConeShape);
	RELEASE(pPSLightPassSphereShape);
	RELEASE(pPSLightPassFullScreen);
	RELEASE(pPSInitialPass);
}


/*******************************************************************************
 * Function Name  : RenderMRT
 * Inputs		  : lpDev9
 * Returns        : Nothing
 * Global Used    : Yes
 * Description    : Performs Deferred Shading using MRT
 *******************************************************************************/
static BOOL RenderMRT(LPDIRECT3DDEVICE9 lpDev9)
{
	HRESULT				hres;
	LPDIRECT3DSURFACE9	lpBackBuffer;
	int					i, j, nLight;
	D3DMATRIX			TextureMatrix;
	D3DMATRIX			TranslationMatrix, ScaleMatrix, WorldMatrix, GlobalTransformationMatrix;
	D3DVECTOR			ConeVector;
	float				fFloat[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float				fH;
	D3DXVECTOR3			ConeTopVertex;
	D3DMATRIX			TextureMatrixForProjectedTextures = { 0.5f,	0.0f,	0.0f,    0.0f, 
															  0.0f,	-0.5f,	0.0f,    0.0f,
															  0.0f,	0.0f,	0.0f,    0.0f,
															  0.5f, 0.5f,	0.0f,    1.0f };


	/***********************************************************
	** INITIAL PASS: Update Z-buffer and store info into MRTs **
	***********************************************************/

	/* Retrieve back buffer render target */
	hres = lpDev9->GetRenderTarget(0, &lpBackBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to retrieve back buffer surface\n");
		return FALSE;
	}
	lpBackBuffer->Release();


	/* Set multiple render targets */
	hres |= lpDev9->SetRenderTarget(3, lpMRT3);
	hres |= lpDev9->SetRenderTarget(2, lpMRT2);
	hres |= lpDev9->SetRenderTarget(1, lpMRT1);
	hres = lpDev9->SetRenderTarget(0, lpMRT0);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetRenderTarget() failed in RenderScene()\n");
		return FALSE;
	}


	/* Z states */
	lpDev9->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	lpDev9->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	/* Stencil states */
	lpDev9->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	/* Blending */
	lpDev9->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	/* Set VS declaration */
	lpDev9->SetVertexDeclaration(pVSDeclPositionTSUVTwoStreams);

	/* Set vertex shader for initial pass */
	hres=lpDev9->SetVertexShader(pVSInitialPass);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetVertexShader() failed()\n");
	}

	/* Set Pixel Shader */
	hres=lpDev9->SetPixelShader(pPSInitialPass);
	if (hres!=D3D_OK)
	{
		OutputDebugString("SetPixelShader() failed()\n");
	}

	/* Set sampler states */
	for (i=0; i<3; i++)
	{
		lpDev9->SetSamplerState(i, D3DSAMP_MINFILTER, nTextureFilter);	
		lpDev9->SetSamplerState(i, D3DSAMP_MAGFILTER, nTextureFilter);	
		lpDev9->SetSamplerState(i, D3DSAMP_MIPFILTER, nMipFilter);
		lpDev9->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		lpDev9->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}

	/* Render all models in initial pass */
	for (i=0; i<NUM_MESHES; i++) 
	{
		/* Set stream sources */
		lpDev9->SetStreamSource(0, g_psMdl[i].lpVertexBufferXYZ, 0, sizeof(XYZVERTEX));
		lpDev9->SetStreamSource(1, g_psMdl[i].lpVertexBufferTSUV, 0, sizeof(TSUVVERTEX));

		/* Set textures */
		lpDev9->SetTexture(0, lpDiffuseMap[Mesh[i].nMaterial]);
		lpDev9->SetTexture(1, lpNormalMap[Mesh[i].nMaterial]);

		/* Render mesh */
		RenderModel(lpDev9, &g_psMdl[i], 0);
	}


	/* Disable all MRTs */
	hres = lpDev9->SetRenderTarget(3, NULL);
	hres |= lpDev9->SetRenderTarget(2, NULL);
	hres |= lpDev9->SetRenderTarget(1, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to disable MRTs\n");
	}


	/* Restore back buffer render target */
	hres = lpDev9->SetRenderTarget(0, lpBackBuffer);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to retrieve back buffer surface\n");
	}


	/* Are we in "VIEW MRT" mode? */
	if (nViewMRTMode)
	{
		/* Set VS declaration */
		lpDev9->SetVertexDeclaration(pVSDeclTPositionUV);

		/* MRT view mode to use */
		switch(nViewMRTMode)
		{
			/* View position MRT */
			case 1: lpDev9->SetTexture(0, pMRT1616XY);
					lpDev9->SetTexture(1, pMRT1616Z);
					lpDev9->SetPixelShader(pPSViewMRTPosition);
					fFloat[0] = 1.0f/5000.0f;
					fFloat[1] = 1.0f/5000.0f;
					fFloat[2] = 1.0f/5000.0f;
					fFloat[3] = 0.0f;
					lpDev9->SetPixelShaderConstantF(0, (float *)fFloat,  1);
					fFloat[0] = 0.5f;
					fFloat[1] = 0.0f;
					fFloat[2] = 0.5f;
					fFloat[3] = 0.0f;
					lpDev9->SetPixelShaderConstantF(1, (float *)fFloat,  1);
					break;

			/* View normal MRT */
			case 2: lpDev9->SetTexture(0, pMRT8888Normal);
					lpDev9->SetPixelShader(pPSViewMRTNormal);
					break;

			/* View diffuse MRT */
			case 3: lpDev9->SetTexture(0, pMRTDiffuse);
					lpDev9->SetPixelShader(pPSViewMRTNormal);
					break;
		}

		/* Sampler states */
		lpDev9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);	
		lpDev9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);	
		lpDev9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		lpDev9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		lpDev9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		lpDev9->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);	
		lpDev9->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);	
		lpDev9->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		lpDev9->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		lpDev9->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		/* Disable Z-writes */
		lpDev9->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		
		/* Render full-screen quad */
		RenderScreenAlignedQuad(lpDev9, (float)dwCurrentWidth, (float)dwCurrentHeight, (float)dwMRTWidth, (float)dwMRTHeight);
	}
	else
	{

		
		/*******************************
		** LIGHT CONTRIBUTIONS PASSES **
		*******************************/



		/* Loop through all enabled lights */
		nLight = g_dwLightNum;
		while (nLight) 
		{
			nLight--;

			/* Skip lights that are not enabled */
			if (!bDrawLight[nLight]) continue;


			/* RENDER SHADOWS ONTO STENCIL BUFFER */
			if (bDrawShadow) 
			{
				/* Clear stencil buffer before drawing volumes for this light */
				lpDev9->Clear(0, NULL, D3DCLEAR_STENCIL, 0xFF000000, 1.0f, STENCIL_REF);

				/* Render shadows */
				RenderShadows(lpDev9, nLight);

				/* Stencil test */
				lpDev9->SetRenderState(D3DRS_STENCILENABLE, !bVisualiseVolumes);
			}

			/* Z states */
			lpDev9->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			lpDev9->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

			/* Set additive blending */
			lpDev9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			lpDev9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			lpDev9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			/* Set light position in Pixel Shader */
			lpDev9->SetPixelShaderConstantF(0, (float *)&LightInfo[nLight].TPosition,  1);

			/* Set light intensity in Pixel Shader */
			fFloat[0] = fFloat[1] = fFloat[2] = fFloat[3] = CurrentLightIntensity[nLight];
			lpDev9->SetPixelShaderConstantF(1, fFloat,  1);

			/* Set lightmap texture transformation matrix to PS */
			D3DXMatrixLookAtLH((D3DXMATRIX *)&TextureMatrix, (D3DXVECTOR3 *)&LightInfo[nLight].TPosition, (D3DXVECTOR3 *)&LightInfo[nLight].TTarget, &g_UpVector);
			D3DXMatrixMultiply((D3DXMATRIX *)&TextureMatrix, (D3DXMATRIX *)&TextureMatrix, (D3DXMATRIX *)&TextureProjectionMatrix);
			D3DXMatrixMultiply((D3DXMATRIX *)&TextureMatrix, (D3DXMATRIX *)&TextureMatrix, (D3DXMATRIX *)&TextureMatrixForProjectedTextures);
			D3DXMatrixTranspose((D3DXMATRIX *)&TextureMatrix, (D3DXMATRIX *)&TextureMatrix);
			lpDev9->SetPixelShaderConstantF(4, (float *)&TextureMatrix, 4);

			/* Set camera position in Pixel Shader */
			lpDev9->SetPixelShaderConstantF(8, (float *)&Camera[nCurrentCamera].fPosition,  1);

			/* Set border color for fullscreen light (only used for shaders with lightmaps) */
			if (nLight==0)
			{
				lpDev9->SetPixelShaderConstantF(14, FullscreenLightBorderColor, 1);
			}

			/* Set max distance for falloff */
			fFloat[0] = 1.0f; fFloat[1] = 1.0f; fFloat[2] = 1.0f; fFloat[3] = 1.0f/LightInfo[nLight].fMaxRange;
			lpDev9->SetPixelShaderConstantF(22, (float *)fFloat,  1);


			/* Set sampler states */

			/* Textures 0, 1, 2 and 3 are MRT textures, hence no filtering is required */
			for (j=0; j<4; j++)
			{
				lpDev9->SetSamplerState(j, D3DSAMP_MINFILTER, D3DTEXF_POINT);	
				lpDev9->SetSamplerState(j, D3DSAMP_MAGFILTER, D3DTEXF_POINT);	
				lpDev9->SetSamplerState(j, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
				lpDev9->SetSamplerState(j, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
				lpDev9->SetSamplerState(j, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			}

			/* Texture 4, 5, 6 and 7 (no MIPMapping for these textures) */
			for (j=4; j<8; j++)
			{
				lpDev9->SetSamplerState(j, D3DSAMP_MINFILTER, nTextureFilter);	
				lpDev9->SetSamplerState(j, D3DSAMP_MAGFILTER, nTextureFilter);	
				lpDev9->SetSamplerState(j, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
				lpDev9->SetSamplerState(j, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
				lpDev9->SetSamplerState(j, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
				lpDev9->SetSamplerState(j, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
			}


			/* Set textures */
			hres = lpDev9->SetTexture(0, pMRT1616XY);
			hres |= lpDev9->SetTexture(1, pMRT1616Z);
			hres |= lpDev9->SetTexture(2, pMRT8888Normal);
			hres |= lpDev9->SetTexture(3, pMRTDiffuse);
			hres |= lpDev9->SetTexture(4, nLight==L_MAJORLIGHT01 ? lpLightMap : NULL);
			hres |= lpDev9->SetTexture(5, lpFallOffSharp);
			hres |= lpDev9->SetTexture(6, lpNormalizationCubeMap);
			hres |= lpDev9->SetTexture(7, lpPowerLookUpTexture);
			if (hres!=D3D_OK)
			{
				OutputDebugString("SetTexture() failed\n");
			}


			/* Render full-screen quad or shape light depending on light type */
			if (LightInfo[nLight].eShape==FULLSCREEN)
			{
				/*********************
				** Fullscreen light **
				*********************/
				
				/* Set VS declaration */
				hres=lpDev9->SetVertexDeclaration(pVSDeclTPositionUV);
				if (hres!=D3D_OK)
				{
					OutputDebugString("SetVertexDeclaration() failed\n");
				}

				/* Set Pixel Shader */
				hres=lpDev9->SetPixelShader(pPSLightPassFullScreen);
				if (hres!=D3D_OK)
				{
					OutputDebugString("SetPixelShader() failed in RenderModel()\n");
				}

				/* Render screen-aligned quad onto back buffer */
				RenderScreenAlignedQuad(lpDev9, (float)dwCurrentWidth, (float)dwCurrentHeight, (float)dwMRTWidth, (float)dwMRTHeight);
			}
			else if (LightInfo[nLight].eShape==SPHERE)
			{
				/************************
				** Sphere-shaped light **
				************************/

				/* Set vertex declaration */
				hres=lpDev9->SetVertexDeclaration(pVSDeclPosition);
				if (hres!=D3D_OK)
				{
					OutputDebugString("SetVertexDeclaration() failed\n");
				}

				/* Set Vertex Shader */
				hres=lpDev9->SetVertexShader(pVSLightPassShape);
				if (hres!=D3D_OK)
				{
					OutputDebugString("SetVertexShader() failed in RenderModel()\n");
				}

				/* Calculate translation matrix for this light */
				D3DXMatrixTranslation((D3DXMATRIX *)&TranslationMatrix, LightInfo[nLight].TPosition.x,  LightInfo[nLight].TPosition.y, LightInfo[nLight].TPosition.z);

				/* Apply uniform scaling to accomodate for the low tesselation of the volumes */
				D3DXMatrixScaling((D3DXMATRIX *)&ScaleMatrix, 1.1f, 1.1f, 1.1f);
					
				/* Concatenate the two matrices */
				D3DXMatrixMultiply((D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&ScaleMatrix, (D3DXMATRIX *)&TranslationMatrix);

				/* Concatenate world, view and projection matrix */
				D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&ViewMatrix);
				D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&ProjectionMatrixForLightShape);
				D3DXMatrixTranspose((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix);

				/* Set VS constants */

				/* Global transformation matrix */
				lpDev9->SetVertexShaderConstantF(0, (float *)&GlobalTransformationMatrix, 4);

				/* Viewport constants */
				fFloat[0] = 1.0f/(2.0f*dwMRTWidth);
				fFloat[1] = 1.0f/(2.0f*dwMRTHeight);
				fFloat[2] = 0.0f;
				fFloat[3] = 0.0f;
				lpDev9->SetVertexShaderConstantF(9, fFloat, 1);

				/* Set stream source */
				lpDev9->SetStreamSource(0, LightInfo[nLight].lpVBLightShape, 0, sizeof(XYZVERTEX));

				/* Cull front faces */
				lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

				/* Set Pixel Shader */
				hres=lpDev9->SetPixelShader(pPSLightPassSphereShape);
				if (hres!=D3D_OK)
				{
					OutputDebugString("SetPixelShader() failed in RenderModel()\n");
				}

				/* Draw sphere model */
				hres=lpDev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, LightInfo[nLight].nNumVertex-2);
				if (hres!=D3D_OK)
				{
					OutputDebugString("Failed to render sphere\n");
				}
				
				/* Back to culling back faces */
				lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			}
			else
			{
				/**********************
				** Cone-shaped light **
				**********************/
				
				/* Set vertex declaration */
				hres=lpDev9->SetVertexDeclaration(pVSDeclPosition);
				if (hres!=D3D_OK)
				{
					OutputDebugString("SetVertexDeclaration() failed\n");
				}

				/* Set Vertex Shader */
				hres=lpDev9->SetVertexShader(pVSLightPassShape);
				if (hres!=D3D_OK)
				{
					OutputDebugString("SetVertexShader() failed in RenderModel()\n");
				}

				/* Compute Cone vector */
				ConeVector.x = LightInfo[nLight].TTarget.x - LightInfo[nLight].TPosition.x;
				ConeVector.y = LightInfo[nLight].TTarget.y - LightInfo[nLight].TPosition.y;
				ConeVector.z = LightInfo[nLight].TTarget.z - LightInfo[nLight].TPosition.z;

				/* ... and normalize it */
				D3DXVec3Normalize((D3DXVECTOR3 *)&ConeVector, (D3DXVECTOR3 *)&ConeVector);

				/* Calculate translation matrix for this light */
				D3DXMatrixTranslation((D3DXMATRIX *)&TranslationMatrix, LightInfo[nLight].TPosition.x,  LightInfo[nLight].TPosition.y, LightInfo[nLight].TPosition.z);

				/* Apply uniform scaling to accomodate for the low tesselation of the volumes */
				D3DXMatrixScaling((D3DXMATRIX *)&ScaleMatrix, 1.1f, 1.1f, 1.1f);

				/* Get rotation matrix between the up vector and our light vector */
				ComputeRotationMatrixFromTwoVectors((D3DXVECTOR3 *)&g_UpVector, (D3DXVECTOR3 *)&ConeVector, (D3DXMATRIX *)&WorldMatrix);
				
				/* Apply scale and rotation matrices to world matrix */
				D3DXMatrixMultiply((D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&ScaleMatrix);
				D3DXMatrixMultiply((D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&TranslationMatrix);

				/* Concatenate world, view and projection matrix */
				D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&WorldMatrix, (D3DXMATRIX *)&ViewMatrix);
				D3DXMatrixMultiply((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&ProjectionMatrixForLightShape);
				D3DXMatrixTranspose((D3DXMATRIX *)&GlobalTransformationMatrix, (D3DXMATRIX *)&GlobalTransformationMatrix);

				/* Set VS constants */

				/* Global transformation matrix */
				lpDev9->SetVertexShaderConstantF(0, (float *)&GlobalTransformationMatrix, 4);

				/* Viewport constants */
				fFloat[0] = 1.0f/(2.0f*dwMRTWidth);
				fFloat[1] = 1.0f/(2.0f*dwMRTHeight);
				fFloat[2] = 0.0f;
				fFloat[3] = 0.0f;
				lpDev9->SetVertexShaderConstantF(9, fFloat, 1);

				/* Set stream source */
				lpDev9->SetStreamSource(0, LightInfo[nLight].lpVBLightShape, 0, sizeof(XYZVERTEX));

				/* Set indices */
				lpDev9->SetIndices(LightInfo[nLight].lpIBLightShape);

				/* Send cone vector */
				lpDev9->SetPixelShaderConstantF(25, (float *)&ConeVector,  1);

				/* Calculate the top vertex of the real cone */
				fH = (LightInfo[nLight].fConeHeight*LightInfo[nLight].fConeTopRadius) /
					 (LightInfo[nLight].fConeTopRadius - LightInfo[nLight].fConeBottomRadius);
				ConeTopVertex.x = LightInfo[nLight].TPosition.x - (fH-LightInfo[nLight].fConeHeight)*ConeVector.x;
				ConeTopVertex.y = LightInfo[nLight].TPosition.y - (fH-LightInfo[nLight].fConeHeight)*ConeVector.y;
				ConeTopVertex.z = LightInfo[nLight].TPosition.z - (fH-LightInfo[nLight].fConeHeight)*ConeVector.z;
				lpDev9->SetPixelShaderConstantF(26, (float *)&ConeTopVertex,  1);

				/* Compute half-cone angle cosine */
				float fAB2 = fH*fH;
				float fAC2 = LightInfo[nLight].fConeTopRadius*LightInfo[nLight].fConeTopRadius;
				float fCosAlpha = fH / (float)sqrt( fAC2 + fAB2 );

				/* Send some constants to the pixel shader */
				fFloat[0] = 1.0f;	fFloat[1] = (1.0f - fCosAlpha);	fFloat[2] = 1.0f/(1.0f - fCosAlpha); fFloat[3] = fCosAlpha;
				lpDev9->SetPixelShaderConstantF(27, (float *)fFloat,  1);

				/* Cull front faces */
				lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

				/* Set Pixel Shader */
				hres=lpDev9->SetPixelShader(pPSLightPassConeShape);
				if (hres!=D3D_OK)
				{
					OutputDebugString("SetPixelShader() failed in RenderModel()\n");
				}

				/* Draw cone model */
				hres=lpDev9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, LightInfo[nLight].nNumVertex, 0, LightInfo[nLight].wNumIndices/3);
				if (hres!=D3D_OK)
				{
					OutputDebugString("Failed to render cone\n");
				}
	
				/* Back to culling back faces */
				lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			}

			/* Disable stencil test */
			lpDev9->SetRenderState(D3DRS_STENCILENABLE, FALSE);

			/* Render light shapes in a visible manner */
			if (bDrawLightZones)
			{
				/* No pixel shader */
				lpDev9->SetPixelShader(NULL);

				/* Set TSS */
				lpDev9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				lpDev9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
				lpDev9->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(10, 10, 30));

				if (LightInfo[nLight].eShape==CONE)
				{
					/* Only render back faces */
					lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

					/* Draw model */
					hres=lpDev9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, LightInfo[nLight].nNumVertex, 0, LightInfo[nLight].wNumIndices/3);
					if (hres!=D3D_OK)
					{
						OutputDebugString("Failed to render sphere\n");
					}

					/* Back to culling back faces */
					lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				}
				else if (LightInfo[nLight].eShape==SPHERE)
				{
					/* Only render back faces */
					lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

					/* Draw sphere model */
					hres=lpDev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, LightInfo[nLight].nNumVertex-2);
					if (hres!=D3D_OK)
					{
						OutputDebugString("Failed to render sphere\n");
					}

					/* Back to culling back faces */
					lpDev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				}
				else
				{
					/* Fullscreen light */

					/* Render screen-aligned quad onto back buffer */
					RenderScreenAlignedQuad(lpDev9, (float)dwCurrentWidth, (float)dwCurrentHeight, (float)dwMRTWidth, (float)dwMRTHeight);
				}
			}

			/* Z test back to normal */
			lpDev9->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

			/* Render light source */
			RenderLightSource(lpDev9, nLight, bDrawLightDirection);
		}
	}

	/* Render tangent space vectors */
	if (bDrawTangentSpace)
	{
		/* Draw tangent space for all meshes */
		for (i=0; i<NUM_MESHES; i++)
		{
			DrawTangentSpace(lpDev9, i);
		}
	}

	/* No problem occured */
	return TRUE;
}


/*******************************************************************************
 * Function Name  : RenderScreenAlignedQuad
 * Inputs		  : lpDev9, fRenderWidth, fRenderHeight, fTextureWidth, fTextureHeight
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Render a screen-aligned quad in homogeneous clipping coordinates 
 *					[-1..1, -1..1]
 *					The D3D offset is added so that the upper-left corner of each 
 *					pixels correspond to the centre of each texel. This function can 
 *					be used with Image-Based Rendering algorithms to sample screen-sized 
 *					textures.
 *					If non-POW2 textures are used then render and texture size are 
 *					usually the same.
 *******************************************************************************/
static void RenderScreenAlignedQuad(LPDIRECT3DDEVICE9 lpDev9, 
									float fRenderWidth, float fRenderHeight,
									float fTextureWidth, float fTextureHeight)
{
	HRESULT			hres;
	XYZWUVVERTEX	QuadVertices[4];
	float			fu0, fv0, fu1, fv1;

	/* Compute UVs to use */
	fu0 = 1.0f/(2.0f*fTextureWidth);
	fv0 = 1.0f/(2.0f*fTextureHeight);
	fu1 = (fRenderWidth/fTextureWidth) + fu0;
	fv1 = (fRenderHeight/fTextureHeight) + fv0;

	/* Set quad vertices */
	QuadVertices[0].x =  0.0f;
	QuadVertices[0].y =  0.0f;
	QuadVertices[0].z =  0.0f;
	QuadVertices[0].w =  1.0f;
	QuadVertices[0].tu = fu0;
	QuadVertices[0].tv = fv0;

	QuadVertices[1].x =  fRenderWidth;
	QuadVertices[1].y =  0.0f;
	QuadVertices[1].z =  0.0f;
	QuadVertices[1].w =  1.0f;
	QuadVertices[1].tu = fu1;
	QuadVertices[1].tv = fv0;

	QuadVertices[2].x =  0.0f;
	QuadVertices[2].y =  fRenderHeight;
	QuadVertices[2].z =  0.0f;
	QuadVertices[2].w =  1.0f;
	QuadVertices[2].tu = fu0;
	QuadVertices[2].tv = fv1;

	QuadVertices[3].x =  fRenderWidth;
	QuadVertices[3].y =  fRenderHeight;
	QuadVertices[3].z =  0.0f;
	QuadVertices[3].w =  1.0f;
	QuadVertices[3].tu = fu1;
	QuadVertices[3].tv = fv1;

	/* Draw particle */
	hres = lpDev9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, QuadVertices, sizeof(XYZWUVVERTEX));
	if (hres!=D3D_OK)
	{
		OutputDebugString("DrawPrimitiveUP() failed in RenderScreenAlignedQuad()\n");
	}
}


/*******************************************************************************
 * Function Name  : RenderLightSource
 * Inputs		  : lpDev9, nLight, bDrawLightDirection
 * Returns        : Nothing
 * Global Used    : 
 * Description    : 
 *******************************************************************************/
static void RenderLightSource(LPDIRECT3DDEVICE9 lpDev9, int nLight, BOOL bDrawLightDirection)
{
	HRESULT hres;

	/* Stencil state */
	lpDev9->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	/* Set additive blending */
	lpDev9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	lpDev9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	lpDev9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	/* No vertex shader */
	lpDev9->SetVertexShader(NULL);

	/* No pixel shader */
	lpDev9->SetPixelShader(NULL);

	/* TSS */
	lpDev9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	lpDev9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	/* Set texture */
	lpDev9->SetTexture(0, lpLightTexture);

	/* Draw light */
	DrawParticle(lpDev9, LightInfo[nLight].TPosition.x, LightInfo[nLight].TPosition.y, LightInfo[nLight].TPosition.z, 10.0f);

	/* Draw light direction */
	if (bDrawLightDirection)
	{
		D3DMATRIX	WorldMatrix;
		D3DVECTOR	Line[2];

		/* Set world matrix to identity */
		D3DXMatrixIdentity((D3DXMATRIX *)&WorldMatrix);
		lpDev9->SetTransform(D3DTS_WORLD, &WorldMatrix);

		/* Set FVF */
		lpDev9->SetFVF(D3DFVF_XYZ);

		/* Set line data */
		Line[0] = LightInfo[nLight].TPosition;
		Line[1] = LightInfo[nLight].TTarget;

		/* Set diffuse lighting */
		lpDev9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		lpDev9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		/* Draw line */
		hres = lpDev9->DrawPrimitiveUP(D3DPT_LINELIST, 1, Line, sizeof(D3DVECTOR));
		if (hres!=D3D_OK)
		{
			OutputDebugString("DrawPrimitiveUP() failed in DrawParticle()\n");
		}
	}
}


/*******************************************************************************
 * Function Name  : DrawTangentSpace
 * Inputs		  : lpDev9, nMesh
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Draw tangent space for mesh nMesh
 *					This is a slow function since DrawPrimitiveUP is called once
 *					per vertex but since this is a debugging and educative feature
 *					no effort was made to optimize it.
 *******************************************************************************/
static void DrawTangentSpace(LPDIRECT3DDEVICE9 lpDev9, int nMesh)
{
	HRESULT				hres;
	D3DMATRIX			WorldMatrix;
	XYZDIFFUSEVERTEX	Line[6];
	XYZVERTEX			*pVertexXYZ;
	TSUVVERTEX			*pVertexTSUV;
	float				fScale=10.0f;
	int					i;

	/* Stencil state */
	lpDev9->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	/* Blending state */
	lpDev9->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	/* No vertex shader */
	lpDev9->SetVertexShader(NULL);

	/* No pixel shader */
	lpDev9->SetPixelShader(NULL);

	/* Set world matrix to identity */
	D3DXMatrixIdentity((D3DXMATRIX *)&WorldMatrix);
	lpDev9->SetTransform(D3DTS_WORLD, &WorldMatrix);

	/* Set FVF */
	lpDev9->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	/* Set diffuse lighting */
	lpDev9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	lpDev9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	/* Lock MANAGED vertex buffer */
	g_psMdl[nMesh].lpVertexBufferXYZ->Lock(0, 0, (void **)&pVertexXYZ, D3DLOCK_READONLY);
	g_psMdl[nMesh].lpVertexBufferTSUV->Lock(0, 0, (void **)&pVertexTSUV, D3DLOCK_READONLY);

	/* Create a set of three line at each vertex */
	for (i=0; i<(int)g_psMdl[nMesh].nNumVertex; i++)
	{
		/* Set line data */
		Line[0].x = pVertexXYZ[i].x;
		Line[0].y = pVertexXYZ[i].y;
		Line[0].z = pVertexXYZ[i].z;
		Line[1].x = Line[0].x + fScale * pVertexTSUV[i].Bx;
		Line[1].y = Line[0].y + fScale * pVertexTSUV[i].By;
		Line[1].z = Line[0].z + fScale * pVertexTSUV[i].Bz;

		Line[2].x = Line[0].x;
		Line[2].y = Line[0].y;
		Line[2].z = Line[0].z;
		Line[3].x = Line[2].x + fScale * pVertexTSUV[i].Tx;
		Line[3].y = Line[2].y + fScale * pVertexTSUV[i].Ty;
		Line[3].z = Line[2].z + fScale * pVertexTSUV[i].Tz;

		Line[4].x = Line[0].x;
		Line[4].y = Line[0].y;
		Line[4].z = Line[0].z;
		Line[5].x = Line[4].x + fScale * pVertexTSUV[i].Nx;
		Line[5].y = Line[4].y + fScale * pVertexTSUV[i].Ny;
		Line[5].z = Line[4].z + fScale * pVertexTSUV[i].Nz;

		/* B=green, T=Blue, N=Red */
		Line[0].diffuse = Line[1].diffuse = D3D_RGBA(0, 255, 0, 255);
		Line[2].diffuse = Line[3].diffuse = D3D_RGBA(0, 0, 255, 255);
		Line[4].diffuse = Line[5].diffuse = D3D_RGBA(255, 0, 0, 255);

		/* Set world matrix */
		D3DXMatrixIdentity((D3DXMATRIX *)&WorldMatrix);
		lpDev9->SetTransform(D3DTS_WORLD, &g_psMdl[nMesh].mWorld);

		/* Draw line */
		hres = lpDev9->DrawPrimitiveUP(D3DPT_LINELIST, 3, Line, sizeof(XYZDIFFUSEVERTEX));
		if (hres!=D3D_OK)
		{
			OutputDebugString("DrawPrimitiveUP() failed in DrawParticle()\n");
		}
	}

	/* Unlock vertex buffers */
	g_psMdl[nMesh].lpVertexBufferTSUV->Unlock();
	g_psMdl[nMesh].lpVertexBufferXYZ->Unlock();
}


/*******************************************************************************
 * Function Name  : RenderLogo
 * Inputs		  : lpDev9
 * Returns        : Nothing
 * Global Used    : 
 * Description    : 
 *******************************************************************************/
static void RenderLogo(LPDIRECT3DDEVICE9 lpDev9)
{
#define LOGO_WIDTH	89.0f
#define LOGO_HEIGHT	60.0f
	HRESULT			hres;
	XYZWUVVERTEX	QuadVertices[4];
	float			fu0, fv0, fu1, fv1;

	/* Compute UVs to use */
	fu0 = 1.0f/(2.0f*128.0f);
	fv0 = 1.0f/(2.0f*64.0f);
	fu1 = (89.0f/128.0f) + fu0;
	fv1 = (60.0f/64.0f) + fv0;

	/* Set quad vertices */
	QuadVertices[0].x =  (float)dwCurrentWidth-LOGO_WIDTH;
	QuadVertices[0].y =  (float)dwCurrentHeight-LOGO_HEIGHT;
	QuadVertices[0].z =  0.0f;
	QuadVertices[0].w =  1.0f;
	QuadVertices[0].tu = fu0;
	QuadVertices[0].tv = fv0;

	QuadVertices[1].x =  (float)dwCurrentWidth;
	QuadVertices[1].y =  (float)dwCurrentHeight-LOGO_HEIGHT;
	QuadVertices[1].z =  0.0f;
	QuadVertices[1].w =  1.0f;
	QuadVertices[1].tu = fu1;
	QuadVertices[1].tv = fv0;

	QuadVertices[2].x =  (float)dwCurrentWidth-LOGO_WIDTH;
	QuadVertices[2].y =  (float)dwCurrentHeight;
	QuadVertices[2].z =  0.0f;
	QuadVertices[2].w =  1.0f;
	QuadVertices[2].tu = fu0;
	QuadVertices[2].tv = fv1;

	QuadVertices[3].x =  (float)dwCurrentWidth;
	QuadVertices[3].y =  (float)dwCurrentHeight;
	QuadVertices[3].z =  0.0f;
	QuadVertices[3].w =  1.0f;
	QuadVertices[3].tu = fu1;
	QuadVertices[3].tv = fv1;

	/* Depth/Stencil state */
	lpDev9->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	lpDev9->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	/* Alpha blending disabled */
	lpDev9->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	/* No vertex shader */
	lpDev9->SetVertexShader(NULL);

	/* Set FVF */
	lpDev9->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

	/* No pixel shader */
	lpDev9->SetPixelShader(NULL);

	/* TSS */
	lpDev9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	lpDev9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	/* Set texture */
	lpDev9->SetTexture(0, lpPowerVRLogoTexture);


	/* Draw particle */
	hres = lpDev9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, QuadVertices, sizeof(XYZWUVVERTEX));
	if (hres!=D3D_OK)
	{
		OutputDebugString("DrawPrimitiveUP() failed in RenderScreenAlignedQuad()\n");
	}
}



/*******************************************************************************
 * Function Name  : RenderInfoText
 * Inputs		  : lpDev9
 * Returns        : Nothing
 * Global Used    : 
 * Description    : 
 *******************************************************************************/
static void RenderInfoText(LPDIRECT3DDEVICE9 lpDev9)
{
	/* Unset pixel shader */
	lpDev9->SetPixelShader(NULL);

#if 0

	/* Display info text */
	if (nTextOn==2)
	{
		char	pszOn[] = "ON";
		char	pszOff[] = "OFF";
		int		i=0;

		/* Stencil state */
		lpDev9->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		D3DTPrint3D(1.0f, 90.0f, 0.5f, 0xFFFFFFFF, "Selected camera: %d  Pos=(%2.f,%2.f,%2.f)", 
			nCurrentCamera, Camera[nCurrentCamera].fPosition[0], Camera[nCurrentCamera].fPosition[1], Camera[nCurrentCamera].fPosition[2]);
		
		if (bModifyTarget)
		{
			D3DTPrint3D(1.0f, 94.0f, 0.5f, 0xFFFFFFFF, "Selected light: %d  Pos=(%2.f,%2.f,%2.f)  TARGET=(%2.f,%2.f,%2.f)", 
				nSelectedLight, LightInfo[nSelectedLight].TPosition.x, LightInfo[nSelectedLight].TPosition.y, LightInfo[nSelectedLight].TPosition.z, 
				LightInfo[nSelectedLight].TTarget.x, LightInfo[nSelectedLight].TTarget.y, LightInfo[nSelectedLight].TTarget.z);
		}
		else
		{
			D3DTPrint3D(1.0f, 94.0f, 0.5f, 0xFFFFFFFF, "Selected light: %d  POS=(%2.f,%2.f,%2.f)  Target=(%2.f,%2.f,%2.f)", 
				nSelectedLight, LightInfo[nSelectedLight].TPosition.x, LightInfo[nSelectedLight].TPosition.y, LightInfo[nSelectedLight].TPosition.z, 
				LightInfo[nSelectedLight].TTarget.x, LightInfo[nSelectedLight].TTarget.y, LightInfo[nSelectedLight].TTarget.z);
		}

		/* Lights */
		D3DTPrint3D(75.0f, 90.0f, 0.5f, 0xFFFFFFFF, "Lights: %s%s%s%s%s%s%s%s%s%s", !(bDrawLight[0] || bDrawLight[1] || bDrawLight[2] || bDrawLight[3] || bDrawLight[4] || bDrawLight[5] || bDrawLight[6] || bDrawLight[7] || bDrawLight[8] || bDrawLight[9]) ? "NONE" : " ", 
		bDrawLight[0] ? "0" : " ", bDrawLight[1] ? "1" : " ", bDrawLight[2] ? "2" : " ", bDrawLight[3] ? "3" : " ", bDrawLight[4] ? "4" : " ", bDrawLight[5] ? "5" : " ",
		bDrawLight[6] ? "6" : " ", bDrawLight[7] ? "7" : " ", bDrawLight[8] ? "8" : " ");
	}

#endif

	
	/* Display frame rate */
	if (nTextOn>0)
	{
		float	fFPS;

		D3DShellGetVariable((enum _D3DShellVariables)FRAME_RATE, &fFPS);
		D3DTPrint3D(1.0f, 1.0f, 0.7f, 0xFFFF0000, "%.1f fps", fFPS);
	}
}


/*******************************************************************************
 * Function Name  : CreateSphereVertexBuffer
 * Inputs		  : lpDev9, fRadius
 * Outputs		  : *lplpVBSphere, *pnNumVertex
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Create a vertex buffer for a sphere model.
 *******************************************************************************/
static void CreateSphereVertexBuffer(LPDIRECT3DDEVICE9 lpDev9, float fRadius,
									 LPDIRECT3DVERTEXBUFFER9 *lplpVBSphere, int *pnNumVertex)
{
	HRESULT		hres;
	XYZVERTEX	*pVertex;
	DWORD		dwNumSphereRings    = 5;
    DWORD		dwNumSphereSegments = 10;	// Choose a tesselation level   
  	
	/* Compute number of vertices */
	(*pnNumVertex) = 2 * dwNumSphereRings * (dwNumSphereSegments+1);

    /* Create Vertex Buffer */
	hres=lpDev9->CreateVertexBuffer((*pnNumVertex)*sizeof(XYZVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, lplpVBSphere, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to create vertex buffer\n");
		return;
	}

	/* Lock vertex buffer */
	hres=(*lplpVBSphere)->Lock(0, 0, (void **)&pVertex, 0);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Lock() failed\n");
		RELEASE((*lplpVBSphere));
		return;
	}

    // Establish constants used in sphere generation
    FLOAT fDeltaRingAngle = ( PI / dwNumSphereRings );
    FLOAT fDeltaSegAngle  = ( 2.0f * PI / dwNumSphereSegments );

    /* Generate the group of rings for the sphere */
    for( DWORD ring = 0; ring < dwNumSphereRings; ring++ )
    {    
        FLOAT r0 = fRadius*sinf( (ring+0) * fDeltaRingAngle );
        FLOAT r1 = fRadius*sinf( (ring+1) * fDeltaRingAngle );
        FLOAT y0 = fRadius*cosf( (ring+0) * fDeltaRingAngle );
        FLOAT y1 = fRadius*cosf( (ring+1) * fDeltaRingAngle );

        // Generate the group of segments for the current ring
        for( DWORD seg = 0; seg < (dwNumSphereSegments+1); seg++ )
        {
            FLOAT x0 =  r0 * sinf( seg * fDeltaSegAngle );
            FLOAT z0 =  r0 * cosf( seg * fDeltaSegAngle );
            FLOAT x1 =  r1 * sinf( seg * fDeltaSegAngle );
            FLOAT z1 =  r1 * cosf( seg * fDeltaSegAngle );

            // Add two vertices to the strip which makes up the sphere
			pVertex->x = x0;
			pVertex->y = y0;
			pVertex->z = z0;
			pVertex++;

			pVertex->x = x1;
			pVertex->y = y1;
			pVertex->z = z1;
			pVertex++;
        }
    }

	/* Unlock vertex buffer */
	(*lplpVBSphere)->Unlock();
}


/*******************************************************************************
 * Function Name  : CreateConeVertexAndIndexBuffers
 * Inputs		  : lpDev9, r, R, H
 * Outpus		  : *lplpVBCone, *pnNumVertex, *lplpIBCone, *pwNumIndices
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Create a vertex and index buffer for a truncated cone model.
 *******************************************************************************/
static void CreateConeVertexAndIndexBuffers(LPDIRECT3DDEVICE9 lpDev9, float r, float R, float H,
											LPDIRECT3DVERTEXBUFFER9 *lplpVBCone, int *pnNumVertex,
											LPDIRECT3DINDEXBUFFER9 *lplpIBCone, WORD *pwNumIndices)
{
	HRESULT		hres;
	XYZVERTEX	*pVertex;
	WORD		*pIndex;
	int			i;
	WORD		dwNumConeSegments = 20;	// Choose a tesselation level 

    
	/* Compute number of vertices and indices */
	(*pnNumVertex) = 2 * (dwNumConeSegments + 1);
	(*pwNumIndices) = (dwNumConeSegments + dwNumConeSegments + 2*dwNumConeSegments) * 3;

    /* Create Vertex Buffer */
	hres=lpDev9->CreateVertexBuffer((*pnNumVertex)*sizeof(XYZVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, lplpVBCone, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to create vertex buffer\n");
		return;
	}

	/* Lock vertex buffer */
	hres=(*lplpVBCone)->Lock(0, 0, (void **)&pVertex, 0);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Lock() failed\n");
		RELEASE((*lplpVBCone));
		return;
	}

    // Establish constants used in sphere generation
    FLOAT fDeltaSegAngle  = ( 2.0f * PI / dwNumConeSegments );


	/* Generate vertices for top disc */

	/* First vertex is disc centre */
	pVertex->x = 0.0f;	pVertex->y = 0.0f;	pVertex->z = 0.0f;	pVertex++;
    for (i=0; i<(int)dwNumConeSegments; i++)
    {   
		pVertex->x = r*(float)cos(i*fDeltaSegAngle);
		pVertex->y = 0.0f;
		pVertex->z = r*(float)sin(i*fDeltaSegAngle);
		pVertex++;
	}

	/* Generate vertices for bottom disc */

	/* First vertex is disc centre */
	pVertex->x = 0.0f;	pVertex->y = H;	pVertex->z = 0.0f;	pVertex++;
    for (i=0; i<(int)dwNumConeSegments; i++)
    {   
		pVertex->x = R*(float)cos(i*fDeltaSegAngle);
		pVertex->y = H;
		pVertex->z = R*(float)sin(i*fDeltaSegAngle);
		pVertex++;
	}

	/* Unlock vertex buffer */
	(*lplpVBCone)->Unlock();


	/* Create Index Buffer */
	hres=lpDev9->CreateIndexBuffer((*pwNumIndices)*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, lplpIBCone, NULL);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Failed to create index buffer\n");
		RELEASE(*lplpVBCone);
		return;
	}

	/* Lock index buffer */
	hres = (*lplpIBCone)->Lock(0, 0, (void **)&pIndex, 0);
	if (hres!=D3D_OK)
	{
		OutputDebugString("Lock() failed\n");
		RELEASE((*lplpIBCone));
		RELEASE((*lplpVBCone));
		return;
	}


	// reset indices tmp
	ZeroMemory(pIndex, (*pwNumIndices)*sizeof(WORD));

	/* Create indices for top disc */
	for (i=1; i<(int)dwNumConeSegments+1; i++)
	{
		*pIndex++ = 0;
		*pIndex++ = i;
		*pIndex++ = (i==(int)dwNumConeSegments) ? 1 : i+1;
	}

	/* Create indices for bottom disc */
	for (i=1; i<(int)dwNumConeSegments+1; i++)
	{
		*pIndex++ = dwNumConeSegments+1;
		*pIndex++ = (i==(int)dwNumConeSegments) ? dwNumConeSegments + 2 : i + dwNumConeSegments + 2;
		*pIndex++ = i + dwNumConeSegments + 1;
	}
	
	/* Create indices for edges disc */
	for (i=1; i<(int)dwNumConeSegments+1; i++)
	{
		// first edge triangle 
		*pIndex++ = i;
		*pIndex++ = i+dwNumConeSegments+1;	
		*pIndex++ = i==dwNumConeSegments ? dwNumConeSegments + 2 : i+dwNumConeSegments+2;

		// second edge triangle
		*pIndex++ = i;
		*pIndex++ = i==dwNumConeSegments ? dwNumConeSegments + 2 : i+dwNumConeSegments+2;	
		*pIndex++ = i==dwNumConeSegments ? 1 : i+1;
	}

	/* Unlock index buffer */
	(*lplpIBCone)->Unlock();
}


/*******************************************************************************
 * Function Name  : ComputeRotationMatrixFromTwoVectors
 * Inputs		  : *pV1, *pV2
 * Outputs		  : *pMatrix
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Returns the rotation matrix corresponding to the rotation
 *					required to bring vector *pV1 to vector *pV2
 *******************************************************************************/
static void ComputeRotationMatrixFromTwoVectors(D3DXVECTOR3 *pV1, D3DXVECTOR3 *pV2, D3DXMATRIX *pMatrix)
{
	D3DXVECTOR3	RotationAxis;
	float		fAngle;

	/* Compute vector corresponding to rotation axis */
	D3DXVec3Cross(&RotationAxis, pV1, pV2);

	/* Compute angle between the two vectors */
	fAngle = (float)acos(D3DXVec3Dot(pV1, pV2));

	/* Compute rotation matrix */
	D3DXMatrixRotationAxis(pMatrix, &RotationAxis, fAngle);
}


/*******************************************************************************
 * Function Name  : IsMeshShadowCaster
 * Inputs		  : nMesh
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Scene-dependant function indicating which meshes are shadow
 *					casters (basically all but walls, ceiling and floor)
 *******************************************************************************/
static BOOL IsMeshShadowCaster(int nMesh)
{
	switch(nMesh)
	{
		case M_FLOOR :
		case M_WALL01 : 
		case M_WALL02 :
		case M_WALL03 :
		case M_WALL04 :
		case M_WALL05 :
		case M_WALL06 :
		case M_WALL07 :
		case M_WALL08 :
		case M_WALL09 :
		case M_WALL10 :
		case M_WALL11 :
		case M_WALL12 :
		case M_WALL13 :
		case M_WALL14 :
		case M_WALL15 :
		case M_WALL16 :
		case M_WALL17 :
		case M_WALL18 :
		case M_WALL19 :
		case M_WALL20 : return FALSE; 
		default : return TRUE;
	}
}


/*******************************************************************************
 * Function Name  : IsMeshShadowedByHeadLight
 * Inputs		  : nMesh
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Scene-dependant function indicating what meshes are affected
 *					by cone light shadows. This is simply an optimization to 
 *					reduce the number of volumes to cast.
 *******************************************************************************/
static BOOL IsMeshShadowedByHeadLight(int nMesh)
{
	switch(nMesh)
	{
		case M_STAND :
		case M_PLATFORM : 
		case M_REDSCULPTURE :
		case M_BLUESCULPTURE :
		case M_LAMP01_BASE :
		case M_LAMP02_BASE :
		case M_LAMP03_BASE :
		case M_LAMP04_BASE :
		case M_LAMP05_BASE :
		case M_LAMP06_BASE :	return TRUE;
		default :				return FALSE;
	}
}


/*******************************************************************************
 * Function Name  : UpdateMenu
 * Returns        : Nothing
 * Global Used    : Yes
 * Description    : Update menu with tickboxes and graying/enabling items as
 *					necessary
 *******************************************************************************/
static void UpdateMenu()
{
	/* OPTIONS menu */

	/* Uncheck everything */
	CheckMenuItem(hUserMenu, ID_OPTIONS_FILTERING_POINTSAMPLING,	MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_FILTERING_BILINEAR,			MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_FILTERING_TRILINEAR,		MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_FILTERING_ANISOTROPIC,		MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_NORMAL,						MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_VIEWPOSITIONMRT,			MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_VIEWNORMALMRT,				MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_VIEWDIFFUSEMRT,				MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_TEXTUREFORMAT_32BITS,		MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_TEXTUREFORMAT_16BITS,		MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_TEXTUREFORMAT_DXT,			MF_UNCHECKED);

	/* Pause/resume */
	CheckMenuItem(hUserMenu, ID_OPTIONS_PAUSESTEP, bPause ? MF_CHECKED : MF_UNCHECKED);

	/* Shadows */
	CheckMenuItem(hUserMenu, ID_OPTIONS_SHADOWS, bDrawShadow ? MF_CHECKED : MF_UNCHECKED);
	EnableMenuItem(hUserMenu, ID_OPTIONS_VISUALISEVOLUMES, bDrawShadow ? MF_ENABLED: MF_GRAYED);
	CheckMenuItem(hUserMenu, ID_OPTIONS_VISUALISEVOLUMES, (bDrawShadow && bVisualiseVolumes) ? MF_CHECKED : MF_UNCHECKED);

	
	/* Filtering mode */
	if (nMipFilter==D3DTEXF_LINEAR && nTextureFilter==D3DTEXF_ANISOTROPIC)	CheckMenuItem(hUserMenu, ID_OPTIONS_FILTERING_ANISOTROPIC, MF_CHECKED); 
	if (nMipFilter==D3DTEXF_LINEAR && nTextureFilter==D3DTEXF_LINEAR)		CheckMenuItem(hUserMenu, ID_OPTIONS_FILTERING_TRILINEAR, MF_CHECKED); 
	if (nMipFilter==D3DTEXF_NONE && nTextureFilter==D3DTEXF_LINEAR)			CheckMenuItem(hUserMenu, ID_OPTIONS_FILTERING_BILINEAR, MF_CHECKED); 
	if (nMipFilter==D3DTEXF_NONE && nTextureFilter==D3DTEXF_POINT)			CheckMenuItem(hUserMenu, ID_OPTIONS_FILTERING_POINTSAMPLING, MF_CHECKED); 

	/* Texture format */
	if (bUseCompressedTextures)
	{
		CheckMenuItem(hUserMenu, ID_OPTIONS_TEXTUREFORMAT_DXT,			MF_CHECKED);
	}
	else if (bUse16BitsTextures)
	{
		CheckMenuItem(hUserMenu, ID_OPTIONS_TEXTUREFORMAT_16BITS,		MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hUserMenu, ID_OPTIONS_TEXTUREFORMAT_32BITS,		MF_CHECKED);
	}


	/* MRT mode */
	switch (nViewMRTMode)
	{
		case 0: CheckMenuItem(hUserMenu, ID_OPTIONS_NORMAL,				MF_CHECKED); break;
		case 1: CheckMenuItem(hUserMenu, ID_OPTIONS_VIEWPOSITIONMRT,	MF_CHECKED); break;
		case 2: CheckMenuItem(hUserMenu, ID_OPTIONS_VIEWNORMALMRT,		MF_CHECKED); break;
		case 3: CheckMenuItem(hUserMenu, ID_OPTIONS_VIEWDIFFUSEMRT,		MF_CHECKED); break;
	}

	/* View tangent space */
	CheckMenuItem(hUserMenu, ID_OPTIONS_VIEWTANGENTSPACE, bDrawTangentSpace? MF_CHECKED: MF_UNCHECKED);


	/* LIGHTS menu */

	/* Lights enabling/disabling */
	CheckMenuItem(hUserMenu, ID_LIGHTS_CYCLINGLIGHTS,	bCyclingLights ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_FULLSCREENLIGHT,	bDrawLight[0] ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_SPHERELIGHT1,	bDrawLight[1] ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_SPHERELIGHT2,	bDrawLight[2] ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_CONELIGHT1,		bDrawLight[3] ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_CONELIGHT2,		bDrawLight[4] ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_CONELIGHT3,		bDrawLight[5] ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_CONELIGHT4,		bDrawLight[6] ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_CONELIGHT5,		bDrawLight[7] ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hUserMenu, ID_LIGHTS_CONELIGHT6,		bDrawLight[8] ? MF_CHECKED : MF_UNCHECKED);

	/* View light zones */
	CheckMenuItem(hUserMenu, ID_LIGHTS_VIEWLIGHTZONES, bDrawLightZones ? MF_CHECKED: MF_UNCHECKED);
}


/* END OF PROGRAM */

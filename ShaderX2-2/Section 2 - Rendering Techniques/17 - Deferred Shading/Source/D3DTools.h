/*****************************************************************************
  Name : D3DTools.h
  Date : October 2001
  Platform : ANSI compatible
 
  $Revision: 1.9 $

  Description : 
  Header file of D3DTools.lib.
  Contains structure definitions and prototypes 
  of all functions in D3DTools.c

  Copyright 2001, PowerVR Technologies.
  PowerVR Technologies is a division of Imagination Technologies Limited. 
  All Rights Reserved.
******************************************************************************/
#ifndef _D3DTOOLS_H_
#define _D3DTOOLS_H_


/******************
** Build options **
******************/

/*
	Enable this (#define USE_3D_DEVICE_WRAPPER) to use the wrapper; also
	enable it in D3DTDevWrap.h (for D3DTDevWrap and D3DTPrint3D modules,
	which do not include this file).
*/
#undef USE_3D_DEVICE_WRAPPER


/************
** Defines **
************/
#ifndef RELEASE
#define RELEASE(x)	if (x) { IDirect3D9_Release(x); (x)=NULL; }
#endif

/* D3DTPrint3D */
#define FONT_SYSTEM -1.0f
#define FONT_ARIAL   1.0f
typedef enum {
	PRINT3D_ACTIVATE_WIN		=	0x01,
	PRINT3D_DEACTIVATE_WIN		=	0x02,
	PRINT3D_ACTIVATE_TITLE		=	0x04,
	PRINT3D_DEACTIVATE_TITLE	=	0x08,
	PRINT3D_FULL_OPAQUE			=	0x10,
	PRINT3D_FULL_TRANS			=	0x20,
	PRINT3D_ADJUST_SIZE_ALWAYS	=	0x40,
	PRINT3D_NO_BORDER			=	0x80,
} PRINT3D_FLAGS;


/**********************************************************************************************
***********************************************************************************************
************************************** D3DTDEVWRAP.H ******************************************
***********************************************************************************************
***********************************************************************************************/
#ifdef USE_3D_DEVICE_WRAPPER


/*****************************************************************************
	D3D Device wrapper.
	Removes extraneous calls to Direct3D; caches render states etc and only
	sets [in the actual device] those which have changed when primitives are drawn
*****************************************************************************/
class CDevWrap {
protected:
	struct SConst	{
		float x, y, z, w;
	};

	struct SState {
		IDirect3DVertexShader9		*pVertexShader;
		SConst						VsConst[256];	// Number of vertex shader constants in vs_2_x

		IDirect3DPixelShader9		*pPixelShader;
		SConst						PsConst[32];	// Number of pixel shader constants in ps_2_x

		IDirect3DBaseTexture9		*pTex[16];	// 16 Textures in ps_2_x
		IDirect3DVertexDeclaration9	*pDecl;

		DWORD						dwFVF;

		DWORD						dwRS[196];
		DWORD						dwTSS[10][33];
		DWORD						dwSS[16][14];

		BOOL						bVDeclFVF;		// Vertex declaration set with FVF? Otherwise VDecl
	};

	IDirect3DDevice9	*m_pDev;
	SState				m_sOld, m_sNew;

public:
	CDevWrap(IDirect3DDevice9 *pDev);
	~CDevWrap();

	inline IDirect3DDevice9 *DeviceGet()
	{
		return m_pDev;
	}

	HRESULT Flush();

	/*
		Wrapper fns
	*/
	void SetTexture(
		DWORD Stage,
		IDirect3DBaseTexture9* pTexture);

	void GetTexture(
		DWORD Stage,
		IDirect3DBaseTexture9** ppTexture);

	void SetRenderState(
		D3DRENDERSTATETYPE State,
		DWORD Value);

	void GetRenderState(
		D3DRENDERSTATETYPE State,
		DWORD* pValue);

	void SetTextureStageState(
		DWORD Stage,
		D3DTEXTURESTAGESTATETYPE Type,
		DWORD Value);

	void GetTextureStageState(
		DWORD Stage,
		D3DTEXTURESTAGESTATETYPE Type,
		DWORD* pValue);

	void SetSamplerState(
		DWORD Sampler,
		D3DSAMPLERSTATETYPE Type,
		DWORD Value);

	void GetSamplerState(
		DWORD Sampler,
		D3DSAMPLERSTATETYPE Type,
		DWORD* pValue);

	void SetPixelShaderConstantF(
		DWORD Register,
		CONST float* pConstantData,
		DWORD ConstantCount);

	void SetVertexShaderConstantF(
		UINT StartRegister,
		CONST float* pConstantData,
		UINT Vector4fCount);

	void SetPixelShader(
		IDirect3DPixelShader9* pShader);

	void SetVertexShader(
		IDirect3DVertexShader9* pShader);

	void GetVertexShader(
		IDirect3DVertexShader9** ppShader);

	void SetStreamSource(
		UINT StreamNumber,
		IDirect3DVertexBuffer9* pStreamData,
		UINT OffsetInBytes,
		UINT Stride);

	void SetIndices(
		IDirect3DIndexBuffer9* pIndexData);

	void SetFVF(
		DWORD FVF);

	void GetFVF(
		DWORD* pFVF);

	void SetVertexDeclaration(
		IDirect3DVertexDeclaration9* pDecl);

	void GetVertexDeclaration(
		IDirect3DVertexDeclaration9** pDecl);

	HRESULT DrawIndexedPrimitive(
		D3DPRIMITIVETYPE Type,
		INT BaseVertexIndex,
		UINT MinIndex,
		UINT NumVertices,
		UINT StartIndex,
		UINT PrimitiveCount);

	HRESULT DrawPrimitive(
		D3DPRIMITIVETYPE PrimitiveType,
		UINT StartVertex,
		UINT PrimitiveCount);

	/*
		Create*() (& some others) pass straight through
	*/
	inline HRESULT CreateIndexBuffer(
		UINT Length,
		DWORD Usage,
		D3DFORMAT Format,
		D3DPOOL Pool,
		IDirect3DIndexBuffer9** ppIndexBuffer,
		HANDLE* pSharedHandle)
	{
		return m_pDev->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
	}

	inline HRESULT CreateTexture(
		UINT Width,
		UINT Height,
		UINT Levels,
		DWORD Usage,
		D3DFORMAT Format,
		D3DPOOL Pool,
		IDirect3DTexture9** ppTexture,
		HANDLE* pSharedHandle)
	{
		return m_pDev->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
	}

	inline HRESULT CreateOffscreenPlainSurface(
		UINT Width,
		UINT Height,
		D3DFORMAT Format,
		D3DPOOL Pool,
		IDirect3DSurface9** pSurface,
		HANDLE* pSharedHandle)
	{
		return m_pDev->CreateOffscreenPlainSurface(Width, Height, Format, Pool, pSurface, pSharedHandle);
	}

	inline HRESULT CreateVertexBuffer(
		UINT Length,
		DWORD Usage,
		DWORD FVF,
		D3DPOOL Pool,
		IDirect3DVertexBuffer9** ppVertexBuffer,
		HANDLE* pSharedHandle)
	{
		return m_pDev->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
	}

	inline HRESULT Present(
		CONST RECT* pSourceRect,
		CONST RECT* pDestRect,
		HWND hDestWindowOverride,
		CONST RGNDATA* pDirtyRegion)
	{
		return m_pDev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

	inline ULONG Release()
	{
		ULONG ulRet = m_pDev ? m_pDev->Release() : 0;
		delete this;
		return ulRet;
	}

	inline HRESULT BeginScene()
	{
//		OutputDebugString("BeginScene() {\n");
		return m_pDev->BeginScene();
	}

	inline HRESULT EndScene()
	{
//		OutputDebugString("} EndScene()\n");
		return m_pDev->EndScene();
	}

	inline HRESULT CreateVertexDeclaration(
		CONST D3DVERTEXELEMENT9* pVertexElements,
		IDirect3DVertexDeclaration9** ppDecl)
	{
		return m_pDev->CreateVertexDeclaration(pVertexElements, ppDecl);
	}

	inline HRESULT GetDeviceCaps(
		D3DCAPS9* pCaps)
	{
		return m_pDev->GetDeviceCaps(pCaps);
	}

	inline HRESULT CreatePixelShader(
		CONST DWORD* pFunction,
		IDirect3DPixelShader9** ppShader)
	{
		return m_pDev->CreatePixelShader(pFunction, ppShader);
	}

	inline HRESULT CreateVertexShader(
		const DWORD* pFunction,
		IDirect3DVertexShader9** ppShader)
	{
		return m_pDev->CreateVertexShader(pFunction, ppShader);
	}

	inline HRESULT SetViewport(
		CONST D3DVIEWPORT9* pViewport)
	{
		return m_pDev->SetViewport(pViewport);
	}

	inline HRESULT TestCooperativeLevel(
		VOID)
	{
		return m_pDev->TestCooperativeLevel();
	}

	inline HRESULT Clear(
		DWORD Count,
		const D3DRECT* pRects,
		DWORD Flags,
		D3DCOLOR Color,
		float Z,
		DWORD Stencil)
	{
		return m_pDev->Clear(Count, pRects, Flags, Color, Z, Stencil);
	}

	inline HRESULT GetFrontBufferData(
		UINT iSwapChain,
		IDirect3DSurface9* pDestSurface)
	{
		return m_pDev->GetFrontBufferData(iSwapChain, pDestSurface);
	}
};

// Use this type rather than IDirect3DDevice9:
typedef CDevWrap			IDirect3DDevice9Wrapped, *PDIRECT3DDEVICE9WRAPPED, *LPDIRECT3DDEVICE9WRAPPED;

#define RAWDEVICE(X)	((X)->DeviceGet())
#define FLUSH(X)	((X)->Flush())

#else /* USE_3D_DEVICE_WRAPPER */

// Use this type rather than IDirect3DDevice9:
typedef IDirect3DDevice9	IDirect3DDevice9Wrapped, *PDIRECT3DDEVICE9WRAPPED, *LPDIRECT3DDEVICE9WRAPPED;

#define RAWDEVICE(X)	(X)
#define FLUSH(X)		/**/

#endif /* USE_3D_DEVICE_WRAPPER */



/**********************************************************************************************
***********************************************************************************************
************************************** D3DTPRINT3D.H ******************************************
***********************************************************************************************
***********************************************************************************************/

/*******************************************************************************
 * Function Name   : D3DTPrint3DSetTextures							 
 * Inputs          : lpDev8, dwScreenX, dwScreenY                                                    
 * Returns         : TRUE or FALSE
 * Globals Used    :                                                   
 * Description     : Initialization and texture upload. Should be called only once
 *					 for a given D3D and DirectDraw device.
 *
 *******************************************************************************/
BOOL D3DTPrint3DSetTextures(LPDIRECT3DDEVICE9WRAPPED lpDev, DWORD dwScreenX, DWORD dwScreenY);

/*******************************************************************************
 * Function Name   : D3DTPrint3DReleaseTextures						 
 * Inputs          : None                                                    
 * Globals Used    : 								
 * Description     : Deallocate the memory allocated in D3DTPrint3DSetTextures(...) 
 *
 *******************************************************************************/
void  D3DTPrint3DReleaseTextures(void);


/*******************************************************************************
 * Function Name   : D3DTPrint3D
 * Inputs          : fPosX, fPosY, fScale, Colour, *Format, ...
 * Globals Used    : 
 * Description     : Display 3D text on screen.
 *					 No window needs to be allocated to use this function.
 *					 However, D3DTPrint3DSetTextures(...) must have been called
 *					 beforehand.
 *					 This function accepts formatting in the printf way.
 *
 *******************************************************************************/
void D3DTPrint3D(float fPosX, float fPosY, float fScale,  int Colour, const TCHAR *Format, ...);


/*******************************************************************************
 * Function Name   : D3DTPrint3DCreateDefaultWindow						 
 * Inputs          : fPosX, fPosY, nXSize_LettersPerLine, *sBody
 * Returns		   : Window handle
 * Globals Used    : 
 * Description     : Creates a default window.
 *					 If Title is NULL the main body will have just one line 
 *					 (for InfoWin).
 *
 *******************************************************************************/
DWORD D3DTPrint3DCreateDefaultWindow(float fPosX, float fPosY, int nXSize_LettersPerLine, TCHAR *sTitle, TCHAR *sBody);


/*******************************************************************************
 * Function Name   : D3DTPrint3DCreateWindow						 
 * Inputs          : dwBufferSizeX, dwBufferSizeY
 * Returns		   : Window handle
 * Globals Used    : 
 * Description     : Allocate a buffer for a newly-created window and return its
 *					 handle.
 *
 *******************************************************************************/
DWORD D3DTPrint3DCreateWindow(DWORD dwBufferSizeX, DWORD dwBufferSizeY);

/*******************************************************************************
 * Function Name	: D3DTPrint3DDeleteAllWindows
 * Inputs			: None
 * Globals Used	:
 * Description		: Delete all windows.
 *
 *******************************************************************************/
void D3DTPrint3DDeleteAllWindows(void);

/*******************************************************************************
 * Function Name	: D3DTPrint3DDeleteWindow
 * Inputs			: dwWin
 * Globals Used	:
 * Description		: Delete the window referenced by dwWin.
 *
 *******************************************************************************/
void D3DTPrint3DDeleteWindow(DWORD dwWin);

/*******************************************************************************
 * Function Name	: D3DTPrint3DDisplayWindow
 * Inputs			: dwWin
 * Globals Used		:
 * Description		: Display window.
 *					  This function MUST be called between a BeginScene/EndScene
 *					  pair as it uses D3D render primitive calls.
 *					  D3DTPrint3DSetTextures(...) must have been called beforehand.
 *					
 *******************************************************************************/
void D3DTPrint3DDisplayWindow(DWORD dwWin);

/*******************************************************************************
 * Function Name	: D3DTPrint3DDisplayAllWindows
 * Inputs			: None
 * Globals Used		:
 * Description		: Display all windows.
 *					  This function MUST be called between a BeginScene/EndScene
 *					  pair as it uses D3D render primitive calls.
 *					  D3DTPrint3DSetTextures(...) must have been called beforehand.
 *					
 *******************************************************************************/
void D3DTPrint3DDisplayAllWindows(void);

/*******************************************************************************
 * Function Name	: D3DTPrint3DWindowOutput
 * Inputs			: dwWin, *Format, ...
 * Globals Used		:
 * Description		: Feed the text buffer of window referenced by dwWin.
 *					  This function accepts formatting in the printf way.
 *					
 *******************************************************************************/
void D3DTPrint3DWindowOutput(DWORD dwWin, const TCHAR *Format, ...);

/*******************************************************************************
 * Function Name	: D3DTPrint3DClearWindowBuffer
 * Inputs			: dwWin
 * Globals Used		:
 * Description		: Clear the text buffer associated with window dwWin.
 *
 *******************************************************************************/
void D3DTPrint3DClearWindowBuffer(DWORD dwWin);

/*******************************************************************************
 * Function Name	: D3DTPrint3DSetWindow
 * Inputs			: dwWin, dwBackgroundColor, dwFontColor, fFontSize,
 *					  fPosX, fPosY, fSizeX, fSizeY.
 * Globals Used		:
 * Description		: Set attributes of window.
 *					  Windows position and size are referred to a virtual screen 
 *					  of 100x100. (0,0) is the top-left corner and (100,100) the 
 *					  bottom-right corner.
 *					  These values are the same for all resolutions.
 *					
 *******************************************************************************/
void  D3DTPrint3DSetWindow(DWORD dwWin, DWORD dwBackgroundColor, DWORD dwFontColor, float fFontSize, 
						   float fPosX, float fPosY, float fSizeX, float fSizeY);

/*******************************************************************************
 * Function Name	: D3DTPrint3DSetTitle
 * Inputs			: dwWin, dwBackgroundColor, fFontSize,
 *					  dwFontColorLeft, *sTitleLeft,
 *					  dwFontColorRight, *sTitleRight,
 * Globals Used		:
 * Description		: Set window title.
 *					
 *******************************************************************************/
void D3DTPrint3DSetTitle(DWORD dwWin, DWORD dwBackgroundColor, float fFontSize,
						 DWORD dwFontColorLeft, char *sTitleLeft,
						 DWORD dwFontColorRigth, char *sTitleRight);

/*******************************************************************************
 * Function Name	: D3DTPrint3DSetWindowFlags
 * Inputs			: dwWin, dwFlags
 * Globals Used		:
 * Description		: Set flags for window referenced by dwWin.
 *					  A list of flag can be found at the top of this header.
 *
 *******************************************************************************/
void D3DTPrint3DSetWindowFlags(DWORD dwWin, DWORD dwFlags);

/*******************************************************************************
 * Function Name	: D3DTPrint3DSetWindowBufferPointer
 * Inputs			: dwWin, dwPosX, dwPosY
 * Globals Used		:
 * Description		: Set the cursor position for the next D3DTPrint3DWindowOutput
 *					  call.
 *					
 *******************************************************************************/
void D3DTPrint3DSetWindowBufferPointer(DWORD dwWin, DWORD dwPosX, DWORD dwPosY);

/*******************************************************************************
 * Function Name	: D3DTPrint3DAdjustWindowSize
 * Inputs			: dwWin, dwMode
 * Globals Used		:
 * Description		: Calculates window size so that all text fits in the window.
 *					  (dwMode 0 = Both, dwMode 1 = X only,  dwMode 2 = Y only) 
 *					
 *******************************************************************************/
void D3DTPrint3DAdjustWindowSize(DWORD dwWin, DWORD dwMode);

/*******************************************************************************
 * Function Name	: D3DTPrint3DSetTopMostWindow
 * Inputs			: dwWin
 * Globals Used		:
 * Description		: Set top-most window. This window will appear on top of all
 *					  others.
 *					
 *******************************************************************************/
void D3DTPrint3DSetTopMostWindow(DWORD dwWin);

/*******************************************************************************
 * Function Name	: D3DTPrint3DSetWindowPos
 * Inputs			: dwWin, fPosX, fPosY
 * Globals Used		:
 * Description		: Set position of window.
 *					  Windows position and size are referred to a virtual screen 
 *					  of 100x100, (0,0) is the top-left corner and (100,100) the 
 *					  bottom-right corner.
 *					  These values are the same for all resolutions.
 *					
 *******************************************************************************/
void D3DTPrint3DSetWindowPos(DWORD dwWin, float fPosX, float fPosY);

/*******************************************************************************
 * Function Name	: D3DTPrint3DSetWindowSize
 * Inputs			: dwWin, fSizeX, fSizeY
 * Globals Used		:
 * Description		: Set size of window.
 *					  Windows position and size are referred to a virtual screen 
 *					  of 100x100, (0,0) is the top-left corner and (100,100) the 
 *					  bottom-right corner.
 *					  These values are the same for all resolutions.
 *					
 *******************************************************************************/
void D3DTPrint3DSetWindowSize(DWORD dwWin, float fSizeX, float fSizeY);


/*******************************************************************************
 * Function Name	: D3DTPrint3DGetLength
 * Inputs			: fFontSize, *sString
 * Globals Used		:
 * Description		: Return font lenght (used for D3DShell menus)
 *					
 *******************************************************************************/
float D3DTPrint3DGetLength (float fFontSize, BYTE *sString);

/*******************************************************************************
 *  Function Name   : D3DTPrint3DGetAspectRatio							 
 *  Inputs          :                                                     
 *  Outputs         :                                                   
 *  Returns         :                                        
 *  Globals Used    :                                                   
 *  Description     : Gets the aspect ratio based on the resolution.
 *******************************************************************************/
void D3DTPrint3DGetAspectRatio(DWORD *dwScreenX, DWORD *dwScreenY);


/**********************************************************************************************
***********************************************************************************************
************************************** D3DTMODEL3DS.H *****************************************
***********************************************************************************************
***********************************************************************************************/

/* Mesh structure */
typedef struct MeshTypeTAG {
	char		pszName[20];		/* Name of mesh */
    char		pszMaterial[20];	/* Name of material used for this mesh */
	int			nMaterialNumber;	/* Index of material used in this mesh */
	float		fCentre[3];			/* Mesh centre */
	float		fMinimum[3];		/* Bounding box's lower front corner */
	float		fMaximum[3];		/* Bounding box's upper back corner */
	int			nNumVertex;			/* Number of vertices in the mesh */
	int			nNumFaces;			/* Number of triangles in the mesh */
	float		*pVertex;			/* List of vertices (x0, y0, z0, x1, y1, z1, x2, etc...) */
	WORD		*pFaces;			/* List of triangles indices */
	float		*pNormals;			/* List of vertex normals (Nx0, Ny0, Nz0, Nx1, Ny1, Nz1, Nx2, etc...) */
	float		*pUV;				/* List of UV coordinate (u0, v0, u1, v1, u2, etc...) */
} MeshType;

/* Material structure */
typedef struct MaterialTypeTAG {
       char		pszMatName[256];	/* Material name */
       char		pszMatFile[256];	/* Material file name (used if textured) */
	   char		pszMatOpaq[256];	/* ? */
	   int		nMatOpacity;		/* Material opacity (used with vertex alpha ?) */
       float	fMatAmbient[3];		/* Ambient RGB value for material */
       float	fMatDiffuse[3];		/* Diffuse RGB value for material */
       float	fMatSpecular[3];	/* Specular RGB value for material */
       float	fMatShininess;		/* Material shininess */
       float	fMatTransparency;	/* Material transparency */
       short	sMatShading;		/* Shading mode used with this material */
} MaterialType;

/* Light structure */
typedef struct LightTypeTAG {
		float	fPosition[3];		/* Light position in World coordinates */
		float	fColour[3];			/* Light colour (0.0f -> 1.0f for each channel) */
} LightType;

/* Camera structure */
typedef struct CameraTypeTAG {
		char	pszName[20];		/* Name of camera */
		float	fPosition[3];		/* Camera position */
		float	fTarget[3];			/* Camera looking point */
		float	fRoll;				/* Camera roll value */
		float	fFocalLength;		/* Camera focal length, in millimeters */
		float	fFOV;				/* Field of view */
} CameraType;

/* Keyframe position structure */
typedef struct _POSITIONKEY {
	
	int			FrameNumber;
	float		fTension;
	float		fContinuity;
	float		fBias;
	D3DXVECTOR3	p;
} POSITIONKEY;

/* Keyframe rotation structure */
typedef struct _ROTATIONKEY {
	
	int			FrameNumber;
	float		fTension;
	float		fContinuity;
	float		fBias;
	float		Angle;
	D3DXVECTOR3	r;
} ROTATIONKEY;

/* Keyframe scaling structure */
typedef struct _SCALINGKEY {
	
	int			FrameNumber;
	float		fTension;
	float		fContinuity;
	float		fBias;
	D3DXVECTOR3	s;
} SCALINGKEY;

/* Node structure */
typedef struct NodeTypeTAG {
	/* Parent node Id */
	short		ParentIndex;

	/* Mesh corresponding to this Node Id */
	short		Mesh;

	/* Mesh corresponding to this Node Id */
	short		Camera;
	short		Target;

	/* Pivot for this mesh */
	float		Pivot[3];

	/* Position */
	int			PositionKeys;
	POSITIONKEY	*pPosition;
	
	/* Rotation */
	int			RotationKeys;
	ROTATIONKEY	*pRotation;

	/* Scaling */
	int			ScalingKeys;
	SCALINGKEY	*pScaling;
} NodeType;

	
/*********************
** Object structure **
*********************/
typedef struct ObjectTypeTAG {
	
	/* Information related to all meshes */
	int				nTotalVertices;		/* Total number of vertices in object */
	int				nTotalFaces;		/* Total number of faces in object */
	float			fGroupCentre[3];	/* Centre of object */
	float			fGroupMinimum[3];	/* Bounding box's lower front corner */
	float			fGroupMaximum[3];	/* Bounding box's upper back corner */

	/* Meshes defined in the .3DS file */
	int				nNumMeshes;			/* Number of meshes composing the object */
	MeshType		*pMesh;				/* List of meshes in object */

	/* Materials defined in the .3DS file */
	int				nNumMaterials;		/* Number of materials used with object */
	MaterialType	*pMaterial;			/* List of materials used with object */

	/* Lights defined in the .3DS file */
	int				nNumLights;			/* Number of lights */
	LightType		*pLight;			/* List of lights */
	
	/* Cameras defined in the .3DS file */
	int				nNumCameras;		/* Number of cameras */
	CameraType		*pCamera;			/* List of cameras */

	/* This is used for animation only. Ignore if animation is not to be used */
	int				nNumFrames;			/* Number of frames */
	int				nNumNodes;			/* Number of nodes */
	NodeType		*pNode;				/* List of nodes */
} ObjectType;


/*******************************************************************************
 * Function Name  : D3DTModel3DSRead
 * Returns        : TRUE or FALSE
 * Input		  : *Name3DSFile
 * Output		  : *pObject
 * Global Used    : None
 * Description    : Read a 3DS file and create an object structure from it.
 *					After use, the object structure must be destroyed with a 
 *					call to D3DTModel3DSDestroy().
 *					The 3DS file can either be a resource or file.
 *					If stored in a resource, the resource identifier must be "TDS".
 *******************************************************************************/
BOOL D3DTModel3DSRead(ObjectType *pObject, char *Name3DSFile);

/*******************************************************************************
 * Function Name  : D3DTModel3DSDestroy
 * Returns        : TRUE or FALSE
 * Input/Output	  : *pObject
 * Global Used    : SubObject[][]
 * Description    : Destroy an object allocated with D3DTModel3DSRead()
 *******************************************************************************/
void D3DTModel3DSDestroy(ObjectType *pObject);

/*******************************************************************************
 * Function Name  : D3DTModel3DSDisplayInfo
 * Returns        : Nothing
 * Input/Output	  : *pObject
 * Global Used    : None
 * Description    : Display model data into debug output
 *******************************************************************************/
void D3DTModel3DSDisplayInfo(ObjectType *pObject);

/*******************************************************************************
 * Function Name  : D3DTModel3DSScale
 * Returns        : Nothing
 * Input/Output	  : *pObject, fScale
 * Global Used    : None
 * Description    : Scale a model's vertices with a uniform scaling value
 *******************************************************************************/
void D3DTModel3DSScale(ObjectType *pObject, const float Scale);

/*******************************************************************************
 * Function Name  : D3DTModel3DSGetAnimationMatrix
 * Returns        : TRUE or FALSE
 * Inputs		  : *pObject, nMesh, fFrameNumber
 * Output		  : *pAnimationMatrix
 * Global Used    : None
 * Description    : Return animation matrix required to animate this mesh at the 
 *					specified frame.
 *					Frame number is a floating point number as (linear) 
 *					interpolation will be used to compute the required matrix at
 *					this frame.
 *******************************************************************************/
BOOL D3DTModel3DSGetAnimationMatrix(ObjectType *pObject, short nMesh, float fFrameNumber, D3DXMATRIX *pAnimationMatrix);

/*******************************************************************************
 * Function Name  : D3DTModel3DSGetCameraAnimation
 * Returns        : TRUE or FALSE
 * Inputs		  : *pObject, nCamera, fFrameNumber
 * Output		  : *pPosition, *pTarget
 * Global Used    : None
 * Description    : Return position and target required to animate this camera at the 
 *					specified frame.
 *					Frame number is a floating point number as (linear) 
 *					interpolation will be used to compute the required matrix at
 *					this frame.
 *******************************************************************************/
BOOL D3DTModel3DSGetCameraAnimation(ObjectType *pObject, short nCamera, float fFrameNumber,	D3DXVECTOR3 *pPosition, D3DXVECTOR3 *pTarget);


/**********************************************************************************************
***********************************************************************************************
************************************** D3DTMOUSE.H ********************************************
***********************************************************************************************
***********************************************************************************************/

/****************************************************************************
 * Function Name  : D3DTMouseInit
 * Inputs		  : hInstance, hWindow
 * Returns		  : TRUE or FALSE
 * Description    :	Initialises the mouse functions.
 *					The application instance handle and the window handle to 
 *					which the mouse is attached must be passed to the function.
 ***************************************************************************/
BOOL D3DTMouseInit(
	const HINSTANCE hInstance,
	const HWND		hWindow);

/****************************************************************************
 * Function Name  : D3DTMouseShutdown
 * Inputs		  : None
 * Description    :	Shuts down DirectInput.
 *					Do not use any functions, other than D3DTMouseInit(),
 *					after calling this.
 ***************************************************************************/
void D3DTMouseShutdown();

/****************************************************************************
 * Function Name  : D3DTMouseQuery
 * Inputs		  : None
 * Outputs		  : *pnX, *pnY, *pnButtons
 * Returns		  : TRUE or FALSE
 * Description    :	Query the mouse status.
 *					pnX			Will contain the relative motion of the mouse 
 *								X axis since this function was last called.
 *					pnY			Will contain the relative motion of the mouse 
 *								Y axis since this function was last called.
 *					pnButtons	Bit0	1 = Left button been pressed
 *								Bit1	1 = Right button been pressed
 *								Bit2	1 = Middle button been pressed
 *
 ***************************************************************************/
BOOL D3DTMouseQuery(
	int * const		pnX,
	int * const		pnY,
	int * const		pnButtons);


/**********************************************************************************************
***********************************************************************************************
************************************** D3DTVERTEX.H *******************************************
***********************************************************************************************
***********************************************************************************************/

/****************************************************************************
 * Function Name  : D3DTVertexReadElement
 * Inputs		  : pVtx, pVertexElements, Usage, UsageIndex
 * Outputs		  : pfOut
 * Returns		  :
 * Description    :	Reads the vertex element specified by Usage and
 *					UsageIndex, from the vertex structure pointed to by
 *					pVtx, which is described by pVertexElements.
 *					Up to 4 values will be placed in the array pfOut.
 ***************************************************************************/
void D3DTVertexReadElement(
	float					* const pfOut,
	const char				* const pVtx,
	const D3DVERTEXELEMENT9	* const pVertexElements,
	const BYTE				Usage,
	const BYTE				UsageIndex);

/****************************************************************************
 * Function Name  : D3DTVertexWriteElement
 * Inputs		  : pfIn, pVtx, pVertexElements, Usage, UsageIndex
 * Outputs		  :
 * Returns		  :
 * Description    :	Writes up to 4 values from the array pfIn, to the vertex
 *					element specified by Usage and UsageIndex, in the
 *					vertex structure pointed to by pVtx, which is described
 *					by pVertexElements.
 ***************************************************************************/
void D3DTVertexWriteElement(
	const float				* const pfIn,
	const char				* const pVtx,
	const D3DVERTEXELEMENT9	* const pVertexElements,
	const BYTE				Usage,
	const BYTE				UsageIndex);

/****************************************************************************
 * Function Name  : D3DTVertexTangentBinormal
 * Inputs		  : pvNor, pfPosA, pfPosB, pfPosC, pfTexA, pfTexB, pfTexC
 * Outputs		  : pvTan, pvBin
 * Returns		  :
 * Description    :	Calculates the tangent and binormal vectors for
 *					vertex 'A' of the triangle defined by the 3 supplied
 *					3D position coordinates (pfPosX) and 2D texture
 *					coordinates (pfTexX).
 ***************************************************************************/
void D3DTVertexTangentBinormal(
	D3DVECTOR		* const pvTan,
	D3DVECTOR		* const pvBin,
	const D3DVECTOR	* const pvNor,
	const float		* const pfPosA,
	const float		* const pfPosB,
	const float		* const pfPosC,
	const float		* const pfTexA,
	const float		* const pfTexB,
	const float		* const pfTexC);

/****************************************************************************
 * Function Name  : D3DTVertexGenerateTangentSpace
 * Inputs		  : pwIdx, nVtxNum, pVtx, pVertexElements, nStride, TexStageIndex, TangentIndex, BinormIndex, nTriNum, fSplitDifference
 * Outputs		  : pnVtxNumOut, pVtxOut, pwIdx
 * Returns		  :
 * Description    :	Calculates the tangent space for all supplied vertices.
 *					Writes tangent and binormal vectors to the output
 *					vertices, copies all other elements from input vertices.
 *					Will split vertices if necessary - i.e. if two triangles
 *					sharing a vertex want to assign it different
 *					tangent-space matrices. The decision whether to split
 *					uses fSplitDifference - of the DP3 of two desired
 *					tangents or two desired binormals is higher than this,
 *					the vertex will be split.
 ***************************************************************************/
BOOL D3DTVertexGenerateTangentSpace(
	int						* const pnVtxNumOut,	// Output vertex count
	char					** const pVtxOut,		// Output vertices (program must free() this)
	WORD					* const pwIdx,			// input AND output; index array for triangle list
	const int				nVtxNum,				// Input vertex count
	const char				* const pVtx,			// Input vertices
	const D3DVERTEXELEMENT9	* const pVertexElements,// Input vertex definition
	const int				nStride,				// Size of a vertex (in bytes)
	const char				TexStageIndex,			// UsageIndex of texture stage to use (from vertex element definition)
	const char				TangentIndex,			// UsageIndex of tangent vector to write (from vertex element definition)
	const char				BinormIndex,			// UsageIndex of binormal vector to write (from vertex element definition)
	const int				nTriNum,				// Number of triangles
	const float				fSplitDifference);		// Split a vertex if the DP3 of tangents/binormals are below this (range -1..1)


#endif

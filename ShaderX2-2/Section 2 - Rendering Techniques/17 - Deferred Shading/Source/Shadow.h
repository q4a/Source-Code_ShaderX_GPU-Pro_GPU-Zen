/****************************************************************************
 * Name : "Shadow.h"
 * Date : October 2001
 *
 * $Revision: 1.1 $
 *
 * Declarations of functions relating to shadow volume generation.
 *
 * Copyright 2001 by Imagination Technologies Limited. All rights reserved.
 ***************************************************************************/


/****************************************************************************
** Structures
****************************************************************************/
typedef struct {
	WORD		wV0, wV1;		/* Indices of the vertices of the edge */
	int			nVis;			/* Bit0 = Visible, Bit1 = Hidden, Bit2 = Reverse Winding */
} SMEdge;


typedef struct {
	SMEdge		*pE0, *pE1, *pE2;	/* Edges of the triangle */
	D3DVECTOR	vNormal;			/* Triangle normal */
	int			nWinding;			/* BitN = Correct winding for edge N */
} SMTriangle;


typedef struct {
	D3DVECTOR	*pVo;		/* Unique vertices in object space */
	D3DVECTOR	*pVw;		/* World-space verts; transform pVo into this before building volumes */
	SMEdge		*pE;
	SMTriangle	*pT;
	unsigned int	nV;		/* Vertex count */
	unsigned int	nE;		/* Edge count */
	unsigned int	nT;		/* Triangle count */
} SShadowMesh;


/*
	Renderable shadow-volume information:
*/
typedef struct {
	D3DVECTOR		*pvStrip;			/* Vertices for strip */
	unsigned int	*pnStripVtxCnt;		/* Length of the strips */
	unsigned int	nStripCnt;			/* Number of strips */
	unsigned int	nStripVtxTot;		/* Total number of vertices in the strips */

	IDirect3DVertexBuffer9	*pivb;		/* Copy pvStrip into this before the render */
	unsigned int	nVBLen;			/* Number of vertices which can fit in pivb */

#ifdef _DEBUG
	unsigned int	nStripCntMax;	/* Highest number of strips used in volume since creation */
	unsigned int	nStripVtxTotMax;	/* Highest number of strips used in volume since creation */
	unsigned int	nSharedVert;	/* Maximum number of times any one vertex is used in the volume */
	unsigned int	nSharedVertMax;	/* Highest value of nSharedVert since creation */
#endif
} SShadowVol;


/****************************************************************************
** Declarations
****************************************************************************/
void ShadowMeshCreate(
	const float				* const pVertex,
	const unsigned int		nNumVertex,
	const unsigned short	* const pFaces,
	const unsigned int		nNumFaces,
	SShadowMesh				* const psMesh,
	SShadowVol				* const psVol,
	const unsigned int		nNumVol);
void ShadowMeshCreateComplete();

BOOL ShadowMeshInit(
	SShadowMesh			* const psMesh,
	SShadowVol			* const psVol,
	const unsigned int	nNumVol,
	LPDIRECT3DDEVICE9	lpDev9);

void ShadowMeshDestroy(
	SShadowMesh			* const psMesh,
	SShadowVol			* const psVol,
	const unsigned int	nNumVol);
void ShadowMeshDestroyComplete();

void ShadowSilhouetteProjectedBuild(
	const SShadowMesh	* const psMesh,
	SShadowVol			* const psVol,
	const D3DVECTOR		* const pvLightWorld,
	const D3DVECTOR		* const pvLightModel);

void ShadowSilhouetteProjectedRender(
	const SShadowVol	* const psVol,
	LPDIRECT3DDEVICE9	lpDev9);

void ShadowMeshRelease(
	SShadowVol			* const psVol,
	const unsigned int	nNumVol);

extern int			nNumberOfShadowVolumeStrips;

/*****************************************************************************
 End of file (Shadow.h)
*****************************************************************************/

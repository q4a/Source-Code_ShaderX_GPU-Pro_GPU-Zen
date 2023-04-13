/****************************************************************************
 * Name : "Shadow.cpp"
 * Date : October 2001
 *
 * $Revision: 1.1 $
 *
 * Functions relating to shadow volume generation.
 *
 * Copyright 2001 by Imagination Technologies Ltd. All rights reserved.
 * Information and source code samples contained herein are
 * provided "as-is", without representations or warranties, and
 * are subject to change without notice. The author cannot support
 * modifications or derivative works created from the sample source
 * code provided. You may use, reproduce, and modify portions or
 * entire sections of the sample source code for the purposes of
 * creating applications. Distribution is limited to executable
 * or binary portions of the sample source code unless you gain
 * written permission from the author.
 ***************************************************************************/
#include <d3d9.h>
#include <math.h>
#include <crtdbg.h>

#include "Shadow.h"


/****************************************************************************
** Defines
****************************************************************************/
#define MAX_SHARED_VERT			(4)
#define MAX_STRIPS_PER_SHADOW	(50)


/****************************************************************************
** Macros
****************************************************************************/
#define FREE(X) { if(X) { free(X); (X) = 0; } }

#ifndef RELEASE
#define RELEASE(x)	if (x) { IDirect3D9_Release(x); (x)=NULL; }
#endif

/****************************************************************************
** Structures
****************************************************************************/
struct SSilhouette {
	WORD pwNext[MAX_SHARED_VERT];
	WORD wCount;
};

struct SVertexVol {
	float sx, sy, sz, rhw;
};

/****************************************************************************
** Globals
****************************************************************************/
static DWORD		g_dwMaxVertexCount = 0;
static SSilhouette	*g_psStrip = NULL;

BOOL				g_bVertBgn;	/* Cap exit point set? */
static SVertexVol	g_sVertBgn;	/* First cap exit point */

/****************************************************************************
** Code
****************************************************************************/

static WORD FindOrCreateVertex(
	SShadowMesh		* const psMesh,
	const D3DVECTOR	* const pV)
{
	WORD	wCurr;

	/*
		First check whether we already have a vertex here
	*/
	for(wCurr = 0; wCurr < psMesh->nV; wCurr++) {
		if(memcmp(&psMesh->pVo[wCurr], pV, sizeof(*pV)) == 0) {
			/* Don't do anything more if the vertex already exists */
			return wCurr;
		}
	}

	/*
		Add the vertex then!
	*/
	psMesh->pVo[psMesh->nV] = *pV;

	return psMesh->nV++;
}

static SMEdge *FindOrCreateEdge(
	SShadowMesh		* const psMesh,
	const D3DVECTOR * const pv0,
	const D3DVECTOR * const pv1)
{
	unsigned int	nCurr;
	WORD			wV0, wV1;

	wV0 = FindOrCreateVertex(psMesh, pv0);
	wV1 = FindOrCreateVertex(psMesh, pv1);

	/*
		First check whether we already have a edge here
	*/
	for(nCurr = 0; nCurr < psMesh->nE; nCurr++) {
		if(
			(psMesh->pE[nCurr].wV0 == wV0 && psMesh->pE[nCurr].wV1 == wV1) ||
			(psMesh->pE[nCurr].wV0 == wV1 && psMesh->pE[nCurr].wV1 == wV0))
		{
			/* Don't do anything more if the edge already exists */
			return &psMesh->pE[nCurr];
		}
	}

	/*
		Add the edge then!
	*/
	psMesh->pE[psMesh->nE].wV0	= wV0;
	psMesh->pE[psMesh->nE].wV1	= wV1;
	psMesh->pE[psMesh->nE].nVis	= 0;

	return &psMesh->pE[psMesh->nE++];
}

static void CrossProduct(
	const D3DVECTOR * const pv0,
	const D3DVECTOR * const pv1,
	const D3DVECTOR * const pv2,
	D3DVECTOR		* const pvOut)
{
	float fX0, fY0, fZ0;
	float fX1, fY1, fZ1;
	float fXr, fYr, fZr;
	float fNormalise;

	fX0 = pv1->x - pv0->x;
	fY0 = pv1->y - pv0->y;
	fZ0 = pv1->z - pv0->z;

	fX1 = pv2->x - pv0->x;
	fY1 = pv2->y - pv0->y;
	fZ1 = pv2->z - pv0->z;

	/* Calculate the cross product... */
    fXr = fY0 * fZ1 - fZ0 * fY1;
    fYr = fZ0 * fX1 - fX0 * fZ1;
    fZr = fX0 * fY1 - fY0 * fX1;

	/* ...normalise it... */
	fNormalise = 1.0f / (float)sqrt(fXr*fXr + fYr*fYr + fZr*fZr);
	fXr *= fNormalise;
	fYr *= fNormalise;
	fZr *= fNormalise;

	/* ...and return it. */
    pvOut->x = fXr;
    pvOut->y = fYr;
    pvOut->z = fZr;
}

static void FindOrCreateTriangle(
	SShadowMesh		* const psMesh,
	const D3DVECTOR	* const pv0,
	const D3DVECTOR	* const pv1,
	const D3DVECTOR	* const pv2)
{
	unsigned int	nCurr;
	SMEdge	*psE0, *psE1, *psE2;

	psE0 = FindOrCreateEdge(psMesh, pv0, pv1);
	psE1 = FindOrCreateEdge(psMesh, pv1, pv2);
	psE2 = FindOrCreateEdge(psMesh, pv2, pv0);
	_ASSERT(psE0);
	_ASSERT(psE1);
	_ASSERT(psE2);

	if(psE0 == psE1 || psE1 == psE2 || psE2 == psE0) {
		/* Don't add degenerate triangles */
		_RPT0(_CRT_WARN, "FindOrCreateTriangle() Degenerate triangle.\n");
		return;
	}

	/*
		First check whether we already have a triangle here
	*/
	for(nCurr = 0; nCurr < psMesh->nT; nCurr++) {
		if(
			(psMesh->pT[nCurr].pE0 == psE0 || psMesh->pT[nCurr].pE0 == psE1 || psMesh->pT[nCurr].pE0 == psE2) &&
			(psMesh->pT[nCurr].pE1 == psE0 || psMesh->pT[nCurr].pE1 == psE1 || psMesh->pT[nCurr].pE1 == psE2) &&
			(psMesh->pT[nCurr].pE2 == psE0 || psMesh->pT[nCurr].pE2 == psE1 || psMesh->pT[nCurr].pE2 == psE2))
		{
			/* Don't do anything more if the triangle already exists */
			return;
		}
	}

	/*
		Add the triangle then!
	*/
	psMesh->pT[psMesh->nT].pE0 = psE0;
	psMesh->pT[psMesh->nT].pE1 = psE1;
	psMesh->pT[psMesh->nT].pE2 = psE2;

	/* Calculate the triangle normal */
	CrossProduct(pv0, pv1, pv2, &psMesh->pT[psMesh->nT].vNormal);

	/* Check which edges have the correct winding order for this triangle */
	psMesh->pT[psMesh->nT].nWinding = 0;
	if(memcmp(&psMesh->pVo[psE0->wV0], pv0, sizeof(*pv0)) == 0) psMesh->pT[psMesh->nT].nWinding |= 0x01;
	if(memcmp(&psMesh->pVo[psE1->wV0], pv1, sizeof(*pv1)) == 0) psMesh->pT[psMesh->nT].nWinding |= 0x02;
	if(memcmp(&psMesh->pVo[psE2->wV0], pv2, sizeof(*pv2)) == 0) psMesh->pT[psMesh->nT].nWinding |= 0x04;

	psMesh->nT++;
}

void ShadowMeshCreate(
	const float				* const pVertex,
	const unsigned int		nNumVertex,
	const unsigned short	* const pFaces,
	const unsigned int		nNumFaces,
	SShadowMesh				* const psMesh,
	SShadowVol				* const psVol,
	const unsigned int		nNumVol)
{
	unsigned int	nCurr;

	/*
		Make sure we haven't finalised our render structures
	*/
	_ASSERT(g_psStrip == NULL);

	/*
		Prep the structure to return
	*/
	memset(psMesh, 0, sizeof(*psMesh));

	/*
		Allocate some working space to find the unique vertices
	*/
	psMesh->pVo = (D3DVECTOR*)malloc(nNumVertex * sizeof(*psMesh->pVo));
	psMesh->pE = (SMEdge*)malloc(nNumFaces * sizeof(*psMesh->pE) * 3);
	psMesh->pT = (SMTriangle*)malloc(nNumFaces * sizeof(*psMesh->pT));
	_ASSERT(psMesh->pVo);
	_ASSERT(psMesh->pE);
	_ASSERT(psMesh->pT);

	for(nCurr = 0; nCurr < nNumFaces; nCurr++) {
		FindOrCreateTriangle(psMesh,
			(D3DVECTOR*)&pVertex[3 * pFaces[3 * nCurr + 0]],
			(D3DVECTOR*)&pVertex[3 * pFaces[3 * nCurr + 1]],
			(D3DVECTOR*)&pVertex[3 * pFaces[3 * nCurr + 2]]);
	}

	_ASSERT(psMesh->nV <= nNumVertex);
	_ASSERT(psMesh->nE < nNumFaces * 3);
	_ASSERT(psMesh->nT == nNumFaces);

	_RPT2(_CRT_WARN, "Unique vertices : %d (from %d)\n", psMesh->nV, nNumVertex);
	_RPT2(_CRT_WARN, "Unique edges    : %d (from %d)\n", psMesh->nE, nNumFaces * 3);
	_RPT2(_CRT_WARN, "Unique triangles: %d (from %d)\n", psMesh->nT, nNumFaces);

	/*
		Create the real unique lists
	*/
	psMesh->pVo = (D3DVECTOR*)realloc(psMesh->pVo, psMesh->nV * sizeof(*psMesh->pVo));
	psMesh->pVw = (D3DVECTOR*)malloc(psMesh->nV * sizeof(*psMesh->pVw));
	psMesh->pE = (SMEdge*)realloc(psMesh->pE, psMesh->nE * sizeof(*psMesh->pE));
	psMesh->pT = (SMTriangle*)realloc(psMesh->pT, psMesh->nT * sizeof(*psMesh->pT));
	_ASSERT(psMesh->pVo);
	_ASSERT(psMesh->pVw);
	_ASSERT(psMesh->pE);
	_ASSERT(psMesh->pT);

#ifdef _DEBUG
	/*
		Check we have sensible model data
	*/
	{
		unsigned int nTri, nEdge;
		OutputDebugString("ShadowMeshCreate() Sanity check...");

		for(nEdge = 0; nEdge < psMesh->nE; nEdge++) {
			nCurr = 0;

			for(nTri = 0; nTri < psMesh->nT; nTri++) {
				if(psMesh->pT[nTri].pE0 == &psMesh->pE[nEdge])
					nCurr++;

				if(psMesh->pT[nTri].pE1 == &psMesh->pE[nEdge])
					nCurr++;

				if(psMesh->pT[nTri].pE2 == &psMesh->pE[nEdge])
					nCurr++;
			}

			/*
				Every edge should be referenced exactly twice
			*/
			_ASSERT(nCurr == 2);
		}

		OutputDebugString("done.\n");
	}
#endif

	/*
		We'll need to know the max vertex count of a model
	*/
	if(psMesh->nV > g_dwMaxVertexCount)
		g_dwMaxVertexCount = psMesh->nV;

	/*
		Allocate memory to store the actual vertices required by the shadow volumes
	*/
	for(nCurr = 0; nCurr < nNumVol; nCurr++) {
		psVol[nCurr].pvStrip = (D3DVECTOR*)calloc(psMesh->nE * 2, sizeof(*psVol[nCurr].pvStrip));
		_ASSERT(psVol[nCurr].pvStrip);

		psVol[nCurr].pnStripVtxCnt = (unsigned int*)calloc(MAX_STRIPS_PER_SHADOW, sizeof(*psVol[nCurr].pnStripVtxCnt));
		_ASSERT(psVol[nCurr].pnStripVtxCnt);
	}
}

void ShadowMeshCreateComplete()
{
	_ASSERT(g_dwMaxVertexCount);

	g_psStrip = (SSilhouette*)calloc(g_dwMaxVertexCount, sizeof(*g_psStrip));
	_ASSERT(g_psStrip);
}


BOOL ShadowMeshInit(
	SShadowMesh			* const psMesh,
	SShadowVol			* const psVol,
	const unsigned int	nNumVol,
	LPDIRECT3DDEVICE9	lpDev9)
{
	unsigned int	nCurr;
	HRESULT			hRes;

	_ASSERT(psMesh);
	_ASSERT(psVol);
	_ASSERT(psMesh->nE);

	/*
		Allocate a vertex buffer for the tranformed/clipped shadow volumes
	*/
	for(nCurr = 0; nCurr < nNumVol; nCurr++) {
#ifdef _DEBUG
		psVol[nCurr].nVBLen		= psMesh->nE * 2;
#endif
		hRes = lpDev9->CreateVertexBuffer(
			psMesh->nE * 2 * sizeof(D3DVECTOR),
			D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			D3DFVF_XYZ, D3DPOOL_DEFAULT, &psVol[nCurr].pivb, NULL);
		_ASSERT(hRes == D3D_OK);
		if(hRes != D3D_OK)
		{
			if (hRes == D3DERR_OUTOFVIDEOMEMORY || hRes == E_OUTOFMEMORY)
			{
				OutputDebugString("Not enough memory to create vertex buffer\n");
			}
			if (hRes == D3DERR_INVALIDCALL)
			{
				OutputDebugString("Invalid call to CreateVertexBuffer\n");
			}

			return FALSE;
		}
	}

	return TRUE;
}

void ShadowMeshDestroy(
	SShadowMesh			* const psMesh,
	SShadowVol			* const psVol,
	const unsigned int	nNumVol)
{
	unsigned int nCurr;

	FREE(psMesh->pVo);
	FREE(psMesh->pVw);
	FREE(psMesh->pE);
	FREE(psMesh->pT);

	for(nCurr = 0; nCurr < nNumVol; nCurr++) {
		FREE(psVol[nCurr].pvStrip);
		FREE(psVol[nCurr].pnStripVtxCnt);
	}
}

void ShadowMeshDestroyComplete()
{
	FREE(g_psStrip);
}

void ShadowSilhouetteProjectedBuildAdd(
	const SShadowMesh	* const psMesh,
	SShadowVol			* const psVol,
	const D3DVECTOR		* const pvFrom,
	WORD				wCurr)
{
	int	nVertexCount;
	D3DVECTOR	*pNew, *pV;
	float		fScale;
	float		fActualLength, fDesiredLength=10000.0f;

	nVertexCount = 0;

	_ASSERT(psVol->nStripCnt < MAX_STRIPS_PER_SHADOW);
	_ASSERT(psVol->nStripVtxTot < (psMesh->nE * 2));
	pNew = &psVol->pvStrip[psVol->nStripVtxTot];

	_ASSERT(wCurr != 0xFFFF);
	_ASSERT(g_psStrip[wCurr].wCount != 0);

	while(TRUE) 
	{
		pV		= &psMesh->pVw[wCurr];

		/*
			Scale vertex out to the ground plane
		*/
//		_ASSERT(pV->y >= 0);
		
//		fScale = pV->y / (pV->y - pvFrom->y);
		fActualLength = (float)sqrt((pV->x-pvFrom->x)*(pV->x-pvFrom->x) + 
									(pV->y-pvFrom->y)*(pV->y-pvFrom->y) + 
									(pV->z-pvFrom->z)*(pV->z-pvFrom->z));
		fScale = fDesiredLength / fActualLength;
//		_ASSERT(fScale <= 0);

		pNew[0].x = pV->x + fScale * (pV->x - pvFrom->x);
		pNew[0].y = pV->y + fScale * (pV->y - pvFrom->y);
		pNew[0].z = pV->z + fScale * (pV->z - pvFrom->z);
		pNew[1] = *pV;

		/*
			Done!
		*/
		nVertexCount	+= 2;
		pNew			+= 2;

		if(g_psStrip[wCurr].wCount == 0)
			break;

		g_psStrip[wCurr].wCount--;
		wCurr	= g_psStrip[wCurr].pwNext[g_psStrip[wCurr].wCount];
	}

	_ASSERT(nVertexCount >= 4);
	_ASSERT((nVertexCount & 0x01) == 0);
	
	_ASSERT(memcmp(&psVol->pvStrip[psVol->nStripVtxTot  ], &psVol->pvStrip[psVol->nStripVtxTot + nVertexCount - 2], sizeof(*pNew)) == 0);
	_ASSERT(memcmp(&psVol->pvStrip[psVol->nStripVtxTot+1], &psVol->pvStrip[psVol->nStripVtxTot + nVertexCount - 1], sizeof(*pNew)) == 0);
	
	psVol->pnStripVtxCnt[psVol->nStripCnt]	= nVertexCount;
	psVol->nStripVtxTot						+= nVertexCount;
	psVol->nStripCnt++;

#ifdef _DEBUG
	if(psVol->nStripVtxTot > psVol->nStripVtxTotMax)
		psVol->nStripVtxTotMax = psVol->nStripVtxTot;

	if(psVol->nStripCnt > psVol->nStripCntMax)
		psVol->nStripCntMax = psVol->nStripCnt;
#endif
}

void ShadowSilhouetteProjectedBuild(
	const SShadowMesh	* const psMesh,
	SShadowVol			* const psVol,
	const D3DVECTOR		* const pvLightWorld,
	const D3DVECTOR		* const pvLightModel)
{
	unsigned int nCurr;
	float		fViewX, fViewY, fViewZ;
	D3DVECTOR	*pvTmp;
	SMTriangle	*psTri;
	SMEdge		*psEdge;
	SSilhouette	*psSil;

	/*
		Ensure sure we have finalised our render structures
	*/
	_ASSERT(g_psStrip);

	psVol->nStripCnt	= 0;
	psVol->nStripVtxTot	= 0;


	/*
		Run through triangles, testing which face the From point
	*/
	for(nCurr = 0; nCurr < psMesh->nT; nCurr++) {
		psTri = &psMesh->pT[nCurr];

		/*
			Calculate the line from the view point to a point on the plane
			No need to normalise the vector, since we only want to know if
			the angle to the plane normal is +ve or -ve
		*/
		pvTmp = &psMesh->pVo[psTri->pE0->wV0];
		fViewX = pvTmp->x - pvLightModel->x;
		fViewY = pvTmp->y - pvLightModel->y;
		fViewZ = pvTmp->z - pvLightModel->z;

		if(fViewX * psTri->vNormal.x + fViewY * psTri->vNormal.y + fViewZ * psTri->vNormal.z >= 0) {
			/* Triangle is in the light */
			psTri->pE0->nVis |= 0x01;
			psTri->pE1->nVis |= 0x01;
			psTri->pE2->nVis |= 0x01;
		} else {
			/* Triangle is in shade; set Bit3 if the winding order needs reversed */
			psTri->pE0->nVis |= 0x02 | (psTri->nWinding & 0x01) << 2;
			psTri->pE1->nVis |= 0x02 | (psTri->nWinding & 0x02) << 1;
			psTri->pE2->nVis |= 0x02 | (psTri->nWinding & 0x04);
		}
	}

	/*
		Run through edges, testing which are silhouette edges
	*/
	for(nCurr = 0; nCurr < psMesh->nE; nCurr++) {
		psEdge = &psMesh->pE[nCurr];

		_ASSERT(psEdge->wV0 < g_dwMaxVertexCount);
		_ASSERT(psEdge->wV1 < g_dwMaxVertexCount);

		if((psEdge->nVis & 0x03) == 0x03) {
			/* Silhouette edge found! */
			if(psEdge->nVis & 0x04) {
				psSil = &g_psStrip[psEdge->wV1];
				_ASSERT(psSil->wCount < MAX_SHARED_VERT);
				psSil->pwNext[psSil->wCount] = psEdge->wV0;
				psSil->wCount++;
			} else {
				psSil = &g_psStrip[psEdge->wV0];
				_ASSERT(psSil->wCount < MAX_SHARED_VERT);
				psSil->pwNext[psSil->wCount] = psEdge->wV1;
				psSil->wCount++;
			}
		}

		/* Zero for next render */
		psEdge->nVis = 0;
	}

#ifdef _DEBUG
	psVol->nSharedVert = 0;
	nCurr = g_dwMaxVertexCount;
	while(nCurr) {
		nCurr--;

		if(nCurr >= psMesh->nV)
			_ASSERT(g_psStrip[nCurr].wCount == 0);

		if(g_psStrip[nCurr].wCount > psVol->nSharedVert)
			psVol->nSharedVert = g_psStrip[nCurr].wCount;
	}

	if(psVol->nSharedVert > psVol->nSharedVertMax)
		psVol->nSharedVertMax = psVol->nSharedVert;
#endif

	/*
		We can reduce the number of strips by starting them at shared vertices
	*/
	nCurr = psMesh->nV;
	while(nCurr) {
		nCurr--;

		while(g_psStrip[nCurr].wCount > 1)
			ShadowSilhouetteProjectedBuildAdd(psMesh, psVol, pvLightWorld, (WORD)nCurr);
	}

	/*
		Add any remaining silhouettes to the shadow volume
	*/
	nCurr = psMesh->nV;
	while(nCurr) {
		nCurr--;

		if(!g_psStrip[nCurr].wCount)
			continue;

		ShadowSilhouetteProjectedBuildAdd(psMesh, psVol, pvLightWorld, (WORD)nCurr);
	}

#ifdef _DEBUG
	nCurr = g_dwMaxVertexCount;
	while(nCurr) {
		nCurr--;

		_ASSERT(g_psStrip[nCurr].wCount == 0);
	}
#endif

	psVol->pivb->Lock(0, 0, (void**)&pvTmp, D3DLOCK_DISCARD);
	memcpy(pvTmp, psVol->pvStrip, psVol->nStripVtxTot * sizeof(D3DVECTOR));
	psVol->pivb->Unlock();
}

void ShadowSilhouetteProjectedRender(
	const SShadowVol	* const psVol,
	LPDIRECT3DDEVICE9	lpDev9)
{
	DWORD dwStrip, dwVtxCnt;

	lpDev9->SetStreamSource(0, psVol->pivb, 0, sizeof(D3DVECTOR));
	dwVtxCnt = 0;

	for(dwStrip = 0; dwStrip < psVol->nStripCnt; dwStrip++) 
	{
		lpDev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, dwVtxCnt, psVol->pnStripVtxCnt[dwStrip] - 2);
		dwVtxCnt += psVol->pnStripVtxCnt[dwStrip];

		//nNumberOfShadowVolumeStrips+=psVol->pnStripVtxCnt[dwStrip];
	}
}


void ShadowMeshRelease(
	SShadowVol			* const psVol,
	const unsigned int	nNumVol)
{
	unsigned int	nCurr;

	for(nCurr = 0; nCurr < nNumVol; nCurr++) {
		RELEASE(psVol[nCurr].pivb);
	}
}




/*****************************************************************************
 End of file (Shadow.cpp)
*****************************************************************************/

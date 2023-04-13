///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgSimpleTree
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSimpleTree.cpp,v 1.3 2002/12/22 10:50:55 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgSimpleTree.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Math/pgIMathTool.h>


#define D3DFVF_SIMPLETREEVERTEX ( D3DFVF_XYZ | D3DFVF_TEX1 )

#define FILL_SIMPLETREEVERTEX( v, ptx, pty, ptz, atu1, atv1 )  \
{   v.pos.x = ptx;  v.pos.y = pty;  v.pos.z = ptz;  \
    v.tex.x = atu1;  v.tex.y = atv1;  \
}


pgSimpleTree::pgSimpleTree(pgTexture* nTexture, int nNumPlanes, float nSizeX, float nSizeY,
						   float nMinX, float nMinY, float nMaxX, float nMaxY, bool nSelfRender) :
						   texture(nTexture), iBuffer(NULL), vBuffer(NULL), vertices(NULL), indices(NULL)
{
	selfRender = nSelfRender;
	numPlanes = nNumPlanes;
	sizeX = nSizeX;
	sizeY = nSizeY;
	tcMinX = nMinX;
	tcMinY = nMinY;
	tcMaxX = nMaxX;
	tcMaxY = nMaxY;
}


void
pgSimpleTree::create()
{
	int p, i, v;

	if(!checkDevice("cannot create TreeType: no render device available"))
		return;

	assert(numPlanes>0);

	numI = numPlanes * 4 + (numPlanes-1) * 2;		// 4 corners + 2 for degentriangles
	numV = numPlanes * 4;							// 4 corners


	// create the buffers
	//
	if(selfRender)
	{
		SAFE_RELEASE(iBuffer)
		SAFE_RELEASE(vBuffer)

		if(FAILED(pd3dDevice->CreateIndexBuffer(numI * sizeof(WORD), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
												D3DFMT_INDEX16, pgIDirectX::getPoolType(), &iBuffer)))
		{
			pgLog::error("creating index buffer for tree failed");
			iBuffer = NULL;
			vBuffer = NULL;
			return;
		}

		if(FAILED(pd3dDevice->CreateVertexBuffer(numV * sizeof(VERTEX),
												 D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
												 D3DFVF_SIMPLETREEVERTEX, pgIDirectX::getPoolType(), &vBuffer)))
		{
			pgLog::error("creating vertex buffer for terrainpatch failed");
			SAFE_RELEASE(iBuffer);
			vBuffer = NULL;
			return;
		}
	}


	// fill the buffers
	//
	VERTEX*	pVertices = NULL;
	WORD*	pIndices = NULL;

	if(selfRender)
	{
		vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);
		iBuffer->Lock(0, 0, (BYTE**)&pIndices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);
	}
	else
	{
		if(vertices)
			delete vertices;
		vertices = new VERTEX[numV];
		pVertices = vertices;

		if(indices)
			delete indices;
		indices = new WORD[numI];
		pIndices = indices;
	}


	for(v=0,p=0; p<numPlanes; p++,v+=4)
	{
		float	phi = pgFPI * (float)p / (float)numPlanes;
		float	dx = (float)(0.5 * sizeX * cos(phi)),
				dz = (float)(0.5 * sizeX * sin(phi));

		FILL_SIMPLETREEVERTEX(pVertices[v+0], pos.x-dx, pos.y+ 0.0f, pos.z+dz, tcMinX, tcMaxY)
		FILL_SIMPLETREEVERTEX(pVertices[v+1], pos.x-dx, pos.y+sizeY, pos.z+dz, tcMinX, tcMinY)
		FILL_SIMPLETREEVERTEX(pVertices[v+2], pos.x+dx, pos.y+ 0.0f, pos.z-dz, tcMaxX, tcMaxY)
		FILL_SIMPLETREEVERTEX(pVertices[v+3], pos.x+dx, pos.y+sizeY, pos.z-dz, tcMaxX, tcMinY)
	}
	assert(v==numV);

	for(i=0,p=0; p<numPlanes; p++,i+=4)
	{
		if(p>0)
		{
			pIndices[i] = (unsigned short)(p*4+0);
			i++;
		}

		pIndices[i+0] = (unsigned short)(p*4+0);
		pIndices[i+1] = (unsigned short)(p*4+1);
		pIndices[i+2] = (unsigned short)(p*4+2);
		pIndices[i+3] = (unsigned short)(p*4+3);

		if(p<numPlanes-1)
		{
			pIndices[i+4] = pIndices[i+3];
			i++;
		}
	}
	assert(i==numI);


	if(selfRender)
	{
		iBuffer->Unlock();
		vBuffer->Unlock();
	}
}


void
pgSimpleTree::destroy()
{
	SAFE_RELEASE(iBuffer)
	SAFE_RELEASE(vBuffer)
}


void
pgSimpleTree::render()
{
	if(!selfRender)
		return;

	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetTexture(0, texture->getD3DTexture());

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	pd3dDevice->SetVertexShader(D3DFVF_SIMPLETREEVERTEX);
	pd3dDevice->SetStreamSource(0, vBuffer, sizeof(VERTEX));
	pd3dDevice->SetIndices(iBuffer, 0);

	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, numV, 0, numI-2);
	pgIDirectX::addTerrainTris(numI-2);
}



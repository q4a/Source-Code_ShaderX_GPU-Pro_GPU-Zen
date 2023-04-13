///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBlendWood
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBlendWood.cpp,v 1.3 2002/12/22 10:50:55 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgBlendWood.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Math/pgIMathTool.h>


pgBlendWood::pgBlendWood()
{
	vBuffer = NULL;
	sorted = false;
	sortedTrees = NULL;
}


void
pgBlendWood::sort()
{
	int		i,j, num=trees.getSize(), jMinX, jMinZ;
	float	minX, minZ;

	xSortedTrees.removeAll();
	zSortedTrees.removeAll();

	for(i=0; i<num; i++)
		trees[i]->flag = 0;

	for(i=0; i<num; i++)
	{
		minX = pgFLOAT_MAX;
		minZ = pgFLOAT_MAX;
		jMinX = jMinZ = -1;

		for(j=0; j<trees.getSize(); j++)
		{
			const pgVec3& pos = trees[j]->pos;

			if(pos.x<minX && !(trees[j]->flag&1))
			{
				minX = pos.x;
				jMinX = j;
			}
			
			if(pos.z<minZ && !(trees[j]->flag&2))
			{
				minZ = pos.z;
				jMinZ = j;
			}
		}

		assert(jMinX!=-1);
		assert(jMinZ!=-1);

		xSortedTrees.addTail(trees[jMinX]);
		trees[jMinX]->flag |= 1;

		zSortedTrees.addTail(trees[jMinZ]);
		trees[jMinZ]->flag |= 2;
	}

	sorted = true;
}


void
pgBlendWood::update(const pgVec3& nViewerPos, const pgVec3& nViewerDir)
{
	if(!vBuffer && !createBuffer())
		return;

	if(!sorted)
		sort();

	float					h = nViewerDir[0];
	//float	h = (float)atan2(nTreePos.x-nViewerPos.x, nTreePos.z-nViewerPos.z) * 180.0f / pgFPI ;

	if(h>=315 || h<45)
	{
		sortedTrees = &zSortedTrees;
		drawDown = true;
	}
	else
	if(h>=135 && h<225)
	{
		sortedTrees = &zSortedTrees;
		drawDown = false;
	}
	else
	if(h>=45 && h<135)
	{
		sortedTrees = &xSortedTrees;
		drawDown = false;
	}
	else
	{
		sortedTrees = &xSortedTrees;
		drawDown = true;
	}


	pgBlendTree::VERTEX*	pVertices = NULL;
	vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);

	if(drawDown)
		for(int i=trees.getSize()-1; i>=0; i--)
		{
			TreeInfo* ti = sortedTrees->getAt(i);
			ti->tree->calcSettings(ti->pos, nViewerPos, nViewerDir, pVertices+i*4, ti->tex0, ti->tex1, ti->textureFactor);
		}
	else
		for(int i=0; i<trees.getSize(); i++)
		{
			TreeInfo* ti = sortedTrees->getAt(i);
			ti->tree->calcSettings(ti->pos, nViewerPos, nViewerDir, pVertices+i*4, ti->tex0, ti->tex1, ti->textureFactor);
		}

	/*for(int i=0; i<trees.getSize(); i++)
	{
		TreeInfo* ti = trees[i];
		ti->tree->calcSettings(ti->pos, nViewerPos, pVertices+i*4, ti->tex0, ti->tex1, ti->textureFactor);
	}*/

	vBuffer->Unlock();
}


void
pgBlendWood::render()
{
	if(!checkDevice("cannot render blend wood: no render device available"))
		return;


	// currently the wood position is fixed...
	D3DXMATRIX	mat;
	D3DXMatrixIdentity(&mat);
	pd3dDevice->SetTransform(D3DTS_WORLD, &mat);


	// setup the render device
	//
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   pgIDirectX::isFillSolid() ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   pgIDirectX::isFillSolid() ? D3DTOP_BLENDFACTORALPHA : D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, pgIDirectX::getFillFlag());

	pd3dDevice->SetVertexShader(D3DFVF_BLENDTREE_VERTEX);
	pd3dDevice->SetStreamSource(0, vBuffer, sizeof(pgBlendTree::VERTEX));


	// draw the trees
	//

	if(sortedTrees)
	{
		if(drawDown)
			for(int i=trees.getSize()-1; i>=0; i--)
			{
				TreeInfo* ti = sortedTrees->getAt(i);

				pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, ti->textureFactor);
				pd3dDevice->SetTexture(0, ti->tex0->getD3DTexture());
				pd3dDevice->SetTexture(1, ti->tex1->getD3DTexture());
				pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4*i, 2);
			}
		else
			for(int i=0; i<trees.getSize(); i++)
			{
				TreeInfo* ti = sortedTrees->getAt(i);

				pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, ti->textureFactor);
				pd3dDevice->SetTexture(0, ti->tex0->getD3DTexture());
				pd3dDevice->SetTexture(1, ti->tex1->getD3DTexture());
				pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4*i, 2);
			}
	}
	else
	for(int i=0; i<trees.getSize(); i++)
	{
		TreeInfo* ti = trees[i];

		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, ti->textureFactor);
		pd3dDevice->SetTexture(0, ti->tex0->getD3DTexture());
		pd3dDevice->SetTexture(1, ti->tex1->getD3DTexture());
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4*i, 2);
	}


	pgIDirectX::addNonLevelTris(4*trees.getSize());
}


bool
pgBlendWood::createBuffer()
{
	if(!checkDevice("cannot create blend wood buffer: no render device available"))
		return false;


	SAFE_RELEASE(vBuffer);

	if(FAILED(pd3dDevice->CreateVertexBuffer(trees.getSize() * 4 * sizeof(pgBlendTree::VERTEX), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
											 D3DFVF_BLENDTREE_VERTEX, pgIDirectX::getPoolType(), &vBuffer)))
	{
		pgLog::error("creating vertex buffer for blend wood failed");
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBlendTree
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBlendTree.cpp,v 1.3 2002/12/22 10:50:54 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgBlendTree.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Math/pgIMathTool.h>


#define FILL_VERTEX( v, ptx, pty, ptz, atu1, atv1 )  \
{   v.pos.x = (ptx);  v.pos.y = (pty);  v.pos.z = (ptz);  \
    v.tex1.x = (atu1);  v.tex1.y = (atv1);  \
}

pgBlendTree::pgBlendTree(float nSizeX, float nSizeY, bool nSelfRender) :
						 vBuffer(NULL), vertices(NULL)
{
	selfRender = nSelfRender;
	sizeX = nSizeX;
	sizeY = nSizeY;
}


void
pgBlendTree::addTexture(pgTexturePtr nTexture)
{
	textures.addTail(nTexture);
}


bool
pgBlendTree::createBuffer()
{
	if(!checkDevice("cannot create blendtree buffer: no render device available"))
		return false;

	if(selfRender)
	{
		SAFE_RELEASE(vBuffer)

		if(FAILED(pd3dDevice->CreateVertexBuffer(NUM_VERTICES * sizeof(VERTEX),
												 D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
												 D3DFVF_BLENDTREE_VERTEX, pgIDirectX::getPoolType(), &vBuffer)))
		{
			pgLog::error("creating vertex buffer for terrainpatch failed");
			vBuffer = NULL;
			return false;
		}
	}

	return true;
}


void
pgBlendTree::destroy()
{
	SAFE_RELEASE(vBuffer)
}


void
pgBlendTree::calcSettings(const pgVec3& nTreePos, const pgVec3& nViewerPos, const pgVec3& /*nViewerDir*/,
						  VERTEX* nVertices, pgTexture*& nTex0, pgTexture*& nTex1, DWORD& nTextureFactor) const
{
	float	phi = (float)atan2(nTreePos.x-nViewerPos.x, nTreePos.z-nViewerPos.z);
	int		numTex = textures.getSize();
	float	a = 1.0f - phi / pgFTWOPI;

	//phi = -nViewerDir[0] * pgFPI / 180.0f;

	float	dx = (float)(0.5 * sizeX * cos(phi)),
			dz = (float)(0.5 * sizeX * sin(phi));


	while(a< 0.0f)	a += 1.0f;
	while(a>=1.0f)	a -= 1.0f;

	int		idx0 = (int)(a * numTex);
	int		idx1 = (idx0+1 < numTex) ? idx0+1 : 0;
	float	fract = a * numTex - idx0;

	DWORD	sbf = (DWORD)(255.0f*(1.0f-fract));
	nTextureFactor = 0xffffffff - (sbf<<24);

	FILL_VERTEX(nVertices[0], nTreePos.x-dx, nTreePos.y+ 0.0f, nTreePos.z+dz, 0.0f, 1.0f)
	FILL_VERTEX(nVertices[1], nTreePos.x-dx, nTreePos.y+sizeY, nTreePos.z+dz, 0.0f, 0.0f)
	FILL_VERTEX(nVertices[2], nTreePos.x+dx, nTreePos.y+ 0.0f, nTreePos.z-dz, 1.0f, 1.0f)
	FILL_VERTEX(nVertices[3], nTreePos.x+dx, nTreePos.y+sizeY, nTreePos.z-dz, 1.0f, 0.0f)

	nTex0 = textures[idx0];
	nTex1 = textures[idx1];
}


void
pgBlendTree::render(const pgVec3& nViewerPos, const pgVec3& nViewerDir)
{
	if(!selfRender || !createBuffer() || !textures.getSize())
		return;


	D3DXMATRIX	mat;
	D3DXMatrixIdentity(&mat);
	pd3dDevice->SetTransform(D3DTS_WORLD, &mat);


	// fill the buffer
	//
	VERTEX*	pVertices = NULL;

	pgTexture	*tex0, *tex1;
	DWORD		textureFactor;

	vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);
	calcSettings(pos, nViewerPos, nViewerDir, pVertices, tex0, tex1, textureFactor);
	vBuffer->Unlock();


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
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, textureFactor);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, pgIDirectX::getFillFlag());

	pd3dDevice->SetTexture(0, tex0->getD3DTexture());
	pd3dDevice->SetTexture(1, tex1->getD3DTexture());


	// draw the tree
	//
	pd3dDevice->SetVertexShader(D3DFVF_BLENDTREE_VERTEX);
	pd3dDevice->SetStreamSource(0, vBuffer, sizeof(VERTEX));

	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_VERTICES-2);
	pgIDirectX::addNonLevelTris(NUM_VERTICES-2);
}



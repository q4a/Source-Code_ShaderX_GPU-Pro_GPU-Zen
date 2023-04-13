///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgSkyBox
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSkyBox.cpp,v 1.2 2002/12/18 22:00:38 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgSkyBox.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Math/pgVec3.h>


#define D3DFVF_SKYBOXVERTEX ( D3DFVF_XYZ | D3DFVF_TEX1 )

#define FILL_SKYBOXVERTEX( v, ptx, pty, ptz, atu1, atv1 )  \
{   v.pos.x = ptx;  v.pos.y = pty;  v.pos.z = ptz;  \
    v.tex.x = atu1;  v.tex.y = atv1;  \
}

struct SKYBOXVERTEX
{
	pgVec3		pos;
	pgVec2		tex;
};


pgSkyBox::pgSkyBox(bool nDownSide) : sizeH(2000.0f, 2000.0f, 2000.0f)
{
	for(int i=0; i<NUM_SIDES; i++)
		textures[i] = NULL;

	vBuffer = NULL;

	downSide = nDownSide;
}


void
pgSkyBox::setTextures(pgTexture* nFront, pgTexture* nBack,
					  pgTexture* nLeft, pgTexture* nRight,
					  pgTexture* nUp, pgTexture* nDown)
{
	textures[FRONT] = nFront;
	textures[BACK] = nBack;
	textures[LEFT] = nLeft;
	textures[RIGHT] = nRight;
	textures[UP] = nUp;
	textures[DOWN] = nDown;
}


void
pgSkyBox::deleteDeviceObjects()
{
	SAFE_RELEASE(vBuffer);
}


bool
pgSkyBox::restoreDeviceObjects()
{
	create();
	return true;
}

void
pgSkyBox::create()
{
	if(!checkDevice("cannot create skybox: no device set"))
		return;

	// create the vertex buffer
	//
	SAFE_RELEASE(vBuffer)
	if(FAILED(pd3dDevice->CreateVertexBuffer(NUM_VERTICES * sizeof(SKYBOXVERTEX), D3DUSAGE_WRITEONLY,
											 D3DFVF_SKYBOXVERTEX, pgIDirectX::getPoolType(), &vBuffer)))
	{
		pgLog::error("creating vertex buffer for skybox failed");
		return;
	}


	// fill the vertex buffer
	//
	SKYBOXVERTEX*	pVertices = NULL;
	float			minX = -sizeH.x/2.0f, minY = -sizeH.y/2.0f, minZ = -sizeH.z/2.0f,
					maxX =  sizeH.x/2.0f, maxY =  sizeH.y/2.0f, maxZ =  sizeH.z/2.0f;
	int				vIdx=0;

	assert(textures[0]);
	float d = 1.0f / textures[0]->getWidth();
	float rd = 1.0f-d;

	vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK);


	// front side
	//
	FILL_SKYBOXVERTEX(pVertices[vIdx+0], minX, minY, maxZ, rd, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+1], minX, maxY, maxZ, rd, d)
	FILL_SKYBOXVERTEX(pVertices[vIdx+2], maxX, minY, maxZ, d, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+3], maxX, maxY, maxZ, d, d)
	vIdx += 4;

	// back side
	//
	FILL_SKYBOXVERTEX(pVertices[vIdx+0], maxX, minY, minZ, rd, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+1], maxX, maxY, minZ, rd, d)
	FILL_SKYBOXVERTEX(pVertices[vIdx+2], minX, minY, minZ, d, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+3], minX, maxY, minZ, d, d)
	vIdx += 4;

	// left side
	//
	FILL_SKYBOXVERTEX(pVertices[vIdx+0], minX, minY, minZ, rd, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+1], minX, maxY, minZ, rd, d)
	FILL_SKYBOXVERTEX(pVertices[vIdx+2], minX, minY, maxZ, d, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+3], minX, maxY, maxZ, d, d)
	vIdx += 4;

	// right side
	//
	FILL_SKYBOXVERTEX(pVertices[vIdx+0], maxX, minY, maxZ, rd, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+1], maxX, maxY, maxZ, rd, d)
	FILL_SKYBOXVERTEX(pVertices[vIdx+2], maxX, minY, minZ, d, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+3], maxX, maxY, minZ, d, d)
	vIdx += 4;

	// up side
	//
	FILL_SKYBOXVERTEX(pVertices[vIdx+0], maxX, maxY, minZ, d, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+1], maxX, maxY, maxZ, rd, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+2], minX, maxY, minZ, d, d)
	FILL_SKYBOXVERTEX(pVertices[vIdx+3], minX, maxY, maxZ, rd, d)
	vIdx += 4;

	// down side
	//
	FILL_SKYBOXVERTEX(pVertices[vIdx+0], minX, minY, minZ, d, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+1], minX, minY, maxZ, rd, rd)
	FILL_SKYBOXVERTEX(pVertices[vIdx+2], maxX, minY, minZ, d, d)
	FILL_SKYBOXVERTEX(pVertices[vIdx+3], maxX, minY, maxZ, rd, d)
	vIdx += 4;


	assert(vIdx == NUM_VERTICES);
	vBuffer->Unlock();
}


void
pgSkyBox::render()
{
	if(vBuffer==NULL)
		return;

	D3DXMATRIX	wMat,
				vMat = *pgIDirectX::getViewMatrix().getD3DMatrix(),
				tMat = *pgIDirectX::getViewMatrix().getD3DMatrix();

	vMat._41 = vMat._43 = 0.0f;
	vMat._42 *= pgIDirectX::getSkyBoxHeightFactor();

	pd3dDevice->SetTransform(D3DTS_VIEW, &vMat);


	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, pgIDirectX::getFillFlag());
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	pd3dDevice->SetVertexShader(D3DFVF_SKYBOXVERTEX);
	pd3dDevice->SetStreamSource(0, vBuffer, sizeof(SKYBOXVERTEX));


	for(int side=0; side<NUM_SIDES; side++)
	{
		if(side!=5 || downSide)
		{
			pd3dDevice->SetTexture(0, textures[side]->getD3DTexture());
			pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, side*4, 2);
		}
	}


	pd3dDevice->SetTransform(D3DTS_VIEW, &tMat);
}

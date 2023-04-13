///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgLensflare
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgLensflare.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgLensflare.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgString.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Math/pgMatrix.h>


#define LF_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)


pgLensflare::pgLensflare() : pos(0.0f, 0.0f, 1000.0f)
{
	flareTex = NULL;
	numFlareImages = 0;
	baseAlpha = 1.0f;

    verts[0].tex = pgVec2(0.0f, 0.0f);
    verts[1].tex = pgVec2(0.0f, 1.0f);
    verts[2].tex = pgVec2(1.0f, 0.0f);
    verts[3].tex = pgVec2(1.0f, 1.0f);

	checkDevice("device not set: lensflare unavailable");
}


void
pgLensflare::loadStdFlareImages()
{
	numFlareImages = NUM_STDFLAREIMGS;
	flareTex = new pgTexturePtr[NUM_STDFLAREIMGS];

	for(int i=0; i<NUM_STDFLAREIMGS; i++)
	{
		pgStringEx fname("flare%d", i);

		flareTex[i] = pgIResourceManager::getTexture(fname, pgIResourceManager::SOURCE_STD);
	}
}


void
pgLensflare::setVertsPosSize(float nPosX, float nPosY, float nSize)
{
	float	dx = nSize,
			dy = nSize * pgIDirectX::getFOVX() / pgIDirectX::getFOVY();

    verts[0].pos = pgVec3(nPosX-dx, nPosY-dy,  0.8f);
    verts[1].pos = pgVec3(nPosX-dx, nPosY+dy,  0.8f);
    verts[2].pos = pgVec3(nPosX+dx, nPosY-dy,  0.8f);
    verts[3].pos = pgVec3(nPosX+dx, nPosY+dy,  0.8f);
}


void
pgLensflare::setVertsColor(float nRed, float nGreen, float nBlue, float nAlpha)
{
	DWORD col = D3DCOLOR_COLORVALUE(nRed, nGreen, nBlue, nAlpha);

	verts[0].color = col;
	verts[1].color = col;
	verts[2].color = col;
	verts[3].color = col;
}


void
pgLensflare::drawFlarePart(float nPos, int nIndex, float nSize, float nRed, float nGreen, float nBlue, float nAlpha)
{
	pgVec2 d = -screenPos;
	pgVec2 p = screenPos + nPos * d;

	setVertsPosSize(p.x,p.y, nSize);
	setVertsColor(nRed*nAlpha, nGreen*nAlpha, nBlue*nAlpha, 1.0f);
	pd3dDevice->SetTexture(0, flareTex[nIndex]->getD3DTexture());
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (LPVOID)verts, sizeof(LF_VERTEX));
}

void
pgLensflare::render()
{
	D3DXVECTOR3	pt, tpt, ppt;

	D3DXMATRIX	vMat = *pgIDirectX::getViewMatrix().getD3DMatrix(),
				pMat = *pgIDirectX::getProjectionMatrix().getD3DMatrix(),
				mat;
	D3DXVECTOR2 sunPos;

	vMat._41 = vMat._43 = 0.0f;
	vMat._42 *= pgIDirectX::getSkyBoxHeightFactor();

	pgIDirectX::setD3DVecFromVec3(pt, pos);
	D3DXVec3TransformCoord(&tpt, &pt, &vMat);

	if(tpt.z<pgIDirectX::getNearPlane())
		return;

	D3DXVec3TransformCoord(&ppt, &tpt, &pMat);

	screenPos.x = ppt.x;
	screenPos.y = ppt.y;

	float alpha = baseAlpha;

	if(ppt.x<-1.2f || ppt.x>1.2f || ppt.y<-1.2f || ppt.y>1.2f)
		return;

	if(ppt.x<-1.0f)
		alpha += 5.0f*(1.0f+ppt.x);

	if(ppt.x>1.0f)
		alpha -= 5.0f*(ppt.x-1.0f);

	if(ppt.y<-1.0f)
		alpha += 5.0f*(1.0f+ppt.y);

	if(ppt.y>1.0f)
		alpha -= 5.0f*(ppt.y-1.0f);

	if(alpha<0.0f)
		return;


	// setup render device
	//
	D3DXMatrixIdentity(&mat);

	pd3dDevice->SetTransform(D3DTS_VIEW, &mat);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pd3dDevice->SetRenderState(D3DRS_LIGHTING,  FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);


    pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
    pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetVertexShader(LF_FVF);


	// render all parts of the flare
	//
	drawFlarePart(0.0f,  4,  0.6f,   0.7f,   0.4f,  0.4f,  alpha);
	drawFlarePart(0.0f,  0,  0.25f,  1.0f,   1.0f,  1.0f,  alpha);
	drawFlarePart(0.0f,  1,  0.2f,   0.3f,   0.1f,  0.0f,  alpha);
	drawFlarePart(0.4f,  5,  0.2f,   0.07f,  0.07f, 0.10f, alpha);
	drawFlarePart(0.32f, 5,  0.05f,  0.07f,  0.07f, 0.10f, alpha);
	drawFlarePart(0.46f, 5,  0.1f,   0.07f,  0.07f, 0.10f, alpha);
	drawFlarePart(0.8f,  5,  0.06f,  0.30f,  0.20f, 0.13f, alpha);
	drawFlarePart(0.98f, 6,  0.01f,  0.50f,  1.00f, 0.50f, alpha);
	drawFlarePart(1.15f, 4,  0.03f,  0.50f,  1.00f, 0.50f, alpha);
	drawFlarePart(1.3f,  3,  0.2f,   0.30f,  0.20f, 0.13f, 1.0f*alpha);
	drawFlarePart(1.25f, 3,  0.1f,   0.30f,  0.20f, 0.13f, 0.7f*alpha);
	drawFlarePart(1.32f, 5,  0.05f,  0.30f,  0.20f, 0.13f, 0.7f*alpha);
	drawFlarePart(1.38f, 5,  0.05f,  0.10f,  0.20f, 0.10f, 0.5f*alpha);
	drawFlarePart(1.5f,  2,  0.3f,   0.10f,  0.20f, 0.10f, 0.4f*alpha);
	drawFlarePart(1.7f,  2,  0.5f,   0.10f,  0.20f, 0.10f, 0.4f*alpha);




	// clean up render device
	//
	pd3dDevice->SetTransform(D3DTS_VIEW, &vMat);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &pMat);
	pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
}

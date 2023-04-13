///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgSegment
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSegment.cpp,v 1.10 2003/01/06 14:37:01 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgSegment.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Render/pgMaterial.h>

pgSegment::pgSegment() : baseMesh(NULL)//, scale(1.0f, 1.0f, 1.0f)
{
	stateBlock = 0;
	settings = SET_ZTEST|SET_ZWRITE|SET_ALPHATEST;
}


void
pgSegment::init()
{
	checkDevice("mesh init failed: render device not available");
}


void
pgSegment::deleteDeviceObjects()
{
	deleteStateBlocks();

	if(baseMesh)
		baseMesh->deleteDeviceObjects();
}


bool
pgSegment::restoreDeviceObjects()
{
	createStageBlock();

	if(baseMesh)
		return baseMesh->restoreDeviceObjects();

	return true;
}


void
pgSegment::updateRenderSettings()
{
	createStageBlock();
}


void
pgSegment::deleteStateBlocks()
{
	if(pd3dDevice && stateBlock)
		pd3dDevice->DeleteStateBlock(stateBlock);
	stateBlock = 0L;
}


void
pgSegment::createStageBlock()
{
	int i;

	if(!checkDevice("mesh state block creation failed: render device not available"))
		return;

	if(!material)
	{
		pgLog::error("can not create stage blocks: no material set");
		return;
	}

	deleteStateBlocks();
	pd3dDevice->BeginStateBlock();

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, (settings&SET_ZTEST) ? D3DZB_TRUE : D3DZB_FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, (settings&SET_ZWRITE) ? D3DZB_TRUE : D3DZB_FALSE);

	if(material->blendSrc==pgMaterial::BLEND_NONE || material->blendDst==pgMaterial::BLEND_NONE)
	{
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	else
	{
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, material->blendSrc);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, material->blendDst);
	}

	if(settings&SET_ALPHATEST)
	{
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
		pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}
	else
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	//pd3dDevice->SetRenderState( D3DRS_FILLMODE, (settings&SET_FILL) ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE, material->culling);

	pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
	pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);

	pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
	pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	pgList<pgTextureStage>& stages = material->getStages();

	for(i=0; i<stages.getSize(); i++)
	{
		const pgTextureStage& stage = stages[i];

		pd3dDevice->SetTexture(i, stage.texture->getD3DTexture());

		pd3dDevice->SetTextureStageState(i, D3DTSS_COLOROP, stage.colorOp);
		pd3dDevice->SetTextureStageState(i, D3DTSS_COLORARG1, stage.colorArg1);
		pd3dDevice->SetTextureStageState(i, D3DTSS_COLORARG2, stage.colorArg2);

		pd3dDevice->SetTextureStageState(i, D3DTSS_ALPHAOP, stage.alphaOp);
		pd3dDevice->SetTextureStageState(i, D3DTSS_ALPHAARG1, stage.alphaArg1);
		pd3dDevice->SetTextureStageState(i, D3DTSS_ALPHAARG2, stage.alphaArg2);

		pd3dDevice->SetTextureStageState(i, D3DTSS_MINFILTER, stage.minFilter);
		pd3dDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, stage.magFilter);
		pd3dDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, stage.magFilter);

		pd3dDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, stage.coordIdx);
		pd3dDevice->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}

	pd3dDevice->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);


	pd3dDevice->EndStateBlock(&stateBlock);
}


void
pgSegment::applyFillAndCull()
{
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, pgIDirectX::isFillSolid() ? (settings&SET_FILL ? D3DFILL_SOLID : D3DFILL_WIREFRAME) : pgIDirectX::getFillFlag());
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, pgIDirectX::getCullFlag(material->culling));
}


int
pgSegment::renderTweenedWithBlendTextures(float nFrame, int nMinFrame, int nMaxFrame,
										  pgTexturePtr nTexture0, pgTexturePtr nTexture1, float nTexBlend)
{
	if(!checkDevice("cannot render tweened mesh: render device not valid"))
		return -1;

	if(!baseMesh)
	{
		pgLog::error("cannot render tweened mesh: base mesh not set");
		return -1;
	}




	DWORD sbf = (DWORD)(255.0f*nTexBlend);
	DWORD factor = sbf + (sbf<<8) + (sbf<<16) + (sbf<<24);

	pd3dDevice->ApplyStateBlock(stateBlock);		// two stages have to be prepared...
	applyFillAndCull();

	pd3dDevice->SetTexture(0, nTexture0->getD3DTexture());
	pd3dDevice->SetTexture(1, nTexture1->getD3DTexture());
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, factor);


	float framef = nMinFrame + nFrame*(nMaxFrame-nMinFrame);

	int frame = (int)framef;
	float subFrame = framef-frame;

	int nextFrame = frame+1;

	if(nextFrame>nMaxFrame)
		nextFrame = nMinFrame;

	if(settings&SET_LIGHT && material)
		pd3dDevice->SetMaterial(&material->ddMaterial);


	applyTransformation();
	baseMesh->renderTweened(frame, nextFrame, subFrame);
	undoTransformation();

	return frame;
}


int
pgSegment::renderTweened(float nFrame, int nMinFrame, int nMaxFrame)
{
	if(!checkDevice("cannot render tweened mesh: render device not valid"))
		return -1;

	if(!baseMesh)
	{
		pgLog::error("cannot render tweened mesh: base mesh not set");
		return -1;
	}


	pd3dDevice->ApplyStateBlock(stateBlock);
	applyFillAndCull();

	float framef = nMinFrame + nFrame*(nMaxFrame-nMinFrame+1);

	int frame = (int)framef;
	float subFrame = framef-frame;

	int nextFrame = frame+1;

	if(nextFrame>nMaxFrame)
		nextFrame = nMinFrame;

	if(settings&SET_LIGHT && material)
		pd3dDevice->SetMaterial(&material->ddMaterial);

	
	applyTransformation();
	baseMesh->renderTweened(frame, nextFrame, subFrame);
	undoTransformation();

	return frame;
}

void
pgSegment::render(int nFrame)
{
	if(!checkDevice("cannot render tweened mesh: render device not valid"))
		return;

	if(!baseMesh)
	{
		pgLog::error("cannot render tweened mesh: base mesh not set");
		return;
	}
	

	pd3dDevice->ApplyStateBlock(stateBlock);
	applyFillAndCull();

	if(settings&SET_LIGHT && material)
		pd3dDevice->SetMaterial(&material->ddMaterial);

	
	applyTransformation();
	baseMesh->render(nFrame);
	undoTransformation();
}


pgString
pgSegment::getStringFromSettings(int nSet)
{
	pgString str;

	if(nSet & SET_LIGHT)
		str += " light";

	if(nSet & SET_ZTEST)
		str += " ztest";

	if(nSet & SET_ZWRITE)
		str += " zwrite";

	if(nSet & SET_ALPHATEST)
		str += " alphatest";

	if(nSet & SET_FILL)
		str += " fill";

	if(nSet & SET_TEXTURED)
		str += " textured";

	// cut the leading blank
	str = str.getSubString(1, str.getLength()-1);

	return str;
}


int
pgSegment::getSettingsFromString(const pgString& nString)
{
	pgList<pgString>	list;
	int					i, sets=0;

	pgIStringTool::fillTokenList(nString, ' ', list);

	for(i=0; i<list.getSize(); i++)
	{
		const pgString& str = list[i];
		
		if(str=="light")
			sets |= SET_LIGHT;
		else if(str=="ztest")
			sets |= SET_ZTEST;
		else if(str=="zwrite")
			sets |= SET_ZWRITE;
		else if(str=="alphatest")
			sets |= SET_ALPHATEST;
		else if(str=="fill")
			sets |= SET_FILL;
		else if(str=="textured")
			sets |= SET_TEXTURED;
	}

	return sets;
}

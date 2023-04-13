///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgAnimated
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgAnimated.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgAnimated.h"

#include <pgLib/Base/pgInTextFile.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Base/pgISettings.h>

#include <pgLib/Math/pgIMathTool.h>

#include <pgLib/Render/pgMesh.h>
#include <pgLib/Render/pgMaterial.h>


pgAnimated::pgAnimated()
{
	curAnim = -1;
	didMeshLoop = false;
	freeze = false;
	hidden = false;
	meshTime = texTime = 0.0f;
}


void
pgAnimated::deleteDeviceObjects()
{
	for(int i=0; i<anims.getSize(); i++)
		if(anims[i]->mesh)
			anims[i]->mesh->deleteDeviceObjects();
}


bool
pgAnimated::restoreDeviceObjects()
{
	bool ret = true;

	for(int i=0; i<anims.getSize(); i++)
		if(anims[i]->mesh)
			if(!anims[i]->mesh->restoreDeviceObjects())
				ret = false;

	return ret;
}


bool
pgAnimated::load(const char* nFileName)
{
	name = nFileName;

	//FILE*		fp;
	AnimInfo*		ai = NULL;
	const char*		fName = pgISettings::tmpFullAniPath(nFileName, ".ani");
	int				i, j;
	pgInTextFile	inFile;

	if(!inFile.open(fName))
	{
		pgLog::error("ani file %s could not be opened", nFileName);
		return false;
	}

	// load and parse the ani file
	//
	while(!inFile.eof())
	{
		const char* line = inFile.readLine();

		if(pgIStringTool::startsWith(line, "#") || pgIStringTool::isEmpty(line))
			continue;																	// ignore this line

		if(pgIStringTool::startsWith(line, "ANIM"))
		{
			ai = new AnimInfo();
			ai->name = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			anims.addTail(ai);
		}
		else
		if(pgIStringTool::startsWith(line, "OFFSET"))
		{
			const char	*off = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			sscanf(off, "%f %f %f", &(ai->offset[0]), &(ai->offset[1]), &(ai->offset[2]));
		}
		else
		if(pgIStringTool::startsWith(line, "ROTATION"))
		{
			const char	*off = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			sscanf(off, "%f %f %f", &(ai->rotation[0]), &(ai->rotation[1]), &(ai->rotation[2]));
			ai->rotation *= pgFPI / 180.0f;
		}
		else
		if(pgIStringTool::startsWith(line, "SCALE"))
		{
			const char	*off = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			sscanf(off, "%f %f %f", &(ai->scale[0]), &(ai->scale[1]), &(ai->scale[2]));
			ai->rotation *= pgFPI / 180.0f;
		}
		else
		if(pgIStringTool::startsWith(line, "TEXTUREDURATION"))
		{
			const char	*dur = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			ai->texDur = (float)atof(dur);
		}
		else
		if(pgIStringTool::startsWith(line, "TEXTURE"))
		{
			const char	*op = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			ai->texOp = pgIStringTool::startsWithIgnoreCase(op, "blend") ? BLEND : SWITCH;
			const char	*names = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(op));

			pgIStringTool::fillTokenList(names, ' ', ai->texNames);
		}
		else
		if(pgIStringTool::startsWith(line, "CULLING"))
		{
			const char	*op = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			ai->culling = pgIStringTool::startsWithIgnoreCase(op, "true");
		}
		else
		if(pgIStringTool::startsWith(line, "FRAMES"))
		{
			const char	*range = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			sscanf(range, "%d-%d", &ai->minFrame, &ai->maxFrame);
		}
		else
		if(pgIStringTool::startsWith(line, "MESHDURATION"))
		{
			const char	*dur = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			ai->meshDur = (float)atof(dur);
		}
		else
		if(pgIStringTool::startsWith(line, "MESH"))
		{
			const char	*op = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			ai->meshOp = pgIStringTool::startsWithIgnoreCase(op, "blend") ? BLEND : SWITCH;
			ai->meshFileName = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(op));
		}
		else
			pgLog::info("unknown line in ani file ('%s'): %s", nFileName, line);
	}



	// load the md2 animations
	//
	for(i=0; i<anims.getSize(); i++)
	{
		pgMaterial* mat = new pgMaterial();
		mat->addStage(pgTextureStage());					// add two empty texture stages, since we will set
		mat->addStage(pgTextureStage());					// the specific textures at runtime
		mat->setCulling(ai->culling ? pgMaterial::CULL_CW : pgMaterial::CULL_NONE);

		pgSegment* seg = new pgSegment();
		seg->setBaseMesh(pgIResourceManager::getBaseMeshMD2(anims[i]->meshFileName));
		seg->setRenderSettings(pgSegment::SET_ZTEST|/*pgSegment::SET_LIGHT|*/pgSegment::SET_ZWRITE|pgSegment::SET_FILL|pgSegment::SET_TEXTURED);
		seg->setMaterial(mat);
		pgMesh* mesh = new pgMesh();
		mesh->addSegment(seg);
		anims[i]->mesh = mesh;
	}


	// load the textures
	//
	for(i=0; i<anims.getSize(); i++)
	{
		if(anims[i]->texNames.getSize()==0)									// no texture name set ? -> use the md2 name
			anims[i]->texNames.addTail(anims[i]->meshFileName);

		for(j=0; j<anims[i]->texNames.getSize(); j++)
		{
			pgTexturePtr tex = pgIResourceManager::getTexture(anims[i]->texNames[j], pgIResourceManager::SOURCE_MD2);
			anims[i]->textures.addTail(tex);
		}
	}


	// check some settings
	//
	for(i=0; i<anims.getSize(); i++)
		if(anims[i]->minFrame==-1 || anims[i]->maxFrame==-1)
		{
			anims[i]->minFrame =0;
			anims[i]->maxFrame = anims[i]->mesh->getNumFrames();
		}

	return true;
}


void
pgAnimated::startAnimation(int nAnim)
{
	freeze = false;

	if(nAnim==curAnim)
	{
		meshTime = 0.0f;
		return;
	}

	texTime = 0.0f;
	curAnim = nAnim;

	if(curAnim<0 || curAnim>=anims.getSize())
		curAnim = 0;
}


void
pgAnimated::startAnimation(const char* nName)
{
	for(int i=0; i<anims.getSize(); i++)
		if(anims[i]->name==nName)
		{
			startAnimation(i);
			return;
		}
}


int
pgAnimated::getAnimationIndex(const char* nName)
{
	for(int i=0; i<anims.getSize(); i++)
		if(anims[i]->name==nName)
			return i;

	return -1;
}


void
pgAnimated::render()
{
	if(curAnim<0 || !anims[curAnim]->mesh)
		return;

	if(!freeze)
	{
		meshTime += pgITime::getLastFrameTime();
		texTime += pgITime::getLastFrameTime();
	}

	AnimInfo* ai = anims[curAnim];
	float tMeshTime = meshTime / ai->meshDur,
		  tTexTime = texTime / ai->texDur,
		  texNrF, texBlend=0.0f,
		  numF = (float)(ai->maxFrame - ai->minFrame + 1);
	int	  curTex, nextTex;
	pgTexturePtr cTex,nTex=NULL;

	if(tMeshTime>=1.0f)
		didMeshLoop = true;

	tMeshTime -= (int)tMeshTime;		// trunc to 0.0-1.0 (looping)
	tTexTime -= (int)tTexTime;			// trunc to 0.0-1.0 (looping)

	texNrF = tTexTime*ai->textures.getSize();
	curTex = (int)(tTexTime*ai->textures.getSize());
	cTex = ai->textures[curTex];

	if(ai->textures.getSize()>1)
	{
		nextTex = curTex+1;
		if(nextTex>=ai->textures.getSize())
			nextTex = 0;
		nTex = ai->textures[nextTex];
		texBlend = texNrF-(int)texNrF;
	}

	if(ai->texOp==SWITCH)
		nTex = NULL;

	if(ai->meshOp==SWITCH)					// we don't really have a non-blendig mode, so we round to keyframes
	{
		float f = (float)(int)(tMeshTime * numF);
		tMeshTime = f / numF;
	}

	if(hidden)
		return;

	ai->mesh->setPosition(ai->offset+pos);
	ai->mesh->setRotation(ai->rotation+rot);
	int frame = ai->mesh->renderTweenedWithBlendTextures(tMeshTime, ai->minFrame, ai->maxFrame, cTex, nTex, texBlend);
	int frameNext = (frame+1)<=ai->maxFrame ? frame+1 : ai->minFrame;

	curBBox = ai->mesh->getBBox(frame);
	curBBox.addBox(ai->mesh->getBBox(frameNext));
}

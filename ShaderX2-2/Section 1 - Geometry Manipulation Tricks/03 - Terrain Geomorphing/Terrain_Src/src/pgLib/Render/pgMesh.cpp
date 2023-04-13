///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgMesh
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMesh.cpp,v 1.3 2002/12/15 20:04:55 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgMesh.h"



pgMesh::pgMesh() 
{

}


void
pgMesh::deleteDeviceObjects()
{
	for(int i=0; i<segments.getSize(); i++)
		segments[i]->deleteDeviceObjects();
}


bool
pgMesh::restoreDeviceObjects()
{
	bool ret = true;

	for(int i=0; i<segments.getSize(); i++)
		if(!segments[i]->restoreDeviceObjects())
			ret = false;

	return ret;
}


int
pgMesh::renderTweenedWithBlendTextures(float nFrame, int nMinFrame, int nMaxFrame,
									   pgTexturePtr nTexture0, pgTexturePtr nTexture1, float nTexBlend)
{
	int i, ret=-1;

	applyTransformation();

	for(i=0; i<segments.getSize(); i++)
	{
		ret = segments[i]->renderTweenedWithBlendTextures(nFrame, nMinFrame, nMaxFrame, nTexture0, nTexture1, nTexBlend);
	}

	undoTransformation();
	return ret;
}


int
pgMesh::renderTweened(float nFrame, int nMinFrame, int nMaxFrame)
{
	int i, ret=-1;

	applyTransformation();

	for(i=0; i<segments.getSize(); i++)
	{
		ret = segments[i]->renderTweened(nFrame, nMinFrame, nMaxFrame);
	}

	undoTransformation();
	return ret;
}


void
pgMesh::render(int nFrame, int nSegment)
{
	int i;

	applyTransformation();

	if(nSegment<0)
		for(i=0; i<segments.getSize(); i++)
		{
			segments[i]->render(nFrame);
		}
		else
		{
			if(segments.getSize()>nSegment)
			{
				segments[nSegment]->render(nFrame);
			}
		}

	undoTransformation();
}


pgAABBox
pgMesh::getBBox(int nIndex)
{
	pgAABBox box;

	if(segments.getSize()>=0)
		box = *segments[0]->getBBox(nIndex);

	for(int i=1; i<segments.getSize(); i++)
		box.addBox(*segments[i]->getBBox(nIndex));

	return box;
}

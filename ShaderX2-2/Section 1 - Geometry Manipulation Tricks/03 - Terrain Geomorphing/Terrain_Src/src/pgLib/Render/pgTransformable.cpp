///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTransformable
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTransformable.cpp,v 1.6 2003/01/06 12:51:11 daniel Exp $
///////////////////////////////////////////////////////////////////


#include <pgLib/Base/pgIDirectX.h>
#include "pgTransformable.h"


pgTransformable::pgTransformable()
{
	transform = NULL;
	useHierarchical = false;
}


void
pgTransformable::createTransform()
{
	if(transform==NULL)
		transform = new pgTransform();
}


void
pgTransformable::destroyTransform()
{
	if(transform!=NULL)
		delete transform;

	transform = NULL;
}


void
pgTransformable::setTransform(pgTransform* nTrans)
{
	if(transform)
		delete transform;

	transform = nTrans;
}


void
pgTransformable::applyTransformation()
{
	if(!transform)
		return;

	pgIDirectX::pushMatrix(pgIDirectX::getViewMatrix());

	pgMatrix mat = transform->getMatrix();

	if(useHierarchical)
		mat.postMult(pgIDirectX::getViewMatrix());
	else
		mat.postMult(pgIDirectX::getCameraMatrix());

	pgIDirectX::setViewMatrix(mat);
}


void
pgTransformable::undoTransformation()
{
	if(!transform)
		return;

	pgIDirectX::setViewMatrix(pgIDirectX::popMatrix());
}

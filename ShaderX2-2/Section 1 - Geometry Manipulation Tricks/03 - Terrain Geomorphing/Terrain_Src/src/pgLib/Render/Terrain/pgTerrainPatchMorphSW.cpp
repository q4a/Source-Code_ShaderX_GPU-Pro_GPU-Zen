///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrainPatchMorphSW
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrainPatchMorphSW.cpp,v 1.13 2002/12/22 10:51:32 daniel Exp $
///////////////////////////////////////////////////////////////////

#include "pgTerrainPatchMorphSW.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/Render/Terrain/pgTerrain.h>

#include <assert.h>


pgTerrainPatchMorphSW::pgTerrainPatchMorphSW(pgTerrain* nMaster, int nX, int nY) : pgTerrainPatchBase(nMaster, nX,nY)
{
	vBuffer = NULL;
	iBuffer = NULL;
	verticesLow = NULL;
	yMoveSelf = yMoveLeft = yMoveLeft2 = yMoveRight = yMoveRight2 = yMoveBottom = yMoveBottom2 = yMoveTop = yMoveTop2 = NULL;
	newFollowsLeft = followsLeft = newFollowsRight = followsRight = newFollowsBottom = followsBottom = newFollowsTop = followsTop = false;
	recalcBorderLeft = recalcBorderRight = recalcBorderBottom = recalcBorderTop = false;
	forceRetessellation = false;
}


void
pgTerrainPatchMorphSW::init()
{
	pgTerrainPatchBase::init();
	verticesLow = new PATCHVERTEX[MAX_VERTICES];
	yMoveSelf = new float[MAX_VERTICES];
	yMoveLeft = new float[MAX_VERTICES];
	yMoveLeft2 = new float[MAX_VERTICES];
	yMoveRight = new float[MAX_VERTICES];
	yMoveRight2 = new float[MAX_VERTICES];
	yMoveTop = new float[MAX_VERTICES];
	yMoveTop2 = new float[MAX_VERTICES];
	yMoveBottom = new float[MAX_VERTICES];
	yMoveBottom2 = new float[MAX_VERTICES];
	newFollowsLeft = followsLeft = newFollowsRight = followsRight = newFollowsBottom = followsBottom = newFollowsTop = followsTop = false;
	recalcBorderLeft = recalcBorderRight = recalcBorderBottom = recalcBorderTop = false;
	forceRetessellation = false;
}


void
pgTerrainPatchMorphSW::deinit()
{
	pgTerrainPatchBase::deinit();

	if(verticesLow)
		delete verticesLow;
	verticesLow = NULL;

	if(yMoveSelf)
		delete yMoveSelf;
	yMoveSelf = NULL;

	if(yMoveLeft)
		delete yMoveLeft;
	yMoveLeft = NULL;

	if(yMoveLeft2)
		delete yMoveLeft2;
	yMoveLeft2 = NULL;

	if(yMoveRight)
		delete yMoveRight;
	yMoveRight = NULL;

	if(yMoveRight2)
		delete yMoveRight2;
	yMoveRight2 = NULL;

	if(yMoveBottom)
		delete yMoveBottom;
	yMoveBottom = NULL;

	if(yMoveBottom2)
		delete yMoveBottom2;
	yMoveBottom2 = NULL;

	if(yMoveTop)
		delete yMoveTop;
	yMoveTop = NULL;

	if(yMoveTop2)
		delete yMoveTop2;
	yMoveTop2 = NULL;

	deleteDeviceObjects();
}


void
pgTerrainPatchMorphSW::deleteDeviceObjects()
{
	SAFE_RELEASE(iBuffer);
	SAFE_RELEASE(vBuffer);
}


bool
pgTerrainPatchMorphSW::restoreDeviceObjects()
{
	forceBufferCreate = true;
	return true;
}


bool
pgTerrainPatchMorphSW::updateTessellation()
{
	pgTerrainPatchMorphSW* leftSW = (pgTerrainPatchMorphSW*)left;
	pgTerrainPatchMorphSW* rightSW = (pgTerrainPatchMorphSW*)right;
	pgTerrainPatchMorphSW* bottomSW = (pgTerrainPatchMorphSW*)bottom;
	pgTerrainPatchMorphSW* topSW = (pgTerrainPatchMorphSW*)top;

	if(leftSW && leftSW->oldRealSelfTes<5 && leftSW->newRealSelfTes!=leftSW->oldRealSelfTes)
		recalcBorderLeft = true;

	if(rightSW && rightSW->oldRealSelfTes<5 && rightSW->newRealSelfTes!=rightSW->oldRealSelfTes)
		recalcBorderRight = true;

	if(bottomSW && bottomSW->oldRealSelfTes<5 && bottomSW->newRealSelfTes!=bottomSW->oldRealSelfTes)
		recalcBorderBottom = true;

	if(topSW && topSW->oldRealSelfTes<5 && topSW->newRealSelfTes!=topSW->oldRealSelfTes)
		recalcBorderTop = true;

	if(pgTerrainPatchBase::updateTessellation())
		forceBufferCreate = true;

	return false;
}


bool
pgTerrainPatchMorphSW::updateTessellation2()
{
	float	selfTesEx = getExactTessellation(),
			leftTesEx = (left && left->isActive()) ? left->getExactTessellation() : -1.0f,
			rightTesEx = (right && right->isActive()) ? right->getExactTessellation() : -1.0f,
			bottomTesEx = (bottom && bottom->isActive()) ? bottom->getExactTessellation() : -1.0f,
			topTesEx = (top && top->isActive()) ? top->getExactTessellation() : -1.0f;

	newFollowsLeft = (selfTesEx<leftTesEx) || (selfTesEx==leftTesEx&& !((pgTerrainPatchMorphSW*)left)->newFollowsRight);
	newFollowsRight = (selfTesEx<rightTesEx) || (selfTesEx==rightTesEx && !((pgTerrainPatchMorphSW*)right)->newFollowsLeft);
	newFollowsBottom = (selfTesEx<bottomTesEx) || (selfTesEx==bottomTesEx && !((pgTerrainPatchMorphSW*)bottom)->newFollowsTop);
	newFollowsTop = (selfTesEx<topTesEx) || (selfTesEx==topTesEx && !((pgTerrainPatchMorphSW*)top)->newFollowsBottom);


	// check if this patch does no longer follow a neighbor
	// if there was no retessellation due to some other reason, we have to do one NOW
	if(!forceBufferCreate)
		if((!newFollowsLeft && followsLeft) || (!newFollowsRight && followsRight) ||
		   (!newFollowsBottom && followsBottom) || (!newFollowsTop && followsTop))
		{
			assert(selfTes-leftTes<=1);
			assert(selfTes-rightTes<=1);
			assert(selfTes-bottomTes<=1);
			assert(selfTes-topTes<=1);

			createTessellation(selfTes, leftTes, rightTes, bottomTes, topTes);
			numVertices = numNewVertices;
			numIndices = numNewIndices;
			forceRetessellation = true;
		}

	fillMorphVertices();
	return false;
}


bool
pgTerrainPatchMorphSW::updateTessellation3()
{
	checkBorderLeft();
	checkBorderRight();
	checkBorderBottom();
	checkBorderTop();

	fillBuffers();

	followsLeft = newFollowsLeft;
	followsRight = newFollowsRight;
	followsBottom = newFollowsBottom;
	followsTop = newFollowsTop;

	recalcBorderLeft = false;
	recalcBorderRight = false;
	recalcBorderBottom = false;
	recalcBorderTop = false;
	forceRetessellation = false;

	return false;
}


int
pgTerrainPatchMorphSW::render()
{
	if(!checkDevice("cannot render terrainpatch: no device set"))
		return 0;

	if(!isVisible())
		return 0;

	if(iBuffer==NULL || vBuffer==NULL)			// nothing to do ?
		return 0;


	pd3dDevice->SetVertexShader(D3DFVF_PATCHVERTEX);
	pd3dDevice->SetStreamSource(0, vBuffer, sizeof(PATCHVERTEX));
	pd3dDevice->SetIndices(iBuffer, 0);


	// draw the patch with one single D3D call
	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, numVertices, 0, numIndices-2);

	return numIndices-2;
}


void
pgTerrainPatchMorphSW::fillMorphVertices()
{
	if(forceBufferCreate || forceRetessellation)
	{
		memcpy(verticesLow, vertices, numNewVertices*sizeof(PATCHVERTEX));

		if(realSelfTes<4)
		{
			reduceShapeTo(realSelfTes+1, verticesLow);
			makeBordersSimpler(verticesLow);
		}

		//if(selfTes==realSelfTes+1)
		//	memcpy(verticesLow, vertices, numNewVertices*sizeof(PATCHVERTEX));

		for(int i=0; i<numNewVertices; i++)
			yMoveSelf[i] = vertices[i].pos.y - verticesLow[i].pos.y;
	}
}


void
pgTerrainPatchMorphSW::checkBorderLeft()
{
	int i, y,yc, pow;

	if(forceBufferCreate || forceRetessellation || followsLeft!=newFollowsLeft)
	{
		for(i=0; i<numNewVertices; i++)
			yMoveLeft[i] = 0.0f;

		for(i=0; i<numNewVertices; i++)
			yMoveLeft2[i] = 0.0f;
	}


	if(newFollowsLeft && (!followsLeft || forceBufferCreate || forceRetessellation || recalcBorderLeft))
	{
		const pgTerrainPatchMorphSW* leftSW = reinterpret_cast<const pgTerrainPatchMorphSW*>(left);

		// copy shared border column
		//
		assert(selfTes==leftSW->rightTes);
		assert(leftTes==leftSW->selfTes);
		assert(leftSW->indexMap);

		pow = getPowerTwo(min(selfTes,leftTes));

		for(y=pow; y<SOURCE_HEIGHT-1; y+=pow)
		{
			int idx = indexMap[0 + y * SOURCE_WIDTH],
				idxL = leftSW->indexMap[SOURCE_WIDTH-1 + y * SOURCE_WIDTH];
			assert(idx!=0xffff);
			assert(idxL!=0xffff);

			verticesLow[idx].pos.y = leftSW->verticesLow[idxL].pos.y;
			vertices[idx].pos.y = leftSW->vertices[idxL].pos.y;
			yMoveLeft[idx] = leftSW->yMoveSelf[idxL];
			yMoveSelf[idx] = 0.0f;
		}


		if(selfTes>=3)
			return;

		// correct border neighbors
		//
		pow = getPowerTwo(selfTes);

		for(y=pow,yc=1; y<SOURCE_HEIGHT-1; y+=pow,yc++)
		{
			float yLT=0.0f,yRB=0.0f;
			int idx, idxLT=0,idxRB=0;

			idx = calculateDiagonalVertices(pow, y, 1, yc, pow, verticesLow, yLT,idxLT, yRB,idxRB);
			assert(idx>=0 && idx!=0xffff);

			float moveLT = vertices[idxLT].pos.y - verticesLow[idxLT].pos.y;

			verticesLow[idx].pos.y = (yLT + yRB) / 2.0f;

			// our vertex is moved 50% by the left (-top) neighbor
			// this garanties that this vertex is on the diagonal when currentError is 0.0f
			yMoveLeft2[idx] = moveLT * 0.5f;

			yMoveSelf[idx] = vertices[idx].pos.y - verticesLow[idx].pos.y;
		}


		// correct neighbors to lie on a diagonal
		//
		if(selfTes!=realSelfTes)
		{
			pow = getPowerTwo(selfTes);

			for(y=pow,yc=1; y<SOURCE_HEIGHT-1; y+=pow,yc++)
			{
				float yLT=0.0f,yRB=0.0f;
				int idx, idxLT=0,idxRB=0;

				idx = calculateDiagonalVertices(pow, y, 1, yc, pow, verticesLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				verticesLow[idx].pos.y = (yLT + yRB) * 0.5f;
				vertices[idx].pos.y = (vertices[idxLT].pos.y + vertices[idxRB].pos.y) * 0.5f;

				// the real new part: this vertex always STAYS on the diagonal
				//
				yMoveLeft2[idx] = 0.0f;
				yMoveLeft[idx] = (vertices[idxLT].pos.y - verticesLow[idxLT].pos.y) * 0.5f;
				yMoveSelf[idx] = (vertices[idxRB].pos.y - verticesLow[idxRB].pos.y) * 0.5f;
			}

			// necessary to recorrect the shared right-top border neighbor
			//
			if(followsBottom)
				recalcBorderBottom = true;
		}
	}
}


void
pgTerrainPatchMorphSW::checkBorderRight()
{
	int i, y,yc, pow, xcMax;

	if(forceBufferCreate || forceRetessellation || followsRight!=newFollowsRight)
	{
		for(i=0; i<numNewVertices; i++)
			yMoveRight[i] = 0.0f;

		for(i=0; i<numNewVertices; i++)
			yMoveRight2[i] = 0.0f;
	}

	if(newFollowsRight && (!followsRight || forceBufferCreate || forceRetessellation || recalcBorderRight))
	{
		const pgTerrainPatchMorphSW* rightSW = reinterpret_cast<const pgTerrainPatchMorphSW*>(right);

		// copy shared border column
		//
		assert(selfTes==rightSW->leftTes);
		assert(rightTes==rightSW->selfTes);
		assert(rightSW->indexMap);

		pow = getPowerTwo(min(selfTes,rightTes));

		for(y=pow; y<SOURCE_HEIGHT-1; y+=pow)
		{
			int idx = indexMap[SOURCE_WIDTH-1 + y * SOURCE_WIDTH],
				idxR = rightSW->indexMap[0 + y * SOURCE_WIDTH];
			assert(idx!=0xffff);
			assert(idxR!=0xffff);

			verticesLow[idx].pos.y = rightSW->verticesLow[idxR].pos.y;
			vertices[idx].pos.y = rightSW->vertices[idxR].pos.y;
			yMoveRight[idx] = rightSW->yMoveSelf[idxR];
			yMoveSelf[idx] = 0.0f;
		}


		// correct border neighbors
		//
		if(realSelfTes<3)
		{
			pow = getPowerTwo(realSelfTes);
			xcMax = (SOURCE_WIDTH-1-pow)/pow;

			for(y=pow,yc=1; y<SOURCE_HEIGHT-1; y+=pow,yc++)
			{
				float yLT=0.0f,yRB=0.0f;
				int idx, idxLT=0,idxRB=0;

				idx = calculateDiagonalVertices(SOURCE_WIDTH-1-pow, y, xcMax, yc, pow, verticesLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				verticesLow[idx].pos.y = (yLT + yRB) * 0.5f;

				// our vertex is moved 50% by the right (-bottom) neighbor
				// this garanties that this vertex is on the diagonal when currentError is 0.0f
				yMoveRight2[idx] = (vertices[idxRB].pos.y - verticesLow[idxRB].pos.y) * 0.5f;

				yMoveSelf[idx] = vertices[idx].pos.y - verticesLow[idx].pos.y;
			}
		}


		// correct neighbors to lie on a diagonal
		//
		if(selfTes!=realSelfTes)
		{
			pow = getPowerTwo(selfTes);
			xcMax = (SOURCE_WIDTH-1-pow)/pow;

			for(y=pow,yc=1; y<SOURCE_HEIGHT-1; y+=pow,yc++)
			{
				float yLT=0.0f,yRB=0.0f;
				int idx, idxLT=0,idxRB=0;

				idx = calculateDiagonalVertices(SOURCE_WIDTH-1-pow, y, xcMax, yc, pow, verticesLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				verticesLow[idx].pos.y = (yLT + yRB) * 0.5f;
				vertices[idx].pos.y = (vertices[idxLT].pos.y + vertices[idxRB].pos.y) * 0.5f;

				// the real new part: this vertex always STAYS on the diagonal
				//
				yMoveRight2[idx] = 0.0f;
				yMoveRight[idx] = (vertices[idxRB].pos.y - verticesLow[idxRB].pos.y) * 0.5f;
				yMoveSelf[idx] = (vertices[idxLT].pos.y - verticesLow[idxLT].pos.y) * 0.5f;
			}

			// necessary to recorrect the shared right-top border neighbor
			//
			if(followsTop)
				recalcBorderTop = true;
		}
	}
}

void
pgTerrainPatchMorphSW::checkBorderBottom()
{
	int i, x,xc, pow;

	if(forceBufferCreate || forceRetessellation || followsBottom!=newFollowsBottom)
	{
		for(i=0; i<numNewVertices; i++)
			yMoveBottom[i] = 0.0f;

		for(i=0; i<numNewVertices; i++)
			yMoveBottom2[i] = 0.0f;
	}


	if(newFollowsBottom && (!followsBottom || forceBufferCreate || forceRetessellation || recalcBorderBottom))
	{
		const pgTerrainPatchMorphSW* bottomSW = reinterpret_cast<const pgTerrainPatchMorphSW*>(bottom);

		// copy shared border column
		//
		assert(selfTes==bottomSW->topTes);
		assert(bottomTes==bottomSW->selfTes);
		assert(bottomSW->indexMap);

		pow = getPowerTwo(min(selfTes,bottomTes));

		for(x=pow; x<SOURCE_WIDTH-1; x+=pow)
		{
			int idx = indexMap[x + 0 * SOURCE_WIDTH],
				idxB = bottomSW->indexMap[x + (SOURCE_WIDTH-1) * SOURCE_WIDTH];
			assert(idx!=0xffff);
			assert(idxB!=0xffff);

			verticesLow[idx].pos.y = bottomSW->verticesLow[idxB].pos.y;
			vertices[idx].pos.y = bottomSW->vertices[idxB].pos.y;
			yMoveBottom[idx] = bottomSW->yMoveSelf[idxB];
			yMoveSelf[idx] = 0.0f;
		}


		if(selfTes>=3)
			return;

		// correct border neighbors
		//
		pow = getPowerTwo(selfTes);

		for(x=pow,xc=1; x<SOURCE_WIDTH-1; x+=pow,xc++)
		{
			float yLT=0.0f,yRB=0.0f;
			int idx, idxLT=0,idxRB=0;

			idx = calculateDiagonalVertices(x, pow, xc, 1, pow, verticesLow, yLT,idxLT, yRB,idxRB);
			assert(idx>=0 && idx!=0xffff);

			float moveRB = vertices[idxRB].pos.y - verticesLow[idxRB].pos.y;

			verticesLow[idx].pos.y = (yLT + yRB) / 2.0f;

			// our vertex is moved 50% by the right (-bottom) neighbor
			// this garanties that this vertex is on the diagonal when currentError is 0.0f
			yMoveBottom2[idx] = moveRB * 0.5f;

			yMoveSelf[idx] = vertices[idx].pos.y - verticesLow[idx].pos.y;
		}


		// correct neighbors to lie on a diagonal
		//
		if(selfTes!=realSelfTes)
		{
			pow = getPowerTwo(selfTes);

			for(x=pow,xc=1; x<SOURCE_WIDTH-1; x+=pow,xc++)
			{
				float yLT=0.0f,yRB=0.0f;
				int idx, idxLT=0,idxRB=0;

				idx = calculateDiagonalVertices(x, pow, xc, 1, pow, verticesLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				verticesLow[idx].pos.y = (yLT + yRB) * 0.5f;
				vertices[idx].pos.y = (vertices[idxLT].pos.y + vertices[idxRB].pos.y) * 0.5f;

				// the real new part: this vertex always STAYS on the diagonal
				//
				yMoveBottom2[idx] = 0.0f;
				yMoveBottom[idx] = (vertices[idxRB].pos.y - verticesLow[idxRB].pos.y) * 0.5f;

				if(yMoveLeft[idx]==0.0f)
					yMoveSelf[idx] = (vertices[idxLT].pos.y - verticesLow[idxLT].pos.y) * 0.5f;
				else
					yMoveSelf[idx] = 0.0f;
			}
		}
	}
}


void
pgTerrainPatchMorphSW::checkBorderTop()
{
	int i, x,xc, pow, ycMax;

	if(forceBufferCreate || forceRetessellation || followsTop!=newFollowsTop)
	{
		for(i=0; i<numNewVertices; i++)
			yMoveTop[i] = 0.0f;

		for(i=0; i<numNewVertices; i++)
			yMoveTop2[i] = 0.0f;
	}

	if(newFollowsTop && (!followsTop || forceBufferCreate || forceRetessellation || recalcBorderTop))
	{
		const pgTerrainPatchMorphSW* topSW = reinterpret_cast<const pgTerrainPatchMorphSW*>(top);

		// copy shared border column
		//
		assert(selfTes==topSW->bottomTes);
		assert(topTes==topSW->selfTes);
		assert(topSW->indexMap);

		pow = getPowerTwo(min(selfTes,topTes));

		for(x=pow; x<SOURCE_WIDTH-1; x+=pow)
		{
			int idx = indexMap[x + (SOURCE_HEIGHT-1) * SOURCE_WIDTH],
				idxT = topSW->indexMap[x + 0 * SOURCE_WIDTH];
			assert(idx!=0xffff);
			assert(idxT!=0xffff);

			verticesLow[idx].pos.y = topSW->verticesLow[idxT].pos.y;
			vertices[idx].pos.y = topSW->vertices[idxT].pos.y;
			yMoveTop[idx] = topSW->yMoveSelf[idxT];
			yMoveSelf[idx] = 0.0f;
		}


		if(selfTes>=3)
			return;

		// correct border neighbors
		//
		pow = getPowerTwo(selfTes);
		ycMax = (SOURCE_WIDTH-1-pow)/pow;

		for(x=pow,xc=1; x<SOURCE_WIDTH-1; x+=pow,xc++)
		{
			float yLT=0.0f,yRB=0.0f;
			int idx, idxLT=0,idxRB=0;

			idx = calculateDiagonalVertices(x, SOURCE_HEIGHT-1-pow, xc, ycMax, pow, verticesLow, yLT,idxLT, yRB,idxRB);
			assert(idx>=0 && idx!=0xffff);

			float moveLT = vertices[idxLT].pos.y - verticesLow[idxLT].pos.y;

			verticesLow[idx].pos.y = (yLT + yRB) / 2.0f;

			// our vertex is moved 50% by the left (-top) neighbor
			// this garanties that this vertex is on the diagonal when currentError is 0.0f
			yMoveTop2[idx] = moveLT * 0.5f;

			yMoveSelf[idx] = vertices[idx].pos.y - verticesLow[idx].pos.y;
		}


		// correct neighbors to lie on a diagonal
		//
		if(selfTes!=realSelfTes)
		{
			pow = getPowerTwo(selfTes);
			ycMax = (SOURCE_WIDTH-1-pow)/pow;

			for(x=pow,xc=1; x<SOURCE_WIDTH-1; x+=pow,xc++)
			{
				float yLT=0.0f,yRB=0.0f;
				int idx, idxLT=0,idxRB=0;

				idx = calculateDiagonalVertices(x, SOURCE_HEIGHT-1-pow, xc, ycMax, pow, verticesLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				verticesLow[idx].pos.y = (yLT + yRB) * 0.5f;
				vertices[idx].pos.y = (vertices[idxLT].pos.y + vertices[idxRB].pos.y) * 0.5f;

				// the real new part: this vertex always STAYS on the diagonal
				//
				yMoveTop2[idx] = 0.0f;
				yMoveTop[idx] = (vertices[idxLT].pos.y - verticesLow[idxLT].pos.y) * 0.5f;

				if(yMoveRight[idx]==0.0f)
					yMoveSelf[idx] = (vertices[idxRB].pos.y - verticesLow[idxRB].pos.y) * 0.5f;
				else
					yMoveSelf[idx] = 0.0f;

			}
		}
	}
}


bool
pgTerrainPatchMorphSW::fillBuffers()
{
	if(!checkDevice("cannot create terrainpatch: no device set"))
		return false;

	if(!checkBuffers())
		return false;


	if(numVertices==0 || numIndices==0)					// nothing to do ???
		return true;


	// fill vertex buffer
	//
	PATCHVERTEX*	pVertices = NULL;
	vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);

	if(selfTes>0 || selfTes<MAX_SUBDIV)
	{
		int		i;

		factorSelf = 1.0f-currentError;
		factorLeft = left ? 1.0f-left->getCurrentError() : 0.0f;
		factorRight = right ? 1.0f-right->getCurrentError() : 0.0f;
		factorBottom = bottom ? 1.0f-bottom->getCurrentError() : 0.0f;
		factorTop = top ? 1.0f-top->getCurrentError() : 0.0f;

		assert(factorSelf>=0.0f);
		assert(factorLeft>=0.0f);
		assert(factorRight>=0.0f);
		assert(factorBottom>=0.0f);
		assert(factorTop>=0.0f);

		// clamping for patches which use selfTes!=realSelfTes
		//
		if(factorSelf>=1.0f)
			factorSelf = 1.0f;
		if(factorLeft>=1.0f)
			factorLeft = 1.0f;
		if(factorRight>=1.0f)
			factorRight = 1.0f;
		if(factorBottom>=1.0f)
			factorBottom = 1.0f;
		if(factorTop>=1.0f)
			factorTop = 1.0f;

		float	factorLeft2 = (1.0f-factorSelf) * factorLeft,
				factorRight2 = (1.0f-factorSelf) * factorRight,
				factorBottom2 = (1.0f-factorSelf) * factorBottom,
				factorTop2 = (1.0f-factorSelf) * factorTop;


		// either this patch has to follow its neighbors or the other way around
		//
		assert(!(left && left->isActive()) || (newFollowsLeft!=reinterpret_cast<const pgTerrainPatchMorphSW*>(left)->newFollowsRight));
		assert(!(right && right->isActive()) || (newFollowsRight!=reinterpret_cast<const pgTerrainPatchMorphSW*>(right)->newFollowsLeft));
		assert(!(bottom && bottom->isActive()) || (newFollowsBottom!=reinterpret_cast<const pgTerrainPatchMorphSW*>(bottom)->newFollowsTop));
		assert(!(top && top->isActive()) || (newFollowsTop!=reinterpret_cast<const pgTerrainPatchMorphSW*>(top)->newFollowsBottom));


		for(i=0; i<numVertices; i++)
		{
			pVertices[i].tex = verticesLow[i].tex;
			pVertices[i].pos.x = verticesLow[i].pos.x;
			pVertices[i].pos.z = verticesLow[i].pos.z;

			pVertices[i].pos.y = verticesLow[i].pos.y + factorSelf * yMoveSelf[i]
							   + factorLeft * yMoveLeft[i] + factorLeft2 * yMoveLeft2[i]
							   + factorRight * yMoveRight[i] + factorRight2 * yMoveRight2[i]
							   + factorBottom * yMoveBottom[i] + factorBottom2 * yMoveBottom2[i]
							   + factorTop * yMoveTop[i] + factorTop2 * yMoveTop2[i];
		}

	}
	else
		memcpy(pVertices, vertices, numVertices*sizeof(PATCHVERTEX));

	vBuffer->Unlock();


	// fill index buffer
	//
	WORD*			pIndices = NULL;

	iBuffer->Lock(0, 0, (BYTE**)&pIndices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);
	memcpy(pIndices, indices, numIndices*sizeof(unsigned short));
	//for(int i=0; i<numIndices; i++)
	//	pgLog::trace("%d: %d\n", i, indices[i]);
	iBuffer->Unlock();

	return true;
}


bool
pgTerrainPatchMorphSW::checkBuffers()
{
	// create new buffers if the size changed
	//
	if(numIndices!=numNewIndices || forceBufferCreate)
	{
		numIndices = numNewIndices;
		SAFE_RELEASE(iBuffer)

		if(numIndices>0)
			if(FAILED(pd3dDevice->CreateIndexBuffer(numIndices * sizeof(WORD), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
													D3DFMT_INDEX16, pgIDirectX::getPoolType(), &iBuffer)))
			{
				pgLog::error("creating index buffer for terrainpatch failed");
				return false;
			}
	}


	if(numVertices!=numNewVertices  || forceBufferCreate)
	{
		numVertices = numNewVertices;
		SAFE_RELEASE(vBuffer)

		if(numVertices>0)
			if(FAILED(pd3dDevice->CreateVertexBuffer(numVertices * sizeof(PATCHVERTEX), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
													 D3DFVF_PATCHVERTEX, pgIDirectX::getPoolType(), &vBuffer)))
			{
				pgLog::error("creating vertex buffer for terrainpatch failed");
				SAFE_RELEASE(iBuffer);
				return false;
			}
	}

	forceBufferCreate = false;
	return true;
}



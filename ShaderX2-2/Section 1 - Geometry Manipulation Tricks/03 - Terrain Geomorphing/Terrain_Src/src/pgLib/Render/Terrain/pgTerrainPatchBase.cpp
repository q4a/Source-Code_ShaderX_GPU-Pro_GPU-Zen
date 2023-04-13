///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrainPatchBase
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrainPatchBase.cpp,v 1.13 2003/03/09 18:15:24 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgTerrainPatchBase.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/Math/pgPlane.h>
#include <pgLib/Render/Terrain/pgTerrain.h>

#include <assert.h>


#define setD3DVec(v1, v0)  {  v1.x = v0.x;  v1.y = v0.y;  v1.z = v0.z;  }


pgTerrainPatchBase::pgTerrainPatchBase(pgTerrain* nMaster, int nX, int nY) :
				active(false), scale(1.0f, 1.0f, 1.0f)
{
	visFlags = 0;

	gridX = nX;
	gridY = nY;

	minY = maxY = -1.0f;

	master = nMaster;
	realSelfTes = newRealSelfTes = newSelfTes = selfTes = leftTes = rightTes = topTes = bottomTes = 5;
	oldRealSelfTes = 5;

	vertices = NULL;
	indices = NULL;
	indexMap = NULL;

	next = prev = NULL;
	left = right = bottom = top = NULL;

	lowVertMethod2 = false;
	currentError = -1.0f;
}


void
pgTerrainPatchBase::setNeighbors(const pgTerrainPatchBase* nLeft, const pgTerrainPatchBase* nRight, const pgTerrainPatchBase *nBottom, const pgTerrainPatchBase* nTop)
{
	left = nLeft;
	right = nRight;
	bottom = nBottom;
	top = nTop;
}


void
pgTerrainPatchBase::init()
{
	newSelfTes = selfTes = leftTes = rightTes = topTes = bottomTes = 5;
	oldRealSelfTes = 5;

	vertices = new PATCHVERTEX[MAX_VERTICES];
	indices = new unsigned short[MAX_INDICES];
	indexMap = new unsigned short[MAX_VERTICES];

	numIndices = numVertices = 0;
	numNewVertices = numNewIndices = 0;

	forceBufferCreate = true;

	active = true;
	addToActiveList();
}


void
pgTerrainPatchBase::deinit()
{
	if(vertices)
		delete vertices;
	vertices = NULL;

	if(indices)
		delete indices;
	indices = NULL;

	if(indexMap)
		delete indexMap;
	indexMap = NULL;

	currentError = -1.0f;

	active = false;
	removeFromActiveList();
}


void
pgTerrainPatchBase::addToActiveList()
{
	assert(master);
	assert(prev==NULL);
	assert(next==NULL);

	prev = NULL;
	next = master->getActiveList();
	if(next)
		next->prev = this;
	master->getActiveList() = this;
}


void
pgTerrainPatchBase::removeFromActiveList()
{
	if(prev)
		prev->next = next;

	if(next)
		next->prev = prev;

	if(master->getActiveList() == this)
	{
		assert(prev==NULL);
		master->getActiveList() = next;
	}

	prev = next = NULL;
}


void
pgTerrainPatchBase::deleteDeviceObjects()
{
}


bool
pgTerrainPatchBase::restoreDeviceObjects()
{
	forceBufferCreate = true;
	return true;
}


int
pgTerrainPatchBase::fillBaseVertices(pgTerrainPatchBase::PATCHVERTEX* nDest)
{
	memcpy(nDest, vertices, numVertices*sizeof(pgTerrainPatchBase::PATCHVERTEX));
	return numVertices;
}


void
pgTerrainPatchBase::setScale(const pgVec3& nScale)
{
	scale = nScale;
}


void
pgTerrainPatchBase::setHeightMapPosition(int nX0, int nY0)
{
	hmX0 = nX0;
	hmY0 = nY0;
}


void
pgTerrainPatchBase::setRealTessellation(int nTes)
{
	oldRealSelfTes = realSelfTes;
	newRealSelfTes = /*newSelfTes =*/ nTes;
}


bool
pgTerrainPatchBase::updateTessellation()
{
	bool	ret = false;
	int	lT = left->getNewTessellationSafe(),
		rT = right->getNewTessellationSafe(),
		bT = bottom->getNewTessellationSafe(),
		tT = top->getNewTessellationSafe();

	//oldRealSelfTes = realSelfTes;

	//bool lReTes = leftTes!=lT && lT>selfTes;

	if(forceBufferCreate || newRealSelfTes!=realSelfTes || newSelfTes!=selfTes || leftTes!=lT || rightTes!=rT || bottomTes!=bT || topTes!=tT)
	{
		realSelfTes = newRealSelfTes;
		selfTes = newSelfTes;
		leftTes = lT;
		rightTes = rT;
		bottomTes = bT;
		topTes = tT;

		//selfTes = 1;
		//leftTes = rightTes = topTes = bottomTes = 2;

		assert(selfTes-lT<=1);
		assert(selfTes-rT<=1);
		assert(selfTes-bT<=1);
		assert(selfTes-tT<=1);

		createTessellation(selfTes, leftTes, rightTes, bottomTes, topTes);

		forceBufferCreate = false;

		//numVertices = numNewVertices;
		//numIndices = numNewIndices;
		ret = true;
	}

	float	er0 = errors[realSelfTes].diff,
			er1 = errors[realSelfTes+1].diff,
			maxErr = master->getMaxError();

	if(realSelfTes>=MAX_SUBDIV)
		er1 = 2.0f*er0;

	currentError = (maxErr-er0)/(er1-er0);

	if(realSelfTes>=4 && currentError>1.0f)
		currentError = 1.0f;

	assert(currentError<=1.0f);

	return ret;
}


bool
pgTerrainPatchBase::updateTessellation2()
{
	// if we want to do geo-morphing we need to create a second
	// vertex array which contains the same amount of vertices, but
	// looking as if they where one tessellation stage higher
	// since some tessellation methods might need results of neighbors
	// this is done in updateTessellation2+3() 

	return false;
}


bool
pgTerrainPatchBase::updateTessellation3()
{
	// if we want to do geo-morphing we need to create a second
	// vertex array which contains the same amount of vertices, but
	// looking as if they where one tessellation stage higher
	// since some tessellation methods might need results of neighbors
	// this is done in updateTessellation2+3() 

	return false;
}


// check if the patch is visible on the screen (clipping)
//
bool
pgTerrainPatchBase::checkVisibility(const D3DXMATRIX* nMatrix, const pgPlane nPlanes[6])
{
	updateVisibility(nMatrix, nPlanes);
	setVisible(visible);

	return visible;
}


void
pgTerrainPatchBase::setVisible(bool nVis)
{
	visible = nVis;

	if(visible)
	{
		flagActive = true;
		if(!active)
			init();
	}
	else if(active)
		deinit();
}


// visibility testing is done by checking all 8 corners of the bounding box
// against all six sides of the viewing frustum
void
pgTerrainPatchBase::updateVisibility(const D3DXMATRIX* nMatrix, const pgPlane nPlanes[6])
{
	D3DXVECTOR3	pt0,pt1,pt2,pt3,pt4,pt5,pt6,pt7, tpt0,tpt1,tpt2,tpt3,tpt4,tpt5,tpt6,tpt7, tp0,tp1,tp2,tp3;
	const D3DXMATRIX&	mat = *nMatrix;

	//visible = true;
	//return;

	// create and transform the cubes corners vertices
	//
	pt0.x = pos.x;				pt0.y = pos.y + minY;		pt0.z = pos.z;
	pt1.x = pos.x + scale.x;	pt1.y = pos.y + minY;		pt1.z = pos.z;
	pt2.x = pos.x;				pt2.y = pos.y + minY;		pt2.z = pos.z + scale.z;
	pt3.x = pos.x + scale.x;	pt3.y = pos.y + minY;		pt3.z = pos.z + scale.z;
	pt4.x = pt0.x;				pt4.y = pt0.y + maxY;		pt4.z = pt0.z;
	pt5.x = pt1.x;				pt5.y = pt1.y + maxY;		pt5.z = pt1.z;
	pt6.x = pt2.x;				pt6.y = pt2.y + maxY;		pt6.z = pt2.z;
	pt7.x = pt3.x;				pt7.y = pt3.y + maxY;		pt7.z = pt3.z;

	D3DXVec3TransformCoord(&tpt0, &pt0, &mat);
	D3DXVec3TransformCoord(&tpt1, &pt1, &mat);
	D3DXVec3TransformCoord(&tpt2, &pt2, &mat);
	D3DXVec3TransformCoord(&tpt3, &pt3, &mat);
	D3DXVec3TransformCoord(&tpt4, &pt4, &mat);
	D3DXVec3TransformCoord(&tpt5, &pt5, &mat);
	D3DXVec3TransformCoord(&tpt6, &pt6, &mat);
	D3DXVec3TransformCoord(&tpt7, &pt7, &mat);


	// check against all six planes
	//
	visible = true;

	for(int i=0; i<6; i++)
	{
		const pgPlane& p = nPlanes[i];
		bool v = false;

		if(tpt0.x*p.x + tpt0.y*p.y + tpt0.z*p.z < p.d)
			v = true;
		else
		if(tpt1.x*p.x + tpt1.y*p.y + tpt1.z*p.z < p.d)
			v = true;
		else
		if(tpt2.x*p.x + tpt2.y*p.y + tpt2.z*p.z < p.d)
			v = true;
		else
		if(tpt3.x*p.x + tpt3.y*p.y + tpt3.z*p.z < p.d)
			v = true;
		else
		if(tpt4.x*p.x + tpt4.y*p.y + tpt4.z*p.z < p.d)
			v = true;
		else
		if(tpt5.x*p.x + tpt5.y*p.y + tpt5.z*p.z < p.d)
			v = true;
		else
		if(tpt6.x*p.x + tpt6.y*p.y + tpt6.z*p.z < p.d)
			v = true;
		else
		if(tpt7.x*p.x + tpt7.y*p.y + tpt7.z*p.z < p.d)
			v = true;

		if(v==false)
		{
			visible = false;
			return;
		}
	}
}


void
pgTerrainPatchBase::calcMinMaxY()
{
	pgVec3 p;

	minY = scale.y;
	maxY = 0.0f;

	for(int x=0; x<SOURCE_WIDTH; x++)
		for(int y=0; y<SOURCE_HEIGHT; y++)
		{
			getVertex(x,y, p);

			if(p.y<minY)
				minY = p.y;
			if(p.y>maxY)
				maxY = p.y;
		}
}


void
pgTerrainPatchBase::updateProjectedErrors(const D3DXMATRIX* nMatrix)
{
	D3DXVECTOR3	corr0, real0, corr, real;

	setD3DVec(corr0, errors[1].correct);
	D3DXVec3TransformCoord(&corr, &corr0, nMatrix);
	assert(corr.z>0.0f);
	if(corr.z<0.0f)
		corr.z = 0.0f;


	for(int i=1; i<=MAX_SUBDIV; i++)
	{
		setD3DVec(real0, errors[i].real);
		D3DXVec3TransformCoord(&real, &real0, nMatrix);
		assert(real.z>0.0f);

		if(real.z<0.0f)
			real.z = 0.0f;

		pgVec3 diff(corr.x-real.x, corr.y-real.y, corr.z-real.z);
		errors[i].diff = diff.length();

		if(errors[i].diff<errors[i-1].diff*1.1f)
			errors[i].diff = errors[i-1].diff*1.1f;
	}
}


void
pgTerrainPatchBase::calcErrors()
{
	for(int i=0; i<=MAX_SUBDIV; i++)
		calcError(i);
}


void
pgTerrainPatchBase::calcError(int nTes)
{
	float	sumError = 0.0f;
	int		numErrors = 0;

	if(nTes!=0)
	{
		int pow = getPowerTwo(nTes),
			x0,y0, xi,yi;

		for(y0=0; y0<SOURCE_HEIGHT-pow; y0+=pow)
			for(x0=0; x0<SOURCE_WIDTH-pow; x0+=pow)
				for(yi=1; yi<pow; yi++)
					for(xi=1; xi<pow; xi++)
					{
						int		x = x0+xi,
								y = y0+yi;
						float	fx0 = (float)xi/(float)pow, fx1 = 1.0f-fx0,
								fy0 = (float)yi/(float)pow, fy1 = 1.0f-fy0;

						float	height00 = getHeight(x0,y0),
								height10 = getHeight(x0+pow,y0),
								height01 = getHeight(x0,y0+pow),
								height11 = getHeight(x0+pow,y0+pow);

						float	paintHeight =	fx1*fy1 * height00 +
												fx0*fy1 * height10 +
												fx1*fy0 * height01 +
												fx0*fy0 * height11,
								correctHeight =  getHeight(x,y);

						float	er = (float)fabs(correctHeight - paintHeight);

						numErrors++;
						sumError += er;
					}

		float error = sumError / numErrors;
		getVertex(SOURCE_WIDTH/2, SOURCE_HEIGHT/2, errors[nTes].correct);
		errors[nTes].real = errors[nTes].correct;
		errors[nTes].real.y += error;
		errors[nTes].diff = error;
	}
}


float
pgTerrainPatchBase::getExactTessellation() const
{
	if(realSelfTes<0)
		return -1.0f;

	if(currentError<0.0f)
		return (float)realSelfTes;
	else
		return (float)realSelfTes + currentError;
}


int
pgTerrainPatchBase::getPowerTwo(int nVal)
{
	switch(nVal)
	{
	case 0:
	default:
		return 1;
	case 1:
		return 2;
	case 2:
		return 4;
	case 3:
		return 8;
	case 4:
		return 16;
	case 5:
		return 32;
	case 6:
		return 64;
	}
}


void
pgTerrainPatchBase::createTessellation(int nCenter, int nLeft, int nRight, int nBottom, int nTop)
{
	assert(indexMap);
	assert(vertices);
	assert(indices);

	memset(indexMap, 0xff, MAX_VERTICES*sizeof(unsigned short));
	numNewVertices = numNewIndices = 0;

	int pow = getPowerTwo(nCenter);

	assert(nCenter-nLeft<=1);
	assert(nCenter-nRight<=1);
	assert(nCenter-nBottom<=1);
	assert(nCenter-nTop<=1);

	bool bLeft(nLeft<nCenter), bRight(nRight<nCenter), bBottom(nBottom<nCenter), bTop(nTop<nCenter);

	if(pow==16)
		addLevel4PatchTriangles(bLeft, bRight, bBottom, bTop);
	else
		for(int y=0; y<SOURCE_HEIGHT-pow; y+=pow)
			addTriangleRow(y, pow, bLeft, bRight, bBottom, bTop);


	assert((numNewIndices&1) == 0);		// indices must always be an even number (we store quads as two triangles)

	if(realSelfTes>selfTes)
		reduceShapeTo(realSelfTes, vertices);

	makeBordersSimpler(vertices);
}


/****************************************************************************************\
*                                                                                        *
*                                                                                        *
*                                Tessellation Sub Methods                                 *
*                                                                                        *
*                                                                                        *
\****************************************************************************************/


// calls makeSimpler() sequentially to make a mesh with
// selfTes<nTes to look like being created with nTes
void
pgTerrainPatchBase::reduceShapeTo(int nTes, PATCHVERTEX *nData)
{
	for(int t=nTes; t>0; t--)
		if(selfTes<t)
			makeSimpler(t, nData);
}


void
pgTerrainPatchBase::makeBordersSimpler(PATCHVERTEX *nData)
{
	int sT = selfTes;
	int x,y, idx,idx0,idx1, pow,
		lDiff = sT-leftTes,
		rDiff = sT-rightTes,
		bDiff = sT-bottomTes,
		tDiff = sT-topTes;


	if((lDiff==1 || lDiff==-1) && left)
	{
		pow = getPowerTwo(min(leftTes,sT));

		for(y=pow; y<SOURCE_HEIGHT-1; y+=2*pow)
		{
			idx = indexMap[0 + y*SOURCE_WIDTH];
			idx0 = indexMap[0 + (y-pow)*SOURCE_WIDTH];
			idx1 = indexMap[0 + (y+pow)*SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idx0!=0xffff);
			assert(idx1!=0xffff);

			nData[idx].pos.y = (nData[idx0].pos.y + nData[idx1].pos.y)/2.0f;
		}
	}

	if((rDiff==1 || rDiff==-1) && right)
	{
		pow = getPowerTwo(min(rightTes,sT));

		for(y=pow; y<SOURCE_HEIGHT-1; y+=2*pow)
		{
			idx = indexMap[SOURCE_WIDTH-1 + y*SOURCE_WIDTH];
			idx0 = indexMap[SOURCE_WIDTH-1 + (y-pow)*SOURCE_WIDTH];
			idx1 = indexMap[SOURCE_WIDTH-1 + (y+pow)*SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idx0!=0xffff);
			assert(idx1!=0xffff);

			nData[idx].pos.y = (nData[idx0].pos.y + nData[idx1].pos.y)/2.0f;
		}
	}

	if((bDiff==1 || bDiff==-1) && bottom)
	{
		pow = getPowerTwo(min(bottomTes, sT));

		for(x=pow; x<SOURCE_WIDTH-1; x+=2*pow)
		{
			idx = indexMap[x + 0*SOURCE_WIDTH];
			idx0 = indexMap[x-pow + 0*SOURCE_WIDTH];
			idx1 = indexMap[x+pow + 0*SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idx0!=0xffff);
			assert(idx1!=0xffff);

			nData[idx].pos.y = (nData[idx0].pos.y + nData[idx1].pos.y)/2.0f;
		}
	}

	if((tDiff==1 || tDiff==-1) && top)
	{
		pow = getPowerTwo(min(topTes,sT));

		for(x=pow; x<SOURCE_WIDTH-1; x+=2*pow)
		{
			idx = indexMap[x + (SOURCE_HEIGHT-1)*SOURCE_WIDTH];
			idx0 = indexMap[x-pow + (SOURCE_HEIGHT-1)*SOURCE_WIDTH];
			idx1 = indexMap[x+pow + (SOURCE_HEIGHT-1)*SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idx0!=0xffff);
			assert(idx1!=0xffff);

			nData[idx].pos.y = (nData[idx0].pos.y + nData[idx1].pos.y)/2.0f;
		}
	}
}


// this methods makes the mesh looking simpler by
// putting vertices into position inbetween its neighbors
// this method can do only for one step at a time
// (to make a tes_1 mesh look like tes_3 first call
// makeSimpler(3,...) then makeSimpler(2,...)
// Call reduceShapeTo() which cares about those constraints
// automatically
void
pgTerrainPatchBase::makeSimpler(int nTes, PATCHVERTEX *nData)
{
	assert(selfTes<nTes);
	assert(nTes>0);

	int pow = getPowerTwo(nTes-1),
		x,y, xc,yc;

	for(yc=y=0; y<SOURCE_HEIGHT; y+=pow,yc++)
		for(xc=x=0; x<SOURCE_WIDTH; x+=pow,xc++)
			calculateInbetweenVertex(x, y, xc, yc, pow, nData);

}


void
pgTerrainPatchBase::calculateInbetweenVertex(int x, int y, int xc, int yc, int pow, PATCHVERTEX *nData)
{
	float	heightLeftTop=0.0f, heightRightBottom=0.0f;
	int		idx, idxLT,idxRB;

	idx = calculateDiagonalVertices(x, y, xc, yc, pow, nData, heightLeftTop, idxLT, heightRightBottom, idxRB);

	if(idx>=0)
		nData[idx].pos.y = (heightLeftTop + heightRightBottom) / 2.0f;
}


int
pgTerrainPatchBase::calculateDiagonalVertices(int x, int y, int xc, int yc, int pow, PATCHVERTEX *nData,
											  float& nHeightLeftTop, int& idxLT, float& nHeightRightBottom, int& idxRB)
{
	int		c = (xc&1) + (yc&1)*2,
			idx = indexMap[x + y*SOURCE_WIDTH];

	assert(idx!=0xffff);

	switch(c)
	{
	case 0:			// even-x & even-y
		// nothing to do...
		return -1;

	case 1:			// odd-x & even-y
		assert(x>0 && x<SOURCE_WIDTH-1);

		idxLT = indexMap[x-pow + y*SOURCE_WIDTH];
		idxRB = indexMap[x+pow + y*SOURCE_WIDTH];

		assert(idxLT!=0xffff);
		assert(idxRB!=0xffff);

		nHeightLeftTop = nData[idxLT].pos.y;
		nHeightRightBottom = nData[idxRB].pos.y;
		return idx;

	case 2:			// even-x & odd-y
		assert(y>0 && y<SOURCE_HEIGHT-1);

		idxLT = indexMap[x + (y+pow)*SOURCE_WIDTH];
		idxRB = indexMap[x + (y-pow)*SOURCE_WIDTH];

		assert(idxLT!=0xffff);
		assert(idxRB!=0xffff);

		nHeightLeftTop = nData[idxLT].pos.y;
		nHeightRightBottom = nData[idxRB].pos.y;
		return idx;

	case 3:			// odd-x & odd-y
		assert(x>0 && x<SOURCE_WIDTH-1);
		assert(y>0 && y<SOURCE_HEIGHT-1);

		idxLT = indexMap[x-pow + (y+pow)*SOURCE_WIDTH];
		idxRB = indexMap[x+pow + (y-pow)*SOURCE_WIDTH];

		assert(idxLT!=0xffff);
		assert(idxRB!=0xffff);

		nHeightLeftTop = nData[idxLT].pos.y;
		nHeightRightBottom = nData[idxRB].pos.y;
		return idx;

	default:
		assert(false);	// should never come here...
		return -1;
	}

	//nData[idx].pos.y = (y0+y1)/2.0f;
}


void
pgTerrainPatchBase::addTriangleRow(int nY, int nPow, bool nLeft, bool nRight, bool nBottom, bool nTop)
{
	int powH = nPow/2;


	// special case: bottom line with nBottom
	//
	if(nY==0 && nBottom)
	{
		addTriangleBottomRow(nPow, nLeft, nRight);
		return;
	}

	if(nY!=0)
	{
		addLastIndexAgain();
		addIndex(getIndex(0,nY));
	}

	// special case: top line with nTop
	//
	if(nY>=SOURCE_HEIGHT-2*nPow && nTop)
	{
		addTriangleTopRow(nPow, nLeft, nRight);
		return;
	}


	// all other cases
	//
	int x, x0=0, x1=SOURCE_WIDTH;

	if(nLeft)
	{
		assert(powH>=1);

		addIndex(getIndex(0,nY));
		addIndex(getIndex(0,nY+powH));
		addIndex(getIndex(nPow,nY));
		addIndex(getIndex(0,nY+nPow));
		addIndex(getIndex(nPow,nY+nPow));
		addLastIndexAgain();							// finalize sub-strip
		x0 = nPow;
	}

	if(nRight)
		x1 -= nPow;

	for(x=x0; x<x1; x+=nPow)
	{
		addIndex(getIndex(x,nY));
		addIndex(getIndex(x,nY+nPow));
	}

	if(nRight)
	{
		assert(powH>=1);

		addIndex(getIndex(SOURCE_WIDTH-1-nPow,nY));
		addLastIndexAgain();							// reverse oriented triangles
		addIndex(getIndex(SOURCE_WIDTH-1,nY));
		addIndex(getIndex(SOURCE_WIDTH-1-nPow,nY+nPow));
		addIndex(getIndex(SOURCE_WIDTH-1,nY+powH));
		addIndex(getIndex(SOURCE_WIDTH-1,nY+nPow));
	}
}


void
pgTerrainPatchBase::addTriangleBottomRow(int nPow, bool nLeft, bool nRight)
{
	int powH = nPow/2;
	int x0=0, x1=SOURCE_WIDTH-1;

	assert(powH>=1);

	if(nLeft)
	{
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(powH,0));
		addIndex(getIndex(0,nPow));
		addIndex(getIndex(nPow,0));
		addIndex(getIndex(nPow,nPow));
		x0 = nPow;
	}

	assert((numNewIndices&1) == 0);

	if(nRight)
		x1 -= nPow;

	for(int x=x0; x<x1; x+=nPow)
	{
		addIndex(getIndex(x,0));
		addLastIndexAgain();						// reverse oriented triangles
		addIndex(getIndex(x+powH,0));
		addIndex(getIndex(x,nPow));
		addIndex(getIndex(x+nPow,0));
		addIndex(getIndex(x+nPow,nPow));
	}

	assert((numNewIndices&1) == 0);

	if(nRight)
	{
		addIndex(getIndex(SOURCE_WIDTH-1-nPow,0));
		addIndex(getIndex(SOURCE_WIDTH-1-nPow,nPow));
		addIndex(getIndex(SOURCE_WIDTH-1-powH,0));
		addIndex(getIndex(SOURCE_WIDTH-1,0));
		addLastIndexAgain();							// finalize sub-strip
		addIndex(getIndex(SOURCE_WIDTH-1-nPow,nPow));
		addIndex(getIndex(SOURCE_WIDTH-1,powH));
		addIndex(getIndex(SOURCE_WIDTH-1,nPow));
	}


	assert((numNewIndices&1) == 0);
}


void
pgTerrainPatchBase::addTriangleTopRow(int nPow, bool nLeft, bool nRight)
{
	int powH = nPow/2;
	int x0=0, x1=SOURCE_WIDTH-1;

	assert(powH>=1);

	if(nLeft)
	{
		addIndex(getIndex(0,SOURCE_HEIGHT-1-nPow));
		addIndex(getIndex(0,SOURCE_HEIGHT-1-powH));
		addIndex(getIndex(nPow,SOURCE_HEIGHT-1-nPow));
		addIndex(getIndex(0,SOURCE_HEIGHT-1));
		addIndex(getIndex(powH,SOURCE_HEIGHT-1));
		addLastIndexAgain();						// reverse oriented triangles
		addIndex(getIndex(nPow,SOURCE_HEIGHT-1-nPow));
		addIndex(getIndex(nPow,SOURCE_HEIGHT-1));
		x0 = nPow;
	}

	assert((numNewIndices&1) == 0);

	if(nRight)
		x1 -= nPow;

	for(int x=x0; x<x1; x+=nPow)
	{
		addIndex(getIndex(x,SOURCE_HEIGHT-1-nPow));
		addIndex(getIndex(x,SOURCE_HEIGHT-1));
		addIndex(getIndex(x+nPow,SOURCE_HEIGHT-1-nPow));
		addIndex(getIndex(x+powH,SOURCE_HEIGHT-1));
		addIndex(getIndex(x+nPow,SOURCE_HEIGHT-1));
		addLastIndexAgain();						// reverse oriented triangles
	}

	if(nRight)
	{
		addIndex(getIndex(x,SOURCE_HEIGHT-1-nPow));
		addIndex(getIndex(x,SOURCE_HEIGHT-1));
		addIndex(getIndex(x+nPow,SOURCE_HEIGHT-1-nPow));
		addIndex(getIndex(x+powH,SOURCE_HEIGHT-1));
		addIndex(getIndex(x+nPow,SOURCE_HEIGHT-1-powH));
		addIndex(getIndex(x+nPow,SOURCE_HEIGHT-1));
	}
}


void
pgTerrainPatchBase::addLevel4PatchTriangles(bool nLeft, bool nRight, bool nBottom, bool nTop)
{
	const int	powH = 8, pow = 16;
	int			code = (nLeft ? 1 : 0) | (nRight ? 2 : 0) | (nBottom ? 4 : 0) | (nTop ? 8 : 0);

	// use the created code for fast selection of the case to build...
	//
	switch(code)
	{
	case 0:
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(pow,pow));
		break;

	case 1:
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,pow));
		addLastIndexAgain();
		break;

	case 2:
		addIndex(getIndex(pow,pow));
		addIndex(getIndex(pow,powH));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,0));
		addLastIndexAgain();
		break;

	case 3:
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,powH));
		addIndex(getIndex(pow,pow));
		break;

	case 4:
		addIndex(getIndex(0,0));
		addLastIndexAgain();
		addIndex(getIndex(powH,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(pow,pow));
		break;

	case 5:
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(powH,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(pow,pow));
		break;

	case 6:
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(powH,0));
		addIndex(getIndex(pow,0));
		addLastIndexAgain();
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,powH));
		addIndex(getIndex(pow,pow));
		break;

	case 7:
		addIndex(getIndex(pow,pow));
		addIndex(getIndex(pow,powH));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(powH,0));
		addIndex(getIndex(0,0));
		addLastIndexAgain();
		break;

	case 8:
		addIndex(getIndex(pow,pow));
		addLastIndexAgain();
		addIndex(getIndex(powH,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(0,0));
		break;

	case 9:
		addIndex(getIndex(pow,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(powH,pow));
		addIndex(getIndex(0,pow));
		addLastIndexAgain();
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(0,0));
		break;

	case 10:
		addIndex(getIndex(pow,pow));
		addIndex(getIndex(pow,powH));
		addIndex(getIndex(powH,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(0,0));
		break;

	case 11:
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,powH));
		addIndex(getIndex(powH,pow));
		addIndex(getIndex(pow,pow));
		addLastIndexAgain();
		break;

	case 12:
		addIndex(getIndex(0,0));
		addLastIndexAgain();
		addIndex(getIndex(powH,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(powH,pow));
		addIndex(getIndex(pow,pow));
		addLastIndexAgain();
		break;

	case 13:
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(powH,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(powH,pow));
		addIndex(getIndex(pow,pow));
		addLastIndexAgain();
		break;

	case 14:
		addIndex(getIndex(pow,pow));
		addIndex(getIndex(pow,powH));
		addIndex(getIndex(powH,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(powH,0));
		addIndex(getIndex(0,0));
		addLastIndexAgain();
		break;

	case 15:
		addIndex(getIndex(0,0));
		addIndex(getIndex(0,powH));
		addIndex(getIndex(powH,0));
		addIndex(getIndex(0,pow));
		addIndex(getIndex(pow,0));
		addIndex(getIndex(powH,pow));
		addIndex(getIndex(pow,powH));
		addIndex(getIndex(pow,pow));
		break;
	}

	assert((numNewIndices&1) == 0);		// indices must always be an even number (we store quads as two triangles)
}


inline float
pgTerrainPatchBase::getHeight(int nX, int nY) const
{
	assert(nX>=0 && nX<SOURCE_WIDTH);
	assert(nY>=0 && nY<SOURCE_HEIGHT);
	assert(master);

	return pos.y + scale.y * master->getHeight(hmX0+nX, hmY0+nY); // - scale.y*0.5f;
}


inline void
pgTerrainPatchBase::getVertex(int nX, int nY, pgVec3& nPoint) const
{
	assert(nX>=0 && nX<SOURCE_WIDTH);
	assert(nY>=0 && nY<SOURCE_HEIGHT);

	nPoint.x = pos.x+scale.x*nX / (float)(SOURCE_WIDTH-1);
	nPoint.y = getHeight(nX,nY);
	nPoint.z = pos.z+scale.z*nY / (float)(SOURCE_HEIGHT-1);
}


inline void
pgTerrainPatchBase::getTexCoord(int nX, int nY, pgVec2& nTCoord) const
{
	assert(nX>=0 && nX<SOURCE_WIDTH);
	assert(nY>=0 && nY<SOURCE_HEIGHT);
	assert(master);

	master->getTexCoord(hmX0+nX, hmY0+nY, nTCoord);
}


inline void
pgTerrainPatchBase::getNormal(int nX, int nY, pgVec3& nNormal) const
{
	assert(nX>=0 && nX<SOURCE_WIDTH);
	assert(nY>=0 && nY<SOURCE_HEIGHT);
	assert(master);

	master->getNormal(hmX0+nX, hmY0+nY, nNormal);
}


unsigned short
pgTerrainPatchBase::getIndex(int nX, int nY)
{
	int idx = nX + nY * SOURCE_WIDTH;

	assert(nX>=0 && nX<SOURCE_WIDTH);
	assert(nY>=0 && nY<SOURCE_HEIGHT);
	assert(idx<MAX_VERTICES);

	if(indexMap[idx]==0xffff)
	{
		indexMap[idx] = (unsigned short)numNewVertices;

		getVertex(nX,nY, vertices[numNewVertices].pos);
		getTexCoord(nX,nY, vertices[numNewVertices].tex);
	
		numNewVertices++;
	}

	return indexMap[idx];
}


int
pgTerrainPatchBase::getNewTessellationSafe() const
{
	if(this==NULL)
		return MAX_SUBDIV;

	if(!isVisible())
		return  selfTes; // MAX_SUBDIV;

	return getNewTessellation();
}


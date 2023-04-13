///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrainPatch
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrainPatch.cpp,v 1.8 2002/12/22 10:51:32 daniel Exp $
///////////////////////////////////////////////////////////////////



#include "pgTerrainPatch.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/Math/pgPlane.h>
#include <pgLib/Render/Terrain/pgTerrain.h>

#include <assert.h>


#define setD3DVec(v1, v0)  {  v1.x = v0.x;  v1.y = v0.y;  v1.z = v0.z;  }


pgTerrainPatch::pgTerrainPatch(pgTerrain* nMaster, int nX, int nY) : pgTerrainPatchBase(nMaster, nX,nY)
{
	vBuffer = NULL;
	iBuffer = NULL;
}


void
pgTerrainPatch::init()
{
	pgTerrainPatchBase::init();
}


void
pgTerrainPatch::deinit()
{
	pgTerrainPatchBase::deinit();
	deleteDeviceObjects();
}


void
pgTerrainPatch::deleteDeviceObjects()
{
	SAFE_RELEASE(iBuffer);
	SAFE_RELEASE(vBuffer);
}


bool
pgTerrainPatch::restoreDeviceObjects()
{
	forceBufferCreate = true;
	return true;
}


bool
pgTerrainPatch::updateTessellation()
{
	if(pgTerrainPatchBase::updateTessellation())
		forceBufferCreate = true;

	return true;
}


bool
pgTerrainPatch::updateTessellation2()
{
	pgTerrainPatch* left = (pgTerrainPatch*)this->left;
	pgTerrainPatch* right = (pgTerrainPatch*)this->right;
	pgTerrainPatch* bottom = (pgTerrainPatch*)this->bottom;
	pgTerrainPatch* top = (pgTerrainPatch*)this->top;

	if(gridX==7 && gridY==5)
		gridX=7;

	if(forceBufferCreate || (left && left->forceBufferCreate)
						 || (right && right->forceBufferCreate)
						 || (bottom && bottom->forceBufferCreate)
						 || (top && top->forceBufferCreate))
	{
		bool updated = checkBorders();

		if(forceBufferCreate || updated)
		{
			bool fOld = forceBufferCreate;
			forceBufferCreate = false;
			fillBuffers();
			forceBufferCreate = fOld;
		}
	}

	return true;
}


bool
pgTerrainPatch::updateTessellation3()
{
	forceBufferCreate = false;
	return true;
}


int
pgTerrainPatch::render()
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


	// rendering is VERY fast: just a single
	// DrawIndexedPrimitive call for each patch
	//
	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, numVertices, 0, numIndices-2);
	return numIndices-2;
}


bool
pgTerrainPatch::checkBorders()
{
	// border correction is only necessary if the neighboring patch
	// looks coarser than it is (realSelfTes>selfTes)

	pgTerrainPatch* left = (pgTerrainPatch*)this->left;
	pgTerrainPatch* right = (pgTerrainPatch*)this->right;
	pgTerrainPatch* bottom = (pgTerrainPatch*)this->bottom;
	pgTerrainPatch* top = (pgTerrainPatch*)this->top;
	bool			ret = false;

	if(left && left->isActive() && (left->selfTes!=left->realSelfTes || left->realSelfTes!=left->oldRealSelfTes))
	{
		int y,pow = getPowerTwo(min(leftTes,selfTes));

		for(y=pow; y<SOURCE_HEIGHT-1; y+=pow)
		{
			int	idx = indexMap[0 + y*SOURCE_WIDTH],
				idxL = left->indexMap[SOURCE_WIDTH-1 + y*SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idxL!=0xffff);

			vertices[idx].pos.y = left->vertices[idxL].pos.y;
		}

		ret = true;
	}

	if(right && right->isActive() && (right->selfTes!=right->realSelfTes || right->realSelfTes!=right->oldRealSelfTes))
	{
		int y,pow = getPowerTwo(min(rightTes,selfTes));

		for(y=pow; y<SOURCE_HEIGHT-1; y+=pow)
		{
			int	idx = indexMap[SOURCE_WIDTH-1 + y*SOURCE_WIDTH],
				idxR = right->indexMap[0 + y*SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idxR!=0xffff);

			vertices[idx].pos.y = right->vertices[idxR].pos.y;
		}

		ret = true;
	}

	if(bottom && bottom->isActive() && (bottom->selfTes!=bottom->realSelfTes || bottom->realSelfTes!=bottom->oldRealSelfTes))
	{
		int x,pow = getPowerTwo(min(bottomTes,selfTes));

		for(x=pow; x<SOURCE_WIDTH-1; x+=pow)
		{
			int	idx = indexMap[x + 0*SOURCE_WIDTH],
				idxB = bottom->indexMap[x + (SOURCE_HEIGHT-1)*SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idxB!=0xffff);

			vertices[idx].pos.y = bottom->vertices[idxB].pos.y;
		}

		ret = true;
	}

	if(top && top->isActive() && (top->selfTes!=top->realSelfTes || top->realSelfTes!=top->oldRealSelfTes))
	{
		int x,pow = getPowerTwo(min(topTes,selfTes));

		for(x=pow; x<SOURCE_HEIGHT-1; x+=pow)
		{
			int	idx = indexMap[x + (SOURCE_HEIGHT-1)*SOURCE_WIDTH],
				idxT = top->indexMap[x + 0*SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idxT!=0xffff);

			vertices[idx].pos.y = top->vertices[idxT].pos.y;
		}

		ret = true;
	}

	return ret;
}


bool
pgTerrainPatch::fillBuffers()
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

	vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK);
	memcpy(pVertices, vertices, numVertices*sizeof(PATCHVERTEX));
	vBuffer->Unlock();


	// fill index buffer
	//
	WORD*			pIndices = NULL;

	iBuffer->Lock(0, 0, (BYTE**)&pIndices, D3DLOCK_NOSYSLOCK);
	memcpy(pIndices, indices, numIndices*sizeof(unsigned short));
	iBuffer->Unlock();

	return true;
}


bool
pgTerrainPatch::checkBuffers()
{
	// create new buffers if the size changed
	//
	if(numIndices!=numNewIndices || forceBufferCreate || iBuffer==NULL)
	{
		numIndices = numNewIndices;
		SAFE_RELEASE(iBuffer)

		if(numIndices>0)
		{
			if(FAILED(pd3dDevice->CreateIndexBuffer(numIndices * sizeof(WORD), D3DUSAGE_WRITEONLY,
													D3DFMT_INDEX16, pgIDirectX::getPoolType(), &iBuffer)))
			{
				pgLog::error("creating index buffer for terrainpatch failed");
				return false;
			}

			pgLog::trace("index-buffer (%d|%d) recreated\n", gridX,gridY);
		}
	}


	if(numVertices!=numNewVertices  || forceBufferCreate || vBuffer==NULL)
	{
		numVertices = numNewVertices;
		SAFE_RELEASE(vBuffer)

		if(numVertices>0)
		{
			if(FAILED(pd3dDevice->CreateVertexBuffer(numVertices * sizeof(PATCHVERTEX), D3DUSAGE_WRITEONLY,
													 D3DFVF_PATCHVERTEX, pgIDirectX::getPoolType(), &vBuffer)))
			{
				pgLog::error("creating vertex buffer for terrainpatch failed");
				SAFE_RELEASE(iBuffer);
				return false;
			}

			pgLog::trace("vertex-buffer (%d|%d) recreated\n", gridX,gridY);
		}
	}

	return true;
}


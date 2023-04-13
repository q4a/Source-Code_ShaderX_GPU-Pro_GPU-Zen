///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgMeshUtil
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMeshUtil.cpp,v 1.5 2002/12/18 22:00:37 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgMeshUtil.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgInTextFile.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Render/pgBaseMesh.h>

#include <stdio.h>


#define FILL_TRIANGLEINDICES( idx, idx0, idx1, idx2 )  \
{   (idx)[0] = idx0;  (idx)[1] = idx1;  (idx)[2] = idx2;  }


bool
pgMeshUtil::setDevice(pgBaseMesh* nMesh, const char* nError)
{
	if(!nMesh)
		return false;

	nMesh->pd3dDevice = pgIDirectX::getDevice();

	if(nMesh->pd3dDevice==NULL)
	{
		pgLog::error(nError);
		return false;
	}

	return true;
}


/***********************************************************************************\
*                                                                                   *
*                                                                                   *
*                                     Box Stuff                                     *
*                                                                                   *
*                                                                                   *
\***********************************************************************************/


pgBaseMesh*
pgMeshUtil::createBox(const pgAABBox& nBox, DWORD nColor)
{
	pgBaseMesh* mesh = new pgBaseMesh();
	mesh->vertexSettings = pgBaseMesh::SET_INDEXED | pgBaseMesh::SET_COLOR | pgBaseMesh::SET_TEXTURE1;

	if(!setDevice(mesh, "DirectX Device does not exist yet. cannot create box mesh"))
	{
		delete mesh;
		return NULL;
	}


	// create an index and a vertex buffer for 6*2 triangles
	//
	pgBaseMesh::Frame* frame = new pgBaseMesh::Frame();

	frame->numIndices = 36;					// 6 * 2 * 3 vertices
	frame->numVertices = 24;					// since we want to texture the box we need seperate vertices for each triangle (6*4)
	frame->numTriangles = frame->numIndices / 3;


	if(FAILED(mesh->pd3dDevice->CreateIndexBuffer(frame->numIndices * sizeof(WORD),
												  pgIDirectX::getDisableWriteOnlyBuffers() ? 0 : D3DUSAGE_WRITEONLY,
												  D3DFMT_INDEX16, pgIDirectX::getPoolType(), &(frame->iBuffer))))
	{
		pgLog::error("creating index buffer for pgBaseMesh (box) failed");
		delete mesh;
		delete frame;
		return NULL;
	}

	if(FAILED(mesh->pd3dDevice->CreateVertexBuffer(frame->numVertices * sizeof(pgBaseMesh::MESHVERTEX_COL1),
												   pgIDirectX::getDisableWriteOnlyBuffers() ? 0 : D3DUSAGE_WRITEONLY,
												   D3DFVF_MESHVERTEX_COL1, pgIDirectX::getPoolType(), &(frame->vBuffer))))
	{
		pgLog::error("creating vertex buffer for pgBaseMesh (box) failed");
		delete mesh;
		delete frame;
		SAFE_RELEASE(frame->iBuffer);
		return NULL;
	}

	//mesh->frames.addTail(frame);

	pgPtrList<pgBaseMesh::Frame>& frames = mesh->getFrames();
	frames.addTail(frame);

	updateBox(mesh, nBox, nColor);
	return mesh;
}


bool
pgMeshUtil::updateBox(pgBaseMesh* nMesh, const pgAABBox& nBox, DWORD nColor)
{
	pgPtrList<pgBaseMesh::Frame>& frames = nMesh->getFrames();

	// if there is not at least one frame,
	// further processing is useless...
	if(frames.getSize()<1)
	{
		pgLog::error("cannot update box: no frames in mesh");
		return false;
	}

	pgBaseMesh::Frame* frame = frames[0];


	// check if the index and vertex buffers have the correct size
	//
	if(frame->numIndices!=36 || frame->numVertices!=24)
	{
		pgLog::error("cannot update box: wrong buffer sizes (%d,%d)", frame->numIndices, frame->numVertices);
		return false;
	}


	// lock, fill & unlock the buffers
	//
	const pgVec3&				min = nBox.getMin(),
								max = nBox.getMax();
	pgBaseMesh::MESHVERTEX_COL1*	pVertices = NULL;
	WORD*						pIndices = NULL;

	if(!frame->iBuffer || !frame->vBuffer)
	{
		pgLog::error("cannot update box: no vertex or index buffer");
		return false;
	}

	frame->iBuffer->Lock(0, 0, (BYTE**)&pIndices, D3DLOCK_NOSYSLOCK);
	frame->vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK);

	// front side
	FILL_MESHVERTEX_COL1A(pVertices[0], min.x, min.y, min.z, nColor, 0.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[1], max.x, min.y, min.z, nColor, 1.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[2], max.x, max.y, min.z, nColor, 1.0f, 0.0f)
	FILL_MESHVERTEX_COL1A(pVertices[3], min.x, max.y, min.z, nColor, 0.0f, 0.0f)

	FILL_TRIANGLEINDICES(pIndices+0, 0,2,1)
	FILL_TRIANGLEINDICES(pIndices+3, 0,3,2)

	// right side
	FILL_MESHVERTEX_COL1A(pVertices[4], max.x, min.y, min.z, nColor, 0.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[5], max.x, min.y, max.z, nColor, 1.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[6], max.x, max.y, max.z, nColor, 1.0f, 0.0f)
	FILL_MESHVERTEX_COL1A(pVertices[7], max.x, max.y, min.z, nColor, 0.0f, 0.0f)

	FILL_TRIANGLEINDICES(pIndices+6, 4,6,5)
	FILL_TRIANGLEINDICES(pIndices+9, 4,7,6)

	// back side
	FILL_MESHVERTEX_COL1A(pVertices[8], max.x, min.y, max.z, nColor, 0.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[9], min.x, min.y, max.z, nColor, 1.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[10], min.x, max.y, max.z, nColor, 1.0f, 0.0f)
	FILL_MESHVERTEX_COL1A(pVertices[11], max.x, max.y, max.z, nColor, 0.0f, 0.0f)

	FILL_TRIANGLEINDICES(pIndices+12, 8,10,9)
	FILL_TRIANGLEINDICES(pIndices+15, 8,11,10)

	// left side
	FILL_MESHVERTEX_COL1A(pVertices[12], min.x, min.y, max.z, nColor, 0.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[13], min.x, min.y, min.z, nColor, 1.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[14], min.x, max.y, min.z, nColor, 1.0f, 0.0f)
	FILL_MESHVERTEX_COL1A(pVertices[15], min.x, max.y, max.z, nColor, 0.0f, 0.0f)

	FILL_TRIANGLEINDICES(pIndices+18, 12,14,13)
	FILL_TRIANGLEINDICES(pIndices+21, 12,15,14)

	// top side
	FILL_MESHVERTEX_COL1A(pVertices[16], min.x, max.y, min.z, nColor, 0.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[17], max.x, max.y, min.z, nColor, 1.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[18], max.x, max.y, max.z, nColor, 1.0f, 0.0f)
	FILL_MESHVERTEX_COL1A(pVertices[19], min.x, max.y, max.z, nColor, 0.0f, 0.0f)

	FILL_TRIANGLEINDICES(pIndices+24, 16,18,17)
	FILL_TRIANGLEINDICES(pIndices+27, 16,19,18)

	// bottom side
	FILL_MESHVERTEX_COL1A(pVertices[20], min.x, min.y, max.z, nColor, 0.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[21], max.x, min.y, max.z, nColor, 1.0f, 1.0f)
	FILL_MESHVERTEX_COL1A(pVertices[22], max.x, min.y, min.z, nColor, 1.0f, 0.0f)
	FILL_MESHVERTEX_COL1A(pVertices[23], min.x, min.y, min.z, nColor, 0.0f, 0.0f)

	FILL_TRIANGLEINDICES(pIndices+30, 20,22,21)
	FILL_TRIANGLEINDICES(pIndices+33, 20,23,22)

	frame->vBuffer->Unlock();
	frame->iBuffer->Unlock();

	return true;
}



/***********************************************************************************\
*                                                                                   *
*                                                                                   *
*                                 Quake2 MD2 Stuff                                  *
*                                                                                   *
*                                                                                   *
\***********************************************************************************/

// since this is quite a lot of code, it resides in an extra file...
//
#include <pgLib/Render/pgMeshUtil_MD2.cxx>



/***********************************************************************************\
*                                                                                   *
*                                                                                   *
*                                Wavefront OBJ Stuff                                *
*                                                                                   *
*                                                                                   *
\***********************************************************************************/

// since this is quite a lot of code, it resides in an extra file...
//
#include <pgLib/Render/pgMeshUtil_OBJ.cxx>



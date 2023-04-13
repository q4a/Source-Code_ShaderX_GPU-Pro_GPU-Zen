///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPMesh
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPMesh.cpp,v 1.2 2002/12/18 22:00:38 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgBSPMesh.h"

#include <pgLib/Render/BSP/pgBSPClasses.h>
#include <pgLib/Render/BSP/pgBSPReader.h>
#include <pgLib/Render/BSP/pgBSPTools.h>

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgInTextFile.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Base/pgITime.h>

#include <pgLib/Math/pgAABBox.h>
#include <pgLib/Math/pgIMathTool.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/Math/pgPlane.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgVec3n.h>

#include <assert.h>
#include <stdio.h>

#pragma warning(disable: 4514)

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

typedef bool *BoolPtr;
typedef char *CharPtr;
typedef int *IntPtr;
typedef IntPtr *IntPPtr;





void Brush::buildEdges()
{
	pgVec3	tCorners[128];
	int		tPlanes[128][3];
	int		tEdges[256][2];
	int		i,j,k;

	numCorners = 0;
	numEdges = 0;

	for(i=0; i<numSides; i++)
		for(j=i+1; j<numSides; j++)
			for(k=j+1; k<numSides; k++)
				if(pgIMathTool::findIntersectionPlanes(sides[i], sides[j], sides[k], tCorners[numCorners]))
					if(pgIMathTool::isPointBehindPlanes(tCorners[numCorners], numSides, sides))
					{
						tPlanes[numCorners][0] = i;
						tPlanes[numCorners][1] = j;
						tPlanes[numCorners][2] = k;
						numCorners++;
						assert(numCorners<128);
					}

	assert(numCorners<128);
	corners = new pgVec3[numCorners];
	for(i=0; i<numCorners; i++)
	{
		corners[i] = tCorners[i];
		bbox.addVertex(corners[i]);
	}

	for(i=0; i<numCorners; i++)
		for(j=i+1; j<numCorners; j++)
		{
			int	 *p0 = tPlanes[i],
				 *p1 = tPlanes[j];

			if(twoSame(p0[0], p0[1], p0[2], p1[0], p1[1], p1[2]))
			{
				tEdges[numEdges][0] = i;
				tEdges[numEdges][1] = j;
				numEdges++;
			}
		}

	assert(numEdges<256);
	edges = new unsigned short[2*numEdges];
	for(i=0; i<numEdges; i++)
	{
		edges[2*i]   = (unsigned short)tEdges[i][0];
		edges[2*i+1] = (unsigned short)tEdges[i][1];
	}
}


//////////////////////////////////////////////////////////////////////////////////////



pgBSPMesh::pgBSPMesh()
{
	numTextures = nupgightmaps = 0;
	textures = lightmaps = NULL;

	patchSubFact = 6;

	iBuffer = NULL;
	vBuffer = NULL;

	numShaders = 0;
	shaders = NULL;

	trisRendered = verticesRendered = matsRendered = 0;

	numBatches = 0;
	batches = NULL;
	numStripLens = 0;
	stripIndices = stripTris = NULL;

	numFaces = 0;
	faces = NULL;
	numMaterialFaces = NULL;
	maxMaterialFaces = NULL;
	materialFaces = NULL;

	numClusters = 0;
	numClusterLeafs = NULL;
	clusterLeafs = NULL;
	clusterVis = NULL;

	nupgeafs = 0;
	nupgeafFaces = NULL;
	leafFaces = NULL;

	numBrushes = 0;
	brushes = NULL;

	nodes = NULL;
	leafs = NULL;
	planes = NULL;

	curCluster = -1;

	/*SetCacheSize(CACHESIZE_GEFORCE3);
	SetStitchStrips(true);
	SetMinStripSize(0);
	SetListsOnly(false);*/
}


const char* pgBSPMesh::getReportString()
{
	//checkCollision(pgVec3(), pgIDirectX::getCameraPos());
	//bool check = checkCollision2(pgIDirectX::getCameraPos(), pgAABBox());
	sprintf(reportStr, "BSP: %d tris / %d verts / %d mats | c/l %d/%d %s", trisRendered, verticesRendered, matsRendered, curCluster, curLeaf,
																		   didCollide>0 ? "COL" : " ");
	if(didCollide>0)
		didCollide--;
	return reportStr;
}


void pgBSPMesh::deleteDeviceObjects()
{
	SAFE_RELEASE(vBuffer)
	SAFE_RELEASE(iBuffer)
}


bool pgBSPMesh::restoreDeviceObjects()
{
	deleteDeviceObjects();

	if(!checkDevice("cannot allocate buffers for bsp mesh: d3d device not set yet"))
		return false;


	if(FAILED(pd3dDevice->CreateIndexBuffer(2 * MAX_INDICES * sizeof(WORD), D3DUSAGE_DYNAMIC,
											D3DFMT_INDEX16, pgIDirectX::getPoolType(), &iBuffer)))		//D3DPOOL_DEFAULT, D3DPOOL_MANAGED
	{
		pgLog::error("creating index buffer for bsp failed");
		return false;
	}

	if(FAILED(pd3dDevice->CreateVertexBuffer(2 * MAX_VERTICES * sizeof(BSPVERTEX), D3DUSAGE_DYNAMIC,
											 D3DFVF_BSPVERTEX, pgIDirectX::getPoolType(), &vBuffer)))
	{
		pgLog::error("creating vertex buffer for bsp failed");
		return false;
	}

	return true;
}


/******************************************************************************************\
*                                                                                          *
*                                                                                          *
*                                      CREATION                                            *
*                                                                                          *
*                                                                                          *
\******************************************************************************************/


bool pgBSPMesh::load(const char* nBSPFileName)
{
	pgBSPReader	bspReader;
	int			leaf, cluster, dcluster, face, brush, vi;
	int			tex,lm;
	int			numTris = 0;
	char		fullName[_MAX_PATH];

	if(!checkDevice("can not create bsp mesh: device not set yet"))
		return false;

	pgISettings::extendToBSPPath(nBSPFileName, fullName);
	strcat(fullName, ".bsp");

	if(!bspReader.load(fullName))
		return false;

	pgISettings::extendToBSPPath(nBSPFileName, fullName);
	strcat(fullName, ".shader");
	loadShaders(fullName);

	// get the bsp data
	//
	nupgeafs = bspReader.lumpLeafs->getNupgeafs();
	const bsp_leaf* bLeafs = bspReader.lumpLeafs->getLeaf(0);

	//int bNupgeaffaces = bspReader.lumpLeaffaces->getNupgeaffaces();
	const int* bLeaffaces = bspReader.lumpLeaffaces->getLeaffaces();

	int bNumTex = bspReader.lumpTextures->getNumTextures();
	const bsp_texture* bTex = bspReader.lumpTextures->getTexture(0);

	//int bNumVert = bspReader.lumpVertices->getNumVertices();
	const bsp_vertex* bVert = bspReader.lumpVertices->getVertex(0);

	//int bNumMesVert = bspReader.lumpMeshvertices->getNumIndices();
	const int *bMeshVert = bspReader.lumpMeshvertices->getIndices();

	int bNumFaces = bspReader.lumpFaces->getNumFaces();
	const bsp_face* bFaces = bspReader.lumpFaces->getFace(0);

	int bNumBrushes = bspReader.lumpBrushes->getNumBrushes();
	const bsp_brush* bBrushes = bspReader.lumpBrushes->getBrushes();
	const bsp_brushside* bBrushSides = bspReader.lumpBrushsides->getBrushsides();

	int bNupgightmaps = bspReader.lumpLightmaps->getNupgightmaps();

	const bsp_visdata* bVis = const_cast<bsp_visdata*>(bspReader.lumpVisdata->getVisdata());


	nodes = bspReader.lumpNodes->duplicate();
	leafs = bspReader.lumpLeafs->duplicate();
	planes = bspReader.lumpPlanes->duplicate();
	leafbrushes = bspReader.lumpLeafbrushes->duplicate();

	//nupgeafs = nupgeafs;
	numTextures = bNumTex;
	nupgightmaps = bNupgightmaps;


	// create textures & lightmaps
	//
	if(!loadShaderTextures())
		return false;

	textures = new pgTexturePtr[numTextures];
	for(tex=0; tex<numTextures; tex++)
	{
		Shader* s = getShader(bTex[tex].name);

		if(s!=NULL)
		{
			textures[tex] = s->imgTextures[0];
			s->tex = tex;
		}
		else
		if((textures[tex]=pgIResourceManager::getTexture(bTex[tex].name, pgIResourceManager::SOURCE_BSP)) == NULL)
		{
			pgLog::error("backup texture not found -> quiting bsp creation");
			return false;
		}
	}


	lightmaps = new pgTexturePtr[nupgightmaps];
	for(lm=0; lm<nupgightmaps; lm++)
		if((lightmaps[lm]=createLightmap(bspReader.lumpLightmaps->getLightmap(lm))) == NULL)
		{
			pgLog::error("creating lightmap %d failed -> quiting bsp creation", lm);
			return false;
		}

	numBatches = 0;
	batches = new Batch[numTextures*(nupgightmaps+1)];

	// count the clusters
	//
	numClusters = 0;
	for(leaf=0; leaf<nupgeafs; leaf++)
		if(bLeafs[leaf].cluster>numClusters)
			numClusters = bLeafs[leaf].cluster;
	numClusters++;


	// save which faces are in which leaf
	//
	nupgeafFaces = new int[nupgeafs];
	leafFaces = new IntPtr[nupgeafs];

	for(leaf=0; leaf<nupgeafs; leaf++)
	{
		nupgeafFaces[leaf] = bLeafs[leaf].n_leaffaces;
		leafFaces[leaf] = new int[nupgeafFaces[leaf]];

		int face0 = bLeafs[leaf].leafface;

		for(face=0; face<nupgeafFaces[leaf]; face++)
			leafFaces[leaf][face] = bLeaffaces[face0 + face];
	}


	// save which leafs are in which clusters
	// step one: allocate memory
	//
	clusterLeafs = new IntPtr[numClusters];
	numClusterLeafs = new int[numClusters];
	for(cluster=0; cluster<numClusters; cluster++)
		numClusterLeafs[cluster] = 0;

	for(leaf=0; leaf<nupgeafs; leaf++)
		if(bLeafs[leaf].cluster>=0)
			numClusterLeafs[bLeafs[leaf].cluster]++;

	for(cluster=0; cluster<numClusters; cluster++)
		clusterLeafs[cluster] = new int[numClusterLeafs[cluster]];

	for(cluster=0; cluster<numClusters; cluster++)
		numClusterLeafs[cluster] = 0;

	// step two: write the leaf indices into the cluster lists
	//
	for(leaf=0; leaf<nupgeafs; leaf++)
	{
		cluster = bLeafs[leaf].cluster;

		if(cluster>=0)
			clusterLeafs[cluster][numClusterLeafs[cluster]++] = leaf;
	}


	// save which cluster is visible from which one
	//
	if(bVis->n_vecs>0 && bVis->sz_vecs>0 && bVis->vecs!=NULL)
	{
		int sz_vecs = bVis->sz_vecs;
		const unsigned char* vecs = bVis->vecs;
		clusterVis = new BoolPtr[numClusters];

		for(cluster=0; cluster<numClusters; cluster++)
		{
			int cb = (1<<(cluster%8)),
				c8 = cluster/8;
			clusterVis[cluster] = new bool[numClusters];

			for(dcluster=0; dcluster<numClusters; dcluster++)
				clusterVis[cluster][dcluster] = (vecs[dcluster*sz_vecs + c8] & cb) != NULL;
		}
	}


	// create index and vertex buffers for rendering
	// 
	if(!restoreDeviceObjects())
		return false;


	// setup the facelist and prepare it
	// for rendering
	numFaces = bNumFaces;
	faces = new Face[numFaces];
	BSPVERTEX* vertices = new BSPVERTEX[MAX_VERTICES];
	WORD* indices = new WORD[MAX_INDICES];

	for(face=0; face<numFaces; face++)
	{
		const bsp_face* bFace = bFaces+face;
		int nv=0, ni=0; //, numVertices=0;
			
		faces[face].type = bFace->type;
		faces[face].texture = bFace->texture;
		faces[face].lightmap = (bNupgightmaps==0) ? -1 : bFace->lm_index;
		
		switch(bFace->type)
		{
		case 1:	// polygon
		case 3:	// mesh
			triangulatePoly(bFace, bVert, bMeshVert, indices, vertices, ni, nv);
			break;

		case 2:	// patch
			triangulatePatch(bFace, bVert, indices, vertices, ni, nv, patchSubFact);
			break;

		case 4:	// billboard
			assert(false);
			break;
		}

		assert(ni<MAX_INDICES && nv<MAX_VERTICES);
		faces[face].numIndices = ni;
		faces[face].numVertices = nv;
		faces[face].numTris = ni/3;
		faces[face].indices = new WORD[ni];
		faces[face].vertices = new BSPVERTEX[nv];
		memcpy(faces[face].indices, indices, ni*sizeof(WORD));
		memcpy(faces[face].vertices, vertices, nv*sizeof(BSPVERTEX));
		numTris += ni/3;

		// build collision data
		//
		switch(bFace->type)
		{
		case 1:	// polygon					in this case we just
		case 3:	// mesh						take the normal vertices
			faces[face].numColIndices = faces[face].numIndices;
			faces[face].colIndices = faces[face].indices;						// point to the same indices -> keep care not to delete twice !!!

			faces[face].numColVertices = nv;
			faces[face].colVertices = new pgVec3[nv];
			for(vi=0; vi<nv; vi++)
				faces[face].colVertices[vi].set((const float*)vertices[vi].p);
			break;

		case 2:	// patch
			triangulatePatch(bFace, bVert, indices, vertices, ni, nv, 2);		// retriangulate the patch in a low resolution (enough for coll.det.)

			faces[face].numColIndices = ni;
			faces[face].colIndices = new WORD[ni];
			memcpy(faces[face].colIndices, indices, ni*sizeof(WORD));

			faces[face].numColVertices = nv;
			faces[face].colVertices = new pgVec3[nv];
			for(vi=0; vi<nv; vi++)
				faces[face].colVertices[vi].set((const float*)vertices[vi].p);
			break;
		}

		// build a bounding box for each face
		// (takes some additional memory but speeds up collision a lot...)
		for(vi=0; vi<faces[face].numColVertices; vi++)
			faces[face].bbox.addVertex(faces[face].colVertices[vi]);


		// create tristrips with the nvidia library
		//
		/*PrimitiveGroup *groups, *rGroups;
		DWORD numGroups;

		GenerateStrips(indices, ni, &groups, &numGroups);
		assert(numGroups==1);
		assert(groups[0].type==PT_STRIP);

		//RemapIndices(groups, numGroups, nv, &rGroups);

		//for(unsigned short idx=0; idx<rGroups[0].numIndices; idx++)
		//	faces[face].vertices[rGroups[0].indices[idx]] = vertices[groups[0].indices[idx]];

		//faces[face].stripIndices = rGroups[0].indices;
		//faces[face].numStripIndices = rGroups[0].numIndices;



		faces[face].stripIndices = groups[0].indices;
		faces[face].numStripIndices = groups[0].numIndices;*/
	}

	delete vertices;
	delete indices;


	// create a structure for fast material sorting
	//
	//first create the basic structure
	materialFaces = new IntPPtr[numTextures];
	numMaterialFaces = new IntPtr[numTextures];
	maxMaterialFaces = new IntPtr[numTextures];
	for(tex=0; tex<numTextures; tex++)
	{
		materialFaces[tex] = new IntPtr[nupgightmaps+1];
		numMaterialFaces[tex] = new int[nupgightmaps+1];
		maxMaterialFaces[tex] = new int[nupgightmaps+1];

		for(lm=0; lm<nupgightmaps+1; lm++)
		{
			numMaterialFaces[tex][lm] = 0;
			maxMaterialFaces[tex][lm] = 0;
		}

		materialFaces[tex]++;			// move lightmap base index from 0 to -1
		numMaterialFaces[tex]++;		// move lightmap base index from 0 to -1
		maxMaterialFaces[tex]++;		// move lightmap base index from 0 to -1
	}

	// next count how many faces there are per texture/lightmap
	for(face=0; face<numFaces; face++)
		maxMaterialFaces[faces[face].texture][faces[face].lightmap]++;

	// now allocate excactly as much as needed
	for(tex=0; tex<numTextures; tex++)
		for(lm=-1; lm<nupgightmaps; lm++)
		{
			int size = maxMaterialFaces[tex][lm];
			if(size==0)
				materialFaces[tex][lm] = NULL;
			else
				materialFaces[tex][lm] = new int[size+1];
		}


	// build the brushes list
	// (later on used for collision detection)
	numBrushes = bNumBrushes;
	brushes = new Brush[numBrushes];
	for(brush=0; brush<numBrushes; brush++)
	{
		const bsp_brush* b = bBrushes+brush;
		Brush* br = brushes+brush;

		br->numSides = b->n_brushsides;
		br->sides = new pgPlane[br->numSides];
		for(int side=0; side<br->numSides; side++)
		{
			const bsp_brushside* s = bBrushSides+b->brushside+side;
			const bsp_plane* plane = planes->getPlane(s->plane);

			br->sides[side].x = plane->normal[0];
			br->sides[side].y = plane->normal[2];
			br->sides[side].z = plane->normal[1];
			br->sides[side].d = plane->dist;
		}

		br->buildEdges();
	}

	foundBrushes = new int[numBrushes];
	numFoundBrushes = 0;

	foundFaces = new int[numFaces];
	numFoundFaces = 0;

	// for testing purpose:
	// activate all faces in the
	// corresponding tex/lm section
	/*for(face=0; face<numFaces; face++)
	{
		Face& f = faces[face];
		materialFaces[f.texture][f.lightmap][numMaterialFaces[f.texture][f.lightmap]++] = face;
		faces[face].added = false;
	}*/

	pgLog::info("bsp level with %d triangles created", numTris);

	//stripIndices = new int[numFaces];
	//stripTris = new int[numFaces];


	return true;
}



/******************************************************************************************\
*                                                                                          *
*                                                                                          *
*                                      RENDERING                                           *
*                                                                                          *
*                                                                                          *
\******************************************************************************************/


void pgBSPMesh::render()
{
	//DWORD dwNumPasses;
	//HRESULT	hr;
	int		lm,tex, cluster, li, fi, face;
	D3DXMATRIX tMat;

	matsRendered = verticesRendered = trisRendered = 0;

	if(textures==NULL)
	{
		pgLog::error("can not render bsp mesh: textures not allocated");
		return;
	}

	if(lightmaps==NULL)
	{
		pgLog::error("can not render bsp mesh: lightmaps not allocated");
		return;
	}

	if(vBuffer==NULL)
	{
		pgLog::error("can not render bsp mesh: vertex buffer not allocated");
		return;
	}

	if(faces==NULL)
	{
		pgLog::error("can not render bsp mesh: faces not allocated");
		return;
	}

	if(numMaterialFaces==NULL || materialFaces==NULL)
	{
		pgLog::error("can not render bsp mesh: materials not allocated");
		return;
	}

	if(clusterLeafs==NULL)
	{
		pgLog::error("can not render bsp mesh: clusterLeafs not allocated");
		return;
	}

	if(!checkDevice("can not render bsp mesh: direct3d device invalid"))
		return;

	if(faces==NULL || materialFaces==NULL || clusterLeafs==NULL || pd3dDevice==NULL)
		return;


	updateShaders();
	curLeaf = findLeaf(pgIDirectX::getCameraPos());
	if(curLeaf>=0)
		curCluster = leafs->getLeaf(curLeaf)->cluster;
	else
		curCluster = -1;


	// check which faces are visible from the current position
	//
	for(lm=-1; lm<nupgightmaps; lm++)
		for(tex=0; tex<numTextures; tex++)
			numMaterialFaces[tex][lm] = 0;					// reset all visibility lists

	if(curCluster==-1 || clusterVis==NULL)
	{
		for(face=0; face<numFaces; face++)
		{
			Face& f = faces[face];
			//if(f.type==2)
			{
				materialFaces[f.texture][f.lightmap][numMaterialFaces[f.texture][f.lightmap]++] = face;
				faces[face].added = false;
			}
		}
	}
	else
	{
		for(cluster=0; cluster<numClusters; cluster++)
			if(clusterVis[curCluster][cluster])
				for(li=0; li<numClusterLeafs[cluster]; li++)
				{
					int leaf = clusterLeafs[cluster][li];
					int	nupgF = nupgeafFaces[leaf];

					if(isLeafVisible(leaf))
						for(fi=0; fi<nupgF; fi++)
						{
							face = leafFaces[leaf][fi];
							Face& f = faces[face];

							if(!f.added /*&& f.type==2*/)
							{
								materialFaces[f.texture][f.lightmap][numMaterialFaces[f.texture][f.lightmap]++] = face;
								f.added = true;
							}
						}
				}

		for(face=0; face<numFaces; face++)
			faces[face].added = false;
	}


	// setup general render state settings
	//
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
	pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	//pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	D3DXMatrixIdentity(&tMat);
	pd3dDevice->SetTransform(D3DTS_WORLD, &tMat);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, pgIDirectX::getCullFlag(D3DCULL_CW));
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, pgIDirectX::getFillFlag());

	/*if(pgIDirectX::getWireframe())
	{
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	else
	{
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}*/


	//////////////////////////////////////////////////////
	// render objects which have a lightmap
	//
	if(nupgightmaps>0)
	{
		// setup the render stages to do texturing at stage 0
		// and modulate the result with a lightmap at stage 1
		// (disable stage 2)
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	//D3DTOP_SELECTARG1
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);		// trilinear filtering is off...

		pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
		pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);		// no mipmapping for the lightmaps
		pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

		pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

		pd3dDevice->SetVertexShader(D3DFVF_BSPVERTEX);

		pd3dDevice->SetTexture(0, textures[0]->getD3DTexture());
		pd3dDevice->SetTexture(1, lightmaps[0]->getD3DTexture());

		//if(!SUCCEEDED( hr = pd3dDevice->ValidateDevice( &dwNumPasses)))
		//	return;

		
		numBatches = 0;
		BSPVERTEX*	pVertices;
		WORD*		pIndices;
		int			numI=0, numV=0;

		iBuffer->Lock(0, 0, (BYTE**)&pIndices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);
		vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);

		for(lm=0; lm<nupgightmaps; lm++)
			renderWithLightmapIndex(lm, pVertices, pIndices, numI, numV);

		vBuffer->Unlock();
		iBuffer->Unlock();

		renderLightmapBatches();
	}




	//////////////////////////////////////////////////////
	// render objects without a lightmap
	//

	// setup the texture stages
	//
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		//D3DTOP_SELECTARG1
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetVertexShader(D3DFVF_BSPVERTEX);

	//if(!SUCCEEDED( hr = pd3dDevice->ValidateDevice( &dwNumPasses)))
	//	return;


	numBatches = 0;
	BSPVERTEX*	pVertices;
	WORD*		pIndices;
	int			numI=0, numV=0;

	// first write all lightmap/texture combinations
	// into one big index/vertex buffer
	iBuffer->Lock(0, 0, (BYTE**)&pIndices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);
	vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);

	//for(lm=0; lm<nupgightmaps; lm++)
	//	renderWithLightmapIndex(lm, pVertices, pIndices, numI, numV);

	renderWithLightmapIndex(-1, pVertices, pIndices, numI, numV);

	vBuffer->Unlock();
	iBuffer->Unlock();

	renderLightmapBatches();

	
}


void
pgBSPMesh::renderWithLightmapIndex(int lm, BSPVERTEX* &pVertices, WORD* &pIndices, int &numI, int &numV)
{
	int tex, i,j;

	for(tex=0; tex<numTextures; tex++)
	{
		int num = numMaterialFaces[tex][lm];

		if(num>0)
		{
			int* fIndices = materialFaces[tex][lm];
			int numT = 0, numI0;
			batches[numBatches].i0 = numI;
			batches[numBatches].v0 = numV;

			for(i=0; i<num; i++)
			{
				Face& face = faces[fIndices[i]];

				numI0 = face.numIndices;
				for(j=0; j<numI0; j++)
					pIndices[numI+j] = (unsigned short)(face.indices[j] + numV);

				memcpy(pVertices+numV, face.vertices, face.numVertices*sizeof(BSPVERTEX));
				numV += face.numVertices;
				numI += face.numIndices;
				numT += face.numTris;
			}

			//int maxVal = 100000;

			if(numV>MAX_VERTICES || numI>MAX_INDICES)
			{
				vBuffer->Unlock();
				iBuffer->Unlock();

				renderLightmapBatches();

				// reset buffer filling
				//
				numBatches = 0;
				numI = numV = numT = 0;
				iBuffer->Lock(0, 0, (BYTE**)&pIndices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);
				vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);


				// now do it again...
				batches[numBatches].i0 = numI;
				batches[numBatches].v0 = numV;

				for(i=0; i<num; i++)
				{
					Face& face = faces[fIndices[i]];

					numI0 = face.numIndices;
					for(j=0; j<numI0; j++)
						pIndices[numI+j] = (unsigned short)(face.indices[j] + numV);

					memcpy(pVertices+numV, face.vertices, face.numVertices*sizeof(BSPVERTEX));
					numV += face.numVertices;
					numI += face.numIndices;
					numT += face.numTris;
				}

			}


			Batch& b = batches[numBatches];
			b.lm = lm;
			b.tex = tex;
			b.numI = numI - b.i0;
			b.numV = numV - b.v0;
			b.numT = numT;
			b.numStrips = num;
			numBatches++;
		}
	}
}



void pgBSPMesh::renderLightmapBatches()
{
	int i, lmOld = -2;

	// draw all the pregenerated triangles
	//
	pd3dDevice->SetStreamSource(0, vBuffer, sizeof(BSPVERTEX));
	pd3dDevice->SetIndices(iBuffer, 0);

	for(i=0; i<numBatches; i++)
	{
		Batch& b = batches[i];

		if(b.lm!=lmOld && b.lm>0)
		{
			pd3dDevice->SetTexture(1, lightmaps[b.lm]->getD3DTexture());
			lmOld = b.lm;
		}

		pd3dDevice->SetTexture(0, textures[b.tex]->getD3DTexture());
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, b.v0, b.numV, b.i0, b.numT);

		pgIDirectX::addLevelTris(b.numT);
		trisRendered += b.numT;
		verticesRendered += b.numV;
		matsRendered++;
	}


}



/******************************************************************************************\
*                                                                                          *
*                                                                                          *
*                                  COLLISION METHODS                                       *
*                                                                                          *
*                                                                                          *
\******************************************************************************************/



bool pgBSPMesh::slideSphere(const pgVec3& nPos0, float nRadius, const pgVec3& nPos1, pgVec3& nFinalPos)
{
	pgVec3	dir = nPos1-nPos0,			// calculate the planned way to go
			savePos = nPos0;			// this will save correct positions (no overlapping with the bsp)
	int		recDepth = 1;

	nFinalPos = slideSphereSub(nPos0, nRadius, dir, savePos, recDepth);
	return !(nPos1==nFinalPos);
}

pgVec3n tDirs[6] = {	pgVec3n(-1.0f, 0.0f, 0.0f),
						pgVec3n( 1.0f, 0.0f, 0.0f),
						pgVec3n( 0.0f,-1.0f, 0.0f),
						pgVec3n( 0.0f, 1.0f, 0.0f),
						pgVec3n( 0.0f, 0.0f,-1.0f),
						pgVec3n( 0.0f, 0.0f, 1.0f) };


pgVec3 pgBSPMesh::findSavePos(const pgVec3& nSavePos, float nRadius)
{
	pgVec3		colSpherePos, colTriPos;
	float		dist;
	bool		stuck=false;
	static int	i=0;

	for(int c=0; c<6; c++)
	{
		if(!collideSphere(nSavePos, nRadius, tDirs[i], colSpherePos, colTriPos, dist, stuck))
			return nSavePos+tDirs[i];
		i++;
		if(i>=6)
			i=0;
	}

	assert(false);
	return nSavePos;
}

pgVec3 pgBSPMesh::slideSphereSub(const pgVec3& nPos, float nRadius, const pgVec3& nDir, pgVec3& nSavePos, int nRecDepth)
{
	float len = nDir.length();

	if(len<pgEPSL)
		return nPos;

	pgVec3	colSpherePos, colTriPos,
			destPos = nPos + nDir,
			newDir, newPos, newDest;
	float	dist, t;
	bool	stuck = false;

	if(nRecDepth>50)
	{
		assert(false);
		return nPos;
	}


	// check if there is a collision on our
	// plane way to go
	if(!collideSphere(nPos, nRadius, nDir, colSpherePos, colTriPos, dist, stuck))
	{
		nSavePos = nPos;								// no collision: return the desired destination position
		pgVec3 dir = nDir * (1.0f-pgEPSL);
		return nPos+dir;
	}
	else
	{													// collision: need to calculate sliding...
		// we were stuck in out last movement
		// try to take the last correct position and stop moving
		if(stuck)
		{
			return findSavePos(nSavePos, nRadius);
			//return nSavePos;
		}

		// if we almost went the complete distance to go,
		// we make no more updates...
		if(dist>=0.05f)
		{
			pgVec3 dir = nDir;
			dir.setLength(dist-pgEPSL);
			newPos = nPos + dir;
		}
		else
			newPos = nPos;

		// calculate the sliding plane
		//
		pgVec3n slidePlaneNormal(newPos - colTriPos);
		pgPlane	slidePlane(colTriPos, slidePlaneNormal);

		// project the destination point onto the sliding plane
		//
		pgIMathTool::findIntersectionRayPlane(slidePlane, destPos, slidePlaneNormal, newDest, t);

		// calculate the new slide vector, which will become
		// our velocity vector for the next iteration
		newDir = newDest - colTriPos;
		nSavePos = nPos;

		// recursively call the function with the new position and velocity 
		//
		return slideSphereSub(newPos, nRadius, newDir, nSavePos, nRecDepth+1); 
	}
}


bool pgBSPMesh::collideSphere(const pgVec3& nSpherePos, float nRadius, const pgVec3& nVelocity,
							  pgVec3& nNearestColSpherePos, pgVec3& nNearestColTriPos, float& nNearestDistance, bool& nStuck)
{
	int i,idx;
	pgVec3 destPos = nSpherePos+nVelocity;
	float dist, nearestDist = pgFLOAT_MAX;
	pgVec3 colSpherePos, colTriPos, nearestColSpherePos, nearestColTriPos;

	pgAABBox bbox(nSpherePos, 2.0f*nRadius, 2.0f*nRadius, 2.0f*nRadius);
	bbox.addBox(pgAABBox(destPos, 2.0f*nRadius, 2.0f*nRadius, 2.0f*nRadius));

	findFaces(bbox);

	if(numFoundFaces==0)
		return false;

	for(i=0; i<numFoundFaces; i++)
	{
		Face& face = faces[foundFaces[i]];

		int numI = face.numColIndices;

		for(idx=0; idx<numI; idx+=3)
		{
			const pgVec3& v0 = face.colVertices[face.colIndices[idx+0]],
						  v1 = face.colVertices[face.colIndices[idx+1]],
						  v2 = face.colVertices[face.colIndices[idx+2]];

			if(pgIMathTool::findIntersectionSphereTriangle(nSpherePos, nRadius, nVelocity, v0, v1, v2, NULL, dist, colSpherePos, colTriPos, nStuck))
			{
				didCollide = 200;

				if(dist<nearestDist)
				{
					nearestColSpherePos = colSpherePos;
					nearestColTriPos = colTriPos;
					nearestDist = dist;
				}
			}
		}
	}

	nNearestColSpherePos = 	nearestColSpherePos;
	nNearestColTriPos = nearestColTriPos;
	nNearestDistance = nearestDist;

	return nearestDist<pgFLOAT_MAX;
}


pgTexturePtr pgBSPMesh::createLightmap(const unsigned char* nData)
{
	LPDIRECT3DTEXTURE8 tex = NULL;
	D3DLOCKED_RECT Lock;

	if(!pd3dDevice)
		return NULL;

    if(FAILED(D3DXCreateTexture(pd3dDevice, LIGHTMAP_SIZE,LIGHTMAP_SIZE, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex)))
        return NULL;

    D3DSURFACE_DESC desc;
    tex->GetLevelDesc(0, &desc);

    if(FAILED(tex->LockRect(0, &Lock, NULL, 0)))
    {
        tex->Release();
        return NULL;
    }

    unsigned char *data = (unsigned char *)Lock.pBits;
	int pitch = Lock.Pitch;
	int x,y;

	for(x=0; x<LIGHTMAP_SIZE; x++)
		for(y=0; y<LIGHTMAP_SIZE; y++)
		{
			data[y*pitch+4*x+0] = lightUp(nData[3*(y*LIGHTMAP_SIZE+x)+2]);
			data[y*pitch+4*x+1] = lightUp(nData[3*(y*LIGHTMAP_SIZE+x)+1]);
			data[y*pitch+4*x+2] = lightUp(nData[3*(y*LIGHTMAP_SIZE+x)+0]);
			data[y*pitch+4*x+3] = 0;
		}

    tex->UnlockRect(0);
	return new pgTexture("lightmap", tex, LIGHTMAP_SIZE,LIGHTMAP_SIZE, pgImage::ARGB8888);
}





/******************************************************************************************\
*                                                                                          *
*                                                                                          *
*                                    SHADER METHODS                                        *
*                                                                                          *
*                    (shader are currently VERY poorly supported)                          *
*                                                                                          *
*                                                                                          *
\******************************************************************************************/



void pgBSPMesh::loadShaders(const char* nFileName)
{
	const char		*line2;
	int				lineNr=0, braces, i;
	bool			animMapFound;
	pgInTextFile	inFile;
	char			texNames[16][256];

	if(!inFile.open(nFileName))
	{
		pgLog::info("no q3-shader file found (%s)", nFileName);
		return;
	}

	numShaders = 0;
	Shader* tmpShaders = new Shader[256];

	while(!inFile.eof())
	{
		const char* line = inFile.readLine();
		lineNr++;

		if(pgIStringTool::isEmpty(line))
			continue;

		Shader& s = tmpShaders[numShaders];
		s.name = new char[strlen(line)+1];
		strcpy(s.name, line);

		line = inFile.readLine();
		lineNr++;
		if(!pgIStringTool::startsWith(line, "{"))
		{
			pgLog::error("can not read q3-shader: expected '{' after shader name in line %d. found '%s'", lineNr, line);
			return;
		}

		braces = 1;
		animMapFound = false;

		while(braces>0)
		{
			line = inFile.readLine();
			lineNr++;

			if(pgIStringTool::isEmpty(line))
				continue;

			if(pgIStringTool::startsWith(line, "{"))
			{
				braces++;
				continue;
			}

			if(pgIStringTool::startsWith(line, "}"))
			{
				braces--;
				continue;
			}

			if(pgIStringTool::startsWith(line, "animMap") && !animMapFound)
			{
				animMapFound = true;
				line2 = pgIStringTool::getPosAfter(line, "animMap");

				line2 = pgIStringTool::skipSpaces(line2);

				if(line2==NULL)
				{
					pgLog::error("can not read q3-shader: error in line %d: '%s'", lineNr, line);
					return;
				}

				sscanf(line2, "%f ", &s.freq);

				line2 = pgIStringTool::skipNonSpaces(line2);

				if(line2==NULL)
				{
					pgLog::error("can not read q3-shader: error in line %d: '%s'", lineNr, line);
					return;
				}

				for(;;)
				{
					line2 = pgIStringTool::skipSpaces(line2);

					const char* line0 = line2;

					line2 = pgIStringTool::skipNonSpaces(line2);
					int len = line2-line0;

					if(len==0)
						break;

					strncpy(texNames[s.numImages], line0, len);
					texNames[s.numImages][len] = 0;
					s.numImages++;

				}

				s.imgNames = new CharPtr[s.numImages];
				for(i=0; i<s.numImages; i++)
				{
					s.imgNames[i] = new char[strlen(texNames[i])+1];
					strcpy(s.imgNames[i], texNames[i]);
				}
			}
		}

		numShaders++;
	}

	shaders = new Shader[numShaders];
	for(i=0; i<numShaders; i++)
		shaders[i] = tmpShaders[i];

	delete tmpShaders;
}



bool pgBSPMesh::loadShaderTextures()
{
	int i, tex;

	if(shaders==NULL)
		return true;

	for(i=0; i<numShaders; i++)
	{
		Shader& s = shaders[i];

		s.imgTextures = new pgTexturePtr[s.numImages];

		for(tex=0; tex<s.numImages; tex++)
		{
			if((s.imgTextures[tex] = pgIResourceManager::getTexture(s.imgNames[tex], pgIResourceManager::SOURCE_BSP)) == NULL)
			{
				pgLog::error("backup texture not found -> quiting bsp creation");
				return false;
			}
		}
	}

	return true;
}


Shader* pgBSPMesh::getShader(const char* nShaderName)
{
	for(int i=0; i<numShaders; i++)
	{
		Shader* s = shaders+i;

		if(strcmp(nShaderName, s->name)==NULL)
			return s;
	}

	return NULL;
}


void pgBSPMesh::updateShaders()
{
	float dt = pgITime::getLastFrameTime();

	for(int i=0; i<numShaders; i++)
	{
		Shader& s = shaders[i];

		s.time += dt;
		float t = 1.0f / s.freq;

		if(s.time >= t)
		{
			s.time -= t;
			s.curImage++;
			if(s.curImage>=s.numImages)
				s.curImage = 0;
			textures[s.tex] = s.imgTextures[s.curImage];
		}
	}

}



/******************************************************************************************\
*                                                                                          *
*                                                                                          *
*                                INTERNAL TOOL METHODS                                     *
*                                                                                          *
*                                                                                          *
\******************************************************************************************/


// searches all brushes that are within the given
// bounding box and saves them in foundBrushes
// number of found brushes is saves in numFoundBrushes
void pgBSPMesh::findBrushes(const pgAABBox& nBBox, int nNode)
{
	const pgVec3&	bMin = nBBox.getMin(),
					bMax = nBBox.getMax();
	int				minX = (int)bMin.x,	minY = (int)bMin.y,	minZ = (int)bMin.z,
					maxX = (int)bMax.x,	maxY = (int)bMax.y,	maxZ = (int)bMax.z;

	if(nNode==0)					// starting node
		numFoundBrushes = 0;

	if(nNode>=0)		// this is really a node
	{
		const bsp_node	*node = nodes->getNode(nNode);

		if(minX>node->maxs[0] || node->mins[0]>maxX ||
		   minY>node->maxs[2] || node->mins[2]>maxY ||
		   minZ>node->maxs[1] || node->mins[1]>maxZ)		// no overlapping
			return;

		findBrushes(nBBox, node->children[0]);
		findBrushes(nBBox, node->children[1]);
	}
	else				// this is a leaf
	{
		const bsp_leaf	*leaf = leafs->getLeaf(-(nNode+1));
		const int		*tLeafbrushes = leafbrushes->getLeafbrushes();
		int				nupgB = leaf->n_leafbrushes,
						brush0 = leaf->leafbrush;

		if(minX>leaf->maxs[0] || leaf->mins[0]>maxX ||
		   minY>leaf->maxs[2] || leaf->mins[2]>maxY ||
		   minZ>leaf->maxs[1] || leaf->mins[1]>maxZ)		// no overlapping
			return;

		for(int bi=0; bi<nupgB; bi++)
		{
			int brush = tLeafbrushes[brush0+bi];
			const Brush* b = brushes+brush;

			if(b->bbox.doesOverlap(nBBox))
			{
				for(int i=0; i<numFoundBrushes; i++)
					if(foundBrushes[i] == brush)
						break;
				if(i==numFoundBrushes)
					foundBrushes[numFoundBrushes++] = brush;
			}
		}
	}

}


// searches all faces that are within the given
// bounding box and saves them in foundFaces
// number of found faces is saves in numFoundFaces
void pgBSPMesh::findFaces(const pgAABBox& nBBox, int nNode)
{
	const pgVec3&	bMin = nBBox.getMin(),
					bMax = nBBox.getMax();
	int				minX = (int)bMin.x,	minY = (int)bMin.y,	minZ = (int)bMin.z,
					maxX = (int)bMax.x,	maxY = (int)bMax.y,	maxZ = (int)bMax.z;

	if(nNode==0)					// starting node
		numFoundFaces = 0;

	if(nNode>=0)		// this is really a node
	{
		const bsp_node	*node = nodes->getNode(nNode);

		if(minX>node->maxs[0] || node->mins[0]>maxX ||
		   minY>node->maxs[2] || node->mins[2]>maxY ||
		   minZ>node->maxs[1] || node->mins[1]>maxZ)		// no overlapping
			return;

		findFaces(nBBox, node->children[0]);
		findFaces(nBBox, node->children[1]);
	}
	else				// this is a leaf
	{
		int leaf = -(nNode+1);
		const bsp_leaf	*l = leafs->getLeaf(leaf);

		if(minX>l->maxs[0] || l->mins[0]>maxX ||
		   minY>l->maxs[2] || l->mins[2]>maxY ||
		   minZ>l->maxs[1] || l->mins[1]>maxZ)		// no overlapping
			return;


		int	nupgF = nupgeafFaces[leaf];

		for(int fi=0; fi<nupgF; fi++)
		{
			int		face = leafFaces[leaf][fi];
			Face&	f = faces[face];

			//if(f.type!=1)
			//	continue;

			if(f.bbox.doesOverlap(nBBox))
			{
				for(int i=0; i<numFoundFaces; i++)
					if(foundFaces[i] == face)
						break;
				if(i==numFoundFaces)
					foundFaces[numFoundFaces++] = face;
			}
		}
	}
}


int pgBSPMesh::findLeaf(const pgVec3& nPos)
{
	//const D3DXMATRIX	*mat = pgIDirectX::getViewMatrix();
	const				bsp_plane* tplanes = planes->getPlanes();
	int					child = 0;
	float				dist;
	float				x=nPos.x, y=nPos.z, z=nPos.y;

	do
	{
		const bsp_node*		node = nodes->getNode(child);
		const bsp_plane*	plane = tplanes+node->plane;

		dist = plane->normal[0]*x + plane->normal[1]*y + plane->normal[2]*z;
		if(dist>plane->dist)
			child = node->children[0];
		else
			child = node->children[1];
	} while(child>=0);

	child = -(child+1);

	const bsp_leaf	*leaf = leafs->getLeaf(child);
	const int		*mins = leaf->mins,
					*maxs = leaf->maxs;

	if(x>=mins[0] && x<=maxs[0] && y>=mins[1] && y<=maxs[1] && z>=mins[2] && z<=maxs[2])
		return child;
	else
		return -1;
}


int pgBSPMesh::findCluster(const pgVec3& nPos)
{
	int leaf = findLeaf(nPos);

	if(leaf>=0)
		return leafs->getLeaf(leaf)->cluster;
	return -1;
}


bool pgBSPMesh::isPointVisible(float nX, float nY, float nZ)
{
	D3DXVECTOR3	vecIn;
	D3DXVECTOR4	vecOut;

	vecIn.x = nX;
	vecIn.y = nY;
	vecIn.z = nZ;

	D3DXVec3Transform(&vecOut, &vecIn, pgIDirectX::getViewMatrix().getD3DMatrix());

	if(vecOut.z<0.0f)
		return false;

	/*float z = vecOut.z * 0.5f;
	
	  if(vecOut.x>z)
		return false;

	if(vecOut.x<-z)
		return false;

	if(vecOut.y>z)
		return false;

	if(vecOut.y>z)
		return false;*/

	return true;
}


bool pgBSPMesh::isLeafVisible(int nLeaf)
{
	const bsp_leaf*		leaf = leafs->getLeaf(nLeaf);
	float				minX0 = (float)leaf->mins[0],
						minY0 = (float)leaf->mins[2],
						minZ0 = (float)leaf->mins[1],
						maxX0 = (float)leaf->maxs[0],
						maxY0 = (float)leaf->maxs[2],
						maxZ0 = (float)leaf->maxs[1];

	if(isPointVisible(minX0, minY0, minZ0))
		return true;

	if(isPointVisible(maxX0, minY0, minZ0))
		return true;

	if(isPointVisible(maxX0, maxY0, minZ0))
		return true;

	if(isPointVisible(minX0, maxY0, minZ0))
		return true;


	if(isPointVisible(minX0, minY0, maxZ0))
		return true;

	if(isPointVisible(maxX0, minY0, maxZ0))
		return true;

	if(isPointVisible(maxX0, maxY0, maxZ0))
		return true;

	if(isPointVisible(minX0, maxY0, maxZ0))
		return true;


	return false;
}

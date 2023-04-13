///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPFile.cpp,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#pragma warning(disable: 4514)

#include "pgBSPFile.h"

#include <string.h>
#include <assert.h>


bsp_lump_entities::bsp_lump_entities(const unsigned char* nData, int nSize)
{
	size = nSize;
	ents = new char[size];
	memcpy(ents, nData, size);
}


bsp_lump_textures::bsp_lump_textures(const unsigned char* nData, int nSize)
{
	//int s = sizeof(bsp_texture);

	numTextures = nSize/sizeof(bsp_texture);
	textures = new bsp_texture[numTextures+1];

	memcpy(textures, nData, nSize);
}


bsp_lump_planes::bsp_lump_planes(const unsigned char* nData, int nSize)
{
	//int s = sizeof(bsp_plane);

	numPlanes = nSize/sizeof(bsp_plane);
	planes = new bsp_plane[numPlanes+1];

	memcpy(planes, nData, nSize);
}


bsp_lump_planes* bsp_lump_planes::duplicate() const
{
	bsp_lump_planes* copy = new bsp_lump_planes();

	copy->numPlanes = numPlanes;
	copy->planes = new bsp_plane[numPlanes+1];
	memcpy(copy->planes, planes, numPlanes*sizeof(bsp_plane));

	return copy;
}


bsp_lump_nodes::bsp_lump_nodes(const unsigned char* nData, int nSize)
{
	//int s = sizeof(bsp_node);

	numNodes = nSize/sizeof(bsp_node);
	nodes = new bsp_node[numNodes+1];

	memcpy(nodes, nData, nSize);
}


bsp_lump_nodes* bsp_lump_nodes::duplicate() const
{
	bsp_lump_nodes* copy = new bsp_lump_nodes();

	copy->numNodes = numNodes;
	copy->nodes = new bsp_node[numNodes+1];
	memcpy(copy->nodes, nodes, numNodes*sizeof(bsp_node));

	return copy;
}


bsp_lump_vertices::bsp_lump_vertices(const unsigned char* nData, int nSize)
{
	//int s = sizeof(bsp_vertex);

	numVertices = nSize/sizeof(bsp_vertex);
	vertices = new bsp_vertex[numVertices+1];

	memcpy(vertices, nData, nSize);
}


bsp_lump_meshvertices::bsp_lump_meshvertices(const unsigned char* nData, int nSize)
{
	numIndices = nSize / 4;
	indices = new int[numIndices+1];

	memcpy(indices, nData, nSize);
}


bsp_lump_faces::bsp_lump_faces(const unsigned char* nData, int nSize)
{
	//int s = sizeof(bsp_face);

	numFaces = nSize/sizeof(bsp_face);
	faces = new bsp_face[numFaces+1];

	memcpy(faces, nData, nSize);
}


bsp_lump_lightmaps::bsp_lump_lightmaps(const unsigned char* nData, int nSize)
{
	nupgightmaps = nSize/(128*128*3);
	lightmaps = new unsigned char[nupgightmaps*128*128*3];

	memcpy(lightmaps, nData, nSize);
}


bsp_lump_leafs::bsp_lump_leafs(const unsigned char* nData, int nSize)
{
	//int s = sizeof(bsp_leaf);

	nupgeafs = nSize/sizeof(bsp_leaf);
	leafs = new bsp_leaf[nupgeafs+1];

	memcpy(leafs, nData, nSize);
}


bsp_lump_leafs* bsp_lump_leafs::duplicate() const
{
	bsp_lump_leafs* copy = new bsp_lump_leafs();

	copy->nupgeafs = nupgeafs;
	copy->leafs = new bsp_leaf[nupgeafs+1];
	memcpy(copy->leafs, leafs, nupgeafs*sizeof(bsp_leaf));

	return copy;
}


bsp_lump_leaffaces::bsp_lump_leaffaces(const unsigned char* nData, int nSize)
{
	//int s = sizeof(bsp_leaf);

	nupgeaffaces = nSize/4;
	leaffaces = new int[nupgeaffaces+1];

	memcpy(leaffaces, nData, nSize);
}


bsp_lump_leafbrushes::bsp_lump_leafbrushes(const unsigned char* nData, int nSize)
{
	nupgeafbrushes = nSize/4;
	leafbrushes = new int[nupgeafbrushes+1];

	memcpy(leafbrushes, nData, nSize);
}


bsp_lump_leafbrushes* bsp_lump_leafbrushes::duplicate() const
{
	bsp_lump_leafbrushes* copy = new bsp_lump_leafbrushes();

	copy->nupgeafbrushes = nupgeafbrushes;
	copy->leafbrushes = new int[nupgeafbrushes+1];
	memcpy(copy->leafbrushes, leafbrushes, nupgeafbrushes*sizeof(int));

	return copy;
}


bsp_lump_brushes::bsp_lump_brushes(const unsigned char* nData, int nSize)
{
	numBrushes = nSize/sizeof(bsp_brush);
	brushes = new bsp_brush[numBrushes+1];

	memcpy(brushes, nData, nSize);
}


bsp_lump_brushsides::bsp_lump_brushsides(const unsigned char* nData, int nSize)
{
	numBrushsides = nSize/sizeof(bsp_brushside);
	brushsides = new bsp_brushside[numBrushsides+1];

	memcpy(brushsides, nData, nSize);
}


bsp_lump_visdata::bsp_lump_visdata(const unsigned char* nData, int nSize)
{
	int vecsSize = nSize - 8;

	visData = new bsp_visdata();
	memcpy(visData, nData, 8);
	visData->vecs = NULL;

	if(visData->n_vecs*visData->sz_vecs>0 && nSize>0)
	{
		assert(vecsSize == visData->n_vecs*visData->sz_vecs);

		visData->vecs = new unsigned char[vecsSize];
		memcpy(visData->vecs, nData+8, vecsSize);
	}
}

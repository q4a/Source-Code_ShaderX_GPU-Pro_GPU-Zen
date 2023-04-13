///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPReader
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPReader.cpp,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#pragma warning(disable: 4514)

#include "pgBSPReader.h"

#include <pgLib/Base/pgLog.h>

#include <stdio.h>
#include <string.h>

pgBSPReader::pgBSPReader()
{
	rawSize = 0;
	rawData = NULL;
}


pgBSPReader::~pgBSPReader()
{
	delete rawData;
	rawData = NULL;
}


bool pgBSPReader::load(const char* nFileName)
{
	FILE* fp;
	int read;

	fp = fopen(nFileName, "rb");

	if(!fp)
	{
		pgLog::error("can not open bsp file '%s'", nFileName);
		return false;
	}

	fseek(fp, 0, SEEK_END);
	rawSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	rawData = new unsigned char[rawSize];
	read = fread(rawData, 1, rawSize, fp);

	if(read != rawSize)
	{
		pgLog::error("can not read bsp file '%s'", nFileName);
		return false;
	}

	memcpy(&bspHeader, rawData, sizeof(bsp_header));

	if(strncmp(bspHeader.magic, "IBSP", 4)!=0)
	{
		pgLog::error("bsp file '%s' is not an IBSP", nFileName);
		return false;
	}

	if(bspHeader.version != 0x2e)
	{
		pgLog::error("bsp file '%s' has wrong version", nFileName);
		return false;
	}

	lumpEntities =		new bsp_lump_entities(rawData+bspHeader.lumps[0].offset, bspHeader.lumps[0].length);
	lumpTextures =		new bsp_lump_textures(rawData+bspHeader.lumps[1].offset, bspHeader.lumps[1].length);
	lumpPlanes =		new bsp_lump_planes(rawData+bspHeader.lumps[2].offset, bspHeader.lumps[2].length);
	lumpNodes =			new bsp_lump_nodes(rawData+bspHeader.lumps[3].offset, bspHeader.lumps[3].length);
	lumpLeafs =			new bsp_lump_leafs(rawData+bspHeader.lumps[4].offset, bspHeader.lumps[4].length);
	lumpLeaffaces =		new bsp_lump_leaffaces(rawData+bspHeader.lumps[5].offset, bspHeader.lumps[5].length);
	lumpLeafbrushes =	new bsp_lump_leafbrushes(rawData+bspHeader.lumps[6].offset, bspHeader.lumps[6].length);
	lumpBrushes =		new bsp_lump_brushes(rawData+bspHeader.lumps[8].offset, bspHeader.lumps[8].length);
	lumpBrushsides =	new bsp_lump_brushsides(rawData+bspHeader.lumps[9].offset, bspHeader.lumps[9].length);
	lumpVertices =		new bsp_lump_vertices(rawData+bspHeader.lumps[10].offset, bspHeader.lumps[10].length);
	lumpMeshvertices =	new bsp_lump_meshvertices(rawData+bspHeader.lumps[11].offset, bspHeader.lumps[11].length);
	lumpFaces =			new bsp_lump_faces(rawData+bspHeader.lumps[13].offset, bspHeader.lumps[13].length);
	lumpLightmaps =		new bsp_lump_lightmaps(rawData+bspHeader.lumps[14].offset, bspHeader.lumps[14].length);
	lumpVisdata =		new bsp_lump_visdata(rawData+bspHeader.lumps[16].offset, bspHeader.lumps[16].length);

	return true;
}

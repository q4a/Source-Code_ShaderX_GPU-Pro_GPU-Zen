///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPReader
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPReader.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BSPREADER_HEADERFILE__
#define __BSPREADER_HEADERFILE__

#include <pgLib/Render/BSP/pgBSPFile.h>


// This class is internal to pgMesh and therefore
// not further documented
class pgBSPReader
{
friend class pgBSPMesh;

public:
	pgBSPReader();
	~pgBSPReader();

	bool load(const char* nFileName);


private:
	int					rawSize;
	unsigned char*		rawData;

	bsp_header			bspHeader;

	bsp_lump_entities*		lumpEntities;
	bsp_lump_textures*		lumpTextures;
	bsp_lump_planes*		lumpPlanes;
	bsp_lump_nodes*			lumpNodes;
	bsp_lump_leafs*			lumpLeafs;
	bsp_lump_leaffaces*		lumpLeaffaces;
	bsp_lump_leafbrushes*	lumpLeafbrushes;
	bsp_lump_brushes*		lumpBrushes;
	bsp_lump_brushsides*	lumpBrushsides;
	bsp_lump_vertices*		lumpVertices;
	bsp_lump_meshvertices*	lumpMeshvertices;
	bsp_lump_faces*			lumpFaces;
	bsp_lump_lightmaps*		lumpLightmaps;
	bsp_lump_visdata*		lumpVisdata;
};


#endif //__BSPREADER_HEADERFILE__

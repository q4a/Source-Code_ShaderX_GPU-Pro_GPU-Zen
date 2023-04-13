///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPFile.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BSPFILE_HEADERFILE__
#define __BSPFILE_HEADERFILE__


struct bsp_lump
{
	unsigned int	offset;
	unsigned int	length;
};


struct bsp_header
{
	char			magic[4];
	unsigned int	version;

	bsp_lump		lumps[17];
};


struct bsp_texture
{
	char name[64];			// Texture name
	int flags;				// Surface flags
	int contents;			// Content flags
};


struct bsp_plane
{
	float normal[3];		// Plane normal.
	float dist;				// Distance from origin to plane along normal.
};


struct bsp_face
{
	int texture;			// Texture index.
	int effect;				// Index into lump 12 (Effects), or -1.
	int type;				// Face type. 1=polygon, 2=patch, 3=mesh, 4=billboard
	int vertex;				// Index of first vertex.
	int n_vertexes;			// Number of vertices.
	int meshvert;			// Index of first meshvert.
	int n_meshverts;		// Number of meshverts.
	int lm_index;			// Lightmap index.
	int lm_start[2];		// Corner of this face's lightmap image in lightmap.
	int lm_size[2];			// Size of this face's lightmap image in lightmap.
	float lm_origin[3];		// World space origin of lightmap.
	float lm_vecs[2][3];	// World space lightmap s and t unit vectors.
	float normal[3];		// Surface normal.
	int size[2];			// Patch dimensions.
};


struct bsp_node
{
	int plane;				// Plane index.
	int children[2];		// Children indices. Negative numbers are leaf indices: -(leaf+1).
	int mins[3];			// Integer bounding box min coord.
	int maxs[3];			// Integer bounding box max coord.
};


struct bsp_leaf
{
	int cluster;			// Visdata cluster index.
	int area;				// Areaportal area.
	int mins[3];			// Integer bounding box min coord.
	int maxs[3];			// Integer bounding box max coord.
	int leafface;			// First leafface for leaf.
	int n_leaffaces;		// Number of leaffaces for leaf.
	int leafbrush;			// First leafbrush for leaf.
	int n_leafbrushes;		// Number of leafbrushes for leaf.
};


struct bsp_brush
{
	int brushside;			// First brushside for brush.
	int n_brushsides;		// Number of brushsides for brush.
	int texture;			// Texture index.
};


struct bsp_brushside
{
	int plane;				// Plane index.
	int texture;			// Texture index.
};


struct bsp_vertex
{
	float pos[3];
	float texc[2][2];
	float normal[3];
	unsigned char color[4];
};


struct bsp_visdata
{
	int n_vecs;				// Number of vectors.
	int sz_vecs;			// Size of each vector, in bytes.
	unsigned char* vecs;	// Visibility data. One bit per cluster per vector.
};





class bsp_lump_entities
{
public:
	bsp_lump_entities(const unsigned char* nData, int nSize);

private:
	int		size;
	char*	ents;
};


class bsp_lump_textures
{
public:
	bsp_lump_textures(const unsigned char* nData, int nSize);

	int getNumTextures() const  { return numTextures; }

	const bsp_texture* getTexture(int nIndex) const  { return textures+nIndex; }

private:
	int				numTextures;
	bsp_texture*	textures;
};


class bsp_lump_planes
{
public:
	bsp_lump_planes(const unsigned char* nData, int nSize);

	bsp_lump_planes* duplicate() const;

	int getNumPlanes() const  { return numPlanes; }

	const bsp_plane* getPlane(int nIndex) const  { return planes+nIndex; }

	const bsp_plane* getPlanes() const  { return planes; }

private:
	bsp_lump_planes() {}

	int			numPlanes;
	bsp_plane*	planes;
};


class bsp_lump_nodes
{
public:
	bsp_lump_nodes(const unsigned char* nData, int nSize);

	bsp_lump_nodes* duplicate() const;

	int getNumNodes() const  { return numNodes; }

	const bsp_node* getNode(int nIndex) const  { return nodes+nIndex; }

private:
	bsp_lump_nodes() {}

	int			numNodes;
	bsp_node*	nodes;
};


class bsp_lump_leafs
{
public:
	bsp_lump_leafs(const unsigned char* nData, int nSize);

	bsp_lump_leafs* duplicate() const;

	int getNupgeafs() const  { return nupgeafs; }

	const bsp_leaf* getLeaf(int nIndex) const  { return leafs+nIndex; }

private:
	bsp_lump_leafs() {}

	int			nupgeafs;
	bsp_leaf*	leafs;
};


class bsp_lump_leaffaces
{
public:
	bsp_lump_leaffaces(const unsigned char* nData, int nSize);

	int getNupgeaffaces() const  { return nupgeaffaces; }

	int getLeafface(int nIndex) const  { return leaffaces[nIndex]; }

	const int* getLeaffaces() const  { return leaffaces; }

private:
	int		nupgeaffaces;
	int*	leaffaces;
};


class bsp_lump_leafbrushes
{
public:
	bsp_lump_leafbrushes(const unsigned char* nData, int nSize);

	bsp_lump_leafbrushes* duplicate() const;

	int getNupgeafbrushes() const  { return nupgeafbrushes; }

	int getLeafbrush(int nIndex) const  { return leafbrushes[nIndex]; }

	const int* getLeafbrushes() const  { return leafbrushes; }

private:
	bsp_lump_leafbrushes() {}

	int		nupgeafbrushes;
	int*	leafbrushes;
};


class bsp_lump_brushes
{
public:
	bsp_lump_brushes(const unsigned char* nData, int nSize);

	int getNumBrushes() const  { return numBrushes; }

	const bsp_brush* getBrush(int nIndex) const  { return brushes+nIndex; }

	const bsp_brush* getBrushes() const  { return brushes; }

private:
	int			numBrushes;
	bsp_brush*	brushes;
};


class bsp_lump_brushsides
{
public:
	bsp_lump_brushsides(const unsigned char* nData, int nSize);

	int getNumBrushsides() const  { return numBrushsides; }

	const bsp_brushside* getBrushside(int nIndex) const  { return brushsides+nIndex; }

	const bsp_brushside* getBrushsides() const  { return brushsides; }

private:
	int				numBrushsides;
	bsp_brushside*	brushsides;
};


class bsp_lump_vertices
{
public:
	bsp_lump_vertices(const unsigned char* nData, int nSize);

	int getNumVertices() const  { return numVertices; }

	const bsp_vertex* getVertex(int nIndex) const  { return vertices+nIndex; }

private:
	int				numVertices;
	bsp_vertex		*vertices;
};


class bsp_lump_meshvertices
{
public:
	bsp_lump_meshvertices(const unsigned char* nData, int nSize);

	int getNumIndices() const  { return numIndices; }

	int getIndex(int nIndex) const  { return indices[nIndex]; }

	const int* getIndices() const  { return indices; }

private:
	int		numIndices;
	int*	indices;
};


class bsp_lump_faces
{
public:
	bsp_lump_faces(const unsigned char* nData, int nSize);

	int getNumFaces() const  { return numFaces; }

	const bsp_face* getFace(int nIndex) const  { return faces+nIndex; }

private:
	int			numFaces;
	bsp_face*	faces;
};


class bsp_lump_lightmaps
{
public:
	bsp_lump_lightmaps(const unsigned char* nData, int nSize);

	int getNupgightmaps() const  { return nupgightmaps; }

	const unsigned char* getLightmap(int nIndex) const  { return lightmaps+nIndex*128*128*3; }

private:
	int				nupgightmaps;
	unsigned char*	lightmaps;
};


class bsp_lump_visdata
{
public:
	bsp_lump_visdata(const unsigned char* nData, int nSize);

	const bsp_visdata* getVisdata() const  { return visData; }

private:
	bsp_visdata*	visData;
};


#endif //__BSPFILE_HEADERFILE__

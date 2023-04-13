///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPClasses
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPClasses.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BSPCLASSES_HEADERFILE__
#define __BSPCLASSES_HEADERFILE__


#include <pgLib/Math/pgAABBox.h>
#include <pgLib/Math/pgPlane.h>
#include <pgLib/Render/pgTexture.h>

#include <d3dx8.h>


//////////////////////////////// BSPVERTEX DEFINITION ////////////////////////////////

struct BSPVERTEX
{
	D3DXVECTOR3 p;
	FLOAT tu1, tv1;
	FLOAT tu2, tv2;
};

#define D3DFVF_BSPVERTEX (D3DFVF_XYZ | D3DFVF_TEX2 )

#define FILL_BSPVERTEX( v, ax, ay, az, acolor, atu1, atv1, atu2, atv2 )  \
{   v.p.x = ax; v.p.y = ay; v.p.z = az; \
    v.tu1 = atu1; v.tv1 = atv1; v.tu2 = atu2; v.tv2 = atv2;\
}

//////////////////////////////////////////////////////////////////////////////////////


// a Face resembles a bsp-face, which can
// itself have many triangles (or even be a patch)
//
class Face
{
public:
	Face()
		{  texture = lightmap = -1;  added = false;  numIndices = numVertices = 0;
		   vertices = NULL;  indices = NULL;  type = -1;  }


	// drawing data
	//
	int		type;
	int		texture, lightmap;
	bool	added;

	int			numTris;
	int			numIndices;
	WORD*		indices;

	//int			numStripIndices;
	//WORD*		stripIndices;
	int			numVertices;
	BSPVERTEX*	vertices;

	// collision data
	//
	int			numColIndices;
	WORD*		colIndices;

	int			numColVertices;
	pgVec3*		colVertices;

	pgAABBox	bbox;
	//pgPlane		plane;
};


// a Brush is a convex polyhedron
// and can be used for collision
//
class Brush
{
public:
	Brush()
		{  numSides = 0;  sides = NULL;  numCorners = 0;  corners = NULL;  }

	int			numSides;
	pgPlane*	sides;

	int			numCorners;
	pgVec3*		corners;

	int				numEdges;
	unsigned short	*edges;

	pgAABBox	bbox;

	void buildEdges();
};


// a Batch stores the work for rendering a bsp level:
//   texture, lightmap to use
//   which and how indices, triangles from the buffers to use
class Batch
{
public:
	Batch()  { lm = tex = i0 = numI = v0 = numV = numT = numStrips = 0;  }

	int	lm, tex;
	int	i0, numI;
	int	v0, numV;
	int numT;
	int numStrips;
};


// a Shader resembles the q3-bsp-shaders
// (anim-textures, etc...)
//
class Shader
{
public:
	Shader()  { name = NULL;  freq = 0.0f; numImages = 0;  imgNames = NULL;  imgTextures = NULL;  time = 0.0f;  curImage = tex = 0; }
	//~Shader()  { delete name;  SAFE_RELEASE(imgTextures);
	//			 for(int i=0; i<numImages; i++) delete imgNames[i];  delete imgNames; }

	char*				name;
	int					curImage;
	int					tex;
	float				freq;
	int					numImages;
	char**				imgNames;
	pgTexturePtr		*imgTextures;
	float				time;
};


#endif //__BSPCLASSES_HEADERFILE__

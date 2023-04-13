///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       MD2 Tool Methods
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMeshUtil_MD2.cxx,v 1.5 2002/12/18 22:00:37 daniel Exp $
///////////////////////////////////////////////////////////////////


#define NUMVERTEXNORMALS	162

float	md2Normals[NUMVERTEXNORMALS][3] = 
{
#include "md2Normals.h"
};


typedef struct
{
   int magic;
   int version;
   int skinWidth;
   int skinHeight;
   int frameSize;
   int numSkins;
   int numVertices;
   int numTexCoords;
   int numTriangles;
   int numGlCommands;
   int numFrames;
   int offsetSkins;
   int offsetTexCoords;
   int offsetTriangles;
   int offsetFrames;
   int offsetGlCommands;
   int offsetEnd;
} model_t;


typedef struct
{
   unsigned char vertex[3];
   unsigned char lightNormalIndex;
} triangleVertex_t;


typedef struct
{
   float scale[3];
   float translate[3];
   char name[16];
   triangleVertex_t vertices[1];
} frame_t;


typedef struct
{
   short vertexIndices[3];
   short textureIndices[3];
} triangle_t;


typedef struct
{
   short s, t;
} textureCoordinate_t;


pgBaseMesh*
pgMeshUtil::loadMD2(const char* nName, bool nLighting)
{
	int						i, fnr, c,
							rawSize, read;
	unsigned char*			rawData;
	char					mdName[_MAX_PATH];

	pgBaseMesh* mesh = new pgBaseMesh();
	mesh->vertexSettings = pgBaseMesh::SET_TEXTURE1;

	if(!setDevice(mesh, "DirectX Device does not exist yet. cannot create box mesh"))
	{
		delete mesh;
		return NULL;
	}


	// try to open and read the md2 file
	pgISettings::extendToMD2Path(nName, mdName);
	strcat(mdName, ".md2");
	FILE* fp = fopen(mdName, "rb");

	if(!fp)
	{
		pgLog::error("cannot open md2 file '%s'", nName);
		delete mesh;
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	rawSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	rawData = new unsigned char[rawSize];
	read = fread(rawData, 1, rawSize, fp);

	if(read != rawSize)
	{
		pgLog::error("reading md2 file '%s' failed", nName);
		delete mesh;
		return false;
	}


	// read through the raw data
	//
	model_t*				header = (model_t*)rawData;
	//char*					skins = (char*)(rawData+header->offsetSkins);
	textureCoordinate_t*	texcoords = (textureCoordinate_t*)(rawData+header->offsetTexCoords);
	triangle_t*				triangles = (triangle_t*)(rawData+header->offsetTriangles);
	frame_t*				frames = (frame_t*)(rawData+header->offsetFrames);
	char*					rawFrames = (char*)frames;

	float					fSkinW = (float)header->skinWidth;
	float					fSkinH = (float)header->skinHeight;
	int						numFrames = header->numFrames;
	int						numTriangles = header->numTriangles;
	int						numVertices = header->numVertices;


	if(nLighting)
	{
		if(FAILED(mesh->pd3dDevice->CreateVertexBuffer(numTriangles * 3 * sizeof(pgBaseMesh::MESHVERTEX_LIG1),
													   pgIDirectX::getDisableWriteOnlyBuffers() ? 0 : D3DUSAGE_WRITEONLY,
													   D3DFVF_MESHVERTEX_LIG1, pgIDirectX::getPoolType(), &(mesh->vBuffer))))
		{
			pgLog::error("creating vertex buffer for lit basemesh failed");
			delete mesh;
			return NULL;
		}
		mesh->vertexSettings |= pgBaseMesh::SET_LIGHT;
	}
	else
	{
		if(FAILED(mesh->pd3dDevice->CreateVertexBuffer(numTriangles * 3 * sizeof(pgBaseMesh::MESHVERTEX_COL1),
													   pgIDirectX::getDisableWriteOnlyBuffers() ? 0 : D3DUSAGE_WRITEONLY,
													   D3DFVF_MESHVERTEX_COL1, pgIDirectX::getPoolType(), &(mesh->vBuffer))))
		{
			pgLog::error("creating vertex buffer for unlit basemesh failed");
			delete mesh;
			return NULL;
		}
		mesh->vertexSettings |= pgBaseMesh::SET_COLOR;
	}


	// fill all frames into buffers
	//
	//pgPtrList<pgBaseMesh::Frame>& mframes = mesh->getFrames();

	for(fnr=0; fnr<numFrames; fnr++)
	{
		pgBaseMesh::Frame* f = new pgBaseMesh::Frame();
		mesh->getFrames().addTail(f);

		f->numIndices = 0;
		f->numTriangles = numTriangles;
		f->numVertices = numTriangles * 3;
		if(nLighting)
			f->vertices = new pgBaseMesh::MESHVERTEX_LIG1[numTriangles * 3];
		else
			f->vertices = new pgBaseMesh::MESHVERTEX_COL1[numTriangles * 3];

		frame_t*			frame = (frame_t*)(rawFrames + fnr * (40 + numVertices*sizeof(triangleVertex_t)));
		triangleVertex_t*	vertices = frame->vertices;

		// vertex scaling
		float				sx = frame->scale[0];
		float				sy = frame->scale[1];
		float				sz = frame->scale[2];

		// vertex translation
		float				dx = frame->translate[0];
		float				dy = frame->translate[1];
		float				dz = frame->translate[2];

		pgAABBox&			box = f->bBox;

		for(c=0,i=0; i<numTriangles; i++)
		{
			// vertex coordinates
			int		i0 = triangles[i].vertexIndices[0];
			int		i1 = triangles[i].vertexIndices[1];
			int		i2 = triangles[i].vertexIndices[2];

			float	x0 = dx + vertices[i0].vertex[0] * sx;
			float	z0 = dy + vertices[i0].vertex[1] * sy;
			float	y0 = dz + vertices[i0].vertex[2] * sz;
			float	x1 = dx + vertices[i1].vertex[0] * sx;
			float	z1 = dy + vertices[i1].vertex[1] * sy;
			float	y1 = dz + vertices[i1].vertex[2] * sz;
			float	x2 = dx + vertices[i2].vertex[0] * sx;
			float	z2 = dy + vertices[i2].vertex[1] * sy;
			float	y2 = dz + vertices[i2].vertex[2] * sz;

			// texture coordinates
			int		t0 = triangles[i].textureIndices[0];
			int		t1 = triangles[i].textureIndices[1];
			int		t2 = triangles[i].textureIndices[2];

			float	u0 = (float)(texcoords[t0].s) / fSkinW;
			float	v0 = (float)(texcoords[t0].t) / fSkinH;
			float	u1 = (float)(texcoords[t1].s) / fSkinW;
			float	v1 = (float)(texcoords[t1].t) / fSkinH;
			float	u2 = (float)(texcoords[t2].s) / fSkinW;
			float	v2 = (float)(texcoords[t2].t) / fSkinH;

			// normals
			int		n0 = vertices[i0].lightNormalIndex;
			int		n1 = vertices[i1].lightNormalIndex;
			int		n2 = vertices[i2].lightNormalIndex;

			float	nx0 = md2Normals[n0][0];
			float	ny0 = md2Normals[n0][1];
			float	nz0 = md2Normals[n0][2];
			float	nx1 = md2Normals[n1][0];
			float	ny1 = md2Normals[n1][1];
			float	nz1 = md2Normals[n1][2];
			float	nx2 = md2Normals[n2][0];
			float	ny2 = md2Normals[n2][1];
			float	nz2 = md2Normals[n2][2];

			if(nLighting)
			{
				pgBaseMesh::MESHVERTEX_LIG1* v = (pgBaseMesh::MESHVERTEX_LIG1*)f->vertices;

				FILL_MESHVERTEX_LIG1A(v[c+2], x0,y0,z0, nx0,ny0,nz0, u0,v0)
				FILL_MESHVERTEX_LIG1A(v[c+1], x1,y1,z1, nx1,ny1,nz1, u1,v1)
				FILL_MESHVERTEX_LIG1A(v[c+0], x2,y2,z2, nx2,ny2,nz2, u2,v2)
			}
			else
			{
				pgBaseMesh::MESHVERTEX_COL1* v = (pgBaseMesh::MESHVERTEX_COL1*)f->vertices;

				FILL_MESHVERTEX_COL1A(v[c+2], x0,y0,z0, 0xffffffff, u0,v0)
				FILL_MESHVERTEX_COL1A(v[c+1], x1,y1,z1, 0xffffffff, u1,v1)
				FILL_MESHVERTEX_COL1A(v[c+0], x2,y2,z2, 0xffffffff, u2,v2)
			}

			// extend the frame's bounding box
			// to contain every vertex
			box.addVertex(pgVec3(x0,y0,z0));
			box.addVertex(pgVec3(x1,y1,z1));
			box.addVertex(pgVec3(x2,y2,z2));

			c += 3;
		}

	}

	/*restoreDeviceObjects();

	if(singleTexStateBlock==0 || multiTexStateBlock==0)
		createStateBlocks();*/

	delete rawData;
	pgLog::info("MD2 mesh '%s' with %d triangles and %d frames created", nName, numTriangles, numFrames);

	return mesh;
}

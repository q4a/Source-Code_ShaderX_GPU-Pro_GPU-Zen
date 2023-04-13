///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBaseMesh
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBaseMesh.h,v 1.12 2003/01/05 20:45:56 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BASEMESH_HEADERFILE__
#define __BASEMESH_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Base/pgString.h>
#include <pgLib/Math/pgAABBox.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/Render/pgD3DObject.h>

#include <D3DX8.h>

class pgAABBox;


// flexible vertex format for meshes with one texture coordinate per vertex
//
#define D3DFVF_MESHVERTEX_TEX1 ( D3DFVF_XYZ | D3DFVF_TEX1 )

#define FILL_MESHVERTEX_TEX1A( v, ptx, pty, ptz, atu1, atv1 )  \
{   v.p.x = ptx;  v.p.y = pty;  v.p.z = ptz;  \
    v.t1.x = atu1;  v.t1.y = atv1;  \
}

#define FILL_MESHVERTEX_TEX1B( v, pt, tex )  \
{   v.p = pt;  \
    v.t1 = tex;  \
}


// flexible vertex format for non lit meshes
//
#define D3DFVF_MESHVERTEX_COL1 ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

#define FILL_MESHVERTEX_COL1A( v, ptx, pty, ptz, acolor, atu1, atv1 )  \
{   v.p.x = ptx;  v.p.y = pty;  v.p.z = ptz;  \
	v.color  = acolor;  \
    v.t1.x = atu1;  v.t1.y = atv1;  \
}

#define FILL_MESHVERTEX_COL1B( v, pt, acolor, tex )  \
{   v.p = pt;  \
	v.color  = acolor;  \
    v.t1 = tex;  \
}


// flexible vertex format for lit meshes
//
#define D3DFVF_MESHVERTEX_LIG1 ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )

#define FILL_MESHVERTEX_LIG1A( v, ptx, pty, ptz, nox, noy, noz, atu1, atv1 )  \
{   v.p.x = ptx;  v.p.y = pty;  v.p.z = ptz;  \
	v.n.x = nox;  v.n.y = noy;  v.n.z = noz;  \
    v.t1.x = atu1;  v.t1.y = atv1;  \
}

#define FILL_MESHVERTEX_LIG1B( v, pt, no, tex )  \
{   v.p = pt;  \
	v.n = no;  \
    v.t1 = tex;  \
}


// flexible vertex format for meshes with two texture coordinates per vertex
//
#define D3DFVF_MESHVERTEX_TEX2 ( D3DFVF_XYZ | D3DFVF_TEX2 )

#define FILL_MESHVERTEX_TEX2A( v, ptx, pty, ptz, atu1, atv1, atu2, atv2 )  \
{   v.p.x = ptx;  v.p.y = pty;  v.p.z = ptz;  \
    v.t1.x = atu1;  v.t1.y = atv1;  \
	v.t2.x = atu2;  v.t2.y = atv2;  \
}

#define FILL_MESHVERTEX_TEX2B( v, pt, tex, tex2 )  \
{   v.p = pt;  \
    v.t1 = tex;  \
	v.t2 = tex2;  \
}


/// pgBaseMesh represents a renderable generic mesh without texturing.
/**
 *  Therefore the basemesh can be shared by many meshes each using
 *  different textures. A pgBaseMesh can consist of several frames
 *  in order to animate an object. pgBaseMesh does NOT setup any
 *  render or blending stages.
 */
class PGLIB_API pgBaseMesh : pgD3DObject
{
friend class pgMeshUtil;
friend class pgXUtil;


public:
	enum {  MAX_VERTICES = 0xffff  };

	enum SETTING	{	SET_UNDEFINED =			0x0000,		/**< Undefined: indicates, that the mesh has not been set up yet  */
						SET_INDEXED =			0x0001,		/**< The mesh uses indexed vertices  */
						SET_TWEENINGSUPPORT =	0x0002,		/**< The mesh does not use vertex and index buffers (if renderTweened() ist used)  */
						SET_LIGHT =				0x0004,		/**< The mesh stores normal for light calculation  */
						SET_COLOR =				0x0008,		/**< The mesh stores a color value  */
						SET_TEXTURE1 =			0x0010,		/**< The mesh stores one texcoord value  */
						SET_TEXTURE2 =			0x0030,		/**< The mesh stores two texcoord values  */
						SET_TEXTURE3 =			0x0070,		/**< The mesh stores three texcoord values  */
						SET_TEXTURE4 =			0x00f0 };	/**< The mesh stores four texcoord values  */

	enum PRIMTYPE {	PRIM_UNDEFINED =        0,						/**< Undefined: indicates, that the mesh has not been created yet  */
					PRIM_POINTS =			D3DPT_POINTLIST,		/**< The mesh uses a list of points  */
					PRIM_LINES =			D3DPT_LINELIST,			/**< The mesh uses a list of lines  */
					PRIM_LINESTRIP =		D3DPT_LINESTRIP,		/**< The mesh uses one linestrip  */
					PRIM_TRIANGLES =		D3DPT_TRIANGLELIST,		/**< The mesh uses a list of triangles  */
					PRIM_TRIANGLESTRIP =	D3DPT_TRIANGLESTRIP,	/**< The mesh uses one trianglestrip  */
					PRIM_TRIANGLEFAN =		D3DPT_TRIANGLEFAN  };	/**< The mesh uses one trianglefan  */


	struct MESHVERTEX_TEX1
	{
		pgVec3		p;
		pgVec2		t1;
	};

	struct MESHVERTEX_COL1
	{
		pgVec3		p;
		DWORD		color;
		pgVec2		t1;
	};

	struct MESHVERTEX_LIG1
	{
		pgVec3		p, n;
		pgVec2		t1;
	};

	struct MESHVERTEX_TEX2
	{
		pgVec3		p;
		pgVec2		t1, t2;
	};

	class Frame
	{
	public:

		Frame(int nNumIndices=0, int nNumVertices=0, int nNumTriangles=0)
			{  numIndices = nNumIndices;  numVertices = nNumVertices;  numTriangles = nNumTriangles; 
			   vBuffer = NULL;  iBuffer = NULL;  vertices = NULL;  indices = NULL;  }

		bool createBuffers(int nSettings);
		bool fillBuffers(int nSettings);

		int						numIndices,
								numVertices,
								numTriangles;
		PDIRECT3DINDEXBUFFER8	iBuffer;
		PDIRECT3DVERTEXBUFFER8	vBuffer;
		void					*vertices;			// these two pointers are used if tweening is done
		WORD					*indices;			// instead of using static buffers
		pgAABBox				bBox;
	};


	pgBaseMesh();


	/// Renders frame number nFrame
	void render(int nFrame);


	/// Renders the mesh by tweening between the frames nFrame0 and nFrame1 by the weight nBlendWeight
	/**
	 *  This method does tweening in software
	 */
	void renderTweened(int nFrame0, int nFrame1, float nBlendWeight);


	/// This method returns the number of keyframes in the basemesh
	int getNumFrames() const  {  return frames.getSize();  }


	int addFrame(Frame* nFrame)  {  return frames.addTail(nFrame);  }


	// Returns the bounding box for the given frame
	pgAABBox* getBBox(int nIndex)  {  return &frames[nIndex]->bBox;  }


	virtual void render();


	pgPtrList<Frame>& getFrames()  {  return frames;  }

	void setRenderSettings(int nSet)  {  vertexSettings = nSet;  }

	int getRenderSettings() const  {  return vertexSettings;  }

	PRIMTYPE getPrimType() const  {  return primType;  }

	void setPrimType(PRIMTYPE nType)  {  primType = nType;  }

	static pgString getStringFromRenderSettings(int nSet);

	static int getRenderSettingsFromString(const pgString& nString);

	static pgString getStringFromPrimType(PRIMTYPE nType);

	static PRIMTYPE getPrimTypeFromString(const pgString& nString);

	static void* createVerticesForSettings(int nSettings, int nNumVertices);


	// implementation of pgRenderable interface

	virtual void deleteDeviceObjects();
	virtual bool restoreDeviceObjects();


	// should not be called from outside
	//
	static bool createBuffers(int nSettings, PDIRECT3DINDEXBUFFER8& iBuffer, int nNumIndices, PDIRECT3DVERTEXBUFFER8& vBuffer, int nNumVertices, bool nForceDynamicVertexBuffer=false);

protected:

	int						vertexSettings;
	PRIMTYPE				primType;

	pgPtrList<Frame>		frames;

	// these two buffers are only used if software tweening is done
	//
	PDIRECT3DINDEXBUFFER8	iBuffer;
	PDIRECT3DVERTEXBUFFER8	vBuffer;
};


#endif //__BASEMESH_HEADERFILE__


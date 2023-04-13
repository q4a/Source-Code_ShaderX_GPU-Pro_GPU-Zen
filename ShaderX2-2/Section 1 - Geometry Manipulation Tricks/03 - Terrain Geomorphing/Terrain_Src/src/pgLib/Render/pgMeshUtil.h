///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgMeshUtil
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMeshUtil.h,v 1.3 2002/12/12 21:15:28 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __MESHUTIL_HEADERFILE__
#define __MESHUTIL_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Math/pgVec3n.h>
#include <pgLib/Render/pgBaseMesh.h>


class pgAABBox;
class pgSegment;
class pgMaterial;
class pgMesh;

#ifndef DWORD
typedef unsigned long       DWORD;
#endif


/// Creates and updates pgBaseMesh and pgMesh objects
class PGLIB_API pgMeshUtil
{
public:
	/// Creates a box mesh with texture coordinates. All vertices get the given color.
	static pgBaseMesh*
		createBox(const pgAABBox& nBox, DWORD nColor);

	/// Updates the mesh to be a box with the given coordinates.
	/**
	 *  Number of vertices and indices must already be correct
	 *  (usually the mesh will already be a box, which only gets new coordinates)
	 */
	static bool
		updateBox(pgBaseMesh* nMesh, const pgAABBox& nBox, DWORD nColor);

	/// Creates an pgBaseMesh from an MD2 file
	/** 
	 *  If nLighting is true a mesh with lighting support
	 *  (normals, no fix vertex colors) is created
	 */
	static pgBaseMesh*
		loadMD2(const char* nName, bool nLighting=false);

	/// Creates an pgMesh from an OBJ file
	/** 
	 *  If nLighting is true a mesh with lighting support (normals) is created
	 */
	static pgMesh*
		loadOBJ(const char* nName, bool nLighting=true);


protected:
	struct OBJInfo
	{
		const char* nName;
		bool nLighting;
		pgList<pgVec3> vertices;
		pgList<pgVec2> texCoords;
		pgList<pgVec2> texCoords2;
		pgList<pgVec3n> normals;
		pgIntList vIndices;
		pgIntList nIndices;
		pgIntList tIndices;
		pgIntList tIndices2;
		pgIntList lengths;
	};

	static bool
		setDevice(pgBaseMesh* nMesh, const char* nError);

	/*static bool
		fillFrameOBJ(const char* nName, bool nLighting, pgMaterial* nMaterial,
					 pgMesh* mesh, pgSegment* segment, pgBaseMesh* baseMesh, pgBaseMesh::Frame* frame,
					 pgList<pgVec3>& vertices, pgList<pgVec2>& texCoords, pgList<pgVec2>& texCoords2, pgList<pgVec3n>& normals,
					 pgIntList& vIndices, pgIntList& nIndices, pgIntList& tIndices, pgIntList& tIndices2, pgIntList& lengths);*/

	static bool
		fillFrameOBJ(pgMaterial* nMaterial, pgMesh* mesh, pgSegment* segment, pgBaseMesh* baseMesh, pgBaseMesh::Frame* frame,
					 const OBJInfo& nOBJInfo);

	static bool
		initObjects(pgMesh* &mesh, pgSegment* &segment, pgBaseMesh* &baseMesh, pgBaseMesh::Frame* &frame);

	static bool
		loadMTL(const char* nName, pgPtrList<pgMaterial>& nMaterials);
};


#endif //__MESHUTIL_HEADERFILE__

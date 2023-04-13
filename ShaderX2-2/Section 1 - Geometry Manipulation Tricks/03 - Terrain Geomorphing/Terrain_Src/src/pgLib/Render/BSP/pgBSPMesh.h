///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPMesh
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPMesh.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BSPMESH2_HEADERFILE__
#define __BSPMESH2_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>
#include <pgLib/Render/BSP/pgBSPFile.h>

#include <d3d8.h>

class Face;
class Brush;
class Batch;
class Shader;

class pgAABBox;
class pgVec3;
class pgVec3n;

struct BSPVERTEX;

/// pgBSPMesh can load and render Q3 mesh objects
/**
 *
 */
class PGLIB_API pgBSPMesh : public pgD3DObject
{
public:
	pgBSPMesh();

	/// Limits of the pgBSPMesh class
	enum {
			LIGHTMAP_SIZE = 128,		/**<  Size of one lightmap  */
			MAX_VERTICES = 65536,		/**<  Maximum number of vertices to be drawn at once  */
			MAX_INDICES =  65536		/**<  Maximum number of indices to be drawn at once  */
	};

	/// Loads the ibsp and the corresponding shader file
	bool load(const char* nBSPFileName);


	/// Sets the subdivision factor for bezier patches
	/**
	 *  The number of faces created for a bezier patch
	 *  is (2^n * 2^n). A value of 6 is therefore
	 *  most times more than enough.
	 */
	void setSubFact(int nSubFact)  {  patchSubFact = nSubFact;  }


	/// Renders the bsp with the current camera position
	void render();


	/// Fills a string with statistics about the level
	const char* getReportString();


	/// Slides a sphere with radius nRadius with planned movement from nPos0 to nPos1 through the bsp level
	/**
	 *  The final position is returned in nFinalPos
	 *  This method returns true if there was a collision
	 */
	bool slideSphere(const pgVec3& nPos0, float nRadius, const pgVec3& nPos1, pgVec3& nFinalPos);

	/// Returns true if the sphere collides with the bsp level
	/**
	 *  During movement from nSpherePos with nVelocity
	 *  Returns the point of the sphere which will collide with a triangle (nNearestColSpherePos)
	 *          the point on a triangle with which it will collide (nNearestColTriPos)
	 *          the distance to go until the collision (nNearestDistance)
	 *          stuck as true if the sphere was already colliding on the initial position
	 *          true if there actually will be a collision
	 */
	bool collideSphere(const pgVec3& nSpherePos, float nRadius, const pgVec3& nVelocity,
					   pgVec3& nNearestColSpherePos, pgVec3& nNearestColTriPos, float& nNearestDistance, bool& nStuck);


	/// Returns a position near nSavePos which does not collide with the level
	pgVec3 findSavePos(const pgVec3& nSavePos, float nRadius);


	void deleteDeviceObjects();
	bool restoreDeviceObjects();


private:
	int						curCluster, curLeaf;

	pgVec3					slideSphereSub(const pgVec3& nPos, float nRadius, const pgVec3& nDir, pgVec3& nSavePos, int nRecDepth);

	int						findLeaf(const pgVec3& nPos);
	int						findCluster(const pgVec3& nPos);
	bool					isLeafVisible(int nLeaf);
	bool					isPointVisible(float nX, float nY, float nZ);
	pgTexturePtr			createLightmap(const unsigned char* nData);

	void					findBrushes(const pgAABBox& nBBox, int nNode=0);
	void					findFaces(const pgAABBox& nBBox, int nNode=0);

	void					renderWithLightmapIndex(int lm, BSPVERTEX*	&pVertices, WORD* &pIndices, int &numI, int &numV);
	void					renderLightmapBatches();


	int						patchSubFact;

	// shader stuff
	//
	void					loadShaders(const char* nFileName);
	bool					loadShaderTextures();
	Shader*					getShader(const char* nShaderName);
	void					updateShaders();


	// this list is filled by findBrushes()
	//
	int						numFoundBrushes;
	int						*foundBrushes;

	// this list is filled by findFaces()
	//
	int						numFoundFaces;
	int						*foundFaces;

	char					reportStr[256];

	// this value is non zero if the sphere did collide
	//
	int						didCollide;

	// data structures for the level itself...
	//
	int						numShaders;
	Shader					*shaders;

	int						numTextures;
	pgTexturePtr			*textures;

	int						nupgightmaps;
	pgTexturePtr			*lightmaps;

	PDIRECT3DINDEXBUFFER8	iBuffer;
	PDIRECT3DVERTEXBUFFER8	vBuffer;

	int						numBatches;
	Batch*					batches;
	int						numStripLens;
	int						*stripIndices;
	int						*stripTris;

	int						numFaces;
	Face					*faces;
	int						**numMaterialFaces;
	int						**maxMaterialFaces;
	int						***materialFaces;

	int						numClusters;
	int						*numClusterLeafs;
	int						**clusterLeafs;
	bool					**clusterVis;

	int						nupgeafs;
	int						*nupgeafFaces;
	int						**leafFaces;

	int						numBrushes;
	Brush					*brushes;

	bsp_lump_nodes*			nodes;
	bsp_lump_leafs*			leafs;
	bsp_lump_planes*		planes;
	bsp_lump_leafbrushes*	leafbrushes;

	// statistics
	//
	int						trisRendered, verticesRendered, matsRendered;


};


#endif //__BSPMESH2_HEADERFILE__

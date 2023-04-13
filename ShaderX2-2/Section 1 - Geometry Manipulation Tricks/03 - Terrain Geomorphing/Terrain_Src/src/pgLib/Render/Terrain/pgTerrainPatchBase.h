///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrainPatchBase
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrainPatchBase.h,v 1.11 2002/12/08 16:36:23 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TERRAINPATCHBASE_HEADERFILE__
#define __TERRAINPATCHBASE_HEADERFILE__

#include <D3DX8.h>

#include <pgLib/pgLib.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Render/pgD3DObject.h>

class pgTerrain;


#define D3DFVF_PATCHVERTEX ( D3DFVF_XYZ | D3DFVF_TEX1 )


class PGLIB_API pgTerrainPatchBase : public pgD3DObject
{
public:
	enum VISFLAGS  {  VIS_BASE = 1,
					  VIS_LAYER0 = 2,
					  VIS_LAYER1 = 4,
					  VIS_LAYER2 = 8,
					  VIS_LAYER3 = 16,
					  VIS_LAYER4 = 32,
					  VIS_LAYERALL = 63 };

	enum {	MAX_SUBDIV = 4,
			SOURCE_WIDTH = 17,
			SOURCE_HEIGHT = 17,
			MAX_VERTICES = SOURCE_WIDTH * SOURCE_HEIGHT,
			MAX_INDICES = MAX_VERTICES*3	};

	enum {  LEFT = 1,
			RIGHT = 2,
			TOP = 4,
			BOTTOM = 8  };



	struct PATCHVERTEX
	{
		pgVec3		pos;
		pgVec2		tex;
	};


	pgTerrainPatchBase(pgTerrain* nMaster, int nX, int nY);


	virtual void init();

	virtual void deinit();

	virtual int render()  {  return 0;  }

	virtual bool updateTessellation();

	virtual bool updateTessellation2();

	virtual bool updateTessellation3();

	virtual void deleteDeviceObjects();

	virtual bool restoreDeviceObjects();

	virtual int fillBaseVertices(pgTerrainPatchBase::PATCHVERTEX* nDest);


	void setRendered(bool nSet)  {  rendered = nSet;  }

	bool getRendered() const  {  return rendered;  }

	void setVisFlags(int nFlags)  {  visFlags = nFlags;  }

	int getVisFlags() const  {  return visFlags;  }

	void setMaster(pgTerrain* nMaster)  {  master = nMaster;  }

	void setScale(const pgVec3& nScale);

	void addToActiveList();

	void removeFromActiveList();

	void setHeightMapPosition(int nX0, int nY0);


	float getExactTessellation() const;

	float getCurrentError() const  {  return currentError;  }


	// returns the current tessellation:
	//   0...maximum details (~2*SOURCE_WIDTH*SOURCE_WIDTH triangles)
	//   4...minimum details (2 triangles)
	int getCurrentTessellation() const  {  return selfTes;  }

	int getNewTessellation() const  {  return newSelfTes;  }

	int getNewTessellationSafe() const;

	int getRealTessellation() const  {  return realSelfTes;  }

	int getNumIndices() const  {  return numNewIndices;  }

	int getNumVertices() const  {  return numNewVertices;  }

	const PATCHVERTEX* getVertices() const  {  return vertices;  }

	const WORD* getIndices() const  {  return indices;  }

	
	bool checkVisibility(const D3DXMATRIX* nWorldMatrix, const pgPlane nPlanes[6]);

	void updateVisibility(const D3DXMATRIX* nWorldMatrix, const pgPlane nPlanes[6]);



	void calcErrors();

	bool isVisible() const  {  return visible;  }

	bool isActive() const  {  return active;  }

	void setVisible(bool nVis);

	void updateProjectedErrors(const D3DXMATRIX* nWorldMatrix);

	float getProjectedError(int nWhich)  {  return errors[nWhich].diff;  }

	void setRealTessellation(int nTes);

	void setTessellation(int nTes)  {  newSelfTes = nTes;  }

	class pgTerrainPatchBase* getNextActive()  {  return next;  }

	int getGridPosX() const  {  return gridX;  }

	int getGridPosY() const  {  return gridY;  }


	void setFlagActive(bool nSet)  {  flagActive = nSet;  }

	bool getFlagActive() const  {  return flagActive;  }

	void setPosition(const pgVec3& nPos)  {  pos = nPos;  }

	void setNeighbors(const pgTerrainPatchBase* nLeft, const pgTerrainPatchBase* nRight, const pgTerrainPatchBase *nUp, const pgTerrainPatchBase* nDown);


	static int getAddLayerFlag(int nLayer)  {  return 1 << nLayer;  }

	void createTessellation(int nCenter, int nLeft, int nRight, int nUp, int nDown);


	const pgTerrainPatchBase* getLeft() const  {  return left;  }

	const pgTerrainPatchBase* getRight() const  {  return right;  }

	const pgTerrainPatchBase* getBottom() const  {  return bottom;  }

	const pgTerrainPatchBase* getTop() const  {  return top;  }

	void calcMinMaxY();

protected:

	static int getPowerTwo(int nVal);
	unsigned short getIndex(int nX, int nY);

	float getHeight(int nX, int nY) const;
	void getVertex(int nX, int nY, pgVec3& nPoint) const;

	void getTexCoord(int nX, int nY, pgVec2& nTCoord) const;
	void getNormal(int nX, int nY, pgVec3& nNormal) const;

	void calcError(int nTes);

	void addIndex(unsigned short nIdx);
	void addLastIndexAgain();

	void reduceShapeTo(int nTes, PATCHVERTEX *nData);
	void makeBordersSimpler(PATCHVERTEX *nData);
	void makeSimpler(int nTes, PATCHVERTEX *nData);
	void calculateInbetweenVertex(int x, int y, int xc, int yc, int pow, PATCHVERTEX *nData);
	int calculateDiagonalVertices(int x, int y, int xc, int yc, int pow, PATCHVERTEX *nData,
								  float& nHeightLeftTop, int& idxLT, float& nHeightRightBottom, int& idxRB);

	void addLevel4PatchTriangles(bool nLeft, bool nRight, bool nBottom, bool nTop);
	void addTriangleRow(int nY, int nPow, bool nLeft, bool nRight, bool nBottom, bool nTop);
	void addTriangleBottomRow(int nPow, bool nLeft, bool nRight);
	void addTriangleTopRow(int nPow, bool nLeft, bool nRight);


	struct Error
	{
		Error()  {  diff = 0.0f;  }
		pgVec3	correct, real;
		float	diff;
	};

	//RENDERMODE				renderMode;
	int						visFlags;

	pgTerrain*				master;

	int						gridX,gridY;

	bool					active;

	bool					visible;
	bool					forceBufferCreate;

	Error					errors[MAX_SUBDIV+2];
	float					currentError;

	int						newSelfTes, selfTes, oldRealSelfTes, newRealSelfTes, realSelfTes, leftTes,rightTes,topTes,bottomTes;
	pgVec3					pos, scale;
	float					minY,maxY;
	int						hmX0, hmY0;

	int						numVertices, numIndices,
							numNewVertices, numNewIndices;

	int						frameIdx;

	// for linking in a list of active patches
	//
	pgTerrainPatchBase		*next, *prev;
	bool					flagActive;


	// neighbors
	//
	const pgTerrainPatchBase	*left, *right, *top, *bottom;

	// dynamic data
	//
	unsigned short			*indexMap;
	PATCHVERTEX				*vertices; //, *verticesLow;
	unsigned short			*indices;

	bool					rendered;
	bool					lowVertMethod2;
	unsigned char			useNeightbors;
};


inline void
pgTerrainPatchBase::addIndex(unsigned short nIdx)
{
	indices[numNewIndices++] = nIdx;
}


inline void
pgTerrainPatchBase::addLastIndexAgain()
{
	if(numNewIndices>0)
		indices[numNewIndices++] = indices[numNewIndices-1];
}



typedef pgTerrainPatchBase		*pgTerrainPatchBasePtr;
typedef pgTerrainPatchBasePtr	*pgTerrainPatchBasePPtr;


#endif //__TERRAINPATCHBASE_HEADERFILE__

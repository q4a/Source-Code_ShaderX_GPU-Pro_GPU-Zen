///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgComplexTree
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgComplexTree.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __COMPLEXTREE_HEADERFILE__
#define __COMPLEXTREE_HEADERFILE__

#include <pgLib/pgLib.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Math/pgVec3n.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>


class PGLIB_API pgComplexTree : public pgD3DObject
{
public:

	pgComplexTree();

	void addBranch(const pgList<pgVec3>& nPosList, int nNumSides,
				   float nRadius0, float nRadius1, const pgVec3n& nBaseDir);

	void create();

	void render();

	bool loadFromFile(const pgString& nFile);

	virtual void deleteDeviceObjects()  {  freeBuffers();  }
	virtual bool restoreDeviceObjects()  {  return createBuffers();  }

protected:
	struct VERTEX
	{
		pgVec3		pos;
		DWORD		color;
		pgVec2		tex;

		bool operator==(const VERTEX&)  {  return false;  }
	};

	struct CBranch
	{
		CBranch() : texRepeat(1.0f, 1.0f)  {  numSides=0;  radius0 = radius1 = 0.0f;  }
		CBranch(const pgList<pgVec3>& nKnotList, int nNumSides, float nRadius0, float nRadius1, const pgVec3n& nBaseDir);

		pgList<pgVec3>	knotList;
		int				numSides;
		float			radius0, radius1;
		pgVec3n			baseDir;
		pgVec2			texRepeat;
	};

	void addCylinder(const CBranch& nBranch, pgList<WORD>& nIList, pgList<VERTEX>& nVList);
	void addLeaf(const pgVec3& nPos, const pgVec3n& nNormal, float nAngle, float nSizeU, float nSizeV,
				 float repeatX, float repeatY, pgList<WORD>& nIList, pgList<VERTEX>& nVList);
	void addLeafs(int nNumLeafs, const pgVec3& nPos, const pgVec3& nSize, float nFuzzy,
				  pgList<WORD>& nIList, pgList<VERTEX>& nVList);
	void freeBuffers();
	bool createBuffers();

	pgVec3					pos;

	pgList<CBranch*>		branches;
	pgTexturePtr			branchTex, leafTex;

	PDIRECT3DINDEXBUFFER8	iBuffer;
	PDIRECT3DVERTEXBUFFER8	vBuffer;

	int						numBranchIndices,
							numLeafIndices,
							numIndices,
							numVertices;

	WORD*					indices;
	VERTEX*					vertices;
};


#endif //__COMPLEXTREE_HEADERFILE__

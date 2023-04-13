///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBlendWood
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBlendWood.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BLENDWOOD_HEADERFILE__
#define __BLENDWOOD_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/Tree/pgBlendTree.h>


class PGLIB_API pgBlendWood : public pgD3DObject
{
public:
	pgBlendWood();

	void addTree(pgBlendTree* nTree, const pgVec3& nPos)  {  trees.addTail(new TreeInfo(nTree, nPos));  SAFE_RELEASE(vBuffer);  sorted = false;  }

	void sort();

	void update(const pgVec3& nViewerPos, const pgVec3& nViewerDir);

	void render();


	virtual void deleteDeviceObjects()  {  SAFE_RELEASE(vBuffer);  }
	virtual bool restoreDeviceObjects()  {  return createBuffer();  }


protected:
	bool createBuffer();

	struct TreeInfo
	{
		TreeInfo()  {}
		TreeInfo(pgBlendTree *nTree, const pgVec3& nPos)  {  tree = nTree;  pos = nPos;  tex0 = tex1 = NULL;  textureFactor = 0;  }

		pgBlendTree		*tree;
		pgVec3			pos;
		pgTexture		*tex0, *tex1;
		DWORD			textureFactor;
		unsigned char	flag;

		//friend bool																	// operator== needed for pgList
		//	operator==(const TreeInfo&, const TreeInfo&)  { return false; }
	};


	pgPtrList<TreeInfo>		trees,
							xSortedTrees, zSortedTrees,
							*sortedTrees;

	bool					drawDown, sorted;

	PDIRECT3DVERTEXBUFFER8	vBuffer;
};


#endif //__BLENDWOOD_HEADERFILE__

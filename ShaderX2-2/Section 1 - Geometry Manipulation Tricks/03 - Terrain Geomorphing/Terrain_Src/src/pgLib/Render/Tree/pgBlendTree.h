///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBlendTree
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBlendTree.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BLENDTREE_HEADERFILE__
#define __BLENDTREE_HEADERFILE__

#include <pgLib/pgLib.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>


#define D3DFVF_BLENDTREE_VERTEX ( D3DFVF_XYZ | D3DFVF_TEX1 )


class PGLIB_API pgBlendTree : public pgD3DObject
{
public:
	enum {  NUM_VERTICES = 4  };

	struct VERTEX
	{
		pgVec3		pos;
		pgVec2		tex1;
	};


	/// initializes the tree
	/*
	 *  nSizeX and nSizeY determine the size of the tree in width and height (since
	 *  the tree has no depth). nSelfRender is true a vertex buffer is created and
	 *  the tree can be rendered by calling render(). in order to render a large number
	 *  of same trees it is wiser to render them by a higher instance.
	 *
	 *  create() has to be called before the tree can actually be rendered.
	 */
	pgBlendTree(float nSizeX, float nSizeY, bool nSelfRender=true);

	/// sets the position of the tree
	/* 
	 *  the position set position resembles the middle at bottom of the quad
	 */
	void setPosition(const pgVec3& nPos)  {  pos = nPos;  }

	/// adds a texture to tree
	/*
	 *  textures can be added at any time. when the tree is rendered ist called the
	 *  corresponding textures are calculated by the angle at which the tree is
	 *  viewed and by the number of textures previously added.
	 */
	void addTexture(pgTexturePtr nTexture);


	void calcSettings(const pgVec3& nTreePos, const pgVec3& nViewerPos, const pgVec3& nViewerDir,
					  VERTEX* nVertices, pgTexture*& nTex0, pgTexture*& nTex1, DWORD& nTextureFactor) const;


	/// renders the tree
	/*
	 *  the tree can only be rendered if it was created with nSelfRender set to true
	 *  since the tree has to face to the viewer, the viewer's position has to
	 *  be passed.
	 */
	void render(const pgVec3& nViewerPos, const pgVec3& nViewerDir);


	virtual void deleteDeviceObjects()  {  destroy();  }
	virtual bool restoreDeviceObjects()  {  return createBuffer();  }


protected:
	bool createBuffer();
	void destroy();

	bool					selfRender;

	pgPtrList<pgTexture>	textures;
	float					sizeX, sizeY;

	pgVec3					pos;

	PDIRECT3DVERTEXBUFFER8	vBuffer;
	VERTEX*					vertices;
};


#endif //__BLENDTREE_HEADERFILE__

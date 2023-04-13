///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgSimpleTree
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSimpleTree.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __SIMPLETREE_HEADERFILE__
#define __SIMPLETREE_HEADERFILE__

#include <pgLib/pgLib.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>


class PGLIB_API pgSimpleTree : public pgD3DObject
{
public:
	struct VERTEX
	{
		pgVec3		pos;
		pgVec2		tex;
	};

	pgSimpleTree(pgTexture* nTexture, int nNumPlanes, float nSizeX, float nSizeY,
				 float nMinX, float nMinY, float nMaxX, float nMaxY, bool nSelfRender=true);

	void setPosition(const pgVec3& nPos)  {  pos = nPos;  }

	void render();

	void create();
	void destroy();


protected:
	bool					selfRender;

	pgTexturePtr			texture;
	int						numPlanes;
	float					sizeX, sizeY;
	float					tcMinX, tcMinY, tcMaxX, tcMaxY;

	pgVec3					pos;

	int						numI, numV;
	PDIRECT3DINDEXBUFFER8	iBuffer;
	PDIRECT3DVERTEXBUFFER8	vBuffer;

	VERTEX*					vertices;
	WORD*					indices;
};


#endif //__SIMPLETREE_HEADERFILE__

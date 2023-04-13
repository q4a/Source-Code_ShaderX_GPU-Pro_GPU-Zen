///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgMesh
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMesh.h,v 1.4 2002/12/15 20:04:55 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __MESH_HEADERFILE__
#define __MESH_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Render/pgSegment.h>
#include <pgLib/Render/pgTexture.h>
#include <pgLib/Render/pgTransformable.h>

class pgAABBox;


/// A pgMesh is a complex 3D object which can consist of several segments.
/**
 *  A pgMesh object contains one or more segment in order to support more than
 *  one texture per mesh. A pgMesh object has to be created/loaded by pgMeshUtil.
 */
class PGLIB_API pgMesh : public pgTransformable
{
friend class pgMeshUtil;
friend class pgXUtil;
public:
	pgMesh();


	/// Adds a segment to the mesh.
	/**
	 *  The mesh can constist of any number of segments
	 */
	void addSegment(pgSegment* nSegment)  {  segments.addTail(nSegment);  }

	int getNumSegments() const  {  return segments.getSize();  }

	pgSegment* getSegment(int nIndex)  {  return segments[nIndex];  }

	void removeSegment(int nIndex)  {  segments.removeIndex(nIndex);  }


	/// Returns the number of frames the segments have
	/**
	 *  It's considered that each segment has the same number of frames.
	 */
	int getNumFrames() const  {  return segments.getSize() ? segments[0]->getNumFrames() : 0;  }


	void deleteDeviceObjects();
	bool restoreDeviceObjects();

	
	/*void setPosition(const pgVec3& nPos)	{  if (transform)  transform->setPosition(nPos);}
	void setRotation(const pgVec3& nRot)	{  if (transform)  transform->setRotation(nRot);}
	void setScale(const pgVec3& nScale)		{  if (transform)  transform->setScale (nScale);}

	void move(const pgVec3& nPos)			{  if (transform)  transform->move  (nPos);		}
	void rotate(const pgVec3& nRot)			{  if (transform)  transform->rotate(nRot);		}
	void scale(const pgVec3& nScale)		{  if (transform)  transform->scalef(nScale);	}*/



	/// Renders the mesh by blending each segement between two frames.
	/**
	 *  this methods expects the mesh to have at least nMaxFrame frames
	 *  nFrame must be between 0.0 and 1.0. the method automatically
	 *  determines between which two frames it has to tween
	 *  nTexture0 must always be !=NULL. if nTexture1 is !=NULL and
	 *  nTexBlend != 0.0 then a blending between the two textures is performed
	 */
	int renderTweenedWithBlendTextures(float nFrame, int nMinFrame, int nMaxFrame,
									   pgTexturePtr nTexture0, pgTexturePtr nTexture1, float nTexBlend);


	/// Renders the mesh by blending each segement between two frames.
	/**
	 *  Works exactly the as renderTweenedWithBlendTextures, except that
	 *  the texture settings are used from the segments' stage settings
	 */
	int renderTweened(float nFrame, int nMinFrame, int nMaxFrame);


	/// Renders the set frame. if nSegment is -1 all segments are rendered
	void render(int nFrame=0, int nSegment=-1);


	/// Returns a bounding box which contains the frame number nIndex in all segments
	pgAABBox getBBox(int nIndex);

	pgPtrList<pgSegment>& getSegments()  {  return segments;  }

protected:
	pgPtrList<pgSegment>	segments;
};


#endif //__MESH_HEADERFILE__

///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgAnimated
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgAnimated.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __ANIMATED_HEADERFILE__
#define __ANIMATED_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Base/pgString.h>
#include <pgLib/Base/pgITime.h>
#include <pgLib/Math/pgAABBox.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>

#include <D3D8.h>

class pgMesh;


/// A pgAnimated object resembles an animated object such as a player character.
/**
 *  A pgAnimated object is constructed by loading an .ani file.
 *  Several animations can be stored in one object, which can each
 *  be started by calling startAnimation()
 */
class PGLIB_API pgAnimated : public pgD3DObject
{
public:
	/// Type of operation done when changing between animation frames.
	enum FRAMEOP {
					BLEND,		/**<  Do vertex blending for smooth animation  */
					SWITCH		/**<  Just switch between frames (faster but looks awfull  */
	};

	pgAnimated();


	/// Loads the given .char file
	/**
	 *  See the specification in the manual
	 */
	bool load(const char* nFileName);


	/// Starts the animation with the given index
	/**
	 *  If the index is invalid the animation nr 0 is started
	 */
	void startAnimation(int nAnim);


	/// starts the animation with the given name
	void startAnimation(const char* nName);


	/// Returns the index of the given animaion name
	/**
	 *  If the name is invalid -1 is returned
	 */
	int getAnimationIndex(const char* nName);


	/// Returns the mesh that is currently in use
	pgMesh* getActiveMesh()  { return anims[curAnim]->mesh; }


	/// Returns the bounding box which is formed by the current mesh
	const pgAABBox& getCurrentBBox() const  {  return curBBox;  }


	/// Returns the name of the object
	const pgString& getName()  { return name; }


	/// Returns true if the mesh animation did loop during the last update
	/**
	 *  For several reasons it can be useful for the owner of a pgAnimated
	 *  object to know when the animation was finished and restarted because
	 *  looping is active.
	 */
	bool getDidMeshLoop() const  {  return didMeshLoop;  }


	/// Resets the didloop flag
	void resetDidMeshLoop()  {  didMeshLoop = false;  }

	/// If nFreeze is true, the animation is stopped (frozen)
	void setFreeze(bool nFreeze)  {  freeze = nFreeze;  }


	/// If nHidden is true, the object is not drawn until it is unhidden.
	/**
	 *  The animation goes on (is updated), but the object is not drawn.
	 *  Call setFreeze(true) if the animation should stop too.
	 */
	void setHidden(bool nHidden)  {  hidden = nHidden;  }


	/// Renders the current underlying mesh.
	virtual void render();


	// implementation of pgRenderable
	virtual void setPosition(const pgVec3& nPos)  {  pos = nPos;  }
	virtual void setRotation(const pgVec3& nRot)  {  rot = nRot;  }
	virtual void deleteDeviceObjects();
	virtual bool restoreDeviceObjects();


protected:
	class AnimInfo
	{
	public:
		AnimInfo()  {  texOp = SWITCH;  meshOp = BLEND;  minFrame=maxFrame=0;  meshDur = texDur = 1.0f;  mesh = NULL;  }

		pgString				name;
		pgString				meshFileName;
		int						minFrame, maxFrame;
		FRAMEOP					meshOp, texOp;
		float					meshDur, texDur;
		bool					culling;
		pgList<pgString>		texNames;
		pgPtrList<pgTexture>	textures;
		pgMesh*					mesh;
		pgVec3					offset, rotation, scale;
	};

	pgString							name;
	pgPtrList<AnimInfo>	anims;


	pgVec3				pos, rot;

	int					curAnim;

	bool				didMeshLoop;
	bool				freeze;
	bool				hidden;
	float				meshTime, texTime;

	pgAABBox			curBBox;
};

typedef pgAnimated* pgAnimatedPtr;


#endif //__ANIMATED_HEADERFILE__

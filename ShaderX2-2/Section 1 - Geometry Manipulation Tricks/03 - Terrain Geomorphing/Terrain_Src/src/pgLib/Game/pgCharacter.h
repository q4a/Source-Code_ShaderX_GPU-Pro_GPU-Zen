///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgCharacter
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgCharacter.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __CHARACTER_HEADERFILE__
#define __CHARACTER_HEADERFILE__


#include <pgLib/pgLib.h>
#include <D3DX8.h>

#include <pgLib/Audio/pgISample.h>

#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Base/pgString.h>

#include <pgLib/Math/pgAABBox.h>

#include <pgLib/Render/pgMesh.h>
#include <pgLib/Render/pgTexture.h>

class pgAnimated;
class pgParticleSystem;
class pgPath;


/// Animated Character Class
/**
 *  This class provides functionality to created
 *  scripted objects which can react onto events
 *  during the game.
 */
class PGLIB_API pgCharacter
{
public:
	/// Type of way a character move along.
	enum WAY  {
					WAY_LINE,	/**< Moves along a line (defined by points in the .CHAR file)  */
					WAY_PATH	/**< Moves along a path (defined by many points in a .PATH file)  */
	};


	/// Type of relation between the character and a (trigger or collision) box or a particle system.
	enum RELATION  {
						REL_NONE,		/**<  No relation  */
						REL_ABS,		/**<  The box is initially positioned in absolute coordinates  */
						REL_REL,		/**<  The box is initially positioned relativ to the character  */
						REL_LOCAL,		/**<  Emitted particles stay in the coordinate system of the character (follow each move)  */
						REL_GLOBAL		/**<  Emitted particles move independent of the character (do not follow)  */
	};


	/// Type of relation between the character and a (trigger or collision) box
	enum MOVEMENT  {
						MOV_FOLLOW,		/**<  The box moves together with the character  */
						MOV_FIX			/**<  The box does not moves with the character (stays at its initial position  */
	};


	/// Type of events
	enum EVENT  {
					EVENT_UNDEFINED,	/**<  No event  */
					EVENT_STARTUP,		/**<  Startup event; sent after programm start  */
					EVENT_GUIDE,		/**<  Event by the Guide  */
					EVENT_PLAYER		/**<  Event by the Player  */
	};

protected:
	class PGLIB_API Particles
	{
	public:
		Particles()  {  psys = NULL;  rel = REL_NONE;  initialEmit = 0;  /*referenced = false;*/  }
		Particles(const char* nName, const char* nPSysName, RELATION nRel, int nInitialEmit, const pgVec3& nEmitRelPos)
			{  name = nName;  psysName=nPSysName;  rel = nRel;  initialEmit = nInitialEmit;  emitRelPos = nEmitRelPos;  /*referenced = false;*/  }

		pgString			name, psysName;
		pgParticleSystem*	psys;
		RELATION			rel;
		pgVec3				emitRelPos;
		int					initialEmit;
		bool				updated;

		friend bool																	// operator== needed for pgList
			operator==(const Particles&, const Particles&)  { return false; }
	};

	class SubAction
	{
	public:
		SubAction()  {  duration = 0.0f;  sample = NULL;  }
		SubAction(const char* nAnimName)  {  animName=nAnimName;  duration = 0.0f;  sample = NULL;  }

		pgString				animName;
		pgList<pgString>		particlesNameList;
		pgPtrList<Particles>	particlesList;
		float					duration;
		pgISample*				sample;

		friend bool																	// operator== needed for pgList
			operator==(const SubAction&, const SubAction&)  { return false; }
	};

	class Action
	{
	public:
		Action()  {  postActionNr = -1;  }

		pgString			name;
		int					priority;
		float				speed;
		pgList<SubAction>	subActions;
		pgString			postActionName;
		int					postActionNr;

		friend bool																	// operator== needed for pgList
			operator==(const Action&, const Action&)  { return false; }
	};

	class PGLIB_API Box
	{
	public:
		Box()  {}
		Box(const char* nName, RELATION nRel, MOVEMENT nMov, const pgAABBox& nBox)
			{  name = nName;  rel = nRel;  mov = nMov;  box = box0 = nBox;  }

		pgString	name;
		RELATION	rel;
		MOVEMENT	mov;
		pgAABBox	box0, box;

		friend bool																	// operator== needed for pgList
			operator==(const Box&, const Box&)  { return false; }
	};

	class PGLIB_API Event
	{
	public:
		Event()  {}
		Event(EVENT nType, const char* nBoxName, const char* nActionName)
			{  type = nType;  boxName = nBoxName;  actionName = nActionName;  }

		EVENT type;
		pgString boxName, actionName;
		int boxIdx, actionIdx;

		friend bool																	// operator== needed for pgList
			operator==(const Event&, const Event&)  { return false; }
	};

	class PGLIB_API Setting
	{
	public:
		pgString			animName;
		bool				drawBoxes;
		int					hitpoints;
		bool				collisionOn;
		float				activeMin, activeMax;

		WAY					way;
		pgString			pathName;
		bool				loopPath;
		pgVec3				pos0, pos1;

		float				hideTime;
	};


public:

	pgCharacter();

	/// Loads a character from a .char file.
	/**
	 *  If nFullPath is true the filename is used directly.
	 *  Otherwise it is extended by the default path and extension
	 */
	bool load(const char* nFileName, bool nFullPath=false);

	/// Tells the character, that an event has happened.
	/**
	 *  Isually every frame there will be events such as
	 *  player, guide, etc...
	 */
	void setEvent(EVENT nEvent, const pgAABBox* nBox);


	/// Converts a string into an event
	EVENT getEventTypeFromString(pgString nEventStr);


	/// Updates the character before it is rendered
	void update();


	/// Sets how far the game progress currently is.
	/**
	 *  Values must be between 0.0 and 1.0 this value is used to 
	 */
	bool setGameProgress(float nVal);


	/// Renders the character
	virtual void render();

	void setPosition(const pgVec3& nPos)  {  pos = nPos;  }
	void setRotation(const pgVec3& nRot)  {  rot = nRot;  }
	virtual void deleteDeviceObjects();
	virtual bool restoreDeviceObjects();

protected:
	int getActionIndex(const pgString nName);
	int getBoxIndex(const pgString nName);
	pgAABBox& getEventBox(const Event& event);
	void updatePosition();
	void applyCurrentAction();
	void stopAllParticleSystems();
	Particles* getParticles(const pgString& nName);

	Setting				setting;

	pgVec3				pos, rot;

	pgAnimated			*anim;
	pgAABBox			collBox;
	bool				active;

	pgPath*				path;

	Action				*curAction;
	int					curSubAction;
	float				scalarPos;

	float				hideTime;

	Setting				info;

	pgList<Action>		actions;
	pgList<Box>			boxes;
	pgList<Event>		events;
	pgList<Particles>	particles;


	pgMesh				boxMesh;
	pgTexture			*borderYellow, *borderGreen;
};

typedef pgPtrList<pgCharacter> pgCharacterList;

#endif //__CHARACTER_HEADERFILE__


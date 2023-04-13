///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgParticleSystem
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgParticleSystem.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __PARTICLESYSTEM_HEADERFILE__
#define __PARTICLESYSTEM_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgString.h>
#include <pgLib/Math/pgVec3n.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>

#include <D3D8.h>


/// Creates and animates a particle system from a .ps file
/**
 *  For more information about .ps files see the manual
 */
class PGLIB_API pgParticleSystem : public pgD3DObject
{
protected:
	class Particle
	{
	public:
		Particle() : active(false), time(0.0f), breakTime(0.0f)  {}

		bool	active;
		pgVec3	pos, dir;
		pgVec3	emitPos;
		float	time;					// the time since the particle was emitted
		int		phase;
		float	breakTime;
		float	phi;
		float	factor;
		float	radius;
	};

public:
	enum  {  MAX_PARTICLES = 65536  };


	/// Predefined effect type
	enum EFFECT  {
					EFFECT_NORMAL,						/**<  Particles emit at the emitter and work only with gravity : DEFAULT  */
					EFFECT_SPHERE_NORMAL,				/**<  Particles emit on a sphere and work only with gravity  */
					EFFECT_SPHERE_FLEE,					/**<  Particles emit on a sphere and move away from the center  */
					EFFECT_CYLINDER_NORMAL,				/**<  Particles emit on a cylinder and work only with gravity  */
					EFFECT_CYLINDER_FLEE,				/**<  Particles emit on a cylinder and move away from the center axis  */
					EFFECT_CYLINDER_ROTATE };			/**<  Particles emit on a cylinder and rotate around the center axis  */


	/// Type of calculation how stage sources are combined.
	enum BLEND  {
					BLEND_UNKNOWN			= 0,							/**<  Undefined  */
					BLEND_ZERO				= D3DBLEND_ZERO,				/**<  Value is replaced (multiplied) by zero  */
					BLEND_ONE				= D3DBLEND_ONE,					/**<  Value is multiplied by one (unchanged)  */
					BLEND_SRCCOLOR          = D3DBLEND_SRCCOLOR,			/**<  Value is multiplied by source color  */
					BLEND_INVSRCCOLOR       = D3DBLEND_INVSRCCOLOR,			/**<  Value is multiplied by one minus source color  */
					BLEND_SRCALPHA          = D3DBLEND_SRCALPHA,			/**<  Value is multiplied by source alpha  */
					BLEND_INVSRCALPHA       = D3DBLEND_INVSRCALPHA,			/**<  Value is multiplied by one minus source alpha  */
					BLEND_DESTALPHA         = D3DBLEND_DESTALPHA,			/**<  Value is multiplied by destination alpha  */
					BLEND_INVDESTALPHA      = D3DBLEND_INVDESTALPHA,		/**<  Value is multiplied by one minus destination alpha  */
					BLEND_DESTCOLOR         = D3DBLEND_DESTCOLOR,			/**<  Value is multiplied by destination color  */
					BLEND_INVDESTCOLOR      = D3DBLEND_INVDESTCOLOR,		/**<  Value is multiplied by one minus destination color  */
					BLEND_SRCALPHASAT       = D3DBLEND_SRCALPHASAT,
					BLEND_BOTHSRCALPHA      = D3DBLEND_BOTHSRCALPHA,
					BLEND_BOTHINVSRCALPHA   = D3DBLEND_BOTHINVSRCALPHA
	};


	class LifePhase
	{
	friend class pgParticleSystem;
	public:
		LifePhase() : texture(NULL), size(0.0f), duration(0.0f)  {}

		LifePhase(pgTexturePtr nTexture, float nSize, float nDuration)
			{  texture = nTexture;  size = nSize;  duration = nDuration;  }

	protected:
		pgTexturePtr	texture;
		float			size;
		float			duration;

		friend bool																	// operator== needed for pgList
			operator==(const LifePhase&, const LifePhase&)  { return false; }
	};


	pgParticleSystem();


	/// Loads settings from a .ps file
	/**
	 *  create() is called internally
	 */
	bool load(const char* nFile);


	/// This methods are only needed if the particle system is created manually.
	/**
	 *  See the technical manual for a description of these settings.
	 */
	void setEffect(EFFECT nEffect)  {  effect = nEffect;  }

	void addLifePhase(const LifePhase& nPhase);

	void setEmitterPosition(const pgVec3& nPos)  { emitterPos = nPos; }

	void setEmitterDirection(const pgVec3n& nDir)  { emitterDir = nDir; }

	void setEmitSpeed(float nSpeed)  {  emitSpeed = nSpeed;  }

	void setGravity(const pgVec3& nGravity)  {  gravity = nGravity;  }

	void setSlowDown(float nFactor)  {  slowDownFactor = nFactor;  }

	void setEmitPerSecond(float nNumToEmit)  {  emitPerSeconds = nNumToEmit;  if(emitPerSeconds==0.0f)  emitPerSeconds = 1.0f;  }

	void setRadius(float nRadius)  {  radius = nRadius;  }

	void setHeight(float nHeight)  {  height = nHeight;  }

	void setRadialSpeed(float nSpeed)  {  radialSpeed = nSpeed;  }

	void setParticleDelay(float nDuration)  {  particleDelay = nDuration;  }


	/// Creates the particle system after all properties have been set.
	/**
	 *  If the particle system was created manually (without loading from file)
	 *  the creation process has to be finished by calling create().
	 */
	bool create(int nMaxParticles);


	/// Frees all resource from the particle system.
	void cleanup();


	/// Tells the particle system to emit (additional) nNum new particles during the next update
	void emitParticles(int nNum)  {  numToEmit += nNum;  }


	/// Updates the particle system. All particles are moved and "grow older"
	void update();


	/// Renders the particle system
	void render();


	/// Tells the particle system to automatically emit as many particles per second as set with setEmitPerSecond()
	void startEmitting()  {  emitActive = true;  emitTime = 0.0f;  }


	/// Stops automatic particle emitting
	void stopEmitting()  {  emitActive = false;  }


	/// Returns true if the automatic particle emitting is currently active.
	bool isEmitting()  {  return emitActive;  }


	// implementation of pgRenderable interface
	//
	void setPosition(const pgVec3& nPos)  {  pos = nPos;  }
	void setRotation(const pgVec3& nRot)  {  rot = nRot;  }

	virtual void deleteDeviceObjects();
	virtual bool restoreDeviceObjects();

	pgString				reportStr;

protected:
	void deleteSorting();
	void allocateSorting();
	//bool checkDevice(const char* nString);
	void updateNormal();

	BLEND getBlendFromString(const pgString& nStr);

	pgVec3					pos, rot;

	EFFECT					effect;
	float					radius;
	float					height;
	float					radialSpeed;

	int						maxParticles;
	int						numToEmit;
	float					emitPerSeconds;
	float					emitTime;
	bool					emitActive;
	Particle*				particles;
	pgList<LifePhase>		lifePhases;
	pgList<float>			phaseLenSums;

	pgVec3					emitterPos;
	pgVec3n					emitterDir;
	float					emitSpeed;
	pgVec3					gravity;
	float					slowDownFactor;
	float					particleDelay;

	BLEND					srcBlend, dstBlend;

	int						numActiveParticles;			// used for performance optimization

	// rendering stuff
	//
	unsigned short**		sortParticles;
	unsigned short*			sortLengths;

	LPDIRECT3DVERTEXBUFFER8 pVB;
	//LPDIRECT3DDEVICE8		pd3dDevice;
};

typedef pgParticleSystem* pgParticleSystemPtr;

#endif //__PARTICLESYSTEM_HEADERFILE__

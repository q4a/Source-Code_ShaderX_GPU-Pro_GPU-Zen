                  /*--.          .-"-. 
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "../Math/Plane.h"
#include "../Math/Color.h"
#include "../Util/Set.h"

enum COLOR_SCHEME {
	COLOR_SCHEME_FIRE,
	COLOR_SCHEME_ICE,
	COLOR_SCHEME_SMOKE,
	COLOR_SCHEME_RAINBOW
};


struct Particle {
	Vertex pos;
	Vertex dir;

	float size;
	float life;
	float initialLife;
	float depth;
};

struct PointForce {
	Vertex pos;
	float strength;
	float linearAttenuation;
	float quadraticAttenuation;
};


class ParticleSystem {
protected:
	Set <Particle> particles;
	Set <Plane> bouncePlanes;
	Set <Plane> killPlanes;
	Set <PointForce> pointForces;
	Vertex directionalForce;
	
	float lastTime, particleCredit;

	Color colors[12];

	Vertex pos;
	Vertex forward, up, right;

	float spawnRate;
	float rightSpread, upSpread;
	float speed, speedSpread;
	float size, sizeSpread;
	float life, lifeSpread;
	float frictionFactor;

	bool coloringEnabled;

public:
	ParticleSystem(const int nPointForces = 0, const int nKillPlanes = 0, const int nBouncePlanes = 0);

	const Vertex &getPosition() const { return pos; }
	void setPosition(const Vertex &position){ pos = position; }
	void setSpawnRate(const float spawnrate){ spawnRate = spawnrate; }

	void setSpeed(const float meanSpeed, const float spread, const float rightspread, const float upspread){
		speed = meanSpeed;
		speedSpread = spread;
		rightSpread = rightspread;
		upSpread = upspread;
	}

	void setLife(const float meanLife, const float spread){
		life = meanLife;
		lifeSpread = spread;
	}

	void setSize(const float meanSize, const float spread){
		size = meanSize;
		sizeSpread = spread;
	}

	void setDirectionalForce(const Vertex &df){ directionalForce = df; }
	void addPointForce(const PointForce &pf){ pointForces.add(pf); }
	void setPointForce(const int force, const PointForce &pf){ pointForces[force] = pf;	}
	void setFrictionFactor(const float friction){ frictionFactor = friction; }

	void addBouncePlane(const Plane &bp){ bouncePlanes.add(bp); }
	void setBouncePlane(const int plane, const Plane &bp){ bouncePlanes[plane] = bp; }

	void addKillPlane(const Plane &bp){ killPlanes.add(bp); }
	void setKillPlane(const int plane, const Plane &bp){ killPlanes[plane] = bp; }

	void setColoringEnabled(const bool enabled){ coloringEnabled = enabled; }
	void setColor(const int color, const Color &col){ colors[color] = col; }

	void setColorScheme(const COLOR_SCHEME colorScheme);

	void update(const float timeStamp);


	void depthSort(const Vertex &pos, const Vertex &depthAxis);

	void draw(const Vertex &dx, const Vertex &dy, int textureUnit = 0);
};


#endif // _PARTICLES_H_

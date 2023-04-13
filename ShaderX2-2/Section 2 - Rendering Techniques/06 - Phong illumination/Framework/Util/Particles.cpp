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

#include "Particles.h"
#include <stdlib.h>
#include <math.h>
#include "../OpenGL/glExtensions.h"

float random(float mean, float diff){
	float r = 2 * (rand() / float(RAND_MAX)) - 1.0f;
	
	return mean + r * fabsf(r) * diff;
}


ParticleSystem::ParticleSystem(const int nPointForces, const int nKillPlanes, const int nBouncePlanes){
	pos = Vertex(0,0,0);
	forward = Vertex(0,1,0);
	up      = Vertex(0,0,1);
	right   = Vertex(1,0,0);
	
	directionalForce = Vertex(0,0,0);
	
	pointForces.setSize(nPointForces);
	killPlanes.setSize(nKillPlanes);
	bouncePlanes.setSize(nBouncePlanes);
	
	spawnRate = 10;
	speed = 100;
	speedSpread = 25;
	rightSpread = 0.3f;
	upSpread    = 0.3f;
	
	size = 100;
	sizeSpread = 10;
	
	life = 2.5f;
	lifeSpread = 0.5f;
	
	frictionFactor = 0.7f;
	
	coloringEnabled = true;
	setColorScheme(COLOR_SCHEME_FIRE);

	lastTime = 0;
	particleCredit = 0;
}


void ParticleSystem::setColorScheme(const COLOR_SCHEME colorScheme){
	float f;
	int i;
	
	switch(colorScheme){
	case COLOR_SCHEME_FIRE:
		for (i = 0; i < 4; i++){
			colors[i    ] = Color(i / 4.0f, 0,0);
			colors[i + 4] = Color(1, i / 4.0f, 0);
			colors[i + 8] = Color((3 - i) / 3.0f,(3 - i) / 3.0f, 1);
		}
		break;
	case COLOR_SCHEME_ICE:
		for (i = 0; i < 6; i++){
			colors[i    ] = Color(0,0,i / 6.0f);
			colors[i + 6] = Color(i / 5.0f,1,1);
		}
		break;
	case COLOR_SCHEME_SMOKE:
		for (i = 0; i < 12; i++){
			f = i / 44.0f;
			colors[i] = Color(f,f,f);
		}
		break;
	case COLOR_SCHEME_RAINBOW:
		colors[0]  = Color(0,   0,   0);
		colors[1]  = Color(0,   0,   0.25f);
		colors[2]  = Color(0,   0,   0.5f);
		colors[3]  = Color(0,   0,   1);
		colors[4]  = Color(0,   0.5f,1);
		colors[5]  = Color(0,   1,   1);
		colors[6]  = Color(0,   1,   0.5f);
		colors[7]  = Color(0,   1,   0);
		colors[8]  = Color(0.5f,1,   0);
		colors[9]  = Color(1,   1,   0);
		colors[10] = Color(1,   0.5f,0);
		colors[11] = Color(1,   0,   0);
		break;
	}
}

void ParticleSystem::update(const float timeStamp){
	Particle p;
	float time, dist, friction;
	unsigned int i,j, len;
	Vertex v, dir;

	time = timeStamp - lastTime;
	lastTime = timeStamp;

	particleCredit += time * spawnRate;
	len = (int) particleCredit;
	particleCredit -= len;

	for (i = 0; i < len; i++){
		p.pos = pos;
		p.dir = (forward + random(0, upSpread) * up + random(0, rightSpread) * right);
		p.dir.fastNormalize();
		p.dir *= random(speed, speedSpread);
		p.size = random(size, sizeSpread);
		p.initialLife =	p.life = random(life, lifeSpread);
		particles.add(p);
	}

	friction = powf(frictionFactor, time);

	i = 0;
	while (i < particles.getSize()){
		if ((particles[i].life -= time) < 0){
			particles.remove(i);
			continue;
		}

		v = Vertex(0,0,0);
		len = pointForces.getSize();
		for (j = 0; j < len; j++){
			dir = pointForces[j].pos - particles[i].pos;
			dist = lengthSqr(dir);
			v += dir * (pointForces[j].strength / (1.0f + sqrtf(dist) * pointForces[j].linearAttenuation + dist * pointForces[j].quadraticAttenuation));
		}

		particles[i].dir += (directionalForce + v) * time;
		particles[i].dir *= friction;
		particles[i].pos += particles[i].dir * time;

		len = bouncePlanes.getSize();
		for (j = 0; j < len; j++){
			if ((dist = bouncePlanes[j].distance(particles[i].pos)) < particles[i].size){
				particles[i].dir = particles[i].dir - 2 * (particles[i].dir * bouncePlanes[j].normal) * bouncePlanes[j].normal;
				particles[i].pos = particles[i].pos + (particles[i].size - dist) * bouncePlanes[j].normal;
			}
		}

		len = killPlanes.getSize();
		for (j = 0; j < len; j++){
			if (killPlanes[j].distance(particles[i].pos) < particles[i].size){
				particles.remove(i);
				i--;
				break;
			}
		}

		i++;
	}
}

int depthComp(const Particle &elem0, const Particle &elem1){
	return (elem0.depth < elem1.depth)? 1 : -1;
}

void ParticleSystem::depthSort(const Vertex &pos, const Vertex &depthAxis){
	int i, len = particles.getSize();

	for (i = 0; i < len; i++){
		particles[i].depth = fabsf((particles[i].pos - pos) * depthAxis);
	}
	
	particles.sort(depthComp);
}


void ParticleSystem::draw(const Vertex &dx, const Vertex &dy, int textureUnit){
	int i, len, colInt;
	float colFrac;
	Color col;
	Vertex v;
	
	textureUnit += GL_TEXTURE0_ARB;
	
	
	glBegin(GL_QUADS);
	len = particles.getSize();
	for (i = 0; i < len; i++){
		if (coloringEnabled){
			colFrac = (11 * particles[i].life) / particles[i].initialLife;
			colInt  = (int) colFrac;
			colFrac -= colInt;
			
			col = colors[colInt] * (1 - colFrac) + colors[colInt + 1] * colFrac;
			
			glColor4fv((GLfloat *) &col);
		}
		
		glMultiTexCoord2f(textureUnit, 0,0);
		v = particles[i].pos + particles[i].size * (-dx + dy);
		glVertex3fv((GLfloat *) &v);
		
		glMultiTexCoord2f(textureUnit, 1,0);
		v = particles[i].pos + particles[i].size * ( dx + dy);
		glVertex3fv((GLfloat *) &v);
		
		glMultiTexCoord2f(textureUnit, 1,1);
		v = particles[i].pos + particles[i].size * ( dx - dy);
		glVertex3fv((GLfloat *) &v);
		
		glMultiTexCoord2f(textureUnit, 0,1);
		v = particles[i].pos + particles[i].size * (-dx - dy);
		glVertex3fv((GLfloat *) &v);
	}
	glEnd();
}


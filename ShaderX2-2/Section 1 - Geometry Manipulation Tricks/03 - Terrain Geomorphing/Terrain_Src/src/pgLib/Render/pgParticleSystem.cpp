///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgParticleSystem
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgParticleSystem.cpp,v 1.2 2002/12/18 22:00:37 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgParticleSystem.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgInTextFile.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Base/pgITime.h>
#include <pgLib/FrameWork/Dxutil.h>
#include <pgLib/Math/pgIMathTool.h>

#include <assert.h>
#include <stdio.h>

typedef unsigned short* ShortPtr;

struct POINTVERTEX
{
    D3DXVECTOR3 v;
};

#define D3DFVF_POINTVERTEX (D3DFVF_XYZ)


pgParticleSystem::pgParticleSystem() : gravity(0.0f, -100.0f, 0.0f), sortParticles(NULL), sortLengths(NULL),
									   maxParticles(0), particles(NULL), pVB(NULL), emitterDir(1.0f, 1.0f, 0.0f)
{
	numToEmit = 0;
	emitSpeed = 1.0f;
	emitPerSeconds = 1.0f;
	emitTime = 0.0f;
	emitActive = false;
	slowDownFactor = 0.95f;
	particleDelay = 0.0f;

	effect = EFFECT_NORMAL;
	radius = 50.0f;
	height = 100.0f;
	radialSpeed = 100.0f;

	numActiveParticles = 0;

	srcBlend = BLEND_SRCALPHA;
	dstBlend = BLEND_INVSRCALPHA;

	pgIMathTool::initSinCos();
}


void
pgParticleSystem::addLifePhase(const LifePhase& nPhase)
{
	//deleteSorting();

	if(lifePhases.getSize()==0)
		phaseLenSums.addTail(nPhase.duration);
	else
		phaseLenSums.addTail(phaseLenSums[phaseLenSums.getSize()-1]+nPhase.duration);

	lifePhases.addTail(nPhase);
	//allocateSorting();
}


void
pgParticleSystem::deleteSorting()
{
	if(!sortParticles)
		return;

	for(int i=0; i<lifePhases.getSize(); i++)
		delete sortParticles[i];
	delete sortParticles;
	sortParticles = NULL;
	delete sortLengths;
	sortLengths = NULL;
}


void
pgParticleSystem::allocateSorting()
{
	if(sortParticles)
		return;

	sortParticles = new ShortPtr[lifePhases.getSize()];
	sortLengths = new unsigned short[lifePhases.getSize()];

	for(int i=0; i<lifePhases.getSize(); i++)
	{
		sortParticles[i] = new unsigned short[maxParticles];
		sortLengths[i] = 0;
	}
}


bool
pgParticleSystem::create(int nMaxParticles)
{
	HRESULT hr;

	if(!checkDevice("cannot create particle system: render device not set"))
		return false;

	if(particles)
	{
		pgLog::error("particle system already created");
		return false;
	}

	maxParticles = nMaxParticles;
	particles = new Particle[maxParticles];

	assert(maxParticles<MAX_PARTICLES);

	allocateSorting();

    if(FAILED(hr = pd3dDevice->CreateVertexBuffer(maxParticles * sizeof(POINTVERTEX), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS, 
												  D3DFVF_POINTVERTEX, pgIDirectX::getPoolType(), &pVB)))
		return false;
	return true;
}


void
pgParticleSystem::deleteDeviceObjects()
{
	SAFE_RELEASE(pVB);
}


bool
pgParticleSystem::restoreDeviceObjects()
{
	deleteDeviceObjects();

    if(FAILED(pd3dDevice->CreateVertexBuffer(maxParticles * sizeof(POINTVERTEX), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS, 
											 D3DFVF_POINTVERTEX, pgIDirectX::getPoolType(), &pVB)))
	{
		pgLog::error("reallocation of buffers for particle system failed");
		return false;
	}

	return true;
}


void
pgParticleSystem::cleanup()
{
	SAFE_RELEASE(pVB);
}


void
pgParticleSystem::update()
{
	float	lastFrameTime = pgITime::getLastFrameTime(),
			timeBetweenEmit = 1.0f / emitPerSeconds;
	int		i;

	if(!particles)
	{
		pgLog::error("particle system cannot be updated: create() not called yet");
		return;
	}

	for(i=0; i<lifePhases.getSize(); i++)
		sortLengths[i] = 0;

	emitTime += lastFrameTime;
	if(emitActive)
		while(emitTime>timeBetweenEmit)
		{
			numToEmit++;
			emitTime -= timeBetweenEmit;
		}

	switch(effect)
	{
	case EFFECT_NORMAL:
	case EFFECT_SPHERE_NORMAL:
	case EFFECT_SPHERE_FLEE:
	case EFFECT_CYLINDER_NORMAL:
	case EFFECT_CYLINDER_FLEE:
	case EFFECT_CYLINDER_ROTATE:
		updateNormal();
		break;

	}

}


void
pgParticleSystem::updateNormal()
{
	int		i;
	float	lastFrameTime = pgITime::getLastFrameTime();
	pgVec3	grav = gravity * lastFrameTime;
	float	rPhi0,rPhi1,rPos, c0,s0,c1,s1;

	if(numActiveParticles==0 && numToEmit==0)				// nothing to do...
		return;

	numActiveParticles = 0;									// start new count...

	for(i=0; i<maxParticles; i++)
	{
		Particle& p = particles[i];
		if(p.active)
		{
			p.breakTime += lastFrameTime;
			numActiveParticles++;

			switch(effect)
			{
			default:
				while(p.breakTime>0.0f)
				{
					p.dir *= slowDownFactor;
					p.breakTime -= 0.1f;
				}

				if(p.time>=particleDelay)
				{
					p.dir += grav;
					p.pos += p.dir * lastFrameTime;
				}
				break;

			case EFFECT_CYLINDER_ROTATE:
				if(p.time>=particleDelay)
				{
					p.phi += radialSpeed * lastFrameTime * p.factor;
					p.pos.x = p.emitPos.x + p.radius * pgIMathTool::cos(p.phi);
					p.pos.z = p.emitPos.z + p.radius * pgIMathTool::sin(p.phi);
					p.dir += grav * p.factor;
					p.pos += p.dir * lastFrameTime;
					p.radius += emitSpeed * lastFrameTime;
				}
				break;
			}

			p.time += lastFrameTime;
			if((p.time-particleDelay)>phaseLenSums[p.phase])
			{
				p.phase++;
				if(p.phase>=lifePhases.getSize())
					p.active = false;
			}

			if(p.active)
			{
				sortParticles[p.phase][sortLengths[p.phase]] = (unsigned short)i;
				sortLengths[p.phase]++;
			}
		}
		else
		if(numToEmit>0)			// need to emit more particles ?
		{
			if(numActiveParticles>=maxParticles)
				continue;

			// calculate the initial position
			//
			switch(effect)
			{
			case EFFECT_NORMAL:
				p.pos = emitterPos;
				break;

			case EFFECT_SPHERE_NORMAL:
			case EFFECT_SPHERE_FLEE:
				rPhi0 = pg360RAND * rand();
				rPhi1 = pg360RAND * rand();
				c0 = pgIMathTool::cos(rPhi0);
				s0 = pgIMathTool::sin(rPhi0);
				c1 = pgIMathTool::cos(rPhi1);
				s1 = pgIMathTool::sin(rPhi1);

				p.pos.x = emitterPos.x + radius * s1 * c0;
				p.pos.y = emitterPos.y + radius * s1 * s0;
				p.pos.z = emitterPos.z + radius * c1;
				break;

			case EFFECT_CYLINDER_NORMAL:
			case EFFECT_CYLINDER_FLEE:
			case EFFECT_CYLINDER_ROTATE:
				rPhi0 = pg360RAND * rand();
				rPos = (height*rand())/RAND_MAX;
				c0 = pgIMathTool::cos(rPhi0);
				s0 = pgIMathTool::sin(rPhi0);
				p.pos.x = 0.0f; //radius * c0;
				p.pos.y = emitterPos.y + rPos;
				p.pos.z = 0.0f; //radius * s0;
				p.phi = rPhi0;
				p.factor = 2.0f * (0.1f + 0.9f * rPos / height);
				break;
			}

			// calculate the initial speed
			//
			switch(effect)
			{
			case EFFECT_NORMAL:
			case EFFECT_SPHERE_NORMAL:
			case EFFECT_CYLINDER_NORMAL:
				p.dir = emitterDir * emitSpeed;
				break;

			case EFFECT_SPHERE_FLEE:
				p.dir = (p.pos-emitterPos) * (emitSpeed/radius);
				break;

			case EFFECT_CYLINDER_FLEE:
			case EFFECT_CYLINDER_ROTATE:
				p.dir = p.pos * (emitSpeed/radius);
				p.dir.y = 0.0f;
				p.radius = radius;
				break;
			}

			p.active = true;
			p.breakTime = 0.0f;
			p.emitPos = emitterPos;					// save the emitter position (needed for later reference)
			p.time = 0.0f;
			p.phase = 0;
			numToEmit--;
			sortParticles[p.phase][sortLengths[p.phase]] = (unsigned short)i;
			sortLengths[p.phase]++;
			numActiveParticles++;
			i--;									// force this particle to be calculated again (and updated this time)
		}
	}
}


inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

void
pgParticleSystem::render()
{
	if(!sortParticles || !pVB)
		return;
	if(!checkDevice("cannot render particle system: render device not set"))
		return;
	if(numActiveParticles==0)
		return;

	HRESULT		hr;
	int			phase, i, v;
	POINTVERTEX	*pVertices;

	// set the device state to render point sprites with one texture layer
	// using modulating
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    //pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(10.0f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(1.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

    pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

    pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	//pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, srcBlend);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, dstBlend);


    pd3dDevice->SetRenderState(D3DRS_LIGHTING,  FALSE );
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    pd3dDevice->SetRenderState(D3DRS_CULLMODE,  D3DCULL_CCW );
    pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT );
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);


	D3DXMATRIX tMat, tMat2, mat;

	D3DXMatrixTranslation(&tMat, pos.x,pos.y,pos.z);
	D3DXMatrixRotationYawPitchRoll(&tMat2, rot.x,rot.y,rot.z);
	D3DXMatrixMultiply(&mat, &tMat2, &tMat);

	pd3dDevice->SetTransform(D3DTS_WORLD, &mat);


	if(FAILED(hr = pVB->Lock(0, maxParticles*sizeof(POINTVERTEX), (BYTE**)&pVertices, D3DLOCK_DISCARD)))
	{
		pgLog::error("cannot render particle system: pVB->Lock(0, %i, %i, D3DLOCK_DISCARD) failed", maxParticles*sizeof(POINTVERTEX), &pVertices);
		return;
	}


	// first fill the one and only vertex buffer completelly
	//
	v = 0;
	for(phase=0; phase<lifePhases.getSize(); phase++)
	{
		int numParticles = sortLengths[phase];

		for(i=0; i<numParticles; i++)
		{
			Particle& p = particles[sortParticles[phase][i]];
			assert(p.active);

			pVertices[v].v.x = p.pos.x;
			pVertices[v].v.y = p.pos.y;
			pVertices[v].v.z = p.pos.z;
			v++;
		}
	}


	pVB->Unlock();
    pd3dDevice->SetStreamSource( 0, pVB, sizeof(POINTVERTEX) );
    pd3dDevice->SetVertexShader( D3DFVF_POINTVERTEX );


	// now render each part of the vertex buffer (phase)
	//
	v = 0;
	for(phase=0; phase<lifePhases.getSize(); phase++)
	{
		pd3dDevice->SetTexture(0, lifePhases[phase].texture->getD3DTexture());

		pd3dDevice->SetRenderState( D3DRS_POINTSIZE, FtoDW(lifePhases[phase].size) );

		if(sortLengths[phase]>0)
		{
			if(FAILED(hr = pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, v, sortLengths[phase])))
			{
				pgLog::error("cannot render particle system: pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, %i, %i) failed", v, sortLengths[phase]);
				return;
			}
			v += sortLengths[phase];
			pgIDirectX::addParticles(sortLengths[phase]);
		}
	}

	reportStr.set("active particles: %d", v);

}



/****************************************************************************************\
*                                                                                        *
*                                     FILE LOADING                                       *
*                                                                                        *
\****************************************************************************************/


pgParticleSystem::BLEND
pgParticleSystem::getBlendFromString(const pgString& nStr)
{
	if(nStr=="ZERO")					return BLEND_ZERO;
	else if(nStr=="ONE")				return BLEND_ONE;
	else if(nStr=="SRCCOLOR")			return BLEND_SRCCOLOR;
	else if(nStr=="INVSRCCOLOR")		return BLEND_INVSRCCOLOR;
	else if(nStr=="SRCALPHA")			return BLEND_SRCALPHA;
	else if(nStr=="INVSRCALPHA")		return BLEND_INVSRCALPHA;
	else if(nStr=="DESTALPHA")			return BLEND_DESTALPHA;
	else if(nStr=="INVDESTALPHA")		return BLEND_INVDESTALPHA;
	else if(nStr=="DESTCOLOR")			return BLEND_DESTCOLOR;
	else if(nStr=="INVDESTCOLOR")		return BLEND_INVDESTCOLOR;
	else if(nStr=="SRCALPHASAT")		return BLEND_SRCALPHASAT;
	else if(nStr=="BOTHSRCALPHA")		return BLEND_BOTHSRCALPHA;
	else if(nStr=="BOTHINVSRCALPHA")	return BLEND_BOTHINVSRCALPHA;
	else return BLEND_UNKNOWN;
}


bool
pgParticleSystem::load(const char* nFile)
{
	int				max=500;
	char			str[256];
	float			size, dur;
	const char*		fName = pgISettings::tmpFullPsPath(nFile, ".ps");
	pgInTextFile	inFile;

	if(!inFile.open(fName))
	{
		pgLog::error("opening particle system file '%s' failed", fName);
		return false;
	}


	while(!inFile.eof())
	{
		const char* line = inFile.readLine();

		if(pgIStringTool::startsWith(line, "#") || pgIStringTool::isEmpty(line))
			continue;																	// ignore this line

		if(pgIStringTool::startsWith(line, "EFFECT"))
		{
			pgString type = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			type.toUpper();
			if(type=="NORMAL")
				effect = EFFECT_NORMAL;
			else if(type=="SPHERE_NORMAL")
				effect = EFFECT_SPHERE_NORMAL;
			else if(type=="SPHERE_FLEE")
				effect = EFFECT_SPHERE_FLEE;
			else if(type=="CYLINDER_NORMAL")
				effect = EFFECT_CYLINDER_NORMAL;
			else if(type=="CYLINDER_FLEE")
				effect = EFFECT_CYLINDER_FLEE;
			else if(type=="CYLINDER_ROTATE")
				effect = EFFECT_CYLINDER_ROTATE;
			else
			{
				pgLog::error("error in particle system file '%s': unknown effect: '%s'", nFile, type.get());
				return false;
			}
		}
		else
		if(pgIStringTool::startsWith(line, "MAX"))
			sscanf(line, "MAX %d", &max);
		else
		if(pgIStringTool::startsWith(line, "GRAVITY"))
			sscanf(line, "GRAVITY %f %f %f", &(gravity.x), &(gravity.y), &(gravity.z));
		else
		if(pgIStringTool::startsWith(line, "EMITSPEED"))
			sscanf(line, "EMITSPEED %f", &emitSpeed);
		else
		if(pgIStringTool::startsWith(line, "SLOWDOWN"))
			sscanf(line, "SLOWDOWN %f", &slowDownFactor);
		else
		if(pgIStringTool::startsWith(line, "EMITSPERSECOND"))
			sscanf(line, "EMITSPERSECOND %f", &emitPerSeconds);
		else
		if(pgIStringTool::startsWith(line, "RADIUS"))
			sscanf(line, "RADIUS %f", &radius);
		else
		if(pgIStringTool::startsWith(line, "HEIGHT"))
			sscanf(line, "HEIGHT %f", &height);
		else
		if(pgIStringTool::startsWith(line, "RADIALSPEED"))
			sscanf(line, "RADIALSPEED %f", &radialSpeed);
		else
		if(pgIStringTool::startsWith(line, "DELAY"))
			sscanf(line, "DELAY %f", &particleDelay);
		else
		if(pgIStringTool::startsWith(line, "BLEND"))
		{
			pgList<pgString> strList;

			pgIStringTool::fillTokenList(line, ' ', strList);

			if(strList.getSize()!=3)
			{
				pgLog::error("error in particle system file '%s': error on blend settings: '%s'", nFile, line);
				continue;
			}

			srcBlend = getBlendFromString(strList[1]);
			if(srcBlend==BLEND_UNKNOWN)
			{
				pgLog::error("error in particle system file '%s': unknown blend setting: '%s'", nFile, strList[1]);
				srcBlend = BLEND_SRCALPHA;
				continue;
			}

			dstBlend = getBlendFromString(strList[2]);
			if(dstBlend==BLEND_UNKNOWN)
			{
				pgLog::error("error in particle system file '%s': unknown blend setting: '%s'", nFile, strList[2]);
				dstBlend = BLEND_INVSRCALPHA;
				continue;
			}

		}
		else
		if(pgIStringTool::startsWith(line, "PHASE"))
		{
			sscanf(line, "PHASE %s %f %f", str, &size, &dur);
			addLifePhase(LifePhase(pgIResourceManager::getTexture(str, pgIResourceManager::SOURCE_STD), size, dur));
		}
		else
			pgLog::error("unknown option in particle file '%s': %s", nFile, str);
	}

	pgLog::info("particle system '%s' loaded", nFile);
	return create(max);
}


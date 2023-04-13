///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgCharacter
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgCharacter.cpp,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgCharacter.h"

#include <pgLib/Audio/pgIAudio.h>

#include <pgLib/Base/pgInTextFile.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Base/pgITime.h>

#include <pgLib/Math/pgPathLinear.h>

#include <pgLib/Render/pgAnimated.h>
#include <pgLib/Render/pgBaseMesh.h>
#include <pgLib/Render/pgMaterial.h>
#include <pgLib/Render/pgMeshUtil.h>
#include <pgLib/Render/pgParticleSystem.h>

#include <math.h>
#include <stdio.h>


pgCharacter::pgCharacter() : anim(NULL), path(NULL)
{
	curAction = NULL;
	curSubAction = 0;
	info.hitpoints = 10;
	info.collisionOn = false;
	scalarPos = 0.0f;
	info.drawBoxes = false;
	borderYellow = borderGreen = NULL;
	info.activeMin = 0.0f;
	info.activeMax = 1.0f;
	active = false;
}


void
pgCharacter::deleteDeviceObjects()
{
	if(anim)
		anim->deleteDeviceObjects();

	for(int i=0; i<particles.getSize(); i++)
		particles[i].psys->deleteDeviceObjects();
}


bool
pgCharacter::restoreDeviceObjects()
{
	bool ret = true;

	if(anim)
		anim->restoreDeviceObjects();

	for(int i=0; i<particles.getSize(); i++)
		if(!particles[i].psys->restoreDeviceObjects())
			ret = false;

	return ret;
}


bool pgCharacter::load(const char* nFileName, bool nFullPath)
{
	const char*		fName;
	pgInTextFile	inFile;
	int				i,j,k;

	if(nFullPath)
		fName = nFileName;
	else
		fName = pgISettings::tmpFullCharPath(nFileName, ".char");

	if(!inFile.open(fName))
	{
		pgLog::error("opening character file '%s' failed", fName);
		return false;
	}


	while(!inFile.eof())
	{
		const char* line = inFile.readLine();

		if(pgIStringTool::startsWith(line, "#") || pgIStringTool::isEmpty(line))
			continue;																	// ignore this line

		if(pgIStringTool::startsWith(line, "BASE"))
		{
			const char* name = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			if(!load(name))
			{
				pgLog::error("opening base file '%s' in character file '%s' failed", name, fName);
				return false;
			}
		}
		else
		if(pgIStringTool::startsWith(line, "ANIM"))
		{
			/*const char**/ info.animName = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			anim = pgIResourceManager::getAnimated(info.animName);
			if(!anim)
			{
				pgLog::error("loading animation '%s' for character '%s' failed", info.animName, nFileName);
				return false;
			}
		}
		else
		if(pgIStringTool::startsWith(line, "PATH"))
		{
			info.way = WAY_PATH;
			pgList<pgString> strList;
			pgIStringTool::fillTokenList(line, ' ', strList);
			if(strList.getSize()!=3)
			{
				pgLog::error("path definition for character '%s' invalid", nFileName);
				return false;
			}

			path = pgIResourceManager::getLinearPath(strList[1]);
			info.loopPath = (strList[2] == "loop");
		}
		else
		if(pgIStringTool::startsWith(line, "ACTIVERANGE"))
		{
			sscanf(line, "ACTIVERANGE %f %f", &info.activeMin, &info.activeMax);
		}
		else
		if(pgIStringTool::startsWith(line, "LINE"))
		{
			info.way = WAY_LINE;
			char strLoop[128];
			sscanf(line, "LINE (%f %f %f) (%f %f %f) %s", &info.pos0.x, &info.pos0.y, &info.pos0.z, &info.pos1.x, &info.pos1.y, &info.pos1.z, strLoop);
			info.loopPath = (pgString("loop") == strLoop);
		}
		else
		if(pgIStringTool::startsWith(line, "HITPOINTS"))
		{
			sscanf(line, "HITPOINTS %d", &info.hitpoints);
		}
		else
		if(pgIStringTool::startsWith(line, "COLLISION"))
		{
			const char	*op = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			info.collisionOn = pgIStringTool::startsWithIgnoreCase(op, "on");
		}
		else
		if(pgIStringTool::startsWith(line, "TRIGGERBOX"))
		{
			char name[128], rel[128], move[128];
			float x0,y0,z0, x1,y1,z1;
			sscanf(line, "TRIGGERBOX %s %s %s (%f %f %f) (%f %f %f)", name, rel, move, &x0,&y0,&z0, &x1,&y1,&z1);
			boxes.addTail(Box(name,
							  pgString("abs")==rel ? REL_ABS : REL_REL,
							  pgString("fix")==move ? MOV_FIX : MOV_FOLLOW,
							  pgAABBox(pgVec3(x0,y0,z0), pgVec3(x1,y1,z1))));
		}
		else
		if(pgIStringTool::startsWith(line, "PARTICLES"))
		{
			char name[128], psysname[128], rel[128];
			pgVec3 pos;
			int initialEmit=0;
			sscanf(line, "PARTICLES %s %s %s %d (%f %f %f)", name, psysname, rel, &initialEmit, &pos.x,&pos.y,&pos.z);
			particles.addTail(Particles(name, psysname, pgString("local")==rel ? REL_LOCAL : REL_GLOBAL, initialEmit, pos));
		}
		else
		if(pgIStringTool::startsWith(line, "DRAWBOXES"))
		{
			const char	*op = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
			info.drawBoxes = pgIStringTool::startsWithIgnoreCase(op, "on");
		}
		else
		if(pgIStringTool::startsWith(line, "ACTION"))
		{
			actions.addTail(Action());
			Action& action = actions.getTail();

			action.name = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));

			while(!inFile.eof())
			{
				const char* line = inFile.readLine();

				if(pgIStringTool::startsWith(line, "#"))
					continue;

				if(pgIStringTool::isEmpty(line))					// an empty line stops the action definition
					break;

				if(pgIStringTool::startsWith(line, "PRIORITY"))
					sscanf(line, "PRIORITY %d", &action.priority);
				else
				if(pgIStringTool::startsWith(line, "MOVESPEED"))
					sscanf(line, "MOVESPEED %f", &action.speed);
				else
				if(pgIStringTool::startsWith(line, "ANIM"))
				{
					pgList<pgString> strList;
					pgIStringTool::fillTokenList(pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line)), ' ', strList);

					if(strList.getSize()<2)			// at least the animation and sound must specified (also 'noanim' is possible)
					{
						pgLog::error("error in ACTION '%s' in file '%s': to few arguments", action.name, nFileName);
						return false;
					}

					action.subActions.addTail(SubAction(strList[0]));
					int i0 = 1;

					if(strList[0]=="noanim")
					{
						if(strList.getSize()<2)
						{
							pgLog::error("error in ACTION %s' in file '%s': meshanim of type noanim must be followed by a duration", action.name, nFileName);
							continue;
						}

						action.subActions.getTail().duration = (float)atof(strList[1]);
						i0++;
					}

					if(i0>=strList.getSize())
					{
						pgLog::error("error in ACTION %s' in file '%s': sound reference missing", action.name, nFileName);
						continue;
					}

					if(strList[i0]!="nosound")
						action.subActions.getTail().sample = pgIAudio::loadSample(strList[i0]);

					i0++;

					for(i=i0; i<strList.getSize(); i++)
						action.subActions.getTail().particlesNameList.addTail(strList[i]);
				}
				else
				if(pgIStringTool::startsWith(line, "POSTACTION"))
					action.postActionName = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line));
				else
					pgLog::error("unknown command in ACTION block %s: '%s'", action.name, line);
			}
		}
		else
		if(pgIStringTool::startsWith(line, "EVENT"))
		{
			pgList<pgString> strList;
			pgIStringTool::fillTokenList(line, ' ', strList);
			if(strList.getSize()!=4)
			{
				pgLog::error("error in EVENT definition in character file '%s': incorrect number of parameters", nFileName);
				continue;
			}

			EVENT evt = getEventTypeFromString(strList[1]);
			if(evt==EVENT_UNDEFINED)
				pgLog::error("undefined EVENT type '%s' in character file '%s'", strList[1], nFileName);
			else
				events.addTail(Event(evt, strList[2], strList[3]));
		}
		else
		{
			pgLog::error("unknown command in character definition file '%s': %s", nFileName, line);
			return false;
		}
	}


	// resolve event names
	//
	for(i=0; i<events.getSize(); i++)
	{
		if((events[i].actionIdx = getActionIndex(events[i].actionName))==-1)
			pgLog::error("error in character definition file '%s': unknown action: '%s'", nFileName, events[i].actionName);

		if(events[i].boxName=="nobox")
			events[i].boxIdx = -2;
		else
		if(events[i].boxName=="collisionbox")
			events[i].boxIdx = -1;
		else
			if((events[i].boxIdx = getBoxIndex(events[i].boxName))==-1)
				pgLog::error("error in character definition file '%s': unknown box: '%s'", nFileName, events[i].boxName);
	}


	// resolve particle system names
	//
	for(i=0; i<particles.getSize(); i++)
	{
		Particles &p = particles[i];

		p.psys = pgIResourceManager::getParticleSystem(p.psysName, true);
		if(p.psys==NULL)
		{
			pgLog::error("error in character file '%s': particle system '%s' not found", nFileName, p.psysName);
			return false;
		}
	}


	// resolve action and subaction names
	//
	for(i=0; i<actions.getSize(); i++)
	{
		Action& a = actions[i];
		if(a.subActions.getSize()==0)
		{
			actions.removeIndex(i);
			pgLog::error("error in character file '%s': action '%s' does not have an animation defined and is therefore removed", nFileName, a.name);
			continue;
		}

		if(a.postActionName=="none")
			a.postActionNr = -1;
		else
		if(a.postActionName=="hide")
			a.postActionNr = -2;
		else
		{
			a.postActionNr = getActionIndex(a.postActionName);
			if(a.postActionNr==-1)
			{
				pgLog::error("error in character file '%s': postaction '%s' is not defined", nFileName, a.postActionName);
			}
		}

		for(j=0; j<a.subActions.getSize(); j++)
		{
			pgList<pgString>& names = a.subActions[j].particlesNameList;
			for(k=0; k<names.getSize(); k++)
			{
				Particles* p = getParticles(names[k]);
				if(p==NULL)
					pgLog::error("error in character file '%s': particles '%s' not found", nFileName, names[k]);
				else
					a.subActions[j].particlesList.addTail(p);
			}
		}
	}


	// update relative boxes
	//
	updatePosition();
	for(i=0; i<boxes.getSize(); i++)
	{
		if(boxes[i].rel==REL_REL && boxes[i].mov==MOV_FIX)
		{
			boxes[i].box0.move(pos);
			boxes[i].box = boxes[i].box0;
		}
		else
		if(boxes[i].rel==REL_ABS && boxes[i].mov==MOV_FOLLOW)
			pgLog::error("triggerbox '%s' is set to absolute and follow, which is not supported", boxes[i].name);
	}


	// create textures and a box
	// for box drawing
	if(info.drawBoxes)
	{
		borderYellow = pgIResourceManager::getTexture("borderYellow", pgIResourceManager::SOURCE_STD);
		borderGreen = pgIResourceManager::getTexture("borderGreen", pgIResourceManager::SOURCE_STD);

		pgBaseMesh* bm = pgMeshUtil::createBox(pgAABBox(pgVec3(-0.5f,-0.5f,-0.5f), 0.5f,0.5f,0.5f), 0);
		pgMaterial* mat = new pgMaterial();
		mat->addStage(pgTextureStage(borderYellow));
		mat->setCulling(pgMaterial::CULL_CCW);
		pgSegment* seg = new pgSegment();
		seg->setBaseMesh(bm);
		seg->setMaterial(mat);
		seg->setRenderSettings(pgSegment::SET_ZTEST|pgSegment::SET_ZWRITE|pgSegment::SET_FILL|pgSegment::SET_TEXTURED);
		seg->updateRenderSettings();
		boxMesh.addSegment(seg);
	}

	return true;
}


void
pgCharacter::setEvent(EVENT nEvent, const pgAABBox* nBox)
{
	int i;

	// run through all events to see if the event type matches
	// and the box overlaps (if there is a box)
	for(i=0; i<events.getSize(); i++)
		if(events[i].type == nEvent)
			if(!nBox || nBox->doesOverlap(getEventBox(events[i])))
			{
				Action& a = actions[events[i].actionIdx];
				if(curAction==NULL || a.priority>curAction->priority)
				{
					curAction = &a;
					curSubAction = 0;
					if(active)
						applyCurrentAction();
				}
			}
}


bool pgCharacter::setGameProgress(float nVal)
{
	if(nVal>=info.activeMin && nVal<=info.activeMax)
	{
		if(!active)										// if we weren't active until now we apply the current action
			applyCurrentAction();
		active = true;
	}
	else
		active = false;

	return active;
}


void
pgCharacter::update()
{
	int i;

	if(!active)
		return;

	if(curAction)
	{
		// move on on the path
		//
		scalarPos += curAction->speed * pgITime::getLastFrameTime();
		if(scalarPos>1.0f)
		{
			if(info.loopPath)
				scalarPos -= 1.0f;
			else
				scalarPos = 1.0f;
		}


		// if the animation finished go on
		// to the next one...
		hideTime -= pgITime::getLastFrameTime();

		if(anim->getDidMeshLoop() || hideTime<=0.0f)				// did the current subaction (animation) play through ?
		{
			anim->resetDidMeshLoop();
			stopAllParticleSystems();

			curSubAction++;
			if(curSubAction>=curAction->subActions.getSize())		// have all subactions been played ?
			{
				curSubAction = 0;

				if(curAction->postActionNr<0)
				{
					if(curAction->postActionNr==-2)
						anim->setHidden(true);
					anim->setFreeze(true);
					curAction = NULL;
				}
				else
					curAction = &actions[curAction->postActionNr];
			}

			applyCurrentAction();
		}


		updatePosition();

		// update the collision box (include the extension due to
		// the rotation by simply including the rotated original box)
		collBox = anim->getCurrentBBox();
		collBox.exentedByRotationY(rot.x);
		collBox.move(pos);


		// update all boxes which
		// follow the character
		for(i=0; i<boxes.getSize(); i++)
			if(boxes[i].mov == MOV_FOLLOW)
			{
				boxes[i].box = boxes[i].box0;
				boxes[i].box.move(pos);
			}
	}


	// update all particle systems
	//
	for(i=0; i<particles.getSize(); i++)
	{
		pgParticleSystem* psys = particles[i].psys;
		if(particles[i].rel==REL_LOCAL)
			psys->setPosition(pos+particles[i].emitRelPos);
		else
		{
			psys->setPosition(pgVec3(0.0f,0.0f,0.0f));
			psys->setEmitterPosition(pos+particles[i].emitRelPos);
		}
		psys->update();
	}
}


void
pgCharacter::render()
{
	if(!active)
		return;

	if(!anim)
	{
		pgLog::error("cannot render character: animated instance missing");
		return;
	}


	// render the mesh itself
	//
	anim->setPosition(pos);
	anim->setRotation(rot);
	anim->render();


	// render all particle systems
	//
	for(int i=0; i<particles.getSize(); i++)
		particles[i].psys->render();


	// collision and triggerboxes can be drawn
	// if activated in the character file...
	if(info.drawBoxes)
	{
		// draw collision box
		//
		pgMeshUtil::updateBox(boxMesh.getSegment(0)->getBaseMesh(), collBox, 0x66ff0000);
		boxMesh.getSegment(0)->getMaterial()->getStage(0).setTexture(borderYellow);
		boxMesh.getSegment(0)->updateRenderSettings();
		boxMesh.render();

		// draw the triggerboxes
		//
		boxMesh.getSegment(0)->getMaterial()->getStage(0).setTexture(borderGreen);
		boxMesh.getSegment(0)->updateRenderSettings();
		for(int i=0; i<boxes.getSize(); i++)
		{
			pgAABBox& box = boxes[i].box;

			pgMeshUtil::updateBox(boxMesh.getSegment(0)->getBaseMesh(), box, 0x66ff0000);
			boxMesh.render();
		}
	}

}


void
pgCharacter::updatePosition()
{
	if(info.way==WAY_PATH)									// if the way type is path we can simply ask the
	{														// path instance for the 3d position of our scalar position
		if(path!=NULL)
		{
			pgVec3n dir;
			path->getPosDir(scalarPos, pos, dir);
			rot.x = (float)atan2(-dir.z, dir.x);
		}
		else
			pgLog::error("cannot set character position: path instance missing");
	}
	else													// if the way type is line we do an interpolation between
	{														// starting and end point of the line
		pos = info.pos0*(1.0f-scalarPos) + info.pos1*scalarPos;
		pgVec3n dir = info.pos1-info.pos0;
		rot.x = (float)atan2(-dir.z, dir.x);
	}
}


void
pgCharacter::applyCurrentAction()
{
	stopAllParticleSystems();

	if(!curAction)
		return;

	SubAction& sa = curAction->subActions[curSubAction];
	const pgString& name = sa.animName;

	if(anim)
	{
		if(name=="noanim")
		{
			anim->setHidden(true);
			anim->startAnimation(0);
			hideTime = sa.duration;
		}
		else
		{
			anim->setHidden(false);
			anim->startAnimation(name);
			hideTime = pgFLOAT_MAX;
		}
	}

	if(sa.sample)
		sa.sample->play();

	pgPtrList<Particles>& particlesList = sa.particlesList;
	for(int j=0; j<particlesList.getSize(); j++)
	{
		particlesList[j]->psys->emitParticles(particlesList[j]->initialEmit);
		particlesList[j]->psys->startEmitting();
	}
}


int
pgCharacter::getActionIndex(const pgString nName)
{
	for(int i=0; i<actions.getSize(); i++)
		if(actions[i].name == nName)
			return i;

	return -1;
}


int
pgCharacter::getBoxIndex(const pgString nName)
{
	for(int i=0; i<boxes.getSize(); i++)
		if(boxes[i].name == nName)
			return i;

	return -1;
}


pgCharacter::EVENT
pgCharacter::getEventTypeFromString(pgString nEventStr)
{
	nEventStr.toLower();

	if(nEventStr=="startup")
		return EVENT_STARTUP;
	else if(nEventStr=="guide")
		return EVENT_GUIDE;
	else if(nEventStr=="player")
		return EVENT_PLAYER;
	else
		return EVENT_UNDEFINED;
}


pgAABBox&
pgCharacter::getEventBox(const Event& event)
{
	if(event.boxIdx<0)
		return collBox;
	else
		return boxes[event.boxIdx].box;
}


void
pgCharacter::stopAllParticleSystems()
{
	for(int i=0; i<particles.getSize(); i++)
		particles[i].psys->stopEmitting();
}


pgCharacter::Particles*
pgCharacter::getParticles(const pgString& nName)
{
	for(int k=0; k<particles.getSize(); k++)
		if(particles[k].name == nName)
			return &particles[k];

	return NULL;
}

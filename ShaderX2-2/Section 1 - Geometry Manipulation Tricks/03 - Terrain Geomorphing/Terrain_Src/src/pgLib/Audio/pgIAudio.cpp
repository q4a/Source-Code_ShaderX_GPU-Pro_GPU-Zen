///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIAudio
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIAudio.cpp,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////


#pragma warning(disable: 4514)


#include "pgIAudio.h"

#include <pgLib/Audio/pgAudioFMOD.h>
#include <pgLib/Base/pgLog.h>


pgIAudio::TYPE	pgIAudio::type = pgIAudio::TYPE_NONE;
pgIAudioDevice*	pgIAudio::device = NULL;


void
pgIAudio::init(TYPE nType)
{
	switch(nType)
	{
	case TYPE_FMOD:
		device = new pgAudioFMOD();
		device->init();
		break;

	default:
		pgLog::error("cannot instanciate audio device: unknown device type");
		break;
	}
}


void
pgIAudio::cleanup()
{
	if(device)
		device->cleanup();
}


void
pgIAudio::update()
{
	if(device)
		device->update();
}


int
pgIAudio::getNumChannels()
{
	return device ? device->getNumChannels() : 0;
}


pgISample*
pgIAudio::loadSample(const char* nName)
{
	if(device)
		return device->loadSample(nName);

	pgLog::error("cannot load sound: audio engine not initialized yet");
	return NULL;
}


/*pgSong*
pgIAudio::loadSong(const char* nName)
{

}*/


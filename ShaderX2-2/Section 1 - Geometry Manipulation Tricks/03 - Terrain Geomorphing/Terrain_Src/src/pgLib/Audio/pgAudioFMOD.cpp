///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgAudioFMOD
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgAudioFMOD.cpp,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////

#pragma warning(disable: 4514)


#include "pgAudioFMOD.h"

#include <pgLib/Audio/pgSampleFMOD.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgLog.h>
#include <Libs/FMOD/Include/fmod.h>


pgAudioFMOD::pgAudioFMOD()
{
}


void
pgAudioFMOD::init()
{
	if(FSOUND_Init(44100, 32, 0)!=0)
		pgLog::info("FMOD audio engine initialized");
	else
		pgLog::info("FMOD audio engine initialization failed");
}


void
pgAudioFMOD::cleanup()
{
	FSOUND_Close();
}


void
pgAudioFMOD::update()
{
	int i;

	// either update or destroy the
	// samples
	for(i=0; i<samples.getSize(); i++)
	{
		if(samples[i]->wantDestroy)
		{
			samples[i]->stop();
			delete samples[i];
			samples.removeIndex(i);
			i--;
		}
		else
			samples[i]->update();
	}
}


int
pgAudioFMOD::getNumChannels()
{
	return FSOUND_GetMaxChannels();
}


pgISample*
pgAudioFMOD::loadSample(const char* nName)
{
	const char*		fullName = pgISettings::tmpFullAudioPath(nName, ".wav");
	FSOUND_SAMPLE*	sound = NULL;
	pgSampleFMOD*	sample = NULL;
	int				i;


	// check if this file is already in memory
	//
	for(i=0; i<sounds.getSize(); i++)
	{
		FSOUND_SAMPLE* sound = (FSOUND_SAMPLE*)sounds[i];

		if(strcmp(FSOUND_Sample_GetName(sound), nName)==0)
		{
			sample = new pgSampleFMOD(nName, sound);
			samples.addTail(sample);
			return sample;
		}
	}


	// not in memory yet? so we try to load it...
	//
	sound = FSOUND_Sample_Load(FSOUND_FREE, fullName, FSOUND_NORMAL, 0);
	if(sound==NULL)
	{
		pgLog::error("loading sound '%s' failed", nName);
		return NULL;
	}

	pgLog::info("sound '%s' loaded", nName);
	sounds.addTail(sound);
	sample = new pgSampleFMOD(nName, sound);
	samples.addTail(sample);
	return sample;
}


/*pgSong*
pgAudioFMOD::loadSong(const char* nName)
{

}*/

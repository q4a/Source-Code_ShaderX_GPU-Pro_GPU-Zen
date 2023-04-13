///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgSampleFMOD
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSampleFMOD.cpp,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////


#pragma warning(disable: 4514)


#include "pgSampleFMOD.h"

#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgITime.h>

#include <Libs/FMOD/Include/fmod.h>


pgSampleFMOD::pgSampleFMOD(const char* nName, FSOUND_SAMPLE* nSample) : sample(nSample)
{
	int freq, len;

	name = nName;
	wantDestroy = false;
	loop = false;
	playing = false;
	channel = -1;

	FSOUND_Sample_GetDefaults(sample, &freq, 0,0,0);
	len = FSOUND_Sample_GetLength(sample);

	length = (freq==0) ? 0.0f : (float)(len) / (float)(freq);
}


pgSampleFMOD::~pgSampleFMOD()
{
	sample = 0;
	channel = -1;
}


void
pgSampleFMOD::destroy()
{
	wantDestroy = true;
}


void
pgSampleFMOD::play(bool nLoop)
{
	channel = FSOUND_PlaySound(FSOUND_FREE, sample);

	if(channel==-1)
	{
		pgLog::error("playing sound sample '%s' failed", name);
		return;
	}

	playing = true;
	timeLeft = length;
	loop = nLoop;
}


void
pgSampleFMOD::stop()
{
	if(!playing)
		return;

	playing = loop = false;

	if(FSOUND_StopSound(channel)==0)
		pgLog::error("stopping sound sample '%s' failed", name);
}


void
pgSampleFMOD::stopLooping()
{
	loop = false;
}


bool
pgSampleFMOD::isPlaying()
{
	if(sample==0 || channel==-1)
		return false;
	
	return false;
}


void
pgSampleFMOD::update()
{
	if(!playing)
		return;

	timeLeft -= pgITime::getLastFrameTime();

	if(timeLeft<=0)
	{
		if(loop)
			play(loop);
		else
			playing = false;
	}
}


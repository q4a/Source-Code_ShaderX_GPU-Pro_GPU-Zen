///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgAudioFMOD
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgAudioFMOD.h,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __AUDIOFMOD_HEADERFILE__
#define __AUDIOFMOD_HEADERFILE__


#include <pgLib/pgLib.h>

#include <pgLib/Audio/pgIAudioDevice.h>
#include <pgLib/Base/pgPtrList.h>

class pgSampleFMOD;


/// internal class for audio usage. use pgAudio for basic audio functionality
class PGLIB_API pgAudioFMOD : public pgIAudioDevice
{
friend class pgIAudio;			// only pgAudio may create and use this class directly
protected:
	virtual void init();

	virtual void cleanup();

	virtual void update();

	virtual int getNumChannels();

	virtual pgISample* loadSample(const char* nName);

	//virtual pgSong* loadSong(const char* nName);

	pgAudioFMOD();

	pgPtrList<void>			sounds;
	pgPtrList<pgSampleFMOD>	samples;
};


#endif //__AUDIOFMOD_HEADERFILE__

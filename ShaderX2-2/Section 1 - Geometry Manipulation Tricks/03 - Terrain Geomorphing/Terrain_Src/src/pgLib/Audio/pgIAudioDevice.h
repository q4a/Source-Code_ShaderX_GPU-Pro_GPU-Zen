///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgAudioDevice
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIAudioDevice.h,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __AUDIODEVICE_HEADERFILE__
#define __AUDIODEVICE_HEADERFILE__


#include <pgLib/pgLib.h>

class pgISample;
//class pgSong;


/// This interface is used internally by pgIAudio
/**
 *  All audiodevices which should be created by pgIAudio
 *  have to implement this interface
 */ 
class PGLIB_API pgIAudioDevice
{
friend class pgIAudio;
protected:
	/// must be called before any sound can be loaded or played
	virtual void init() = 0;

	/// should be called before quiting the programm
	virtual void cleanup() = 0;

	/// must be called every frame to update the audio engine
	virtual void update() = 0;

	/// returns the total number of channels available for playing sounds
	virtual int getNumChannels() = 0;

	/// loads a sound sample (.wav, .mp3, etc...) 
	virtual pgISample* loadSample(const char* nName) = 0;

	// loads a song (.mid, .mod, etc...)
	//
	//virtual pgSong* loadSong(const char* nName) = 0;
};


#endif //__AUDIODEVICE_HEADERFILE__

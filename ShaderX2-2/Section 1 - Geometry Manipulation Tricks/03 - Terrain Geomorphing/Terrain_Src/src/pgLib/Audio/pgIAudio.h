///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIAudio
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIAudio.h,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __AUDIO_HEADERFILE__
#define __AUDIO_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Audio/pgISample.h>


class pgIAudioDevice;


/// Defines the basic audio interface.
/**
 *  This class is the entry point for audio support in pgLib,
 *  such as loading audio samples.
 */
class PGLIB_API pgIAudio
{
public:
	/// Type of audio devices to be used
	enum TYPE  {
                   TYPE_NONE,  /**< No Audio Device */
	               TYPE_FMOD   /**< FMOD Audio Engine */
	};

	/// Must be called before any sound can be loaded or played.
	/**
	 *  Currently only FMOD is supported as underlying audio engine.
	 */
	static void init(TYPE nType);

	/// Should be called before quiting the programm
	static void cleanup();

	/// Must be called every frame to update the audio engine
	static void update();

	/// Returns the total number of channels available for playing sounds
	static int getNumChannels();

	/// Loads a sound sample (.wav, .mp3, etc...)
	static pgISample* loadSample(const char* nName);

	// Loads a song (.mid, .mod, etc...)
	//
	//static pgSong* loadSong(const char* nName);

protected:
	pgIAudio() {}					// prevent this class from being instanciated

	static TYPE				type;
	static pgIAudioDevice*	device;
};

#endif //__AUDIO_HEADERFILE__

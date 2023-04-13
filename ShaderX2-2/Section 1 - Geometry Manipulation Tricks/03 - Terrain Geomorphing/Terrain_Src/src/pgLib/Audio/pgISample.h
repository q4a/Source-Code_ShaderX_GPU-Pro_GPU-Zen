///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgISample
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgISample.h,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __SAMPLE_HEADEEFILE__
#define __SAMPLE_HEADEEFILE__


#include <pgLib/pgLib.h>

/// Basic sound sample interface
/**
 *  Use this interface to play samples.
 *  Samples can be played once or looped.
 */
class PGLIB_API pgISample
{
public:
	/// returns the name of the sound sample
	virtual const char* getSampleName() const = 0;

	/// Sets the sample to be destroyed as soon as possible
	/**
	 *  It is not garantied when the sample will be destroyed.
	 *  The concrete time of destroying and freeing up 
	 *  of resources is up to the audio engine.
	 */
	virtual void destroy() = 0;

	/// Starts playing a sound
	/**
	 *  If nLoop is passed as true the sound will play looped
	 *  until stop() is called.
	 */
	virtual void play(bool nLoop=false) = 0;

	/// Stops playing the sound immediately
	/**
	 *  How "immediately" the sound stops playing is up to the
	 *  underlying audio engine. Durations around 50ms can be
	 *  seen as "very immediately"
	 */
	virtual void stop() = 0;

	/// Keeps on playing the sound is finished, but the sound will not loop anymore
	/**
	 *  After this method is called the sample keeps on playing until it is finished
	 *  but will not loop again.
	 */
	virtual void stopLooping() = 0;

	/// Returns true if the sound is currently playing.
	virtual bool isPlaying() = 0;

protected:
	virtual ~pgISample() {};
	virtual void update() = 0;
};

#endif //__SAMPLE_HEADEEFILE__

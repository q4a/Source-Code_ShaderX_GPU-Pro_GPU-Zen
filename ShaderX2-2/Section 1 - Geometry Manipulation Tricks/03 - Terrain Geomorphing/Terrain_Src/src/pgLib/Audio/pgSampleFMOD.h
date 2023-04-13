///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgSampleFMOD
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSampleFMOD.h,v 1.1.1.1 2002/10/20 09:33:00 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __SAMPLEFMOD_HEADEEFILE__
#define __SAMPLEFMOD_HEADEEFILE__

#include <pgLib/Base/pgPtrList.h>

#include <pgLib/pgLib.h>

struct FSOUND_SAMPLE;

#include <pgLib/Audio/pgISample.h>
#include <pgLib/Base/pgString.h>

// basic FMOD sample implementation
//
class PGLIB_API pgSampleFMOD : public pgISample
{
friend class pgAudioFMOD;

public:
	// implement the pgISample interface
	//
	virtual const char* getSampleName() const  {  return name;  }

	virtual void destroy();

	virtual void play(bool nLoop=false);

	virtual void stop();

	virtual void stopLooping();

	virtual bool isPlaying();

protected:
	pgSampleFMOD(const char* nName, FSOUND_SAMPLE* nSample);
	virtual ~pgSampleFMOD();
	virtual void update();

	FSOUND_SAMPLE*	sample;
	int				channel;
	bool			loop;
	bool			playing;
	float			length;
	bool			wantDestroy;
	float			timeLeft;
	pgString		name;
};

#endif //__SAMPLEFMOD_HEADEEFILE__

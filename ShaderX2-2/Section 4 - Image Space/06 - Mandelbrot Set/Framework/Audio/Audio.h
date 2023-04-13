                  /*--.          .-"-. 
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "../Util/Set.h"
#include "AudioStream.h"

#include <AL/al.h>
#include <AL/alc.h>



typedef int SoundID;

#define SOUND_NONE    (-1)
#define SOUND_UNDEFINED ((unsigned int) (-2))

// Sound flags
#define SND_LOOPING    0x1
#define SND_STREAM     0x2


struct Sound {
	AudioStream *audioStream;
	unsigned char *samples[2];
	
	ALenum format;
	ALuint source;
	ALuint buffer[2];

	unsigned int currStream;
	bool endOfStream;
	bool isStreaming;
	bool isLooping;
};

class Audio {
protected:
	ALCcontext *ctx;
	ALCdevice *dev;

	Set <Sound> sounds;

	SoundID insertSound(Sound &sound);
	void removeSound(Sound &sound);
	void deleteSound(const int index);
public:
	Audio();
	~Audio();

	void clear();

	SoundID addSound(const char *fileName, unsigned int flags = 0);
	bool playSound(const SoundID soundID);
	bool stopSound(const SoundID soundID);
	bool pauseSound(const SoundID soundID);

	bool updateSoundStream(const SoundID soundID);
};

#endif // _AUDIO_H_

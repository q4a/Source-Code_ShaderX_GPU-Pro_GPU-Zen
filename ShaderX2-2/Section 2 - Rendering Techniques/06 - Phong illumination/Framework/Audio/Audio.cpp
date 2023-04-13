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

#include "Audio.h"
#include <stdio.h>

#include "../Platform.h"

#ifdef WIN32

#pragma comment (lib, "OpenAL32.lib")

#endif

// Buffer 64k samples
#define BUF_STREAM_SIZE 16384


Audio::Audio(){
	dev = alcOpenDevice(NULL);
	ctx = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(ctx);
	
	//alListenerf(AL_GAIN, 0.0f);
}

Audio::~Audio(){
	clear();

	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(dev);
}

void Audio::clear(){
	int index = sounds.getSize();
	while (index--){
		deleteSound(index);
	}
}


SoundID Audio::addSound(const char *fileName, unsigned int flags){
	static ALenum formats[] = {AL_FORMAT_MONO8, AL_FORMAT_STEREO8, AL_FORMAT_MONO16, AL_FORMAT_STEREO16};

	Sound sound;
	sound.audioStream = new AudioStream();
	if (!sound.audioStream->open(fileName)){
		delete sound.audioStream;
		return SOUND_NONE;
	}

	alGenSources(1, &sound.source);

	sound.format = formats[2 * sound.audioStream->getBytesPerSample() + sound.audioStream->getChannels() - 3];
	sound.isLooping = (flags & SND_LOOPING) != 0;
	sound.endOfStream = false;

	unsigned int size;
	if ((sound.isStreaming = ((flags & SND_STREAM) != 0))){
		alGenBuffers(2, sound.buffer);

		for (int i = 0; i < 2; i++){
			size = sound.audioStream->read(&sound.samples[i], BUF_STREAM_SIZE);
			alBufferData(sound.buffer[i], sound.format, sound.samples[i], size, sound.audioStream->getFrequency());
		}
		sound.currStream = 0;

	} else {
		alGenBuffers(1, sound.buffer);
		alSourcei(sound.source, AL_LOOPING, sound.isLooping? AL_TRUE : AL_FALSE);

		size = sound.audioStream->read(&sound.samples[0]);
		alBufferData(sound.buffer[0], sound.format, sound.samples[0], size, sound.audioStream->getFrequency());

		alSourcei(sound.source, AL_BUFFER, sound.buffer[0]);
	}

	return insertSound(sound);
}

SoundID Audio::insertSound(Sound &sound){
	int i, len = sounds.getSize();
	for (i = 0; i < len; i++){
		if (sounds[i].source == SOUND_UNDEFINED){
			sounds[i] = sound;
			return i;
		}
	}

	return sounds.add(sound);
}

void Audio::removeSound(Sound &sound){
	alDeleteSources(1, &sound.source);
	int n = sound.isStreaming? 2 : 1;

	alDeleteBuffers(n, sound.buffer);
	for (int i = 0; i < n; i++){
		delete sound.samples[i];
	}

	sound.audioStream->close();
	delete sound.audioStream;
}

void Audio::deleteSound(const int index){
	if (sounds[index].source != SOUND_UNDEFINED){
		removeSound(sounds[index]);

		sounds[index].source = SOUND_UNDEFINED;
	}
}

bool Audio::playSound(const SoundID soundID){
	if (soundID >= 0){
		if (sounds[soundID].isStreaming){
			alSourceQueueBuffers(sounds[soundID].source, 1, &sounds[soundID].buffer[sounds[soundID].currStream]);
			alSourceQueueBuffers(sounds[soundID].source, 1, &sounds[soundID].buffer[1 - sounds[soundID].currStream]);
		}

		alSourcePlay(sounds[soundID].source);
		return true;
	}
	return false;
}

bool Audio::stopSound(const SoundID soundID){
	if (soundID >= 0){
		alSourceStop(sounds[soundID].source);
		return true;
	}
	return false;
}

bool Audio::pauseSound(const SoundID soundID){
	if (soundID >= 0){
		alSourcePause(sounds[soundID].source);
		return true;
	}
	return false;
}

bool Audio::updateSoundStream(const SoundID soundID){
	if (soundID >= 0 && sounds[soundID].isStreaming){
		ALuint source = sounds[soundID].source;

		ALint nProc = 0;
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &nProc);
		if (nProc > 0){
			int curr = sounds[soundID].currStream;
			alSourceUnqueueBuffers(source, 1, &sounds[soundID].buffer[curr]);
			delete sounds[soundID].samples[curr];

			unsigned int size = sounds[soundID].audioStream->read(&sounds[soundID].samples[curr], BUF_STREAM_SIZE);
			alBufferData(sounds[soundID].buffer[curr], sounds[soundID].format, sounds[soundID].samples[curr], size, sounds[soundID].audioStream->getFrequency());
			if (sounds[soundID].endOfStream){
				ALint nBuf = 0;
				alGetSourcei(source, AL_BUFFERS_QUEUED, &nBuf);

				if (nBuf == 0){
					sounds[soundID].endOfStream = false;
				}
			} else {
				alSourceQueueBuffers(sounds[soundID].source, 1, &sounds[soundID].buffer[curr]);
			}

			sounds[soundID].currStream = 1 - curr;
			if (size != BUF_STREAM_SIZE * sounds[soundID].audioStream->getTotalSampleBytes()){
				sounds[soundID].audioStream->restart();
				/*char *name = new char[strlen(sounds[soundID].audioStream->getName()) + 1];
				strcpy(name, sounds[soundID].audioStream->getName());

				delete sounds[soundID].audioStream;
				sounds[soundID].audioStream = new AudioStream();
				sounds[soundID].audioStream->open(name);
				delete name;*/


				if (!sounds[soundID].isLooping) sounds[soundID].endOfStream = true;
			}

		}

		return true;
	}
	return false;
}


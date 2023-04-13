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

#ifndef _AUDIOSTREAM_H_
#define _AUDIOSTREAM_H_

#include <stdio.h>
#include "../Util/Fifo.h"
#include "../Platform.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>


#define WHOLE_FILE 0xFFFFFFFF

enum SNDTYPE {
	WAV,
	OGG,
};


class AudioStream {
private:
	Fifo <unsigned char *> data;
	
	unsigned int bytesPerSample;
	unsigned int nChannels;
	unsigned int frequency;

	unsigned int blockSize;
	unsigned int currPos;
	unsigned int bufferedSize;
	unsigned int currWritePos;

	OggVorbis_File vf;
	SNDTYPE sndType;
	FILE *file;
	char *name;


public:
	AudioStream();
	~AudioStream();

	unsigned int getBytesPerSample() const { return bytesPerSample; }
	unsigned int getTotalSampleBytes() const { return bytesPerSample * nChannels; }
	unsigned int getChannels() const { return nChannels; }
	unsigned int getFrequency() const { return frequency; }

	char *getName() const { return name; }

	bool open(const char *fileName);
	unsigned int read(unsigned char **buffer, unsigned int numSamples = WHOLE_FILE);
	void close();

	bool restart();
};



#endif // _AUDIOSTREAM_H_

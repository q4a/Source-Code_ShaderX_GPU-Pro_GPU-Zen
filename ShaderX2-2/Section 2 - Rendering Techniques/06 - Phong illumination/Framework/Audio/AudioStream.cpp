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

#include "AudioStream.h"
#include <string.h>

#ifdef WIN32

#ifdef _DEBUG
#pragma comment (lib, "vorbisfile_d.lib")
#pragma comment (lib, "vorbis_d.lib")
#pragma comment (lib, "ogg_d.lib")
#else
#pragma comment (lib, "vorbisfile.lib")
#pragma comment (lib, "vorbis.lib")
#pragma comment (lib, "ogg.lib")

#endif

#endif


#pragma pack (push, 1)

struct WavHeader {
	uint32 riff;
	uint32 riffSize;
	uint32 wave;
	uint32 fmt;
	uint32 chunkSize;
	uint16 format;
	uint16 nChannels;
	uint32 frequency;
	uint32 avgBytesPerSec;
	uint16 nBlockAlign;
	uint16 nBitsPerSample;
};

#pragma pack (pop)



AudioStream::AudioStream(){
	bytesPerSample = 0;
	nChannels = 0;
	frequency = 0;

	blockSize = 65536;
	currPos = 0;
	bufferedSize = 0;
	currWritePos = 0;

	file = NULL;
	name = NULL;
}

AudioStream::~AudioStream(){
	close();
	delete name;
}

bool AudioStream::open(const char *fileName){
	char *ext = strrchr(fileName, '.');
	if (ext == NULL) return false;

	if ((file = fopen(fileName, "rb")) == NULL) return false;

	if (stricmp(ext, ".ogg") == 0){

		memset(&vf, 0, sizeof(vf));
		if (ov_open(file, &vf, NULL, 0) < 0){
			fclose(file);
			file = NULL;
			return false;
		}

		vorbis_info *vi = ov_info(&vf, -1);

		bytesPerSample = 2;
		nChannels = vi->channels;
		frequency = vi->rate;
	
		sndType = OGG;

	} else {
	
		WavHeader header;
		fread(&header, sizeof(header), 1, file);

		if (header.riff != MCHAR4('R','I','F','F') || header.wave != MCHAR4('W','A','V','E') || header.fmt != MCHAR4('f','m','t',' ')){
			fclose(file);
			file = NULL;
			return false;
		}

		bytesPerSample = header.nBitsPerSample / 8;
		nChannels = header.nChannels;
		frequency = header.frequency;
		sndType = WAV;
	}

	if (name == NULL){
		name = new char[strlen(fileName) + 1];
		strcpy(name, fileName);
	}

	return true;
}

unsigned int AudioStream::read(unsigned char **buffer, unsigned int numSamples){
	unsigned int size = numSamples * bytesPerSample * nChannels;
	unsigned char *wavData;
	int current_section;
	bool eof = false;

	if (currWritePos) wavData = data.getLast();

	// Try to read as much as possible and buffer it up
	while (bufferedSize < size && !eof){
		if (currWritePos == 0){
			wavData = new unsigned char[blockSize];
			
			data.enqueue(wavData);
		}

		unsigned int readBytes;
		if (sndType == OGG){
		    readBytes = ov_read(&vf, (char *) wavData + currWritePos, blockSize - currWritePos, 0, bytesPerSample, (bytesPerSample > 1), &current_section);
			if (readBytes <= 0) eof = true;
		} else {
			readBytes = fread(wavData + currWritePos, 1, blockSize - currWritePos, file);
			if (readBytes != blockSize) eof = true;
		}
		bufferedSize += readBytes;
		currWritePos += readBytes;
		if (currWritePos == blockSize) currWritePos = 0;
	}

	unsigned int fillSize = (bufferedSize < size)? bufferedSize : size;
	unsigned int returnSize = fillSize; 

	bufferedSize -= fillSize;

	unsigned char *dest = *buffer = new unsigned char[fillSize];

	while (fillSize){
		unsigned char *src = data.getFirst();
		unsigned int maxBlock = blockSize - currPos;

		if (fillSize >= maxBlock){
			memcpy(dest, src + currPos, maxBlock);

			delete src;
			data.dequeue();

			currPos = 0;
			fillSize -= maxBlock;
			dest += maxBlock;
		} else {
			memcpy(dest, src + currPos, fillSize);
			currPos += fillSize;
			fillSize = 0;
		}
	}

	return returnSize;
}


void AudioStream::close(){
	if (sndType == OGG) ov_clear(&vf);

	if (file != NULL) fclose(file);
	file = NULL;

	int n = data.getSize();
	while (n--){
		delete data.getFirst();
		data.dequeue();
	}
}

bool AudioStream::restart(){
	close();
	currPos = 0;
	bufferedSize = 0;
	currWritePos = 0;
	return open(name);
}

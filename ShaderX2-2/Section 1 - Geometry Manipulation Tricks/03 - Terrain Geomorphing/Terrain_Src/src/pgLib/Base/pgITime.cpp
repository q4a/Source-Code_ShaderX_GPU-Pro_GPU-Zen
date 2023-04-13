///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgITime
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgITime.cpp,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgITime.h"


static float lastFrameTime = 0.0f, fps = 0.0f;

static bool firstUpdate = true;
static LARGE_INTEGER time0, time1, freq;



void pgITime::update()
{
	lastFrameTime = 0.0f;
	fps = 0.0f;
	QueryPerformanceCounter(&time1);

	if(firstUpdate)
	{
		firstUpdate = false;
		QueryPerformanceFrequency(&freq);
		time0 = time1;
	}

	if(freq.QuadPart!=0)
		lastFrameTime =  (float)(time1.QuadPart-time0.QuadPart) / (float)(freq.QuadPart);

	if(lastFrameTime>0.0f)
		fps = 1.0f / lastFrameTime;

	time0 = time1;
}


float
pgITime::getLastFrameTime()
{
	return lastFrameTime;
}


float
pgITime::getFPS()
{
	return fps;
}


pgTimeInstance pgITime::getCurrentTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	return pgTimeInstance(time);
}


float pgITime::getTimeSince(const pgTimeInstance& nInst)
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	if(freq.LowPart!=0)
		return (float)(time.QuadPart-nInst.getQuadPart()) / (float)(freq.QuadPart);

	return 0.0f;
}


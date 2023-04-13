/*----------------------------------------------------------------------------*\
|
|								NovodeX Technology
|
|							     www.novodex.com
|
\*----------------------------------------------------------------------------*/
#define WIN32_LEAN_AND_MEAN
#define NOCRYPT
#define NOGDI
#define NOMCX
#define NOIME

#include <windows.h>

#include "Time.h"
Time::Time()
	{
	GetElapsedSeconds();
	}

//hi precision timer:

second Time::GetElapsedSeconds()	//returns ms elapsed since last call.
	{
	second curTime, newTime;
	newTime = GetTimeTicks();
	curTime = newTime-lstTimeS;
	lstTimeS = newTime;
	return (curTime / GetClockFrequency());
	}

second Time::PeekElapsedSeconds()
	{
	return (GetTimeTicks()-lstTimeS)/GetClockFrequency();
	}

second Time::GetTimeTicks()
	{
	LARGE_INTEGER a;
	QueryPerformanceCounter (&a);
	return double(a.QuadPart);
	}

second Time::GetClockFrequency()
	{
	static int haveIt=0;
	static second hz=0.0;
	if (!haveIt) 
		{
		LARGE_INTEGER a;
		QueryPerformanceFrequency (&a);
		hz = second(a.QuadPart);
		haveIt = 1;
		}
	return hz;
	}

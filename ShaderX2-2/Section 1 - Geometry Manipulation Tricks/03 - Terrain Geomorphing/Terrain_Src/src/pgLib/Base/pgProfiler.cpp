///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgProfiler
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgProfiler.cpp,v 1.2 2002/12/24 16:04:36 daniel Exp $
///////////////////////////////////////////////////////////////////


#include <pgLib/Base/pgProfiler.h>


pgList<pgProfiler::ProfileInfo> pgProfiler::timerList;
LARGE_INTEGER					pgProfiler::globalBeginTime;


pgProfiler::pgProfiler(const char* nName)
{
	ProfileInfo pi;

	pi.name = nName;
	pi.time.QuadPart = 0;

	if(timerList.getSize()==0)
		pgProfiler::globalBegin();

	listIndex = timerList.addTail(pi);
}


pgProfiler::~pgProfiler()
{
}


/*void
pgProfiler::beginSection()
{
	QueryPerformanceCounter(&beginTime);
}


void
pgProfiler::endSection()
{
	LARGE_INTEGER&	time = timerList.getAt(listIndex).time;
	LARGE_INTEGER	tmpTime;

	QueryPerformanceCounter(&tmpTime);

	time.QuadPart += tmpTime.QuadPart - beginTime.QuadPart;
}*/


bool
pgProfiler::globalBegin()
{
	if(timerList.getSize()!=0)
		return false;

	ProfileInfo pi;

	pi.name = "Overall";
	pi.time.QuadPart = 0;

	timerList.addTail(pi);

	QueryPerformanceCounter(&globalBeginTime);
	return true;
}


bool
pgProfiler::globalEnd()
{
	if(timerList.getSize()==0)
		return false;

	ProfileInfo&	pi = timerList.getAt(0);
	LARGE_INTEGER	time;

	if(pi.name != "Overall")
		return false;

	pi.name = "Overall DONE";

	QueryPerformanceCounter(&time);

	pi.time.QuadPart = time.QuadPart - globalBeginTime.QuadPart;
	return true;
}


void
pgProfiler::logResult()
{
#ifndef PGLIB_PROFILE
	pgLog::info("==============================================================");
	pgLog::info("");
	pgLog::info(" profiling disabled. enable by defining compiler variable PGLIB_PROFILE");
	pgLog::info("");
	pgLog::info("==============================================================");
#endif //PGLIB_PROFILE

	pgLog::info("==============================================================");
	pgLog::info("");
	pgLog::info(" profiling results:");
	pgLog::info("");


	if(timerList.getSize()==0)
	{
		pgLog::info("  nothing profiled");
		pgLog::info("");
		pgLog::info("==============================================================");
		return;
	}

	ProfileInfo&	globalPi = timerList.getAt(0);
	LARGE_INTEGER	freq;
	pgString		str;

	if(globalPi.name!="Overall DONE" && !pgProfiler::globalEnd())
	{
		pgLog::error("calling pgProfiler::globalEnd() failed");
		pgLog::info("");
		pgLog::info("==============================================================");
		return;
	}

	QueryPerformanceFrequency(&freq);

	pgLog::info(" overall time: %.2f seconds", (double)globalPi.time.QuadPart / (double)freq.QuadPart);

	for(int i=1; i<timerList.getSize(); i++)
	{
		ProfileInfo&	pi = timerList.getAt(i);
		double			v = 100.0*(double)pi.time.QuadPart / (double)globalPi.time.QuadPart;

		str.set("%s %.2f", (v<10.0 ? " " : ""), v);

		pgLog::info(str + "%%%%  : " + pi.name);
	}


	pgLog::info("");
	pgLog::info("==============================================================");
}

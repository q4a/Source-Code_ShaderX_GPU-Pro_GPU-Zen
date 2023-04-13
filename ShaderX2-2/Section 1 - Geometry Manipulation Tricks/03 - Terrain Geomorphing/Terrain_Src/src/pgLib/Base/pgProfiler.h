///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgProfiler
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgProfiler.h,v 1.2 2002/12/24 16:04:36 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __PROFILE_HEADERFILE__
#define __PROFILE_HEADERFILE__

#include <pgLib/pgLib.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgString.h>

#include <windows.h>


/// pgProfiler provides precise profiling methods
/**
 *  pgProfiler uses the processors VERY HIGH PRECISSION
 *  timer functions, which are precise enough to bench
 *  even single assembler commands...
 */
class PGLIB_API pgProfiler
{
public:
	pgProfiler(const char* nName);

	~pgProfiler();


	void beginSection();

	void endSection();


	static void logResult();


protected:
	static bool globalBegin();
	static bool globalEnd();


	int				listIndex;
	LARGE_INTEGER	beginTime;

	struct ProfileInfo
	{
		pgString		name;
		LARGE_INTEGER	time;
	};

	friend bool operator==(const ProfileInfo&, const ProfileInfo&)  {  return false;  }


	static pgList<ProfileInfo>	timerList;
	static LARGE_INTEGER		globalBeginTime;
};


inline void
pgProfiler::beginSection()
{
#ifdef PGLIB_PROFILE
	QueryPerformanceCounter(&beginTime);
#endif //PGLIB_PROFILE
}


inline void
pgProfiler::endSection()
{
#ifdef PGLIB_PROFILE
	LARGE_INTEGER&	time = timerList.getAt(listIndex).time;
	LARGE_INTEGER	tmpTime;

	QueryPerformanceCounter(&tmpTime);

	time.QuadPart += tmpTime.QuadPart - beginTime.QuadPart;
#endif //PGLIB_PROFILE
}


#endif //__PROFILE_HEADERFILE__

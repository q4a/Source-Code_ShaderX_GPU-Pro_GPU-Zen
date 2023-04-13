///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgITime
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgITime.h,v 1.2 2002/12/07 19:58:57 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TIMER_HEADERFILE__
#define __TIMER_HEADERFILE__

#include <pgLib/pgLib.h>

#include <windows.h>


/// this class represents a specific instance in time (a distinct point on the timeline)
class PGLIB_API pgTimeInstance
{
public:
	pgTimeInstance()  {}
	pgTimeInstance(LARGE_INTEGER nTime)  {  time = nTime;  }

	LONGLONG getQuadPart() const  {  return time.QuadPart;  }

private:
	LARGE_INTEGER time;
};


/// pgTime provides precise timing methods
/**
 *  pgTime uses the processors VERY HIGH PRECISSION
 *  timer functions, which are precise enough to bench
 *  even single assembler commands...
 */
namespace pgITime
{
	/// Update must only be called once per frame (before any other pgTime method is used)
	PGLIB_API void update();

	/// Returns in seconds, how long the last frame took
	/**
	 *  The time difference between the last two
	 *  calls to update() is returned.
	 */
	PGLIB_API float getLastFrameTime();

	/// returns the current frames per second based on the last frame time
	PGLIB_API float getFPS();

	/// returns the current time in very high precision as pgTimeInstance object
	PGLIB_API pgTimeInstance getCurrentTime();

	/// returns the time if seconds since which have passed since nInst
	PGLIB_API float getTimeSince(const pgTimeInstance& nInst);
};


#endif //__TIMER_HEADERFILE__

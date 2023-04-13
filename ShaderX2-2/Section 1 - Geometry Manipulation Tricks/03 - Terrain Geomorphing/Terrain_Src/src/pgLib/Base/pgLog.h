///////////////////////////////////////////////////////////////////
//
// project:     Geisterschloss
// class:       pgLog
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgLog.h,v 1.3 2002/12/24 15:10:41 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __LOG_HEADERFILE__
#define __LOG_HEADERFILE__


#include <pgLib/pgLib.h>

/// Use this class from anywhere to do logging into a file
/**
 *  Before the first logging method is invoked, init() must be called.
 */
class PGLIB_API pgLog
{
public:
	/// init() has to be called before the first log can be saved.
	/**
	 *  Since most of pgLibs classes do logging during their initialization,
	 *  this method has to be called, before any other initialization is done.
	 *  Therefore calling pgLog::init() should be the first call of any pgLib method.
	 */
	static void init(const char* nLogFileName);


	/// Logs an info message
	/**
	 *  Parameters work same as for printf
	 */
	static void info(const char *szFormat, ...);


	/// Logs a warning message
	/**
	 *  Parameters work same as for printf
	 */
	static void warning(const char *szFormat, ...);


	/// Logs an error message
	/**
	 *  Parameters work same as for printf
	 */
	static void error(const char *szFormat, ...);


	/// Writes a message to debug output in visual studio
	/**
	 *  Parameters work same as for printf
	 */
	static void trace(const char *szFormat, ...);

private:
	static char fileName[1024];
};


#endif //__LOG_HEADERFILE__

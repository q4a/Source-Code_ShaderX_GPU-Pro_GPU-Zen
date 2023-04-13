///////////////////////////////////////////////////////////////////
//
// project:     Geisterschloss
// class:       pgLog
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgLog.cpp,v 1.4 2002/12/24 15:10:41 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgLog.h"

#include <direct.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


char pgLog::fileName[1024];


void pgLog::init(const char* nLogFileName)
{
	_getcwd(fileName, _MAX_PATH);
	strcat(fileName, "\\");
	strcat(fileName, nLogFileName);


	FILE* fp = fopen(fileName, "a+");

	if(fp==NULL)
		return;

	fprintf(fp, "\n\n\n");
	fclose(fp);


	info("================================================================");
	info("");
	info("                           Log opened");
	info("");
	info("================================================================");
	info("");
}


void pgLog::info(const char *szFormat, ...)
{
	char buffer[512], buffer2[512];  // Large buffer

	// Format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsprintf(buffer, szFormat, pArgs);
	va_end(pArgs);

    char tmpbuf[128];
    time_t ltime;
    struct tm today;

	time(&ltime);
	today = *localtime(&ltime);
	strftime(tmpbuf, 128, "%Y-%m-%d %H:%M:%S", &today);

	sprintf(buffer2, "%s INFO:  %s", tmpbuf, buffer);


	FILE* fp = fopen(fileName, "a+");

	if(fp==NULL)
		return;

	fprintf(fp, buffer2);
	fprintf(fp, "\n");
	fclose(fp);
}


void pgLog::warning(const char *szFormat, ...)
{
	char buffer[512], buffer2[512];  // Large buffer

	// Format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsprintf(buffer, szFormat, pArgs);
	va_end(pArgs);

    char tmpbuf[128];
    time_t ltime;
    struct tm today;

	time(&ltime);
	today = *localtime(&ltime);
	strftime(tmpbuf, 128, "%Y-%m-%d %H:%M:%S", &today);

	sprintf(buffer2, "%s WARNING:  %s", tmpbuf, buffer);


	FILE* fp = fopen(fileName, "a+");

	if(fp==NULL)
		return;

	fprintf(fp, buffer2);
	fprintf(fp, "\n");
	fclose(fp);
}


void pgLog::error(const char *szFormat, ...)
{
	char buffer[512], buffer2[512];  // large buffers

	// format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsprintf(buffer, szFormat, pArgs);
	va_end(pArgs);

    char tmpbuf[128];
    time_t ltime;
    struct tm *today;

	time(&ltime);
	today = localtime(&ltime);
	strftime(tmpbuf, 128, "%Y-%m-%d %H:%M:%S", today);

	sprintf(buffer2, "%s ERROR:  %s", tmpbuf, buffer);


	FILE* fp = fopen(fileName, "a+");

	if(fp==NULL)
		return;

	fprintf(fp, buffer2);
	fprintf(fp, "\n");
	fclose(fp);
}


void pgLog::trace(const char *szFormat, ...)
{
	szFormat;

#ifdef _DEBUG
	char buffer[512];  // large buffer

	// format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsprintf(buffer, szFormat, pArgs);
	va_end(pArgs);

	OutputDebugString(buffer);
#endif //_DEBUG
}

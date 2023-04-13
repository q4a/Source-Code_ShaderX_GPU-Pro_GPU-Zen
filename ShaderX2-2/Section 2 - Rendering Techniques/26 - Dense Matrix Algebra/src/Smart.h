#ifndef __SMART_H__
#define __SMART_H__
/*-------------------------------*\
| Some smart stuff
|
|
\*-------------------------------*/
#include <stdio.h>		//to define NULL
#include <assert.h>		//a convenience
#include <string.h>		//for strcpy below
#include <stdlib.h>

#define sdelete(a) if (a!=NULL) {delete a; a = NULL;}


//basic type definitions (only somewhat platform dependant.)
typedef unsigned char	uint8;
typedef signed char		sint8;
typedef unsigned short	uint16;
typedef signed short	sint16;
typedef unsigned int	uint32;
typedef signed int		sint32;
typedef uint32			milisec;
typedef double			second;
typedef char *			string;
typedef float			flo;




#endif //__SMART_H__


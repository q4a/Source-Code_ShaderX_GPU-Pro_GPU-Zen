/////////////////////////////////////////////////////////////////
//
// Global types
// Copyright GP
// Created 3.6.2000
//
//$DONTCONVERTME
//
/////////////////////////////////////////////////////////////////

#ifndef __GLOBALS_TYPES__
#define __GLOBALS_TYPES__


#ifndef NO_XUBL_TYPES

   // this check prevents from declaring basic types 
   #ifndef NO_BASIC_TYPES

      // basic types
      //#define char                  signed small
      //#define int                   signed __int32

      // other basic types types
      typedef unsigned __int8			byte;

   #endif

   typedef unsigned __int32	   uint;
   typedef unsigned __int16		word;

   // basic integer signed types
   typedef signed __int8         int8;
   typedef signed __int16		   int16;
   typedef signed __int32			int32;
   typedef signed __int64        int64;

   // basic integer unsigned types
   typedef unsigned __int8       uint8;
   typedef unsigned __int16		uint16;
   typedef unsigned __int32		uint32;
   typedef unsigned __int64      uint64;
#endif

#endif


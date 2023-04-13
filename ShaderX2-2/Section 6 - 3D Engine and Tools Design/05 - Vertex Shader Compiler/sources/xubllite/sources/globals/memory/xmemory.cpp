///////////////////////////////////////////////////////////////////////////
//
// X Memory management logging
// Copyright GP
//
///////////////////////////////////////////////////////////////////////////
//$XHEADER

#include "xmemory.h"
#include <globals/globals.h>

void  *__resize__(void *ptr , int oldsize , int newsize )
{
   if( ptr )     
   {
      if( newsize )
      {
         char *buf=xnewa( char , newsize );
         memcpy( buf , ptr , min( newsize , oldsize ) );
         //
         xdeletea( ptr );
         //
         return buf;
      }
      else
      {
         xdeletea( ptr );
         return 0;
      }
   }
   else
   {
      if( newsize )
         return xnewa( char , newsize );
      else
         return 0;
   }
}

XDLL void * __new__( unsigned int size )
{
   if( size<=0 ) return 0;
   //
   void *a=(int*)malloc( size );

   // Clear memory
   memset( a , 0 , size );

   // return value 
   return a;
}

XDLL void __delete__( void *ptr )
{
   if( ptr==0 ) return;

   // delete memory
   free( ptr );

   return;
}

///////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////

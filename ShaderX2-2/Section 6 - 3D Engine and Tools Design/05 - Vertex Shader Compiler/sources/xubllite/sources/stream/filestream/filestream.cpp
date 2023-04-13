///////////////////////////////////////////////////////////////////////////
//
// Copyright GP
//
// File Stream
//
///////////////////////////////////////////////////////////////////////////
//$XHEADER
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>

#include "filestream.h"
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>

XDLL TFileStreamPipeReqCallback      *pFileStreamPipeReqRead=0;
XDLL TFileStreamPipeReqCallback      *pFileStreamPipeReqWrite=0;


int TFileStream::Open( const char *aname, TStreamMode mode )
{
   Close();
   //
   xdelete( pPipe );
   //
   if( !aname || aname[0]==0 ) return 1;
   //
   name=aname;
   //
   opened=0;
   //
   char rm[5];
   switch(mode)
   {
   case smReadWrite: strcpy(rm,"rb+"); break;
   case smWrite:     strcpy(rm,"wb"); break;
   case smRead: 
   default:          strcpy(rm,"rb"); break;
   }
   //
   fhandle = fopen(  name , rm );
   //
   if( fhandle<=0 )
   {
      switch(mode) {
      case smWrite:     
         if( pFileStreamPipeReqWrite ) pPipe=pFileStreamPipeReqWrite( aname );
         break;
      case smRead: 
         if( pFileStreamPipeReqRead ) pPipe=pFileStreamPipeReqRead( aname );
         break;
      }
      if( !pPipe ) 
      {
         switch(mode)
         {
         case smReadWrite:
         case smWrite: 
            Status=ssUnableToCreateFile;
            break;
         default: 
            Status=ssFileNotFound;
         };
         return 1;
      }
   }
   //
   Status=ssOK;
   opened=1;
   //
   return 0;
};



int TFileStream::GetDate()
{
   if( pPipe ) return 0;
   //
   if( !opened ) 
   {
      Status=ssFileNotOpened;
      return -1; 
   }
   struct stat    stat;
   //
   fstat( _fileno( fhandle ) , &stat );
   //
   return stat.st_mtime;
}




///////////////////////////////////////////////////////////////////////////
//
// Copyright GP
//
// File Stream
//
///////////////////////////////////////////////////////////////////////////

#ifndef	__FILE_STREAM__
#define	__FILE_STREAM__

#include <stream/stream/stream.h>

///////////////////////////////////////////////////////////////////////////
//
// TFileStream
//
///////////////////////////////////////////////////////////////////////////

typedef TStream *(TFileStreamPipeReqCallback)(const char *fn);

extern XDLL TFileStreamPipeReqCallback      *pFileStreamPipeReqRead;
extern XDLL TFileStreamPipeReqCallback      *pFileStreamPipeReqWrite;

class XDLL TFileStream : public TStream {
   int               opened;
   FILE              *fhandle;
   TStream           *pPipe;
public:
	                  TFileStream()		
                     { 
                        fhandle=0;	
                        opened=0;	
                        pPipe=0;
                     };
	                  ~TFileStream()		
                     { 
                        Close();	
                        xdelete( pPipe );
                     };
	                  // 
	                  TFileStream( const char *name, TStreamMode mode=smRead )
	                  {
                        fhandle=0;	
                        opened=0;	
                        pPipe=0;
                        //
		                  Open( name, mode );
	                  };
public:
	int               Open( const char *aname, TStreamMode mode=smRead );
	void              Close();
	int               Read( void *buffer, int size );
	int               Write( void *buffer, int size );
   //
	int               Seek( int np , TStreamSeek seek=ssStart );
	int               GetPos();
	int               GetSize();
   //                
   int               Commit();
	//                
   int               Opened();
   //                
   int               Eof();
   int               GetDate();
};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

inline void TFileStream::Close()
{
   if( pPipe )
   {
      pPipe->Close();
      return;
   }
   //
   if (opened) 
   {
      fclose( fhandle );
      opened=0;
   }
   else
   {
      Status=ssFileNotOpened;
   };
};

inline int TFileStream::Read( void *buffer, int size )
{
   if( pPipe ) return pPipe->Read( buffer , size );
   //
   if (!opened)
   {
      Status=ssFileNotOpened;
      return -1;
   }
   //
	int r=fread( buffer , 1 , size , fhandle );
	//
   return r;
};

inline int TFileStream::Write( void *buffer, int size )
{
   if( pPipe ) return pPipe->Write( buffer , size );
   //
   if (!opened)
   {
      Status=ssFileNotOpened;
      return -1;
   }
   //
   return fwrite( buffer , 1 , size , fhandle );
};

inline int TFileStream::Seek( int np , TStreamSeek seek )
{
   if( pPipe ) return pPipe->Seek( np , seek );
   //
   if (!opened)
   {
      Status=ssFileNotOpened;
      return -1;
   }
   //
   int whence=SEEK_SET;
   //
   switch( seek ){
   case ssCurrent  : whence=SEEK_CUR; break;
   case ssEnd      : whence=SEEK_END; break;
   };
   //
   return fseek( fhandle , np , whence );
}

inline int TFileStream::GetPos()
{
   if( pPipe ) return pPipe->GetPos();
   //
   if (!opened)
   {
      Status=ssFileNotOpened;
      return -1;
   }
   //
   return (int)ftell( fhandle );
}

inline int TFileStream::GetSize()
{
   if( pPipe ) return pPipe->GetSize();
   //
   if (!opened)
   {
      Status=ssFileNotOpened;
      return -1;
   }
   //
   /**/
   int pos=GetPos();
   fseek( fhandle , 0 , SEEK_END );
	int size=GetPos();
   Seek( pos );
   return size;
}

inline int TFileStream::Commit()
{
   if( pPipe ) return pPipe->Commit();
   //
   return fflush( fhandle );
}

inline int TFileStream::Opened()
{
   if( pPipe ) return pPipe->Opened();
   //
   return opened;
}

inline int TFileStream::Eof()
{ 
   if( pPipe ) return pPipe->Eof();
   //
   return feof( fhandle ); 
}

#endif

///////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////

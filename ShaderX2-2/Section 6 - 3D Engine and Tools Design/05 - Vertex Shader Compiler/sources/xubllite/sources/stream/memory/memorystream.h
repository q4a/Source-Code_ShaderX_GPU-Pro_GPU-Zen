///////////////////////////////////////////////////////////////////////////
//
// Memory Stream
// Copyright GP
//
///////////////////////////////////////////////////////////////////////////

#ifndef	__MEMORY_STREAM__
#define	__MEMORY_STREAM__

#include <globals/globals.h>

///////////////////////////////////////////////////////////////////////////
//
// TMemoryStream
//
///////////////////////////////////////////////////////////////////////////

class XDLL TMemoryStream : public TStream {
   int               pos;
   TArray<char>      aBuffer;
public:
	                  TMemoryStream()		
                     { 
                        pos=0;
                     }
                     //
	                  TMemoryStream( const char *name, TStreamMode mode=smRead )
	                  {
		                  Open( name, mode );
	                  };
public:
   //
	int Open( const char *aname, TStreamMode mode=smRead )
   {
      pos=0;
      aBuffer.Clear();
      return 0;
   };
   //
   void Close(){}
   //
	int Read( void *buffer, int size )
   {
      if( pos >= aBuffer.Count() ) return 0;
      //
      int rs=size;
      if( pos + rs > aBuffer.Count() ) rs=aBuffer.Count()-pos;
      memcpy( buffer , &aBuffer[ pos ] , rs );
      pos+=rs;
      return rs;
   };
   //
	int Write( void *buffer, int size )
   {
      if(size<=0) return 0;
      //
      if( pos+size >= aBuffer.Count() ) aBuffer.SetCount( pos + size );
      //
      memcpy( &aBuffer[ pos ] , buffer , size );
      pos+=size;
      return size;
   };
   int Eof()
   {
      return pos >= aBuffer.Count();
   }
   //
	int Seek( int np , TStreamSeek seek=ssStart )
   {
      switch( seek ) {
      case ssEnd     : pos =aBuffer-np;
      case ssCurrent : pos+=np;
      default        : pos =np;
      };
      //
      if( pos > aBuffer ) pos=aBuffer;
      if( pos < 0 ) pos=0;
      //
      return 0;
   }
	int GetPos(){return pos;}
   //
	int GetSize(){return aBuffer.Count();}
   //
	void SetSize(int size)
   {
      aBuffer.SetCount(size);
      //
      pos=min( size , pos );
      pos=max( pos , 0 );
   }
   //
   int Opened(){return 1;}
   //
   void *GetBuffer(){return &aBuffer[0];}
   //
   void Assign(const void *buffer,int size) // assign buffer
   {
      int pos=size;
      //
      aBuffer.SetCount( size );
      memcpy( &aBuffer[0] , buffer , size );
   }
};

///////////////////////////////////////////////////////////////////////////
//
// TDirectStream
//
///////////////////////////////////////////////////////////////////////////

class XDLL TDirectStream : public TStream {
   int               pos;
   int               opened;
   //
   int               size;
   char              *pBuffer;
public:
	                  TDirectStream()		
                     { 
                        pos=0;
                        opened=0;
                        size=0;
                        pBuffer=0;
                     }
                     //
	                  TDirectStream( const char *name, TStreamMode mode=smRead )
	                  {
		                  Open( name, mode );
	                  };
public:
   //
	int Open( const char *aname, TStreamMode mode=smRead )
   {
      pos=0;
      opened=1;
      size=0;
      pBuffer=(char*)aname;
      return 0;
   };
   //
	void AssignBuffer( void *buf, int asize )
   {
      pos=0;
      opened=1;
      size=asize;
      pBuffer=(char*)buf;
   };
   //
	void Close()
   {
      opened=0;
   };
   //
	int Read( void *buffer, int wsize )
   {
      if(!opened) return 0;
      if( pos >= size ) return 0;
      //
      int rs=wsize;
      if( pos + rs > size ) rs=size-pos;
      //
      memcpy( buffer , &pBuffer[ pos ] , rs );
      pos+=rs;
      return rs;
   };
   //
	int Write( void *buffer, int wsize )
   {
      if(!opened) return 0;
      //
      if(wsize<=0) return 0;
      //
      int rs=wsize;
      if( pos + rs > size ) rs=size-pos;
      //
      memcpy( &pBuffer[ pos ] , buffer , rs );
      pos+=rs;
      return rs;
   };
   int Eof()
   {
      return pos >= size;
   }
   //
	int Seek( int np , int whence=SEEK_SET )
   {
      if(!opened) return 0;
      //
      pos=np;
      if( pos>size ) pos=size;
      if( pos<0 ) pos=0;
      //
      return 0;
   }
	int GetPos(){return pos;}
   //
	int GetSize(){return size;}
   //
   int Opened(){return opened;}
   //
   void *GetBuffer(){return pBuffer;}
};



#endif
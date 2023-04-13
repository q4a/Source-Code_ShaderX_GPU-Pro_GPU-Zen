///////////////////////////////////////////////////////////////////////////
//
// Stream
// Copyright GP
// Main Stream 
//
///////////////////////////////////////////////////////////////////////////

#ifndef	__MAINSTREAM__
#define	__MAINSTREAM__

#include <globals/globals.h>

// Stream modes
enum TStreamMode {smRead,smWrite,smReadWrite};
enum TStreamSeek {ssStart,ssCurrent,ssEnd};


const int      stream2streambuffersize    = 0x10000;

/////////////////////////////////////////////////////////////////////////
// Stream status constants
/////////////////////////////////////////////////////////////////////////

enum TStreamStatus {
   ssUnsupported              =  -1,
   ssOK                       =   0,
   ssFileNotOpened            =   1,
   ssInvalidStreamMode        =   2,
   ssErrorReadingFile         =   3,
   ssErrorWritingFile         =   4,
   ssFileNotFound             =   5,
   ssUnableToCreateFile       =   6,
};

/////////////////////////////////////////////////////////////////////////
//
// TStreamBase
//
/////////////////////////////////////////////////////////////////////////

class XDLL TStreamBase {
public:
   virtual           ~TStreamBase(){}
public:
	virtual int       Read( void *buffer, int size )=0;
	virtual int       Write( void *buffer, int size )=0;
   //
	virtual void      Restart()=0;
public:
   // on stream 
	virtual int       ReadStream( TStreamBase *outstream, int size=MAXINT );
	virtual int       WriteStream( TStreamBase *instream, int size=MAXINT );
public:
   // functionality
	virtual int       ReadString( TString &str , int size );
	virtual int       WriteString( const TString &str , int size=-1 );
};

/////////////////////////////////////////////////////////////////////////
//
// TStream
//
/////////////////////////////////////////////////////////////////////////

class XDLL TStream : public TStreamBase {
public:
   TName             name;
   TStreamStatus     Status;
   //
public: // constructor / destructor
                     TStream()
                     {
                        Status=ssOK;
                     }
   //
   virtual           ~TStream(){}
   //
public: // basic functionality
   virtual int       Opened()=0;
	virtual int       Open( const char *name, TStreamMode mode=smRead )=0;
	virtual void      Close()=0;
   //
public: // inherited from TStreamBase
	virtual int       Read( void *buffer, int size )=0;
	virtual int       Write( void *buffer, int size )=0;
   //
   virtual void      Restart(){Seek(0);}
   //
public: // advanced
	virtual int       GetPos(){return ssUnsupported;}
   virtual int       GetSize(){return ssUnsupported;}
	virtual int       Seek( int np , TStreamSeek seek=ssStart){return ssUnsupported;}
   //
   virtual int       Eof(){return ssUnsupported;}
   //
   virtual int       Lock(){return ssUnsupported;}
   virtual int       Unlock(){return ssUnsupported;}
   //
   virtual int       Commit(){return ssUnsupported;}
   //
public:// functuality
   // on char
	virtual int       Write( const char *c );
   int               Readln(char *buf,int maxbuf=-1);
   int               Readln(TString &str);
};

////////////////////////////////////////////////////////////////////////////////
//
// TStreamPipe functions implementation
//
////////////////////////////////////////////////////////////////////////////////

inline int TStreamBase::ReadStream( TStreamBase *outstream, int size )
{
   int   rsize=min( size , stream2streambuffersize );
   void  *buf=xnewa( char , rsize );
   //
   int read=0;
   do
   {
      rsize=min( size , stream2streambuffersize );
      //
      int cread=Read( buf , rsize );
      if( cread<=0 ) break;
      //
      outstream->Write( buf , cread );
      //
      read+=cread;
      //
      size-=cread;
   } while( size > 0 );
   //
   xdelete( buf );
   //
   return read;
}

inline int TStreamBase::WriteStream( TStreamBase *instream, int size )
{
   int   rsize=min( size , stream2streambuffersize );
   void  *buf=xnewa( char , rsize );
   //
   int read=0;
   do
   {
      rsize=min( size , stream2streambuffersize );
      //
      int cread=instream->Read( buf , rsize );
      if( cread<=0 ) break;
      //
      Write( buf , cread );
      //
      read+=cread;
      //
      size-=cread;
   } while( size > 0 );
   //
   xdelete( buf );
   //
   return read;
}

// functionality
inline int TStreamBase::ReadString( TString &str, int size )
{
   str.SetSize( size );
   int r=Read( str.Lock() , size );
   //
   str.Unlock();
   // ensure that string ends with 0
   if( r<size ) str[r]=0;
   //
   return r;
}

inline int TStreamBase::WriteString( const TString &str , int size )
{
   return Write( (void*)tochar(str) , (size>0?size:(int)str) );
}


////////////////////////////////////////////////////////////////////////////////
//
// TStream functions implementation
//
////////////////////////////////////////////////////////////////////////////////

// on char
inline int TStream::Write( const char *c )
{
   return Write( (void*)c , strlen(c) );
}
//
inline int TStream::Readln(char *buf,int maxbuf)
{
   int nbuf=0;
   while(1)
   {
      int r=Read( &buf[nbuf] , 1 );
      //
      if(r!=1) 
      {
         buf[nbuf]=0;
         //
         if(nbuf) return nbuf;   
             else return -1;
      }
      //
      if(buf[nbuf]==13)
      {
         // take out #10 if DOSEOF
         #ifdef DOSEOL
            Read( &buf[nbuf] , 1 );
         #endif
         buf[nbuf]=0;
         // return -1 (to ignore 13)
         return max( nbuf-1 , 0 );
      }
      if(buf[nbuf]==10)
      {
         buf[nbuf]=0;
         // return -1 (to ignore 10)
         return max( nbuf-1 , 0 );
      }
      //
      nbuf++;
      // check max length
      if(maxbuf>0 && nbuf>=maxbuf) return nbuf;
   }
}

inline int TStream::Readln(TString &str)
{
   int r=Readln( str.Lock() , str.GetSize() );
   str.Unlock();
   //
   return r;
}

/////////////////////////////////////////////////////////////////////////
//
// TStreamPipe
//
/////////////////////////////////////////////////////////////////////////

class XDLL TStreamPipe : public TStreamBase {
	virtual int       Open( TStream *stream , TStreamMode mode=smRead )=0;
};



#endif

/////////////////////////////////////////////////////////////////////////
//
// 
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//
// String
//
// Copyright by GP
// Created 3.6.2000
//
/////////////////////////////////////////////////////////////////////
//$XHEADER
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>

#include <globals/globals.h>

/////////////////////////////////////////////////////////////////////
//
// TString
//
/////////////////////////////////////////////////////////////////////

TString::TString()
{
   attr=0;
   //
   count=0;
   size=0;
   //
   pString=0;
}

TString::TString(int asize)
{
   attr=0;
   //
   count=0;
   size=0;
   //
   pString=0;
   //
   SetSize( asize );
}

TString::TString(const TString &string)
{
   attr=0;
   //
   count=0;
   size=0;
   //
   pString=0;
   //
   *this=string;
}

TString::TString(const char c,int num)
{
   attr=0;
   if( num < 0 ) num=0;
   //
   count=num;
   size=0;
   //
   pString=0;
   //
   SetSize( num );
   //
   if( num ) memset( pString , c , num );
}

TString::TString(const char *c)
{
   if( !c ) 
   {
      attr=0;
      //
      count=0;
      size=0;
      //
      pString=0;
      //
      SetSize(0);
      //
      return;
   }
   //
   attr=0;
   //
   count=strlen( c );
   size=0;
   //
   pString=0;
   //
   SetSize( count );
   //
   if( count ) strcpy( pString , c );
}

//
TString::~TString()
{
   SetSize( -1 );
}

void TString::Update()
{
   count=strlen( pString );
}

/////////////////////////////////

TString &TString::Add(const TString &string)
{
   if( count + (int)string >= size ) 
      SetSize( count + (int)string );
   //
   strcat( pString , string );
   //
   Update();
   //
   return *this;
}

TString &TString::Insert(int pos,const TString &string)
{
   #ifdef _DEBUG
      if( pos<0 || pos>count ) MAbort("TString::Insert() error:  error: range check error." );
   #endif
   //
   if( pos==0 )
   {
      // insert at start
      char *c=xnewa( char , count + (int)string + 1 );
      strcpy( c , string );
      strcat( c , pString );
      //
      *this=c;
      //
      xdeletea( c );
      //
      return *this;
   }
   //
   if( pos>=count )
   {
      // insert at end
      Add( string );
      //
      return *this;
   }
   // regular insert
   char *c=xnewa( char , count + (int)string + 1 );
   strncpy( c , pString , pos );
   c[pos]=0;
   strcat( c , string );
   strcat( c , &pString[pos] );
   //
   *this=c;
   //
   xdeletea( c );
   //
   return *this;
}


TString &TString::Delete(int pos,int num)
{
   #ifdef _DEBUG
      if( pos<0 || pos>count ) MAbort("TString::Insert() error:  error: range check error." );
   #endif
   //
   if( num==-1 || num>=count-pos) 
   {
      pString[ pos ]=0;
      count=pos;
      //
      return *this;
   }
   // perform delete
   int b=count-num;
   for(int a=pos;a<b;a++) pString[a]=pString[a+num];
   //
   count-=num;
   pString[count]=0;
   //
   return *this;
}

TString &TString::Delete(const TString &string,int pos,int num)
{
   *this=string;
   Delete( pos , num );
   //
   return *this;
}

TString &TString::Copy(int pos,int num)
{
   #ifdef _DEBUG
      if( pos<0 || pos>count ) MAbort("TString::Insert() error:  error: range check error." );
   #endif
   //
   if( num==-1 || pos+num > count )
   {
      num=count-pos;
   }
   //
   if( pos==0 )
   {
      pString[num]=0;
      count=num;
      //
      return *this;
   }
   // perform copy
   for(int a=0,b=pos;a<num;a++,b++) pString[a]=pString[b];
   //
   pString[num]=0;
   count=num;
   //
   return *this;
}

TString &TString::Copy(const TString &string,int pos,int num)
{
   // TODO: optimize
   *this=string;
   Copy( pos , num );
   //
   return *this;
}

TString &TString::LengthLeft(int num,const char c)
{
   int a=num-count;
   if( a<=0 ) return *this;
   TString add( c , a );
   //
   Insert( 0 , add );
   //
   return *this;
}

TString &TString::LengthRight(int num,const char c)
{
   int a=num-count;
   if( a<=0 ) return *this;
   TString add( c , a );
   //
   Add( add );
   //
   return *this;
}

/////////////////////////////////


char *TString::Lock()
{
   if( attr & saLocked ) 
   {
      #ifdef _DEBUG
         MWarning("TString::Lock() warning: string locked." );
      #endif
      return NULL;
   }
   attr |= saLocked;
   //
   return pString;
}

int TString::Unlock()
{
   if( !(attr & saLocked) ) return 1;
   //
   attr &=~ saLocked;
   //
   Update();
   //
   return 0;
}


/////////////////////////////////

//
void TString::TrimLeft(const char c)
{
   // count how many chars on left == c
   int a=0;
   for(a=0;pString[a]==c && a<count;a++);
   //
   Delete( 0 , a );
}

void TString::TrimRight(const char c)
{
   // count how many chars on right == c
   for(int a=count-1;pString[a]==c && a>=0 ;a-- );
   Delete( a+1 );
}


void TString::Trim(const char c)
{
   TrimLeft( c );
   TrimRight( c );
}


//
void TString::Replace(const char cthis,const char cwiththat,int start)
{
   #ifdef _DEBUG
      if( start<0 || start>count ) MAbort("TString::Replace(char) error: range check error." );
   #endif
   for(int a=start;a<count;a++)
      if( pString[a]==cthis )
         pString[a]=cwiththat;
}


void TString::Replace(const TString &sthis,const TString &swiththat,int start)
{
   #ifdef _DEBUG
      if( start<0 || start>count ) MAbort("TString::Replace(TString) error: range check error." );
   #endif
   while(1)
   {
      int pos=Pos( sthis , start );
      if( pos==-1 ) return;
      //
      start=pos+1;
      //
      Delete( pos , sthis );
      Insert( pos , swiththat );
   }
}


//
int TString::Pos(const char c,int start) const
{
   #ifdef _DEBUG
      if( start<0 || start>count ) MAbort("TString::Pos(char) error: range check error." );
   #endif
   for(int a=start;a<count;a++)
      if( pString[a]==c ) return a;
   //
   return -1;
}


int TString::PosReverse(const char c,int start) const
{
   if( start==-1 ) start=count;
   //
   #ifdef _DEBUG
      if( start<0 || start>count ) MAbort("TString::PosReverse(char) error: range check error." );
   #endif
   for(int a=start-1;a>=0;a--)
      if( pString[a]==c ) return a;
   //
   return -1;
}

//
int TString::Pos(const TString &sthis,int start) const
{
   #ifdef _DEBUG
      if( start<0 || start>count ) MAbort("TString::Pos(TString) error: range check error." );
   #endif
   //
   if( start + (int)sthis > count  ) return -1;
   //
   int   skey=0;
   int   tkey=0;
   // create start keys
   for(int a=start,a1=0;a1<sthis;a++,a1++)
   {
      tkey+=pString[a];
      skey+=((const char*)sthis)[a1];
   }
   //
   int b=count-sthis;
   for(int n=start;n<=b;a++,n++)
   {
      if( skey==tkey && memcmp( &pString[n] , (const char*)sthis , sthis )==0 ) return n;
      // create new key
      tkey-=pString[n];
      tkey+=pString[a];
   }
   //
   return -1;
}


int TString::PosReverse(const TString &sthis,int start) const
{
   if( start==-1 ) start=count-1;
   //
   #ifdef _DEBUG
      if( start<0 || start>count ) MAbort("TString::PosReverse(TString) error: range check error." );
   #endif
   //
   if( start + (int)sthis < count  ) return -1;
   //
   int   skey=0;
   int   tkey=0;
   // create start keys
   for(int a=start,a1=(int)sthis-1;a1>=0;a--,a1--)
   {
      tkey+=pString[a];
      skey+=((const char*)sthis)[a1];
   }
   //
   int b=count-sthis;
   for(int n=start;n>=0;a--,n--)
   {
      if( skey==tkey && memcmp( &pString[n+1-sthis] , (const char*)sthis , sthis )==0 ) return n;
      // create new key
      tkey-=pString[n];
      tkey+=pString[a];
   }
   //
   return -1;
}

const int LowerUpperConv   = 'a' - 'A';

TString &TString::MakeUpper()
{
   for(int a=0;a<count;a++)
      if( pString[a]>='a' && pString[a]<='z' )
         pString[a]-=LowerUpperConv;
   return *this;
}

TString &TString::MakeLower()
{
   for(int a=0;a<count;a++)
      if( pString[a]>='A' && pString[a]<='Z' )
         pString[a]+=LowerUpperConv;
   return *this;
}

////////////

TString &TString::Format(const char *f,...)
{
   MRetrievePrintf( f );
   //
   *this=__buf;
   //
   return *this;
}

////////////
//

TString &TString::operator+=(const TString &string)
{
   Add( string );
   //
   return *this;
}

TString TString::operator+(const TString &string)
{
   TString a=pString;
   a.Add( string );
   //
   return a; 
}

TString TString::operator<<(const TString &string)
{
   if( count ) return *this + string;
          else return string;
}

TString TString::operator<<(const char c)
{
   if( count ) return *this + TString(c);
          else return TString(c);
}

TString TString::operator<<(const char *c)
{
   if( count ) return *this + TString(c);
          else return TString(c);
}

TString TString::operator<<(const int i)
{
   char buf[64];
   sprintf( buf , "%d" , i );
   //
   if( count ) return *this + TString( buf );
          else return TString( buf );
}

TString TString::operator<<(const float f)
{
   char buf[64];
   sprintf( buf , "%f" , f );
   //
   if( count ) return *this + TString( buf );
          else return TString( buf );
}

TString &TString::operator=(const TString &string)
{
   SetSize( string );
   count=string;
   //
   if( count )
      strcpy( pString , string );
   //
   return *this;
}

//
int TString::operator==(const TString &string) const
{
   if( (pString==0) && (tochar( string )==0) ) return 1;
   if( (pString==0) || (tochar( string )==0) ) return 0;
   //
   return strcmp( pString , string )==0;
}

int TString::operator*=(const TString &string) const
{
   if( (pString==0) && (tochar( string )==0) ) return 1;
   if( (pString==0) || (tochar( string )==0) ) return 0;
   //
   return stricmp( pString , string )==0;
}

int TString::operator==(const char *c) const
{
   if( (pString==0) && (c==0) ) return 1;
   if( (pString==0) || (c==0) ) return 0;
   //
   return strcmp( pString , c )==0;
}

int TString::operator*=(const char *c) const
{
   if( (pString==0) && (c==0) ) return 1;
   if( (pString==0) || (c==0) ) return 0;
   //
   return strcmp( pString , c )==0;
}

int TString::operator!=(const TString &string) const
{
   if( (pString==0) && (tochar( string )==0) ) return 0;
   if( (pString==0) || (tochar( string )==0) ) return 1;
   //
   return strcmp( pString , string )!=0;
}

int TString::operator!=(const char *c) const
{
   if( (pString==0) && (c==0) ) return 0;
   if( (pString==0) || (c==0) ) return 1;
   //
   return strcmp( pString , c )!=0;
}

int TString::operator>(const TString &string) const
{
   return strcmp( pString , string )>0;
}

int TString::operator>(const char *c) const
{
   return strcmp( pString , c )>0;
}

int TString::operator<(const TString &string) const
{
   return strcmp( pString , string )<0;
}

int TString::operator<(const char *c) const
{
   return strcmp( pString , c )<0;
}

int TString::operator>=(const TString &string) const
{
   return strcmp( pString , string )>=0;
}

int TString::operator>=(const char *c) const
{
   return strcmp( pString , c )>=0;
}

int TString::operator<=(const TString &string) const
{
   return strcmp( pString , string )<=0;
}

int TString::operator<=(const char *c) const
{
   return strcmp( pString , c )<=0;
}

///////////////////////////////////////////////////////////
// Stream 
///////////////////////////////////////////////////////////

int TString::SaveToStream(TStream &s)
{
   return -1;
}

int TString::LoadFromStream(TStream &s)
{
   return -1;
}

///////////////////////////////////////////////////////////
// Conversion
///////////////////////////////////////////////////////////

int TString::toInt()
{
   return atoi( pString );
}

float TString::toFloat()
{
   return (float)atof( pString );
}

char *TString::toChar(char *c)
{
   strcpy( c , pString );
   return c;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////


TString formatbuffer;

const char XDLL *Format(const char *f,...)
{
   MRetrievePrintf( f );
   //
   formatbuffer=__buf;
   //
   return formatbuffer;
}

/////////////////////////////////////////////////////////////////////
//
// 
//
/////////////////////////////////////////////////////////////////////


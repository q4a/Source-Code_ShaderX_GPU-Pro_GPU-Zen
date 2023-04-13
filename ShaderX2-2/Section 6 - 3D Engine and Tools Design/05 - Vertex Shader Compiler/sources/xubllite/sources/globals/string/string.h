/////////////////////////////////////////////////////////////////////
//
// String
//
// Copyright by GP
// Created 3.6.2000
//
/////////////////////////////////////////////////////////////////////
#ifndef __STRING__
#define __STRING__

const int                  saLocked       = 1;


#define tochar(a) ((const char*)a)


/////////////////////////////////////////////////////////////////////
//
// TString
//
/////////////////////////////////////////////////////////////////////

class XDLL TString {
private:
   //!Pointer to string data.
   char                    *pString;
   //!Main string attributes.
   char                    attr;
   //!Length of string (number of characters).
   int                     count;
   //!Storage capacity of string.
   int                     size;
   //!Update string.
   void                    Update();
public:
   //!Basic constructor.
                           TString();
   //!Storage size is set to <asize>.
                           TString(int asize);
   //!Create copy of <string> (default copy constructor).
                           TString(const TString &string);
   //!Create string with <num> chars <c>.
                           TString(const char c,int num=1);
   //!Create string from char* <c>.
                           TString(const char *c);
                           //
                           ~TString();
   //
public:
   //!Set size of string (if <force> is zero and <asize> is less that <size>, <pString> is NOT resized!)
   void                    SetSize(int asize,int force=0)
   {
      if( asize<0 ) 
      {
         xdeletea( pString );
      }
      else 
      {
         if( pString==0 || size==0 || asize > size || force )
         {
            pString=(char*)xresize( pString , size+1 , asize+1 );
            size=asize;
         }
         //
         pString[ min( asize , count ) ]=0;
      }
   }
   //!Returns length of string.
   int                     Length() const {return count;}
   //!Returns storage size of string.
   int                     GetSize() const {return size;}
   //!Returns 1 if string is empty
   int                     IsEmpty() const {return count==0;}
   //!Clears string.
   void                    Clear()
   {
      count=0;
      if( pString ) pString[0]=0;
   }

   //
   //!Add <string> to the end of string.
   TString                 &Add(const TString &string);
   //!Insert <string> at <pos>.
   TString                 &Insert(int pos,const TString &string);
   //
   //!Delete <num> chars from <pos> (if <num> is not specified, all chars from <pos> are deleted).
   TString                 &Delete(int pos,int num=-1);
   //!Copy <string> and performs delete.
   TString                 &Delete(const TString &string,int pos,int num=-1);
   //!Copy <num> chars from <pos> (if <num> is not specified, all chars from <pos> are copied).
   TString                 &Copy(int pos,int num=-1);
   //!Copy <string> and performs copy.
   TString                 &Copy(const TString &string,int pos,int num=-1);
   //!Copy <num> chars from left.
   TString                 &Left(int num){Copy( 0 , num );return *this;}
   //!Copy <num> chars from right.
   TString                 &Right(int num){Copy( count-num );return *this;}
   //!Copy <num> chars from left from string <s>.
   TString                 &Left(const TString &s,int num){Copy( s , 0 , num );return *this;}
   //!Copy <num> chars from right from string <s>.
   TString                 &Right(const TString &s,int num){Copy( s , int(s)-num );return *this;}
   //!Add as many chars <c> on left as needed to get length of string <num>.
   TString                 &LengthLeft(int num,const char c=' ');
   //!Add as many chars <c> on right as needed to get length of string <num>.
   TString                 &LengthRight(int num,const char c=' ');
   //
   //!Locks string and returns pointer to <pString>.
   char                    *Lock();
   //!Unlocks string and performs Update().
   int                     Unlock();
   //
   //!Trim all chars <c> from left.
   void                    TrimLeft(const char c=' ');
   //!Trim all chars <c> from right.
   void                    TrimRight(const char c=' ');
   //!Trim all startinf and ending chars <c>.
   void                    Trim(const char c=' ');
   //
   //!Replace char <cthis> with <cwiththat> starting from <start> on.
   void                    Replace(const char cthis,const char cwiththat,int start=0);
   //!Replace string <sthis> with <swiththat> starting from <start> on.
   void                    Replace(const TString &sthis,const TString &swiththat,int start=0);
   //
   //!Returns position of char <c> in string from right, starting from <start>. Returns -1 if no char <c> in string.
   int                     Pos(const char c,int start=0) const;
   //!Returns position of char <c> in string from left, starting from <start>. Returns -1 if no char <c> in string.
   int                     PosReverse(const char c,int start=-1) const;
   //
   //!Returns position of string <string> in string from right, starting from <start>. Returns -1 if no <string> in string.
   int                     Pos(const TString &string,int start=0) const;
   //!Returns position of string <string> in string from left, starting from <start>. Returns -1 if no <string> in string.
   int                     PosReverse(const TString &string,int start=-1) const;

   // upper/lower conversion

   //!Make all upper case.
   TString                 &MakeUpper();
   //!Make all lower case.
   TString                 &MakeLower();


   ////////////
   //!Return single character at <a> in zero-based index.
   char                    &operator[](int a) 
   {
      #ifdef _DEBUG
         if( a<0 || a>count ) MAbort("TString::operator[] error: range check error." );
      #endif
      return pString[a];
   }
   //!Return single const character at <a> in zero-based index.
   const char              operator()(int a) const 
   {
      #ifdef _DEBUG
         if( a<0 || a>count ) MAbort("TString::operator() error: range check error." );
      #endif
      return pString[a];
   }
   ////////////

   //!Formats string like printf.
   TString                 &Format(const char *f,...);

   ////////////

   //!Operator to enable TString act like int.
                           operator int() const {return count;}
   //!Operator to enable TString act like const char*.
                           operator const char*() const {return pString;}

   //!Creating string from string.
   TString                 &operator=(const TString &string);

   //!Adding string to string.
   TString                 &operator+=(const TString &string);
   //!Adding string to string.
   TString                 operator+(const TString &string);

   //!String case sensitive compare operator ==.
   int                     operator==(const TString &string) const;
   //!String case insensitive compare operator ==.
   int                     operator*=(const TString &string) const;
   //!String case sensitive compare operator ==.
   int                     operator==(const char *c) const;
   //!String case insensitive compare operator ==.
   int                     operator*=(const char *c) const;
   //!String case sensitive compare operator !=.
   int                     operator!=(const TString &string) const;
   //!String case sensitive compare operator !=.
   int                     operator!=(const char *c) const;
   //
   //!String case sensitive compare operator >.
   int                     operator>(const TString &string) const;
   //!String case sensitive compare operator >.
   int                     operator>(const char *c) const;
   //!String case sensitive compare operator <.
   int                     operator<(const TString &string) const;
   //!String case sensitive compare operator <.
   int                     operator<(const char *c) const;
   //!String case sensitive compare operator >=.
   int                     operator>=(const TString &string) const;
   //!String case sensitive compare operator >=.
   int                     operator>=(const char *c) const;
   //!String case sensitive compare operator <=.
   int                     operator<=(const TString &string) const;
   //!String case sensitive compare operator <=.
   int                     operator<=(const char *c) const;

   //!String adding operators for string.
   TString                 operator<<(const TString &string);
   //!String adding operators for char.
   TString                 operator<<(const char c);
   //!String adding operators for char *.
   TString                 operator<<(const char *c);
   //!String adding operators for int.
   TString                 operator<<(const int i);
   //!String adding operators for float.
   TString                 operator<<(const float f);
   //
   //!String creation operators from string.
   TString                 &operator<<=(const TString &string)
   {
      *this=*this << string;
      return *this;
   }
   //!String creation operators from char.
   TString                 &operator<<=(const char c)
   {
      *this=*this << c;
      return *this;
   }
   //!String creation operators from char*.
   TString                 &operator<<=(const char *c)
   {
      *this=*this << c;
      return *this;
   }
   //!String creation operators from int.
   TString                 &operator<<=(const int i)
   {
      *this=*this << i;
      return *this;
   }
   //!String creation operators from float.
   TString                 &operator<<=(const float f)
   {
      *this=*this << f;
      return *this;
   }
public:
   // stream
   int                     SaveToStream(TStream &s);
   int                     LoadFromStream(TStream &s);
public:
   // conversion
   int                     toInt();
   float                   toFloat();
   char                    *toChar(char *c);
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

inline int operator==(const char *c,const TString &string)
{
   return strcmp( string , c )==0;
}

inline int operator*=(const char *c,const TString &string)
{
   return stricmp( string , c )==0;
}

inline int operator>(const char *c,const TString &string)
{
   return strcmp( string , c )>0;
}

inline int operator<(const char *c,const TString &string)
{
   return strcmp( string , c )<0;
}

inline int operator>=(const char *c,const TString &string)
{
   return strcmp( string , c )>=0;
}

inline int operator<=(const char *c,const TString &string)
{
   return strcmp( string , c )<=0;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

inline TString operator<<(const char *c,const TString &s)
{
   return TString(c) << s;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

inline TString toString(int i,int radix=10 )
{
   TString s(32);
   //
   itoa( i , s.Lock() , radix );
   s.Unlock();
   //
   return s;
}

// use negative value to get min dec
// example:
//    toString( 3.12    , 3 )   "3.120"
//    toString( 3.12    ,-3 )   "3.12"
//    toString( 3       , 3 )   "3.000"
//    toString( 3       ,-3 )   "3"
inline TString toString(float f,int dec=3)
{
   TString s;
   //
   int id,is;
   s=fcvt( f , 128 , &id , &is );
   int n=id+abs( dec );
   if( n<0 || n>=s.Length() ) return "";
   //
   while( id<0 ) 
   {
      s="0" << s;
      id++;
   }
   //
   s[n]=0;
   s.Insert( id , "." );
   // starting zero
   if( id==0 ) s=TString("0") << s;
   //
   if( is ) s=TString("-") << s;
   if( dec < 0 )
   {
      s.TrimRight('0');
      s.TrimRight('.');
   }
   //
   return s;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

inline TString Left(const TString &s,int n)
{
   TString i(s);
   i.Left( n );
   return i;
}

inline TString Right(const TString &s,int n)
{
   TString i(s);
   i.Right( n );
   return i;
}

inline TString Copy(const TString &s,int pos,int num=-1)
{
   TString i(s);
   i.Copy( pos , num );
   return i;
}

inline TString Delete(const TString &s,int pos,int num=-1)
{
   TString i(s);
   i.Delete( pos , num );
   return i;
}

inline TString Insert(const TString &s,int pos,const TString &ins)
{
   TString i(s);
   i.Insert( pos , ins );
   return i;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//!Formats string like printf.
const char XDLL *Format(const char *f,...);

#endif
///////////////////////////////////////////////////////////////////////////
//
// Copyright GP
// Created 3.6.2000
//
// Set
//
///////////////////////////////////////////////////////////////////////////

#ifndef __TSET__
#define __TSET__

#include "tarray.h"

template <class T> class TSet {
protected:
   //!Number of objects in set.
   int                     count;
   //!set capacity.
   int                     size;
   //!set storage.
   T                       *pArray;
protected:
   //!Resize set from size <size> to <newsize>. Just for internal use.
   void                    _Resize(int newsize)
   {
      pArray=(T*)xresize( pArray , size*sizeof( T ) ,  newsize*sizeof( T ) );
      size=newsize;
   }
   //
   //!Insert object <data> in set at position <a>. Just for internal use.
   void                    _Insert(int a,const T &data);
   //!Add object <data> to end of set. Just for internal use.
   void                    _Add(const T &data);
   //!Set number of objects in set. If a is greater than size, set will be resized. Just for internal use.
   void                    _SetCount(int a);
public:
                           TSet()
                           {
                              count=0;
                              size=0;
                              pArray=0;
                           }
                           TSet(const TSet &data)
                           {
                              count=0;
                              size=0;
                              pArray=0;
                              //
                              *this=data;
                           } 
                           TSet(const TArray<T> &data)
                           {
                              count=0;
                              size=0;
                              pArray=0;
                              *this=data;
                           }
                           ~TSet()
                           {
                              Clear();
                           }
public:
   ////////////////////////////
   // size/count control
   ////////////////////////////
   //!Retrieve number of objects in set.
   int                     Count() const { return count; }
   //!Retrieve set storage capacity.
   int                     Size() const { return size; }
   //!Set set storage capacity. If a is less than count, objects a..count-1 will be deleted.
   void                    SetSize(int a);
   //!Set count (and size if resize!=0) to zero.
   void                    Clear(int resize=1);

   ////////////////////////////
   // adding
   ////////////////////////////
   //!Add object into set. If object already exists do nothing. Returns 0 if object added 1 otherwise.
   int                     Add(const T &data);
   //!Add set of objects <data> into set.
   void                    Add(const TSet &data);
   //!Add array of objects <data> into set.
   void                    Add(const TArray &data);

   ////////////////////////////
   // deleting
   ////////////////////////////
   //!Remove object <data>. Returns 0 if object deleted, 1 if not found.
   int                     Remove(const T &data);

   ////////////////////////////
   // search
   ////////////////////////////
   //!Find object index (returns -1 if search failed).
   int                     Find(const T &data) const;  
   //!Find object index or -index-10 to first bigger object or -1 of no bigger object in set.
   int                     FindNear(const T &data) const;  

   ////////////////////////////
   // operators
   ////////////////////////////
   //!operator[] returns <a>th object in set.
   T                       &operator[](const int a);
   //!operator() returns <a>th object in set. Be carefull usage is readonly!
   const T                 operator()(const int a) const;
   //!operator int() enables set to act like integer. Example: TSet<int> sA;fill it up;for(int a=0;a<sA;a++)...
   operator                int() const
   {
      return count;
   }

   TSet                    &operator=(const TArray<T> &data)
   {
      _SetCount( 0 );
      //
      SetSize( data );
      //
      for(int a=0;a<data;a++)
         Add( data(a) );
      //
      return *this;
   }
   TSet                    &operator=(const TSet &data)
   {
      SetSize( data );
      //
      _SetCount( 0 );
      Add( data );
      //
      return *this;
   }


   ////////////////////////////
   // load/save
   ////////////////////////////
   //int                     const SaveToStream(TStream &s);
   //int                     LoadFromStream(TStream &s);
};


//////////////////////////////////////////
// Size/count control
//////////////////////////////////////////
template<class T>void TSet<T>::_SetCount(int a)
{
   if( a==count ) return;
   //
   if( a < count ) 
   {
      // destruct some
      for(int d=a;d<count;d++)
         destruct( pArray[d] );
      count=a;
   }
   else
   {
      // check if size ok
      if( size < a ) SetSize( a );
      // create some
      for(int n=count;n<a;n++)
         construct( pArray[n] );
      count=a;
   }
}

template<class T>void TSet<T>::SetSize(int a)
{
   if( a<0 ) a=0;
   //
   _Resize( a );
}

template<class T>void TSet<T>::Clear(int resize)
{
   _SetCount( 0 );
   if( resize ) _Resize( 0 );
}

//////////////////////////////////////////
// adding
//////////////////////////////////////////
template<class T>void TSet<T>::_Insert(int a,const T &data)
{
   //
   if( count==0 && a==0 )
   {
      _Add( data );
      //
      return;
   }
   //
   #ifdef SET_RANGECHECK 
      if( a<0 || a>=count ) MAbort( "TSet::Insert() error: range check error." );
   #endif
   //
   _SetCount( count+1 );
   //
   for(int i=count-1;i>a;i--)
      pArray[i]=pArray[i-1];
   //
   pArray[a]=data;
}

template<class T>void TSet<T>::_Add(const T &data)
{
   if( count>=size ) SetSize( (size*2)+1 );
   construct( pArray[ count ] );
   pArray[ count++ ]=data;
   //
}

template<class T>int TSet<T>::Add(const T &data)
{
   if( count==0 ) 
   {
      _Add( data );
      return 0;
   }
   int a=FindNear( data );
   if( a>=0 ) return 1;
   //
   if( a==-1 ) _Add( data );
          else _Insert( -a-10 , data );
   //
   return 0;
}

template<class T>void TSet<T>::Add(const TSet<T> &data)
{
   if( count+data.Count()>=size ) SetSize( count+data.Count()+1 );
   //
   for(int a=0;a<data;a++) Add( data(a) );
}

template<class T>void TSet<T>::Add(const TArray<T> &data)
{
   if( count+data.Count()>=size ) SetSize( count+data.Count()+1 );
   //
   for(int a=0;a<data;a++) Add( data(a) );
}

//////////////////////////////////////////
// deleting
//////////////////////////////////////////
template<class T>int TSet<T>::Remove(const T &data)
{
   int a=Find( data );
   if( a==-1 ) return 1;
   //
   for(int i=a;i<count-1;i++) 
      pArray[i]=pArray[i+1];
   //
   count--;
   //
   destruct( pArray[ count ] );
   //
   return 0;
}


//////////////////////////////////////////
// search
//////////////////////////////////////////
template<class T>int TSet<T>::Find(const T &data) const
{
   if( count==0 ) return -1;
   // bisection
   float bia     =0;
   float bib     =(float)count;
   float bipos   =bib / 2;
   //
   while( (bib-bia)>0.5f )
   {
      int i=(int)bipos;
      if( data == pArray[i] ) return i;
      // bisection
      if( data < pArray[i] ) bib=bipos;
                        else bia=bipos;
      //
      bipos=(bia+bib) / 2;
   }
   //
   return -1;
}

/*
template<class T>int TSet<T>::FindNear(const T &data) const
{
   if( count==0 ) return -1;

   // bisection
   int bisize=count;
   int bipos =bisize >> 1;
   //
   while(1)
   {
      if( data == pArray[bipos] ) return bipos;
      // near check
      int less=data < pArray[bipos];
      if( less && pArray[bipos-1]  <  data ) return -10-bipos;
      //
      if( bisize<=0 ) return -1;
      //
      bisize>>=1;
      if( less ) bipos-=MAX( bisize >> 1 , 1 );
            else bipos+=MAX( bisize >> 1 , 1 );
      //
      if( bipos < 0     ) return -1;
      if( bipos >=count ) return -1;
   }
}
/**/
template<class T>int TSet<T>::FindNear(const T &data) const
{
   if( count==0 ) return -1;
   // bisection
   float bia     =0;
   float bib     =(float)count;
   float bipos   =bib / 2;
   //
   while( (bib-bia)>0.5f )
   {
      int i=(int)bipos;
      if( data == pArray[i] ) return i;
      // near check
      int less=data < pArray[i];
      if( i==0 )
      {
         if( less )
         {
            return -10;
         }
         else
         {
            return -1;
         }
      }
      else
      {
         if( less && pArray[i-1] < data ) return -10-i;
      }
      // bisection
      if( less ) bib=bipos;
            else bia=bipos;
      //
      bipos=(bia+bib) / 2;
   }
   //
   return -1;
}
/**/

//////////////////////////////////////////
// operators
//////////////////////////////////////////

template<class T>T &TSet<T>::operator[](const int a)
{
   #ifdef ARRAY_RANGECHECK 
      if( a<0 || a>=count ) MAbort( "TSet::operator[] error: range check error." );
   #endif
   return pArray[a];
}

template<class T>const T TSet<T>::operator()(const int a) const
{
   #ifdef ARRAY_RANGECHECK 
      if( a<0 || a>=count ) MAbort( "TSet::operator() error: range check error." );
   #endif
   return pArray[a];
}

#endif
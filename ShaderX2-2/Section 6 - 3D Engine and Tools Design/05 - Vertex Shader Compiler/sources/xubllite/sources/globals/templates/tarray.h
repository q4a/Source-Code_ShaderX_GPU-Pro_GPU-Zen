///////////////////////////////////////////////////////////////////////////
//
// Copyright GP
// Created 3.6.2000
//
// Dynamic array template definition 
//
///////////////////////////////////////////////////////////////////////////

#ifndef __TARRAY__
#define __TARRAY__

#include <globals/macros.h>
#include "tquicksort.h"

class TStream;

/////////////////////////////////////////////////////////////
//
// TArray
//
// doesn't call constructors or destructors!
//
/////////////////////////////////////////////////////////////

template <class T> class TArray {
protected:
   //!Number of objects in array.
   int                     count;
   //!Array capacity.
   int                     size;
   //!Array storage.
   T                       *pArray;
protected:
   //!Resize array from size <size> to <newsize>.
   void                    Resize(int newsize)
   {
      pArray=(T*)xresize( pArray , size * sizeof( T ) ,  newsize * sizeof( T ) );
      size=newsize;
      count=min( count , size );
   }
   //
public:
                           TArray()
                           {
                              count=0;
                              size=0;
                              pArray=0;
                           }
                           TArray(int acount)
                           {
                              count=0;
                              size=0;
                              pArray=0;
                              //
                              SetCount( acount );
                           }
                           TArray(const TArray &data)
                           {
                              count=0;
                              size=0;
                              pArray=0;
                              //
                              SetSize( data );
                              Add( data );
                           }
                           ~TArray()
                           {
                              Clear(1);
                           }
public:
   ////////////////////////////
   // size/count control
   ////////////////////////////
   //!Retrieve number of objects.
   int                     Count() const { return count; }
   //!Retrieve array storage capacity.
   int                     Size() const { return size; }
   //!Set number of objects in array. If a is greater than size, array will be resized.
   void                    SetCount(int a);
   //!Set array storage capacity. If a is less than count, objects a..count-1 will be deleted.
   void                    SetSize(int a);
   //!Set count (and size if resize!=0) to zero.
   void                    Clear(int resize=1);

   ////////////////////////////
   // adding
   ////////////////////////////
   //!Add object in parameter to end of array (same as Push).
   T                       &Add(const T &data=T());
   //!Add unique object in parameter to end of array (same as Push).
   T                       &AddUnique(const T &data=T());
   //!Add array of objects to end of array.
   void                    Add(const TArray &data);
   //!Add unique array of objects to end of array.
   void                    AddUnique(const TArray &data);
   //!Insert object in parameter in array at position <a>.
   T                       &Insert(int a,const T &data=T());
   //!Insert array of objects in array at position <a>.
   void                    Insert(int a,const TArray &data);
   //!Pushes object into array.
   void                    Push(const T &data=T());
   //!Pushes object into array as first object.
   void                    PushFirst(const T &data=T());

   ////////////////////////////
   // deleting
   ////////////////////////////
   //!Remove object at index <a> with preserving object order. If deleteobject is 0 object destructor is not called and caller must handle it.
   void                    Remove(int a,int deleteobject=1);
   //!Remove object at <data> with preserving object order. If deleteobject is 0 object destructor is not called and caller must handle it.
   void                    RemoveFind(const T &data,int deleteobject=1);
   //!Remove object at index <a> without preserving object order (replaces it with last object). If deleteobject is 0 object destructor is not called and caller must handle it.
   void                    RemoveFast(int a,int deleteobject=1);
   //!Remove object <data> without preserving object order (replaces it with last object). If deleteobject is 0 object destructor is not called and caller must handle it.
   void                    RemoveFastFind(const T &data,int deleteobject=1);
   //!Pops last object.
   T                       &Pop(T &buffer);
   //!Pops first object.
   T                       &PopFirst(T &buffer);
   //!Pops last object.
   void                    Pop();
   //!Pops first object.
   void                    PopFirst();

   ////////////////////////////
   // search
   ////////////////////////////
   //!Find object index starting at start (returns -1 if search failed). Requires implementation of operator ==.
   int                     Find(const T &data,int start=0) const;  
   //!Find object index by address starting at start (returns -1 if search failed).
   int                     FindAddr(const T *data,int start=0) const;  


   ////////////////////////////
   // operators
   ////////////////////////////
   //!Array access operator.
   T                       &operator[](const int a);
   //!Readonly array access operator.
   const T                 &operator()(const int a) const;
   //!Array copy constructor
   /**/
   TArray                  &operator=(const TArray &data)
                           {
                              Clear(0);
                              Add( data );
                              //
                              return *this;
                           }
   /*
                           operator=(const TArray &data)
                           {
                              Clear(0);
                              Add( data );
                           }
   /**/

   //!operator int() enables array to act like integer. Example: TArray<int> aA;fill it up;for(int a=0;a<aA;a++)...
   operator                int() const
   {
      return count;
   }

	void SaveToStream( TStream &s )
	{
	   s.Write( &count, sizeof(count) );
	   s.Write( pArray, sizeof(T) * count );
	}

	int LoadFromStream( TStream &s )
	{
      int fcount = 0;
      s.Read( &fcount, sizeof(fcount) );
      SetSize( fcount );
      //
      s.Read( pArray, sizeof(T) * fcount );
      count=fcount;
      return 0;
	}
   //
	void SaveToStreamStream( TStream &s )
	{
	   s.Write( &count, sizeof(count) );
      for(int a=0;a<count;a++)
         pArray[a].SaveToStream( s );
	}

	int LoadFromStreamStream( TStream &s )
	{
      Clear();
      //
      int fcount = 0;
      s.Read( &fcount, sizeof(fcount) );
      SetCount( fcount );
      //
      for(int a=0;a<fcount;a++)
         pArray[a].LoadFromStream( s );
      count=fcount;
      return 0;
	}
   //
	void SaveToStreamStreamp( TStream &s )
	{
	   s.Write( &count, sizeof(count) );
      for(int a=0;a<count;a++)
         pArray[a]->SaveToStream( s );
	}

	int LoadFromStreamStreamp( TStream &s )
	{
      Clear();
      //
      int fcount = 0;
      s.Read( &fcount, sizeof(fcount) );
      SetCount( fcount );
      //
      for(int a=0;a<fcount;a++)
         pArray[a]->LoadFromStream( s );
      count=fcount;
      return 0;
	}
   //
   ////////////////////////////
   // sort
   ////////////////////////////
   //!Incremental sort of array. Requires implementation of operator <.
   void SortInc()  
   { 
      if(count>1) TQuickSortInc( pArray , count ); 
   }

   //!Decremental sort of array. Requires implementation of operator <.
   void SortDec()  
   { 
      if(count>1) TQuickSortDec( pArray , count ); 
   }
};


//////////////////////////////////////////
// Size/count control
//////////////////////////////////////////
template<class T>void TArray<T>::SetCount(int a)
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

template<class T>void TArray<T>::SetSize(int a)
{
   if( a<0 ) a=0;
   //
   Resize( a );
}

template<class T>void TArray<T>::Clear(int resize)
{
   SetCount( 0 );
   if( resize ) Resize( 0 );
   count=0;
}

//////////////////////////////////////////
// adding
//////////////////////////////////////////
template<class T>T &TArray<T>::Add(const T &data)
{
   if( count>=size ) SetSize( (size*2)+1 );
   //
   construct( pArray[ count ] );
   //
   int c=count++;
   pArray[ c ]=data;
   return pArray[ c ];
   // MSVC error - this will cause error with word size ????
   //return (pArray[ count++ ]=data);
}

template<class T>T &TArray<T>::AddUnique(const T &data)
{
   int a=Find( data );
   //
   if( a>=0 ) return pArray[a];
   //
   return Add( data );
}

template<class T>void TArray<T>::Add(const TArray<T> &data)
{
   if( count+data.Count()>=size ) SetSize( count+data.Count() );
   //
   for(int a=0,c=count;a<data;a++,c++)
   {
      construct( pArray[c] );
      //
      pArray[c]=data(a);
   }
   //
   count+=data;
}

template<class T>void TArray<T>::AddUnique(const TArray<T> &data)
{
   if( count+data.Count()>=size ) SetSize( count + data.Count() );
   //
   for(int a=0;a<data;a++) 
      AddUnique( data(a) );
}

template<class T>T &TArray<T>::Insert(int a,const T &data)
{
   #ifdef _DEBUG
      if( a<0 ) MAbort( "TArray::RemoveFast() error: range check error." );
   #endif
   if( a >= count ) return Add( data );
   //
   SetCount( count+1 );
   //
   for(int i=count-1;i>a;i--)
      pArray[i]=pArray[i-1];
   //
   pArray[a]=data;
   return pArray[a];
}

template<class T>void TArray<T>::Insert(int a,const TArray<T> &data)
{
   #ifdef _DEBUG
      if( a<0 ) MAbort( "TArray::Insert(int,TArray) error: range check error." );
   #endif
   if( a >= count ) 
   {
      Add( data );
      return;
   }
   //
   SetCount( count+data );
   // create gap
   for(int i=count-1;i>a+data-1;i--)
      pArray[i]=pArray[i-data];
   // fill new data into gap
   for(int x=a,y=0;y<data;x++,y++)
      pArray[x]=data(y);
}

template<class T>void TArray<T>::Push(const T &data)
{
   Add( data );
}

template<class T>void TArray<T>::PushFirst(const T &data)
{
   Insert( 0 , data );
}

//////////////////////////////////////////
// deleting
//////////////////////////////////////////
template<class T>void TArray<T>::Remove(int a,int deleteobject)
{
   #ifdef _DEBUG
      if( a<0 || a>=count ) MAbort( "TArray::RemoveFast() error: range check error." );
   #endif
   //
   if( deleteobject ) destruct( pArray[a] );
   //
   for(int i=a;i<count-1;i++)
      pArray[i]=pArray[i+1];
   //
   count--;
}

template<class T>void TArray<T>::RemoveFind(const T &data,int deleteobject)
{
   int a=Find( data );
   if( a>=0 ) Remove( a );
}

template<class T>void TArray<T>::RemoveFast(int a,int deleteobject)
{
   #ifdef _DEBUG
      if( a<0 || a>=count ) MAbort( "TArray::RemoveFast() error: range check error." );
   #endif
   //
   if( deleteobject ) destruct( pArray[a] );
   //
   count--;
   if( a<count ) pArray[a]=pArray[ count ];
}

template<class T>void TArray<T>::RemoveFastFind(const T &data,int deleteobject)
{
   int a=Find( data );
   if( a>=0 ) RemoveFast( a );
}

// destructor for this object must be called manualy
template<class T>T &TArray<T>::Pop(T &buffer)
{
   if( !count ) return buffer;
   //
   buffer=pArray[ count-1 ];
   Remove( count-1 );
   //
   return buffer;
}

// destructor for this object must be called manualy
template<class T>T &TArray<T>::PopFirst(T &buffer)
{
   if( !count ) return buffer;
   //
   buffer=pArray[ 0 ];
   Remove( 0 );
   //
   return buffer;
}

// destructor for this object must be called manualy
template<class T>void TArray<T>::Pop()
{
   if( !count ) return;
   //
   Remove( count-1 );
}

// destructor for this object must be called manualy
template<class T>void TArray<T>::PopFirst()
{
   if( !count ) return;
   //
   Remove( 0 );
}

//////////////////////////////////////////
// search
//////////////////////////////////////////
template<class T>int TArray<T>::Find(const T &data,int start) const
{
   #ifdef _DEBUG
      if( start<0 ) MAbort( "TArray::Find error: range check error." );
   #endif
   //
   for(int a=start;a<count;a++)
      if( pArray[a]==data ) return a;
   return -1;
}

template<class T>int TArray<T>::FindAddr(const T *data,int start) const
{
   #ifdef _DEBUG
      if( start<0 ) MAbort( "TArray::FindAddr error: range check error." );
   #endif
   //
   for(int a=start;a<count;a++)
      if( (int)&pArray[a]==(int)data ) return a;
   return -1;
}

//////////////////////////////////////////
// sort
//////////////////////////////////////////

template<class T>T &TArray<T>::operator[](const int a)
{
   #ifdef _DEBUG
       if( a<0 || a>=count ) MAbort( "TArray::operator[] error: range check error." );
   #endif
   return pArray[a];
}

template<class T>const T &TArray<T>::operator()(const int a) const
{
   #ifdef _DEBUG
      if( a<0 || a>=count ) MAbort( "TArray::operator() error: range check error." );
   #endif
   return pArray[a];
}



/////////////////////////////////////////////////////////////
//
// TpArray
//
// creates / destroys objects it points to
//
/////////////////////////////////////////////////////////////

template<class T>
class TpArray  {
private:
   TpArray                 &operator=( const TpArray &data ){}
   TArray<T *> array;
public:
                           TpArray(const TpArray<T> &src)
                           {
                              array=src.array;
                           }
                           //


                           TpArray(): array() {}
                           TpArray(int nmax): array(nmax) {}
                           //
                           ~TpArray()        { Clear(); }
public:
   /*
	TpArray                 &operator=( const TpArray &data )
	{
      //array=src.array;
      array.SetCount(0);
      array.SetSize( data );
      //
      for(int a=0;a<data;a++) Add( data(a) );
      //
		return *this;
	}
   /**/
   // insert copy of object
   T                       &Add(const T& data )
   {
      T *n=xnew( T );
      *n=data;
      return *array.Add( n );
   }
   // insert copy of object
   T                       &Add()
   {
      T *data = xnew( T );
      return *array.Add( data );
   }
   void Insert( int slotn , const T &data )
   {
      #ifdef _DEBUG
          if(a<0 || a>=count) MAbort("TpArray::Insert out of range");
      #endif
      array.Insert(slotn, xnewp( T , ( data ) ));
   }

   // insert instance of object
   T *Add(T* data)
   {
      return array.Add( instance(data) );
   }
   void Insert( int slotn , T *data )
   {
      //array.Insert(slotn, data );
      array.Insert(slotn, instance(data) );
   }

   void Clear( int deleteobjects=1 )
   {
      if( deleteobjects )
      {
         for ( int i = 0; i < array.Count(); i++ ) 
            if(array[i]) xdelete( array[i] );
      }
      array.Clear();
   }
   void Remove(int slotn, int deleteobjects=1 )
   {
      #ifdef _DEBUG
          if(slotn<0 || slotn>=array.Count()) MAbort("TpArray::Remove out of range");
      #endif
      if(deleteobjects) xdelete( array[slotn] );
      array.Remove(slotn);
   }
   void RemoveFast(int slotn, int deleteobjects=1 )
   {
      #ifdef _DEBUG
          if(slotn<0 || slotn>=array.Count()) MAbort("TpArray::RemoveFast out of range");
      #endif
      if(deleteobjects) xdelete( array[slotn] );
      array.RemoveFast(slotn);
   }
   //
   int RemoveFind(T *obj, int deleteobjects=1 )
   {
      //
      int a=FindAddr( obj );
      if(a!=-1) Remove( a , deleteobjects );
      return a;
   }
   int RemoveFastFind(T *obj, int deleteobjects=1 )
   {
      //
      int a=FindAddr( obj );
      if(a!=-1) RemoveFast( a , deleteobjects );
      return a;
   }
	T& operator [](int a)
	{
      #ifdef _DEBUG
          if(a<0 || a>=array.Count() ) MAbort("TpArray::operator [] out of range");
      #endif
		return *array[a];
	}
	const T   &operator ()(int a) const 
	{
      #ifdef _DEBUG
          if(a<0 || a>=array.Count() ) MAbort("TpArray::operator [] out of range");
      #endif
		return *array[a];
	}
   void SetCount( int ncount )
   {
      //
      array.SetSize( ncount );
      Clear();
      //
      for(int a=0;a<ncount;a++) Add();
   }
   //
   int Count()   { return array.Count(); }

   //!operator int() enables array to act like integer. Example: TArray<int> aA;fill it up;for(int a=0;a<aA;a++)...
   operator                int() const
   {
      return array.Count();
   }
   //
   int FindAddr(T *f)
   {
      for(int a=0;a<array.Count();a++)
         if(array[a]==f)
            return a;
      //
      return -1;
   }
	void SaveToStream( TStream &s )
	{
      int a=array.Count();
	   s.Write( &a , sizeof(a) );        
      //
      for(int i=0;i<a;i++)
	      array[i]->SaveToStream( s );
	}

	int LoadFromStream( TStream &s )
	{
      int fcount = 0;
      s.Read( &fcount, sizeof(fcount) );
      //SetCount( fcount );
      if( array.Count() < fcount ) return 1;
      //
      for(int i=0;i<fcount;i++)
	      array[i]->LoadFromStream( s );
      //
      return 0;
	}
};


#endif 

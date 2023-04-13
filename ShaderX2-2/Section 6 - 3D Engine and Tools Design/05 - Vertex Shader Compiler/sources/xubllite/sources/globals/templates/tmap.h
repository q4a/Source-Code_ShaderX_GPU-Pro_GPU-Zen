///////////////////////////////////////////////////////////////////////////
//
// Copyright GP
// Created 3.6.2000
//
// Mapping
//
///////////////////////////////////////////////////////////////////////////

#ifndef __TMAP__
#define __TMAP__

#include "tset.h"

template <class K,class R> class TMapKey {
public:
   K                       Key;
   R                       Result;
public:
                           TMapKey(){};
                           TMapKey(const K &key)
                           {
                              Key=key;
                           };
                           TMapKey(const K &key,const R &result)
                           {
                              Key=key;
                              Result=result;
                           };
public:
   //
   int                     operator < (const TMapKey &k) const
   {
      return Key < k.Key;
   }
   //
   int                     operator == (const TMapKey &k) const
   {
      return Key == k.Key;
   }
};

template <class K,class R> class TMap {
   typedef TMapKey<K,R>    AIMapKey;
protected:
   TSet<AIMapKey>          sKey;
protected:
   //!Find key index (returns -1 if search failed).
   int                     _Find(const K &key) const;  
public:
   //!Add key <key> into map. If key already exists do nothing. Returns 0 if key added 1 otherwise.
   int                     Add(const K &key,const R &result);
   //!Remove key <key>. Returns 1 if key deleted, 0 if key not found.
   int                     Remove(const K &key);
   //!Clear key table.
   void                    Clear();
   //!Returns <i>th element.
   R                       &Index(int i);
   //!Returns 1 if key <key> exists 0 otherwise
   int                     IsKey(const K &key) const {return _Find( key )!=-1;}
   //!Find returns result for <key>.
   R                       &Find(const K &key);
   //!operator[] returns result for <key>.
   R                       &operator[](const K &key);
   //!operator() returns readonly result for <key>.
   const R                 operator()(const K &key) const;
   //Count returns number of elements in map.
   int                     Count() const {return (int)sKey;}
   //!operator int() enables map to act like integer - returning number of elements in map.
   operator                int() const{return Count();}
};

//////////////////////////////////////////
// Size/count control
//////////////////////////////////////////
template<class K,class R>int TMap<K,R>::Add(const K &key,const R &result)
{
   return sKey.Add( AIMapKey( key , result ) );
}

template<class K,class R>int TMap<K,R>::Remove(const K &key)
{
   int a=_Find( key );
   if( a==-1 ) return 0;
   //
   sKey.Remove( sKey[a] );
   //
   return 1;
}

template<class K,class R>void TMap<K,R>::Clear()
{
   sKey.Clear();
}

template<class K,class R>int TMap<K,R>::_Find(const K &key) const  
{
   return sKey.Find( key );
}

//////////////////////////////////////////
// operators
//////////////////////////////////////////

template<class K,class R>R &TMap<K,R>::Index(int i)
{
   #ifdef _DEBUG
      if( i<0 || i>=sKey) MAbort("TMap::Index() error: out of range.");
   #endif
   return sKey[ i ].Result;
}

template<class K,class R>R &TMap<K,R>::Find(const K &key)
{
   int a=_Find( key );
   //
   if( a>=0 ) return sKey[ a ].Result;
   //
   MAbort("TMap::operator[] error: key not found.");
   // Should never come here-here just to prevent compiler error.
   return sKey[0].Result;
}

template<class K,class R>R &TMap<K,R>::operator[](const K &key)
{
   return Find( key );
}

template<class K,class R>const R TMap<K,R>::operator()(const K &key) const
{
   int a=_Find( key );
   //
   if( a>=0 ) return sKey( a ).Result;
   //
   MAbort("TMap::operator() error: key not found.");
   // Should never come here-here just to prevent compiler error.
   return sKey(0).Result;
}

#endif
///////////////////////////////////////////////////////////////////////////
//
// X Memory management logging
// Copyright GP
//
///////////////////////////////////////////////////////////////////////////

#ifndef __XMEMORY__
#define __XMEMORY__

#include <globals/macros.h>

//////////////////////////////////////////////////
// Memory macros
//////////////////////////////////////////////////

XDLL void   __delete__( void *ptr );
XDLL void   *__new__( unsigned int size );
XDLL void   *__resize__(void *ptr , int oldsize , int newsize );

//////////////////////////////////////////////
// Declare New and Delete
//////////////////////////////////////////////


// delete override
inline void operator     delete( void *ptr )
{
   __delete__(ptr);
}

// new override
inline void * operator   new( unsigned int size )
{
   return __new__(size);
}

//////////////////////////////////////////////
// New and Delete macros
//////////////////////////////////////////////

#define                       xnew( type )                        new type
#define                       xnewp( type, params )               new type params
#define                       xnewa( type, arraysize )            new type [arraysize]
                                                                  
#define                       xdelete( obj )                      _PDELETE( obj )
#define                       xdeletea( obj )                     _PDELETEA( obj )

#define                       xresize( ptr , size , newsize )     __resize__( ptr , size , newsize )

#define                       xset( ptr , value )                 memset( &ptr , value , sizeof( ptr ) )
#define                       xsetptr( ptr , value )              memset( ptr , value , sizeof( ptr ) )

#define                       xcopy( dest , src )                 memcpy( dest , src , sizeof( src ) )


// placement new override
inline void * operator   new( unsigned int size , void *p)
{
   return p;
}

inline void operator delete(void *, void *)
{
   return; 
}

// constructor call
template <class ANY_TYPE>
inline void construct( ANY_TYPE &ptr )
{
   new( (void*) &ptr ) ANY_TYPE();
}

// destructor call
template <class ANY_TYPE>
inline void destruct( ANY_TYPE &ptr )
{
   ptr.~ANY_TYPE();
}

// destructor call
template <class ANY_TYPE>
inline ANY_TYPE *instance( ANY_TYPE *ptr )
{
   return ptr;
}


#endif

//////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////


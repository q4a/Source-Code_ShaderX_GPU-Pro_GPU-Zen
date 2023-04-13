/////////////////////////////////////////////////////////////////
//
// Global macros
//
// Copyright GP
// Created 3.6.2000
//
//$DONTCONVERTME
//
/////////////////////////////////////////////////////////////////

#ifndef __MACROS__
#define __MACROS__ 

//////////////////////////////////////////////////////////////
// main dll definition
//////////////////////////////////////////////////////////////

#ifndef XUBL_LIB
   #ifdef XUBL_EXPORTS
      #define XDLL __declspec(dllexport)
   #else
      #define XDLL __declspec(dllimport)
   #endif
#else
   #define XDLL 
#endif
   


//////////////////////////////////////////////////////////////
// Sytem macros
//////////////////////////////////////////////////////////////

// disable template export trought library warning
#pragma warning(disable:4251)

// define DOSEOL like EOL (#13#10)
// comment this to get UNIXEOL like EOL (#13)
#define DOSEOL       
//#define UNIXEOL

//////////////////////////////////////////////////////////////
// Main abort and warning functions
//////////////////////////////////////////////////////////////

XDLL void KernelAbort( const char *msg , const char *msgadd , const char *file, int line);
XDLL void KernelWarning( const char *msg , const char *msgadd , const char *file, int line);

//////////////////////////////////////////////////////////////
// Other macros
//////////////////////////////////////////////////////////////

#define TRUE      1
#define FALSE     0

#define MAXINT     0x7fffffff
#define MININT    -0x80000000

#ifndef MAbort
   #define MAbort(_x)         KernelAbort( _x , "" , __FILE__ , __LINE__ )
#endif

#ifndef MWarning
   #define MWarning(_x)       KernelWarning( _x , "" , __FILE__ , __LINE__ )
#endif

#ifdef _DEBUG
   #define MAssert(x)  { if(x) KernelAbort( "Assertion\n", #x , __FILE__ , __LINE__ ); }
#else
   #define MAssert(x) 
#endif

//////////////////////////////////////////////
// safe memory release
//////////////////////////////////////////////

#ifndef _PDELETE
   #define _PDELETE( x )   {if(x){delete  (x);x=0;}}
#endif

#ifndef _PDELETEA
   #define _PDELETEA( x )  {if(x){delete[](x);x=0;}}
#endif

#ifndef PRELEASE
   #define PRELEASE(p)     {if(p){(p)->Release();(p)=0;}}
#endif

#ifndef PFREE
   #define PFREE(p)        {if(p){(p)->Free();(p)=0;} }
#endif

#ifndef PDESTROY
   #define PDESTROY(p)     {if(p){(p)->Destroy();(p)=0;}}
#endif

//////////////////////////////////////////////
//
// Templates
//
//////////////////////////////////////////////

template<class _anytype> 
inline _anytype min(const _anytype &a, const _anytype &b)
{
   return ( a < b ? a : b );
}

template<class _anytype> 
inline _anytype max(const _anytype &a, const _anytype &b)
{
   return ( a > b ? a : b );
}

template<class _anytype> 
inline _anytype abs(const _anytype &a)
{
   return ( a < 0 ? -a : a );
}

template <class _anytype> 
inline void swap(_anytype& x, _anytype& y)
{
   _anytype z = x;
   x = y;
   y = z;
}

//////////////////////////////////////////////
// Swap bytes
//////////////////////////////////////////////

template<class _anytype>
inline _anytype vSwap( _anytype a )
{
   if(sizeof(a)==2)
   {
      return (_anytype)( (((WORD*)&a)[0] >> 8) | (((WORD*)&a)[0] << 8) );
   }
   else if(sizeof(a)==4)
   {
#ifdef INTEL_ASM
      __asm
      {
         mov      eax, [ dword ptr a ]
         bswap    eax
         mov      [ dword ptr a ], eax
      }
      return a;
#else
      DWORD b = *(DWORD*)&a;
      b = ( b>>24 | (b&0xff0000)>>8 | (b&0xff00)<<8 | b<<24 );
      return *(_anytype*)&b;
#endif
   }
   return a;
}


#ifdef INTEL_MEM
   #define vIntel   ( _x )    (_x)
   #define vMotorola( _x )    vSwap(_x)
#else
   #define vMotorola( _x )    (_x)
   #define vIntel   ( _x )    vSwap(_x)
#endif


#endif 

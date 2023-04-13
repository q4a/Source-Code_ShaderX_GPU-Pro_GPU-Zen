///////////////////////////////////////////////////////////////////////////
//
// Kernel
// Created 13.05.2001
// Copyright by GP
//
//$DONTCONVERTME
//
///////////////////////////////////////////////////////////////////////////

#include "kernel.h"
#include <windows.h>

int ShowMessage(const char *message)
{
   MessageBox( NULL , message , NULL , MB_OK | MB_ICONERROR | MB_SYSTEMMODAL );
   return 0;
}

int ShowMessage(const char *windowname,const char *message)
{
   MessageBox( NULL , message , NULL , MB_OK | MB_ICONERROR | MB_SYSTEMMODAL );
   return 0;
}

void TerminateApplication()
{
   TerminateProcess( GetCurrentProcess() , 1 );
}

void Pause(int milisec)
{
   Sleep( milisec );
}


typedef int    (*IInitFunct)(); 

int InitializeDirectX()
{
   HMODULE lib=LoadLibraryA("directx.renderer");
   if( !lib ) return 1;
   //
   IInitFunct  Init=(IInitFunct)GetProcAddress(lib,"InitializeDirectX");
   //
   if( !Init ) return 2;
   //
   return Init();
}

int InitializeOpenGl()
{
   HMODULE lib=LoadLibraryA("opengl.renderer");
   if( !lib ) return 1;
   //
   IInitFunct  Init=(IInitFunct)GetProcAddress(lib,"InitializeOpenGL");
   //
   if( !Init ) return 2;
   //
   return Init();
}




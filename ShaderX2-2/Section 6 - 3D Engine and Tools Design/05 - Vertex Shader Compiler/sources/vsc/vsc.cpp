///////////////////////////////////////////////////
//
// VSC
// Vertex shader compiler usage example 
// -compile buffer
// -compile file 
//
// xubllite.dll      is in xubllite/xubllite/debug or release directory
// vscompilerxl.dll  is in xubl/vscompiler/debugxl or releasexl directory
//
///////////////////////////////////////////////////

#include <globals/globals.h>
#include <stream/stream.h>

#include "vscompiler.h"

TArray<TString> aFunctionSwap;

///////////////////////////////////////////////////
//
// CompileBuffer
//
///////////////////////////////////////////////////

int CompileBuffer(TVertexShaderCompiler *c,const char *src)
{
   // set 'comment output' to none
   c->commentlevel=-1;
   //
   c->aFunctionRename=aFunctionSwap;

   // read file into buffer
   TString sin,sout;
   TFileStream f( src );
   //
   f.ReadString( sin , 10000 );

   ///////////////////////////////////////////////////
   // Compilation
   ///////////////////////////////////////////////////
   if( c->CompileBuffer( sin , sout ) )
   {
      printf("error compiling source file.\n");
      return 3;
   }

   ///////////////////////////////////////////////////
   // Output 
   ///////////////////////////////////////////////////

   // output assembler code
   printf("[\n%s]\n" , tochar( sout ) );

   // output parameters
   printf("parameters:\n");
   for(int a=0;a<c->Result.aParameter;a++)
   {
      printf("   parameter [%s] = c%d\n" , 
         tochar( c->Result.aParameter[a].Name ) , 
         c->Result.aParameter[a].index );

   }

   // output plugin usage
   printf("plugin usage:\n");
   for(a=0;a<c->Result.aPlugin;a++)
   {
      printf("   plugin [%s]\n" , tochar( c->Result.aPlugin[a].PluginName ) );
      for(int v=0;v<c->Result.aPlugin[a].aConst;v++)
         if( c->Result.aPlugin[a].aConst[v]>=0 )
            printf("      var%d=c%d\n" , 
               v ,
               c->Result.aPlugin[a].aConst[v] );
   }

   // output constant usage
   printf("const usage:\n");
   for(a=0;a<c->Result.aConst;a++)
      printf("   c%d=(%s,%s,%s,%s)\n" , 
         c->Result.aConst[a].index ,
         (c->Result.aConst[a].Value.x!=unusedconstvalue ? tochar( toString( c->Result.aConst[a].Value.x ) ) : "?" ),
         (c->Result.aConst[a].Value.y!=unusedconstvalue ? tochar( toString( c->Result.aConst[a].Value.y ) ) : "?" ),
         (c->Result.aConst[a].Value.z!=unusedconstvalue ? tochar( toString( c->Result.aConst[a].Value.z ) ) : "?" ),
         (c->Result.aConst[a].Value.w!=unusedconstvalue ? tochar( toString( c->Result.aConst[a].Value.w ) ) : "?" ) );
   //
   return 0;
}

///////////////////////////////////////////////////
//
// CompileFile
//
///////////////////////////////////////////////////

int CompileFile(TVertexShaderCompiler *c,const char *src,const char *des)
{
   ///////////////////////////////////////////////////
   // Compilation
   ///////////////////////////////////////////////////
   // set 'comment level' to all
   //c->commentlevel=10;
   c->commentlevel=1;
   //
   c->aFunctionRename=aFunctionSwap;
   //
   if( c->CompileFile( src , des ) )
   {
      printf("error compiling source file '%s'.\n" , src );
      return 3;
   }
   return 0;
}


///////////////////////////////////////////////////
//
// main
//
///////////////////////////////////////////////////

int main(int argn,char **argc)
{
   TVertexShaderCompiler *c=CreateVertexShaderCompiler();
   if( !c ) return 1;
   //
   printf( "Vertex shader compiler v%.3f\nUsage example\n\n" , c->GetVersion() );
   //
   if( argn<3 )
   {
      printf("usage: vsc <infile.cvs> <outfile.vss> [include path] [[*procedure swap]...]\n");
      return 2;
   }
   //
   printf( "Compiling '%s'...\n" , argc[1] );
   //
   if( argc[3] ) c->IncludePath=argc[3];
            else c->IncludePath="vscinclude";

   for(int a=2;a<argn;a++)
      if( argc[a][0]=='*' ) aFunctionSwap.Add( argc[a] );
   //
   for(a=0;a<aFunctionSwap;a++)
   {
      aFunctionSwap[a].Copy(1);
      aFunctionSwap[a].Trim();
   }
   //
   if( aFunctionSwap & 1 )
   {
      printf("error: number of Procedure Swap parameters must be odd.\n");
      return 1;
   }
   //
   if( aFunctionSwap )
   {
      printf("Function swap:\n");
      for(a=0;a<aFunctionSwap;a+=2)
      {
         printf("   '%s' with '%s'\n" , tochar( aFunctionSwap[a] ) , tochar( aFunctionSwap[a+1] ) );
      }
   }

   ///////////////////////////////////////
   // buffer compilation
   ///////////////////////////////////////
   printf("\nBuffer compilation...\n");
   CompileBuffer( c , argc[1] );

   ///////////////////////////////////////
   // file compilation
   ///////////////////////////////////////
   printf("\nFile compilation...\n");
   CompileFile( c , argc[1] , argc[2] );
 
   //
   xdelete( c );
   //
	return 0;
}

///////////////////////////////////////////////////
//
// 
//
///////////////////////////////////////////////////

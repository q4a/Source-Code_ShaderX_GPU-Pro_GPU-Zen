////////////////////////////////////////////////////////////////////////
//
// Vertex Shader Compiler
// Created 30.07.2002
//
////////////////////////////////////////////////////////////////////////
//$XHEADER

//$XHEADER
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>

#include "vscompiler.h"

#include "compiler.h"

#include <stream/stream.h>

#ifndef NO_PLUGIN
   #include <win/library.h>

   MDefineLibraryEntry( "vscompiler.dll" );
#endif

////////////////////////////////////////////////////////////////////////
// Register plugins
////////////////////////////////////////////////////////////////////////

#ifndef NO_PLUGIN

   MRegisterPlugin( TVertexShaderCompiler , Compiler );

#else
   TVertexShaderCompiler *CreateVertexShaderCompiler()
   {
      return xnew( TVertexShaderCompiler );
   }
#endif

/////////////////////////////////////////////////////////////
// globals for lex and bison
/////////////////////////////////////////////////////////////

int   ParseCompileFile(const char *fname);
int   ParseCompileBuffer(const char *buf,int size);

////////////////////////////////////////////////////////////////////////
//
// TVertexShaderCompiler
//
////////////////////////////////////////////////////////////////////////

CCompiler      *pCompiler=0;

int TVertexShaderCompiler::CompileFile(const char *in,const char *out)
{
   int            res=0;
   CCompiler      Com;
   CFunction      *mainf=0;
   SCompilerTree  *mainv=0;
   //
   pCompiler=&Com;

   SCResetPlugins();

   /////////////////////////////////////////////////
   // set compiler settings
   /////////////////////////////////////////////////


   Com.optimization  = optimization;
   Com.commentlevel  = commentlevel;
   Com.IncludePath   = IncludePath;
   Com.constcount    = constcount;
   Com.tempcount     = tempcount;
   Com.inputcount    = inputcount;
   memcpy( Com.regperins , regperins , sizeof( regperins ) );
   Com.vsversion     = vsversion;
   Com.aFunctionRename=aFunctionRename;
   Com.aDefine       = aDefine;


   /////////////////////////////////////////////////
   // set register reservations
   /////////////////////////////////////////////////
   Com.aConstUsed.SetCount( Com.constcount*4 );
   Com.aConst.SetCount( Com.constcount*4 );
   //
   Com.aTempUsed.SetCount( Com.tempcount*4 );

   //
   SCompilerTree *root=0;
   pCompilerTreeMng=xnew( SCompilerTreeManager );
   pParameterMng=xnew( SParameterManager );
   pScriptDebug=xnew( SDebug );
   //
   if( ParseCompileFile( in ) ) goto error;
   root=pCompilerTreeMng->pRoot;

   //

   if( Com.Error ) goto error;

   //
   Com.pAsmOutput=xnewp( TFileStream , ( out , smWrite ) );
   //
   Com.AnalizeCode( root );
   Com.ResetPluginsUsage();
   //
   mainf=Com.FindFunction( "vsmain" );
   //
   if( !mainf ) return 1;
   //
   mainv=mainf->pTree->aTree[2];
   //
   Com.VariableUsage( mainv );
   //
   Com.PrepareCodeGeneration();
   Com.pActiveFunction=mainf;
   Com.PrepareFunctionVariables();
   //
   Com.CreateCode( mainv );
   //
   Com.Finalize();
   Com.OutputAssembler();
   //
   Com.pAsmOutput->Close();
   //
   if( Com.Error ) goto error;
   //
   goto noerror;
error:
   res=1;
noerror:
   xdelete( pParameterMng );
   xdelete( pScriptDebug );
   xdelete( pCompilerTreeMng );
   //                                      
   return res;
}

int TVertexShaderCompiler::CompileBuffer(const TString &in,TString &out)
{
   int            res=0;
   CCompiler      Com;
   CFunction      *mainf=0;
   SCompilerTree  *mainv=0;
   //
   pCompiler=&Com;

   SCResetPlugins();

   /////////////////////////////////////////////////
   // set compiler settings
   /////////////////////////////////////////////////


   Com.optimization  = optimization;
   Com.commentlevel  = commentlevel;
   Com.IncludePath   = IncludePath;
   Com.constcount    = constcount;
   Com.tempcount     = tempcount;
   Com.inputcount    = inputcount;
   memcpy( Com.regperins , regperins , sizeof( regperins ) );
   Com.vsversion     = vsversion;
   Com.aFunctionRename=aFunctionRename;
   Com.aDefine       = aDefine;

   /////////////////////////////////////////////////
   // set register reservations
   /////////////////////////////////////////////////
   Com.aConstUsed.SetCount( Com.constcount*4 );
   Com.aConst.SetCount( Com.constcount*4 );
   //
   Com.aTempUsed.SetCount( Com.tempcount*4 );

   //
   SCompilerTree *root=0;
   pCompilerTreeMng=xnew( SCompilerTreeManager );
   pParameterMng=xnew( SParameterManager );
   pScriptDebug=xnew( SDebug );
   //
   if( ParseCompileBuffer( tochar( in ) , in ) ) goto error;
   root=pCompilerTreeMng->pRoot;

   //

   if( Com.Error ) goto error;

   //
   Com.pAsmOutput=xnewp( TMemoryStream , ( "" , smWrite ) );
   //
   Com.AnalizeCode( root );
   Com.ResetPluginsUsage();
   //
   mainf=Com.FindFunction( "vsmain" );
   //
   if( !mainf ) return 1;
   //
   mainv=mainf->pTree->aTree[2];
   //
   Com.VariableUsage( mainv );
   //
   Com.PrepareCodeGeneration();
   Com.pActiveFunction=mainf;
   Com.PrepareFunctionVariables();
   //
   Com.CreateCode( mainv );
   //
   Com.Finalize();
   Com.OutputAssembler();
   //
   Com.pAsmOutput->Close();
   //
   Com.pAsmOutput->Seek(0);
   Com.pAsmOutput->ReadString( out , Com.pAsmOutput->GetSize() );
   //
   Result=Com.Result;
   //
   if( Com.Error ) goto error;
   //
   goto noerror;
error:
   res=1;
noerror:
   xdelete( pParameterMng );
   xdelete( pScriptDebug );
   xdelete( pCompilerTreeMng );
   //
   return res;
}

////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////

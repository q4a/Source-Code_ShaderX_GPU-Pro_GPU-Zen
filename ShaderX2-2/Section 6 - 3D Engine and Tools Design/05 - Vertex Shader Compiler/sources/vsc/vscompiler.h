////////////////////////////////////////////////////////////////////////
//
// Vertex Shader Compiler
// Created 30.07.2002
//
////////////////////////////////////////////////////////////////////////

#ifndef __VSCOMPILER__
#define __VSCOMPILER__

#define NO_PLUGIN 
// define will produce function CreateVertexShaderCompiler to create compiler
// otherwise plugin will be created

#include <globals/globals.h>

#ifndef NO_PLUGIN
   #include <globals/plugin/plugin.h>
#endif

const float       vscompilerversion = 0.103f;
const float       unusedconstvalue  = 123E-45f;

#ifdef VSC_EXPORTS
   #define XVSDLL __declspec(dllexport)
#else
   #define XVSDLL __declspec(dllimport)
#endif


enum EVariableStorage {
   vsNone         =0,
   vsInput        =1,
   vsOutput       =2,
   vsTemp         =3,
   vsConst        =4,
};

class XVSDLL TProgramParameter {
public:
   TName                         Name;
   int                           index;
};

class XVSDLL TPluginUsage {
public:
   TString                       PluginName;
   TArray<int>                   aConst;
};

class XVSDLL TConstUsage {
public:
   int                           index;
   TVector                       Value;
};

class XVSDLL TCompilerResult {
public:
   TArray<TProgramParameter>     aParameter;
   TArray<TPluginUsage>          aPlugin;
   TArray<TConstUsage>           aConst;
   TArray<TString>               aErrors;
};

////////////////////////////////////////////////////////////////////////
//
// TVertexShaderCompiler
//
////////////////////////////////////////////////////////////////////////

#ifdef NO_PLUGIN
   class TVertexShaderCompiler;

   XVSDLL TVertexShaderCompiler *CreateVertexShaderCompiler();

   class XVSDLL TVertexShaderCompiler {
#else
   class XVSDLL TVertexShaderCompiler : public TPlugin {
#endif

public:
   // compiler settings
   int                           optimization;        // optimization level
   int                           commentlevel;        // comment level (-1:no comments .. 10:all comment)
   TString                       IncludePath;         // include path
   // assembler restructions
   int                           constcount;          // max input register
   int                           tempcount;           // max input register
   int                           inputcount;          // max input register
   //
   int                           regperins[5];        // max registers of storage type per instruction
   //
   int                           vsversion;           // initial version 1000 or 1100
   //
   TArray<TString>               aDefine;             // defines
   //
   TArray<TString>               aFunctionRename;     // array of function renaming (use name[a*2+1] instead of name[a*2])
                                                      // example:
                                                      // ('vstransform','vstransform_normal');
                                                      // will use function 'vstransform_normal' in place of
                                                      // of 'vstransform'.
public:
   // compiler return
   TCompilerResult               Result;
   //
public:
                                 TVertexShaderCompiler()
                                 {
                                    optimization=10;
                                    vsversion=1000;
                                    //
                                    constcount=96;
                                    tempcount=12;
                                    inputcount=16;
                                    //
                                    regperins[ vsConst ]=1;
                                    regperins[ vsTemp  ]=3;
                                    regperins[ vsInput ]=1;
                                 }
public:
   float                         GetVersion(){return vscompilerversion;}
   //
   int                           CompileFile(const char *in,const char *out);
   int                           CompileBuffer(const TString &in,TString &out);
};

#endif

////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Compiler
// Created 23.04.2002
//
////////////////////////////////////////////////////////////////////////

#ifndef __COMPILER__
#define __COMPILER__

#include <globals/globals.h>
#include <stream/stream.h>

#include "compilertree.h"
#include "compilererrors.h"

#include "vscompiler.h"

////////////////////////////////////////////////////////////////////////
//
// CVariable
//
////////////////////////////////////////////////////////////////////////

enum EVariableType {
   vtVoid,
   vtFloat,
   vtVector,
};

class CVariable {
public:
   char                          valid;         // hold class valid variable data
   //
   char                          used;          // is variable used
   char                          assigned;      // is variable assigned   
   char                          param;         // is variable function parameter
   //
   char                          templevel;     // is variable temporary
   //
   TString                       modifier;      // variable modifier (x,y,xy,...)
   char                          neg;           // negative modifier
   //
   EVariableStorage              Storage;       
   EVariableType                 Type;
   //
   TString                       Name;          // variable name
   TString                       ParamName;     // parameter assignment variable name
   //
   int                           arraysize;     // array size (for const param)
   //
   CVariable                     *pArray;       // array parameter
   //
   int                           index;         // register index (together with storage they determine register)
public:
                                 CVariable()
                                 {
                                    valid=0;
                                    param=0;
                                    used=0;
                                    neg=0;
                                    assigned=0;
                                    arraysize=0;
                                    Storage=vsTemp;
                                    index=0;
                                    templevel=0;
                                    //
                                    pArray=0;
                                    //
                                    ParamName="";
                                 }
                                 CVariable(const CVariable &v)
                                 {
                                    *this=v;
                                 }
                                 ~CVariable()
                                 {
                                    Clear();
                                 }
public:
   CVariable                     &operator=(const CVariable &v)
   {
      //valid=1;
      valid     =v.valid;
      //
      used      =v.used;      
      assigned  =v.assigned;      
      param     =v.param;         
      modifier  =v.modifier;      
      Storage   =v.Storage;       
      Type      =v.Type;      
      neg       =v.neg;
      Name      =v.Name;          
      ParamName =v.ParamName; 
      if( v.pArray ) 
      {
         #ifdef NO_PLUGIN
            pArray    = xnew( CVariable );
            *pArray   = *v.pArray;   
         #else
            pArray    =instance( v.pArray );   
         #endif
      }
      else 
      {
         pArray    =0;
      }
      index     =v.index;         
      //
      return *this;
   }
                                 operator==(const CVariable &v) const
   {
      if( modifier  != v.modifier   ) return 0;
      if( Storage   != v.Storage    ) return 0;
      if( Type      != v.Type       ) return 0;
      if( Name      != v.Name       ) return 0;   
      if( neg       != v.neg        ) return 0;   
      if( ParamName != v.ParamName  ) return 0;
      if( index     != v.index      ) return 0;
      //
      if( int(pArray) ^ int(v.pArray) ) return 0;
      //
      return 1;
   }
public:
   void                          Clear()
   {
      valid=0;
      index=-1;
      xdelete( pArray );
   }
public:
   TString                       toString() const;
};

////////////////////////////////////////////////////////////////////////
//
// CFunction
//
////////////////////////////////////////////////////////////////////////

class CFunction {
public:
   TString                       Name;
   SCompilerTree                 *pTree;
   EVariableType                 Type;
   //
   TArray<CVariable>             aParam;
   TArray<CVariable>             aVariable;
};

////////////////////////////////////////////////////////////////////////
//
// CCompiler
//
////////////////////////////////////////////////////////////////////////

enum EAsmCommandType {
   actInstruction,
   actLabel,
   actComment,
};

class CAsmCommand {
public:
   EAsmCommandType               Type;
   //
   TString                       Instruction;
   CVariable                     Var[4];
   TString                       Comment;
public:
                                 CAsmCommand()
                                 {
                                    Type=actInstruction;
                                 }
                                 CAsmCommand(const char *label,const char *comment)
                                 {
                                    Type=actLabel;
                                    Instruction=label;
                                    Comment=comment;
                                 }
                                 CAsmCommand(const char *comment)
                                 {
                                    Type=actComment;
                                    Comment=comment;
                                 }
public:
   void                          Set(const char *ins,CVariable *v0,CVariable *v1=0,CVariable *v2=0,CVariable *v3=0,const char *comment=0)
   {
      Type=actInstruction;
      Instruction=ins;
      Var[0].Clear();
      Var[1].Clear();
      Var[2].Clear();
      Var[3].Clear();
      if( v0 ) Var[0]=*v0;
      if( v1 ) Var[1]=*v1;
      if( v2 ) Var[2]=*v2;
      if( v3 ) Var[3]=*v3;
      if( comment ) Comment=comment;
   }
   TString                       toString(int commentlevel);
};

class CVariableStack {
public:
   int                           temp;
   //
   CVariable                     Var;
public:
                                 CVariableStack()
                                 {
                                 }
                                 CVariableStack(int idx)
                                 {
                                    temp=1;
                                    //
                                    Var.Name="temp";
                                    Var.Storage=vsTemp;
                                    Var.Type=vtVector;
                                    Var.index=idx;
                                    Var.assigned=1;
                                 }
                                 CVariableStack(const CVariable &v,int intemp=0)
                                 {
                                    temp=intemp;
                                    //
                                    Var=v;
                                 }
};

class CPlugin {
public:
   TString                       Name;
   char                          used;
   int                           radical;
   //
   TArray<CVariable>             aParam;
public:
                                 CPlugin()
                                 {
                                    used=0;
                                    radical=0;
                                 }
};

class CCompiler {
public:
   SError                        Error;
   CFunction                     *pActiveFunction;
   CPlugin                       *pActivePlugin;
   CVariable                     *pGetVariable;
   //
   CFunction                     InputVertex;
   CFunction                     OutputVertex;
   //
   TArray<CPlugin>               aPlugin;
   TArray<CFunction>             aFunction;
   //
   TArray<CVariable>             aGlobalVariable;
   //
   EVariableStorage              BlockMode;
   //
   int                           optimization;
   int                           commentlevel;
   // optimization
   TString                       lastins;
   TString                       lastcom;
   CVariable                     lastop[4];
   // settings
   TString                       IncludePath;
   // restructions
   int                           constcount;
   int                           tempcount;
   int                           inputcount;
   //
   int                           regperins[5];
   //
   TArray<TString>               aIncluded;
   int                           vsversion;
   //
   TArray<TString>               aFunctionRename;     // array of function renaming (see vscompiler for more info).
   TArray<CAsmCommand>           aAssembler;
public:
   // compiler return
   TCompilerResult               Result;
public:
                                 CCompiler();
                                 ~CCompiler();
public:
   // expression solver
   TArray<int>                   aRegister;
   TArray<CVariableStack>        aVariableStack;
   //
   void                          RegisterPush(CVariable *v,int temp=0);
   CVariable                     RegisterPush();
   CVariable                     RegisterPop(int takeoff=1,int idx=0);
   //
   void                          CheckModifier(CVariable &v,SCompilerTree *t);
   int                           IsFloatModifier(SCompilerTree *t);
   //
   void                          AnalizeRegisterUsage(SCompilerTree *t,CVariable *res,CVariable *v0,CVariable *v1=0,CVariable *v2=0,CVariable *v3=0);
   //
   void                          Expression4(const char *ins,SCompilerTree *t);
   void                          Expression3(const char *ins,SCompilerTree *t);
   void                          Expression2(const char *ins,SCompilerTree *t);
   void                          AssignExpression(const char *ins,SCompilerTree *t);
   void                          IfElseExpression(const char *ins,SCompilerTree *t);
   void                          IfExpression(const char *ins,SCompilerTree *t);
   // assembler output stream 
   TStream                       *pAsmOutput;
public:
   // temp registers
   TArray<int>                   aTempUsed;
   //
   CVariable                     AddTemp1();
   CVariable                     AddTemp4();
   void                          FreeTemp(const CVariable &v);
public:
   // precompiler
   TArray<TString>               aDefine;
   // constant registers
   TArray<float>                 aConst;
   TArray<int>                   aConstUsed;
   //
   CVariable                     AddConstant1(float c,int canbeused=1);
   CVariable                     AddConstant4(float x,float y,float z,float w=0,int canbeused=1);
public:
   // assembler output
   void                          AsmOut(const char *ins,CVariable *p0=0,CVariable *p1=0,CVariable *p2=0,CVariable *p3=0,const char *comment=0);
   void                          AsmOut2(const char *ins,CVariable *p0=0,CVariable *p1=0,CVariable *p2=0,CVariable *p3=0,const char *comment=0);
   void                          AsmOutLabel(const char *label,const char *comment=0);
   void                          AsmOutComment(int level,const char *f,...);
   //
   void                          OutputAssembler();
public:
   // defines
   void                          AddDefine(const char *c);
   void                          RemoveDefine(const char *c);
   int                           Defined(const char *c);
public:
   // function 
   void                          CreateFunction(const char *name,const char *type,SCompilerTree *t);
   CFunction                     *FindFunction(const char *name);
public:
   // variable
   CVariable                     *CreateVariable(const char *name,const char *type,EVariableStorage storage,SCompilerTree *t,int functparam=0);
   CVariable                     *FindVariable(const TString &name,const TString &classname="");
   CVariable                     GetVariable( SCompilerTree *t , int analize=0 );
public:
   TString                       GetInclude(SCompilerTree *t,const char *inc,int sys=0);
   int                           Include(SCompilerTree *t,const char *inc,int sys=0);
   //
   void                          AnalizeCode( SCompilerTree *t , int num=-1 , int level=0, int active=1);
   void                          VariableUsage( SCompilerTree *t );
   void                          CreateCode( SCompilerTree *t , int num=-1 , int level=0);
   //
   void                          ResetPluginsUsage();
   void                          PrepareFunctionVariables();
   void                          PrepareCodeGeneration();
   void                          Finalize();
};

////////////////////////////////////////////////////////////////////////
// SCPlugin manipulation 
////////////////////////////////////////////////////////////////////////

void  SCResetPlugins();
void  SCRegisterPlugin( const char *name );
int   SCIsPlugin( const char *name );

#endif

////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////

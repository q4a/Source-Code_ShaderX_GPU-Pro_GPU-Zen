////////////////////////////////////////////////////////////////////////
//
// CompilerTree
// Created 16.04.2002
//
////////////////////////////////////////////////////////////////////////

#ifndef __COMPILER_TREE__
#define __COMPILER_TREE__

#include <globals/globals.h>

class SParameter;
class SParameterManager;
class SCompilerTree;
class SCompilerTreeManager;
class SDebug;

////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////

extern SCompilerTreeManager   *pCompilerTreeMng;
extern SParameterManager      *pParameterMng;
extern SDebug                 *pScriptDebug;

void ClearCompiler();

////////////////////////////////////////////////////////////////////////
//
// SDebugInfo
//
////////////////////////////////////////////////////////////////////////

class SDebugInfo {
public:
   short                sourceidx;
   short                line;
   short                col;
public:
                        SDebugInfo()
                        {
                           sourceidx=-1;
                           line=-1;
                           col=-1;
                        }
};

////////////////////////////////////////////////////////////////////////
//
// SParameter
//
////////////////////////////////////////////////////////////////////////

enum EParameterType {
   ptNone,
   ptInt,
   ptFloat,
   ptString,
};

class SParameter {
public:
   EParameterType       Type;    
   int                  param;
public:
   SParameterManager    *pManager;               
   SDebugInfo           Debug;
public:
                        SParameter()
                        {
                           Type=ptNone;
                           param=0;
                           pManager=0;
                        }
                        ~SParameter();
public:
   void                 Set(int i)
   {
      Type=ptInt;
      param=i;
   }
   void                 Set(float f)
   {
      Type=ptFloat;
      (float&)param=f;
   }
   void                 Set(const char *s);
public:
   const char           *toString();
   const char           *GetType();
};

////////////////////////////////////////////////////////////////////////
//
// SParameterManager
//
////////////////////////////////////////////////////////////////////////

class SParameterManager {
public:
   TArray<TString>      aConst;
   TArray<SParameter*>  aParam;
public:
                        ~SParameterManager()
                        {
                           Clear();
                        }
public:
   void                 Clear();
   int                  AddConst( const char *name );
public:
   SParameter           *CreateParameter();
   SParameter           *CreateParameter(int i);
   SParameter           *CreateParameter(float t);
   SParameter           *CreateParameter(const char *s);
};

////////////////////////////////////////////////////////////////////////
//
// SDebug
//
////////////////////////////////////////////////////////////////////////

class SDebug {
public:
   // debug info
   SDebugInfo              Debug;
   //
   TArray<TString>         aSource;
public:
   int                     AddSource( const char *c );
};


////////////////////////////////////////////////////////////////////////
//
// SCompilerTreeMng
//
////////////////////////////////////////////////////////////////////////

class SCompilerTreeManager {
public:
   TArray<SCompilerTree*>  aTree;
   //
   SCompilerTree           *pRoot;
public:
                           SCompilerTreeManager();
                           ~SCompilerTreeManager();
public:
   void                    Add(SCompilerTree *t);
   void                    Delete(SCompilerTree *t,int deleteobject=1);
};
////////////////////////////////////////////////////////////////////////
//
// SCompilerTree
//
////////////////////////////////////////////////////////////////////////

enum ENodeType {
   ntNone,

   ntImport,            ntImportClass,
   ntImportMethod,      ntImportDefVar,         ntMethodAccess,

   ntPlugin,            ntPluginConst,

   ntClass,             ntClassPrivate,         ntClassPublic,
   ntClassPotected,     ntClassMethod,          ntClassDefVar,
   ntClassConstructor,  ntClassDestructor,      ntClassVar,

   ntInclude,           ntIncludeSys,
   ntIfDef,             ntIfnDef,               ntEndIf,          
   ntDefine,            ntUndefine,             ntOptimization,

   ntProgram,           ntFunction,             ntIf,
   ntFor,               ntDoWhile,              ntWhile,             ntSwitch,
   ntCase,              ntDefault,              ntContinue,          ntBreak,
   ntReturn,            ntThrow,                ntCatch,             
   ntScope,             ntEmpty,                ntStorage,

   ntRunThread,         ntFuncCall,             ntFuncCallRet,       ntEllipsis,             

   // expression

   ntExpression,        ntLExpresion,           
   
   ntReserve,

   ntAdd,               ntSub,                  ntDiv,               ntMod,
   ntMul,               ntOr,                   ntAnd,               ntXor,
   ntShl,               ntShr,                  

   ntIfGeElse,          ntIfLsElse,             ntIfGe,              ntIfLs,
   
   ntExprAdd,           ntExprDp3,              ntExprDp4,           ntExprDst,
   ntExprExpp,          ntExprLit,              ntExprLogp,          ntExprMad,
   ntExprMax,           ntExprMin,              ntExprMov,           ntExprMul,
   ntExprRcp,           ntExprRsq,              ntExprSge,           ntExprSlt,
   ntExprSub,           ntExprExp,              ntExprFrc,           ntExprLog,
   ntExprM3x2,          ntExprM3x3,             ntExprM3x4,          ntExprM4x3,
   ntExprM4x4,

   ntEq,                ntNe,                   ntGt,                ntGe,
   ntLt,                ntLe,                   ntLand,              ntLor,

   ntAssign,            ntSetAdd,               ntSetSub,            ntSetDiv,
   ntSetMod,            ntSetMul,               ntSetOr,             ntSetAnd,
   ntSetShl,            ntSetShr,               ntCond,

   ntToken,             ntFloatModifier,        ntVectorModifier,
   ntInput,             ntOutput,
   ntVertexShader,

   ntVariable,    
   ntInputVariable,
   ntDefVariable,
   ntLParam,            
   ntOutputLParam,
   ntLParamArray,          
   ntReference,
   ntModifier,

   ntRadical,

   ntNeg,               ntPreDec,               ntPreInc,            ntPostDec,
   ntPostInc,           ntNot,                  ntCom,               ntEnv,

   ntArray,             ntArrayConst,           ntArrayAccess,       ntArrayElemExpr,

   ntType,
   ntStringConst,       ntIntConst,             ntFloatConst,        ntNullConst,
   ntVectorConst,

   ntRegisterA,         ntRegisterT,            ntRegisterI,         ntRegisterC,
   ntRegisterO,

   ntAsmAdd,            ntAsmDp3,               ntAsmDp4,            ntAsmDst, 
   ntAsmExpp,           ntAsmLit,               ntAsmLogp,           ntAsmMad, 
   ntAsmMax,            ntAsmMin,               ntAsmMov,            ntAsmMul, 
   ntAsmRcp,            ntAsmRsq,               ntAsmSge,            ntAsmSlt, 
   ntAsmSub,         

   ntAsmExp,            ntAsmFrc,               ntAsmLog,            ntAsmM3x2,
   ntAsmM3x3,           ntAsmM3x4,              ntAsmM4x3,           ntAsmM4x4,
};


class SCompilerTree {
public:
   ENodeType               Type;
   //
   SCompilerTree           *pParent;
   //
   SCompilerTree           *pNext;
   SCompilerTree           *aTree[4];
   //
   SParameter              *pParameter;
public:
   // debug
   SDebugInfo              Debug;
public:
                           SCompilerTree()
                           {
                              Type=ntNone;
                              pNext=0;
                              pParameter=0;
                              aTree[0]=aTree[1]=aTree[2]=aTree[3]=0;
                              pParent=0;
                              //
                              pCompilerTreeMng->Add( this );
                           }
                           SCompilerTree( ENodeType inType , SCompilerTree *p0=0 , SCompilerTree *p1=0 , SCompilerTree *p2=0 , SCompilerTree *p3=0 )
                           {
                              pNext=0;
                              pParameter=0;
                              pParent=0;
                              //
                              Type=inType;
                              //
                              aTree[0]=p0;
                              aTree[1]=p1;
                              aTree[2]=p2;
                              aTree[3]=p3;
                              //
                              if( p0 ) p0->pParent=this;
                              if( p1 ) p1->pParent=this;
                              if( p2 ) p2->pParent=this;
                              if( p3 ) p3->pParent=this;
                              //
                              if( p0 ) Debug=p0->Debug;
                              //
                              pCompilerTreeMng->Add( this );
                           }
                           SCompilerTree( ENodeType inType , SParameter *p , SCompilerTree *p0=0 , SCompilerTree *p1=0 , SCompilerTree *p2=0 , SCompilerTree *p3=0 )
                           {
                              pNext=0;
                              //
                              Type=inType;
                              Debug=p->Debug;
                              //
                              pParameter=p;
                              pParent=0;
                              //
                              aTree[0]=p0;
                              aTree[1]=p1;
                              aTree[2]=p2;
                              aTree[3]=p3;
                              //
                              if( p0 ) p0->pParent=this;
                              if( p1 ) p1->pParent=this;
                              if( p2 ) p2->pParent=this;
                              if( p3 ) p3->pParent=this;
                              //
                              pCompilerTreeMng->Add( this );
                           }
                           ~SCompilerTree()
                           {
                              // parameters are deleted with ParameterManager
                              //xdelete( pParameter  );
                              //
                              //xdelete( aTree[0]    );
                              //xdelete( aTree[1]    );
                              //xdelete( aTree[2]    );
                              //xdelete( aTree[3]    );
                              //
                              //xdelete( pNext       );
                              //
                              pCompilerTreeMng->Delete( this , 0 );
                           }
public:
   void                    SetNext( SCompilerTree *n )
   {
      if( !n ) return;
      //
      pNext=n;
      n->pParent=pParent;
   }
   int                     IsExpression();
public:
   const char              *GetNodeName();
};

#endif

////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////

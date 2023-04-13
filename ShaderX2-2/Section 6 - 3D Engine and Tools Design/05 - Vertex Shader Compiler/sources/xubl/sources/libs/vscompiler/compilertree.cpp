////////////////////////////////////////////////////////////////////////
//
// CompilerTree
// Created 16.04.2002
//
////////////////////////////////////////////////////////////////////////
//$XHEADER
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>

#include "compilertree.h"

////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////

SCompilerTreeManager    *pCompilerTreeMng=0;
SParameterManager       *pParameterMng=0;
SDebug                  *pScriptDebug=0;

////////////////////////////////////////////////////////////////////////
//
// SParameter
//
////////////////////////////////////////////////////////////////////////

SParameter::~SParameter()
{
   if( pManager ) pManager->aParam.RemoveFastFind( this );
}

void SParameter::Set(const char *s)
{
   if( !pManager ) MAbort( "SVariable::Set error: no manager assigned." );
   //
   Type=ptString;
   param=pManager->AddConst( s );
}                   

const char *SParameter::toString()
{
   static char buf[32];
   //
   switch( Type ){
   case ptInt     : {
                        ::toString( param ).toChar( buf );
                        //
                        return buf;
                    }
   case ptFloat   : {
                        ::toString( (float&)param ).toChar(  buf );
                        //
                        return buf;
                    }
   case ptString  : return pManager->aConst[ param ];
   default        : return "<unknown>";
   }
}

const char *SParameter::GetType()
{
   switch( Type ){
   case ptInt     : return "ptInt";   
   case ptFloat   : return "ptFloat"; 
   case ptString  : return "ptString";
   default        : return "";
   }
}



////////////////////////////////////////////////////////////////////////
//
// SParameterManager
//
////////////////////////////////////////////////////////////////////////

int SParameterManager::AddConst( const char *name )
{
   for(int a=0;a<aConst;a++)
      if( aConst[a]==name ) return a;
   //
   aConst.Add( name );
   //
   return aConst-1;
}

extern int      yycol;

SParameter *SParameterManager::CreateParameter()
{
   SParameter *v=xnew( SParameter );
   //
   v->Debug=pScriptDebug->Debug;
   v->Debug.col=yycol;
   //
   v->pManager=this;
   //
   aParam.Add( v );
   //
   return v;
}

SParameter *SParameterManager::CreateParameter(int i)
{
   SParameter *v=CreateParameter();
   //
   v->Debug=pScriptDebug->Debug;
   v->Debug.col=yycol;
   v->Set( i );
   //
   return v;
}

SParameter *SParameterManager::CreateParameter(float f)
{
   SParameter *v=CreateParameter();
   //
   v->Debug=pScriptDebug->Debug;
   v->Debug.col=yycol;
   v->Set( f );
   //
   return v;
}

SParameter *SParameterManager::CreateParameter(const char *s)
{
   SParameter *v=CreateParameter();
   //
   v->Debug=pScriptDebug->Debug;
   v->Debug.col=yycol;
   v->Set( s );
   //
   return v;
}

void SParameterManager::Clear()
{
   aConst.Clear();
   //
   while( aParam )
   {
      SParameter *p=aParam[0];
      //
      xdelete( p );
   }
   //for(int a=0;a<aParam;a++)
   //   xdelete( aParam[a] );
   // 
   aParam.Clear();
}

////////////////////////////////////////////////////////////////////////
//
// SCompilerTree
//
////////////////////////////////////////////////////////////////////////

#define MCaseReturnString( a ) case a: return #a


int SCompilerTree::IsExpression()
{
   SCompilerTree *r=this->pParent;
   while( r )
   {
      if( r->Type==ntVariable ) return 0;
      if( r->Type==ntLExpresion ) return 1;
      //
      r=r->pParent;
   }
   //
   return 0;
}
const char *SCompilerTree::GetNodeName()
{
   switch( Type ){
   MCaseReturnString( ntProgram          );
   MCaseReturnString( ntInclude          );
   MCaseReturnString( ntIncludeSys       );
   MCaseReturnString( ntIfDef            );  
   MCaseReturnString( ntIfnDef           );  
   MCaseReturnString( ntEndIf            );
   MCaseReturnString( ntDefine           );  
   MCaseReturnString( ntUndefine         );
   MCaseReturnString( ntOptimization     );
   //
   MCaseReturnString( ntFloatModifier    );
   MCaseReturnString( ntVectorModifier   );
   //
   MCaseReturnString( ntInput            );
   MCaseReturnString( ntOutput           );
   MCaseReturnString( ntVertexShader     );
   //
   MCaseReturnString( ntReserve          );
   //
   MCaseReturnString( ntClass            );  
   MCaseReturnString( ntClassVar         );
   MCaseReturnString( ntClassConstructor );  
   MCaseReturnString( ntClassDestructor  );  
   MCaseReturnString( ntClassPrivate     );     
   MCaseReturnString( ntClassPublic      );
   MCaseReturnString( ntClassPotected    );  
   MCaseReturnString( ntClassMethod      );     
   MCaseReturnString( ntClassDefVar      );
   //
   MCaseReturnString( ntRadical          );  
   //
   MCaseReturnString( ntPlugin           );
   MCaseReturnString( ntPluginConst      );
   //
   MCaseReturnString( ntIfGe             );
   MCaseReturnString( ntIfLs             );
   MCaseReturnString( ntIfGeElse         );
   MCaseReturnString( ntIfLsElse         );
   //                                    
   MCaseReturnString( ntFunction         );
   MCaseReturnString( ntScope            );
   MCaseReturnString( ntToken            );
   MCaseReturnString( ntIf               );
   MCaseReturnString( ntFor              );
   MCaseReturnString( ntDoWhile          );
   MCaseReturnString( ntWhile            );
   MCaseReturnString( ntSwitch           );
   MCaseReturnString( ntCase             );
   MCaseReturnString( ntDefault          );
   MCaseReturnString( ntContinue         );
   MCaseReturnString( ntBreak            );
   MCaseReturnString( ntReturn           );
   MCaseReturnString( ntThrow            );
   MCaseReturnString( ntCatch            );
   MCaseReturnString( ntExpression       );
   MCaseReturnString( ntLExpresion       );
   MCaseReturnString( ntReference        );
   MCaseReturnString( ntEmpty            );
   MCaseReturnString( ntAdd              );
   MCaseReturnString( ntSub              );
   MCaseReturnString( ntDiv              );
   MCaseReturnString( ntMod              );
   MCaseReturnString( ntMul              );
   //
   MCaseReturnString( ntExprAdd          );
   MCaseReturnString( ntExprDp3          );
   MCaseReturnString( ntExprDp4          );
   MCaseReturnString( ntExprDst          );
   MCaseReturnString( ntExprExpp         );
   MCaseReturnString( ntExprLit          );
   MCaseReturnString( ntExprLogp         );
   MCaseReturnString( ntExprMad          );
   MCaseReturnString( ntExprMax          );
   MCaseReturnString( ntExprMin          );
   MCaseReturnString( ntExprMov          );
   MCaseReturnString( ntExprMul          );
   MCaseReturnString( ntExprRcp          );
   MCaseReturnString( ntExprRsq          );
   MCaseReturnString( ntExprSge          );
   MCaseReturnString( ntExprSlt          );
   MCaseReturnString( ntExprSub          );
   MCaseReturnString( ntExprExp          );
   MCaseReturnString( ntExprFrc          );
   MCaseReturnString( ntExprLog          );
   MCaseReturnString( ntExprM3x2         );
   MCaseReturnString( ntExprM3x3         );
   MCaseReturnString( ntExprM3x4         );
   MCaseReturnString( ntExprM4x3         );
   MCaseReturnString( ntExprM4x4         );
   //
   MCaseReturnString( ntOr               );
   MCaseReturnString( ntAnd              );
   MCaseReturnString( ntXor              );
   MCaseReturnString( ntShl              );
   MCaseReturnString( ntShr              );
   MCaseReturnString( ntEq               );
   MCaseReturnString( ntNe               );
   MCaseReturnString( ntGt               );
   MCaseReturnString( ntGe               );
   MCaseReturnString( ntLt               );
   MCaseReturnString( ntLe               );
   MCaseReturnString( ntLand             );
   MCaseReturnString( ntLor              );
   MCaseReturnString( ntAssign           );
   MCaseReturnString( ntSetAdd           );
   MCaseReturnString( ntSetSub           );
   MCaseReturnString( ntSetDiv           );
   MCaseReturnString( ntSetMod           );
   MCaseReturnString( ntSetMul           );
   MCaseReturnString( ntSetOr            );
   MCaseReturnString( ntSetAnd           );
   MCaseReturnString( ntSetShl           );
   MCaseReturnString( ntSetShr           );
   MCaseReturnString( ntCond             );
   MCaseReturnString( ntRunThread        );
   MCaseReturnString( ntFuncCall         );
   MCaseReturnString( ntFuncCallRet      );
   MCaseReturnString( ntVariable         );
   MCaseReturnString( ntInputVariable    );
   MCaseReturnString( ntDefVariable      );
   MCaseReturnString( ntLParam           );
   MCaseReturnString( ntOutputLParam     );
   MCaseReturnString( ntLParamArray      );
   MCaseReturnString( ntNeg              );
   MCaseReturnString( ntPreDec           );
   MCaseReturnString( ntPreInc           );
   MCaseReturnString( ntPostDec          );
   MCaseReturnString( ntPostInc          );
   MCaseReturnString( ntNot              );
   MCaseReturnString( ntCom              );
   MCaseReturnString( ntEnv              );
   MCaseReturnString( ntArray            );
   MCaseReturnString( ntArrayConst       );
   MCaseReturnString( ntArrayAccess      );
   MCaseReturnString( ntArrayElemExpr    );
   MCaseReturnString( ntType             );
   MCaseReturnString( ntStringConst      );
   MCaseReturnString( ntIntConst         );
   MCaseReturnString( ntFloatConst       );
   MCaseReturnString( ntNullConst        );
   MCaseReturnString( ntVectorConst      );
   MCaseReturnString( ntImport           );
   MCaseReturnString( ntImportClass      );
   MCaseReturnString( ntImportMethod     );
   MCaseReturnString( ntImportDefVar     );
   MCaseReturnString( ntMethodAccess     );
   MCaseReturnString( ntStorage          );
   MCaseReturnString( ntEllipsis         ); 
   //
   MCaseReturnString( ntModifier         );
   //
   MCaseReturnString( ntAsmAdd           ); 
   MCaseReturnString( ntAsmDp3           );    
   MCaseReturnString( ntAsmDp4           ); 
   MCaseReturnString( ntAsmDst           );
   MCaseReturnString( ntAsmExpp          ); 
   MCaseReturnString( ntAsmLit           );    
   MCaseReturnString( ntAsmLogp          ); 
   MCaseReturnString( ntAsmMad           );
   MCaseReturnString( ntAsmMax           ); 
   MCaseReturnString( ntAsmMin           );    
   MCaseReturnString( ntAsmMov           ); 
   MCaseReturnString( ntAsmMul           );
   MCaseReturnString( ntAsmRcp           ); 
   MCaseReturnString( ntAsmRsq           );    
   MCaseReturnString( ntAsmSge           ); 
   MCaseReturnString( ntAsmSlt           );
   MCaseReturnString( ntAsmSub           ); 
   //
   MCaseReturnString( ntRegisterA        );
   MCaseReturnString( ntRegisterI        );
   MCaseReturnString( ntRegisterT        );
   MCaseReturnString( ntRegisterC        );
   MCaseReturnString( ntRegisterO        );
   //                                    
   MCaseReturnString( ntAsmExp           ); 
   MCaseReturnString( ntAsmFrc           );    
   MCaseReturnString( ntAsmLog           ); 
   MCaseReturnString( ntAsmM3x2          );
   MCaseReturnString( ntAsmM3x3          ); 
   MCaseReturnString( ntAsmM3x4          );    
   MCaseReturnString( ntAsmM4x3          ); 
   MCaseReturnString( ntAsmM4x4          );
   //
   default                 : return "<unknown>";
   }
}

////////////////////////////////////////////////////////////////////////
//
// SDebug
//
////////////////////////////////////////////////////////////////////////

int SDebug::AddSource(const char *c)
{
   for(int a=0;a<aSource;a++)
      if( aSource[a]==c ) return Debug.sourceidx=a;
   //
   Debug.sourceidx=aSource;
   Debug.line=1;
   //
   aSource.Add( c );
   //
   return Debug.sourceidx;
}

////////////////////////////////////////////////////////////////////////
//
// SCompilerTreeManager
//
////////////////////////////////////////////////////////////////////////

SCompilerTreeManager::SCompilerTreeManager()
{
   pRoot=0;
}

SCompilerTreeManager::~SCompilerTreeManager()
{
   while( aTree )
   {
      Delete( aTree[0] );
   }
}

void SCompilerTreeManager::Add(SCompilerTree *t)
{
   aTree.Add( t );
}

void SCompilerTreeManager::Delete(SCompilerTree *t,int deleteobject)
{
   aTree.RemoveFastFind( t );
   //
   if( deleteobject ) xdelete( t );
}

////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////

void ClearCompiler()
{
}

////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////

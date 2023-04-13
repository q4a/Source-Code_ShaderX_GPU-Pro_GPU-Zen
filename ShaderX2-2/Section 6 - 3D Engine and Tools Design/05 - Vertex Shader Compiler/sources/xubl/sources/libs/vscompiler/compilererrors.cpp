////////////////////////////////////////////////////////////////////////
//
// Compiler errors
// Created 01.05.2002
//
////////////////////////////////////////////////////////////////////////
//$XHEADER
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>

#include <globals/globals.h>

#include "compilererrors.h"

#include <core/kernel.h>

static TString FormatErrorMessage(const char *name)
{
   TString s;
   //
   s.Copy( name , 2 );
   for(int a=1;a<s.Length()-1;a++)
   {
      if( s[a-1]!=' ' &&
          s[a] >= 'A' && s[a] <= 'Z' ) //&&  
          //!(s[a+1] >= 'A' && s[a+1] <= 'Z') )
      {
         s[a]-='A'-'a';
         s.Insert( a , " ");
      }
   }

   //
   return s;
}

#define MCaseReturnString( a ) case a: return tochar( FormatErrorMessage( #a ) )

TString SError::toString()
{
   switch( Error ) {
   case 0: return "";      
   // parser errors
   MCaseReturnString( PEInvalidToken                  );
   MCaseReturnString( PEInvalidFunctionArgumentDefinition );
   MCaseReturnString( PEMissingSemicolonBefore );

   // compiler errors
   MCaseReturnString( CECannotOpenIncludeFile         );
   MCaseReturnString( CERedefinition                  );    
   MCaseReturnString( CEUndeclaredIdentifier          );    
   MCaseReturnString( CECannotConvert                 );    
   MCaseReturnString( CEMissingDefaultParameter       );   
   MCaseReturnString( CEFunctionReturningAValue       );

   // misc
   MCaseReturnString( CEModifierOnFloat               ); 
   MCaseReturnString( CEInvalidModifier               );
   MCaseReturnString( CEReturnNotLastCommand          );
   MCaseReturnString( CEReadingWriteOnlyRegister      );
   MCaseReturnString( CEWritingReadOnlyRegister       );  
   MCaseReturnString( CETooManyRegistersAccessedInInstruction );   
   MCaseReturnString( CEArrayAccessOnNonConstVariable );
   MCaseReturnString( CEOutOfRegisters                );
   //
   MCaseReturnString( CEEllipsisMustBeLastParameter   );    
   // internal errors
   MCaseReturnString( IENoVariableOnStack             );    
   MCaseReturnString( IENoJumpLabel                   );
   MCaseReturnString( IENoBreakLabel                  );
   MCaseReturnString( IENoContinueLabel               );
   MCaseReturnString( IENoLogical0Label               );
   MCaseReturnString( IENoLogical1Label               );
   MCaseReturnString( IEVariableStackEmpty            );
   //
   default:    return Format("unknown error %04x", Error ); 
   }
}

void SError::OutputError()
{
   if( !Error ) return;
   //
   TString err;
   TString line;
   //
   if( Debug.sourceidx>=0 )
   {
      line.Format("%s(%d:%d) : " , tochar( pScriptDebug->aSource[ Debug.sourceidx ] ) , 
               Debug.line , Debug.col );
   }
   //
   if( ErrorParam )
   {
      if( ErrorParam[0]=='@' )
      {
         TString s;
         s.Copy(ErrorParam,1);
         //
         err.Format("error C%04x : %s %s.\n" , Error , tochar( toString() ) , tochar( s ) );
      }
      else
      {
         err.Format("error C%04x : '%s' : %s.\n" , Error , tochar( ErrorParam ) , tochar( toString() ) );
      }
   }
   else
   {
      err.Format("error C%04x : %s.\n" , Error , tochar( toString() ) );
   }
   //
   err=line << err;
   //
   printf( err );
   //
   ShowMessage( "VSC error.\n\n" << err );
}

int SError::SetError(EScriptError err,TString param)
{
   ErrorParam=param;
   Error=err;
   //
   Debug.sourceidx=-1;
   //
   OutputError();
   //
   return Error;
}

int SError::SetError(EScriptError err,SDebugInfo deb,TString param)
{
   ErrorParam=param;
   Error=err;
   //
   Debug=deb;
   //
   OutputError();
   //
   return Error;
}

void SError::OutputWarning(SDebugInfo deb,int code,TString text)
{
   if( deb.sourceidx>=0 )
   {
      printf("%s(%d:%d) : " , tochar( pScriptDebug->aSource[ deb.sourceidx ] ) , 
               deb.line , deb.col );
   }
   //
   printf("warning C%04x : %s.\n" , code , tochar( text ) );
}






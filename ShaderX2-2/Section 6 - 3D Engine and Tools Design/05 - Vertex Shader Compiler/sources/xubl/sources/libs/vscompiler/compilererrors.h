////////////////////////////////////////////////////////////////////////
//
// Compiler errors
// Created 01.05.2002
//
////////////////////////////////////////////////////////////////////////

#ifndef __COMPILER_ERRORS__
#define __COMPILER_ERRORS__

#include "compilertree.h"

enum EScriptError {
   seNone                                 = 0x0000,
   // parser errors
   PEInvalidToken                         = 0x1000,
   PEInvalidFunctionArgumentDefinition,
   PEMissingSemicolonBefore               = 0x2143,

   // compiler errors
   CECannotOpenIncludeFile                = 0x1083,
   CERedefinition                         = 0x2086,
   CEUndeclaredIdentifier                 = 0x2065,
   CECannotConvert                        = 0x2440,
   CEMissingDefaultParameter              = 0x2548,
   CEFunctionReturningAValue              = 0x2562,
   //
   CEModifierOnFloat                      = 0x2800,   
   CEInvalidModifier                      = 0x2801,   
   CEReturnNotLastCommand                 = 0x2802,
   CEReadingWriteOnlyRegister             = 0x2803,
   CEWritingReadOnlyRegister              = 0x2804,
   CETooManyRegistersAccessedInInstruction= 0x2805,
   CEArrayAccessOnNonConstVariable        = 0x2806,
   CEOutOfRegisters                       = 0x2807,

   // xubl errors
   CEEllipsisMustBeLastParameter          = 0x3000,

   // internal errors
   IENoVariableOnStack                    = 0x8000,
   IENoJumpLabel,
   IENoBreakLabel,
   IENoContinueLabel,
   IENoLogical0Label,
   IENoLogical1Label,
   IEVariableStackEmpty,
};

class SError {
public:
   EScriptError                  Error;
   TString                       ErrorParam;
   SDebugInfo                    Debug;
public:
                                 SError()
                                 {
                                    Error=seNone;
                                 }
public:
   void                          Clear(){Error=seNone;ErrorParam="";}
   int                           SetError(EScriptError err,TString param="");
   int                           SetError(EScriptError err,SDebugInfo deb,TString param="");
   TString                       toString();
   void                          OutputError();
   //
   void                          OutputWarning(SDebugInfo deb,int code,TString param);
public:
                                 operator =(EScriptError err)
                                 {
                                    SetError( err );
                                 }
                                 operator int()
                                 {
                                    return (int)Error;
                                 }
                                 operator const char *()
                                 {
                                    return toString();
                                 }
};

#endif

////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////

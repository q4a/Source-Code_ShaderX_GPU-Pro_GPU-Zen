%{

//$XHEADER
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>

#include <malloc.h>

#include "compiler.h"

extern CCompiler      *pCompiler;

// forward definitions (because of C++)
int                     yylex();
void                    yyerror(char *err);
extern EScriptError     LastError;


int ParseInclude(const char *fname);


#define YYDEBUG 1

%}


%union {
   SParameter           *par;
   SCompilerTree        *tree;
}

%token   <tree>   IMPORT
%token   <tree>   BREAK
%token   <tree>   STATIC
%token   <tree>   REGISTER
%token   <tree>   VERTEXSHADER
%token   <tree>   INPUT
%token   <tree>   OUTPUT
%token   <par>    CONST
%token   <tree>   CASE
%token   <tree>   CATCH
%token   <tree>   CONTINUE
%token   <tree>   PLUGIN
%token   <tree>   DEFAULT
%token   <tree>   DO
%token   <tree>   ELSE
%token   <tree>   FOR
%token   <tree>   IF
%token   <par>    RADICAL
%token   <par>    RESERVE
%token   <par>    TEMP
%token   <tree>   NULLLITERAL
%token   <tree>   RETURN
%token   <tree>   THROW
%token   <tree>   TRY
%token   <tree>   SWITCH
%token   <tree>   WHILE
%token   <tree>   ELLIPSIS
%token   <tree>   ','
%token   <tree>   ';'
%token   <tree>   ')'
%token   <tree>   ']'
%token   <tree>   '{'
%token   <tree>   '}'
%token   <tree>   '#'
%token   <tree>   OPT0
%token   <tree>   OPT1
%token   <tree>   OPT2
%token   <tree>   OPT10
%token   <tree>   INCLUDE
%token   <tree>   DEFINE
%token   <tree>   UNDEFINE
%token   <tree>   DEFELSE
%token   <tree>   IFDEF
%token   <tree>   IFNDEF
%token   <tree>   ENDIF
%token   <tree>   ADD 
%token   <tree>   DP3 
%token   <tree>   DP4 
%token   <tree>   DST 
%token   <tree>   EXPP
%token   <tree>   LIT 
%token   <tree>   LOGP
%token   <tree>   MAD 
%token   <tree>   MAX 
%token   <tree>   MIN 
%token   <tree>   MOV 
%token   <tree>   MUL 
%token   <tree>   RCP 
%token   <tree>   RSQ 
%token   <tree>   SGE 
%token   <tree>   SLT 
%token   <tree>   SUB 
%token   <tree>   EXP 
%token   <tree>   FRC 
%token   <tree>   LOG 
%token   <tree>   M3X2
%token   <tree>   M3X3
%token   <tree>   M3X4
%token   <tree>   M4X3
%token   <tree>   M4X4

%token   <par>    MODIFIER
                  
%token   <tree>   CLASS
%token   <tree>   PRIVATE
%token   <tree>   PROTECTED
%token   <tree>   PUBLIC

%token   <par>    ILITERAL
%token   <par>    FLITERAL
%token   <par>    SLITERAL

%token   <par>    AREGISTER
%token   <par>    TREGISTER
%token   <par>    CREGISTER
%token   <par>    IREGISTER
%token   <par>    OREGISTER

%token   <par>    IDENT
                  CLASS_IDENT
                  

%token   <par>    VOID
%token   <par>    CHAR
%token   <par>    BYTE
%token   <par>    SHORT
%token   <par>    WORD
%token   <par>    LONG
%token   <par>    INT
%token   <par>    FLOAT
%token   <par>    VECTOR
%token   <par>    STRING
                  
%token   <par>    IVERTEX
%token   <par>    OVERTEX

%type    <par>    filename
                  
%right   <tree>   '=' SET_ADD SET_SUB SET_DIV SET_MOD SET_MUL SET_AND SET_OR SET_XOR SET_SHL SET_SHR
%left    <tree>   '?' ':'
%left    <tree>   LOR
%left    <tree>   LAND
%left    <tree>   '|'
%left    <tree>   '^'
%left    <tree>   '&'
%left    <tree>   EQ NE 
%left    <tree>   '<' LE '>' GE
%left    <tree>   SHL SHR
%left    <tree>   '+' '-'
%left    <tree>   '%' '*' '/'
%left    <tree>   '(' '[' '.'
%right   <tree>   UPLUS UMINUS '!' '~' '$' INC DEC

%type    <tree>   argument_list
%type    <tree>   not_empty_argument_list

%type    <tree>   class_argument_list
%type    <tree>   argument_array_def
%type    <tree>   not_empty_class_argument_list
%type    <tree>   class_definition                  

%type    <tree>   program 
%type    <tree>   precompiler_stmt
%type    <tree>   precompiler_ifdef
%type    <tree>   precompiler_block
%type    <tree>   root_stmt 
%type    <tree>   statements
%type    <tree>   stmt
%type    <tree>   purestatements
%type    <tree>   purestmt
                  
%type    <tree>   assembler_stmt
%type    <tree>   func_def_stmt 
%type    <tree>   input_definition 
%type    <tree>   output_definition 
%type    <tree>   expr_stmt
%type    <tree>   empty_stmt
%type    <tree>   return_stmt
                  
%type    <tree>   vectorconst

%type    <tree>   storage_type
%type    <tree>   basic_type
%type    <tree>   var_type
%type    <tree>   lparam

%type    <tree>   def_var
%type    <tree>   def_var_list
%type    <tree>   def_const_var
%type    <tree>   def_const_var_list
%type    <tree>   definition
%type    <tree>   argument
%type    <tree>   class_argument
%type    <tree>   block
                  
%type    <tree>   expr
%type    <tree>   expr_asm_funct
%type    <tree>   mad_expr
%type    <tree>   set_expr
%type    <tree>   unary_expr
%type    <tree>   array_access
%type    <tree>   literal
%type    <tree>   expr_list
%type    <tree>   not_empty_expr_list


%type    <tree>   spec_stmt         
%type    <par>    reserve_type      
%type    <tree>   reserve_param     
%type    <tree>   reserve_stmt


%type    <tree>   plugin_stmt
%type    <tree>   plugin_type
                  
%start program

%%


/***********************************************************************************/
/* Program definition **************************************************************/
/***********************************************************************************/

program           :  root_stmt                           { 
                                                            $$ = $1;
                                                            //
                                                            pCompilerTreeMng->pRoot=$$;
                                                         }
                  |  root_stmt program                   { 
                                                            $$ = $1;
                                                            $1->SetNext( $2 );
                                                            //
                                                            pCompilerTreeMng->pRoot=$$;
                                                         }

root_stmt         :  precompiler_stmt
                  |  input_definition
                  |  output_definition
                  |  plugin_stmt
                  |  func_def_stmt
                  |  block
                  |  spec_stmt

/***********************************************************************************/
/* Specific statements *************************************************************/
/***********************************************************************************/

spec_stmt         : reserve_stmt ';'

reserve_type      : TEMP                                 { $$ = pParameterMng->CreateParameter( "temp" ); }
                  | CONST                                { $$ = pParameterMng->CreateParameter( "const" ); }

reserve_param     : FLITERAL                             { $$ = xnewp( SCompilerTree , ( ntToken      , $1 ) ); }
                  | FLITERAL ',' reserve_param           { 
                                                            $$=xnewp( SCompilerTree , ( ntToken      , $1 ) );
                                                            $$->pNext=$3;
                                                         }
                  | FLITERAL '.' '.' reserve_param       { 
                                                            $$=xnewp( SCompilerTree , ( ntToken      , $1 ) );
                                                            $$->aTree[0]=$4;
                                                            $$->pNext=$4->pNext;
                                                            $4->pNext=0;
                                                         }

reserve_stmt      : RESERVE reserve_type '(' reserve_param ')'
                                                         { $$ = xnewp( SCompilerTree , ( ntReserve    , $2 , $4 ) ); }


/***********************************************************************************/
/* Function definition *************************************************************/
/***********************************************************************************/

func_def_stmt     :  var_type IDENT '(' argument_list ')' block 
                                                         { $$ = xnewp( SCompilerTree , ( ntFunction   , $2 , $1 , $4 , $6 ) ); }

argument_list     :                                      { $$ = NULL; } 
                  |  not_empty_argument_list             { $$ = $1; }


argument          :  var_type def_const_var argument_array_def
                                                         { $$ = xnewp( SCompilerTree , ( ntDefVariable, $1 , $2 , $3 ) ); }

not_empty_argument_list
                  :  argument                            
                  |  argument ',' not_empty_argument_list{ $$ = $1; $1->SetNext( $3 ); }


class_argument_list     
                  :                                      { $$ = NULL; } 
                  |  not_empty_class_argument_list       { $$ = $1; };


class_argument    :  var_type def_const_var argument_array_def
                                                         { $$ = xnewp( SCompilerTree , ( ntDefVariable, $1 , $2 , $3 ) ); }

argument_array_def:                                      { $$ = 0; }
                  |  '[' FLITERAL ']'                    { $$ = xnewp( SCompilerTree , ( ntArray , $2 ) ); }

not_empty_class_argument_list
                  :  class_argument ';'                           
                  |  class_argument ';' not_empty_class_argument_list
                                                         { $$ = $1; $1->SetNext( $3 ); }



/***********************************************************************************/
/* plugin definition ***************************************************************/
/***********************************************************************************/

plugin_type       :                                      { $$ = 0; }
                  | RADICAL                              { $$ = xnewp( SCompilerTree , ( ntRadical ) ); }

plugin_stmt       : plugin_type PLUGIN IDENT '(' argument_list ')' ';'
                                                         { 
                                                            $$ = xnewp( SCompilerTree , ( ntPlugin     , $3 , $5 , $1 ) ); 
                                                            //
                                                            SCRegisterPlugin( $3->toString() );
                                                         }

input_definition  : INPUT class_definition ';'           { $$ = xnewp( SCompilerTree , ( ntInput      , $2 ) ); }

output_definition : OUTPUT class_definition ';'          { $$ = xnewp( SCompilerTree , ( ntOutput     , $2 ) ); }

class_definition  : VERTEXSHADER '{' class_argument_list '}'
                                                         { $$ = xnewp( SCompilerTree , ( ntVertexShader , $3 ) ); }

/***********************************************************************************/
/* Statements **********************************************************************/
/***********************************************************************************/

statements        :                                      { $$ = NULL; } 
                  |  stmt statements                     { $1->SetNext( $2 ); $$ = $1; }

stmt              :  definition ';'
                  |  assembler_stmt
                  |  expr_stmt 
                  |  empty_stmt
                  |  return_stmt
                  |  precompiler_stmt
                  |  error                               {  LastError=PEMissingSemicolonBefore;
                                                            yyerror(0); 
                                                            return 1; }

purestatements    :                                      { $$ = NULL; } 
                  |  purestmt purestatements             { $1->SetNext( $2 ); $$ = $1; }

purestmt          :  definition ';'
                  |  assembler_stmt
                  |  expr_stmt 
                  |  empty_stmt
                  |  return_stmt
                  |  error                               {  LastError=PEMissingSemicolonBefore;
                                                            yyerror(0); 
                                                            return 1; }

block             :  '{' statements '}'                  { $$ = xnewp( SCompilerTree , ( ntScope      , $2 ) ); } 

return_stmt       :  RETURN expr ';'                     { $$ = xnewp( SCompilerTree , ( ntReturn     , $2 ) ); } 

empty_stmt        : ';'                                  { $$ = xnewp( SCompilerTree , ( ntEmpty         ) ); }

/***********************************************************************************/
/* Precompiler statements **********************************************************/
/***********************************************************************************/

precompiler_block : program
                  //| block
                  //| statements
                  | purestatements

precompiler_ifdef : IFDEF IDENT                          { $$ = xnewp( SCompilerTree , ( ntIfDef         , $2 ) ); }  
                  | IFNDEF IDENT                         { $$ = xnewp( SCompilerTree , ( ntIfnDef        , $2 ) ); }  

precompiler_stmt  :  INCLUDE '<' filename '>'            { 
                                                            $$ = xnewp( SCompilerTree , ( ntIncludeSys    , $3 ) ); 
                                                            ParseInclude( pCompiler->GetInclude( $$ , $3->toString() , 1 ) );
                                                         } 
                  |  INCLUDE SLITERAL                    
                                                         { 
                                                            $$ = xnewp( SCompilerTree , ( ntInclude       , $2 ) ); 
                                                            ParseInclude( pCompiler->GetInclude( $$ , $2->toString() ) );
                                                         }  
                  |  precompiler_ifdef precompiler_block ENDIF           
                                                         {  
                                                            $1->aTree[0]=$2; 
                                                            $$=$1;
                                                         }  
                  |  precompiler_ifdef precompiler_block DEFELSE precompiler_block ENDIF           
                                                         {  
                                                            $1->aTree[0]=$2; 
                                                            $1->aTree[1]=$4;
                                                            $$=$1;
                                                         }  
                  |  DEFINE IDENT                        { $$ = xnewp( SCompilerTree , ( ntDefine        , $2 ) ); }
                  |  UNDEFINE IDENT                      { $$ = xnewp( SCompilerTree , ( ntUndefine      , $2 ) ); }
                  |  OPT0                                { $$ = xnewp( SCompilerTree , ( ntOptimization  , pParameterMng->CreateParameter(  0 ) ) ); }
                  |  OPT1                                { $$ = xnewp( SCompilerTree , ( ntOptimization  , pParameterMng->CreateParameter(  1 ) ) ); }
                  |  OPT2                                { $$ = xnewp( SCompilerTree , ( ntOptimization  , pParameterMng->CreateParameter(  2 ) ) ); }
                  |  OPT10                               { $$ = xnewp( SCompilerTree , ( ntOptimization  , pParameterMng->CreateParameter( 10 ) ) ); }
                  /**/

/***********************************************************************************/
/* Assembler statements ************************************************************/
/***********************************************************************************/

assembler_stmt    :  ADD   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmAdd   , $2 , $4 , $6 ) ); }
                  |  DP3   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmDp3   , $2 , $4 , $6 ) ); }
                  |  DP4   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmDp4   , $2 , $4 , $6 ) ); }
                  |  DST   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmDst   , $2 , $4 , $6 ) ); }
                  |  EXPP  lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmExpp  , $2 , $4 ) ); }
                  |  LIT   lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmLit   , $2 , $4 ) ); }
                  |  LOGP  lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmLogp  , $2 , $4 ) ); }
                  |  MAD   lparam ',' expr ',' expr ',' expr ';'     
                                                         { $$ = xnewp( SCompilerTree , ( ntAsmMad   , $2 , $4 , $6 , $8 ) ); }
                  |  MAX   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmMax   , $2 , $4 , $6 ) ); }
                  |  MIN   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmMin   , $2 , $4 ) ); }
                  |  MOV   lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmMov   , $2 , $4 ) ); }
                  |  MUL   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmMul   , $2 , $4 , $6 ) ); }
                  |  RCP   lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmRcp   , $2 , $4 ) ); }
                  |  RSQ   lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmRsq   , $2 , $4 ) ); }
                  |  SGE   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmSge   , $2 , $4 , $6 ) ); }
                  |  SLT   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmSlt   , $2 , $4 , $6 ) ); }
                  |  SUB   lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmSub   , $2 , $4 , $6 ) ); }
                                                                                                
                  |  EXP   lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmExp   , $2 , $4  ) ); }
                  |  FRC   lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmFrc   , $2 , $4  ) ); }
                  |  LOG   lparam ',' expr ';'           { $$ = xnewp( SCompilerTree , ( ntAsmLog   , $2 , $4  ) ); }
                  |  M3X2  lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmM3x2  , $2 , $4 , $6 ) ); }
                  |  M3X3  lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmM3x3  , $2 , $4 , $6 ) ); }
                  |  M3X4  lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmM3x4  , $2 , $4 , $6 ) ); }
                  |  M4X3  lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmM4x3  , $2 , $4 , $6 ) ); }
                  |  M4X4  lparam ',' expr ',' expr ';'  { $$ = xnewp( SCompilerTree , ( ntAsmM4x4  , $2 , $4 , $6 ) ); }
                                                                              
/***********************************************************************************/
/* Expresions **********************************************************************/
/***********************************************************************************/

expr_stmt         :  set_expr ';'                        { $$ = xnewp( SCompilerTree , ( ntExpression , $1 ) ); }

                  // function call

                  |  IDENT '(' expr_list ')'             { $$ = xnewp( SCompilerTree , ( ntFuncCall   , $1, $3 ) ); }

lparam            :  IDENT                               { $$ = xnewp( SCompilerTree , ( ntLParam     , $1 ) ); }
                  |  IDENT '.' IDENT                     { 
                                                            SCompilerTree *v = xnewp( SCompilerTree   , ( ntModifier   , $3 ) ); 
                                                            $$ = xnewp( SCompilerTree , ( ntLParam    , $1 , 0 , 0 , 0 , v ) );
                                                         }
                  |  OVERTEX '.' IDENT                   { $$ = xnewp( SCompilerTree , ( ntOutputLParam  , $3 ) ); }
                  |  OVERTEX '.' IDENT '.' IDENT         { 
                                                            SCompilerTree *v = xnewp( SCompilerTree   , ( ntModifier   , $5 ) ); 
                                                            $$ = xnewp( SCompilerTree , ( ntOutputLParam  , $3 , 0 , 0 , 0 , v ) ); 
                                                         }

set_expr          :  lparam '=' expr                     { $$ = xnewp( SCompilerTree , ( ntAssign     , $1, $3 ) ); }
                  |  lparam SET_ADD expr                 { $$ = xnewp( SCompilerTree , ( ntSetAdd     , $1, $3 ) ); }
                  |  lparam SET_SUB expr                 { $$ = xnewp( SCompilerTree , ( ntSetSub     , $1, $3 ) ); }
                  |  lparam SET_MUL expr                 { $$ = xnewp( SCompilerTree , ( ntSetMul     , $1, $3 ) ); }
                  //
                  |  lparam SET_DIV expr                 { 
                                                            SCompilerTree *v=xnewp( SCompilerTree , ( ntExprRcp    , $3 ) );
                                                            $$ = xnewp( SCompilerTree , ( ntExprMul    , $1, v ) );
                                                         }


                  // assembler instructions like functions   
expr_asm_funct    :  ADD   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprAdd    ,$3 , $5 ) ); }
                  |  DP3   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprDp3    ,$3 , $5 ) ); }
                  |  DP4   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprDp4    ,$3 , $5 ) ); }
                  |  DST   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprDst    ,$3 , $5 ) ); }
                  |  EXPP  '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprExpp   ,$3      ) ); }
                  |  LIT   '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprLit    ,$3      ) ); }
                  |  LOGP  '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprLogp   ,$3      ) ); }
                  |  MAD   '(' expr ',' expr ',' expr ')'{ $$ = xnewp( SCompilerTree , ( ntExprMad    ,$3 , $5 , $7 ) ); }
                  |  MAX   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprMax    ,$3 , $5 ) ); }
                  |  MIN   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprMin    ,$3      ) ); }
                  |  MOV   '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprMov    ,$3      ) ); }
                  |  MUL   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprMul    ,$3 , $5 ) ); }
                  |  RCP   '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprRcp    ,$3      ) ); }
                  |  RSQ   '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprRsq    ,$3      ) ); }
                  |  SGE   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprSge    ,$3 , $5 ) ); }
                  |  SLT   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprSlt    ,$3 , $5 ) ); }
                  |  SUB   '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprSub    ,$3 , $5 ) ); }
                                                                                                       
                  |  EXP   '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprExp    ,$3  ) ); }
                  |  FRC   '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprFrc    ,$3  ) ); }
                  |  LOG   '(' expr ')'                  { $$ = xnewp( SCompilerTree , ( ntExprLog    ,$3  ) ); }
                  |  M3X2  '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprM3x2   ,$3 , $5 ) ); }
                  |  M3X3  '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprM3x3   ,$3 , $5 ) ); }
                  |  M3X4  '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprM3x4   ,$3 , $5 ) ); }
                  |  M4X3  '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprM4x3   ,$3 , $5 ) ); }
                  |  M4X4  '(' expr ',' expr ')'         { $$ = xnewp( SCompilerTree , ( ntExprM4x4   ,$3 , $5 ) ); }

                  // mad (multiply and add) instructions shortcuts

mad_expr          :  expr '*' expr '+' expr              { $$ = xnewp( SCompilerTree , ( ntExprMad    , $1, $3 , $5 ) ); } 
                  |  expr '+' expr '*' expr              { $$ = xnewp( SCompilerTree , ( ntExprMad    , $3, $5 , $1 ) ); } 

expr              :  unary_expr

                  //|  mad_expr

                  |  expr_asm_funct
                  |  expr_asm_funct '.' IDENT            { $$ = xnewp( SCompilerTree , ( ntVectorModifier    , $3 , $1 ) ); }
                                     
                  // instructions shortcuts
                  |  expr '+' expr                       { $$ = xnewp( SCompilerTree , ( ntExprAdd    , $1, $3 ) ); } 
                  |  expr '-' expr                       { $$ = xnewp( SCompilerTree , ( ntExprSub    , $1, $3 ) ); } 
                  |  expr '/' expr                       { 
                                                            SCompilerTree *v=xnewp( SCompilerTree , ( ntExprRcp    , $3 ) );
                                                            $$ = xnewp( SCompilerTree , ( ntExprMul    , $1, v ) );
                                                         }
                  |  expr '*' expr                       { $$ = xnewp( SCompilerTree , ( ntExprMul    , $1, $3 ) ); }
                  |  expr '%' expr                       { $$ = xnewp( SCompilerTree , ( ntExprDp3    , $1, $3 ) ); } 

                  // simple if statement

                  |  '(' expr GE  expr '?' expr ':' expr ')'
                                                         { $$ = xnewp( SCompilerTree , ( ntIfGeElse   , $2 , $4 , $6 , $8 ) ); }
                  |  '(' expr '<' expr '?' expr ':' expr ')'
                                                         { $$ = xnewp( SCompilerTree , ( ntIfLsElse   , $2 , $4 , $6 , $8 ) ); }

                  |  '(' expr GE  expr '?' expr ')'      { $$ = xnewp( SCompilerTree , ( ntIfGe       , $2 , $4 , $6 ) ); }
                  |  '(' expr '<' expr '?' expr ')'      { $$ = xnewp( SCompilerTree , ( ntIfLs       , $2 , $4 , $6 ) ); }

                  // modifier
                  |  unary_expr '.' IDENT                {
                                                            $$ = $1;
                                                            $$->aTree[3]=xnewp( SCompilerTree   , ( ntModifier   , $3 ) ); 
                                                         }

                  // function call
                  |  IDENT '(' expr_list ')'             { $$ = xnewp( SCompilerTree , ( ntFuncCallRet, $1, $3 ) ); }

unary_expr        :  IDENT array_access                  { $$ = xnewp( SCompilerTree , ( ntVariable   , $1, 0 , 0 , $2  ) ); }

                  // class access

                  |  CLASS_IDENT '.' IDENT array_access  { 
                                                            $$ = xnewp( SCompilerTree , ( ntClassVar , $3 , 0 , 0 , $4 ) ); 
                                                            $$->aTree[0] = xnewp( SCompilerTree , ( ntClass , $1 ) ); 
                                                         }
                  // input & output vertex

                  |  IVERTEX '.' IDENT                   { $$ = xnewp( SCompilerTree , ( ntInputVariable , $3 ) ); }
                  |  OVERTEX '.' IDENT                   { $$ = xnewp( SCompilerTree , ( ntOutputLParam  , $3 ) ); }

                  // expression modifiers

                  |  literal                             
                  |  '(' expr ')'                        { $$ = $2; }
                  |  FLOAT  '(' expr ')'                 { $$ = xnewp( SCompilerTree , ( ntFloatModifier , $3      ) ); }
                  |  FLOAT  '(' expr ')' '.' IDENT       { $$ = xnewp( SCompilerTree , ( ntFloatModifier , $6 , $3 ) ); }
                  |  VECTOR '(' expr ')' '.' IDENT       { $$ = xnewp( SCompilerTree , ( ntVectorModifier, $6 , $3 ) ); }
                  |  '+' expr %prec UPLUS                { $$ = $2; }
                  |  '-' expr %prec UMINUS               { $$ = xnewp( SCompilerTree , ( ntNeg        , $2 ) ); }


array_access      :                                      { $$=0;}
                  |  '[' expr ']'                        {
                                                            $$ = xnewp( SCompilerTree , ( ntArrayAccess   , $2 ) ); 
                                                         }

/***********************************************************************************/

expr_list         :                                      { $$ = NULL; }                      
                  |  not_empty_expr_list                 
                                                         
not_empty_expr_list                                      
                  :  expr                                
                  |  expr ',' not_empty_expr_list        { $1->SetNext( $3 ); $$ = $1; }

/***********************************************************************************/
/* Constants ***********************************************************************/
/***********************************************************************************/

vectorconst       :  '(' FLITERAL ',' FLITERAL ')'
                                                         {  
                                                            SCompilerTree *p1=xnewp( SCompilerTree , ( ntFloatConst , $2 ) );
                                                            SCompilerTree *p2=xnewp( SCompilerTree , ( ntFloatConst , $4 ) );
                                                            //
                                                            $$ = xnewp( SCompilerTree , ( ntVectorConst   , p1 , p2 ) ); 
                                                         }
                  |  '(' FLITERAL ',' FLITERAL ',' FLITERAL ')'
                                                         {  
                                                            SCompilerTree *p1=xnewp( SCompilerTree , ( ntFloatConst , $2 ) );
                                                            SCompilerTree *p2=xnewp( SCompilerTree , ( ntFloatConst , $4 ) );
                                                            SCompilerTree *p3=xnewp( SCompilerTree , ( ntFloatConst , $6 ) );
                                                            //
                                                            $$ = xnewp( SCompilerTree , ( ntVectorConst   , p1 , p2 , p3 ) ); 
                                                         }
                  |  '(' FLITERAL ',' FLITERAL ',' FLITERAL ',' FLITERAL ')'
                                                         {  
                                                            SCompilerTree *p1=xnewp( SCompilerTree , ( ntFloatConst , $2 ) );
                                                            SCompilerTree *p2=xnewp( SCompilerTree , ( ntFloatConst , $4 ) );
                                                            SCompilerTree *p3=xnewp( SCompilerTree , ( ntFloatConst , $6 ) );
                                                            SCompilerTree *p4=xnewp( SCompilerTree , ( ntFloatConst , $8 ) );
                                                            //
                                                            $$ = xnewp( SCompilerTree , ( ntVectorConst   , p1 , p2 , p3 , p4 ) ); 
                                                         }

literal           
                  :  ILITERAL                            { $$ = xnewp( SCompilerTree , ( ntIntConst      , $1  ) ); }
                  |  FLITERAL                            { $$ = xnewp( SCompilerTree , ( ntFloatConst    , $1  ) ); }
                  //|  SLITERAL                            { $$ = xnewp( SCompilerTree , ( ntStringConst   , $1  ) ); }
                  //|  NULLLITERAL                         { $$ = xnewp( SCompilerTree , ( ntNullConst           ) ); }
                  |  vectorconst
                                                         
/***********************************************************************************/
/* Variable definition *************************************************************/
/***********************************************************************************/

storage_type      :  REGISTER                            { $$ = xnewp( SCompilerTree , ( ntStorage       , pParameterMng->CreateParameter("register") ) ); }
                  |  CONST                               { $$ = xnewp( SCompilerTree , ( ntStorage       , pParameterMng->CreateParameter("const") ) ); }

basic_type        :  VOID                                { $$ = xnewp( SCompilerTree , ( ntType          , pParameterMng->CreateParameter("void") ) ); }
                  |  FLOAT                               { $$ = xnewp( SCompilerTree , ( ntType          , pParameterMng->CreateParameter("float") ) ); }
                  |  VECTOR                              { $$ = xnewp( SCompilerTree , ( ntType          , pParameterMng->CreateParameter("vector") ) ); }

var_type          :  basic_type

def_var           :  IDENT                               { $$ = xnewp( SCompilerTree , ( ntVariable      , $1 ) ); }
                  |  IDENT '=' expr                      { 
                                                            $$ = xnewp( SCompilerTree , ( ntVariable      , $1 ) ); 
                                                            $$->aTree[0] = xnewp( SCompilerTree , ( ntAssign  , $1 , 0 , $3 ) ); 
                                                            $$->aTree[0]->aTree[0] = xnewp( SCompilerTree , ( ntLParam  , $1 ) ); 
                                                         }

def_var_list      :  def_var
                  |  def_var ',' def_var_list            { $$=$1; $1->SetNext( $3 ); }

def_const_var     :  IDENT                               { $$ = xnewp( SCompilerTree , ( ntVariable      , $1 ) ); }
                  |  IDENT '=' literal                   { $$ = xnewp( SCompilerTree , ( ntVariable      , $1 , $3 ) ); }

def_const_var_list:  def_var
                  |  def_var ',' def_var_list            { $$=$1; $1->SetNext( $3 ); }

definition        :  var_type def_var_list               { $$ = xnewp( SCompilerTree , ( ntDefVariable   , $1 , $2 ) ); }
                  |  storage_type var_type def_var_list  { $$ = xnewp( SCompilerTree , ( ntDefVariable   , $2 , $3 , $1 ) ); }


/***********************************************************************************/
/* misc definitions ****************************************************************/
/***********************************************************************************/

filename          :  IDENT                               { $$ = $1; }
                  |  IDENT '.' filename                  { 
                                                            TString s;
                                                            s=TString( $1->toString() ) << "." << $3->toString();
                                                            xdelete( $3 );
                                                            $$->Set( tochar( s ) );
                                                         }
                  |  IDENT '/' filename                  { 
                                                            TString s;
                                                            s=TString( $1->toString() ) << "/" << $3->toString();
                                                            xdelete( $3 );
                                                            $$->Set( tochar( s ) );
                                                         }
                  |  IDENT '\\' filename                 { 
                                                            TString s;
                                                            s=TString( $1->toString() ) << "\\" << $3->toString();
                                                            xdelete( $3 );
                                                            $$->Set( tochar( s ) );
                                                         }
                  |  IDENT '-' filename                  { 
                                                            TString s;
                                                            s=TString( $1->toString() ) << "-" << $3->toString();
                                                            xdelete( $3 );
                                                            $$->Set( tochar( s ) );
                                                         }

/***********************************************************************************/

%%

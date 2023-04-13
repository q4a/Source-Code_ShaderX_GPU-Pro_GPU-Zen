%{

//$XHEADER
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>

#include "compiler.h"
#include "parser.h"

int      yycol=0;

static int SlideYYInput(char *buf,int max_size);

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) (result = SlideYYInput(buf,max_size))

void AckRead()
{
   yycol+=yyleng;
}

static const int LowerUpperConv   = 'a' - 'A';

static int stoi(char *n,int sys)
{
   int r=0;
   const char hex[]="0123456789ABCDEF";
   char *i;
   i=n;
   while(*i!=0)
   {
      char c[2];
	   c[0]=*i;
	   c[1]=0;
      //
      if( c[0] > 'Z' ) c[0]-=LowerUpperConv;
      //
      int v=(int)strstr( hex , c );
	   v-=(int)hex;
	   //
	   r=r*sys+v;
	   //
	   i++;
   }
   //
   return r;
}


#define MAX_INCLUDE_DEPTH        16

int               include_stack_idx = 0;
YY_BUFFER_STATE   include_stack[ MAX_INCLUDE_DEPTH ];
SDebugInfo        include_stack_debug[ MAX_INCLUDE_DEPTH ];
TStream           *include_stack_stream[ MAX_INCLUDE_DEPTH ];

TString           buffer;
TStream           *pStream=0;

/**/


%}

DIGIT             [0-9]
BINDIGIT          [0-1]
HEXDIGIT          [0-9a-fA-F]
                  
PMODIFIER         [xyzw]
                  
NUMBER            [0-9]+
                  
LETTER            [a-zA-Z_]
FILELETTER        [a-zA-Z_./1-9]
                  
STRING            \"[^\n\"]*\"

%%

plugin            {AckRead(); return PLUGIN           ;}
break             {AckRead(); return BREAK            ;}
input             {AckRead(); return INPUT            ;}
output            {AckRead(); return OUTPUT           ;}
in                {AckRead(); return IVERTEX          ;}
out               {AckRead(); return OVERTEX          ;}

vertexshader      {AckRead(); return VERTEXSHADER     ;}

reserve           {AckRead(); return RESERVE          ;}
temp              {AckRead(); return TEMP             ;}
case              {AckRead(); return CASE             ;}
const             {AckRead(); return CONST            ;}
radical           {AckRead(); return RADICAL          ;}
static            {AckRead(); return STATIC           ;}
register          {AckRead(); return REGISTER         ;}
catch	            {AckRead(); return CATCH	         ;}
continue	         {AckRead(); return CONTINUE	      ;}
default	         {AckRead(); return DEFAULT	         ;}
do	               {AckRead(); return DO	            ;}
else	            {AckRead(); return ELSE	            ;}
for	            {AckRead(); return FOR	            ;}
if	               {AckRead(); return IF	            ;}
import	         {AckRead(); return IMPORT	         ;}
nullliteral       {AckRead(); return NULLLITERAL      ;}
return	         {AckRead(); return RETURN	         ;}
throw	            {AckRead(); return THROW	         ;}
try	            {AckRead(); return TRY	            ;}
switch	         {AckRead(); return SWITCH	         ;}
while	            {AckRead(); return WHILE	         ;}
inc	            {AckRead(); return INC	            ;}
dec	            {AckRead(); return DEC	            ;}

void              {AckRead(); return VOID             ;}
char              {AckRead(); return CHAR             ;}
byte              {AckRead(); return BYTE             ;}
short             {AckRead(); return SHORT            ;}
word              {AckRead(); return WORD             ;}
int               {AckRead(); return INT              ;}
long              {AckRead(); return LONG             ;}
float             {AckRead(); return FLOAT            ;}
string            {AckRead(); return STRING           ;}  
vector            {AckRead(); return VECTOR           ;}  
                                                      
class             {AckRead(); return CLASS            ;}
private           {AckRead(); return PRIVATE          ;}
protected         {AckRead(); return PROTECTED        ;}
public            {AckRead(); return PUBLIC           ;}

add               {AckRead(); return ADD              ;}
dp3               {AckRead(); return DP3              ;}
dp4               {AckRead(); return DP4              ;}
dst               {AckRead(); return DST              ;}
expp              {AckRead(); return EXPP             ;}
lit               {AckRead(); return LIT              ;}
logp              {AckRead(); return LOGP             ;}
mad               {AckRead(); return MAD              ;}
max               {AckRead(); return MAX              ;}
min               {AckRead(); return MIN              ;}
mov               {AckRead(); return MOV              ;}
mul               {AckRead(); return MUL              ;}
rcp               {AckRead(); return RCP              ;}
rsq               {AckRead(); return RSQ              ;}
sge               {AckRead(); return SGE              ;}
slt               {AckRead(); return SLT              ;}
sub               {AckRead(); return SUB              ;}

exp               {AckRead(); return EXP              ;}
frc               {AckRead(); return FRC              ;}
log               {AckRead(); return LOG              ;}
m3x2              {AckRead(); return M3X2             ;}
m3x3              {AckRead(); return M3X3             ;}
m3x4              {AckRead(); return M3X4             ;}
m4x3              {AckRead(); return M4X3             ;}
m4x4              {AckRead(); return M4X4             ;}

"#include"        {AckRead(); return INCLUDE          ;}
"#define"         {AckRead(); return DEFINE           ;}
"#def"            {AckRead(); return DEFINE           ;}
"#undefine"       {AckRead(); return UNDEFINE         ;}
"#undef"          {AckRead(); return UNDEFINE         ;}
"#ifdef"          {AckRead(); return IFDEF            ;}
"#ifndef"         {AckRead(); return IFNDEF           ;}
"#endif"          {AckRead(); return ENDIF            ;}
"#else"           {AckRead(); return DEFELSE          ;}

"#O0"             {AckRead(); return OPT0             ;}
"#O1"             {AckRead(); return OPT1             ;}
"#O2"             {AckRead(); return OPT2             ;}
"#On"             {AckRead(); return OPT10            ;}
                                          
                                          
"+="              {AckRead(); return SET_ADD	         ;}
"-="              {AckRead(); return SET_SUB	         ;}
"/="              {AckRead(); return SET_DIV	         ;}
"%="	            {AckRead(); return SET_MOD	         ;}
"*="   	         {AckRead(); return SET_MUL	         ;}
"&="     	      {AckRead(); return SET_AND	         ;}
"|="    	         {AckRead(); return SET_OR	         ;}  
"^="     	      {AckRead(); return SET_XOR	         ;}
"<<="    	      {AckRead(); return SET_SHL	         ;}
">>="    	      {AckRead(); return SET_SHR	         ;}
"&&"              {AckRead(); return LAND	            ;}
"=="	            {AckRead(); return EQ	            ;}
"!="	            {AckRead(); return NE	            ;}
"<="	            {AckRead(); return LE	            ;}
">="	            {AckRead(); return GE	            ;}
"<<"              {AckRead(); return SHL              ;}
">>"              {AckRead(); return SHR              ;}
"||"              {AckRead(); return LOR	            ;}  
"++"              {AckRead(); return INC	            ;}
"--"              {AckRead(); return DEC              ;}
"..."             {AckRead(); return ELLIPSIS         ;}
"."	            {AckRead(); return yytext[0]        ;}


"..."({PMODIFIER}+)             {
                                 yylval.par=pParameterMng->CreateParameter( &yytext[1] );
                                 //
                                 AckRead();
                                 //
						            	return MODIFIER;
                              }

"a"({NUMBER})                 {
						            	//int a=atoi( &yytext[1] );
                                 //yylval.par=pParameterMng->CreateParameter( a );
                                 yylval.par=pParameterMng->CreateParameter( yytext );
                                 //
                                 AckRead();
                                 //
						            	return AREGISTER;
                              }

"r"({NUMBER})                 {
						            	//int a=atoi( &yytext[1] );
                                 //yylval.par=pParameterMng->CreateParameter( a );
                                 yylval.par=pParameterMng->CreateParameter( yytext );
                                 //
                                 AckRead();
                                 //
						            	return TREGISTER;
                              }

"c"({NUMBER})                 {
						            	//int a=atoi( &yytext[1] );
                                 //yylval.par=pParameterMng->CreateParameter( a );
                                 yylval.par=pParameterMng->CreateParameter( yytext );
                                 //
                                 AckRead();
                                 //
						            	return CREGISTER;
                              }

"v"({NUMBER})                 {
						            	//int a=atoi( &yytext[1] );
                                 //yylval.par=pParameterMng->CreateParameter( a );
                                 yylval.par=pParameterMng->CreateParameter( yytext );
                                 //
                                 AckRead();
                                 //
						            	return IREGISTER;
                              }

("-")?{DIGIT}+"."({DIGIT}+)	{
						            	double a=atof( yytext );
                                 yylval.par=pParameterMng->CreateParameter( (float)a );
                                 //
                                 AckRead();
                                 //
						            	return FLITERAL;
						            };
("-")?{DIGIT}+"."({DIGIT}*)"f"  {
                                 char  text[64];
                                 strcpy( text , yytext );
                                 text[ strlen( text )-1 ]=0;
						            	double a=atof( text );
                                 yylval.par=pParameterMng->CreateParameter( (float)a );
                                 //
                                 AckRead();
                                 //
						            	return FLITERAL;
						            };
("-")?{NUMBER}				      {
						            	int a=atoi( yytext );
                                 yylval.par=pParameterMng->CreateParameter( (float)a );
						            	//
                                 AckRead();
                                 //
						            	return FLITERAL;
						            };
                              
"0b"{BINDIGIT}+	            {
						               int a=stoi( &yytext[2] , 2 );
                                 yylval.par=pParameterMng->CreateParameter( (float)a );
                                 //
                                 AckRead();
                                 //
						            	return FLITERAL;
						            }
"0x"{HEXDIGIT}+	            {
						            	int a=stoi( &yytext[2] , 16 );
                                 yylval.par=pParameterMng->CreateParameter( (float)a );
                                 //
                                 AckRead();
                                 //
						            	return FLITERAL;
						            }

{LETTER}({LETTER}|{NUMBER})*  {
                                 yylval.par=pParameterMng->CreateParameter( yytext );
                                 AckRead();
                                 //
                                 if( SCIsPlugin( yytext ) ) return CLASS_IDENT;
                                 //
                                 //printf("[%s]\n" , yytext );
                                 //
								         return IDENT;
						            };
                              
                              
\t                            {  AckRead(); }
                              
\n|\r						         { 
                                 AckRead();
                                 yycol=1;
                                 //
                                 pScriptDebug->Debug.line++;
                              }
                              
"//"[^\n]*                    {  AckRead(); }

                              
"/*"                          {
                                 AckRead();
                                 //
                                 int c=0,lc=0;
                                 while(1)
                                 {
                                    lc=c;
                                    c=yyinput();
                                    // end of line 
                                    if( c=='\n' ) 
                                    {
                                       pScriptDebug->Debug.line++;
                                       yycol=0;
                                    }
                                    yycol++;
                                    //
                                    // end of file
                                    if( c==EOF ) break;
                                    // end of comment
                                    if( lc=='*' && c=='/' ) break;
                                 }
                                 //
                                 yycol++;
                              }
                              
" "                           { 
                                 AckRead(); 
                              }
                              
                              
.						            {
                                 AckRead();
                                 //
                                 return yytext[0];
                              };


<<EOF>>                       {
                                 if( --include_stack_idx < 0 )
                                 {
                                    yyterminate();
                                 }
                                 else
                                 {
                                    xdelete( pStream ); 
                                    pScriptDebug->Debug=include_stack_debug[ include_stack_idx ];
                                    pStream=include_stack_stream[ include_stack_idx ];
                                    yy_delete_buffer( YY_CURRENT_BUFFER );
                                    yy_switch_to_buffer( include_stack[include_stack_idx] );
                                 }                              
                              }
                              
%%                            
                              
#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)

int            yyparse();
//
int            parseerror=0;
//
EScriptError   LastError=PEInvalidToken;


void yyerror(char *err)
{
   SError Error;
   //
   pScriptDebug->Debug.col=yycol;
   Error.SetError( LastError , pScriptDebug->Debug , yytext );
   //
   parseerror=1;
}

int yywrap()
{
	return 1;
}

int SlideYYInput(char *buf,int max_size)
{
   if( !YY_CURRENT_BUFFER->yy_input_file ) 
   {
      if( pStream )
      {
         return pStream->Read( buf , max_size );
      }
      else if( buffer )
      {
         int n=min( max_size , buffer );
         //
         memcpy( buf , tochar( buffer ) , n );
         //
         buffer.Delete( 0 , n );
         //
         return n;
      }
      return 0;
   }
   //
   return fread( buf , 1 , max_size ,  YY_CURRENT_BUFFER->yy_input_file );
}

int isatty(int a)
{
   return 0;
}

int ParseInclude(const char *fname)
{
   if( !fname || fname[0]==0 ) return 1;
   //
   if ( include_stack_idx >= MAX_INCLUDE_DEPTH )
   {
      fprintf( stderr, "Includes nested too deeply" );
      return 1;
   }
   include_stack_debug  [ include_stack_idx   ]=pScriptDebug->Debug;
   include_stack        [ include_stack_idx   ]=YY_CURRENT_BUFFER;
   include_stack_stream [ include_stack_idx++ ]=pStream;
   //
   pScriptDebug->AddSource( fname );
   //
   yycol=0;
   //
   TFileStream *f=xnewp( TFileStream , ( fname ) );
   //
   if( !f->Opened() )
   {
      xdelete( f );
      return 2;
   }
   //
   pStream=f;
   //
   yy_switch_to_buffer( yy_create_buffer( 0 , YY_BUF_SIZE ) );
   //
   return 0;
}

int ParseCompileFile(const char *fname)
{
   include_stack_idx = 0;
   //
   pCompilerTreeMng->pRoot=0;
   yyrestart(0);
   //
   yyin=fopen( fname , "r" );
   if( !yyin ) return 1;
	//
   pScriptDebug->AddSource( fname );
   //
   yy_switch_to_buffer( yy_create_buffer( yyin , YY_BUF_SIZE ) );
   //
	yyparse();
   //
   if( yyin ) fclose( yyin );
   //
   return parseerror;
}

int ParseCompileBuffer(const char *buf,int size)
{
   include_stack_idx = 0;
   //
   pCompilerTreeMng->pRoot=0;
   yyrestart(0);
   //
   pScriptDebug->AddSource( "buffer input" );
   //
   yy_switch_to_buffer( yy_create_buffer( 0 , size+2 ) );
   //
   buffer=buf;
   //
	yyparse();
   //
   return parseerror;
}

void ParseRestart()
{
	yyrestart(0);
}


/**/


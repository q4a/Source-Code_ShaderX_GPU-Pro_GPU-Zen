////////////////////////////////////////////////////////////////////////
//
// Compiler code generation
// Created 24.04.2002
//
////////////////////////////////////////////////////////////////////////
static const char*   __XMEMORY_MODULE_NAME__ = __FILE__;
#include <globals/memory/xmemory.h>

#include "compiler.h"

#include <stream/stream.h>

//#define TREE_OUTPUT
//#define CODE_TREE_OUTPUT
//#define OPTIMIZATION_OUTPUT

//#define PUSHPOP_OUTPUT
//#define ASM_CODE_OUTPUT

/////////////////////////////////////////////////////////////
// globals for lex and bison
/////////////////////////////////////////////////////////////

int   ParseCompileFile(const char *fname);

/////////////////////////////////////////////////////////////
//
// CVariable
//
/////////////////////////////////////////////////////////////

TString CVariable::toString() const
{
   TString s;
   // special registers
   if( index==-2 ) return "a0.x";
   //
   if( assigned && index >=0 )
   {
      //
      switch( Storage ) {
      case vsInput   : s="v"; break;
      case vsOutput  : s="o"; break;
      case vsTemp    : s="r"; break;
      case vsConst   : s="c"; break;
      };
      //
      if( pArray ) s<<=TString("[a0.x + ") << index << "]";
              else s<<=index;
   }
   else
   {
      if( !param || ParamName=="" ) s=Name;
                               else s=ParamName;
      //
      if( Storage==vsInput    ) s="in"<<s;
      if( Storage==vsOutput   ) s="out"<<s;
   }
   //
   //if( pArray   ) s<<="[a0.x]";
   //
   if( modifier ) s<<="." << modifier;
   //
   if( neg ) s="-" << s;
   //
   return s;
}

/////////////////////////////////////////////////////////////
//
// CCompiler
//
/////////////////////////////////////////////////////////////

CCompiler::CCompiler()
{
   pAsmOutput=0;
   //
   optimization=10;
   vsversion=1000;
   //
   pActiveFunction=0;
   pActivePlugin=0;
   //
   constcount=96;
   tempcount=12;
   inputcount=16;
   //
   regperins[ vsConst ]=1;
   regperins[ vsTemp  ]=3;
   regperins[ vsInput ]=1;

   // create direct variables
   for(int a=0;a<tempcount;a++)
   {
      CVariable *v=&aGlobalVariable.Add();
      //
      v->valid=1;
      v->Name=Format( "r%d" , a );
      v->Type=vtVector;
      v->Storage=vsTemp;
      v->index=a;
   }
   for(    a=0;a<constcount;a++)
   {
      CVariable *v=&aGlobalVariable.Add();
      //
      v->valid=1;
      v->Name=Format( "c%d" , a );
      v->Type=vtVector;
      v->Storage=vsConst;
      v->index=a;
   }
   for(    a=0;a<inputcount;a++)
   {
      CVariable *v=&aGlobalVariable.Add();
      //
      v->valid=1;
      v->Name=Format( "v%d" , a );
      v->Type=vtVector;
      v->Storage=vsInput;
      v->index=a;
   }
   //
   BlockMode=vsTemp;
}

CCompiler::~CCompiler()
{
   xdelete( pAsmOutput );
}

/////////////////////////////////////////////////////////////
// Assembler output
/////////////////////////////////////////////////////////////

TString CAsmCommand::toString(int commentlevel)
{
   TString s;
   //
   switch( Type ) {
   case actInstruction: 
      s=TString("   ") << Instruction << " ";
      s.LengthRight( 10 );
      //
      if( Var[0].valid )
      {
         s<<=Var[0].toString() << " ";
         s.LengthRight( 10+12*1 );
      }
      if( Var[1].valid )
      {
         s<<="," << Var[1].toString() << " ";
         s.LengthRight( 10+12*2 );
      }
      if( Var[2].valid )
      {
         s<<="," << Var[2].toString() << " ";
         s.LengthRight( 10+12*3 );
      }
      if( Var[3].valid )
      {
         s<<="," << Var[3].toString() << " ";
      }
      if( Comment.Length() && commentlevel>=0 )
      {
         s.LengthRight( 55 );
         s<<=TString( "// ") << Comment;
      }
      break;
   case actLabel:
      break;
      s=TString( Instruction ) << ":" ;
      //
      if( Comment )
      {
         s.LengthRight( 55 );
         s<<=TString( "// ") << Comment;
      }
      break;
   case actComment:
      s=TString("   // ") << Comment;
      break;
   };
   //
   return s;
}

void CCompiler::AsmOut(const char *ins,CVariable *p0,CVariable *p1,CVariable *p2,CVariable *p3,const char *comment)
{
   CVariable ax;
   ax.index=-2;
   ax.valid=1;
   // check if any operation is array access
   if( p1 && p1->index>=0 && p1->pArray ) AsmOut2( "mov" , &ax , p1->pArray , 0 , 0 , "array access" );
   if( p2 && p2->index>=0 && p2->pArray ) AsmOut2( "mov" , &ax , p2->pArray , 0 , 0 , "array access" );
   if( p3 && p3->index>=0 && p3->pArray ) AsmOut2( "mov" , &ax , p3->pArray , 0 , 0 , "array access" );
   //
   CAsmCommand &a=aAssembler.Add();
   a.Set( ins , p0 , p1 , p2 , p3 , comment );
}

void CCompiler::AsmOut2(const char *ins,CVariable *p0,CVariable *p1,CVariable *p2,CVariable *p3,const char *comment)
{
   CAsmCommand &a=aAssembler.Add();
   a.Set( ins , p0 , p1 , p2 , p3 , comment );
}

void CCompiler::AsmOutLabel(const char *label,const char *comment)
{
   aAssembler.Add( CAsmCommand( label , comment ) );
}

void CCompiler::AsmOutComment(int level,const char *f,...)
{
   if( level > commentlevel ) return;
   //
   MRetrievePrintf( f );
   //
   aAssembler.Add( CAsmCommand( __buf ) );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

int isSameRegister(const CVariable &v0,const CVariable &v1)
{
   if( !v0.valid || !v1.valid ) return 0;
   //
   return v0.index==v1.index && v0.Storage==v1.Storage;
}

int isSameRegister2(const CVariable &v0,const CVariable &v1)
{
   if( v0.modifier=="" ) return isSameRegister( v0 , v1 );
   // full compare
   return v0.toString()==v1.toString();
}

void CCompiler::OutputAssembler()
{
   int a;
   //
   //optimization=0;
   /**/
   ////////////////////////////////////////////////////////
   //
   // optimization
   //
   ////////////////////////////////////////////////////////
   CAsmCommand    *lcom=0;
   int            ncom=0;
   //
   for(a=0;a<aAssembler;a++)
   {
      #ifdef OPTIMIZATION_OUTPUT
         if( aAssembler!=ncom )
         {
            ncom=aAssembler;
            //
            printf("Optimization\n----------------------------------------------\n");
            for(int i=0;i<aAssembler;i++)
            {
               TString s=aAssembler[i].toString( commentlevel );
               printf("%02d | %s\n" , i , tochar( s ) );
            }
         }
      #endif
      //
      CAsmCommand &com=aAssembler[a];

      // check if optimization is posible
      if( com.Type!=actInstruction ) continue;
      if( !com.Var[0].valid ) continue;
      if( com.Var[0].index==-2 ) continue;
      //
      CVariable   &var=com.Var[0];
      TString     vars=var.toString();
      int         usage=0;
      CAsmCommand *lusage=0;

      // analize code
      // find variable scope and number of commands using it
      for(int scope=a+1;scope<aAssembler;scope++)
      {
         CAsmCommand &ccom=aAssembler[scope];
         // check if only a part of register is beeing overwritten
         // in this case end search (no .x variable)
         if( var.modifier=="" && 
             isSameRegister( var , ccom.Var[0] ) && 
             ccom.Var[0].modifier!="" ) 
         {
            scope=a;
            usage=-1;
            break;
         }
         // check is register is beeing overwriten (.x variable)
         if( isSameRegister2( var , ccom.Var[0] ) ) break;

         // check if register is beeing used
         if( isSameRegister( var , ccom.Var[1] ) || 
             isSameRegister( var , ccom.Var[2] ) ||
             isSameRegister( var , ccom.Var[3] ) ) 
         {
            lusage=&aAssembler[scope];
            usage++;
         }
      }
      // check if used in last scope command
      if( scope < aAssembler )
      {
         CAsmCommand &ccom=aAssembler[scope];
         //
         if( isSameRegister( var , ccom.Var[1] ) || 
             isSameRegister( var , ccom.Var[2] ) ||
             isSameRegister( var , ccom.Var[3] ) ) 
         {
            lusage=&aAssembler[scope];
            usage++;
         }
      }
      #ifdef OPTIMIZATION_OUTPUT
         printf("OptAnalize(line %d, var '%s', scope %d, usage %d)\n" , a , tochar( vars ) , scope , usage );
      #endif


      if( usage==0 )
      {
         // delete command if not used and start analizing from start
         if( var.Storage==vsTemp )
         {
            aAssembler.Remove( a );
            a=-1;
            //
            #ifdef OPTIMIZATION_OUTPUT
               printf("Optimization not used\n" );
            #endif
         }
         //
         continue;
      }

      //////////////////////////////////////////////////////////////
      // Optimization
      //////////////////////////////////////////////////////////////

      //////////////////////////////////////////////////////////////
      // source:
      //   mov A B 
      //   *** ? A ?
      //   *** ? ? A
      // optimization:
      //   *** ? B ?
      //   *** ? ? B
      //////////////////////////////////////////////////////////////
      /**/
      if( optimization > 0 )
         if( com.Instruction=="mov" && com.Var[0].modifier=="" )
         {
            CVariable   &nvar=com.Var[1];
            //
            int canopt=1;
            //
            for(int b=a+1;b<scope;b++)
            {
               CAsmCommand &c=aAssembler[b];
               //
               if( c.Var[1].toString()==vars || 
                   c.Var[2].toString()==vars ||
                   c.Var[3].toString()==vars )
               {
                  // check if switch is valid
                  int usage=0;
                  //
                  if( c.Var[1].toString()==vars ) usage++;
                  else if( c.Var[1].valid && c.Var[1].Storage==nvar.Storage ) usage++;
                  //
                  if( c.Var[2].toString()==vars ) usage++;
                  else if( c.Var[2].valid && c.Var[2].Storage==nvar.Storage ) usage++;
                  //
                  if( c.Var[3].toString()==vars ) usage++;
                  else if( c.Var[3].valid && c.Var[3].Storage==nvar.Storage ) usage++;
                  //
                  if( c.Var[3].valid ) usage--;
                  //
                  if( usage > regperins[ nvar.Storage ] )
                  {
                     c.Comment<<= "|" << com.Comment << " (o1 failed)";
                     canopt=0;
                     //
                     break;
                  }
               }
            }
            if( canopt )
            {
               int doopt=0;
               for(b=a+1;b<scope;b++)
               {
                  CAsmCommand &c=aAssembler[b];
                  int         opt=0;
                  //
                  if( c.Var[1].toString()==vars ) {c.Var[1]=nvar; opt=1; }
                  if( c.Var[2].toString()==vars ) {c.Var[2]=nvar; opt=1; }
                  if( c.Var[3].toString()==vars ) {c.Var[3]=nvar; opt=1; }
                  //
                  if( opt ) 
                  {
                     doopt++;
                     c.Comment<<= "|" << com.Comment << " (o1)";
                  }
               }
               if( doopt )
               {
                  aAssembler.Remove( a-- );
                  //
                  #ifdef OPTIMIZATION_OUTPUT
                     printf("Optimization o1\n" );
                  #endif
               }
            }
            continue;
         }
      /**/

      //////////////////////////////////////////////////////////////
      // source:
      //   *** A  ....
      //   mov R  A
      // optimization:
      //   *** R  ....
      //////////////////////////////////////////////////////////////
      if( optimization > 0 )
         if( usage==1 && com.Instruction!="mov" && lusage->Instruction=="mov" &&
             lusage->Var[1].modifier==com.Var[0].modifier )
         {
            lusage->Instruction=com.Instruction;
            if( com.Var[0].modifier ) lusage->Var[0].modifier=com.Var[0].modifier;
            lusage->Var[1]=com.Var[1];
            lusage->Var[2]=com.Var[2];
            lusage->Var[3]=com.Var[3];
            lusage->Comment<<= "|" << com.Comment << " (o1.1)";
            //
            aAssembler.Remove( a-- );
            //
            #ifdef OPTIMIZATION_OUTPUT
               printf("Optimization o1.1\n" );
            #endif
            //
            continue;
         }
      //////////////////////////////////////////////////////////////
      // source:
      //   [0]    [1][2][3]
      //   mul    A  1  2
      //  [ins    p0 p1 p2]
      //   add    R  A  3  or  add    R  3  A
      // optimization:
      //   [0]    [1][2][3][4] 
      //   mad    R  1  2  3
      // Additional check: do we have to many constant registers accesses
      //
      //////////////////////////////////////////////////////////////
      if( optimization > 1 && lcom )
      {
         if( com.Instruction=="add" && lcom->Instruction=="mul" )
         {
                 if( com.Var[1]==lcom->Var[0] )
            {
               int c=(lcom->Var[1].Storage==vsConst?1:0) + 
                     (lcom->Var[2].Storage==vsConst?1:0) + 
                     (com.Var[2].Storage  ==vsConst?1:0);
               //
               if( c <= regperins[ vsConst ]+1 )
               {
                  lcom->Instruction="mad";
                  lcom->Var[0]=com.Var[0];
                  lcom->Var[3]=com.Var[2];
                  lcom->Comment<<="|" << com.Comment << " (o2.1)";
                  //
                  aAssembler.Remove( a-- );
                  //
                  #ifdef OPTIMIZATION_OUTPUT
                     printf("Optimization o2.1\n" );
                  #endif
                  //
                  continue;
               }
               else lcom->Comment<<=" (o2.1 failed)";
            }
            else if( com.Var[2]==lcom->Var[0] )
            {
               int c=(lcom->Var[1].Storage==vsConst?1:0) + 
                     (lcom->Var[2].Storage==vsConst?1:0) + 
                     (com.Var[1].Storage  ==vsConst?1:0);
               //
               if( c <= regperins[ vsConst ]+1 )
               {
                  lcom->Instruction="mad";
                  lcom->Var[0]=com.Var[0];
                  lcom->Var[3]=com.Var[1];
                  lcom->Comment<<="|" << com.Comment << " (o2.2)";
                  //
                  aAssembler.Remove( a-- );
                  //
                  #ifdef OPTIMIZATION_OUTPUT
                     printf("Optimization o2.2\n" );
                  #endif
                  //
                  continue;
               }
               else lcom->Comment<<=" (o2.2 failed)";
            }
         }
      }
      // 
      lcom=&com;
   }
   /**/
   ////////////////////////////////////////////////////////
   //
   // code generation
   //
   ////////////////////////////////////////////////////////
   for(a=0;a<aAssembler;a++)
   {
      TString s=aAssembler[a].toString( commentlevel );
      //
      #ifdef ASM_CODE_OUTPUT
         printf("| %s\n" , tochar( s ) );
      #endif
      //
      pAsmOutput->WriteString( s );
      pAsmOutput->WriteString( "\n" );
   }
}

////////////////////////////////////////////////////////////////////////
// Precompiler
////////////////////////////////////////////////////////////////////////

void CCompiler::AddDefine(const char *c)
{
   aDefine.Add( c );
}

void CCompiler::RemoveDefine(const char *c)
{
   if( !Defined(c) ) return;
   //
   for(int a=0;a<aDefine;a++)
      if( aDefine[a]==c )
      {
         aDefine.RemoveFast( a );
         return;
      }
}

int CCompiler::Defined(const char *c)
{
   for(int a=0;a<aDefine;a++)
      if( aDefine[a]==c ) return 1;
   //
   return 0;
}

/////////////////////////////////////////////////////////////
// Function
/////////////////////////////////////////////////////////////

void CCompiler::CreateFunction(const char *name,const char *type,SCompilerTree *t)
{
   if( FindFunction( name ) )
   {
      if(t) Error.SetError( CERedefinition , t->Debug , name );
       else Error.SetError( CERedefinition , name );
      return;
   }
   //
   //AsmOutComment("CreateFunction( %s )" , name );
   //
   CFunction *f=&aFunction.Add();
   //
   f->Name=name;
   f->pTree=t;
   //
   TName atype( type );
   //
        if( atype=="float"    ) f->Type=vtFloat;
   else if( atype=="vector"   ) f->Type=vtVector;
   else if( atype=="void"     ) f->Type=vtVoid;
   else if(t) Error.SetError( CEUndeclaredIdentifier , t->Debug , type );
         else Error.SetError( CEUndeclaredIdentifier , type );
   //
   pActiveFunction=f;
   // add parameters
   for(SCompilerTree *param=t->aTree[1];param;param=param->pNext)
   {
      //printf("PARAM %s\n" , tochar( param->aTree[1]->pParameter->toString() ) );
      //
      CreateVariable(param->aTree[1]->pParameter->toString() , 
                     param->aTree[0]->pParameter->toString() ,
                     vsTemp ,
                     param , 1 );
   }
}

CFunction *CCompiler::FindFunction(const char *name)
{
   // function rename
   for(int a=0;a<aFunctionRename;a+=2)
      if( name==aFunctionRename[a] ) 
      {
         name=aFunctionRename[a+1];
         break;
      }
   //
   for(a=0;a<aFunction;a++)
      if( aFunction[a].Name==name ) return &aFunction[a];
   //
   return 0;
}

/////////////////////////////////////////////////////////////
// Variable
/////////////////////////////////////////////////////////////

CVariable *CCompiler::CreateVariable(const char *name,const char *type,EVariableStorage storage,SCompilerTree *t,int functparam)
{
   if( FindVariable( name ) )
   {
      if(t) Error.SetError( CERedefinition , t->Debug , name );
       else Error.SetError( CERedefinition , name );
      return 0;
   }
   //
   //printf("CreateVariable( %s : %s S%d)\n" , name , type , storage );
   //
   CVariable *v=0;
   if( pActivePlugin )
   {
      v=&pActivePlugin->aParam.Add();
   }
   else
   {
      if( pActiveFunction ) 
      {
         if( !functparam ) 
         {
            v=&pActiveFunction->aVariable.Add();
         }
         else 
        {
           v=&pActiveFunction->aParam.Add();
           v->assigned=1;
        }
      }
      else 
      {
         v=&aGlobalVariable.Add();
      }
   }
   //
   v->Name=name;
   //
   TName atype( type );
   //
        if( atype=="float"    ) v->Type=vtFloat;
   else if( atype=="vector"   ) v->Type=vtVector;
   else if(t) Error.SetError( CEUndeclaredIdentifier , t->Debug , name );
         else Error.SetError( CEUndeclaredIdentifier , name );
   //
   if( functparam ) v->param=1;
   //
   v->valid=1;
   v->Storage=storage;
   v->index=-1;
   //
   if( storage==vsConst || storage==vsInput || storage==vsOutput ) 
      v->assigned=1;
   /**/
   return v;
}

CVariable *CCompiler::FindVariable(const TString &name,const TString &classname)
{
   // plugin variables
   if( classname )
   {
      CPlugin *p=0;
      // find plugin
      for(int a=0;a<aPlugin;a++)
         if( aPlugin[a].Name==classname ) 
         {
            p=&aPlugin[a];
            break;
         }
      if( !p ) return 0;
      //
      p->used=1;

      // find plugin variable
      for(a=0;a<p->aParam;a++)
         if(p->aParam[a].Name==name) 
         {
            p->aParam[a].used=1;
            return &p->aParam[a];
         }
      //
      return 0;
   }

   // global variables
   for(int a=0;a<aGlobalVariable;a++)
      if(aGlobalVariable[a].Name==name) return &aGlobalVariable[a];

   // function 
   if( !pActiveFunction ) return 0;

   // function parameters
   for(    a=0;a<pActiveFunction->aParam;a++)
      if( pActiveFunction->aParam[a].Name==name ) return &pActiveFunction->aParam[a];

   // function locals
   for(    a=0;a<pActiveFunction->aVariable;a++)
      if( pActiveFunction->aVariable[a].Name==name ) return &pActiveFunction->aVariable[a];
   //
   return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//
// Analize Code 
//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define MReserveConst( c ) { \
   AsmOutComment( 2 , "Reservation: c%d" , c );\
   if( c<0 || c >= constcount )\
   {Error.SetError( CEUndeclaredIdentifier , t->Debug  , "c[x] reservation" );\
   }else\
   {aConstUsed[ int( c )*4+0 ]=2;\
   aConstUsed[ int( c )*4+1 ]=2;\
   aConstUsed[ int( c )*4+2 ]=2;\
   aConstUsed[ int( c )*4+3 ]=2;}}
//
#define MReserveTemp( c ) { \
   AsmOutComment( 2 , "Reservation: r%d" , c );\
   if( c<0 || c >= tempcount )\
   {Error.SetError( CEUndeclaredIdentifier , t->Debug , "r[x] reservation" );\
   }else\
   {aTempUsed[ int( c )*4+0 ]=2;\
   aTempUsed[ int( c )*4+1 ]=2;\
   aTempUsed[ int( c )*4+2 ]=2;\
   aTempUsed[ int( c )*4+3 ]=2;}}


void CCompiler::AnalizeCode( SCompilerTree *t , int num , int level, int active)
{
   if( !t ) return;
   //
   int      a;
   int      recurse=1;
   int      donext=1;
   //
   //if( !active ) return;
   if( Error ) return;

   /**/
   #ifdef TREE_OUTPUT
      // DEBUG
      //if( t->Debug.line >=0 ) printf("debug line %d:%d\n" , t->Debug.line , t->Debug.col );
      //
      for(a=0;a<level;a++) printf("   ");
      //
      if( num >=0 )
         printf( "%s[%d]%s" , active?" ":"~" ,num , t->GetNodeName() );
      else
         printf( "%s[-]%s" , active?" ":"~" , t->GetNodeName() );
      //
      //printf("   P(%s)" , t->pParent?t->pParent->GetNodeName():"no parent" );
      //
      if( t->pParameter )
      {
         const char *a=t->pParameter->toString();
         //
         printf(" [%s:%s]" , t->pParameter->GetType() , a );
      }
      printf("\n");

      // if not active only output
      if( !active )
      {
         for(a=0;a<4;a++)
            AnalizeCode( t->aTree[a] , a , level+1 , 0 );
         AnalizeCode( t->pNext , -1 , level , 0 );
         //
         return;
      }
   #endif
   /**/
   if( !active ) return;
   //
   ////////////////////////////////////////////
   // open scope (if needed)
   ////////////////////////////////////////////
   switch( t->Type ) {
   /////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
   case ntIfDef:
      if( Defined( t->pParameter->toString() ) ) 
      {
         AnalizeCode( t->aTree[0] );
         // disable else
         t->aTree[1]=0;
      }
      else 
      {
         // disable if
         t->aTree[0]=0;
      }
      recurse=0;
      //
      break;
   case ntIfnDef:
      if( !Defined( t->pParameter->toString() ) ) 
      {
         AnalizeCode( t->aTree[0] );
         // disable else
         t->aTree[1]=0;
      }
      else 
      {
         // disable if
         t->aTree[0]=0;
      }
      //
      recurse=0;
      //
      break;
   case ntDefine:
      if( Defined( t->pParameter->toString() ) )
         Error.OutputWarning( t->Debug , 0x4000 , TString("define '") << t->pParameter->toString() << "' already defined" );
      AddDefine( t->pParameter->toString() );
      break;
   case ntUndefine:
      if( !Defined( t->pParameter->toString() ) )
         Error.OutputWarning( t->Debug , 0x4000 , TString("define '") << t->pParameter->toString() << "' not defined" );
      RemoveDefine( t->pParameter->toString() );
      break;
   /////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
   case ntVertexShader : 
      if( t->pParent->Type==ntInput  ) 
      {
         BlockMode=vsInput;
         //
         pActiveFunction=&InputVertex;
      }
      if( t->pParent->Type==ntOutput ) 
      {
         BlockMode=vsOutput;
         //
         pActiveFunction=&OutputVertex;
      }
      //
      break;
   case ntPlugin : 
      {
         CPlugin &p=aPlugin.Add();
         //
         p.Name=t->pParameter->toString();
         p.used=0;
         //
         if( t->aTree[1] && t->aTree[1]->Type==ntRadical ) p.radical=1;
         //
         BlockMode=vsConst;
         //
         pActiveFunction=0;
         pActivePlugin=&p;
      }
      break;
      //
   case ntFunction : 
      {
         CreateFunction( t->pParameter->toString() , t->aTree[0]->pParameter->toString() , t );
         //
         AnalizeCode( t->aTree[2] );
         //
         recurse=0;
      }
      break;
   }
   ////////////////////////////////////////////
   // Pre variables
   ////////////////////////////////////////////
   /**/
   switch( t->Type ) {
   /*
   case ntFuncCall : 
   case ntFuncCallRet : 
      {
         CFunction *f=FindFunction( t->pParameter->toString() );
         //
         if( f ) 
         {
            AnalizeCode( f->pTree->aTree[1] );
            AnalizeCode( f->pTree->aTree[2] );
         }
      }
      break;
   /**/
   case ntReserve:
      {
         int temp=0;
         //
         /**/
         TString s=t->pParameter->toString();
         temp=s=="temp";

         //
         for(SCompilerTree *n=t->aTree[0];n;n=n->pNext)
         {
            int b=int( (float&)n->pParameter->param );
            if( n->aTree[0] )
            {
               for(int a=b;a<=(float&)n->aTree[0]->pParameter->param;a++)
                  if( !temp ) MReserveConst( a )
                         else MReserveTemp( a );
            }
            else
            {
               if( !temp ) MReserveConst( b )
                      else MReserveTemp( b );
            }
         }
         /**/
         recurse=0;
      }
   case ntDefVariable: 
      {
         for(SCompilerTree *n=t->aTree[1];n;n=n->pNext)
         {
            // create variable
            //CVariable *var=CreateVariable( t , n->pParameter->toString() , t->aTree[0]->pParameter->toString() );
            CVariable *var=0;
            //
            var=CreateVariable(  n->pParameter->toString() , 
                                 t->aTree[0]->pParameter->toString() , 
                                 BlockMode , 
                                 t );
            //
            if( !var ) continue;

            // check if we have initialization
            if( n->aTree[0] ) 
            {
               //var->used=1;
               var->assigned=1;
               //
               AnalizeCode( n->aTree[0] , 0 , level+3 );
            }
            // check if we have array 
            if( t->aTree[2] && t->aTree[2]->Type==ntArray )
            {
               var->arraysize=int( (float&)t->aTree[2]->pParameter->param );
            }
         }
         //
         recurse=0;
      }
      break;
   case ntClassVar: 
   case ntLParam:
      {
         // validate variable
         //CVariable *var=FindVariable( t->pParameter->toString() );
         GetVariable( t , 1 );
         //
         if( pGetVariable )
         {
            pGetVariable->used=1;
            pGetVariable->assigned=1;
         }
         else
         {
            Error.SetError( CEUndeclaredIdentifier , t->Debug , t->pParameter->toString() );
            return;
         }
      }
      break;
   case ntVariable: 
      {
         // validate variable
         //CVariable *var=FindVariable( t->pParameter->toString() );
         GetVariable( t , 1 );
         //
         if( pGetVariable )
         {
            if( !pGetVariable->assigned )
            {
               //printf("Warning: Variable %s unassigned.\n" , tochar( t->pParameter->toString() ) );
               Error.OutputWarning( t->Debug , 0x4700 , "local variable '" << pGetVariable->Name << "' used without having been initialized" );
            }
            //
            pGetVariable->assigned=1;
            pGetVariable->used=1;
         }
         else
         {
            Error.SetError( CEUndeclaredIdentifier , t->Debug , t->pParameter->toString() );
            return;
         }
      }
      break;
   }
   /**/

   ////////////////////////////////////////////
   // recurse on
   ////////////////////////////////////////////
   if( recurse )
   {
      for(a=0;a<4;a++)
         AnalizeCode( t->aTree[a] , a , level+1 );
   }
   else
   {
      for(a=0;a<4;a++)
         AnalizeCode( t->aTree[a] , a , level+1 , 0 );
   }

   ////////////////////////////////////////////
   // assignment usage
   ////////////////////////////////////////////
   switch( t->Type ) {
   case ntAssign:
   case ntSetAdd:
   case ntSetSub:
   case ntSetDiv:
   case ntSetMul:
      {
         // validate variable
         //CVariable *var=FindVariable( t->aTree[0]->pParameter->toString() );
         GetVariable( t->aTree[0] , 1 );
         //
         if( pGetVariable )
         {
            pGetVariable->used=1;
         }
         else
         {
            Error.SetError( CEUndeclaredIdentifier , t->Debug , t->aTree[0]->pParameter->toString() );
         }
      }
      break;
   // Close block
   case ntVertexShader : 
   case ntPlugin : 
      BlockMode=vsTemp;
      pActivePlugin=0;
      break;
   }

   ////////////////////////////////////////////
   // siblings
   ////////////////////////////////////////////
   AnalizeCode( t->pNext , -1 , level );
}

void CCompiler::VariableUsage( SCompilerTree *t )
{
   if( !t ) return;
   //
   int      a;
   //
   if( Error ) return;

   ////////////////////////////////////////////
   // Pre variables
   ////////////////////////////////////////////
   /**/
   switch( t->Type ) {
   /*
   case ntDefVariable: 
      {
         for(SCompilerTree *n=t->aTree[1];n;n=n->pNext)
         {
            // create variable
            CVariable *var=FindVariable( n->pParameter->toString() , t->aTree[0]->pParameter->toString() );
            //
            if( !var ) continue;
         }
      }
      break;
   /**/
   case ntFuncCall : 
   case ntFuncCallRet : 
      {
         CFunction *f=FindFunction( t->pParameter->toString() );
         //
         if( f ) 
         {
            VariableUsage( f->pTree->aTree[1] );
            VariableUsage( f->pTree->aTree[2] );
         }
      }
      break;
   case ntClassVar: 
   case ntLParam:
   case ntVariable: 
      {
         // validate variable
         GetVariable( t , 1 );
         //
         if( pGetVariable )
         {
            pGetVariable->used=1;
            pGetVariable->assigned=1;
         }
      }
   }
   /**/

   ////////////////////////////////////////////
   // recurse on
   ////////////////////////////////////////////
   for(a=0;a<4;a++)
      VariableUsage( t->aTree[a] );

   /*
   ////////////////////////////////////////////
   // assignment usage
   ////////////////////////////////////////////
   switch( t->Type ) {
   case ntAssign:
   case ntSetAdd:
   case ntSetSub:
   case ntSetDiv:
   case ntSetMul:
      {
         // validate variable
         //CVariable *var=FindVariable( t->aTree[0]->pParameter->toString() );
         GetVariable( t->aTree[0] , 1 );
         //
         if( pGetVariable ) pGetVariable->used=1;
      }
      break;
   }
   /**/

   ////////////////////////////////////////////
   // siblings
   ////////////////////////////////////////////
   VariableUsage( t->pNext );
}




/////////////////////////////////////////////////////////////
// Register stack
/////////////////////////////////////////////////////////////

void CCompiler::RegisterPush(CVariable *v , int temp )
{
   #ifdef PUSHPOP_OUTPUT
      printf("   -RegisterPush(%d) '%s'\n" , (int) aVariableStack + 1 , tochar( v->toString() ) );
   #endif

   // find empty register
   aVariableStack.Add( CVariableStack( *v , temp ) );
}

CVariable CCompiler::RegisterPush()
{
   // find empty register
   CVariable v=AddTemp4();
   //
   #ifdef PUSHPOP_OUTPUT
      printf("   -RegisterPush(%d) '%s'\n" , (int) aVariableStack + 1 , tochar( v.toString() ) );
   #endif
   //
   aVariableStack.Add( CVariableStack( v , 1 ) );
   //
   v.assigned=1;
   v.Name="push";
   //
   return v;
}

CVariable CCompiler::RegisterPop(int takeoff,int idx)
{
   if( aVariableStack==0 ) 
   {
      printf("Variable stack empty!!!\n");
   }
   //printf("RegisterPop\n");
   // find empty register
   CVariableStack s;
   //
   if( takeoff )
   {
      aVariableStack.Pop( s );
      // if temp then free register
      if( s.temp ) FreeTemp( s.Var );
   }
   else
   {
      s=aVariableStack[ aVariableStack-1-idx ];
   }
   //
   #ifdef PUSHPOP_OUTPUT
      printf("   -RegisterPop (%d) '%s'\n" , (int) aVariableStack , tochar( s.Var.toString() ) );
   #endif
   //
   return s.Var;
}

/////////////////////////////////////////////////////////////
// Temporary registers
/////////////////////////////////////////////////////////////

CVariable CCompiler::AddTemp1()
{
   ///////////////////////////////////////
   // find fist empty temp register
   ///////////////////////////////////////
   int n=-1;
   for(int a=0;a<constcount*4;a++)
      if( !aTempUsed[a] )
      {
         // first empty register component
         n=a;
         break;
      }
   ///////////////////////////////////////
   // if not, reserve new register
   ///////////////////////////////////////
   if( n==-1 )
   {
      printf("no more empty constant registers.");

   }
   // reserve constant
   aTempUsed[n]=1;

   ///////////////////////////////////////
   // create variable
   ///////////////////////////////////////
   CVariable v;
   //
   v.Name="temp1";
   v.assigned=1;
   //
   v.valid=1;
   v.Storage=vsTemp;
   v.Type=vtFloat;
   v.index=n / 4;
   //
   switch( n % 4 ) {
   case 0: v.modifier="x"; break;
   case 1: v.modifier="y"; break;
   case 2: v.modifier="z"; break;
   case 3: v.modifier="w"; break;
   }
   //
   return v;
}

CVariable CCompiler::AddTemp4()
{
   ///////////////////////////////////////
   // find fist empty temp register
   ///////////////////////////////////////
   int n=-1;
   for(int a=0;a<constcount*4;a+=4)
      // first empty register
      if( !aTempUsed[a] && !aTempUsed[a+1] && !aTempUsed[a+2] && !aTempUsed[a+3])
      {
         n=a;
         break;
      }

   ///////////////////////////////////////
   // if not, reserve new register
   ///////////////////////////////////////
   if( n==-1 )
   {
      printf("no more empty temp registers.");
   }
   // reserve temp register
   aTempUsed[n  ]=1;
   aTempUsed[n+1]=1;
   aTempUsed[n+2]=1;
   aTempUsed[n+3]=1;

   ///////////////////////////////////////
   // create variable
   ///////////////////////////////////////
   CVariable v;
   //
   v.Name="temp4";
   v.assigned=1;
   //
   v.valid=1;
   v.Storage=vsTemp;
   v.Type=vtVector;
   v.index=n/4;
   //
   return v;
}

void CCompiler::FreeTemp( const CVariable &v )
{
   if( v.Storage!=vsTemp ) return;
   //
   TString s=v.modifier;
   //
   if( s=="" ) s="xyzw";
   //
   if( s.Pos("x")>=0 ) aTempUsed[ v.index*4+0 ]=0;
   if( s.Pos("y")>=0 ) aTempUsed[ v.index*4+1 ]=0;
   if( s.Pos("z")>=0 ) aTempUsed[ v.index*4+2 ]=0;
   if( s.Pos("w")>=0 ) aTempUsed[ v.index*4+3 ]=0;
}

/////////////////////////////////////////////////////////////
// Constants registers
/////////////////////////////////////////////////////////////

CVariable CCompiler::AddConstant1(float c,int canbeused)
{
   ///////////////////////////////////////
   // check if some constant has the same value
   ///////////////////////////////////////
   int n=-1,fe=-1;
   for(int a=0;a<constcount*4;a++)
      if( aConstUsed[a] )
      {
         if( aConst[a]==c && aConstUsed[a]==1 && canbeused ) 
         {
            n=a;
            break;
         }
      }
      else
      {
         // first empty register component
         if( fe==-1 ) fe=a;
      }
   ///////////////////////////////////////
   // if not, reserve new register
   ///////////////////////////////////////
   if( n==-1 )
   {
      if( fe==-1 ) 
      {
         printf("no more empty constant registers.");
      }
      //
      n=fe;

      // reserve constant
      aConst[n]=c;
   }
   if( canbeused ) aConstUsed[n]=1;
              else aConstUsed[n]=2;

   ///////////////////////////////////////
   // create variable
   ///////////////////////////////////////
   CVariable v;
   //
   v.Name="const1";
   v.assigned=1;
   //
   v.Storage=vsConst;
   v.Type=vtFloat;
   v.index=n / 4;
   //
   switch( n % 4 ) {
   case 0: v.modifier="x"; break;
   case 1: v.modifier="y"; break;
   case 2: v.modifier="z"; break;
   case 3: v.modifier="w"; break;
   }
   //
   v.valid=1;
   //
   return v;
}

CVariable CCompiler::AddConstant4(float x,float y,float z,float w,int canbeused)
{
   ///////////////////////////////////////
   // check if some constant has the same value
   ///////////////////////////////////////
   int n=-1,fe=-1;
   for(int a=0;a<constcount*4;a+=4)
   {
      if( canbeused && aConstUsed[a]==1 && aConstUsed[a+1]==1 && aConstUsed[a+2]==1 && aConstUsed[a+3]==1 )
      {
         if( aConst[a]==x && aConst[a+1]==y && aConst[a+2]==z && aConst[a+3]==w ) 
         {
            n=a;
            break;
         }
      }
      // first empty register
      if( !aConstUsed[a] && !aConstUsed[a+1] && !aConstUsed[a+2] && !aConstUsed[a+3])
      {
         if( fe==-1 ) fe=a;
      }
   }
   ///////////////////////////////////////
   // if not, reserve new register
   ///////////////////////////////////////
   if( n==-1 )
   {
      if( fe==-1 ) 
      {
         printf("no more empty constant registers.");
      }
      //
      n=fe;

      int r=1;
      if( !canbeused ) r=2;
      // reserve constant
      aConstUsed[n  ]=r;
      aConstUsed[n+1]=r;
      aConstUsed[n+2]=r;
      aConstUsed[n+3]=r;
      aConst[n  ]=x;
      aConst[n+1]=y;
      aConst[n+2]=z;
      aConst[n+3]=w;
   }

   ///////////////////////////////////////
   // create variable
   ///////////////////////////////////////
   CVariable v;
   //
   v.Name="const4";
   v.assigned=1;
   //
   v.Storage=vsConst;
   v.Type=vtVector;
   v.index=n/4;
   //
   v.valid=1;
   //
   return v;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Expressions
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void ValidateModifier( const TString &mod , SError &Error , SCompilerTree *t)
{
   if( mod.Length()<1 || mod.Length() >4 ) 
      Error.SetError( CEInvalidModifier , t->Debug , mod );
   //
   for(int a=0;a<mod;a++)
   {
      const char c=mod(a);
      //
      if( c!='x' && c!='y' && c!='z' && c!='w' ) 
         Error.SetError( CEInvalidModifier , t->Debug , mod );
   }
}

void CCompiler::CheckModifier(CVariable &v,SCompilerTree *t)
{
   if( !t || !t->pParent ) return;
   // check for modifier
   if( t->pParent->Type==ntFloatModifier ) 
   {
      if( v.Type!=vtVector )
      {
         Error.SetError( CECannotConvert , t->Debug , "float to vector");
         return;
      }
      //
      if( !t->pParent->pParameter ) v.modifier="x";
                               else t->pParent->pParameter->toString();
      //
      ValidateModifier( v.modifier , Error , t );
      if( v.modifier.Length() != 1 ) 
         Error.SetError( CEInvalidModifier , t->Debug , v.modifier );
      //
      v.Type=vtFloat;
   }
   //
   if( t->pParent->Type==ntVectorModifier ) 
   {
      v.modifier=t->pParent->pParameter->toString();
      //
      ValidateModifier( v.modifier , Error , t );
      //
      if( v.modifier.Length()==1 ) v.Type=vtFloat;
                              else v.Type=vtVector;
   }
}

int CCompiler::IsFloatModifier(SCompilerTree *t)
{
   if( !t || !t->pParent ) return 0;
   // check for modifier
   if( t->pParent->Type==ntFloatModifier || t->pParent->Type==ntVectorModifier ) 
   {
      TString s;
      //
      if( !t->pParent->pParameter) s="x";
                              else s=t->pParent->pParameter->toString();
      //
      if( s.Length() == 1 ) return 1;
   }
   return 0;
}

void CCompiler::AnalizeRegisterUsage(SCompilerTree *t,CVariable *res,CVariable *v0,CVariable *v1,CVariable *v2,CVariable *v3)
{
   // check read/write usage
   if(       v0->Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v0->toString() );
   if( v1 && v1->Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v1->toString() );
   if( v2 && v2->Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v2->toString() );
   //
   if( res->Storage==vsConst || res->Storage==vsInput ) Error.SetError( CEWritingReadOnlyRegister , t->Debug , v2->toString() );
   // count usage
   int   storageusage[5]={0,0,0,0,0};
   /**/
   // mad can have one more register
   if( v2!=0 )
      storageusage[ vsConst ]=-1;
   //
   storageusage[ v0->Storage ]++;
   //
   if( v1 ) 
   {
      storageusage[ v1->Storage ]++;
      if( storageusage[ v1->Storage ] > regperins[ v1->Storage ] ) 
      {
         CVariable   n;
         if( v1->Type==vtFloat ) n=AddTemp1();
                            else n=AddTemp4();
         //
         AsmOut( "mov" , &n , v1 , 0 , 0 , "max reg per ins. salvage" );
         //
         *v1=n;
         //
         v1->templevel=3;
         //FreeTemp( n );
      }
   }
   if( v2 ) 
   {
      storageusage[ v2->Storage ]++;
      if( storageusage[ v2->Storage ] > regperins[ v2->Storage ] ) 
      {
         CVariable   n;
         if( v2->Type==vtFloat ) n=AddTemp1();
                            else n=AddTemp4();
         //
         AsmOut( "mov" , &n , v2 , 0 , 0 , "max reg per ins. salvage" );
         //
         *v2=n;
         //
         v1->templevel=3;
         //
         //FreeTemp( n );
      }
   }
   //
   /*
   if( storageusage[ vsConst  ] > regperins[ vsConst  ] ) Error.SetError( CETooManyRegistersAccessedInInstruction, t->Debug , "constant register" );
   if( storageusage[ vsTemp   ] > regperins[ vsTemp   ] ) Error.SetError( CETooManyRegistersAccessedInInstruction, t->Debug , "temporary register" );
   if( storageusage[ vsInput  ] > regperins[ vsInput  ] ) Error.SetError( CETooManyRegistersAccessedInInstruction, t->Debug , "input register" );
   /**/
}

void CCompiler::Expression4(const char *ins,SCompilerTree *t)
{
   CVariable v2=RegisterPop();
   CVariable v1=RegisterPop();
   CVariable v0=RegisterPop();
   CVariable r;
   //
   AnalizeRegisterUsage( t , &r , &v0 , &v1 , &v2 );
   //
   if( ( v0.Type==vtFloat && v1.Type==vtFloat && v2.Type==vtFloat ) || IsFloatModifier( t ) )
      r=AddTemp1();
   else 
   {
      r=AddTemp4();
      //
      CheckModifier( r , t );
   }
   //
   RegisterPush( &r , 1 );
   //
   AsmOut( ins , &r , &v0 , &v1 , &v2 );
   //
   if( v0.templevel==3 ) FreeTemp( v0 );
   if( v1.templevel==3 ) FreeTemp( v1 );
   if( v2.templevel==3 ) FreeTemp( v2 );
}

void CCompiler::Expression3(const char *ins,SCompilerTree *t)
{
   CVariable v1=RegisterPop();
   CVariable v0=RegisterPop();
   CVariable r;
   //
   AnalizeRegisterUsage( t , &r , &v0 , &v1 );
   //
   if( ( v0.Type==vtFloat && v1.Type==vtFloat ) || IsFloatModifier( t ) )
      r=AddTemp1();
   else 
   {
      r=AddTemp4();
      //
      CheckModifier( r , t );
   }
   //
   RegisterPush( &r , 1 );
   //
   AsmOut( ins , &r , &v0 , &v1 );
   //
   if( v0.templevel==3 ) FreeTemp( v0 );
   if( v1.templevel==3 ) FreeTemp( v1 );
}

void CCompiler::Expression2(const char *ins,SCompilerTree *t)
{
   CVariable v0=RegisterPop();
   CVariable r;
   //
   AnalizeRegisterUsage( t , &r , &v0 );
   //
   if( v0.Type==vtFloat || IsFloatModifier( t ) ) 
      r=AddTemp1();
   else 
   {
      r=AddTemp4();
      //
      CheckModifier( r , t );
   }
   //
   RegisterPush( &r , 1 );
   //
   AsmOut( ins , &r , &v0 );
   //
   if( v0.templevel==3 ) FreeTemp( v0 );
}

void CCompiler::AssignExpression(const char *ins,SCompilerTree *t)
{
   if( aVariableStack >=2 )
   {
      //
      CVariable v1=RegisterPop();
      CVariable v0=RegisterPop();
      //
      if( v0.Storage!=vsTemp && v0.Storage!=vsOutput )
         Error.SetError( CEWritingReadOnlyRegister , t->Debug , v0.toString() );
      //
      if( v1.Storage==vsOutput )
         Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v1.toString() );
      //
      AsmOut( ins , &v0 , &v1 , 0 , 0 , "assignment" );
   }
   // check variable stack
   /*
   if( aVariableStack )
   {
      printf("Variable stack screwed!\n");
   }
   /**/
}

void CCompiler::IfExpression(const char *ins,SCompilerTree *t)
{
   /**/
   CVariable v2=RegisterPop();
   CVariable v1=RegisterPop();
   CVariable v0=RegisterPop();
   //
   // ins: > or <
   // ( v0 ins v1 ? v2 : 0 )
   //
   CVariable r;
   //
   if( v0.Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v0.toString() );
   if( v1.Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v1.toString() );
   if( v2.Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v2.toString() );
   //
   //
   if( ( v1.Type==vtFloat && v2.Type==vtFloat ) || IsFloatModifier( t ) )
      r=AddTemp1();
   else 
   {
      r=AddTemp4();
      //
      CheckModifier( r , t );
   }
   //
   RegisterPush( &r , 1 );
   //
   CVariable t2=AddTemp1();
   //
   //CVariable zero=AddConstant( 0 );
   //
   AsmOut( ins   , &t2 , &v0 , &v1 );
   AsmOut( "mul" , &r  , &v2 , &t2 );
   /**/
}

void CCompiler::IfElseExpression(const char *ins,SCompilerTree *t)
{
   CVariable v3=RegisterPop();
   CVariable v2=RegisterPop();
   CVariable v1=RegisterPop();
   CVariable v0=RegisterPop();
   //
   // ins: > or <
   // ( v0 ins v1 ? v2 : v3 )
   //
   CVariable r;
   CVariable t1;
   CVariable t2;
   //
   if( v0.Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v0.toString() );
   if( v1.Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v1.toString() );
   if( v2.Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v2.toString() );
   if( v3.Storage==vsOutput ) Error.SetError( CEReadingWriteOnlyRegister , t->Debug , v3.toString() );
   //
   EVariableType  vt=vtVector;
   //
   if( v2.Type==vtFloat && v3.Type==vtFloat ) vt=vtFloat;

   //
   if( vt==vtFloat )
   {
      r=AddTemp1();
      t1=AddTemp1();
   }
   else 
   {
      r=AddTemp4();
      CheckModifier( r , t );
      t1=AddTemp4();
   }
   //
   RegisterPush( &r , 1 );
   //
   t2=AddTemp1();
   //
   AsmOut( "sub" , &t1 , &v3 , &v2 );
   AsmOut( ins   , &t2 , &v0 , &v1 );
   AsmOut( "mad" , &r  , &t1 , &t2 , &v2 );
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//
// Create Code 
//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

CVariable CCompiler::GetVariable( SCompilerTree *t , int analize )
{
   int         found=0;
   CVariable   r;
   //
   pGetVariable=0;
   //
   switch( t->Type ) {
   case ntClassVar : 
      {
         CVariable *v=FindVariable( t->pParameter->toString() , t->aTree[0]->pParameter->toString() );
         if( v ) 
         {
            found=1;
            pGetVariable=v;
            r=*v;
         }
      }
      break;
   case ntLParam : 
   case ntVariable : 
      {
         CVariable *v=FindVariable( t->pParameter->toString() );
         if( v ) 
         {
            found=1;
            pGetVariable=v;
            r=*v;
         }
      }
      break;
   /*
   case ntArrayAccess :
      {
         CVariable *v=FindVariable( t->pParameter->toString() );
         if( v ) 
         {
            found=1;
            pGetVariable=v;
            r=*v;
         }
      }
      break;
   /**/
   case ntInputVariable:
      {
         TString  s=t->pParameter->toString();
         //
         for(int a=0;a<InputVertex.aVariable;a++)
            if( InputVertex.aVariable[a].Name==s ) 
            {
               found=1;
               //
               r=InputVertex.aVariable[a];
               pGetVariable=&InputVertex.aVariable[a];
               //
               break;
            }
      }
      break;
   case ntOutputLParam:
      {
         TString  s=t->pParameter->toString();
         //
         for(int a=0;a<OutputVertex.aVariable;a++)
            if( OutputVertex.aVariable[a].Name==s ) 
            {
               found=1;
               //
               r=OutputVertex.aVariable[a];
               pGetVariable=&OutputVertex.aVariable[a];
               //
               break;
            }
      }
      break;
   default:
      printf("Invalid GetVariable tree type.\n");
      break;
   }
   //
   if( !found )
   {
      if( analize ) return r;
      //
      Error.SetError( CEUndeclaredIdentifier , t->Debug , t->pParameter->toString() );
      return r;
   }
   // check if we have array access
   if( t->aTree[2] && t->aTree[2]->Type==ntArrayAccess && analize )
   {
      // adjust version (a0.x is available only in vs.1.1)
      vsversion=1100;
   }
   //
   if( t->aTree[2] && t->aTree[2]->Type==ntArrayAccess && !analize )
   {
      CreateCode( t->aTree[2]->aTree[0] );
      //
      CVariable a=RegisterPop();
      //
      r.pArray=xnew( CVariable );
      //
      *r.pArray=a;
      //
      if( r.Storage!=vsConst )
      {
         Error.SetError( CEArrayAccessOnNonConstVariable , t->aTree[2]->Debug , t->pParameter->toString() );
      }
   }
   // check if we have class access
   if( t->aTree[0] && t->aTree[0]->Type==ntClass )
   {
      if( r.modifier.Length()==1 ) r.Type=vtFloat;
                              else r.Type=vtVector;
      //
      r.Name=t->aTree[0]->pParameter->toString() << TString("::") << t->pParameter->toString();
   }

   // check if we have modifier
   if( t->aTree[3] && t->aTree[3]->Type==ntModifier ) 
   {
      if( r.Type==vtFloat )
      {
         // modifier on float
         Error.SetError( CEModifierOnFloat , t->Debug , t->pParameter->toString() );
      }
      //
      r.modifier=t->aTree[3]->pParameter->toString();
      //
      //ValidateModifier( r.modifier , Error , t );
      //
      if( r.modifier.Length()==1 ) r.Type=vtFloat;
                              else r.Type=vtVector;
   }
   /**/
   return r;
}

void CCompiler::CreateCode( SCompilerTree *t , int num , int level )
{
   if( !t ) return;
   if( Error ) return;
   //
   int      a;
   int      recurse=1;
   int      donext=1;
   //
   if( Error ) return;

   // output
   #ifdef CODE_TREE_OUTPUT
   {
      for(a=0;a<level;a++) printf("   ");
      //
      printf( "CreateCode(%s) " , t->GetNodeName() );
      //
      if( t->pParameter )
      {
         const char *a=t->pParameter->toString();
         //
         printf(" [%s:%s]" , t->pParameter->GetType() , a );
      }
      printf("\n");
   }
   #endif

   ////////////////////////////////////////////
   // code creation
   ////////////////////////////////////////////
   switch( t->Type ) {

   /////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
   case ntOptimization:
      optimization=t->pParameter->param;
      break;
   case ntIfDef:
   case ntIfnDef:
      if( t->aTree[0] ) CreateCode( t->aTree[0] , 0 , level+1 );
      if( t->aTree[1] ) CreateCode( t->aTree[1] , 0 , level+1 );
      recurse=0;
      //
      break;
   /////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////
   // define variable
   ////////////////////////////////////////////
   case ntDefVariable: 
      {
         for(SCompilerTree *n=t->aTree[1];n;n=n->pNext)
         {
            CVariable *var=FindVariable( n->pParameter->toString() );
            //
            if( !var || !var->used ) continue;

            // check if we have initialization
            if( n->aTree[0] ) 
            {
               var->assigned=1;
               //
               AsmOutComment(3,"initialization of '%s'" , tochar( n->pParameter->toString() ) );
               CreateCode( n->aTree[0] , 0 , level+3 );
            }
         }
         //
         recurse=0;
      }
      break;


   ////////////////////////////////////////////
   // assembler
   ////////////////////////////////////////////
   // instructions with 4 operators
   case ntAsmMad :
      {
         CVariable v0=GetVariable( t->aTree[0] );
         CVariable v1=GetVariable( t->aTree[1] );
         CVariable v2=GetVariable( t->aTree[2] );
         CVariable v3=GetVariable( t->aTree[3] );
         //
         if( Error ) return;
         //
         AsmOut( "mad" , &v0 , &v1 , &v2 , &v3 );
         //
         recurse=0;
      }
      break;
   // instructions with 3 operators
   case ntAsmAdd :
   case ntAsmDp3 :
   case ntAsmDp4 :
   case ntAsmDst :
   case ntAsmMax :
   case ntAsmMin :
   case ntAsmMul :
   case ntAsmSge :
   case ntAsmSlt :
   case ntAsmSub :
   case ntAsmM3x2 :
   case ntAsmM3x3 :
   case ntAsmM3x4 :
   case ntAsmM4x3 :
   case ntAsmM4x4 :
      {
         CVariable v0=GetVariable( t->aTree[0] );
         CVariable v1=GetVariable( t->aTree[1] );
         CVariable v2=GetVariable( t->aTree[2] );
         //
         if( Error ) return;
         //
         TString ins="";
         //
         switch( t->Type ) {
         case ntAsmAdd  :  ins="add";  break;
         case ntAsmDp3  :  ins="dp3";  break;
         case ntAsmDp4  :  ins="dp4";  break;
         case ntAsmDst  :  ins="dst";  break;
         case ntAsmMax  :  ins="max";  break;
         case ntAsmMin  :  ins="min";  break;
         case ntAsmMul  :  ins="mul";  break;
         case ntAsmSge  :  ins="sge";  break;
         case ntAsmSlt  :  ins="slt";  break;
         case ntAsmSub  :  ins="sub";  break;
         case ntAsmM3x2 :  ins="m3x2"; break;
         case ntAsmM3x3 :  ins="m3x3"; break;
         case ntAsmM3x4 :  ins="m3x4"; break;
         case ntAsmM4x3 :  ins="m4x3"; break;
         case ntAsmM4x4 :  ins="m4x4"; break;
         }
         //
         AsmOut( ins , &v0 , &v1 , &v2 );
         //
         recurse=0;
      }
      break;
   // instructions with 2 operators
   case ntAsmExpp:
   case ntAsmLit:
   case ntAsmLogp:
   case ntAsmMov:
   case ntAsmRcp:
   case ntAsmRsq:
   case ntAsmExp:
   case ntAsmFrc:
   case ntAsmLog:
      {
         CVariable v0=GetVariable( t->aTree[0] );
         CVariable v1=GetVariable( t->aTree[1] );
         //
         if( Error ) return;
         //
         TString ins="";
         //
         switch( t->Type ) {
         case ntAsmExpp :  ins="expp"; break;
         case ntAsmLit  :  ins="lit";  break;
         case ntAsmLogp :  ins="logp"; break;
         case ntAsmMov  :  ins="mov";  break;
         case ntAsmRcp  :  ins="rcp"; break;
         case ntAsmRsq  :  ins="rsq"; break;
         case ntAsmExp  :  ins="exp"; break;
         case ntAsmFrc  :  ins="frc"; break;
         case ntAsmLog  :  ins="log"; break;
         }
         //
         AsmOut( ins , &v0 , &v1 );
         //
         recurse=0;
      }
      break;
   ////////////////////////////////////////////
   // function call
   ////////////////////////////////////////////
   case ntReturn  :
      {
         if( pActiveFunction->Type==vtVoid )
         {
            Error.SetError( CEFunctionReturningAValue , t->Debug );
            return;
         }
         //
         CreateCode( t->aTree[0] );
         //
         if( t->pNext ) Error.SetError( CEReturnNotLastCommand , t->Debug );
         //
         return;
      }
      break;
   case ntFuncCall : 
   case ntFuncCallRet : 
      {
         CFunction *f=FindFunction( t->pParameter->toString() );
         //
         if( !f )
         {
            Error.SetError( CEUndeclaredIdentifier , t->Debug , t->pParameter->toString() );
            return;
         }
         //
         if( t->Type==ntFuncCallRet && f->Type==vtVoid )
         {
            Error.SetError( CECannotConvert , t->Debug , "void' to 'non-void" );
            return;
         }

         //
         //AsmOutComment("inline function '%s'" , tochar( t->pParameter->toString() ) );
         // check parameters and assign function param regs
         int nparam=0;
         //
         for(SCompilerTree *param=t->aTree[0];param;nparam++,param=param->pNext)
         {
            // check if to many parameters
            if( f->aParam<=nparam ) 
            {
               Error.SetError( PEMissingSemicolonBefore , param->Debug , param->pParameter->toString() );
               //
               return;
            }
            //
            CreateCode( param );
         }
         //
         nparam--;
         a=0;
         for(param=t->aTree[0];param;a++,nparam--,param=param->pNext)
         {
            // just peek into stack (leave on stack so the regs are reserved)
            CVariable p=RegisterPop( 0 , nparam );
            //
            if( p.Type != f->aParam[a].Type )
            {
               Error.SetError( CECannotConvert , param->Debug );
               return;
            }
            //
            f->aParam[a].ParamName  =p.Name;
            f->aParam[a].Storage    =p.Storage;
            f->aParam[a].index      =p.index;
            f->aParam[a].modifier   =p.modifier;
            f->aParam[a].param      =1;
            f->aParam[a].assigned   =p.assigned;
         }
         // check if not enought parameters
         if( f->aParam!=a ) 
         {
            Error.SetError( PEInvalidFunctionArgumentDefinition , t->aTree[a-1]->Debug , f->Name );
            //
            return;
         }
         //
         CFunction *oldf=pActiveFunction;
         pActiveFunction=f;
         //
         PrepareFunctionVariables();
         // dump parameters
         if( f->aParam )
         {
            AsmOutComment(4,"   function parameters assignment:");
            for(int a=0;a<f->aParam;a++)
               AsmOutComment(4,"      '%s'='%s',%s" , 
                     tochar( f->aParam[a].Name ) ,
                     tochar( f->aParam[a].ParamName ) ,
                     tochar( f->aParam[a].toString() ) );
         }
         //
         CreateCode( f->pTree->aTree[2] );

         // take off result
         CVariable result;
         if( f->Type!=vtVoid ) result=RegisterPop();

         // take parameters off stack
         for(param=t->aTree[0];param;param=param->pNext) RegisterPop();
         //
         pActiveFunction=oldf;
         //
         recurse=0;
         //
         // push result back
         if( f->Type!=vtVoid ) 
         {
            CheckModifier( result , t );
            RegisterPush( &result );
            //
            AsmOutComment(4,"function end (result in '%s')" , tochar( result.toString() ) );
         }
         else
         {
            AsmOutComment(4,"function end");
         }
      }
      break;

   ////////////////////////////////////////////
   // push registers for expressions
   ////////////////////////////////////////////
   case ntLParam:
   case ntVariable:
   case ntClassVar:
      {
         CVariable v=GetVariable( t );
         CheckModifier( v , t );
         RegisterPush( &v );
         return;
      }
      break;
      /*
      // Obsolete
   case ntArrayAccess:
      {
         CreateCode( t->aTree[1] );
         //
         CVariable a=RegisterPop();
         //
         AsmOut( "mov" , "a0.x" , a.toString() );
         // adjust version (a0.x is available only in vs.1.1)
         vsversion=1100;
         //
         CVariable v;
         if( t->aTree[2] )
         {
            v=GetVariable( t->aTree[2] );
         }
         else
         {
            v=GetVariable( t );
            CheckModifier( v , t );
         }
         //
         v.pArray=xnew( CVariable );
         //
         v.pArray->Name="a0.x";
         //
         RegisterPush( &v );
         //
         return;
      }
      break;
      /**/
   case ntInputVariable:
      {
         TString  s=t->pParameter->toString();
         //
         for(int a=0;a<InputVertex.aVariable;a++)
            if( InputVertex.aVariable[a].Name==s ) 
            {
               //CVariable v=InputVertex.aVariable[a];
               //CheckModifier( v , t );
               CVariable v=GetVariable( t );
               //
               RegisterPush( &v );
               //
               return;
            }
         //
         Error.SetError( CEUndeclaredIdentifier , t->Debug , TString("in.") << t->pParameter->toString() );
         return;
      }
      break;
   case ntOutputLParam:
      {
         TString  s=t->pParameter->toString();
         //
         for(int a=0;a<OutputVertex.aVariable;a++)
            if( OutputVertex.aVariable[a].Name==s ) 
            {
               //CVariable v=OutputVertex.aVariable[a];
               //CheckModifier( v , t );
               CVariable v=GetVariable( t );
               //
               RegisterPush( &v );
               //
               return;
            }
         //
         Error.SetError( CEUndeclaredIdentifier , t->Debug , TString("out.") << t->pParameter->toString() );
         return;
      }
      break;
   case ntFloatConst:
      {
         RegisterPush( &AddConstant1( (float&)t->pParameter->param ) );
         return;
      }
      break;
   case ntVectorConst:
      {
         CVariable v=AddConstant4((float&)t->aTree[0]->pParameter->param , 
                                  (float&)t->aTree[1]->pParameter->param , 
                                  (t->aTree[2]?(float&)t->aTree[2]->pParameter->param:0) , 
                                  (t->aTree[3]?(float&)t->aTree[3]->pParameter->param:0) );
         CheckModifier( v , t );
         RegisterPush( &v );
         //
         return;
      }
      break;
   }

   ////////////////////////////////////////////
   // recurse on
   ////////////////////////////////////////////
   if( recurse )
   {
      for(a=0;a<4;a++)
         CreateCode( t->aTree[a] , a , level+1 );
   }

   ////////////////////////////////////////////
   // expressions
   ////////////////////////////////////////////
   switch( t->Type ) {
   case ntNeg :
      {
         CVariable v=RegisterPop();
         //
         v.neg^=1;
         //
         RegisterPush( &v );
      }
      break;
   case ntExprAdd : Expression3( "add"          , t ); return;
   case ntExprDp3 : Expression3( "dp3"          , t ); return;
   case ntExprDp4 : Expression3( "dp4"          , t ); return;
   case ntExprDst : Expression3( "dst"          , t ); return;
   case ntExprExpp: Expression2( "expp"         , t ); return;
   case ntExprLit : Expression2( "lit"          , t ); return;
   case ntExprLogp: Expression2( "logp"         , t ); return;
   case ntExprMad : Expression4( "mad"          , t ); return;
   case ntExprMax : Expression3( "max"          , t ); return;
   case ntExprMin : Expression3( "min"          , t ); return;
   case ntExprMov : Expression2( "mov"          , t ); return;
   case ntExprMul : Expression3( "mul"          , t ); return;
   case ntExprRcp : Expression2( "rcp"          , t ); return;
   case ntExprRsq : Expression2( "rsq"          , t ); return;
   case ntExprSge : Expression3( "sge"          , t ); return;
   case ntExprSlt : Expression3( "slt"          , t ); return;
   case ntExprSub : Expression3( "sub"          , t ); return;
   //                                           
   case ntExprExp : Expression2( "exp"          , t ); return;
   case ntExprFrc : Expression2( "frc"          , t ); return;
   case ntExprLog : Expression2( "log"          , t ); return;
   case ntExprM3x2: Expression3( "m3x2"         , t ); return;
   case ntExprM3x3: Expression3( "m3x3"         , t ); return;
   case ntExprM3x4: Expression3( "m3x4"         , t ); return;
   case ntExprM4x3: Expression3( "m4x3"         , t ); return;
   case ntExprM4x4: Expression3( "m4x4"         , t ); return;
   //
   case ntAssign  : AssignExpression( "mov"     , t ); return;
   case ntSetAdd  : AssignExpression( "add"     , t ); return;
   case ntSetSub  : AssignExpression( "sub"     , t ); return;
   case ntSetMul  : AssignExpression( "mul"     , t ); return;
   case ntSetDiv  : AssignExpression( "div"     , t ); return;
   //
   case ntIfGeElse: IfElseExpression( "sge"     , t ); return;
   case ntIfLsElse: IfElseExpression( "slt"     , t ); return;
   //
   case ntIfGe    : IfExpression( "sge"         , t ); return;
   case ntIfLs    : IfExpression( "slt"         , t ); return;
   }

   ////////////////////////////////////////////
   // siblings
   ////////////////////////////////////////////
   CreateCode( t->pNext , -1 , level );
}

/////////////////////////////////////////////////////////////
// Prepare generation
/////////////////////////////////////////////////////////////

void CCompiler::PrepareFunctionVariables()
{
   if( !pActiveFunction ) return;

   /////////////////////////////////////////////////
   // check if variables uses/assigned and
   // assign registers to variables
   /////////////////////////////////////////////////
   AsmOutComment(3,"Function '%s' variable assignment:" , tochar( pActiveFunction->Name ) );

   //
   /**/
   if( pActiveFunction->Name=="vsmain" )
   {
      if( pActiveFunction->aParam ) AsmOutComment(0,"vsmain parameter assignment:" );
      //
      for(int a=0;a<pActiveFunction->aParam;a++)
      {
         CVariable &p=pActiveFunction->aParam[a];
         CVariable op=p;
         //
         //if( p.Type==vtFloat ) p=AddConstant1(0,0);
         //                 else p=AddConstant4(0,0,0,0,0);
         // force usage of vector
         p=AddConstant4(0,0,0,0,0);
         //
         p.Name=op.Name;
         p.Type=op.Type;
         if( p.Type==vtFloat ) p.modifier="x";
         //
         AsmOutComment(0, "   parameter '%s'=%s." , tochar( p.Name ) , tochar( p.toString() ) );
         //
         TProgramParameter &par=Result.aParameter.Add();
         //
         par.Name=TName( p.Name );
         par.index=p.index;
      }
   }
   /**/

   for(int a=0;a<pActiveFunction->aVariable;a++ )
   {
      CVariable &v=pActiveFunction->aVariable[a];
      //
      if( !v.used )
      {
         //Error.OutputWarning( var->Debug , 0x4700 , "local variable '" << var->Name << "' used without having been initialized" );
         AsmOutComment(3, "   '%s' unused." , tochar( v.Name ) );
         //
         continue;
      }
      //
      if( v.index==-1 )
      {
         if( v.Type==vtFloat )
         {
            CVariable nv=AddTemp1();
            //
            v.index=nv.index;
            v.modifier=nv.modifier;
         }
         if( v.Type==vtVector )
         {
            CVariable nv=AddTemp4();
            //
            v.index=nv.index;
         }
         //
         AsmOutComment(3, "   '%s'=%s." , tochar( v.Name ) , tochar( v.toString() ) );
      }
   }
}

void CCompiler::PrepareCodeGeneration()
{
   /////////////////////////////////////////////////
   // Output vertex shader version
   /////////////////////////////////////////////////

   TString s=toString( vsversion );
   //
   s.Insert( s.Length() - 3 , "." );
   s.TrimRight( '0' );
   if( s[ s.Length()-1 ]=='.' ) s<<='0';
   //
   s="vs." << s;
   //
   AsmOut( s );

   /////////////////////////////////////////////////
   // check if plugin variables uses/assigned and
   // assign registers to variables
   /////////////////////////////////////////////////
   for(int a=0;a<aPlugin;a++)
   {
      CPlugin *pPlugin=&aPlugin[a];
      //
      AsmOutComment(3,"Plugin '%s' const assignment:" , tochar( pPlugin->Name ) );
      //
      //
      for(int a=0;a<pPlugin->aParam;a++ )
      {
         CVariable &v=pPlugin->aParam[a];
         //
         if( pPlugin->radical && pPlugin->used ) v.used=1;
         //
         if( !v.used )
         {
            //Error.OutputWarning( var->Debug , 0x4700 , "local variable '" << var->Name << "' used without having been initialized" );
            AsmOutComment(3, "   '%s' unused." , tochar( v.Name ) );
            //
            continue;
         }
         //
         if( v.index==-1 )
         {
            CVariable nv;
            //
            if( v.Type==vtFloat ) nv=AddConstant1( 0 , 0 );
                             else nv=AddConstant4( 0 , 0 , 0 , 0 , 0 );
            //
            if( v.arraysize ) 
            {
               for(int a=0;a<v.arraysize-1;a++)
                  if( v.Type==vtFloat ) AddConstant1( 0 , 0 );
                                   else AddConstant4( 0 , 0 , 0 , 0 , 0 );
            }
            //
            v.index=nv.index;
            v.modifier=nv.modifier;
            //
            AsmOutComment(3, "   '%s'=%s." , tochar( v.Name ) , tochar( v.toString() ) );
         }
      }
   }
}

void CCompiler::Finalize()
{
   Result.aConst.Clear();
   Result.aPlugin.Clear();
   //
   if( aVariableStack )
   {
      //printf("Stack screwed (%d).\n" , (int)aVariableStack );
   }
   //
   for(int a=0;a<aPlugin;a++)
   {
      int used=0;
      for(int v=0;v<aPlugin[a].aParam;v++)
      {
         if( aPlugin[a].aParam[v].used ) 
         {
            used=1;
            break;
         };
      }
      if( !used ) continue;
      //
      TPluginUsage &pu=Result.aPlugin.Add();
      //
      pu.PluginName=aPlugin[a].Name;
      pu.aConst.SetCount( aPlugin[a].aParam );
      //
      for(v=0;v<aPlugin[a].aParam;v++)
      {
         if( aPlugin[a].aParam[v].used )
         {
            AsmOutComment( 0 , "Plugin: %s::%s=%s" , tochar( aPlugin[a].Name ) , tochar( aPlugin[a].aParam[v].Name ) , tochar( aPlugin[a].aParam[v].toString() ) );
            //
            pu.aConst[v]=aPlugin[a].aParam[v].index;
         }
         else
         {
            pu.aConst[v]=-1;
         }
      }
   }
   //
   for(a=0;a<aConstUsed;a+=4)
      if( aConstUsed[a]==1 || aConstUsed[a+1]==1 || aConstUsed[a+2]==1 || aConstUsed[a+3]==1 )
      {
         TConstUsage &cu=Result.aConst.Add();
         cu.index=a >> 2;
         cu.Value.Set( unusedconstvalue , unusedconstvalue , unusedconstvalue , unusedconstvalue );
         if( aConstUsed[a  ]==1 ) cu.Value.x=aConst[a  ];
         if( aConstUsed[a+1]==1 ) cu.Value.y=aConst[a+1];
         if( aConstUsed[a+2]==1 ) cu.Value.z=aConst[a+2];
         if( aConstUsed[a+3]==1 ) cu.Value.w=aConst[a+3];
         //
         AsmOutComment( 0 , "Constant: c%d=(%s,%s,%s,%s)" , a/4 , 
            (aConstUsed[a  ]==1 ? tochar( toString( aConst[a  ] ) ) : "?" ),
            (aConstUsed[a+1]==1 ? tochar( toString( aConst[a+1] ) ) : "?" ),
            (aConstUsed[a+2]==1 ? tochar( toString( aConst[a+2] ) ) : "?" ),
            (aConstUsed[a+3]==1 ? tochar( toString( aConst[a+3] ) ) : "?" ) );
      }
}


/////////////////////////////////////////////////////////////
// include
/////////////////////////////////////////////////////////////

TString CCompiler::GetInclude(SCompilerTree *t,const char *inc,int sys)
{
   if( sys )
   {
      TString p( IncludePath );
      p<<=";.;";
      p.Replace('\\','/');
      //
      while(1)
      {
         int a=p.Pos(';');
         //
         if( a==-1 ) break;
         //
         TString dir;
         //
         dir.Copy( p , 0 , a );
         p.Copy( a+1 );
         //
         dir.Trim();
         if( dir=="" ) continue;
         //
         if( dir[ dir.Length()-1 ]!='/' ) dir<<='/';
         //
         TString fn=dir << inc;
         fn.Replace('\\','/');
         //
         TFileStream f( fn );
         //
         if( f.Opened() ) return fn;
      }
   }
   else
   {
      TString file=pScriptDebug->aSource[ t->Debug.sourceidx ];
      file.Replace('\\','/');
      int a=file.PosReverse( '/' );
      if( a )
      {
         file.Delete( a+1 );
         //
         file<<=inc;
      }
      else
      {
         file=inc;
      }
      //
      TFileStream f( file );
      //
      if( f.Opened() ) return file;
   }
   //
   Error.SetError( CECannotOpenIncludeFile , inc );
   //printf("
   //exit( 1 );
   //
   return "";
}

int CCompiler::Include(SCompilerTree *t,const char *inc,int sys)
{
   //
   if( sys )
   {
      TString p( IncludePath );
      p<<=";.;";
      p.Replace('\\','/');
      //
      while(1)
      {
         int a=p.Pos(';');
         //
         if( a==-1 ) return 1;
         //
         TString dir;
         //
         dir.Copy( p , 0 , a );
         p.Copy( a+1 );
         //
         dir.Trim();
         if( dir=="" ) continue;
         //
         if( dir[ dir.Length()-1 ]!='/' ) dir<<='/';
         //
         TString fn=dir << inc;
         fn.Replace('\\','/');
         //
         TFileStream f( fn );
         //
         if( f.Opened() )
         {
            // check if file already included
            if( aIncluded.Find( fn )>=0 ) return 0;
            //
            aIncluded.Add( fn );
            //
            if( ParseCompileFile( fn ) ) return 2;
            // link new tree
            t->aTree[0]=pCompilerTreeMng->pRoot;
            pCompilerTreeMng->pRoot->pParent=t;
            pCompilerTreeMng->pRoot=0;
            //
            return 0;
         }
      }
   }
   else
   {
      TString file=pScriptDebug->aSource[ t->Debug.sourceidx ];
      file.Replace('\\','/');
      int a=file.PosReverse( '/' );
      if( a )
      {
         file.Delete( a+1 );
         //
         file<<=inc;
      }
      else
      {
         file=inc;
      }
      //
      TFileStream f( file );
      //
      if( f.Opened() ) 
      {
         // check if file already included
         if( aIncluded.Find( file )>=0 ) return 0;
         //
         aIncluded.Add( file );
         //
         if( ParseCompileFile( file ) ) return 2;
         // link tree
         t->aTree[0]=pCompilerTreeMng->pRoot;
         pCompilerTreeMng->pRoot->pParent=t;
         //
         return 0;
      }
   }
   //
   return 1;
}

void CCompiler::ResetPluginsUsage()
{
   for(int a=0;a<aPlugin;a++)
   {
      aPlugin[a].used=0;
      //
      for(int b=0;b<aPlugin[a].aParam;b++)
         aPlugin[a].aParam[b].used=0;
   }
}

////////////////////////////////////////////////////////////////////////
// Plugins
////////////////////////////////////////////////////////////////////////

TArray<TString>   aPlugin;

void SCResetPlugins()
{
   aPlugin.Clear();
}

void SCRegisterPlugin( const char *name )
{
   //printf( "Registerplugin '%s'\n" , name );
   //
   if( SCIsPlugin( name ) ) return;
   //
   aPlugin.Add( name );
}

int SCIsPlugin( const char *name )
{
   return aPlugin.Find( name )>=0;
}

/////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////

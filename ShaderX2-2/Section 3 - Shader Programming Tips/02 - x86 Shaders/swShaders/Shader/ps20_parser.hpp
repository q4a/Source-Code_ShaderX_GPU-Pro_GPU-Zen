/******************************************************************
 * File: ps20_parser.hpp
 *
 * This file was automatically generated by CppCC Version 0.0.5
 * DO NOT EDIT MANUALLY
 ******************************************************************/


#ifndef __PS20_PARSER_HH__
#define __PS20_PARSER_HH__

#include <string>
#include <exception>
using namespace std;

#include "ps20_token.hpp"
#include "ps20_scanner.hpp"
#include "PS20Assembler.hpp"

#include "Instruction.hpp"
#include "Operand.hpp"
#include "Error.hpp"
using namespace swShader;

#include <string.h>
#include <list>

#pragma warning(disable : 4290)
#pragma warning(disable : 4786)


namespace swShader
{

class ParseException : public exception
{
  public:
    
    ParseException (const string &message_ = "Parse exception") :
      message(message_)
    {}
    
    ParseException (const Position &pos_,
                   const string &message_ = "Parse exception") :
      message(message_), pos(pos_)
    {}
    
    ~ParseException () throw ()
    {}
    
    virtual operator string () const
    {
      return string(pos) + ": " + message;
    }
    
    virtual const char* what () throw ()
    {
      return message.c_str();
    }
    
    virtual Position& where () throw ()
    {
      return pos;
    }
    
  private:
  
    Position pos;
    string message;
}; // end class PasreException


class ps20_parser_base
{
  protected:
    bool onParseError (ParseException &pex) { return false; }
};


class ps20_parser : public ps20_parser_base,  public PS20Assembler


{
  public: // methods
    
    ps20_parser (istream *in_ = NULL) throw (ScanException, ParseException) :
       scanner(in_), _la(0)
    {}
    
    // productions entry points:
    
    void
     main ()
       throw (ScanException, ParseException);
    void
     instruction ()
       throw (ScanException, ParseException);
    void
     psInstruction ()
       throw (ScanException, ParseException);
    void
     dclInstruction ()
       throw (ScanException, ParseException);
    void
     dcl_2dInstruction ()
       throw (ScanException, ParseException);
    void
     dcl_cubeInstruction ()
       throw (ScanException, ParseException);
    void
     dcl_volumeInstruction ()
       throw (ScanException, ParseException);
    void
     defInstruction ()
       throw (ScanException, ParseException);
    void
     absInstruction ()
       throw (ScanException, ParseException);
    void
     addInstruction ()
       throw (ScanException, ParseException);
    void
     cmpInstruction ()
       throw (ScanException, ParseException);
    void
     crsInstruction ()
       throw (ScanException, ParseException);
    void
     dp2addInstruction ()
       throw (ScanException, ParseException);
    void
     dp3Instruction ()
       throw (ScanException, ParseException);
    void
     dp4Instruction ()
       throw (ScanException, ParseException);
    void
     expInstruction ()
       throw (ScanException, ParseException);
    void
     frcInstruction ()
       throw (ScanException, ParseException);
    void
     logInstruction ()
       throw (ScanException, ParseException);
    void
     lrpInstruction ()
       throw (ScanException, ParseException);
    void
     m3x2Instruction ()
       throw (ScanException, ParseException);
    void
     m3x3Instruction ()
       throw (ScanException, ParseException);
    void
     m3x4Instruction ()
       throw (ScanException, ParseException);
    void
     m4x3Instruction ()
       throw (ScanException, ParseException);
    void
     m4x4Instruction ()
       throw (ScanException, ParseException);
    void
     madInstruction ()
       throw (ScanException, ParseException);
    void
     maxInstruction ()
       throw (ScanException, ParseException);
    void
     minInstruction ()
       throw (ScanException, ParseException);
    void
     movInstruction ()
       throw (ScanException, ParseException);
    void
     mulInstruction ()
       throw (ScanException, ParseException);
    void
     nopInstruction ()
       throw (ScanException, ParseException);
    void
     nrmInstruction ()
       throw (ScanException, ParseException);
    void
     powInstruction ()
       throw (ScanException, ParseException);
    void
     rcpInstruction ()
       throw (ScanException, ParseException);
    void
     rsqInstruction ()
       throw (ScanException, ParseException);
    void
     sincosInstruction ()
       throw (ScanException, ParseException);
    void
     subInstruction ()
       throw (ScanException, ParseException);
    void
     texkillInstruction ()
       throw (ScanException, ParseException);
    void
     texldInstruction ()
       throw (ScanException, ParseException);
    void
     texldbInstruction ()
       throw (ScanException, ParseException);
    void
     texldpInstruction ()
       throw (ScanException, ParseException);
    void
     instructionModifier ()
       throw (ScanException, ParseException);
    void
     dst ()
       throw (ScanException, ParseException);
    void
     dest ()
       throw (ScanException, ParseException);
    void
     sn ()
       throw (ScanException, ParseException);
    void
     cn ()
       throw (ScanException, ParseException);
    void
     fValue1 ()
       throw (ScanException, ParseException);
    void
     fValue2 ()
       throw (ScanException, ParseException);
    void
     fValue3 ()
       throw (ScanException, ParseException);
    void
     fValue4 ()
       throw (ScanException, ParseException);
    void
     src ()
       throw (ScanException, ParseException);
    void
     src0 ()
       throw (ScanException, ParseException);
    void
     src1 ()
       throw (ScanException, ParseException);
    void
     src2 ()
       throw (ScanException, ParseException);
    void
     outputRegister ()
       throw (ScanException, ParseException);
    void
     inputRegister ()
       throw (ScanException, ParseException);
    void
     sourceModifier ()
       throw (ScanException, ParseException);
    void
     writeMask ()
       throw (ScanException, ParseException);
    void
     sourceSwizzle ()
       throw (ScanException, ParseException);
    void
     colorRegister ()
       throw (ScanException, ParseException);
    void
     constantFloatRegister ()
       throw (ScanException, ParseException);
    void
     inputTextureCoordinateRegister ()
       throw (ScanException, ParseException);
    void
     samplerRegister ()
       throw (ScanException, ParseException);
    void
     temporaryRegister ()
       throw (ScanException, ParseException);
    void
     outputColorRegister ()
       throw (ScanException, ParseException);
    void
     outputDepthRegister ()
       throw (ScanException, ParseException);
    
    // productions LA methods:
    
    bool __la_main_ () throw (ScanException);
    bool __la_instruction_ () throw (ScanException);
    bool __la_psInstruction_ () throw (ScanException);
    bool __la_dclInstruction_ () throw (ScanException);
    bool __la_dcl_2dInstruction_ () throw (ScanException);
    bool __la_dcl_cubeInstruction_ () throw (ScanException);
    bool __la_dcl_volumeInstruction_ () throw (ScanException);
    bool __la_defInstruction_ () throw (ScanException);
    bool __la_absInstruction_ () throw (ScanException);
    bool __la_addInstruction_ () throw (ScanException);
    bool __la_cmpInstruction_ () throw (ScanException);
    bool __la_crsInstruction_ () throw (ScanException);
    bool __la_dp2addInstruction_ () throw (ScanException);
    bool __la_dp3Instruction_ () throw (ScanException);
    bool __la_dp4Instruction_ () throw (ScanException);
    bool __la_expInstruction_ () throw (ScanException);
    bool __la_frcInstruction_ () throw (ScanException);
    bool __la_logInstruction_ () throw (ScanException);
    bool __la_lrpInstruction_ () throw (ScanException);
    bool __la_m3x2Instruction_ () throw (ScanException);
    bool __la_m3x3Instruction_ () throw (ScanException);
    bool __la_m3x4Instruction_ () throw (ScanException);
    bool __la_m4x3Instruction_ () throw (ScanException);
    bool __la_m4x4Instruction_ () throw (ScanException);
    bool __la_madInstruction_ () throw (ScanException);
    bool __la_maxInstruction_ () throw (ScanException);
    bool __la_minInstruction_ () throw (ScanException);
    bool __la_movInstruction_ () throw (ScanException);
    bool __la_mulInstruction_ () throw (ScanException);
    bool __la_nopInstruction_ () throw (ScanException);
    bool __la_nrmInstruction_ () throw (ScanException);
    bool __la_powInstruction_ () throw (ScanException);
    bool __la_rcpInstruction_ () throw (ScanException);
    bool __la_rsqInstruction_ () throw (ScanException);
    bool __la_sincosInstruction_ () throw (ScanException);
    bool __la_subInstruction_ () throw (ScanException);
    bool __la_texkillInstruction_ () throw (ScanException);
    bool __la_texldInstruction_ () throw (ScanException);
    bool __la_texldbInstruction_ () throw (ScanException);
    bool __la_texldpInstruction_ () throw (ScanException);
    bool __la_instructionModifier_ () throw (ScanException);
    bool __la_dst_ () throw (ScanException);
    bool __la_dest_ () throw (ScanException);
    bool __la_sn_ () throw (ScanException);
    bool __la_cn_ () throw (ScanException);
    bool __la_fValue1_ () throw (ScanException);
    bool __la_fValue2_ () throw (ScanException);
    bool __la_fValue3_ () throw (ScanException);
    bool __la_fValue4_ () throw (ScanException);
    bool __la_src_ () throw (ScanException);
    bool __la_src0_ () throw (ScanException);
    bool __la_src1_ () throw (ScanException);
    bool __la_src2_ () throw (ScanException);
    bool __la_outputRegister_ () throw (ScanException);
    bool __la_inputRegister_ () throw (ScanException);
    bool __la_sourceModifier_ () throw (ScanException);
    bool __la_writeMask_ () throw (ScanException);
    bool __la_sourceSwizzle_ () throw (ScanException);
    bool __la_colorRegister_ () throw (ScanException);
    bool __la_constantFloatRegister_ () throw (ScanException);
    bool __la_inputTextureCoordinateRegister_ () throw (ScanException);
    bool __la_samplerRegister_ () throw (ScanException);
    bool __la_temporaryRegister_ () throw (ScanException);
    bool __la_outputColorRegister_ () throw (ScanException);
    bool __la_outputDepthRegister_ () throw (ScanException);
    
    // lookahead entry points:
    
    // LA entry points for main
    
    bool __la_0_ (int depth) throw (ScanException);
    // LA entry points for instruction
    
    // LA entry points for psInstruction
    
    // LA entry points for dclInstruction
    
    // LA entry points for dcl_2dInstruction
    
    // LA entry points for dcl_cubeInstruction
    
    // LA entry points for dcl_volumeInstruction
    
    // LA entry points for defInstruction
    
    // LA entry points for absInstruction
    
    // LA entry points for addInstruction
    
    // LA entry points for cmpInstruction
    
    // LA entry points for crsInstruction
    
    // LA entry points for dp2addInstruction
    
    // LA entry points for dp3Instruction
    
    // LA entry points for dp4Instruction
    
    // LA entry points for expInstruction
    
    // LA entry points for frcInstruction
    
    // LA entry points for logInstruction
    
    // LA entry points for lrpInstruction
    
    // LA entry points for m3x2Instruction
    
    // LA entry points for m3x3Instruction
    
    // LA entry points for m3x4Instruction
    
    // LA entry points for m4x3Instruction
    
    // LA entry points for m4x4Instruction
    
    // LA entry points for madInstruction
    
    // LA entry points for maxInstruction
    
    // LA entry points for minInstruction
    
    // LA entry points for movInstruction
    
    // LA entry points for mulInstruction
    
    // LA entry points for nopInstruction
    
    // LA entry points for nrmInstruction
    
    // LA entry points for powInstruction
    
    // LA entry points for rcpInstruction
    
    // LA entry points for rsqInstruction
    
    // LA entry points for sincosInstruction
    
    // LA entry points for subInstruction
    
    // LA entry points for texkillInstruction
    
    // LA entry points for texldInstruction
    
    // LA entry points for texldbInstruction
    
    // LA entry points for texldpInstruction
    
    // LA entry points for instructionModifier
    
    // LA entry points for dst
    
    // LA entry points for dest
    
    // LA entry points for sn
    
    // LA entry points for cn
    
    // LA entry points for fValue1
    
    // LA entry points for fValue2
    
    // LA entry points for fValue3
    
    // LA entry points for fValue4
    
    // LA entry points for src
    
    // LA entry points for src0
    
    // LA entry points for src1
    
    // LA entry points for src2
    
    // LA entry points for outputRegister
    
    // LA entry points for inputRegister
    
    // LA entry points for sourceModifier
    
    // LA entry points for writeMask
    
    bool __la_1_ (int depth) throw (ScanException);
    // LA entry points for sourceSwizzle
    
    bool __la_2_ (int depth) throw (ScanException);
    bool __la_3_ (int depth) throw (ScanException);
    // LA entry points for colorRegister
    
    // LA entry points for constantFloatRegister
    
    // LA entry points for inputTextureCoordinateRegister
    
    bool __la_4_ (int depth) throw (ScanException);
    // LA entry points for samplerRegister
    
    // LA entry points for temporaryRegister
    
    // LA entry points for outputColorRegister
    
    // LA entry points for outputDepthRegister
    
    
    
  private: // methods
    
    void consumeToken (int tId)  throw (ScanException, ParseException)
    {
      if ((token = scanner.la())->id != tId)
      {
        ParseException pex(scanner.la()->bPos, "Parse error.");
        if (!onParseError(pex)) throw pex;
      }
      scanner.unchecked_consume();
    }
    
    bool laConsumeToken (int tId)  throw (ScanException, ParseException)
    {
      if (scanner.la()->id != tId) return false;
      scanner.unchecked_consume();
      return true;
    }
    
  public: // data
    
    ps20_scanner scanner;
    int _la;
    ps20_token *token;
    
  private: // user code
  
	private:
		Operand t;   // Temporary operand
	
		Instruction::Mnemonic m;  // Operation
		Instruction::Modifier x;  // Instrution modifier
		Operand d;   // Destination
		Operand s0;   // Source 0
		Operand s1;   // Source 1
		Operand s2;   // Source 2
		Operand s3;   // Source 3
	

  
}; //end class ps20_parser


} //end namespace swShader


#endif // ifndef __PS20_PARSER_HH__


/* End of file. */

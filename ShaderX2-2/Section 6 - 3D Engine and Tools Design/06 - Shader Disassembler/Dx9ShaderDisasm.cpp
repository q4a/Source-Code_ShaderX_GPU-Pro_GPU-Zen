#include "Dx9ShaderDisasm.h"

#include "windows.h"
#include "d3d9types.h"			// ShaderToken bit fields are defined here

#include <StdIo.h>
#include <StdLib.h>

const char*					CDx9ShaderDisasm::StrNewLine = "\r\n";
Nat32*						CDx9ShaderDisasm::Tokens = Null;
Natural						CDx9ShaderDisasm::iToken = 0;
Bool						CDx9ShaderDisasm::IsPixelShader = False;
Natural						CDx9ShaderDisasm::VersionId = 0;
char						CDx9ShaderDisasm::StrLine[256];
const char*					CDx9ShaderDisasm::StrSpaces = "                ";	// 16 spaces (alignement purpose)
const Natural				CDx9ShaderDisasm::cSpace = strlen(StrSpaces);
CDx9ShaderDisasm::SOutput	CDx9ShaderDisasm::Output;

CDx9ShaderDisasm::SOutput::~SOutput()
{
	delete [] Chars;
}
void CDx9ShaderDisasm::SOutput::Free()
{
	delete [] Chars;
	Chars = Null;
	Count = 0;
	Alloc = 0;
}

void CDx9ShaderDisasm::SOutput::CatLine(const char* _Line)
{
	const Natural	cLine = strlen(_Line);	
	const Natural	cCR = strlen(StrNewLine);
	const Natural	cNew = Count+cLine+cCR+1;	// 1 is for trailing "\0"
	if (cNew>Alloc)
	{	// realloc
		char*	NewChars = new char[cNew];
		if (Count)
			memcpy(NewChars, Chars, Count);
		delete [] Chars;
		Chars = NewChars;
		Alloc = cNew;
	}
	memcpy(Chars+Count, _Line, cLine);
	memcpy(Chars+Count+cLine, StrNewLine, cCR);
	Chars[cNew-1] = '\0';
	Count = cNew-1;
}

Natural CDx9ShaderDisasm::CatNatural(Natural _Nat, Natural _cCharMin)
{
	const Natural	iLast = strlen(StrLine);
	char			Format[16];
	sprintf(Format, "%%0%dd", _cCharMin);
	sprintf(StrLine+iLast, Format, _Nat);
	return strlen(StrLine+iLast);
}

Natural CDx9ShaderDisasm::CatStringLower(const char* _String) 
{
	const Natural	iLast = strlen(StrLine);
	const Natural	cCat = strlen(_String);
	for (Natural i=0; i<cCat; i++) {
		StrLine[iLast+i] = tolower(_String[i]);
	}
	StrLine[iLast+cCat] = 0;
	return cCat;
}

Natural CDx9ShaderDisasm::CatInstr(const char* _Name, Bool _IsParam1Dst, Natural _cSpaceAfter)
{
	const Nat32	TokenOpCode = Tokens[iToken++];
	const Nat32	TokenRegDst = _IsParam1Dst ? Tokens[iToken] : 0;	// Branching instr. have only source parameters
	Natural		cChar = 0;
	cChar += CatStringLower(_Name);
	const Natural	OpCode		= MMask_And(TokenOpCode, D3DSI_OPCODE_MASK);
	const Natural	Specific	= MMask_And(TokenOpCode, D3DSP_OPCODESPECIFICCONTROL_MASK)>>D3DSP_OPCODESPECIFICCONTROL_SHIFT;
	const char*		CompNames[] = {"gt", "eq", "ge", "lt", "ne", "le"};
	switch (OpCode)
	{
	case D3DSIO_IFC:
	case D3DSIO_BREAKC:
	case D3DSIO_SETP: 
		cChar += CatChar('_');
		if (Specific-1 < sizeof(CompNames)/sizeof(CompNames[0]))
			cChar += CatString(CompNames[Specific-1]);	
		else
			cChar += CatChar('?');
		break;
	}
	const Natural	Shift = MMask_And(TokenRegDst, D3DSP_DSTSHIFT_MASK)>>D3DSP_DSTSHIFT_SHIFT;
	switch (Shift)
	{
	case 0x00: break;
	case 0x01: cChar += CatString("_x2"); break;
	case 0x02: cChar += CatString("_x4"); break;
	case 0x03: cChar += CatString("_x8"); break;
	case 0x0d: cChar += CatString("_d8"); break;
	case 0x0e: cChar += CatString("_d4"); break;
	case 0x0f: cChar += CatString("_d2"); break;
	default:   cChar += CatString("_?"); break;
	}
	if (MMask_And(TokenRegDst, D3DSPDM_SATURATE))			cChar+=CatString("_sat");
	if (MMask_And(TokenRegDst, D3DSPDM_PARTIALPRECISION))	cChar+=CatString("_pp");
	if (MMask_And(TokenRegDst, D3DSPDM_MSAMPCENTROID))		cChar+=CatString("_centroid");
	if (_cSpaceAfter>0) {
		if (cChar<_cSpaceAfter)	cChar += CatSpaces(_cSpaceAfter-cChar);
		else					cChar += CatChar(' ');
	}
	return cChar;
}

static Natural	Disasm_RegisterGetType(Nat32 _Token)
{
	Natural			CharCount = 0;
	const Natural	RegType1 = MMask_And(D3DSP_REGTYPE_MASK , _Token)>>D3DSP_REGTYPE_SHIFT ;
	const Natural	RegType2 = MMask_And(D3DSP_REGTYPE_MASK2, _Token)>>D3DSP_REGTYPE_SHIFT2;
	return RegType2 | RegType1;
}

Natural CDx9ShaderDisasm::CatRegister(Nat32 _Token)
{
	Natural			CharCount = 0;
	const Natural	RegType = Disasm_RegisterGetType(_Token);
	Natural			RegIndex = MMask_And(D3DSP_REGNUM_MASK, _Token);
	const char*		RegName;
	Natural			IndexOffset = 0;	// No index if -1
	const char*		RastOutNames[] = {"oPos", "oFog", "oPts"};
	const char*		MiscNames[] = {"vPos", "vFace"};
	switch (RegType)
	{
	case D3DSPR_TEMP:			RegName = "r"; break;
	case D3DSPR_INPUT:			RegName = "v"; break;
	case D3DSPR_CONST:			RegName = "c"; break;
	case D3DSPR_ADDR:		/*case D3DSPR_TEXTURE:*/	
								RegName = IsPixelShader ? "t" : "a"; break;
	case D3DSPR_RASTOUT:		RegName = RastOutNames[RegIndex]; IndexOffset = -1; break;
	case D3DSPR_ATTROUT:		RegName = "oD"; break;
	case D3DSPR_TEXCRDOUT:	/*case D3DSPR_OUTPUT:*/
								RegName = "oT"; break;
	case D3DSPR_CONSTINT:		RegName = "i"; break;
	case D3DSPR_COLOROUT:		RegName = "oC"; break;
	case D3DSPR_DEPTHOUT:		RegName = "oDepth"; IndexOffset = -1; break;
	case D3DSPR_SAMPLER:		RegName = "s";	break;
	case D3DSPR_CONST2:			RegName = "c"; IndexOffset = 2048; break;
	case D3DSPR_CONST3:			RegName = "c"; IndexOffset = 4096; break;
	case D3DSPR_CONST4:			RegName = "c"; IndexOffset = 6144; break;
	case D3DSPR_CONSTBOOL:		RegName = "b"; break;
	case D3DSPR_LOOP:			RegName = "aL";  IndexOffset = -1; break;
	case D3DSPR_TEMPFLOAT16:	RegName = "f"; break;
	case D3DSPR_MISCTYPE:		RegName = MiscNames[RegIndex]; IndexOffset = -1; break;
	case D3DSPR_LABEL:			RegName = "l"; break;
	case D3DSPR_PREDICATE:		RegName = "p"; break;
	default:					RegName = "?"; Assert(False); break;
	}
	CharCount += CatString(RegName);
	if (IndexOffset!=-1)
	{
		RegIndex += IndexOffset;
		CharCount+=CatNatural(RegIndex);
	}
	return CharCount;
}

const Natural	SrcMod_None		= (D3DSPSM_NONE		>>D3DSP_SRCMOD_SHIFT);
const Natural	SrcMod_Neg		= (D3DSPSM_NEG		>>D3DSP_SRCMOD_SHIFT);     
const Natural	SrcMod_Bias		= (D3DSPSM_BIAS		>>D3DSP_SRCMOD_SHIFT);    
const Natural	SrcMod_BiasNeg	= (D3DSPSM_BIASNEG	>>D3DSP_SRCMOD_SHIFT); 
const Natural	SrcMod_Sign		= (D3DSPSM_SIGN		>>D3DSP_SRCMOD_SHIFT);    
const Natural	SrcMod_SignNeg	= (D3DSPSM_SIGNNEG	>>D3DSP_SRCMOD_SHIFT); 
const Natural	SrcMod_Comp		= (D3DSPSM_COMP		>>D3DSP_SRCMOD_SHIFT);    
const Natural	SrcMod_X2		= (D3DSPSM_X2		>>D3DSP_SRCMOD_SHIFT);
const Natural	SrcMod_X2Neg	= (D3DSPSM_X2NEG	>>D3DSP_SRCMOD_SHIFT);
const Natural	SrcMod_dz		= (D3DSPSM_DZ		>>D3DSP_SRCMOD_SHIFT);      
const Natural	SrcMod_dw		= (D3DSPSM_DW		>>D3DSP_SRCMOD_SHIFT);      
const Natural	SrcMod_Abs		= (D3DSPSM_ABS		>>D3DSP_SRCMOD_SHIFT);     
const Natural	SrcMod_AbsNeg	= (D3DSPSM_ABSNEG	>>D3DSP_SRCMOD_SHIFT);  
const Natural	SrcMod_Not		= (D3DSPSM_NOT		>>D3DSP_SRCMOD_SHIFT);     

Natural CDx9ShaderDisasm::CatParamRegSrc(Natural _cSpaceBefore, Natural _cSpaceAfter)
{
	const Nat32		Token = Tokens[iToken++];
	Natural			Swizzle = MMask_And(Token, D3DVS_SWIZZLE_MASK);
	Natural			Modifier = MMask_And(Token, D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT;
	const char		SwizzleChars[4] = {'x','y','z','w'};
	const char*		StrMod = "";
	switch (Modifier)
	{
	case SrcMod_None:		
	case SrcMod_Bias:
	case SrcMod_Sign:		
	case SrcMod_X2:
	case SrcMod_dz:		
	case SrcMod_dw:			break;		

	case SrcMod_Neg:		
	case SrcMod_BiasNeg:	
	case SrcMod_SignNeg:	
	case SrcMod_X2Neg:		StrMod = "-";		break;

	case SrcMod_Comp:		StrMod = "1-";		break;

	case SrcMod_Abs:		StrMod = "abs(";	break;
	case SrcMod_AbsNeg:		StrMod = "-abs(";	break;

	case SrcMod_Not:		StrMod = "!";		break;

	default:				StrMod = "?";		break;
	}
	Natural			cCharBefore = strlen(StrMod);
	if (cCharBefore<_cSpaceBefore)
		cCharBefore += CatSpaces(_cSpaceBefore-cCharBefore);
	CatString(StrMod);
	Natural			cChar;
	cChar = CatRegister(Token);
	if (MMask_And(Token, D3DSHADER_ADDRESSMODE_MASK)==D3DSHADER_ADDRMODE_RELATIVE)
	{
		if (IsPixelShader || VersionId>=0x0200)
		{
			const Nat32		TokenAdr = Tokens[iToken++];
			const Natural	RegType = Disasm_RegisterGetType(TokenAdr);
			cChar+=CatChar('[');
			cChar+=CatRegister(TokenAdr);
			if (RegType==D3DSPR_ADDR) 
			{	// A component must be selected (written in swizzle of Address register)
				Natural		AdrComp = MMask_And(TokenAdr>>D3DVS_SWIZZLE_SHIFT, 0x03); 
				cChar+=CatChar('.');
				cChar+=CatChar(SwizzleChars[AdrComp]);
			}
			cChar+=CatChar(']');
		}
		else
		{
			cChar+=CatString("[a0.x]");
		}
	}
	switch (Modifier)
	{
	case SrcMod_None:		break;	
	case SrcMod_Neg:		break;
	case SrcMod_Bias:		
	case SrcMod_BiasNeg:	cChar+=CatString("_bias");	break;
	case SrcMod_Sign:		
	case SrcMod_SignNeg:	cChar+=CatString("_bx2");	break;
	case SrcMod_Comp:		break;
	case SrcMod_X2:
	case SrcMod_X2Neg:		cChar+=CatString("_x2");	break;
	case SrcMod_dz:			cChar+=CatString("_dz");	break;
	case SrcMod_dw:			cChar+=CatString("_dw");	break;
	case SrcMod_Abs:
	case SrcMod_AbsNeg:		cChar+=CatChar(')');		break;
	case SrcMod_Not:		break;
	}
	if (Swizzle!=D3DVS_NOSWIZZLE)
	{		
		cChar += CatChar('.');
		Swizzle >>= D3DVS_SWIZZLE_SHIFT;
		Natural	LastSwizzle = MMask_And(Swizzle>>6, 0x03);
		Natural	cSwizzleChar = 4;
		Natural	i = 4;
		while (MMask_And(Swizzle>>i, 0x03)==LastSwizzle) {
			if (--cSwizzleChar==1) break;
			i -= 2;
		}
		for (i=0; i<cSwizzleChar; i++) {
			cChar += CatChar(SwizzleChars[MMask_And(Swizzle, 0x03)]);
			Swizzle >>= 2;
		}
	}
	if (cChar<_cSpaceAfter)
		cChar += CatSpaces(_cSpaceAfter-cChar);
	return cCharBefore+cChar;
}

void CDx9ShaderDisasm::CatParamRegDst()
{
	const Nat32		Token = Tokens[iToken++];
	Natural			cChar = 0;
	cChar += CatRegister(Token);
	if (!MMask_Test(Token, D3DSP_WRITEMASK_ALL))
	{
		cChar += CatChar('.');
		if (MMask_And(Token, D3DSP_WRITEMASK_0)) {cChar+=CatChar('x');}
		if (MMask_And(Token, D3DSP_WRITEMASK_1)) {cChar+=CatChar('y');}
		if (MMask_And(Token, D3DSP_WRITEMASK_2)) {cChar+=CatChar('z');}
		if (MMask_And(Token, D3DSP_WRITEMASK_3)) {cChar+=CatChar('w');}
	}
	const Natural	MinChar = 8;
	if (cChar<MinChar)
		CatSpaces(MinChar-cChar);
}

void CDx9ShaderDisasm::CatParamReal()
{
	const Real		Token = (Real&)Tokens[iToken++];
	const Natural	iLast = strlen(StrLine);
	sprintf(StrLine+iLast, "%.6g", Token);
}

void CDx9ShaderDisasm::CatParamBool()
{
	const Bool		Token = (Bool&)Tokens[iToken++];
	CatString(Token ? "true" : "false");
}

void CDx9ShaderDisasm::CatParamInteger()
{
	const Integer	Token = (Integer&)Tokens[iToken++];
	const Natural	iLast = strlen(StrLine);
	sprintf(StrLine+iLast, "%d", Token);
}

const char* CDx9ShaderDisasm::DisAssemble(Nat32* _Tokens, Natural _cToken)
{
	Output.Reset();
	Tokens = _Tokens;
	for (iToken=0; iToken<_cToken;) {
		Nat32			Token = _Tokens[iToken];
		StrLine[0] = 0;
		// Special tokens :
		if (MMask_And(0xFFFF0000, Token)==0xFFFE0000 ||
			MMask_And(0xFFFF0000, Token)==0xFFFF0000) 
		{	// VS / PS token
			const Natural	VerLow	= MMask_And(0x000000FF, Token);
			const Natural	VerHigh	= MMask_And(0x0000FF00, Token) >> 8;
			VersionId = MMask_And(0x0000FFFF, Token);
			if (MMask_And(0xFFFF0000, Token)==0xFFFE0000)	{CatString("vs_"); IsPixelShader=False;}
			else											{CatString("ps_"); IsPixelShader=True;}
			CatNatural(VerHigh);
			CatChar('_');
			if (VerLow==0xFF)						CatString("sw");		// ps_2_sw
			else if (VerLow==0x01 && VerHigh==0x02)	CatChar('x');			// ps_2_x
			else									CatNatural(VerLow);
			CatLine();
			iToken++;
			continue;	// done
		}
		if (MMask_And(0x80000000, Token)!=0 ||	// 31		Should be 0
			MMask_And(0x20000000, Token)!=0)	// 29		Should be 0 
		{
			Assert(False);
			sprintf(StrLine, "Invalid token (0x%08X)", Token);
			CatLine();
			iToken++;
			continue;
		}
		const Natural	OpCode =	MMask_And(D3DSI_OPCODE_MASK, Token);
		if (OpCode==D3DSIO_COMMENT)
		{
			const Natural	cNat32 = MMask_And(0x7FFF0000, Token) >> 16;
			iToken += 1+cNat32;	// Skip comments
			continue;
		}
		else if (OpCode==D3DSIO_END)
		{
			break;
		}
		const Natural	OpSpecif =	MMask_And(D3DSP_OPCODESPECIFICCONTROL_MASK, Token)>>D3DSP_OPCODESPECIFICCONTROL_SHIFT;
		// TAG Warning : sometimes OpLength is not correct (MOV : Len=0 instead of 2...)
		// const Natural	OpLength =	MMask_And(D3DSI_INSTLENGTH_MASK, Token)>>D3DSI_INSTLENGTH_SHIFT;
		const Bool		IsPredic =	MMask_And(D3DSHADER_INSTRUCTION_PREDICATED, Token)!=0;
		const Bool		IsCoIssue =	MMask_And(D3DSI_COISSUE, Token)!=0;
		const Bool		IsDst_RegDst = True;
		const Bool		IsDst_RegSrc = False;
		Integer			Instr_SpaceAfter = ESpace_Instr;
		if (IsCoIssue) {
			Instr_SpaceAfter -= CatString("+");
		}
		if (IsPredic) 
		{	// Predicated instruction parameters order is
			// Token_Instr Token_Param1 Token_Predicate Token_Param2 Token_Param3 ...
			const Natural	iTokenInstr = iToken;
			iToken += 2;
			Instr_SpaceAfter -= CatChar('(');
			Instr_SpaceAfter -= CatParamRegSrc(1, 4);
			Instr_SpaceAfter -= CatChar(')');
			iToken = iTokenInstr;
		}
		if (Instr_SpaceAfter<0)
			Instr_SpaceAfter = 0;
		switch (OpCode)
		{
#define MMwTempoMacro0(__InstrId)							\
		case D3DSIO_##__InstrId:							\
			CatInstr(#__InstrId, False, Instr_SpaceAfter);	\
			break;
#define MMwTempoMacro1(__InstrId, _1)							\
		case D3DSIO_##__InstrId:								\
			CatInstr(#__InstrId, IsDst_##_1, Instr_SpaceAfter);	\
			CatParam##_1();										\
			break;
#define MMwTempoMacro2(__InstrId, _1,_2)						\
		case D3DSIO_##__InstrId:								\
			CatInstr(#__InstrId, IsDst_##_1, Instr_SpaceAfter);	\
			CatParam##_1();										\
			CatString(", ");									\
			if (IsPredic) iToken++; /* Skip Predic Token */		\
			CatParam##_2();										\
			break;
#define MMwTempoMacrN2(__InstrId, __Name, _1,_2)				\
		case D3DSIO_##__InstrId:								\
			CatInstr(__Name, IsDst_##_1, Instr_SpaceAfter);		\
			CatParam##_1();										\
			CatString(", ");									\
			if (IsPredic) iToken++; /* Skip Predic Token */		\
			CatParam##_2();										\
			break;
#define MMwTempoMacro3(__InstrId, _1,_2,_3)						\
		case D3DSIO_##__InstrId:								\
			CatInstr(#__InstrId, IsDst_##_1, Instr_SpaceAfter);	\
			CatParam##_1();										\
			CatString(", ");									\
			if (IsPredic) iToken++; /* Skip Predic Token */		\
			CatParam##_2();										\
			CatString(", ");									\
			CatParam##_3();										\
			break;
#define MMwTempoMacro4(__InstrId, _1,_2,_3,_4)					\
		case D3DSIO_##__InstrId:								\
			CatInstr(#__InstrId, IsDst_##_1, Instr_SpaceAfter);	\
			CatParam##_1();										\
			CatString(", ");									\
			if (IsPredic) iToken++; /* Skip Predic Token */		\
			CatParam##_2();										\
			CatString(", ");									\
			CatParam##_3();										\
			CatString(", ");									\
			CatParam##_4();										\
			break;
#define MMwTempoMacro5(__InstrId, _1,_2,_3,_4,_5)				\
		case D3DSIO_##__InstrId:								\
			CatInstr(#__InstrId, IsDst_##_1, Instr_SpaceAfter);	\
			CatParam##_1();										\
			CatString(", ");									\
			if (IsPredic) iToken++; /* Skip Predic Token */		\
			CatParam##_2();										\
			CatString(", ");									\
			CatParam##_3();										\
			CatString(", ");									\
			CatParam##_4();										\
			CatString(", ");									\
			CatParam##_5();										\
			break;

#include "Dx9ShaderBCode_Instr.h"

		// Dcl special case
		case D3DSIO_DCL:
			{
				Natural	cChar = 0;
				cChar += CatInstr("dcl", False, 0);	
				const Natural	Semantic = _Tokens[iToken++];
				const Natural	RegType = Disasm_RegisterGetType(_Tokens[iToken]);
				if (RegType==D3DSPR_INPUT && (!IsPixelShader || VersionId>=0x0300))
				{
					const char*	UsageNames[] = {
						"position", "blendweight", "blendindices", "normal", "psize", "texcoord",
						"tangent", "binormal", "tessfactor", "positiont", "color", "fog", "depth", "sample"
					};
					const Natural	Usage		= MMask_And(Semantic, D3DSP_DCL_USAGE_MASK)>>D3DSP_DCL_USAGE_SHIFT;
					const Natural	UsageIndex	= MMask_And(Semantic, D3DSP_DCL_USAGEINDEX_MASK)>>D3DSP_DCL_USAGEINDEX_SHIFT;
					cChar += CatChar('_');
					if (UsageIndex < sizeof(UsageNames)/sizeof(UsageNames[0]))
						cChar += CatString(UsageNames[Usage]);
					else
						cChar += CatChar('?');
					cChar += CatNatural(UsageIndex);
				}
				if (RegType==D3DSPR_SAMPLER && (IsPixelShader || VersionId>=0x0300)) 
				{
					const Natural	TextureType = MMask_And(Semantic, D3DSP_TEXTURETYPE_MASK)>>D3DSP_TEXTURETYPE_SHIFT;
					const char*	TypeNames[] = {
						"2d", "cube", "volume"
					};
					if (TextureType-2 < sizeof(TypeNames)/sizeof(TypeNames[0])) {
						cChar += CatChar('_');
						cChar += CatString(TypeNames[TextureType-2]);
					}
				}
				const Natural	MinChar = 20;
				if (cChar<MinChar)
					CatSpaces(MinChar-cChar);
				CatParamRegDst();			
			}
			break;

		// Version dependant instructions
		case D3DSIO_TEXCOORD:
			if (VersionId<0x0104)
			{
				CatInstr("texcoord", True);
				CatParamRegDst();
			}
			else
			{
				CatInstr("texcrd", True);
				CatParamRegDst();
				CatString(", ");
				CatParamRegSrc();
			}
			break;

		case D3DSIO_TEX:
			if (VersionId<0x0104)
			{
				CatInstr("tex", True);
				CatParamRegDst();
			}
			else if (VersionId==0x0104)
			{
				CatInstr("texld", True);
				CatParamRegDst();
				CatString(", ");
				CatParamRegSrc();
			}
			else {
				const char*	Name;
				if		(MMask_And(Token, D3DSI_TEXLD_PROJECT))		Name = "texldp";
				else if (MMask_And(Token, D3DSI_TEXLD_BIAS))		Name = "texldb";
				else												Name = "texld";
				CatInstr(Name, True);
				CatParamRegDst();
				CatString(", ");
				CatParamRegSrc();
				CatString(", ");
				CatParamRegSrc();
			}
			break;

		case D3DSIO_SINCOS:
			CatInstr("sincos", True);
			CatParamRegDst();
			CatString(", ");
			CatParamRegSrc();
			if (VersionId<0x0300)
			{
				CatString(", ");
				CatParamRegSrc();
				CatString(", ");
				CatParamRegSrc();
			}
			break;

		default:
			Assert(False);
			sprintf(StrLine, "Invalid token (0x%08X)", Token);
			iToken++;
			break;
		}
#undef MMwTempoMacro0
#undef MMwTempoMacro1
#undef MMwTempoMacro2
#undef MMwTempoMacro3
#undef MMwTempoMacro4
#undef MMwTempoMacro5
		
		CatLine();
	}
	return Output.Chars;
}

#ifndef __Dx9ShaderDisasm__584331__
#define __Dx9ShaderDisasm__584331__

#include "StdTypes.h"
#include <String.h>

class CDx9ShaderDisasm
{
public:
	static const char*	DisAssemble(Nat32* _Tokens, Natural _cToken);
	static void			FreeMemory() {Output.Free();}

	static const char*	StrNewLine;	// default is "\r\n"

protected:
	static Natural	CatChar(const char _Char) {
		const Natural	iLast = strlen(StrLine);
		StrLine[iLast+0] = _Char;
		StrLine[iLast+1] = 0;
		return 1;
	}
	static Natural	CatString(const char* _String) {
		const Natural	iLast = strlen(StrLine);
		const Natural	cCat = strlen(_String);
		memcpy(StrLine+iLast, _String, cCat+1);
		return cCat;
	}
	static Natural	CatSpaces(Natural _SpaceCount) {
		return CatString(StrSpaces + cSpace-_SpaceCount);
	}
	enum {
		ESpace_Instr = 11,
	};

	static Natural	CatStringLower(const char* _String);
	static Natural	CatNatural(Natural _Nat, Natural _cCharMin=0);
	static Natural	CatInstr(const char* _Name, Bool _IsParam1Dst, Natural _cSpaceAfter=ESpace_Instr);
	static Natural	CatRegister(Nat32 _Token);
	static Natural	CatParamRegSrc(Natural _cSpaceBefore=2, Natural _cSpaceAfter=12);
	static void		CatParamRegDst();
	static void		CatParamReal();
	static void		CatParamBool();
	static void		CatParamInteger();
	static void		CatLine() {Output.CatLine(StrLine);}


	static Nat32*			Tokens;
	static Natural			iToken;
	static Bool				IsPixelShader;
	static Natural			VersionId;
	static char				StrLine[256];
	static const char*		StrSpaces;
	static const Natural	cSpace;
	// Output 
	struct SOutput {
		SOutput() {Chars=Null; Count=0; Alloc=0;}
		~SOutput();
		void		Free();
		void		Reset() {Count=0;}
		void		CatLine(const char* _Line);
		char*		Chars;
		Natural		Count;
		Natural		Alloc;
	};
	static SOutput			Output;
};

#endif
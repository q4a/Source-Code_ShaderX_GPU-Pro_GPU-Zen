#include "PS20Assembler.hpp"

#include "Instruction.hpp"
#include "Error.hpp"
#include "Texture.hpp"

#include <float.h>

namespace swShader
{
	float4 PS20Assembler::r[12];
	float4 PS20Assembler::t[8];
	float4 PS20Assembler::s[16];
	float4 PS20Assembler::v[2];
	float4 PS20Assembler::c[32];
	float4 PS20Assembler::oC[4];
	float4 PS20Assembler::oDepth;

	float4 PS20Assembler::internal[8];

	Operand PS20Assembler::tmp0(Operand::INTERNAL_REGISTER, 0);
	Operand PS20Assembler::tmp1(Operand::INTERNAL_REGISTER, 1);
	Operand PS20Assembler::tmp2(Operand::INTERNAL_REGISTER, 2);
	Operand PS20Assembler::tmp3(Operand::INTERNAL_REGISTER, 3);
	Operand PS20Assembler::tmp4(Operand::INTERNAL_REGISTER, 4);
	Operand PS20Assembler::tmp5(Operand::INTERNAL_REGISTER, 5);
	Operand PS20Assembler::tmp6(Operand::INTERNAL_REGISTER, 6);
	Operand PS20Assembler::tmp7(Operand::INTERNAL_REGISTER, 7);

	PS20Assembler::PS20Assembler()
	{
		intermediate = new Instruction();
		instruction = intermediate;
	}

	PS20Assembler::~PS20Assembler()
	{
		delete intermediate;
		intermediate = 0;
	}

	void (*PS20Assembler::shader())()
	{
		return callable();
	}

	void PS20Assembler::setConstant(int index, const float value[4])
	{
		if(index < 0 || index >= 32) throw Error("Constant register index out of [0, 31] range: %d", index);

		c[index][0] = value[0];
		c[index][1] = value[1];
		c[index][2] = value[2];
		c[index][3] = value[3];
	}

	void PS20Assembler::setMnemonic(Instruction::Mnemonic mnemonic)
	{
		instruction->mnemonic = mnemonic;
	}

	void PS20Assembler::setModifier(Instruction::Modifier modifier)
	{
		instruction->modifier = modifier;
	}

	void PS20Assembler::setDestination(const Operand &operand)
	{
		instruction->destination = operand;
	}

	void PS20Assembler::setSource0(const Operand &operand)
	{
		instruction->source0 = operand;
	}

	void PS20Assembler::setSource1(const Operand &operand)
	{
		instruction->source1 = operand;
	}

	void PS20Assembler::setSource2(const Operand &operand)
	{
		instruction->source2 = operand;
	}

	void PS20Assembler::setSource3(const Operand &operand)
	{
		instruction->source3 = operand;
	}

	void PS20Assembler::newInstruction()
	{
		instruction = instruction->newNext();
	}

	void PS20Assembler::encode()
	{
		if(!intermediate) throw INTERNAL_ERROR;

		setEchoFile("Shader.asm");

		try
		{
			pushad();
			freeAll();

			mov(ecx, dword_ptr [&lx]);
			cmp(ecx, dword_ptr [&rx]);
			jge("return");

				setupInterpolants();

			mov(edi, dword_ptr [&colorBuffer]);
			mov(edx, dword_ptr [&depthBuffer]);

		label("scanlineLoop");

				pixel();

			inc(ecx);
			cmp(ecx, dword_ptr [&rx]);
			jnge("scanlineLoop");

		label("endScanline");

			emms();

		label("return");
			popad();
			ret();
		}
		catch(const Error &error)
		{
			throw Error("Fatal pixel shader assembler error: ") << error;
		}
		catch(...)
		{
			throw INTERNAL_ERROR;
		}
	}

	void PS20Assembler::setupInterpolants()
	{
		for(int i = 0; i < 2; i++) vDcl[i] = false;
		for(int i = 0; i < 8; i++) tDcl[i] = false;
		for(int i = 0; i < 16; i++) sDcl[i] = false;

		instruction = intermediate;

		if(instruction->mnemonic == Instruction::PS_2_0)
		{
			PS_2_0();
		}
		else
		{
			throw Error("First shader instruction should be PS_2_0");
		}

		instruction = instruction->next;

		while(instruction)
		{
			Operand &dest = instruction->destination;

			if(instruction->mnemonic == Instruction::DCL)			DCL(dest);
			if(instruction->mnemonic == Instruction::DCL_2D)		DCL_2D(dest);
			if(instruction->mnemonic == Instruction::DCL_CUBE)		DCL_CUBE(dest);
			if(instruction->mnemonic == Instruction::DCL_VOLUME)	DCL_VOLUME(dest);

			instruction = instruction->next;
		};

		if(vDcl[0])
		{
			movaps(xmm0, xmmword_ptr [&C]);
			movaps(xmmword_ptr [&v[0]], xmm0);
		}

		if(vDcl[1])
		{
			movaps(xmm0, xmmword_ptr [&L]);
			movaps(xmmword_ptr [&v[1]], xmm0);
		}

		for(i = 0; i < 8; i++)
		{
			if(tDcl[i])
			{
				movaps(xmm0, xmmword_ptr [&T[i]]);
				movaps(xmmword_ptr [&t[i]], xmm0);
			}
		}

		// Reset to detect usage before declaration
		for(i = 0; i < 2; i++) vDcl[i] = false;
		for(i = 0; i < 8; i++) tDcl[i] = false;
		for(i = 0; i < 16; i++) sDcl[i] = false;

		movss(xmm0, dword_ptr [&w]);
		movss(dword_ptr [&RHW], xmm0);

		movss(xmm0, dword_ptr [&z]);
		movss(dword_ptr [&Z], xmm0);
	}

	void PS20Assembler::pixel()
	{
		instruction = intermediate;

		depthTest();

		while(instruction)
		{
			Operand &dst = instruction->destination;
			Operand &dest = instruction->destination;
			const Operand &src = instruction->source0;
			const Operand &src0 = instruction->source0;
			const Operand &src1 = instruction->source1;
			const Operand &src2 = instruction->source2;
			const Operand &src3 = instruction->source3;
			const Operand &fValue1 = instruction->source0;
			const Operand &fValue2 = instruction->source1;
			const Operand &fValue3 = instruction->source2;
			const Operand &fValue4 = instruction->source3;

			switch(instruction->mnemonic)
			{
			case Instruction::PS_2_0:		PS_2_0();										break;

			case Instruction::DCL:			DCL(dest);										break;
			case Instruction::DCL_2D:		DCL_2D(dest);									break;
			case Instruction::DCL_CUBE:		DCL_CUBE(dest);									break;
			case Instruction::DCL_VOLUME:	DCL_VOLUME(dest);								break;

			case Instruction::DEF:			DEF(dest, fValue1, fValue2, fValue3, fValue4);	break;

			case Instruction::ABS:			ABS(dst, src);									break;
			case Instruction::ADD:			ADD(dst, src0, src1);							break;
			case Instruction::CMP:			CMP(dst, src0, src1, src2);						break;
			case Instruction::CRS:			CRS(dst, src0, src1);							break;
			case Instruction::DP2ADD:		DP2ADD(dst, src0, src1, src2);					break;
			case Instruction::DP3:			DP3(dst, src0, src1);							break;
			case Instruction::DP4:			DP4(dst, src0, src1);							break;
			case Instruction::EXP:			EXP(dst, src);									break;
			case Instruction::FRC:			FRC(dst, src);									break;
			case Instruction::LOG:			LOG(dst, src);									break;
			case Instruction::LRP:			LRP(dst, src0, src1, src2);						break;
			case Instruction::M3X2:			M3X2(dst, src0, src1);							break;
			case Instruction::M3X3:			M3X3(dst, src0, src1);							break;
			case Instruction::M3X4:			M3X4(dst, src0, src1);							break;
			case Instruction::M4X3:			M4X3(dst, src0, src1);							break;
			case Instruction::M4X4:			M4X4(dst, src0, src1);							break;
			case Instruction::MAD:			MAD(dst, src0, src1, src2);						break;
			case Instruction::MAX:			MAX(dst, src0, src1);							break;
			case Instruction::MIN:			MIN(dst, src0, src1);							break;
			case Instruction::MOV:			MOV(dst, src);									break;
			case Instruction::MUL:			MUL(dst, src0, src1);							break;
			case Instruction::NOP:			NOP();											break;
			case Instruction::NRM:			NRM(dst, src);									break;
			case Instruction::POW:			POW(dst, src0, src1);							break;
			case Instruction::RCP:			RCP(dst, src);									break;
			case Instruction::RSQ:			RSQ(dst, src);									break;
			case Instruction::SINCOS:		SINCOS(dst, src0, src1, src2);					break;
			case Instruction::SUB:			SUB(dst, src0, src1);							break;

			case Instruction::TEXKILL:		TEXKILL(src);									break;
			case Instruction::TEXLD:		TEXLD(dst, src0, src1);							break;
			case Instruction::TEXLDB:		TEXLDB(dst, src0, src1);						break;
			case Instruction::TEXLDP:		TEXLDP(dst, src0, src1);						break;

			case Instruction::INVALID:														break;
			default:						throw INTERNAL_ERROR;
			}

			freeTemps();

			annotate("\n");

			instruction = instruction->next;
		}

		writeOC0();

	label("texkill");

		interpolate();
	}

	void PS20Assembler::depthTest()
	{
		movss(r128(tmp0), dword_ptr [&Z]);
		comiss(r128(tmp0), dword_ptr [edx+4*ecx]);

		if(depthCompareMode == COMPARE_ALWAYS)
		{
		}
		else if(depthCompareMode == COMPARE_NEVER)
		{
			jmp("texkill");
		}
		if(depthCompareMode == COMPARE_LESS)
		{	
			jnb("texkill");
		}
		else if(depthCompareMode == COMPARE_GREATER_EQUAL)
		{
			jnae("texkill");
		}
		else if(depthCompareMode == COMPARE_LESS_EQUAL)
		{
			jnbe("texkill");
		}
		else if(depthCompareMode == COMPARE_GREATER)
		{
			jna("texkill");
		}
		else
		{
			throw INTERNAL_ERROR;
		}

		if(depthWriteEnable)
		{
			movss(dword_ptr [edx+4*ecx], r128(tmp0));
		}

		freeTemps();
	}

	void PS20Assembler::writeOC0()
	{
		static const float4 _255 = {255, 255, 255, 255};

		movaps(x128(tmp0), m128(oC0Register));
		mulps(r128(tmp0), xmmword_ptr [&_255]);
		cvtps2pi(mm0, r128(tmp0));
		movhlps(r128(tmp0), r128(tmp0));
		cvtps2pi(mm1, r128(tmp0));
		packssdw(mm0, mm1);
		pshufw(mm0, mm0, 0xC6);
		packuswb(mm0, mm0);
		movd(dword_ptr [edi+4*ecx], mm0);

		freeTemps();
	}

	void PS20Assembler::interpolate()
	{
		// Vertex color/light interpolation
		if(vDcl[0]) addps(r128(v0Register), xmmword_ptr [&dC_dx]);
		if(vDcl[1]) addps(r128(v1Register), xmmword_ptr [&dL_dx]);

		// Texture coordinate interpolation
		if(tDcl[0]) addps(r128(t0Register), xmmword_ptr [&dT_dx[0]]);
		if(tDcl[1]) addps(r128(t1Register), xmmword_ptr [&dT_dx[1]]);
		if(tDcl[2]) addps(r128(t2Register), xmmword_ptr [&dT_dx[2]]);
		if(tDcl[3]) addps(r128(t3Register), xmmword_ptr [&dT_dx[3]]);
		if(tDcl[4]) addps(r128(t4Register), xmmword_ptr [&dT_dx[4]]);
		if(tDcl[5]) addps(r128(t5Register), xmmword_ptr [&dT_dx[5]]);
		if(tDcl[6]) addps(r128(t6Register), xmmword_ptr [&dT_dx[6]]);
		if(tDcl[7]) addps(r128(t7Register), xmmword_ptr [&dT_dx[7]]);

		movss(r128(tmp0), dword_ptr[&RHW]);
		addss(r128(tmp0), dword_ptr[&dw_dx]);
		movss(dword_ptr [&RHW], r128(tmp0));

		movss(r128(tmp0), dword_ptr[&Z]);
		addss(r128(tmp0), dword_ptr[&dz_dx]);
		movss(dword_ptr [&Z], r128(tmp0));

		freeTemps();
		spillAll();
	}

	void *PS20Assembler::reference(const Operand &reg)
	{
		switch(reg.type)
		{
		case Operand::COLOR_REGISTER:
			return &v[reg.index];
		case Operand::CONSTANT_FLOAT_REGISTER:
			return &c[reg.index];
		case Operand::INPUT_TEXTURE_COORDINATE_REGISTER:
			return &t[reg.index];
	//	case Operand::SAMPLER_REGISTER:
	//		return 0;
		case Operand::TEMPORARY_REGISTER:
			return &r[reg.index];
		case Operand::OUTPUT_COLOR_REGISTER:
			return &v[reg.index];
		case Operand::OUTPUT_DEPTH_REGISTER:
			return &oDepth;
	//	case Operand::CONSTANT_FLOAT_LITERAL:
	//		return 0;
		case Operand::INTERNAL_REGISTER:
			return &internal[reg.index];
		default:
			throw INTERNAL_ERROR;
		}
	}

	const OperandXMMREG PS20Assembler::r128(const Operand &reg, int next)
	{
		checkDcl(reg);

		Operand op = reg;
		op.index += next;

		return CodeGenerator::r128(reference(op));
	}

	const OperandXMMREG PS20Assembler::x128(const Operand &reg, int next)
	{
		checkDcl(reg);

		Operand op = reg;
		op.index += next;

		return CodeGenerator::x128(reference(op));
	}

	const OperandR_M128 PS20Assembler::m128(const Operand &r_m, int next)
	{
		checkDcl(r_m);

		Operand op = r_m;
		op.index += next;

		return CodeGenerator::m128(reference(op));
	}

	const OperandXMM32 PS20Assembler::xmm32(const Operand &r_m, int next)
	{
		return (OperandXMM32&)m128(r_m, next);
	}

	void PS20Assembler::checkDcl(const Operand &op)
	{
		if(op.type == Operand::COLOR_REGISTER)
		{
			if(op.index < 0 || op.index >= 2) throw INTERNAL_ERROR;

			if(vDcl[op.index] == true) return;
			else throw Error("Use of undeclared color register v%d", op.index);
		}
		else if(op.type == Operand::INPUT_TEXTURE_COORDINATE_REGISTER)
		{
			if(op.index < 0 || op.index >= 8) throw INTERNAL_ERROR;

			if(tDcl[op.index] == true) return;
			else throw Error("Use of undeclared input texture register t%d", op.index);
		}
		else if(op.type == Operand::SAMPLER_REGISTER)
		{
			if(op.index < 0 || op.index >= 16) throw INTERNAL_ERROR;

			if(sDcl[op.index] == true) return;
			else throw Error("Use of undeclared sampler register s%d", op.index);
		}
	}

	void PS20Assembler::free(const Operand &tmp)
	{
		CodeGenerator::free(reference(tmp));
	}

	void PS20Assembler::freeTemps()
	{
		free(tmp0);
		free(tmp1);
		free(tmp2);
		free(tmp3);
		free(tmp4);
		free(tmp5);
		free(tmp6);
		free(tmp7);
	}

	void PS20Assembler::PS_2_0()
	{
		return;
	}

	void PS20Assembler::DCL(Operand &dest)
	{
		if(dest.type == Operand::COLOR_REGISTER)
		{
			if(dest.index < 0 || dest.index >= 2) throw INTERNAL_ERROR;

			vDcl[dest.index] = true;
		}
		else if(dest.type == Operand::INPUT_TEXTURE_COORDINATE_REGISTER)
		{
			if(dest.index < 0 || dest.index >= 8) throw INTERNAL_ERROR;

			tDcl[dest.index] = true;
		}
		else
		{
			throw Error("Only color and texture registers can be DCL'ed");
		}
	}

	void PS20Assembler::DCL_2D(Operand &dest)
	{
		if(dest.type != Operand::SAMPLER_REGISTER) throw INTERNAL_ERROR;
		if(dest.index < 0 || dest.index > 15) throw INTERNAL_ERROR;
		if(!sampler[dest.index].texture) throw Error("No texture set for declared sampler s%d", dest.index);
		sDcl[dest.index] = true;
	}

	void PS20Assembler::DCL_CUBE(Operand &dest)
	{
		throw Error("Cube maps not supported");
	}

	void PS20Assembler::DCL_VOLUME(Operand &dest)
	{
		throw Error("Volume textures not supported");
	}

	void PS20Assembler::DEF(Operand &dest, FValue1 fValue1, FValue2 fValue2, FValue3 fValue3, FValue4 fValue4)
	{
		c[dest.index][0] = fValue1.value;
		c[dest.index][1] = fValue2.value;
		c[dest.index][2] = fValue3.value;
		c[dest.index][3] = fValue4.value;
	}

	void PS20Assembler::ABS(Operand &dst, const Operand &src)
	{
		static const int4 SIGN_MASK = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};

		NEG_SWIZZLE(tmp0, src);

		andps(r128(tmp0), xmmword_ptr [SIGN_MASK]);

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::ADD(Operand &dst, const Operand &src0, const Operand &src1)
	{
		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);

		addps(r128(tmp0), m128(tmp1));

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::CMP(Operand &dst, const Operand &src0, const Operand &src1, const Operand &src2)
	{
		static const int4 ZERO = {0x00000000, 0x00000000, 0x00000000, 0x00000000};

		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);
		NEG_SWIZZLE(tmp2, src2);
		
		cmpltps(r128(tmp0), xmmword_ptr [ZERO]);
		andps(r128(tmp2), m128(tmp0));
		andnps(r128(tmp0), m128(tmp1));
		orps(r128(tmp2), m128(tmp0));

		SAT_MASK(dst, tmp2);
	}

	void PS20Assembler::CRS(Operand &dst, const Operand &src0, const Operand &src1)
	{
		// dest.x = src0.y * src1.z - src0.z * src1.y;
		// dest.y = src0.z * src1.x - src0.x * src1.z;
		// dest.z = src0.x * src1.y - src0.y * src1.x;

		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);

		movaps(x128(tmp3), m128(tmp0));
		movaps(x128(tmp2), m128(tmp1));
		shufps(r128(tmp3), m128(tmp0), 0xD2);
		shufps(r128(tmp2), m128(tmp1), 0xC9);
		mulps(r128(tmp3), m128(tmp2));
		movaps(x128(tmp2), m128(tmp1));
		shufps(r128(tmp2), m128(tmp1), 0xD2);
		movaps(x128(tmp1), m128(tmp0));
		shufps(r128(tmp1), m128(tmp0), 0xD9);
		mulps(r128(tmp1), m128(tmp2));
		subps(r128(tmp1), m128(tmp3));

		SAT_MASK(dst, tmp1);
	}

	void PS20Assembler::DP2ADD(Operand &dst, const Operand &src0, const Operand &src1, const Operand &src2)
	{
		// dest = src0.r * src1.r + src0.g * src1.g + src2.selected_component

		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);
		
		mulps(r128(tmp0), m128(tmp1));
		movlps(r128(tmp1), r128(tmp0));
		shufps(r128(tmp1), m128(tmp1), 0x01);
		addss(r128(tmp0), xmm32(tmp1));
		shufps(r128(tmp0), m128(tmp0), 0x55);

		SAT_MOV_X(dst, tmp0);
	}

	void PS20Assembler::DP3(Operand &dst, const Operand &src0, const Operand &src1)
	{
		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);

		mulps(r128(tmp0), m128(tmp1));
		movhlps(r128(tmp1), r128(tmp0));
		addss(r128(tmp1), xmm32(tmp0));
		shufps(r128(tmp0), m128(tmp0), 0x01);
		addss(r128(tmp0), xmm32(tmp1));

		SAT_MOV_X(dst, tmp0);
	}

	void PS20Assembler::DP4(Operand &dst, const Operand &src0, const Operand &src1)
	{
		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);

		mulps(r128(tmp0), m128(tmp1));
		movhlps(r128(tmp1), r128(tmp0));
		addps(r128(tmp0), m128(tmp1));
		movss(r128(tmp1), xmm32(tmp0));
		shufps(r128(tmp0), m128(tmp0), 0x01);
		addss(r128(tmp1), xmm32(tmp0));

		SAT_MOV_X(dst, tmp1);
	}

	void PS20Assembler::EXP(Operand &dst, const Operand &src)
	{
		static const float A = -2.91421356e+0f;

		static float TEMP;

		NEG_SWIZZLE(tmp0, src);

		movss(r128(tmp1), xmm32(tmp0));
		cvtss2si(eax, xmm32(tmp1));
		cvtsi2ss(r128(tmp1), eax);
		subss(r128(tmp0), xmm32(tmp1));
		movss(r128(tmp1), dword_ptr [&A]);
		subss(r128(tmp1), xmm32(tmp0));
		addss(r128(tmp0), dword_ptr [&A]);
		rcpss(r128(tmp0), xmm32(tmp0));
		mulss(r128(tmp0), xmm32(tmp1));
		add(eax, 127);
		shl(eax, 23);
		mov(dword_ptr [&TEMP], eax);
		mulss(r128(tmp0), dword_ptr [&TEMP]);
		shufps(r128(tmp0), r128(tmp0), 0x00);

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::FRC(Operand &dst, const Operand &src)
	{
		static const float4 HALF = {0.5f, 0.5f, 0.5f, 0.5f};
		static const int4 MASK = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};

		NEG_SWIZZLE(tmp0, src);

		andps(r128(tmp0), xmmword_ptr [MASK]);
		subps(r128(tmp0), xmmword_ptr [HALF]);
		cvtps2pi(mm0, r128(tmp0));
		movhlps(r128(tmp1), r128(tmp0));
		cvtps2pi(mm1, r128(tmp1));
		cvtpi2ps(r128(tmp1), mm1);
		movlhps(r128(tmp1), r128(tmp1));
		cvtpi2ps(r128(tmp1), mm0);
		addps(r128(tmp0), xmmword_ptr [HALF]);
		subps(r128(tmp0), m128(tmp1));

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::LOG(Operand &dst, const Operand &src)
	{
		static const float A =  3.42234550e0f;
		static const float B =  1.42234550e0f;

		static float TEMP1;
		static float TEMP2;

		NEG_SWIZZLE(tmp0, src);

		movss(dword_ptr [&TEMP1], r128(tmp0));
		mov(eax, dword_ptr [&TEMP1]);
		and(eax, 0x007FFFFF);
		or(eax, 0x3F800000);
		mov(dword_ptr [&TEMP2], eax);
		movss(r128(tmp0), dword_ptr [&TEMP2]);
		movss(r128(tmp1), xmm32(tmp0));
		mulss(r128(tmp0), dword_ptr [&A]);
		subss(r128(tmp0), dword_ptr [&A]);
		addss(r128(tmp1), dword_ptr [&B]);
		rcpss(r128(tmp1), xmm32(tmp1));
		mulss(r128(tmp0), xmm32(tmp1));
		mov(eax, dword_ptr [&TEMP1]);
		and(eax, 0x7F800000);
		shr(eax, 23);
		sub(eax, 127);
		cvtsi2ss(r128(tmp1), eax);
		addss(r128(tmp0), xmm32(tmp1));
		shufps(r128(tmp0), r128(tmp0), 0x00);

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::LRP(Operand &dst, const Operand &src0, const Operand &src1, const Operand &src2)
	{
		// dest = src2 + src0 * (src1 - src2)

		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);
		NEG_SWIZZLE(tmp2, src2);

		subps(r128(tmp1), m128(tmp2));
		mulps(r128(tmp1), m128(tmp0));
		addps(r128(tmp1), m128(tmp2));

		SAT_MASK(dst, tmp1);
	}

	void PS20Assembler::M3X2(Operand &dst, const Operand &src0, const Operand &src1)
	{
		movaps(x128(tmp0), m128(src1, 0));
		movaps(x128(tmp1), m128(src1, 1));
		movaps(x128(tmp2), m128(tmp0));
		movaps(x128(tmp3), m128(tmp1)); 

		unpckhps(r128(tmp0), m128(tmp0));
		unpckhps(r128(tmp1), m128(tmp1));
		movaps(x128(tmp4), m128(tmp0));
		unpcklps(r128(tmp4), m128(tmp1));

		unpcklps(r128(tmp2), m128(tmp2));
		unpcklps(r128(tmp3), m128(tmp3));
		movaps(x128(tmp5), m128(tmp2));
		unpckhps(r128(tmp2), m128(tmp3));
		unpcklps(r128(tmp5), m128(tmp3));

		NEG_SWIZZLE(tmp1, src0);

		movaps(x128(tmp6), m128(tmp1));
		movaps(x128(tmp7), m128(tmp1));
		shufps(r128(tmp1), m128(tmp1), 0x00);
		shufps(r128(tmp6), m128(tmp6), 0x55);
		shufps(r128(tmp7), m128(tmp7), 0xAA);

		mulps(r128(tmp1), m128(tmp5));
		mulps(r128(tmp6), m128(tmp2));
		mulps(r128(tmp7), m128(tmp4));

		addps(r128(tmp1), m128(tmp6));
		addps(r128(tmp1), m128(tmp7));

		SAT_MOV_XY(dst, tmp1);
	}

	void PS20Assembler::M3X3(Operand &dst, const Operand &src0, const Operand &src1)
	{
		movaps(x128(tmp0), m128(src1, 0));
		movaps(x128(tmp1), m128(src1, 1));
		movaps(x128(tmp2), m128(tmp0));
		movaps(x128(tmp3), m128(tmp1)); 

		unpckhps(r128(tmp0), m128(src1, 2));
		unpckhps(r128(tmp1), m128(tmp1));
		movaps(x128(tmp4), m128(tmp0));
		unpcklps(r128(tmp4), m128(tmp1));

		unpcklps(r128(tmp2), m128(src1, 2));
		unpcklps(r128(tmp3), m128(tmp3));
		movaps(x128(tmp5), m128(tmp2));
		unpckhps(r128(tmp2), m128(tmp3));
		unpcklps(r128(tmp5), m128(tmp3));

		NEG_SWIZZLE(tmp1, src0);

		movaps(x128(tmp6), m128(tmp1));
		movaps(x128(tmp7), m128(tmp1));
		shufps(r128(tmp1), m128(tmp1), 0x00);
		shufps(r128(tmp6), m128(tmp6), 0x55);
		shufps(r128(tmp7), m128(tmp7), 0xAA);

		mulps(r128(tmp1), m128(tmp5));
		mulps(r128(tmp6), m128(tmp2));
		mulps(r128(tmp7), m128(tmp4));

		addps(r128(tmp1), m128(tmp6));
		addps(r128(tmp1), m128(tmp7));

		SAT_MOV_XYZ(dst, tmp1);
	}

	void PS20Assembler::M3X4(Operand &dst, const Operand &src0, const Operand &src1)
	{
		movaps(x128(tmp0), m128(src1, 0));
		movaps(x128(tmp1), m128(src1, 1));
		movaps(x128(tmp2), m128(tmp0));
		movaps(x128(tmp3), m128(tmp1)); 

		unpckhps(r128(tmp0), m128(src1, 2));
		unpckhps(r128(tmp1), m128(src1, 3));
		movaps(x128(tmp4), m128(tmp0));
		unpcklps(r128(tmp4), m128(tmp1));

		unpcklps(r128(tmp2), m128(src1, 2));
		unpcklps(r128(tmp3), m128(src1, 3));
		movaps(x128(tmp5), m128(tmp2));
		unpckhps(r128(tmp2), m128(tmp3));
		unpcklps(r128(tmp5), m128(tmp3));

		NEG_SWIZZLE(tmp1, src0);

		movaps(x128(tmp6), m128(tmp1));
		movaps(x128(tmp7), m128(tmp1));
		shufps(r128(tmp1), m128(tmp1), 0x00);
		shufps(r128(tmp6), m128(tmp6), 0x55);
		shufps(r128(tmp7), m128(tmp7), 0xAA);

		mulps(r128(tmp1), m128(tmp5));
		mulps(r128(tmp6), m128(tmp2));
		mulps(r128(tmp7), m128(tmp4));

		addps(r128(tmp1), m128(tmp6));
		addps(r128(tmp1), m128(tmp7));

		SAT_MOV_XYZW(dst, tmp1);
	}

	void PS20Assembler::M4X3(Operand &dst, const Operand &src0, const Operand &src1)
	{
		movaps(x128(tmp0), m128(src1, 0));
		movaps(x128(tmp1), m128(src1, 1));
		movaps(x128(tmp2), m128(tmp0));
		movaps(x128(tmp3), m128(tmp1)); 

		unpckhps(r128(tmp0), m128(src1, 2));
		unpckhps(r128(tmp1), m128(tmp1));
		movaps(x128(tmp4), m128(tmp0)); 
		unpckhps(r128(tmp0), m128(tmp1));
		unpcklps(r128(tmp4), m128(tmp1));

		unpcklps(r128(tmp2), m128(src1, 2));
		unpcklps(r128(tmp3), m128(tmp3));
		movaps(x128(tmp5), m128(tmp2));
		unpckhps(r128(tmp2), m128(tmp3));
		unpcklps(r128(tmp5), m128(tmp3));

		NEG_SWIZZLE(tmp1, src0);

		movaps(x128(tmp6), m128(tmp1));
		movaps(x128(tmp7), m128(tmp1));
		movaps(x128(tmp3), m128(tmp1));
		shufps(r128(tmp1), m128(tmp1), 0x00);
		shufps(r128(tmp6), m128(tmp6), 0x55);
		shufps(r128(tmp7), m128(tmp7), 0xAA);
		shufps(r128(tmp3), m128(tmp3), 0xFF);

		mulps(r128(tmp1), m128(tmp5));
		mulps(r128(tmp6), m128(tmp2));
		mulps(r128(tmp7), m128(tmp4));
		mulps(r128(tmp3), m128(tmp0));

		addps(r128(tmp1), m128(tmp6));
		addps(r128(tmp7), m128(tmp3));
		addps(r128(tmp1), m128(tmp7));

		SAT_MOV_XYZ(dst, tmp1);
	}

	void PS20Assembler::M4X4(Operand &dst, const Operand &src0, const Operand &src1)
	{
		movaps(x128(tmp0), m128(src1, 0));
		movaps(x128(tmp1), m128(src1, 1));
		movaps(x128(tmp2), m128(tmp0));
		movaps(x128(tmp3), m128(tmp1)); 

		unpckhps(r128(tmp0), m128(src1, 2));
		unpckhps(r128(tmp1), m128(src1, 3));
		movaps(x128(tmp4), m128(tmp0)); 
		unpckhps(r128(tmp0), m128(tmp1));
		unpcklps(r128(tmp4), m128(tmp1));

		unpcklps(r128(tmp2), m128(src1, 2));
		unpcklps(r128(tmp3), m128(src1, 3));
		movaps(x128(tmp5), m128(tmp2));
		unpckhps(r128(tmp2), m128(tmp3));
		unpcklps(r128(tmp5), m128(tmp3));

		NEG_SWIZZLE(tmp1, src0);

		movaps(x128(tmp6), m128(tmp1));
		movaps(x128(tmp7), m128(tmp1));
		movaps(r128(tmp3), m128(tmp1));
		shufps(r128(tmp1), m128(tmp1), 0x00);
		shufps(r128(tmp6), m128(tmp6), 0x55);
		shufps(r128(tmp7), m128(tmp7), 0xAA);
		shufps(r128(tmp3), m128(tmp3), 0xFF);

		mulps(r128(tmp1), m128(tmp5));
		mulps(r128(tmp6), m128(tmp2));
		mulps(r128(tmp7), m128(tmp4));
		mulps(r128(tmp3), m128(tmp0));

		addps(r128(tmp1), m128(tmp6));
		addps(r128(tmp7), m128(tmp3));
		addps(r128(tmp1), m128(tmp7));

		SAT_MOV_XYZW(dst, tmp1);
	}

	void PS20Assembler::MAD(Operand &dst, const Operand &src0, const Operand &src1, const Operand &src2)
	{
		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);
		NEG_SWIZZLE(tmp2, src2);

		mulps(r128(tmp0), m128(tmp1));
		addps(r128(tmp0), m128(tmp2));

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::MAX(Operand &dst, const Operand &src0, const Operand &src1)
	{
		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);

		maxps(r128(tmp0), m128(tmp1));

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::MIN(Operand &dst, const Operand &src0, const Operand &src1)
	{
		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);

		minps(r128(tmp0), m128(tmp1));

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::MOV(Operand &dst, const Operand &src)
	{
		NEG_SWIZZLE(tmp0, src);
		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::MUL(Operand &dst, const Operand &src0, const Operand &src1)
	{
		NEG_SWIZZLE(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);

		mulps(r128(tmp0), m128(tmp1));

		SAT_MASK(dst, tmp0);
	}

	void PS20Assembler::NOP()
	{
		nop();
	}

	void PS20Assembler::NRM(Operand &dst, const Operand &src)
	{
		NEG_SWIZZLE(tmp0, src);
		NEG_SWIZZLE(tmp1, dst);

		movaps(r128(tmp1), m128(tmp0));
		mulps(r128(tmp1), m128(tmp0));
		movhlps(r128(tmp0), r128(tmp1));
		addss(r128(tmp0), xmm32(tmp1));
		shufps(r128(tmp1), m128(tmp1), 0x01);
		addss(r128(tmp1), xmm32(tmp0));
		rsqrtss(r128(tmp1), xmm32(tmp1));
		shufps(r128(tmp1), m128(tmp1), 0x00);
		mulps(r128(tmp1), m128(tmp0));

		SAT_MASK(dst, tmp1);
	}

	void PS20Assembler::POW(Operand &dst, const Operand &src0, const Operand &src1)
	{
		LOG(tmp0, src0);
		NEG_SWIZZLE(tmp1, src1);

		mulss(r128(tmp0), xmm32(src1));

		EXP(dst, tmp0);
	}

	void PS20Assembler::RCP(Operand &dst, const Operand &src)
	{
		NEG_SWIZZLE(tmp0, src);

		if(instruction->modifier == Instruction::_PP)
		{
			rcpss(r128(tmp1), xmm32(tmp0));
		}
		else
		{
			movss(r128(tmp2), xmm32(tmp0));
			rcpss(r128(tmp1), xmm32(tmp0));
			mulss(r128(tmp2), xmm32(tmp1));
			mulss(r128(tmp2), xmm32(tmp1));
			addss(r128(tmp1), xmm32(tmp1));
			subss(r128(tmp1), xmm32(tmp2));
		}

		shufps(r128(tmp1), r128(tmp1), 0x00);

		SAT_MASK(dst, tmp1);
	}

	void PS20Assembler::RSQ(Operand &dst, const Operand &src)
	{
		NEG_SWIZZLE(tmp0, src);

		if(instruction->modifier == Instruction::_PP)
		{
			rsqrtss(r128(tmp1), xmm32(tmp0));
		}
		else
		{
			static const float THREE = 3.0f;
			static const float HALF = 0.5f;

			rsqrtss(r128(tmp0), xmm32(tmp0));
			movss(r128(tmp1), xmm32(tmp0));
			mulss(r128(tmp0), xmm32(tmp0));
			mulss(r128(tmp0), xmm32(tmp0));
			movss(r128(tmp1), dword_ptr [&THREE]);
			subss(r128(tmp1), xmm32(tmp0));
			mulss(r128(tmp1), xmm32(tmp1));
			mulss(r128(tmp1), dword_ptr [&HALF]);
		}

		shufps(r128(tmp1), r128(tmp1), 0x00);

		SAT_MASK(dst, tmp1);
	}

	void PS20Assembler::SINCOS(Operand &dst, const Operand &src0, const Operand &src1, const Operand &src2)
	{
		static const float C = -4.96818924e-1f;
		static const float B =  3.95277743e-2f;
		static const float A = -9.84989568e-4f;

		static const float ONE = 1.0f;

		NEG_SWIZZLE(tmp0, src0);

		mulss(r128(tmp0), xmm32(tmp0));
		movss(r128(tmp1), xmm32(tmp0));
		mulss(r128(tmp0), dword_ptr [&C]);
		mulss(r128(tmp1), xmm32(tmp1));
		movss(r128(tmp2), xmm32(tmp1));
		mulss(r128(tmp1), dword_ptr [&B]);
		addss(r128(tmp0), xmm32(tmp1));
		mulss(r128(tmp2), xmm32(tmp0));
		mulss(r128(tmp2), dword_ptr [&A]);
		addss(r128(tmp0), xmm32(tmp2));
		addss(r128(tmp0), dword_ptr [&ONE]);
		movss(r128(tmp1), xmm32(tmp0));
		mulss(r128(tmp1), xmm32(tmp1));
		movss(r128(tmp2), dword_ptr [&ONE]);
		subss(r128(tmp2), xmm32(tmp1));
		rsqrtss(r128(tmp2), xmm32(tmp2));
		rcpss(r128(tmp2), xmm32(tmp2));
		movlhps(r128(tmp0), r128(tmp2));
		shufps(r128(tmp0), m128(tmp0), 0x08);

		SAT_MOV_XY(dst, tmp0);
	}

	void PS20Assembler::SUB(Operand &dst, const Operand &src0, const Operand &src1)
	{
		movaps(r128(dst), m128(src0));
		subps(r128(dst), m128(src1));
	}

	void PS20Assembler::TEXKILL(const Operand &src)
	{
		static const float4 XYZ = {0, 0, 0, -FLT_MAX};

		movaps(r128(tmp0), m128(src));

		cmpltps(r128(tmp0), xmmword_ptr [&XYZ]);
		movmskps(eax, r128(tmp0));
		test(eax, eax);
		jnz("texkill");
	}

	void PS20Assembler::TEXLD(Operand &dst, const Operand &src0, const Operand &src1)
	{
		checkDcl(src1);

		const int mipmapOffset = (int)&((Texture*)0)->mipmap;
		const int uFracOffset = (int)&((Texture*)0)->uFrac;
		const int vFracOffset = (int)&((Texture*)0)->vFrac;
		const int uIntOffset = (int)&((Texture*)0)->uInt;
		const int vIntOffset = (int)&((Texture*)0)->vInt;
		const int uHalfOffset = (int)&((Texture*)0)->uHalf;
		const int vHalfOffset = (int)&((Texture*)0)->vHalf;

		movss(r128(tmp0), dword_ptr [&RHW]);
		rcpss(r128(tmp1), r128(tmp0));
		mulss(r128(tmp0), r128(tmp1));
		mulss(r128(tmp0), r128(tmp1));
		addss(r128(tmp1), r128(tmp1));
		subss(r128(tmp1), r128(tmp0));
		shufps(r128(tmp1), r128(tmp1), 0x00);

		movaps(r128(tmp2), m128(src0));
		mulps(r128(tmp2), m128(tmp1));

		if(sampler[src1.index].addressingMode == Sampler::ADDRESSING_CLAMP)
		{
			static const float4 clampZero = {0, 0, FLT_MIN, FLT_MIN};
			static const float4 clampOne = {1, 1, FLT_MAX, FLT_MAX};

			maxps(r128(tmp2), xmmword_ptr [&clampZero]);
			minps(r128(tmp2), xmmword_ptr [&clampOne]);
		}

		static const float4 scale = {1 << 16, 1 << 16, 0, 1 << 16};

		mulps(r128(tmp2), xmmword_ptr [&scale]);
		cvtps2pi(mm0, r128(tmp2));

		if(sampler[src1.index].addressingMode == Sampler::ADDRESSING_MIRROR)
		{
			movq(mm1, mm0);
			pshufw(mm2, mm0, 0xDD);
			pslld(mm1, 15);
			pslld(mm2, 15);
			psrad(mm1, 31);
			psrad(mm2, 31);
			punpckldq(mm1, mm2);
			pxor(mm0, mm1);
		}

		// Mipmap LOD
		static const float clamp = 65535.0f;

		shufps(r128(tmp2), r128(tmp2), 0xFF);
		mulss(r128(tmp2), r128(tmp1));
		minss(r128(tmp2), dword_ptr [&clamp]);
		cvtss2si(ebx, r128(tmp2));
		bsr(ebx, ebx);

		mov(ebp, dword_ptr [&sampler[src1.index].texture]);
		mov(esi, dword_ptr [ebp+4*ebx+mipmapOffset]);

		if(sampler[src1.index].textureFilter == Sampler::FILTER_LINEAR)
		{
			static const qword _F_F = 0x0000FFFF0000FFFF;
			static const qword __FF = 0x00000000FFFFFFFF;

			pshufw(mm1, mm0, 0xAA);
			pshufw(mm0, mm0, 0x00);

			paddw(mm1, qword_ptr [ebp+8*ebx+vHalfOffset]);
			paddw(mm0, qword_ptr [ebp+8*ebx+uHalfOffset]);

			movq(mm3, mm1);
			movq(mm2, mm0);
			movq(mm4, mm0);
			psrlw(mm3, qword_ptr [ebp+8*ebx+vFracOffset]);
			punpcklwd(mm2, mm3);
			punpckhwd(mm4, mm3);
			psrld(mm2, qword_ptr [ebp+8*ebx+uFracOffset]);
			psrld(mm4, qword_ptr [ebp+8*ebx+uFracOffset]);

			psllw(mm0, qword_ptr [ebp+8*ebx+uIntOffset]);
			psllw(mm1, qword_ptr [ebp+8*ebx+vIntOffset]);
			pxor(mm0, qword_ptr [&_F_F]);
			pxor(mm1, qword_ptr [&__FF]);
			pmulhuw(mm0, mm1);

			movd(eax, mm2);
			punpcklbw(mm3, qword_ptr [esi+eax*4]);
			psrlq(mm2, 32);
			movd(eax, mm2);
			punpcklbw(mm2, qword_ptr [esi+eax*4]);

			movd(eax, mm4);
			punpcklbw(mm5, qword_ptr [esi+eax*4]);
			psrlq(mm4, 32);
			movd(eax, mm4);
			punpcklbw(mm4, qword_ptr [esi+eax*4]);

			pshufw(mm1, mm0, 0x00);
			pmulhuw(mm3, mm1);
			pshufw(mm1, mm0, 0x55);
			pmulhuw(mm2, mm1);
			pshufw(mm1, mm0, 0xAA);
			pmulhuw(mm5, mm1);
			pshufw(mm1, mm0, 0xFF);
			pmulhuw(mm4, mm1);

			paddusw(mm3, mm2);
			paddusw(mm5, mm4);
			paddusw(mm5, mm3);
		}
		else   // FILTER_POINT
		{
			pshufw(mm1, mm0, 0xAA);
			psrlw(mm1, qword_ptr [ebp+8*ebx+vFracOffset]);
			punpcklwd(mm0, mm1);
			psrld(mm0, qword_ptr [ebp+8*ebx+uFracOffset]);

			movd(eax, mm0);
			punpcklbw(mm5, qword_ptr [esi+eax*4]);
		}

		psrlw(mm5, 8);
		punpckhwd(mm1, mm5);
		punpcklwd(mm5, mm5);

		static const float4 unscale = {1.0f / 0x00FF00FF, 1.0f / 0x00FF00FF, 1.0f / 0x00FF00FF, 1.0f / 0x00FF00FF};

		cvtpi2ps(r128(dst), mm1);
		movlhps(r128(dst), r128(dst));
		cvtpi2ps(r128(dst), mm5);
		mulps(r128(dst), xmmword_ptr [&unscale]);
	}

	void PS20Assembler::TEXLDB(Operand &dst, const Operand &src0, const Operand &src1)
	{
		throw Error("TEXLDB not supported");
	}

	void PS20Assembler::TEXLDP(Operand &dst, const Operand &src0, const Operand &src1)
	{
		throw Error("TEXLDP not supported");
	}

	// Helper macro instructions

	void PS20Assembler::NEG(Operand &tmp, const Operand &src)
	{
		static const int4 SIGN_MASK = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

		movaps(r128(tmp), m128(src));

		if(src.mod == Operand::NEGATE)
		{
			xorps(r128(tmp), xmmword_ptr [SIGN_MASK]);
		}
	}

	void PS20Assembler::SWIZZLE(Operand &tmp, const Operand &src)
	{
		movaps(r128(tmp), m128(src));
		shufps(r128(tmp), r128(tmp), src.swizzle());
	}

	void PS20Assembler::MASK(Operand &dst, Operand &tmp)
	{
		if(dst.type == Operand::OUTPUT_COLOR_REGISTER)
		{
			movaps(r128(dst), m128(tmp));
			return;
		}

		if(dst.sel == xMask)
		{
			movss(r128(dst), xmm32(tmp));
		}
		else if(dst.sel == xyzwMask)
		{
			movaps(r128(dst), r128(tmp));
		/*
			// Free old dst and make tmp the new dst
			for(int i = 0; i < 8; i++)
			{
				if(xmm[i] == dst)
				{
					free(i);
					break;
				}
			}

			for(i = 0; i < 8; i++)
			{
				if(xmm[i] == tmp)
				{
					free(i);
					allocate(i, dst);
					break;
				}
			}

			// Not allocated to a register
			if(i == 8)
			{
				movaps(r128(dst), m128(tmp));
			}
		*/
		}
		else
		{
			static const int4 MASK[] = {{-1,  0,  0,  0},	// x
			                            { 0, -1,  0,  0},	// y
			                            { 0,  0, -1,  0},	// z
		                                { 0,  0,  0, -1},	// w
			                            {-1, -1,  0,  0},	// xy
			                            {-1,  0, -1,  0},	// xz
		                                {-1,  0,  0, -1},	// xw
			                            { 0, -1, -1,  0},	// yz
			                            { 0, -1,  0, -1},	// yw
			                            { 0,  0, -1, -1},	// zw
			                            {-1, -1, -1,  0},	// xyz
			                            {-1, -1,  0, -1},	// xyw
		                                {-1,  0, -1, -1},	// xzw
			                            { 0, -1, -1, -1},	// yzw
			                            {-1, -1, -1, -1}};	// xyzw

			int m = -1;

			if(dst.sel == xMask) m = 0;
			if(dst.sel == yMask) m = 1;
			if(dst.sel == zMask) m = 2;
			if(dst.sel == wMask) m = 3;
			if(dst.sel == xyMask) m = 4;
			if(dst.sel == xzMask) m = 5;
			if(dst.sel == xwMask) m = 6;
			if(dst.sel == yzMask) m = 7;
			if(dst.sel == ywMask) m = 8;
			if(dst.sel == zwMask) m = 9;
			if(dst.sel == xyzMask) m = 10;
			if(dst.sel == xywMask) m = 11;
			if(dst.sel == xzwMask) m = 12;
			if(dst.sel == yzwMask) m = 13;
			if(dst.sel == xyzwMask) m = 14;

			if(m == -1) throw INTERNAL_ERROR;

			subps(r128(tmp), m128(dst));
			andps(r128(tmp), xmmword_ptr [&MASK[m]]);
			addps(r128(dst), m128(tmp));
		}
	}

	void PS20Assembler::SAT(Operand &dst, Operand &tmp)
	{
		if(instruction->modifier == Instruction::_SAT)
		{
			static const float4 ZERO = {0, 0, 0, 0};
			static const float4 ONE = {1, 1, 1, 1};

			movaps(r128(dst), m128(tmp));
			maxps(r128(dst), xmmword_ptr [ZERO]);
			minps(r128(dst), xmmword_ptr [ONE]);
		}
	}

	void PS20Assembler::NEG_SWIZZLE(Operand &tmp, const Operand &src)
	{
		NEG(tmp, src);
		tmp.sel = src.sel;
		SWIZZLE(tmp, tmp);
	}

	void PS20Assembler::SAT_MASK(Operand &dst, Operand &tmp)
	{
		SAT(tmp, tmp);
		MASK(dst, tmp);
	}

	void PS20Assembler::SAT_MOV_X(Operand &dst, Operand &tmp)
	{
		Operand dst_x = dst;
		dst_x.sel = xMask;

		SAT(tmp, tmp);
		MASK(dst_x, tmp);
	}

	void PS20Assembler::SAT_MOV_XY(Operand &dst, Operand &tmp)
	{
		Operand dst_xy = dst;
		dst_xy.sel = xyMask;

		SAT(tmp, tmp);
		MASK(dst_xy, tmp);
	}

	void PS20Assembler::SAT_MOV_XYZ(Operand &dst, Operand &tmp)
	{
		Operand dst_xyz = dst;
		dst_xyz.sel = xyzMask;

		SAT(tmp, tmp);
		MASK(dst_xyz, tmp);
	}

	void PS20Assembler::SAT_MOV_XYZW(Operand &dst, Operand &tmp)
	{
		Operand dst_xyzw = dst;
		dst_xyzw.sel = xyzwMask;

		SAT(tmp, tmp);
		MASK(dst_xyzw, tmp);
	}

	void PS20Assembler::movaps(OperandXMMREG tmp1, OperandXMMREG xmm2)
	{
		if(tmp1.reg != xmm2.reg)
		{
			Assembler::movaps(tmp1, xmm2);
		}
	}

	void PS20Assembler::movaps(OperandXMMREG xmm, OperandMEM128 m128)
	{
		Assembler::movaps(xmm, m128);
	}

	void PS20Assembler::movaps(OperandXMMREG xmm, OperandR_M128 r_m128)
	{
		if(r_m128.type != SoftWire::Operand::XMMREG || xmm.reg != r_m128.reg)
		{
			Assembler::movaps(xmm, r_m128);
		}
	}

	void PS20Assembler::movaps(OperandMEM128 m128, OperandXMMREG xmm)
	{
		Assembler::movaps(m128, xmm);
	}

	void PS20Assembler::movaps(OperandR_M128 r_m128, OperandXMMREG xmm)
	{
		if(r_m128.type != SoftWire::Operand::XMMREG || r_m128.reg != xmm.reg)
		{
			Assembler::movaps(r_m128, xmm);
		}
	}

	void PS20Assembler::movups(OperandXMMREG tmp1, OperandXMMREG xmm2)
	{
		Assembler::movups(tmp1, xmm2);
	}

	void PS20Assembler::movups(OperandXMMREG xmm, OperandMEM128 m128)
	{
		Assembler::movups(xmm, m128);
	}

	void PS20Assembler::movups(OperandXMMREG xmm, OperandR_M128 r_m128)
	{
		Assembler::movups(xmm, r_m128);
	}

	void PS20Assembler::movups(OperandMEM128 m128, OperandXMMREG xmm)
	{
		Assembler::movups(m128, xmm);
	}

	void PS20Assembler::movups(OperandR_M128 r_m128, OperandXMMREG xmm)
	{
		Assembler::movups(r_m128, xmm);
	}

	void PS20Assembler::movss(OperandXMMREG tmp1, OperandXMMREG xmm2)
	{
		Assembler::movss(tmp1, xmm2);
	}

	void PS20Assembler::movss(OperandXMMREG xmm, OperandMEM32 m32)
	{
		Assembler::movss(xmm, m32);
	}

	void PS20Assembler::movss(OperandXMMREG xmm, OperandXMM32 r_m32)
	{
		Assembler::movss(xmm, r_m32);
	}

	void PS20Assembler::movss(OperandMEM32 m32, OperandXMMREG xmm)
	{
		Assembler::movss(m32, xmm);
	}

	void PS20Assembler::movss(OperandXMM32 r_m32, OperandXMMREG xmm)
	{
		Assembler::movss(r_m32, xmm);
	}

	void PS20Assembler::movhlps(OperandXMMREG tmp1, OperandXMMREG xmm2)
	{
		Assembler::movhlps(tmp1, xmm2);
	}

	void PS20Assembler::movlhps(OperandXMMREG tmp1, OperandXMMREG xmm2)
	{
		Assembler::movlhps(tmp1, xmm2);
	}

	void PS20Assembler::shufps(OperandXMMREG tmp1, OperandXMMREG xmm2, unsigned char imm8)
	{
		if(tmp1.reg != xmm2.reg || imm8 != 0xE4)
		{
			Assembler::shufps(tmp1, xmm2, imm8);
		}
	}

	void PS20Assembler::shufps(OperandXMMREG xmm, OperandMEM128 m128, unsigned char imm8)
	{
		Assembler::shufps(xmm, m128, imm8);
	}

	void PS20Assembler::shufps(OperandXMMREG xmm, OperandR_M128 r_m128, unsigned char imm8)
	{
		if(r_m128.type != SoftWire::Operand::XMMREG || xmm.reg != r_m128.reg || imm8 != 0xE4)
		{
			Assembler::shufps(xmm, r_m128, imm8);
		}
	}

	void PS20Assembler::pshufw(OperandMMREG mm1, OperandMMREG mm2, unsigned char imm8)
	{
		Assembler::pshufw(mm1, mm2, imm8);
	}

	void PS20Assembler::pshufw(OperandMMREG mm, OperandR_M64 r_m64, unsigned char imm8)
	{
		Assembler::pshufw(mm, r_m64, imm8);
	}

	void PS20Assembler::pshufw(OperandMMREG mm, OperandMEM64 mem64, unsigned char imm8)
	{
		Assembler::pshufw(mm, mem64, imm8);
	}
}
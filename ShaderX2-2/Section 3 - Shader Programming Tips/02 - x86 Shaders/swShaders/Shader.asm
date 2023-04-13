
;Tue Jul 22 06:16:16 2003

	PUSHAD
	MOV	ecx,	dword ptr [0x00485548]
	CMP	ecx,	dword ptr [0x0048554c]
	JGE	return
	MOVAPS	xmm0,	xmmword ptr [0x004857a0]
	MOVAPS	xmmword ptr [0x00485450],	xmm0
	MOVSS	xmm0,	dword ptr [0x00485554]
	MOVSS	dword ptr [0x0036fa38],	xmm0
	MOVSS	xmm0,	dword ptr [0x00485550]
	MOVSS	dword ptr [0x0036fa3c],	xmm0
	MOV	edi,	dword ptr [0x00485558]
	MOV	edx,	dword ptr [0x0048555c]
scanlineLoop:
	MOVAPS	xmm0,	xmmword ptr [0x004852f0]
	MOVSS	xmm0,	dword ptr [0x0036fa3c]
	COMISS	xmm0,	dword ptr [edx+ecx*4]
	JNB	texkill
	MOVSS	dword ptr [edx+ecx*4],	xmm0
; 

; 

; 

; 

	MOVSS	xmm0,	dword ptr [0x0036fa38]
	MOVAPS	xmm1,	xmmword ptr [0x00485300]
	RCPSS	xmm1,	xmm0
	MULSS	xmm0,	xmm1
	MULSS	xmm0,	xmm1
	ADDSS	xmm1,	xmm1
	SUBSS	xmm1,	xmm0
	SHUFPS	xmm1,	xmm1,	0x00
	MOVAPS	xmm2,	xmmword ptr [0x00485310]
	MOVAPS	xmm2,	xmmword ptr [0x00485450]
	MULPS	xmm2,	xmm1
	MULPS	xmm2,	xmmword ptr [0x00460470]
	CVTPS2PI	mm0,	xmm2
	SHUFPS	xmm2,	xmm2,	0xff
	MULSS	xmm2,	xmm1
	MINSS	xmm2,	dword ptr [0x00460460]
	CVTSS2SI	ebx,	xmm2
	BSR	ebx,	ebx
	MOV	ebp,	dword ptr [0x00485620]
	MOV	esi,	dword ptr [ebp+ebx*4+24]
	PSHUFW	mm1,	mm0,	0xaa
	PSHUFW	mm0,	mm0,	0x00
	PADDW	mm1,	qword ptr [ebp+ebx*8+728]
	PADDW	mm0,	qword ptr [ebp+ebx*8+600]
	MOVQ	mm3,	mm1
	MOVQ	mm2,	mm0
	MOVQ	mm4,	mm0
	PSRLW	mm3,	qword ptr [ebp+ebx*8+472]
	PUNPCKLWD	mm2,	mm3
	PUNPCKHWD	mm4,	mm3
	PSRLD	mm2,	qword ptr [ebp+ebx*8+344]
	PSRLD	mm4,	qword ptr [ebp+ebx*8+344]
	PSLLW	mm0,	qword ptr [ebp+ebx*8+88]
	PSLLW	mm1,	qword ptr [ebp+ebx*8+216]
	PXOR	mm0,	qword ptr [0x00460458]
	PXOR	mm1,	qword ptr [0x00460450]
	PMULHUW	mm0,	mm1
	MOVD	eax,	mm2
	PUNPCKLBW	mm3,	qword ptr [esi+eax*4]
	PSRLQ	mm2,	0x20
	MOVD	eax,	mm2
	PUNPCKLBW	mm2,	qword ptr [esi+eax*4]
	MOVD	eax,	mm4
	PUNPCKLBW	mm5,	qword ptr [esi+eax*4]
	PSRLQ	mm4,	0x20
	MOVD	eax,	mm4
	PUNPCKLBW	mm4,	qword ptr [esi+eax*4]
	PSHUFW	mm1,	mm0,	0x00
	PMULHUW	mm3,	mm1
	PSHUFW	mm1,	mm0,	0x55
	PMULHUW	mm2,	mm1
	PSHUFW	mm1,	mm0,	0xaa
	PMULHUW	mm5,	mm1
	PSHUFW	mm1,	mm0,	0xff
	PMULHUW	mm4,	mm1
	PADDUSW	mm3,	mm2
	PADDUSW	mm5,	mm4
	PADDUSW	mm5,	mm3
	PSRLW	mm5,	0x08
	PUNPCKHWD	mm1,	mm5
	PUNPCKLWD	mm5,	mm5
	MOVAPS	xmm3,	xmmword ptr [0x00485390]
	CVTPI2PS	xmm3,	mm1
	MOVLHPS	xmm3,	xmm3
	CVTPI2PS	xmm3,	mm5
	MULPS	xmm3,	xmmword ptr [0x00460440]
; 

	MOVSS	xmm0,	dword ptr [0x0036fa38]
	RCPSS	xmm1,	xmm0
	MULSS	xmm0,	xmm1
	MULSS	xmm0,	xmm1
	ADDSS	xmm1,	xmm1
	SUBSS	xmm1,	xmm0
	SHUFPS	xmm1,	xmm1,	0x00
	MOVAPS	xmm2,	xmmword ptr [0x00485450]
	MULPS	xmm2,	xmm1
	MULPS	xmm2,	xmmword ptr [0x00460470]
	CVTPS2PI	mm0,	xmm2
	SHUFPS	xmm2,	xmm2,	0xff
	MULSS	xmm2,	xmm1
	MINSS	xmm2,	dword ptr [0x00460460]
	CVTSS2SI	ebx,	xmm2
	BSR	ebx,	ebx
	MOV	ebp,	dword ptr [0x0048562c]
	MOV	esi,	dword ptr [ebp+ebx*4+24]
	PSHUFW	mm1,	mm0,	0xaa
	PSHUFW	mm0,	mm0,	0x00
	PADDW	mm1,	qword ptr [ebp+ebx*8+728]
	PADDW	mm0,	qword ptr [ebp+ebx*8+600]
	MOVQ	mm3,	mm1
	MOVQ	mm2,	mm0
	MOVQ	mm4,	mm0
	PSRLW	mm3,	qword ptr [ebp+ebx*8+472]
	PUNPCKLWD	mm2,	mm3
	PUNPCKHWD	mm4,	mm3
	PSRLD	mm2,	qword ptr [ebp+ebx*8+344]
	PSRLD	mm4,	qword ptr [ebp+ebx*8+344]
	PSLLW	mm0,	qword ptr [ebp+ebx*8+88]
	PSLLW	mm1,	qword ptr [ebp+ebx*8+216]
	PXOR	mm0,	qword ptr [0x00460458]
	PXOR	mm1,	qword ptr [0x00460450]
	PMULHUW	mm0,	mm1
	MOVD	eax,	mm2
	PUNPCKLBW	mm3,	qword ptr [esi+eax*4]
	PSRLQ	mm2,	0x20
	MOVD	eax,	mm2
	PUNPCKLBW	mm2,	qword ptr [esi+eax*4]
	MOVD	eax,	mm4
	PUNPCKLBW	mm5,	qword ptr [esi+eax*4]
	PSRLQ	mm4,	0x20
	MOVD	eax,	mm4
	PUNPCKLBW	mm4,	qword ptr [esi+eax*4]
	PSHUFW	mm1,	mm0,	0x00
	PMULHUW	mm3,	mm1
	PSHUFW	mm1,	mm0,	0x55
	PMULHUW	mm2,	mm1
	PSHUFW	mm1,	mm0,	0xaa
	PMULHUW	mm5,	mm1
	PSHUFW	mm1,	mm0,	0xff
	PMULHUW	mm4,	mm1
	PADDUSW	mm3,	mm2
	PADDUSW	mm5,	mm4
	PADDUSW	mm5,	mm3
	PSRLW	mm5,	0x08
	PUNPCKHWD	mm1,	mm5
	PUNPCKLWD	mm5,	mm5
	MOVAPS	xmm4,	xmmword ptr [0x004853a0]
	CVTPI2PS	xmm4,	mm1
	MOVLHPS	xmm4,	xmm4
	CVTPI2PS	xmm4,	mm5
	MULPS	xmm4,	xmmword ptr [0x00460440]
; 

	MOVAPS	xmm0,	xmm4
	MOVAPS	xmm1,	xmmword ptr [0x004850f0]
	SHUFPS	xmm1,	xmm1,	0x00
	MULPS	xmm0,	xmm1
	MOVAPS	xmm4,	xmm0
; 

	MOVAPS	xmm0,	xmm3
	MOVAPS	xmm1,	xmm4
	ADDPS	xmm0,	xmm1
	MOVAPS	xmm3,	xmm0
; 

	MOVAPS	xmm0,	xmm3
	MOVAPS	xmm5,	xmmword ptr [0x00485370]
	MOVAPS	xmm5,	xmm0
; 

; 

	MOVAPS	xmm0,	xmm5
	MULPS	xmm0,	xmmword ptr [0x00460620]
	CVTPS2PI	mm0,	xmm0
	MOVHLPS	xmm0,	xmm0
	CVTPS2PI	mm1,	xmm0
	PACKSSDW	mm0,	mm1
	PSHUFW	mm0,	mm0,	0xc6
	PACKUSWB	mm0,	mm0
	MOVD	dword ptr [edi+ecx*4],	mm0
texkill:
	MOVAPS	xmm6,	xmmword ptr [0x00485450]
	ADDPS	xmm6,	xmmword ptr [0x004855a0]
	MOVSS	xmm0,	dword ptr [0x0036fa38]
	ADDSS	xmm0,	dword ptr [0x0048553c]
	MOVSS	dword ptr [0x0036fa38],	xmm0
	MOVSS	xmm0,	dword ptr [0x0036fa3c]
	ADDSS	xmm0,	dword ptr [0x00485534]
	MOVSS	dword ptr [0x0036fa3c],	xmm0
	MOVAPS	xmmword ptr [0x004852f0],	xmm0
	MOVAPS	xmmword ptr [0x00485300],	xmm1
	MOVAPS	xmmword ptr [0x00485310],	xmm2
	MOVAPS	xmmword ptr [0x00485390],	xmm3
	MOVAPS	xmmword ptr [0x004853a0],	xmm4
	MOVAPS	xmmword ptr [0x00485370],	xmm5
	MOVAPS	xmmword ptr [0x00485450],	xmm6
	INC	ecx
	CMP	ecx,	dword ptr [0x0048554c]
	JNGE	scanlineLoop
endScanline:
	EMMS
return:
	POPAD
	RET

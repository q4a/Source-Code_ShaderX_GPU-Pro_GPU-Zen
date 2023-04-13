                  /*--.          .-"-. 
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/


#include "CPU.h"

int cpuFamily;
char cpuVendor[13];
char cpuBrandName[49];
bool cpuCMOV, cpu3DNow, cpu3DNowExt, cpuMMX, cpuMMXExt, cpuSSE, cpuSSE2;


#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
uint64 getHz(){
	LARGE_INTEGER t1,t2,tf;
	uint64 c1,c2;

	QueryPerformanceFrequency(&tf);
	QueryPerformanceCounter(&t1);
	c1 = getCycleNumber();

	// Some spin-wait
	for (volatile int i = 0; i < 1000000; i++);

	QueryPerformanceCounter(&t2);
	c2 = getCycleNumber();
	
	return ((c2 - c1) * tf.QuadPart / (t2.QuadPart - t1.QuadPart));
}

void cpuidAsm(uint32 func, uint32 *a, uint32 *b, uint32 *c, uint32 *d){
	__asm {
		PUSH	EAX
		PUSH	EBX
		PUSH	ECX
		PUSH	EDX

		MOV		EAX, func
		CPUID
		MOV		EDI, a
		MOV		[EDI], EAX
		MOV		EDI, b
		MOV		[EDI], EBX
		MOV		EDI, c
		MOV		[EDI], ECX
		MOV		EDI, d
		MOV		[EDI], EDX

		POP		EDX
		POP		ECX
		POP		EBX
		POP		EAX
	}
}

#elif defined(LINUX)

#include <sys/time.h>

uint64 getHz(){
	static struct timeval t1, t2;
	static struct timezone tz;
	unsigned long long c1,c2;

	gettimeofday(&t1, &tz);
	c1 = getCycleNumber();

	// Some spin-wait
	for (volatile int i = 0; i < 1000000; i++);

	gettimeofday(&t2, &tz);
	c2 = getCycleNumber();

	return (1000000 * (c2 - c1)) / ((t2.tv_usec - t1.tv_usec) + 1000000 * (t2.tv_sec - t1.tv_sec));
}
#endif

void initCPU(){
	uint32 maxi, maxei, a, b, c, d;

	cpuVendor[12]    = '\0';
	cpuBrandName[0]  = '\0';

	cpuCMOV     = false;
	cpuMMX      = false;
	cpuSSE      = false;
	cpuSSE2     = false;
	cpu3DNow    = false;
	cpu3DNowExt = false;
	cpuMMXExt   = false;
	cpuFamily   = 0;

	cpuid(0, maxi, ((uint32 *) cpuVendor)[0], ((uint32 *) cpuVendor)[2], ((uint32 *) cpuVendor)[1]);

	if (maxi >= 1){
		cpuid(1, a, b, c, d);
		cpuCMOV = (d & 0x00008000) != 0;
		cpuMMX  = (d & 0x00800000) != 0;
		cpuSSE  = (d & 0x02000000) != 0;
		cpuSSE2 = (d & 0x04000000) != 0;
		cpuFamily = (a >> 8) & 0x0F;

		cpuid(0x80000000, maxei, b, c, d);
		if (maxei >= 0x80000001){
			cpuid(0x80000001, a, b, c, d);
			cpu3DNow    = (d & 0x80000000) != 0;
			cpu3DNowExt = (d & 0x40000000) != 0;
			cpuMMXExt   = (d & 0x00400000) != 0;

			if (maxei >= 0x80000002){
				cpuid(0x80000002, ((uint32 *) cpuBrandName)[0], ((uint32 *) cpuBrandName)[1], ((uint32 *) cpuBrandName)[2], ((uint32 *) cpuBrandName)[3]);
				cpuBrandName[16] = '\0';

				if (maxei >= 0x80000003){
					cpuid(0x80000003, ((uint32 *) cpuBrandName)[4], ((uint32 *) cpuBrandName)[5], ((uint32 *) cpuBrandName)[6], ((uint32 *) cpuBrandName)[7]);
					cpuBrandName[32] = '\0';

					if (maxei >= 0x80000004){
						cpuid(0x80000004, ((uint32 *) cpuBrandName)[8], ((uint32 *) cpuBrandName)[9], ((uint32 *) cpuBrandName)[10], ((uint32 *) cpuBrandName)[11]);
						cpuBrandName[48] = '\0';
					}
				}
			}
		}
	}
	


/*	__asm {
		PUSHFD
		POP		EAX
		MOV		EBX, EAX
		XOR		EAX, 00200000h
		PUSH	EAX
		POPFD
		PUSHFD
		POP		EAX
		CMP		EAX, EBX
		JZ		ExitCpuTest

			XOR		EAX, EAX
			CPUID

			MOV		DWORD PTR [VendorID],		EBX
			MOV		DWORD PTR [VendorID + 4],	EDX
			MOV		DWORD PTR [VendorID + 8],	ECX
			MOV		DWORD PTR [VendorID + 12],	0

			MOV		EAX, 1
			CPUID
			TEST	EDX, 0x00008000
			SETNZ	AL
			MOV		CPU_CMOV, AL
			TEST	EDX, 0x00800000
			SETNZ	AL
			MOV		CPU_MMX, AL
	
			TEST	EDX, 0x02000000
			SETNZ	AL
			MOV		CPU_SSE, AL

			SHR		EAX, 8
			AND		EAX, 0x0000000F
			MOV		CPUfamily, EAX
	
			MOV		CPU_3DNow, 0
			MOV		EAX, 80000000h
			CPUID
			CMP		EAX, 80000000h
			JBE		NoExtendedFunction
				MOV		EAX, 80000001h
				CPUID
				TEST	EDX, 80000000h
				SETNZ	AL
				MOV		CPU_3DNow, AL

				TEST	EDX, 40000000h
				SETNZ	AL
				MOV		CPU_3DNowExt, AL

				TEST	EDX, 0x00400000
				SETNZ	AL
				MOV		CPU_MMXext, AL

				MOV		EAX, 80000005h
				CPUID
				SHR		ECX, 24
				MOV		DataCache, ECX
				SHR		EDX, 24
				MOV		InstCache, EDX
				
				MOV		EAX, 80000006h
				CPUID
				SHR		ECX, 16
				MOV		L2Cache, ECX

				
			JMP		ExitCpuTest

			NoExtendedFunction:
			MOV		EAX, 2
			CPUID

			MOV		ESI, 4
			TestCache:
				CMP		DL, 0x40
				JNA		NotL2
					MOV		CL, DL
					SUB		CL, 0x40
					SETZ	CH
					DEC		CH
					AND		CL, CH
					MOV		EBX, 64
					SHL		EBX, CL
					MOV		L2Cache, EBX
				NotL2:
				CMP		DL, 0x06
				JNE		Next1
					MOV		InstCache, 8
				Next1:
				CMP		DL, 0x08
				JNE		Next2
					MOV		InstCache, 16
				Next2:
				CMP		DL, 0x0A
				JNE		Next3
					MOV		DataCache, 8
				Next3:
				CMP		DL, 0x0C
				JNE		Next4
					MOV		DataCache, 16
				Next4:
				SHR		EDX, 8
				DEC		ESI
			JNZ	TestCache

		ExitCpuTest:
	}
	*/
}

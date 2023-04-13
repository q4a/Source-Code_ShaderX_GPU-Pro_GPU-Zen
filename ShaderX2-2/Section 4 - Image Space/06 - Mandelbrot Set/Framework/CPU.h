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


#ifndef _CPU_H_
#define _CPU_H_

#include "Platform.h"

extern int cpuFamily;
extern char cpuVendor[13];
extern char cpuBrandName[49];
extern bool cpuCMOV, cpu3DNow, cpu3DNowExt, cpuMMX, cpuMMXExt, cpuSSE;


#if defined(WIN32)
#define CPUID __asm _emit 0x0F __asm _emit 0xA2
#define RDTSC __asm _emit 0x0F __asm _emit 0x31

void cpuidAsm(uint32 func, uint32 *a, uint32 *b, uint32 *c, uint32 *d);
#define cpuid(func, a, b, c, d) cpuidAsm(func, &a, &b, &c, &d)


#pragma warning(disable: 4035)
inline uint64 getCycleNumber(){
	__asm {
		RDTSC
	}
}

#elif defined(LINUX)
#define cpuid(in,a,b,c,d)\
  asm volatile ("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

#define rdtsc(a,d)\
  asm volatile ("rdtsc" : "=a" (a), "=d" (d));

inline uint64 getCycleNumber(){
	union {
		struct {
			unsigned int a,d;
		};
		long long ad;
	} ads;

	rdtsc(ads.a,ads.d);
	return ads.ad;
}
#endif

uint64 getHz();
void initCPU();

#endif // _CPU_H_

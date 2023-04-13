#include "CPUID.hpp"

namespace swShader
{
	// NOTE: keep this order
	bool CPUID::pentium = detectPentium();
	bool CPUID::MMX = detectMMX();
	bool CPUID::SSE = detectSSE();

	bool CPUID::detectPentium()
	{
		bool x;

		__asm
		{
			pushad
			pushfd

			pushfd
			pop  eax
			mov  ecx,eax
			xor  eax,0x200000
			push eax
			popfd
			pushfd      
			pop  eax
			xor  eax,ecx                           
			je   noCpuid
                    
			popfd
			popad
			
			mov  x, 1
			
			jmp  CPUIDpresent 
			
		noCpuid:
			popfd
			popad

			mov x, 0
		CPUIDpresent:
		}

		return x;
	}

	bool CPUID::detectMMX()
	{                        
		if(!pentium) return false;

		bool x = false;

		__asm
		{
			mov eax,1
			cpuid
			test edx,0x00800000
			je noMMX
			mov x, 1  
		noMMX:
		}

		return x;
	}


	bool CPUID::detectSSE()
	{
		if(!pentium) return false;

		bool x = false;
             
		__asm
		{                       
			mov eax, 1
			cpuid                                                   
			test EDX, 0x02000000
			je noSSE
			mov x, 1
		noSSE:
		}

		if(x)
		{
			try 
			{
				__asm
				{
					andps xmm0, xmm0
				}
			}
			catch(...)
			{
				return false;
			}

			return true;
		}

		return false;
	}
}
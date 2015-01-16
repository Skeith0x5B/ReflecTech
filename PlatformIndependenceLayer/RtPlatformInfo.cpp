/*
	Reflec-Tech
	-----------
	File		:	PlatformInfo.cpp
	Author		:	Jamie Taylor
	Last Edit	:	23/03/12
	Desc		:	Find out more about the platform.  Supports x86/x86_64 architecture only atm.  No ARM etc support ...
*/
#include "RtPlatformInfo.h"


/**************************************************************************************************************************/
// For SIMD / CPUID queries
/**************************************************************************************************************************/
I32 SupportsCPUID()
{
	// MSVC
#if RT_COMPILER == RT_COMPILER_MSVC
	#if RT_ARCHITECTURE == RT_ARCHITECTURE_64 && _MSC_VER >= 1400
		return 1;
	#else
		__asm
		{
			pushfd			// push flags
			pop eax			// pop flags into eax
			mov eax, ecx		// make a copy to restore eflags later
			
			xor eax, 200000h 	// set bit 21 (result held in eax)
			push eax		// push result
			popfd			// ... and pop it into the flags register	
			pushfd			// push flags
			
			// it's here that the CPU will reset the bit if it doesn't support CPUID
			
			pop eax			// has the bit reset?
			push ecx		// push original flags/flag state
			popfd			// ... and pop it into flags register, restoring the register to its original state
			
			xor eax, ecx		// xor instruction will set the zero flag only if both operands are equal - http://webster.cs.ucr.edu/AoA/DOS/AoADosIndex.html
			
			jz Has_CPUID
			mov eax, 0		// no CPUID
			
		Has_CPUID:
			mov eax, 1		// has CPUID
		}
	#endif // MSVC
#elif RT_COMPILER == RT_COMPILER_GCC
	#if RT_ARCHITECTURE == RT_ARCHITECTURE_64
		return 1;
	#else
		U32 oldFlags, newFlags;
		__asm__
		(
			"pushfl			;"		// push flags
			"pop	%0		;"		// .. and pop them into oldFlags
			"movl	%0,	%1	;"		// copy oldFlags to newFlags
			
			"xor	%2,	%1	;"		// set bit 21 in newFlags
			"push	%1		;"		// push newFlags
			"popfl			;"		// and pop it into flags
			"pushfl			;"		// push flags
			
			// it's here that the CPU will reset the bit if it doesn't support CPUID
			
			"popl	%1		;"		// pop into newFlags - has bit 21 reset?
			"push	%0		;"		// push oldFlags
			"popfl			;"		// and pop into flags regiser - restoring it to its original state
			
			"=r"(oldFlags), "=r"(newFlags)		// output, "=r" -> any genreal purpose register
			"n"(0x200000)				// input, "n" -> an immediate integer operand with a known numeric value is allowed
		);
		return (newFlags != oldFlags);
	#endif // GCC
#else
	// NOTE: Add support for other compilers here...
	return 0;
#endif // RT_COMPILER == RT_COMPILER_MSVC
}
/**************************************************************************************************************************/

I32 CpuidQuery(I32 query, CpuDesc &result)
{
#if RT_COMPILER == RT_COMPILER_MSVC
	#if RT_ARCHITECTURE == RT_ARCHITECTURE_64
		int cpuInfo[4];
		__cpuid(cpuInfo, query);
		
		result.eax = cpuInfo[0];
		result.ebx = cpuInfo[1];
		result.ecx = cpuInfo[2];
		result.edx = cpuInfo[3];

		return result.eax;
	#else
		__asm
		{
			// set eax with the query and call cpuid
			mov eax, query
			cpuid
			
			// set up the desination
			mov edi, result
			
			mov [edi.eax], eax
			mov [edi.ebx], ebx
			mov [edi.ecx], ecx
			mov [edi.edx], edx
		}
	#endif // MSVC - 32 or 64 bit
#elif RT_COMPILER == RT_COMPILER_GCC
	#if RT_ARCHITECTURE == RT_ARCHITECTURE_64
		// "=a" -> eax, "=b" -> ebx etc... | "a" -> "specifies the `a’ or `d’ registers" - http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
		__asm__
		(
			"cpuid": "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)	: "a"(query)
		);
	#else
		__asm__
		(
			//"movl	%%eax,	%0	;"
			//"cpuid		;"
			
			"pushl	%%ebx		;"	// save old value of ebx (Global offset table)
			"cpuid			;"	// carry out the CPUID query
			"movl	%%ebx,	%%edi	;"	// move ebx to edi, as we need to reset ebx
			"popl	%%ebx		;"	// restore ebx
			
			: "a"(query) // input
																	// "=D" -> edi register - where ebx value was moved before being restored
			: "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=D"(result.edx) // output
		);	
		return result.eax;
	#endif // GCC - 32 or 64 bit
#else
	// NOTE: Add support for other compilers here...
	return 0;
#endif // RT_COMPILER == RT_COMPILER_MSVC 
}
/**************************************************************************************************************************/

/*
	Reflec-Tech
	-----------
	File		:	PlatformInfo.h
	Author		:	Jamie Taylor
	Last Edit	:	23/03/12
	Desc		:	Find out more about the platform.
*/
#ifndef PLATFORM_INFO_H
#define PLATFORM_INFO_H


#include "../RtCommonHeaders.h"


/**************************************************************************************************************************/
// SIMD sets
/**************************************************************************************************************************/
#define CPUID_SSE3		(1<<0)
#define CPUID_SSSE3		(1<<9)
#define CPUID_SSE4_1	(1<<19)
#define CPUID_SSE4_2	(1<<20)
#define CPUID_SSE4_A	(1<<6)	// AMD ONLY
//#define CPUID_XOP		(1<<11) // part of AMDs SSE5 implementation									   -|
//#define CPUID_FMA4	(1<<16) // part of AMDs SSE5 implementation										|- SSE5
//#define CPUID_CVT16	(1<<?)  // part of AMDs SSE5 implementation - can't find in AMD documentation  -|

#define CPUID_AVX		(1<<28) // Advanced Vector eXtensions - proposed & supported by Intel with SandyBridge and AMD with Bulldozer (onward)

#define CPUID_MMX		(1<<23)
#define CPUID_MMXEX		(1<<22)	// MMX Extensions - AMD ONLY
#define CPUID_SSE		(1<<25)
#define CPUID_SSE2		(1<<26)
#define CPUID_3DNOW		(1<<31) // 3DNow ----------- deprecated
#define CPUID_3DNOWEX	(1<<30) // 3DNowExtensions - deprecated
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Hold some basic information on the CPU
/**************************************************************************************************************************/
struct CpuDesc
{
	CpuDesc() { eax = ebx = ecx = edx = featuresFlag = 0; /*vendor = 0;*/ }
	// registers
	U32 eax, ebx, ecx, edx;
	// features flag
	U32 featuresFlag;
	// vendor name/string
	std::string vendor; //U8 *vendor; // removed 17.08: std::string vendor
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// For SIMD / CPUID queries
/**************************************************************************************************************************/
I32 SupportsCPUID();

I32 CpuidQuery(I32 query, CpuDesc &rs);
/**************************************************************************************************************************/


#endif // PLATFORM_INFO_H

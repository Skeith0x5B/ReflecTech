/*
	Reflec-Tech
	-----------
	File		:	RtAssert.h
	Author		:	Jamie Taylor
	Last Edit	:	28/07/12
	Desc		:	Basic assertion macros which work across all supported platforms/compilers. Will be updated as needed.
					NOTE : "\" used in a #define statement allows for multi-line defines.
					NOTE: REPLACE THE CALLS TO STD::COUT WITH A PROPER CALL TO THE FILESYSTEM
*/  
#ifndef RT_ASSERT_H
#define RT_ASSERT_H


#include "../RtCommonHeaders.h"


/**************************************************************************************************************************/
// Set DebugBreak() - this is the function that will actually pause execution and open the debugger if one's there.
/**************************************************************************************************************************/
#if RT_PLATFORM == RT_PLATFORM_WINDOWS
	#if RT_COMPILER == RT_COMPILER_MSVC
		// __debugbreak() works on 32/64/Itanium MSVC + x64 on MSVC won't support inline assembly so __asm { int 3 } is effectively out the question, 
		// other compilers on windows seem to use either __asm { int 3 } asm("int $3")
		#define DebugBreak() __debugbreak() 
	#endif // RT_COMPILER == RT_COMPILER_MSVC
#elif RT_PLATFORM == RT_PLATFORM_LINUX
	#if RT_COMPILER == RT_COMPILER_GCC
		#define DebugBreak() asm("int $3") 
	#endif // RT_PLATFORM == RT_PLATFORM_LINUX
#endif // RT_PLATFORM
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// The actual assertions, one is 'general purpose' and one is effectively a 'debug/performance critical' assertion.
/**************************************************************************************************************************/
#ifdef RT_ASSERTION
	#define RT_ASSERT(expr)																\
		if(expr) { }																	\
		else																			\
		{																				\
			std::cout << __FILE__ << ", " << __LINE__ << ", " << #expr << std::endl;	\
			DebugBreak();																\
		}
#else
	#define RT_ASSERT(expr) 
#endif

// This is the 'debug assertion'. It requires RT_SLOW_ASSERTION to be defined, this is done in an effort
// to give a higher level of granularity and control. Allowing you to toggle assertions in performance sensitive sections 
// whilst possibly leaving others active.
#ifdef RT_SLOW_ASSERTION
 	#define RT_SLOW_ASSERT(expr)														\
		if(expr) { }																	\
		else																			\
		{																				\
			std::cout << __FILE__ << ", " << __LINE__ << ", " << #expr << std::endl;	\
			DebugBreak();																\
		}
#else
	#define RT_SLOW_ASSERT(expr)
#endif // RT_SLOW_ASSERTION
/**************************************************************************************************************************/


#endif // RT_ASSERT_H
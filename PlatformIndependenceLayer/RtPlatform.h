/*
    Reflec-Tech
    -----------
    File        :    RtPlatform.h
    Author      :    Jamie Taylor
    Last Edit   :    10/02/12
    Desc        :    Basic platform detectection and some atomic data type declarations.
*/
#ifndef RT_PLATFORM_H
#define RT_PLATFORM_H


#include "../RtConfiguration.h"


/**************************************************************************************************************************/
// Platform aspects to check
/**************************************************************************************************************************/
// Architecture detection
#define RT_ARCHITECTURE_32 1
#define RT_ARCHITECTURE_64 2


// OS detection
#define RT_PLATFORM_WINDOWS 1
#define RT_PLATFORM_LINUX   2
// add additional OS' here...


// Compiler detection
#define RT_COMPILER_MSVC 1
#define RT_COMPILER_GCC  2
// add additional compilers here...
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Platform detection
/**************************************************************************************************************************/
// Architecture - 32 || 64 bit
// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
//     GNUC - AMD64      || MSVC - AMD64 || GNUC - Intel IA64    || MSVC - Intel IA64
#if defined(__x86_64)    || (_M_X64)     || (__ia64__)           || (_M_IA64) // look for 64 bit, fall back to 32 bit if unsuccessful
    #define RT_ARCHITECTURE RT_ARCHITECTURE_64
#else
    #define RT_ARCHITECTURE RT_ARCHITECTURE_32
#endif // Architecture
/**************************************************************************************************************************/

// Platform (OS)
// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Operating_Systems
// "lean and mean" on windows effectively disables some "advanced" features to speed up build times and reduce exe size - the MFC is a common "target" for this define
#if defined(_WIN64) // had to add this in 'project properties - definitions/macros' - MSVC
    #define RT_PLATFORM  RT_PLATFORM_WINDOWS
    #define WIN64_LEAN_AND_MEAN
    #include <Windows.h>

    typedef HWND handle; // 7/10/12
    typedef HINSTANCE instance; // 01/07/13
    typedef HINSTANCE libraryHandle; // 14/07/13
#elif (_WIN32)
    #define RT_PLATFORM  RT_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN // added Jul 26
    #include <Windows.h>

    typedef HWND handle; // 7/10/12
    typedef HINSTANCE instance; // 01/07/13
    typedef HINSTANCE libraryHandle; // 14/07/13
#elif defined(__linux__)
    #define RT_PLATFORM     RT_PLATFORM_LINUX

    typedef void* handle; // 7/10/12
    typedef void* instance; // 01/07/13
    typedef void* libraryHandle; // 14/07/13
#else
    // 28 Jul - #pragma error is dropped for #error due to better cross-platform support (GCC doesn't support #pragma error)
    #error Unknown/unsupported platform (OS) - compilation stopped. 
#endif // OS
/**************************************************************************************************************************/

// Compiler
// only supporting msvc and gnuc at the moment - more to come...
// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(_MSC_VER)
    #if _MSC_VER >= 1400 // vs 2005 onward support __cpuid intrinsic
        #include <intrin.h>
    #endif // _MSC_VER >= 1400
    #define RT_COMPILER RT_COMPILER_MSVC
#elif defined(__GNUC__)
    #define RT_COMPILER RT_COMPILER_GCC
#else
    #error Unknown/unsupported compiler - compilation stopped.
#endif // Compiler
/**************************************************************************************************************************/

// Endianness - check for explicit big endian declaration, otherwise assume little endian
#ifdef RT_ENDIAN_BIG
    #define RT_ENDIAN RT_ENDIAN_BIG
#else
    #define RT_ENDIAN RT_ENDIAN_LITTLE
#endif // Endian
/**************************************************************************************************************************/

// Debug on/off
#define RT_DEBUG 1
/**************************************************************************************************************************/

/**************************************************************************************************************************/
// Atomic data types
/**************************************************************************************************************************/
// Unsigned int types
typedef unsigned char   U8;
typedef unsigned short  U16;
typedef unsigned int    U32;


// Signed int types
typedef char    I8;
typedef short   I16;
typedef int     I32;

// 64 bit int (look for MSVC)
#if RT_COMPILER == RT_COMPILER_MSVC
    typedef unsigned __int64 U64;
    typedef __int64 I64;
#else
    typedef unsigned long long U64;
    typedef long long I64;
#endif // (_MSC_VER)


/* Float/double, #define RT_DOUBLE_PRECISION for 64 bit floating point value via double
#ifdef RT_DOUBLE_PRECISION
    typedef double Real;
#else
    typedef float Real;
#endif*/

// 07/07/12: Change/addition made after reading http://www.altdevblogaday.com/2012/02/05/dont-store-that-in-a-float/ - make both 32 and 64 bit Reals available always
typedef float  F32;
typedef double F64;

// 22/07/12
// max/min (limits)
#define I32_MAX  2,147,483,647
#define I32_MIN -2,147,483,648
#define U32_MAX  4,294,967,295

// added 20/04/13 to make NULL pointers more explicit
#define NULL 0
/**************************************************************************************************************************/


#endif // RT_PLATFORM_H

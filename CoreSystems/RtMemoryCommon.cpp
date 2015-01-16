/*
===============================================================================

	ReflecTech
	==========
	File		:	RtMmeoryCommon.cpp
	Author		:	Jamie Taylor
	Last Edit	:	25/03/13
	Desc		:	Common os/api independent memory operations.

===============================================================================
*/


#include "RtMemoryCommon.h"


/*
================
ZeroMem

Set a portion of memory to zero.
================
*/
void ZeroMem( void *memory, size_t bytesToSet ) {
	I8 *p = reinterpret_cast<I8*>( memory );
	while( bytesToSet-- ) {
		*p++ = 0;
	}
}

/*
================
SetMem

Set a portion of memory to a specific value.
================
*/
void SetMem( void *memory, U32 value, size_t bytesToSet ) {
	I8 *p = reinterpret_cast<I8*>( memory );
	while( bytesToSet-- ) {
		*p++ = static_cast<I8>( value );
	}
}

/*
================
CopyMem

Copies one portion of memory to another
================
*/
void CopyMem( void *source, const void *dest, size_t bytesToCopy ) {
	I8 *src = reinterpret_cast<I8*>( source );
	I8 *dst = reinterpret_cast<I8*>( const_cast<void*>( dest )  );
	while( bytesToCopy-- ) {
		*dst++ = *src++;
	}
}
/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtMmeoryCommon.h
    Author      :    Jamie Taylor
    Last Edit   :    25/03/13
    Desc        :    Common os/api independent memory operations.

===============================================================================
*/


#ifndef RT_MEMORY_COMMON_H
#define RT_MEMORY_COMMON_H


#include "../PlatformIndependenceLayer/RtPlatform.h"


/*
===============================================================================

Common memory operations

Using while loops here saves 4 assembly/bytecode isntructions over for loops.

===============================================================================
*/


/*
================
ZeroMem

Set a portion of memory to zero.
================
*/
void ZeroMem( void *memory, size_t bytesToSet );

/*
================
SetMem

Set a portion of memory to a specific value.
================
*/
void SetMem( void *memory, U32 value, size_t bytesToSet );

/*
================
CopyMem

Copies one portion of memory to another
================
*/
void CopyMem( void *source, const void *dest, size_t bytesToCopy );


#endif // RT_MEMORY_COMMON_H

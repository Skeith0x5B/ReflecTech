/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtHeapAllocator.h
    Author      :    Jamie Taylor
    Last Edit   :    31/07/13
    Desc        :    Basic heap allocator - uses memory from the heap and thus shouldn't be heavily used.

                     Cutom allocators are used since they allow complete control over allocation&construction via
                     operator and placement new and destruction&deallocation via operator delete. The custom allocators
                     also support aligned allocations and allocation logging/tracking.

                     Pre-allocated buffers should be preferred where possible. 

                     "The pointer returned shall be suitably aligned so that it can be converted to a pointer of any complete object type"- C++ standard (!c++x11).
                     We still re-use the alignment code from the other allocators to ensure we have the desired alignment.

                     31/07/13 - Was going to add allocArray and deAllocArray but decided against it,
                                alloc large blocks for arrays and free with alloc and deAlloc. Also 
                                revamped to follow new id-inspired code conventions.

===============================================================================
*/

 
#ifndef RT_HEAP_ALLOCATOR_H
#define RT_HEAP_ALLOCATOR_H


#include "../RtCommonHeaders.h"
#include "RtAssert.h"

#include "RtMemoryTracker.h"


/*
===============================================================================

Heap Allocator class

===============================================================================
*/
template<typename T = void>
class HeapAllocator {
public:
         HeapAllocator( void );
         ~HeapAllocator( void );

    // Allocate - get uninitialized memory
    T*   Allocate( size_t sizeInBytes, U8 alignment = 1 );

    // DeAllocate
    void DeAllocate( T *freeThis );

    // Construct & destruct- don't use on POD or void
    template<class U>
    void Construct( U *posInMem, const U &val = 0 );
    template<class U>
    void Destruct( U *toDestruct );

private:
    // ...
};


/*
================
HeapAllocator<T>::HeapAllocator
================
*/
template<class T>
HeapAllocator<T>::HeapAllocator( void ) {
    // ...
}

/*
================
HeapAllocator<T>::~HeapAllocator
================
*/
template<class T>
HeapAllocator<T>::~HeapAllocator( void ) {
    // ...
}

/*
================
HeapAllocator<T>::Allocate
================
*/
template<class T>
T* HeapAllocator<T>::Allocate( size_t sizeInBytes, U8 alignment ) {
    RT_SLOW_ASSERT( ( alignment < 1 ) == false );
    size_t temp = reinterpret_cast<size_t>( operator new( ( sizeInBytes + alignment ) ) ); // use sizeInBytes, sizeof( void ) = illegal

    // log the allocation if tracker enabled
#ifdef RT_MEMORY_TRACKER
    void *vp = reinterpret_cast<void*>( temp );
    //MemoryTracker::GetSingletonReference( ).RecordAlloc( vp, file, line, source, usage, sizeInBytes, alignment );
#endif // RT_MEMORY_TRACKER
        
    // get the misalignment
    size_t misalignment = ( temp & ( alignment-1 ) ); // alignment-1 = mask
    // get the adjustment needed
    size_t adjustment = alignment - misalignment;
    // get the newly aligned address
    temp += adjustment; // temp should now hold the aligned address

    //store adjustment information in the byte immediately preceeding adjusted address
    uintptr_t *p = reinterpret_cast<uintptr_t*>( temp-1 );
    *p = adjustment;

    return reinterpret_cast<T*>( temp );
}

/*
================
HeapAllocator<T>::DeAllocate
================
*/
template<class T>
void HeapAllocator<T>::DeAllocate( T *freeThis ) {
    // get the alignment info and adjust if needed
    U8 *temp = reinterpret_cast<U8*>( freeThis );
    temp = ( temp - ( *( temp-1 ) ) );

    // log the de-allocation if tracker enabled
#ifdef RT_MEMORY_TRACKER
    //MemoryTracker::GetSingletonReference().RecordDeAlloc(temp);
#endif // RT_MEMORY_TRACKER

    operator delete( reinterpret_cast<void*>( temp ) );
}

/*
================
HeapAllocator<T>::Construct
================
*/
template<class T>
template<class U>
void HeapAllocator<T>::Construct( U *posInMem, const U &val ) {
    // use placement new
    new(reinterpret_cast<void*>( posInMem ) ) U( val );
}

/*
================
HeapAllocator<T>::Destruct
================
*/
template<class T>
template<class U>
void HeapAllocator<T>::Destruct( U *toDestruct ) {
    toDestruct->~U( );
}


#endif // RT_HEAP_ALLOCATOR_H

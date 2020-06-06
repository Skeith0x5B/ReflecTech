/*
    Reflec-Tech
    -----------
    File        :    DoubleEndedStackAllocator.h
    Author      :    Jamie Taylor
    Last Edit   :    29/07/12
    Desc        :    Double Ended Stack fashioned memory allocator.
                     |Bottom->    |    <-Top|
*/
#ifndef DOUBLE_ENDED_STACK_ALLOCATOR_H
#define DOUBLE_ENDED_STACK_ALLOCATOR_H


#include "../RtCommonHeaders.h"


/**************************************************************************************************************************/
// Double Ended Stack allocator class definition
/**************************************************************************************************************************/
template<class T = void>
class DoubleEndedStackAllocator
{
public:
    // Ctor - explicit keyword removed when param "mem" default arg was removed
    DoubleEndedStackAllocator(size_t sizeInBytes, U8 *mem, size_t offset = 0);

    // Dtor
    ~DoubleEndedStackAllocator();

    // Aligned and unaligned allocation
    T* Allocate(size_t sizeInBytes, U8 alignment = 1, bool fromBottom = true); // 1 byte for same reason as other allocators, 2nd byte to store fromBottom

    // Aligned and unaligned de-allocation
    void DeAllocate(T *freeThis);

    // Construct & destruct - not necessary for POD and void
    template<class U>
    void Construct(U *posInMem, const U &val);
    template<class U>
    void Destruct(U *toDestruct);

    // Completely empty/reset the stack
    void Clear();

    // Get current top and bottom pointers
    size_t GetTop() const;
    size_t GetBottom() const;

    // Get remaining space in bytes
    size_t GetRemainingSpace() const;

private:
    // pointer to the dynamically allocated memory (new/malloc...)
    U8 *stack;
    // pointer to the current stack top
    U8 *top;
    // pointer to current stack 'bottom'
    U8 *bottom;
    // so we don't stray from our allocated block (buffer overflow)
    U8 *limit;
    // were we maybe passed part of a global buffer, or are we allocating our own memory from scratch?
    bool usingLocal;
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
template<class T>
DoubleEndedStackAllocator<T>::DoubleEndedStackAllocator(size_t sizeInBytes, U8 *mem, size_t offset)
{
    // using pre-existing buffer?
    if(mem)
    {
        bottom = stack = &mem[offset];
        limit = &mem[(offset + sizeInBytes)];
        top = limit - 1;
        usingLocal = false;
    }
    else
    // dynmically allocate memory via global new
    {
        bottom = stack = reinterpret_cast<U8*>(operator new((sizeInBytes)));
        limit = &stack[sizeInBytes];
        top = limit - 1;
        usingLocal = true;
    }
}
/**************************************************************************************************************************/

template<class T>
DoubleEndedStackAllocator<T>::~DoubleEndedStackAllocator()
{
    top = bottom = limit = 0;

    if(usingLocal)
        operator delete(reinterpret_cast<void*>(stack));

    stack = 0;
}
/**************************************************************************************************************************/

// Made with help from "Game Engine Architecture" book page 212
template<class T>
T* DoubleEndedStackAllocator<T>::Allocate(size_t sizeInBytes, U8 alignment, bool fromBottom)
{
    // don't stray into the other half of the buffer
    RT_SLOW_ASSERT(!(alignment < 2));
    RT_SLOW_ASSERT(!((bottom + (sizeInBytes + alignment)) > top));
    RT_SLOW_ASSERT(!((top - (sizeInBytes + alignment + 4)) < bottom)); // we need 4 bytes to hold the allocation size

    // get current bottom/top
    size_t temp = 0;
    if(fromBottom)
    {
        temp = reinterpret_cast<size_t>(bottom);
        bottom += (sizeInBytes + alignment);
    }
    else
    {
        top -= (sizeInBytes + alignment + 4); 
        temp = reinterpret_cast<size_t>(top);
    }

    // get the misalignment - mask = alignment-1
    size_t misalignment = (temp & (alignment-1));
    // get the adjustment needed
    size_t adjustment = alignment - misalignment;
    // get the newly aligned address
    temp += adjustment;

    // store appropriate allocation info
    if(fromBottom)
    {
        // store adjustment info in preceeding byte
        U8 *p = reinterpret_cast<U8*>(temp - 2);
        *p = 1; // set fromBottom to true
        *(++p) = adjustment;
    }
    else
    {
        // store total allocation size, ~brute forcing it somewhat~
        U8 *p = reinterpret_cast<U8*>(temp - 5);
        *p = (sizeInBytes + alignment + 4);
        *(p+4) = 0; // set from bottom to false
    }

    // return the address
    return reinterpret_cast<T*>(temp);
} // Allocate()
/**************************************************************************************************************************/

// De-allocation
template<class T>
void DoubleEndedStackAllocator<T>::DeAllocate(T *freeThis)
{
    U8 *temp = reinterpret_cast<U8*>(freeThis);
    if(*(temp - 1) == 1)
        bottom = (temp - (*(temp - 1))); // go back to the original address, read alignment modification from preceeding byte
    else
        top += (*(temp - 5)); // " " " " " ", read total allocation size from preceeding 4 bytes (shouldn't need more than 4 bytes for this) 2^32-1
}
/**************************************************************************************************************************/

// Construct an object at a memory address
template<class T>
template<class U>
void DoubleEndedStackAllocator<T>::Construct(U *posInMem, const U &val)
{
    // use placement new
    new(reinterpret_cast<void*>(posInMem)) U(val);
}
/**************************************************************************************************************************/

// Destroy an object
template<class T>
template<class U>
void DoubleEndedStackAllocator<T>::Destruct(U *toDestruct)
{
    toDestruct->~U();
}
/**************************************************************************************************************************/

// Empty/clear the stack
template<class T>
void DoubleEndedStackAllocator<T>::Clear()
{
    top = &stack[0];
    bottom = limit - 1;
}
/**************************************************************************************************************************/

// Get current stack top
template<class T>
size_t DoubleEndedStackAllocator<T>::GetTop() const
{
    return (top != limit)? reinterpret_cast<size_t>(top) : reinterpret_cast<size_t>(top - 1);
}
/**************************************************************************************************************************/

// Get current stack bottom
template<class T>
size_t DoubleEndedStackAllocator<T>::GetBottom() const
{
    return (bottom != (stack - 1))? reinterpret_cast<size_t>(bottom) : reinterpret_cast<size_t>(bottom - 1);
}
/**************************************************************************************************************************/

// Get remaining space
template<class T>
size_t DoubleEndedStackAllocator<T>::GetRemainingSpace() const
{
    return static_cast<size_t>(top - bottom);
}
/**************************************************************************************************************************/

#endif // DOUBLE_ENDED_STACK_ALLOCATOR_H

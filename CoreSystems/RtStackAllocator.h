/*
    ReflecTech
    -----------
    File        :    StackAllocator.h
    Author      :    Jamie Taylor
    Last Edit   :    30/06/12
    Desc        :    Stack fashioned memory allocator.
*/
#ifndef RT_STACK_ALLOCATOR_H
#define RT_STACK_ALLOCATOR_H


#include "../RtCommonHeaders.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T = void> // default to void for standard 'raw' memory allocator
class StackAllocator
{
public:
    // Default ctor - to aid in creating double buffered allocators
    StackAllocator();
    // Ctor
    StackAllocator(size_t sizeInBytes, U8 *mem, size_t offset = 0);
    // Dtor
    ~StackAllocator();

    // Aligned and unaligned allocation
    T* Allocate(size_t sizeInBytes, U8 alignment = 1);
    // Aligned and unaligned de-allocation
    void DeAllocate(T *freeThis);

    // Construct & destruct - not necessary for POD and void
    template<class U>
    void Construct(U *posInMem, const U &val = 0);
    template<class U>
    void Destruct(U *toDestruct);

    // Completely empty/reset stack marker
    void Clear();

    // Get current stack top
    size_t GetTop() const;
    // Get remaining space in bytes
    size_t GetRemainingSpace() const;

private:
    // pointer to the dynamically allocated memory (new/malloc...)
    U8 *stack;
    // pointer to the current stack top
    U8 *top;
    // so we don't stray from our allocated block (buffer overflow)
    U8 *limit; // void* ?
    // were we maybe passed part of a global buffer, or are we allocating our own memory via global new?
    bool usingLocal;
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
template<class T>
StackAllocator<T>::StackAllocator()
{
    stack = 0;
    top = 0;
    limit = 0;
    usingLocal = false;
}
/**************************************************************************************************************************/

template<class T> 
StackAllocator<T>::StackAllocator(size_t sizeInBytes, U8 *mem, size_t offset)
{
    // have we been handed an already existing buffer via mem?
    if(mem)
    {
        top = stack = &mem[offset];
        limit = &mem[(offset + sizeInBytes)];
        usingLocal = false;
    }
    else 
    // no - so make our own local buffer via global new (not advised, context switch)
    {
        top = stack = reinterpret_cast<U8*>(operator new((sizeInBytes)));
        limit = &stack[sizeInBytes];
        usingLocal = true;
    }
}
/**************************************************************************************************************************/

template<class T>
StackAllocator<T>::~StackAllocator()
{
    top = limit = 0;

    if(usingLocal)
        operator delete(reinterpret_cast<void*>(stack));

    stack = 0;
}
/**************************************************************************************************************************/

// Made with help from "Game Engine Architecture" page 212
// alignment defaults to one to simplify deallocation calls & code
template<class T>
T* StackAllocator<T>::Allocate(size_t sizeInBytes, U8 alignment)
{
    RT_SLOW_ASSERT(!(alignment < 1)); // used to avoid having user specify isAligned when freeing
    RT_SLOW_ASSERT(!((top + (sizeInBytes + alignment)) > limit));

    // get current top and move stack top
    size_t temp = reinterpret_cast<size_t>(top);
    top += (sizeInBytes + alignment);

    // get the misalignment; mask = alignment-1
    size_t misalignment = (temp & (alignment-1));
    // get the adjustment needed
    size_t adjustment = alignment - misalignment;
    // get the newly aligned address
    temp += adjustment;

    // store adjustment info in preceeding byte
    uintptr_t *p = reinterpret_cast<uintptr_t*>(temp - 1);
    *p = adjustment;

    // return the address
    return reinterpret_cast<T*>(temp);
}
/**************************************************************************************************************************/

// De-allocation
template<class T>
void StackAllocator<T>::DeAllocate(T *freeThis)
{
    U8 *temp = reinterpret_cast<U8*>(freeThis);
    top = (temp - (*(temp - 1)));
}
/**************************************************************************************************************************/

// Construct an object at a memory address
template<class T>
template<class U>
void StackAllocator<T>::Construct(U *posInMem, const U &val)
{
    // use placement new
    new(reinterpret_cast<void*>(posInMem)) U(val);
}
/**************************************************************************************************************************/

// Destroy an object
template<class T>
template<class U>
void StackAllocator<T>::Destruct(U *toDestruct)
{
    toDestruct->~U();
}
/**************************************************************************************************************************/

// Empty/clear stack -> top = stack[0]
template<class T>
void StackAllocator<T>::Clear()
{
    top = &stack[0];
}
/**************************************************************************************************************************/

// Get current stack top
template<class T>
size_t StackAllocator<T>::GetTop() const
{
    // prevent accessing invalid memory via buffer overflow/overstepping allocated memory limits?
    return (top != limit)? reinterpret_cast<size_t>(top) : reinterpret_cast<size_t>(limit - 1);
}
/**************************************************************************************************************************/

// How much free space is left?
template<class T>
size_t StackAllocator<T>::GetRemainingSpace() const
{
    return static_cast<size_t>(limit - top);
}
/**************************************************************************************************************************/


#endif // RT_STACK_ALLOCATOR_H

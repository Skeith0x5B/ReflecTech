/*
    Reflec-Tech
    -----------
    File        :    RtScopedArrayPtr_V_T.h
    Author      :    Jamie Taylor
    Last Edit   :    21/08/12
    Desc        :    Initial (first) attempt at making a basic scoped pointer class. Scoped pointer will take care of clean-up itself when it goes out of scope.
                     Holds an internal data pointer and a pointer to the allocator the allocation was made from. 

                     _V - volatile: highly likely to change
                     _T - test: may not be kept in

                     This array variant is effectively the same as RtScopedPtr, only ops* and -> are removed and op[] added. This class
                     is added more for clarity and to make intentions that bit clearer.
*/  
#ifndef RT_SCOPED_ARRAY_PTR_V_T_H
#define RT_SCOPED_ARRAY_PTR_V_T_H


#include "../RtConfiguration.h"
#include "RtAssert.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T, template<class U> class Allocator>
class ScopedArrayPtr
{
public:
    // ctor & dtor
    ScopedArrayPtr(T *p, I32 _size, Allocator<T> &allctr);
    ~ScopedArrayPtr();

    // operators
    T& operator[](I32 index) const;
    // get internal pointer
    T* Get() const;

    // delete/destroy the internal pointer using the callBackAllocator and set the members to the args
    void Reset(T *newMem, I32 newSize, Allocator<T> &newCallBackAllctr);
    // swap this scoped pointer with another
    void Swap(ScopedArrayPtr &swapWith);

private:
    T *internalPointer;
    Allocator<T> *callBackAllocator;
    I32 size;

    // forbidden ops
    ScopedArrayPtr(const ScopedArrayPtr &ref) { /*do nothing*/ }
    ScopedArrayPtr& operator=(const ScopedArrayPtr &rhs) { /*do nothing*/ }
    bool operator==(const ScopedArrayPtr &rhs) { /*do nothing*/ }
    bool operator!=(const ScopedArrayPtr &rhs) { /*do nothing*/ }
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation
/**************************************************************************************************************************/
template<class T, template<class U> class Allocator>
ScopedArrayPtr<T, Allocator>::ScopedArrayPtr(T *p, I32 _size, Allocator<T> &allctr)
{
    RT_SLOW_ASSERT(p);

    internalPointer = p; 
    size = _size;
    callBackAllocator = const_cast<Allocator<T>*>(&allctr); 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
ScopedArrayPtr<T, Allocator>::~ScopedArrayPtr()
{
    // need to destroy all elements in the array
    for(I32 i=0; i<size; ++i)
        callBackAllocator->Destruct/*<T>*/(&internalPointer[i]);
    callBackAllocator->DeAllocate(internalPointer);

    internalPointer = 0; 
    size = 0;
    callBackAllocator = 0; 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T& ScopedArrayPtr<T, Allocator>::operator[](I32 index) const
{
    return *(internalPointer + index);
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T* ScopedArrayPtr<T, Allocator>::Get() const
{
    return internalPointer;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void ScopedArrayPtr<T, Allocator>::Reset(T *newPtr, I32 newSize, Allocator<T> &newCallBackAllctr)
{
    RT_SLOW_ASSERT(newPtr);

    // destroy existing pointer 
    callBackAllocator->Destruct(internalPointer);
    callBackAllocator->DeAllocate(internalPointer);

    internalPointer = newPtr; 
    size = newSize;
    callBackAllocator = &newCallBackAllctr;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void ScopedArrayPtr<T, Allocator>::Swap(ScopedArrayPtr &swapWith)
{ 
    RT_SLOW_ASSERT(swapWith != this);
    RT_SLOW_ASSERT(swapWith.internalPointer);

    T *tempPtr = internalPointer; 
    Allocator<T> tempAllctr = callBackAllocator; 
    I32 tempSize = size;

    internalPointer = swapWith.internalPointer;
    size = swapWith.size;
    callBackAllocator = swapWith.callBackAllocator; 

    swapWith.internalPointer = internalPointer;
    swapWith.size = tempSize;
    swapWith.callBackAllocator = callBackAllocator; 
}
/**************************************************************************************************************************/


#endif // RT_SCOPED_ARRAY_PTR_V_T_H

/*
    Reflec-Tech
    -----------
    File        :    RtScopedPtr_V_T.cpp
    Author      :    Jamie Taylor
    Last Edit   :    21/08/12
    Desc        :    Initial (first) attempt at making a basic scoped pointer class. Scoped pointer will take care of clean-up itself when it goes out of scope.
                     Holds an internal data pointer and a pointer to the allocator the allocation was made from. 

                     _V - volatile: highly likely to change
                     _T - test: may not be kept
*/  
#ifndef RT_SCOPED_PTR_V_T_H
#define RT_SCOPED_PTR_V_T_H


#include "RtAssert.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T, template<class U> class Allocator>
class ScopedPtr // : public Uncopyable?
{
public:
    // ctor & dtor
    ScopedPtr(T *p, const Allocator<T> &allctr);
    ~ScopedPtr();

    // operators
    T& operator*() const;
    T* operator->() const;
    // get internal pointer
    T* Get() const;

    // delete/destroy the internal pointer using the callBackAllocator and set the members to the args
    void Reset(T *newMem, const Allocator<T> &newCallBackAllctr);
    // swap this scoped pointer with another
    void Swap(ScopedPtr &swapWith);

private:
    T *internalPointer;
    Allocator<T> *callBackAllocator;

    // forbidden ops
    ScopedPtr(const ScopedPtr &ref) { /*do nothing*/ }
    ScopedPtr& operator=(const ScopedPtr &rhs) { /*do nothing*/ }
    bool operator==(const ScopedPtr &rhs) { /*do nothing*/ }
    bool operator!=(const ScopedPtr &rhs) { /*do nothing*/ }
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation
/**************************************************************************************************************************/
template<class T, template<class U> class Allocator>
ScopedPtr<T, Allocator>::ScopedPtr(T *p, const Allocator<T> &allctr)
{
    RT_SLOW_ASSERT(p);

    internalPointer = p; 
    callBackAllocator = const_cast<Allocator<T>*>(&allctr); 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
ScopedPtr<T, Allocator>::~ScopedPtr()
{
     callBackAllocator->Destruct(internalPointer); 
     callBackAllocator->DeAllocate(internalPointer);

     internalPointer = 0; 
     callBackAllocator = 0; 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T& ScopedPtr<T, Allocator>::operator*() const
{
    return *internalPointer;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T* ScopedPtr<T, Allocator>::operator->() const
{
    return internalPointer;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T* ScopedPtr<T, Allocator>::Get() const
{
    return internalPointer;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void ScopedPtr<T, Allocator>::Reset(T *newPtr, const Allocator<T> &newCallBackAllctr)
{
    RT_SLOW_ASSERT(newPtr);

    // destroy existing pointer 
    callBackAllocator->Destruct(internalPointer);
    callBackAllocator->DeAllocate(internalPointer);

    internalPointer = newPtr; 
    callBackAllocator = const_cast<Allocator<T>*>(&newCallBackAllctr);
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void ScopedPtr<T, Allocator>::Swap(ScopedPtr &swapWith)
{ 
    RT_SLOW_ASSERT(swapWith != this);
    RT_SLOW_ASSERT(swapWith.internalPointer);

    T *tempPtr = internalPointer; 
    Allocator<T> *tempAllctr = callBackAllocator; 

    internalPointer = swapWith.internalPointer;
    callBackAllocator = swapWith.callBackAllocator; 

    swapWith.internalPointer = tempPtr;
    swapWith.callBackAllocator = tempAllctr; 
}
/**************************************************************************************************************************/


#endif // RT_SCOPED_PTR_V_T_H

/*
    Reflec-Tech
    -----------
    File        :    RtSharedArrayPtr_V_T.h
    Author      :    Jamie Taylor
    Last Edit   :    21/08/12
    Desc        :    Initial (first) attempt at making a basic shared pointer class. Scoped pointer will take care of clean-up itself when 
                     all objects/pointers 'holding' it have finished with it.
                     Holds an internal data pointer and a pointer to the allocator the allocation was made from. 

                     _V - volatile: highly likely to change
                     _T - test: may not be kept

                     This array variant is effectively the same as RtScopedPtr, only ops* and -> are removed and op[] added. This class
                     is added more for clarity and to make intentions that bit clearer.
*/  
#ifndef RT_SHARED_ARRAY_PTR_V_T_H
#define RT_SHARED_ARRAY_PTR_V_T_H


#include "RtAssert.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T, template<class U> class Allocator>
class SharedArrayPtr
{
public:
    SharedArrayPtr(T *pointTo, I32 _size, Allocator<T> &allctr);
    ~SharedArrayPtr();

    void AddRef() const;
    void DropRef();
    bool IsValid() const;
    bool IsUnique() const;
    I32 GetRefCount() const;

    // operators
    T& operator[](I32 index) const;
    // get internal pointer
    T* Get() const;

    // deletes/destroys the internal pointer and resets the internal members to the args
    void Reset(T *newPtr, I32 newSize, Allocator<T> &newAllctr);
    // swap this with another shared pointer
    void Swap(SharedArrayPtr &swapWith);   
       
private:
    T *internalPointer;
    Allocator<T> *callBackAllocator;
    I32 size;
    mutable I32 refCount;

    // forbidden ops
    SharedArrayPtr(const SharedArrayPtr &ref) { /*do nothing*/ }
    SharedArrayPtr& operator=(const SharedArrayPtr &rhs) { /*do nothing*/ }
    bool operator==(const SharedArrayPtr &rhs) { /*do nothing*/ }
    bool operator!=(const SharedArrayPtr &rhs) { /*do nothing*/ }
};
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
SharedArrayPtr<T, Allocator>::SharedArrayPtr(T *pointTo, I32 _size, Allocator<T> &allctr)
{
    RT_SLOW_ASSERT(pointTo);

    internalPointer = pointTo;
    callBackAllocator = &allctr;
    size = _size;
    refCount = 0;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
SharedArrayPtr<T, Allocator>::~SharedArrayPtr()
{
    RT_ASSERT(refCount == 0); 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void SharedArrayPtr<T, Allocator>::AddRef() const
{ 
    ++refCount; 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void SharedArrayPtr<T, Allocator>::DropRef()
{
     RT_ASSERT(refCount > 0); 

    --refCount; 

    if(refCount == 0) 
    { 
        // need to destroy all elements in the array
        for(I32 i=0; i<size; ++i)
            callBackAllocator->Destruct/*<T>*/(&internalPointer[i]);
        callBackAllocator->DeAllocate(internalPointer);

        internalPointer = 0;
        callBackAllocator = 0;
        size = 0;
        this->~SharedArrayPtr();
    }
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
bool SharedArrayPtr<T, Allocator>::IsValid() const
{
    return (internalPointer == 0);
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
bool SharedArrayPtr<T, Allocator>::IsUnique() const
{
    return (refCount == 1);
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
I32 SharedArrayPtr<T, Allocator>::GetRefCount() const
{
    return refCount;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T& SharedArrayPtr<T, Allocator>::operator[](I32 index) const
{
    return *(internalPointer + index);
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T* SharedArrayPtr<T, Allocator>::Get() const
{
    return internalPointer;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void SharedArrayPtr<T, Allocator>::Reset(T *newPtr, I32 newSize, Allocator<T> &newAllctr)
{ 
    RT_SLOW_ASSERT(newPtr.internalPointer); 

    // destroy/delete internal pointer
    callBackAllocator->Destruct(internalPointer);
    callBackAllocator->DeAllocate(internalPointer);

    // and 'reset'
    internalPointer = newPtr.internalPointer; 
    callBackAllocator = &newAllctr;
    size = newSize;
    refCount = 0; 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void SharedArrayPtr<T, Allocator>::Swap(SharedArrayPtr &swapWith)
{ 
    RT_SLOW_ASSERT(swapWith != this);
    RT_SLOW_ASSERT(swapWith.internalPointer); 

    T *tmpPtr = internalPointer; 
    Allocator<T> *tmpAllctr = callBackAllocator;
    I32 tmpSize = size;
    I32 tmpCnt = refCount;

    internalPointer = swapWith.internalPointer; 
    callBackAllocator = swapWith.callBackAllocator;
    size = swapWith.size;
    refCount = swapWith.refCount; 

    swapWith.internalPointer = tmpPtr; 
    swapWith.callBackAllocator = tmpAllctr;
    swapWith.size = tmpSize;
    swapWith.refCount = tmpCnt; 
}
/**************************************************************************************************************************/


#endif // RT_SHARED_ARRAY_PTR_V_T_H

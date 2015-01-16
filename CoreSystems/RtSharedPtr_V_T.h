/*
	Reflec-Tech
	-----------
	File		:	RtSharedPtr_V_T.h
	Author		:	Jamie Taylor
	Last Edit	:	21/08/12
	Desc		:	Initial (first) attempt at making a basic shared pointer class. Scoped pointer will take care of clean-up itself when 
					all objects/pointers 'holding' it have finished with it.
					Holds an internal data pointer and a pointer to the allocator the allocation was made from. 

					_V - volatile: highly likely to change
					_T - test: may not be kept
*/  
#ifndef RT_SHARED_PTR_V_T_H
#define RT_SHARED_PTR_V_T_H


#include "RtAssert.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T, template<class U> class Allocator>
class SharedPtr
{
public:
    SharedPtr(T *pointTo, const Allocator<T> &allctr);
    ~SharedPtr();
       
    void AddRef() const;
    void DropRef();
    bool IsValid() const;
    bool IsUnique() const;
    U32 GetRefCount() const;
       
	// operators
    T& operator*() const;
    T* operator->() const;
    T* GetInternalPointer() const;
       
    // deletes/destroys the internal pointer and resets the internal members to the args
    void Reset(T *newinternalPointer, const Allocator<T> &newAllctr);
    // swap this with another shared pointer
    void Swap(SharedPtr &swapWith);   
       
private:
    T *internalPointer;
    Allocator<T> *callBackAllocator;
    mutable U32 refCount;

	SharedPtr(const SharedPtr &ref) { /*do nothing*/ }
	SharedPtr& operator=(const SharedPtr &rhs) { /*do nothing*/ }
	bool operator==(const SharedPtr &rhs) { /*do nothing*/ }
	bool operator!=(const SharedPtr &rhs) { /*do nothing*/ }
};
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
SharedPtr<T, Allocator>::SharedPtr(T *pointTo, const Allocator<T> &allctr)
{
	RT_SLOW_ASSERT(pointTo);

	internalPointer = pointTo;
	callBackAllocator = const_cast<Allocator<T>*>(&allctr);
	refCount = 0;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
SharedPtr<T, Allocator>::~SharedPtr()
{  
	RT_SLOW_ASSERT(refCount == 0);
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void SharedPtr<T, Allocator>::AddRef() const
{ 
	++refCount; 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void SharedPtr<T, Allocator>::DropRef()
{
 	RT_SLOW_ASSERT(refCount > 0); 

	--refCount; 

	if(refCount == 0) 
	{ 
		// destroy/delete internal pointer
		callBackAllocator->Destruct(internalPointer);
		callBackAllocator->DeAllocate(internalPointer);

		internalPointer = 0;
		callBackAllocator = 0;
		this->~SharedPtr();
	}
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
bool SharedPtr<T, Allocator>::IsValid() const
{
	return (internalPointer == 0);
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
bool SharedPtr<T, Allocator>::IsUnique() const
{
	return (refCount == 1);
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
U32 SharedPtr<T, Allocator>::GetRefCount() const
{
	return refCount;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T& SharedPtr<T, Allocator>::operator*() const
{
	return &internalPointer;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T* SharedPtr<T, Allocator>::operator->() const
{
	return internalPointer;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
T* SharedPtr<T, Allocator>::GetInternalPointer() const
{
	return internalPointer;
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void SharedPtr<T, Allocator>::Reset(T *newPtr, const Allocator<T> &newAllctr)
{ 
	RT_SLOW_ASSERT(newPtr.internalPointer); 

	// destroy/delete internal pointer
	callBackAllocator->Destruct(internalPointer);
	callBackAllocator->DeAllocate(internalPointer);

	// and 'reset'
	internalPointer = newPtr.internalPointer; 
	callBackAllocator = const_cast<Allocator<T>*>(&newAllctr);
	refCount = 0; 
}
/**************************************************************************************************************************/

template<class T, template<class U> class Allocator>
void SharedPtr<T, Allocator>::Swap(SharedPtr &swapWith)
{ 
	RT_SLOW_ASSERT(swapWith != this);
	RT_SLOW_ASSERT(swapWith.internalPointer); 

	T *tmpPtr = internalPointer; 
	Allocator<T> *tmpAllctr = callBackAllocator;
	int tmpCnt = refCount;

	internalPointer = swapWith.internalPointer; 
	callBackAllocator = swapWith.callBackAllocator;
	refCount = swapWith.refCount; 

	swapWith.internalPointer = tmpPtr; 
	swapWith.callBackAllocator = tmpAllctr;
	swapWith.refCount = tmpCnt; 
}
/**************************************************************************************************************************/


#endif // RT_SHARED_PTR_V_T_H

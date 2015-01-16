/*
	ReflecTech
	-----------
	File		:	RtDynamicPoolAllocator.h
	Author		:	Jamie Taylor
	Last Edit	:	03/08/12
	Desc		:	Dynamic pool memory allocator, will allocate memory for elements dynamically from the heap at runtime. This trades the speed of pre-allocated
					memory for the flexibility of dynamic allocation. This also uses more memory as we need to make sure each element is aligned as opposed to 
					simply aligning our pre-allocated buffer; this means memory consumption of ((elementSize + alignment) * numberOf) bytes at any one time.
*/
#ifndef RT_DYNAMIC_POOL_ALLOCATOR_H
#define RT_DYNAMIC_POOL_ALLOCATOR_H


#include "../RtCommonHeaders.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T = void>
class DynamicPoolAllocator
{
public:
	// Ctor
	DynamicPoolAllocator(U32 _elementSize, U32 numberOf = DEFAULT_DYNAMIC_POOL_START_SIZE, U8 _alignment = 1);
	// Dtor
	~DynamicPoolAllocator();

	// Get and return pool elements 
	T* Allocate();
	void DeAllocate(T *putBack);

	// Construct & destruct - not necessary for POD and void
	template<class U>
	void Construct(U *posInMem, const U &val = 0);
	template<class U>
	void Destruct(U *toDestruct);

private:
	// Stays in 'linked list' when free, memory holds data when in use 
	// and is 'recycled' to hold address of next element when put back in pool
	struct PoolElement
	{
		PoolElement *next;
	};

	// Aligned and unaligned allocation - dynamically allocate memory for pool elements
	T* Alloc();
	// Hand back dynamically allocated memory
	void Clear();

	// alignment, need to store this, static pool allocator can simply align the buffer but dynamic allocation isn't necessarily contiguous so store it internally 
	U8 alignment;
	// element size, T may be smaller than the size of an int (memory address) such as the *next member of the PoolElement struct, so we'll store it ourselves - Added back 17.08
	U32 elementSize;
	// head of the free element 'linked list'
	PoolElement *head;
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
template<class T>
DynamicPoolAllocator<T>::DynamicPoolAllocator(U32 _elementSize, U32 numberOf, U8 _alignment)
{
	RT_SLOW_ASSERT(!(_alignment < 1)); // used to avoid having user specify isAligned when freeing

	elementSize = (_elementSize > sizeof(PoolElement)) ? _elementSize : sizeof(PoolElement); // - Added back 17.08 | sizeof(T) could arguably stand in for elementSize
	alignment = _alignment;

	// setup x initial elements - starting with the head
	head  = reinterpret_cast<PoolElement*>(Alloc()); 
	head->next = 0;

	// additional elements, if numberOf > 1
	for(U32 i=0; i<(numberOf - 1); ++i)
	{
		PoolElement *temp = reinterpret_cast<PoolElement*>(Alloc());
		temp->next = head;
		head = temp;
	}
}
/**************************************************************************************************************************/

template<class T>
DynamicPoolAllocator<T>::~DynamicPoolAllocator()
{
	Clear();
}
/**************************************************************************************************************************/

// Get an element from the pool; exposed to user
template<class T>
T* DynamicPoolAllocator<T>::Allocate()
{
	if(head != 0) // static pool allocator would assert this to ensure we're not out of memory
	{
		PoolElement *temp = head;
	    head = head->next;
		return reinterpret_cast<T*>(temp);
	}
	else //  but the  dynamic variant can grow
	    return Alloc();
}
/**************************************************************************************************************************/

// Aligned and unaligned allocation; not exposed to user
template<class T>
T* DynamicPoolAllocator<T>::Alloc()
{
	// dynamically allocate memory for another element
	size_t  temp = reinterpret_cast<size_t>(operator new(elementSize + alignment)); 

	// get the misalignment - mask = alignment-1
	size_t misalignment = (temp & (alignment-1));
	// get the adjustment needed
	size_t adjustment = alignment - misalignment;
	// get the newly aligned address
	temp += adjustment;

	// store adjustment info in preceeding byte
	uintptr_t *p = reinterpret_cast<uintptr_t*>(temp - 1);
	*p = adjustment;

	return reinterpret_cast<T*>(temp);
}
/**************************************************************************************************************************/

// Hand back a pool element; exposed to user
template<class T>
void DynamicPoolAllocator<T>::DeAllocate(T *putBack)
{
	PoolElement *temp = reinterpret_cast<PoolElement*>(putBack);
	temp->next = head;
	head = temp;
}
/**************************************************************************************************************************/

template<class T>
template<class U>
void DynamicPoolAllocator<T>::Construct(U *posInMem, const U &val)
{
	// use placement new
	new(reinterpret_cast<void*>(posInMem)) U(val);
}
/**************************************************************************************************************************/

template<class T>
template<class U>
void DynamicPoolAllocator<T>::Destruct(U *toDestruct)
{
	toDestruct->~U();
}
/**************************************************************************************************************************/

// Release all memory/pool elements - including head; not exposed to user
template<class T>
void DynamicPoolAllocator<T>::Clear()
{
	U8 *toFree = 0;
	PoolElement *temp = 0;
	while(head != 0)
	{
	    temp = head;
	    head = head->next;
 	    toFree = reinterpret_cast<U8*>(temp);
	    toFree = (toFree - (*(toFree-1)));

	    operator delete(reinterpret_cast<void*>(toFree));
	}
	head = 0;
}
/**************************************************************************************************************************/


#endif // RT_TEMPLATED_DYNAMIC_POOL_ALLOCTOR_H

/*
	ReflecTech
	-----------
	File		:	PoolAllocator.h
	Author		:	Jamie Taylor
	Last Edit	:	31/07/12
	Desc		:	Basic pool memory allocator. This implementation is effectively static, you can hand it a pre-allocated buffer
					or an offset into one and ((elementSize * numberOf) + alignment) bytes will be taken. Otherwise you can set @mem
					to null and the allocator will allocate its own local buffer to use - this is also ((elementSize * numberOf) + alignment) bytes
					in size.
*/
#ifndef RT_POOL_ALLOCATOR_H
#define RT_POOL_ALLOCATOR_H


#include "../RtCommonHeaders.h"
#include "RtAssert.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T = void>
class PoolAllocator
{
public:
	// Ctor
	PoolAllocator(U32 _elementSize, U32 numberOf = DEFAULT_STATIC_POOL_START_SIZE, U8 *mem = 0, U32 offset = 0, U8 alignment = 1);
	// Dtor
	~PoolAllocator();

	// Get and return pool elements - doesn't manage memory - only pool elements
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

	// Allocate memory for an element
	T* Alloc();
	// Hand back dynamically allocated memory
	void Clear();

	// pointer to the dynamically allocated memory (new/malloc...)
	U8 *memory;
	// so we don't stray from our allocated block (buffer overflow)
	U8 *limit;
	// using local buffer via new/malloc or part of pre-allocated buffer?
	bool usingLocal;

	// head of the free element 'linked list'
	PoolElement *head;
	// used for internal buffer management 
	U32 bufferOffset;
	// size of an element
	U32 elementSize;
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
template<class T>
PoolAllocator<T>::PoolAllocator(U32 _elementSize, U32 numberOf, U8 *mem, U32 offset, U8 alignment)
{
	RT_SLOW_ASSERT(!(alignment < 1)); // used to avoid having user specify isAligned when freeing

	// 1 - get memory via new or pre-allocated buffer
	// 2 - divide into pool elements.
	elementSize = (_elementSize > sizeof(PoolElement)) ? _elementSize : sizeof(PoolElement); // - Added back 17.08

	// 1
	//--
	if(mem) // using pre-existing buffer?
	{
		memory = &mem[offset];
		limit = &memory[offset + (elementSize * numberOf) + alignment];
		usingLocal = false;
	}
	else // no - so make our own local buffer via global new (not advised, context switch)
	{
		memory = reinterpret_cast<U8*>(operator new((elementSize * numberOf) + alignment));
		limit = &memory[(elementSize * numberOf) + alignment];
		usingLocal = true;
	}

	// all elements are equal in size (eg pool of 16 byte elements, 8 byte elements etc...) - so simply align the start of the buffer
	// eg: 8 byte alignment, move buffer start from 0x12345671 to 0x12345678; then index += elementSize = 0x12345670
	size_t temp = reinterpret_cast<size_t>(memory);

	// get the misalignment - mask = alignment-1
	size_t misalignment = (temp & (alignment-1));
	// get the adjustment needed
	size_t adjustment = alignment - misalignment;
	// get the newly aligned address
	temp += adjustment;

	// store adjustment info in preceeding byte
	uintptr_t *p = reinterpret_cast<uintptr_t*>(temp - 1);
	*p = adjustment;

	memory = reinterpret_cast<U8*>(temp);

	// 2
	//--
	bufferOffset = 0;

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
PoolAllocator<T>::~PoolAllocator()
{
	Clear();
}
/**************************************************************************************************************************/

// Get an element from the pool; exposed to user
template<class T>
T* PoolAllocator<T>::Allocate()
{
	RT_SLOW_ASSERT(head != 0);

	PoolElement *temp = head;
	head = head->next;

	return reinterpret_cast<T*>(temp);
}
/**************************************************************************************************************************/

// Aligned and unaligned allocation; not exposed to user
template<class T>
T* PoolAllocator<T>::Alloc() 
{
	// don't stray from the allocated buffer
	RT_SLOW_ASSERT(!(&memory[bufferOffset + elementSize] > limit));

	T* temp = reinterpret_cast<T*>(&memory[bufferOffset]);
	bufferOffset += elementSize; 

	return temp;
}
/**************************************************************************************************************************/

// Hand back a pool element; exposed to user
template<class T>
void PoolAllocator<T>::DeAllocate(T *putBack)
{
	PoolElement *temp = reinterpret_cast<PoolElement*>(putBack);
	temp->next = head;
	head = temp;
}
/**************************************************************************************************************************/

// Construct an object
template<class T>
template<class U>
void PoolAllocator<T>::Construct(U *posInMem, const U &val)
{
	// use placement new
	new(reinterpret_cast<void*>(posInMem)) U(*val);
}
/**************************************************************************************************************************/

// Destroy an object
template<class T>
template<class U>
void PoolAllocator<T>::Destruct(U *toDestruct)
{
	toDestruct->~U();
}
/**************************************************************************************************************************/

// Release all memory/pool elements; not exposed to user
template<class T>
void PoolAllocator<T>::Clear()
{
	head = 0;
	limit = 0;

	if(usingLocal)
	{
	    memory = (memory - (*(memory-1)));
	    operator delete(reinterpret_cast<void*>(memory));
	}

	memory = 0;
}
/**************************************************************************************************************************/
#endif // RT_POOL_ALLOCTOR_H

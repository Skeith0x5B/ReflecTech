/*
	Reflec-Tech
	-----------
	File		:	DoubleBufferedAllocator.h
	Author		:	Jamie Taylor
	Last Edit	:	06/03/12
	Desc		:	Basic double buffered memory allocator; a single frame allocator is effectively a piece of memory
					being managed with a stack allocator and cleared at the end of each frame.
					A double buffer allocator uses two stacks and swaps them at the end or start of
					each frame, allowing you to cross the frame boundary to a degree.
*/
#ifndef DOUBLE_BUFFERED_ALLOCATOR_H
#define DOUBLE_BUFFERED_ALLOCATOR_H


#include "RtStackAllocator.h"
#include "../RtCommonHeaders.h"


/**************************************************************************************************************************/
// Double buffered allocator class definition
/**************************************************************************************************************************/
template<class T = void>
class DoubleBufferedAllocator
{
public:
	// Can provide a different buffer to each of the member stacks, each with different offset if you wish
	explicit DoubleBufferedAllocator(size_t sizeInBytes, U8 *mem1, U8 *mem2, size_t offset1 = 0, size_t offset2 = 0);

	// Dtor
	~DoubleBufferedAllocator();

	// Make an allocation from the current buffer
	void* Allocate(size_t sizeInBytes, U8 alignment = 1);

	// Construct & destruct - not necessary for POD and void
	template<class U>
	void Construct(U *posInMem, const U &val = 0);
	template<class U>
	void Destruct(U *toDestruct);

	// Swap buffers
	void SwapBuffers();

	// Clear the current buffer
	void ClearCurrentBuffer();

private:
	// Keep track of the current stack being used
	U32 currBuffer;
	// The two stacks
	StackAllocator<T> buffers[2];
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
template<class T>
DoubleBufferedAllocator<T>::DoubleBufferedAllocator(size_t sizeInBytes, U8 *mem1, U8 *mem2, size_t offset1, size_t offset2)
{
	// create two new stack allocators
	StackAllocator<T> stck1(sizeInBytes, reinterpret_cast<U8*>(mem1), offset1);
	StackAllocator<T> stck2(sizeInBytes, reinterpret_cast<U8*>(mem2), offset2);
	buffers[0] =  stck1;
	buffers[1] =  stck2;

	// start with the first of the two member stacks
	currBuffer = 0;
}
/**************************************************************************************************************************/

template<class T>
DoubleBufferedAllocator<T>::~DoubleBufferedAllocator()
{
	buffers[0].Clear();
	buffers[1].Clear();
}
/**************************************************************************************************************************/

template<class T>
void* DoubleBufferedAllocator<T>::Allocate(size_t sizeInBytes, U8 alignment)
{
	return buffers[currBuffer].Allocate(sizeInBytes, alignment);
}
/**************************************************************************************************************************/

// Construct an object at a memory address
template<class T>
template<class U>
void DoubleBufferedAllocator<T>::Construct(U *posInMem, const U &val)
{
	buffers[currBuffer].Construct(posInMem, val);
}
/**************************************************************************************************************************/

// Destroy an object
template<class T>
template<class U>
void DoubleBufferedAllocator<T>::Destruct(U *toDestruct)
{
	buffers[currBuffer].Destruct(toDestruct);
}
/**************************************************************************************************************************/

template<class T>
void DoubleBufferedAllocator<T>::SwapBuffers()
{
	currBuffer = !currBuffer;
}
/**************************************************************************************************************************/

template<class T>
void DoubleBufferedAllocator<T>::ClearCurrentBuffer()
{
	buffers[currBuffer].Clear();
}
/**************************************************************************************************************************/


#endif // DOUBLE_BUFFERED_ALLOCATOR_H

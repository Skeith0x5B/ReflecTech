/*
	Reflec-Tech
	-----------
	File		:	RtVector.h
	Author		:	Jamie Taylor
	Last Edit	:	11/08/12
	Desc		:	Vector container implementation. These containers are not made to try and 'top' the STL, these are made
					for learning purposes/experience. These responsibilities may well by delegated to the STL for ReflecTech 2
					but for now I'll try making them myself for the experience.

					Uses the engines heap allocator.
*/
#ifndef RT_VECTOR_H
#define RT_VECTOR_H


// DEFAULT_VECTOR_CAPACITY can be found/altered in RtConfiguration.h which is included in RtCommonHeaders.h, this is in an 
// effort to keep engine configuation all in one place and thus provide on single point of access/control over the engine settings/tuning.
#include "RtCommonHeaders.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T, class Allocator>
class Vector
{
public:
	// ctors & dtor
	Vector(U32 size = DEFAULT_VECTOR_CAPACITY, bool isAligned = false, size_t alignment = 0);
	Vector(const Vector<T, Allocator> &ref);
	~Vector();
	
	// overloaded operators
	void operator=(const Vector<T, Allocator> &rhs);
	bool operator==(const Vector<T, Allocator> &rhs) const;
	bool operator!=(const Vector<T, Allocator> &rhs) const;
	T& operator[](U32 index);
	const T& operator[](U32 index) const;

	// set each element to a default value
	void InitToDefault(const T &defaultValue);

	// insert and remove elements
	void PushBack(const T &value);
	void PushFront(const T &value);
	void Insert(U32 index, const T &value);
	void Remove(U32 index, U32 count = 1); // starting at specified index - remove count elements (default to 1 as we'll be removing at least 1 element)
	void PopBack();

	// get first or last element - accessing a random element is done via the operator[] overloads
	const T& GetFirst() const;
	T& GetFirst();
	const T& GetLast() const;
	T& GetLast();

	// check for an element via forward or reverse linear search - will return the index of the element if able, -1 otherwise
	U32 LinearSearch(const T &lookingFor) const;
	U32 ReverseLinearSearch(const T &lookingFor) const;
	// HEAP SORT & BINARY SEARCH?

	// check on the vector properties/configuration
	U32 Capacity() const;
	U32 Occupied() const;
	bool IsEmpty() const;
	bool IsAligned() const;
	size_t Alignment() const;

	// resize and clear the vector
	void Resize(U32 newSize);
	void Clear();

private:
	T *data;
	U32 capacity;
	U32 occupied;
	size_t alignment;
	bool isAligned;
	//bool isSorted;

	// memory allocator
	Allocator allctr;
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
template<class T, class Allocator>
Vector<T, Allocator>::Vector(U32 size, bool _isAligned, size_t _alignment)
{
	occupied = 0;
	capacity = size;
	isAligned = _isAligned;
	alignment = _alignment;
	
	data = allctr.Allocate(size * sizeof(T), alignment);
}
/**************************************************************************************************************************/

template<class T, class Allocator>
Vector<T, Allocator>::Vector(const Vector<T, Allocator> &ref)
{
	*this = ref;
}
/**************************************************************************************************************************/

template<class T, class Allocator>
Vector<T, Allocator>::~Vector()
{
	Clear();
	
	// just set to 'safe/null' values, arguable pointless but just to be safe
	capacity = occupied = 0;
	alignment = 0;
	isAligned = false;
}
/**************************************************************************************************************************/

template<class T, class Allocator>
void Vector<T, Allocator>::operator=(const Vector<T, Allocator> &rhs)
{
	// first check for 'no-op' scenerio
	RT_ASSERT(rhs != this);
	
	// set properties
	capacity = rhs.capacity;
	occupied = rhs.occupied;
	isAligned = rhs.isAligned;
	alignment = rhs.alignment;

	// any data to copy over?
	if(rhs.data)
	{
		// yes - copy it over
		data = allctr.Allocate(capacity * sizeof(T), alignment);
		
		for(U32 i=0; i<capacity; ++i)
			allctr.Construct(&data[i], rhs.data[i]);
	}
	else
	{
		// no - just set to null for now
		data = 0;
	}
}
/**************************************************************************************************************************/

template<class T, class Allocator>
bool Vector<T, Allocator>::operator==(const Vector<T, Allocator> &rhs) const
{
	return (occupied == rhs.Occupied);
}
/**************************************************************************************************************************/

template<class T, class Allocator>
bool Vector<T, Allocator>::operator!=(const Vector<T, Allocator> &rhs) const
{
	return (occupied != rhs.Occupied);
}
/**************************************************************************************************************************/

template<class T, class Allocator>
T& Vector<T, Allocator>::operator[](U32 index)
{
	return data[index];
}
/**************************************************************************************************************************/

template<class T, class Allocator>
const T& Vector<T, Allocator>::operator[](U32 index) const
{
	return data[index];
}
/**************************************************************************************************************************/

template<class T, class Allocator>
void Vector<T, Allocator>::InitToDefault(const T &defaultValue)
{
	for(U32 i=0; i<capacity; ++i)
		allctr.Construct(&data[i], defaultValue);

	occupied = capacity;
}
/**************************************************************************************************************************/

template<class T, class Allocator>
void Vector<T, Allocator>:: PushBack(const T &value)
{
	Insert(occupied, value);
}
/**************************************************************************************************************************/

template<class T, class Allocator>
void Vector<T, Allocator>::PushFront(const T &value)
{
	Insert(0, value);
}
/**************************************************************************************************************************/

// insert an element at a specified index - will grow the vector if necessary
template<class T, class Allocator>
void Vector<T, Allocator>::Insert(U32 index, const T &value)
{
	// make a copy incase the element is in the same array
	T temp(value);

	// first - do we need to allocate more memory?
	if(occupied + 1 > capacity)
	{
		// grow the vector 1.5 times - U32eger division
		Resize(capacity + (capacity >> 1));
	}

	// is the element being placed at the end?
	if(index >= occupied)
	{
		// then simply place it at the end and stop
		allctr.Construct(&data[occupied], temp);
	}
	else
	{
		// move the elements up to free the desired index
		allctr.Construct(&data[occupied], data[occupied-1]);
		for(U32 i=occupied-1; i>index; --i)
		{
			allctr.Destruct(&data[i]);
			allctr.Construct(&data[i], data[i-1]);
		}

		// insert the desired element at the desired index
		allctr.Construct(&data[index], temp);
	}

	++occupied;
}
/**************************************************************************************************************************/

// starting at specified index - remove count elements (default to 1 as we'll be removing at least 1 element) /* previous default = 0 */
// NOTE: this will not free the memory being used by the vector - to remove the elements and free the memory - use Clear()
template<class T, class Allocator>
void Vector<T, Allocator>::Remove(U32 index, U32 count)
{
	RT_ASSERT(count > occupied || occupied < 1);

	if(count == occupied && index > 0)
		--count; // added 04/08/12, eg: index = 1; 1 + (count==occupied) > capacity = bounds error so quickly check for it

	// remove specified number of elements
	U32 upto = index+count;
	for(U32 i=index; i<upto; ++i)
	{
		allctr.Destruct(&data[i]);
		--occupied;
	}

	// move remaining elements down to fill the gap
	upto =  index+(occupied-index);
	for(U32 i=index; i<upto; ++i)
	{	
		allctr.Construct(&data[i], data[i+count]);
		allctr.Destruct(&data[i+count]);
	}
}
/**************************************************************************************************************************/

template<class T, class Allocator>
void Vector<T, Allocator>::PopBack()
{
	allctr.Destruct(&data[--occupied]);
}
/**************************************************************************************************************************/

template<class T, class Allocator>
const T& Vector<T, Allocator>::GetFirst() const
{
	return data[0];
}
/**************************************************************************************************************************/

template<class T, class Allocator>
T& Vector<T, Allocator>::GetFirst()
{
	return data[0];
}
/**************************************************************************************************************************/

template<class T, class Allocator>
const T& Vector<T, Allocator>::GetLast() const
{
	return data[occupied-1];
}
/**************************************************************************************************************************/

template<class T, class Allocator>
T& Vector<T, Allocator>::GetLast()
{
	return data[occupied-1];
}
/**************************************************************************************************************************/

template<class T, class Allocator>
U32 Vector<T, Allocator>::LinearSearch(const T &lookingFor) const
{
	for(U32 i=0; i<occupied; ++i)
	{
		if(data[i] == lookingFor)
			return i;
	}

	return -1;
}
/**************************************************************************************************************************/

template<class T, class Allocator>
U32 Vector<T, Allocator>::ReverseLinearSearch(const T &lookingFor) const
{
	// i = (occupied-1) to counter ++occupied at end of insert
	for(U32 i=(occupied-1); i>=0; --i)
	{
		if(data[i] == lookingFor)
			return i;
	}

	return -1;
}
/**************************************************************************************************************************/

template<class T, class Allocator>
U32 Vector<T, Allocator>::Capacity() const
{
	return capacity;
}
/**************************************************************************************************************************/

template<class T, class Allocator>
U32 Vector<T, Allocator>::Occupied() const
{
	return occupied;
}
/**************************************************************************************************************************/

template<class T, class Allocator>
bool Vector<T, Allocator>::IsEmpty() const
{
	return (occupied == 0);
}
/**************************************************************************************************************************/

template<class T, class Allocator>
bool Vector<T, Allocator>::IsAligned() const
{
	return isAligned;
}
/**************************************************************************************************************************/

template<class T, class Allocator>
size_t Vector<T, Allocator>::Alignment() const
{
	return alignment;
}
/**************************************************************************************************************************/

// resize the vector - can be made bigger or smaller; caution is advised when down-sizing the vector to avoid unU32ented loss of data
template<class T, class Allocator>
void Vector<T, Allocator>::Resize(U32 newSize)
{
	// increase size 1.5 times, x >> 1 == x / 2, we simply bit-shift for speed 
	T *temp = data;
	data = allctr.Allocate(newSize * sizeof(T), alignment);
	
	U32 upto = (occupied < newSize)? occupied : newSize;

	// copy over the elements
	for(U32 i=0; i<upto; ++i)
		allctr.Construct(&data[i], temp[i]);
			
	// destroy all elements in the old vector
	for(U32 i=0; i<occupied; ++i)
		allctr.Destruct(&temp[i]);    
	                        
	// free the old memory occupied by the vector
	allctr.DeAllocate(temp, isAligned);
	temp = 0;
	
	// alter vector properties
	if(occupied > newSize)
		occupied = newSize;

	capacity = newSize;
}
/**************************************************************************************************************************/

// empty the vector - remove the values and free the memory being used
template<class T, class Allocator>
void Vector<T, Allocator>::Clear()
{
	// remove the values
	for(U32 i=0; i<occupied; ++i)
		allctr.Destruct(&data[i]);
	// and free the memory
	allctr.DeAllocate(data, isAligned);

	occupied = 0;
}
/**************************************************************************************************************************/


#endif // RT_VECTOR_H
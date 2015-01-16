/*
	Reflec-Tech
	-----------
	File		:	RtSinglyLinkedList_H_V.h
	Author		:	Jamie Taylor
	Last Edit	:	18/08/12
	Desc		:	Basic Doubly linked list data structure - made for learning/practice not to 'top' the stl.
					No bi-directional iteration/size tracking in an effort to promote speed.
					This version uses the engine's heap allocator.

					_H - using heap allocator
					_DP - using dynamic pool allocator
					_V - volatile; source code likely/probable to change

					First time I've needed to use the typename keyword: http://publib.boulder.ibm.com/infocenter/comphelp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8a.doc%2Flanguage%2Fref%2Fkeyword_typename.htm
*/
#ifndef RT_DOUBLY_LINKED_LIST_H_V_H
#define RT_DOUBLY_LINKED_LIST_H_V_H


#include "../RtCommonHeaders.h"
#include "RtHeapAllocator.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T>
class DoublyLinkedList
{
private:
  	// forward declare here to avoid issues referencing
	struct Node
	{
		Node() { next = 0; }
		Node(const T &val) { data = val; next = 0; prev = 0; }

		T data;
		Node *next;
		Node *prev;
	};

public:
	// iterators - const and non const
	class ConstIterator; // forward declare
	class Iterator
	{
	public:
		// ctors & dtor
		Iterator(Node &ref) { pointingTo = &ref; }
		~Iterator() { pointingTo = 0; }

		// operators
		bool operator==(const Iterator &ref) { return (pointingTo == ref.pointingTo); }
		bool operator!=(const Iterator &ref) { return (pointingTo != ref.pointingTo); }
		bool operator==(const ConstIterator &ref) { return (pointingTo == ref.pointingTo); }
		bool operator!=(const ConstIterator &ref) { return (pointingTo != ref.pointingTo); }

		T& operator*() { return pointingTo->data; }
		T* operator->() { return &pointingTo->data; }

		Iterator& operator++() { pointingTo = pointingTo->next; return *this; }
		Iterator operator++(I32 notUsed) { Iterator temp = *this; pointingTo = pointingTo->next; return temp; }
		Iterator& operator--() { pointingTo = pointingTo->prev; return *this; }
		Iterator operator--(I32 notUsed) { Iterator temp = *this; pointingTo = pointingTo->prev; return temp; }

	private:
		friend class DoublyLinkedList<T>; // for access to private 'node' member
		friend class ConstIterator; // for access to 'pointingTo' private member
		Node *pointingTo;
	};

	class ConstIterator
	{
	public:
		// ctors & dtor
		ConstIterator(const Node &ref) { pointingTo = &ref; }
		ConstIterator(const Iterator &itr) { pointingTo = itr.pointingTo; } // make a constItr version of non-const itr
		~ConstIterator() { pointingTo = 0; }

		// operators
		bool operator==(const ConstIterator &ref) { return (pointingTo == ref.pointingTo); }
		bool operator!=(const ConstIterator &ref) { return (pointingTo != ref.pointingTo); }
		bool operator==(const Iterator &ref) { return (pointingTo == ref.pointingTo); }
		bool operator!=(const Iterator &ref) { return (pointingTo != ref.pointingTo); }

		const T& operator*() const { return pointingTo->data; }
		const T* operator->() const { return &pointingTo->data; }

		ConstIterator& operator++() { pointingTo = pointingTo->next; return *this; }
		ConstIterator operator++(I32 notUsed) { Iterator temp = *this; pointingTo = pointingTo->next; return temp; }
		ConstIterator& operator--() { pointingTo = pointingTo->prev; return *this; }
		ConstIterator operator--(I32 notUsed) { Iterator temp = *this; pointingTo = pointingTo->prev; return temp; }

	private:
		friend class DoublyLinkedList<T>; // for access to private 'node' member
		friend class Iterator; // for access to 'pointingTo' private member
		const Node *pointingTo;
	};


	// ctors/dtor
	DoublyLinkedList(bool _isAligned = false, size_t _alignment = 1);
	DoublyLinkedList(const DoublyLinkedList<T> &ref);
	~DoublyLinkedList();

	// overloaded operators
	DoublyLinkedList& operator=(const DoublyLinkedList<T> &rhs);

	// insert/remove elements
	void InsertBefore(const Iterator &itr, const T &val);
	void InsertAfter(const Iterator &itr, const T &val);
	void Remove(Iterator &itr);

	// insert elements
	void PushFront(const T &val);
	void PushBack(const T &val);  
	void PopFront();			  
	void PopBack();			

	// 'fetch' elements
	const Node& CFront() const;
	Node& Front(); 
	const Node& CBack() const;
	Node& Back();
	// access element via iterator
	ConstIterator CBegin() const;
	Iterator Begin();
	ConstIterator CEnd() const;
	Iterator End();

	// check on list properties
	bool IsEmtpy() const;
	U32 Size() const;
	bool IsAligned() const;
	size_t Alignment() const;

	// remove all elements (excluding head and tail)
	void Clear();

private:	
	// private internal clear - this will destroy all elements (head and tail included) and free memory
	void InternalClear();

	// size / alignment etc...
	U32 size;
	size_t alignment;
	bool isAligned;

	// dummy header & tail
	Node *head;
	Node *tail;

	// mem allocator - dynamic heap alloctor (templated)
	HeapAllocator<Node> memAllctr;
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
template<class T>
DoublyLinkedList<T>::DoublyLinkedList(bool _isAligned, size_t _alignment)
{
	// set up general properties
	size = 0;
	alignment = _alignment;
	isAligned = _isAligned;

	// setup the header & tail nodes
	head = memAllctr.Allocate(sizeof(Node), alignment);
	memAllctr.Construct<Node>(head, 0);
	head->next = 0;
	head->prev = 0;

	tail = memAllctr.Allocate(sizeof(Node), alignment);
    memAllctr.Construct<Node>(tail, 0);
    tail->next = 0;
	tail->prev = 0;

	// initial setup
	head->next = tail;
	tail->prev = head;
}
/**************************************************************************************************************************/

template<class T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList<T> &ref)
{
	*this = ref;
}
/**************************************************************************************************************************/

template<class T>
DoublyLinkedList<T>::~DoublyLinkedList()
{
	// free memory
	InternalClear();
}
/**************************************************************************************************************************/

template<class T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList<T> &rhs)
{
	// check for 'no-op' scenerio
	if(this == &rhs)
		return;

	// set attributes
	size = rhs.size;
	alignment = rhs.alignment;
	isAligned = rhs.isAligned;

	// destroy pre-existing elements
	Clear();

	// any data to copy over?
	if(!rhs.IsEmpty())
	{
		// yes - copy it over
	    Node *temp = rhs.head->next;
		while(temp != rhs.tail)
		{
			PushBack(temp->data);
			temp = temp->next;	
		}
	}
	
	return *this;
}
/**************************************************************************************************************************/

template<class T>
void DoublyLinkedList<T>::InsertBefore(const Iterator &itr, const T &val)
{
	// create new node
	Node *temp = memAllctr.Allocate(sizeof(Node), alignment);
	memAllctr.Construct<Node>(temp, val);

	// insert into list
	temp->next = itr.pointingTo;
	temp->prev = itr.pointingTo->prev;
	temp->prev->next = temp;
	itr.pointingTo->prev = temp;

	++size;
}
/**************************************************************************************************************************/

template<class T>
void DoublyLinkedList<T>::InsertAfter(const Iterator &itr, const T &val)
{
	// create new node
	Node *temp = memAllctr.Allocate(sizeof(Node), alignment);
	memAllctr.Construct<Node>(temp, val);

	// insert into list
	temp->next = itr.pointingTo->next;
	temp->prev = itr.pointingTo;
	temp->next->prev = temp;
	itr.pointingTo->next = temp;	

	++size;
}
/**************************************************************************************************************************/

template<class T>
void DoublyLinkedList<T>::Remove(Iterator &itr)
{	
	// move pointers around/disconnect
	itr.pointingTo->next->prev = itr.pointingTo->prev;
	itr.pointingTo->prev->next = itr.pointingTo->next;
	itr.pointingTo->next = 0;
    itr.pointingTo->prev = 0;

	// release memory
	memAllctr.Destruct(itr.pointingTo);
	memAllctr.DeAllocate(itr.pointingTo);

	--size;
}
/**************************************************************************************************************************/

template<class T>
void DoublyLinkedList<T>::PushFront(const T &val)
{
	Iterator tempItr(*head->next);
	InsertBefore(tempItr, val);
}
/**************************************************************************************************************************/

template<class T>
void DoublyLinkedList<T>::PushBack(const T &val)
{
	Iterator tempItr(*tail->prev);
	InsertAfter(tempItr, val);
}
/**************************************************************************************************************************/

template<class T>
void DoublyLinkedList<T>::PopFront()
{
	Iterator tempItr(*head->next);
	Remove(tempItr);
}
/**************************************************************************************************************************/

template<class T>
void DoublyLinkedList<T>::PopBack()
{
	Iterator tempItr(*tail->prev);
	Remove(tempItr);
}
/**************************************************************************************************************************/

template<class T>
const typename DoublyLinkedList<T>::Node& DoublyLinkedList<T>::CFront() const
{
	return *(head->next);
}
/**************************************************************************************************************************/

template<class T>
typename DoublyLinkedList<T>::Node& DoublyLinkedList<T>::Front()
{
	return *(head->next);
}
/**************************************************************************************************************************/

template<class T>
const typename DoublyLinkedList<T>::Node& DoublyLinkedList<T>::CBack() const
{
	return *(tail->prev);        
}
/**************************************************************************************************************************/

template<class T>
typename DoublyLinkedList<T>::Node& DoublyLinkedList<T>::Back()
{
	return *(tail->prev);
}
/**************************************************************************************************************************/

// first time I've needed to use the typename keyword: http://publib.boulder.ibm.com/infocenter/comphelp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8a.doc%2Flanguage%2Fref%2Fkeyword_typename.htm
template<class T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::Begin()
{
	return Iterator(*head->next);
}
/**************************************************************************************************************************/

template<class T>
typename DoublyLinkedList<T>::ConstIterator DoublyLinkedList<T>::CBegin() const
{
	return ConstIterator(*head->next);
}
/**************************************************************************************************************************/

template<class T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::End()
{
	return Iterator(*tail->prev);
}
/**************************************************************************************************************************/

template<class T>
typename DoublyLinkedList<T>::ConstIterator DoublyLinkedList<T>::CEnd() const
{
	return ConstIterator(*tail->prev);
}
/**************************************************************************************************************************/

template<class T>
bool DoublyLinkedList<T>::IsEmtpy() const
{
	return (size == 0);
}
/**************************************************************************************************************************/

template<class T>
U32 DoublyLinkedList<T>::Size() const
{
	return size;
}
/**************************************************************************************************************************/

template<class T>
bool DoublyLinkedList<T>::IsAligned() const
{
	return isAligned;
}
/**************************************************************************************************************************/

template<class T>
size_t DoublyLinkedList<T>::Alignment() const
{
	return alignment;
}
/**************************************************************************************************************************/

// remove all elements - aside from the head and tail, this is the only 'clear' option exposed to the user
template<class T>
void DoublyLinkedList<T>::Clear()
{
	Node *temp = head;
	// destroy elements - head and tail excluded
	while(temp->next != tail)
	{
		head = head->next;
		memAllctr.Destruct(temp);
		memAllctr.DeAllocate(temp);
		temp = head;
	}	
	// ensure  head and tail are correctly set
	head->next = tail;
	tail->prev = head;
}
/**************************************************************************************************************************/

// this 'internal clear' does the same as Clear() above along with destroying the head and tail nodes
template<class T>
void DoublyLinkedList<T>::InternalClear()
{
	Node *temp = head;
	// destroy elements - head and tail included
	while(temp != 0)
	{
		head = head->next;
		memAllctr.Destruct(temp);
		memAllctr.DeAllocate(temp);
		temp = head;
	}	
	tail = temp;
}
/**************************************************************************************************************************/


#endif // RT_DOUBLY_LINKED_LIST_H_V_H

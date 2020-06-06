/*
    Reflec-Tech
    -----------
    File        :    RtSinglyLinkedList_DP_V.h
    Author      :    Jamie Taylor
    Last Edit   :    18/08/12
    Desc        :    Basic Singly linked list data structure - made for learning/practice not to 'top' the stl.
                     No bi-directional iteration/size tracking in an effort to promote speed.
                     This version uses the engine's dynamic pool allocator.

                     _H - using heap allocator
                     _DP - using dynamic pool allocator
                     _V - volatile; source code likely/probable to change

                     First time I've needed to use the typename keyword: http://publib.boulder.ibm.com/infocenter/comphelp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8a.doc%2Flanguage%2Fref%2Fkeyword_typename.htm
*/
#ifndef RT_SINGLY_LINKED_LIST_DP_V_H
#define RT_SINGLY_LINKED_LIST_DP_V_H


#include "../RtCommonHeaders.h"
#include "RtDynamicPoolAllocator.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
template<class T>
class SinglyLinkedList
{
private:
      // forward declare here to avoid issues referencing
    struct Node
    {
        Node() { next = 0; }
        Node(const T &val) { data = val; next = 0; }

        T data;
        Node *next;
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

        // forward/linear iteration only
        Iterator& operator++() { pointingTo = pointingTo->next; return *this; }
        Iterator operator++(I32 notUsed) { Iterator temp = *this; pointingTo = pointingTo->next; return temp; }

    private:
        friend class SinglyLinkedList<T>; // for access to private 'node' member
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

        // forward/linear iteration only
        ConstIterator& operator++() { pointingTo = pointingTo->next; return *this; }
        ConstIterator operator++(I32 notUsed) { Iterator temp = *this; pointingTo = pointingTo->next; return temp; }

    private:
        friend class SinglyLinkedList<T>; // for access to private 'node' member
        friend class Iterator; // for access to 'pointingTo' private member
        const Node *pointingTo;
    };


    // ctors/dtor
    SinglyLinkedList(bool _isAligned = false, U8 _alignment = 1);
    SinglyLinkedList(const SinglyLinkedList<T> &ref);
    ~SinglyLinkedList();

    // overloaded operators
    SinglyLinkedList& operator=(const SinglyLinkedList<T> &rhs);

    // insert/remove elements
    void InsertAfter(const Iterator &itr, const T &val);
    void Remove(Iterator &itr);

    // insert elements
    void PushFront(const T &val); 
    void PopFront();

    // 'fetch' elements
    const Node& CFront() const;
    Node& Front(); 
    // access element via iterator
    ConstIterator CBegin() const;
    Iterator Begin();

    // check on list properties
    bool IsEmpty() const;
    bool IsAligned() const;
    U8 Alignment() const;

    // remove all elements (excluding head)
    void Clear();

private:
    // private internal clear - this will destroy all elements (head included) and free memory
    void InternalClear();

    // alignment etc...
    U8 alignment;
    bool isAligned;

    // dummy head
    Node *head;

    // mem allocator - dynamic pool allocator (templated)
    DynamicPoolAllocator<Node> *memAllctr;
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
template<class T>
SinglyLinkedList<T>::SinglyLinkedList(bool _isAligned, U8 _alignment)
{
    // set up general properties
    alignment = _alignment;
    isAligned = _isAligned;

    // we cannot simply have the pool allocator as a member as its ctor relies on param values we won't have
    // at compile time and since we want to avoid dynamic allcation where possible, we construct the allocator here,
    // and declare it static so it will stay in memory, we then set the pointer to it to work around scope issues
    static DynamicPoolAllocator<Node> memallctr(sizeof(Node), 1, alignment); // make 1 node - which'll effectively be the head in the pool allocators internal list (still a usable node) so set to 1
    memAllctr = &memallctr;

    // setup head and first element
    head = memAllctr->Allocate();
    //Node temp;
    memAllctr->Construct<Node>(head/*, temp*/); // <Node> - MSVC needs this - make a platform specific #define ?
    head->next = 0;
}
/**************************************************************************************************************************/

template<class T>
SinglyLinkedList<T>::SinglyLinkedList(const SinglyLinkedList<T> &ref)
{
    *this = ref;
}
/**************************************************************************************************************************/

template<class T>
SinglyLinkedList<T>::~SinglyLinkedList()
{
    // free memory
    InternalClear();
}
/**************************************************************************************************************************/

template<class T>
SinglyLinkedList<T>& SinglyLinkedList<T>::operator=(const SinglyLinkedList<T> &rhs)
{
    // check for 'no-op' scenerio
    if(this == &rhs)
        return;

    // set attributes
    alignment = rhs.alignment;
    isAligned = rhs.isAligned;

    // destroy pre-existing elements
    Clear();

    // any data to copy over?
    if(!rhs.IsEmpty())
    {
        // yes - copy it over
        Node *temp = rhs.head->next;
        while(temp != 0)
        {
            PushFront(temp->data);
            temp = temp->next;    
        }
    }
    
    return *this;
}
/**************************************************************************************************************************/

template<class T>
void SinglyLinkedList<T>::InsertAfter(const Iterator &itr, const T &val)
{
    // create new node
    Node *temp = memAllctr->Allocate();
    memAllctr->Construct<Node>(temp, val); // <Node> - MSVC needs this - platform specific #define ?

    // insert into list
    temp->next = itr.pointingTo->next;
    itr.pointingTo->next = temp;
}
/**************************************************************************************************************************/

template<class T>
void SinglyLinkedList<T>::Remove(Iterator &itr)
{    
    // have to loop round due to lack of previous pointers
    Iterator tmpItr(*head);
    while(tmpItr.pointingTo->next != itr.pointingTo)//while(tmpItr != itr)
        tmpItr++;

    tmpItr.pointingTo->next = itr.pointingTo->next; // re-route pointers
    itr.pointingTo->next = 0; // safely disconnect

    // release memory
    memAllctr->Destruct(itr.pointingTo);
    memAllctr->DeAllocate(itr.pointingTo);
}
/**************************************************************************************************************************/

template<class T>
void SinglyLinkedList<T>::PushFront(const T &val)
{
    Iterator tempItr(*head);
    InsertAfter(tempItr, val);
}
/**************************************************************************************************************************/

template<class T>
void SinglyLinkedList<T>::PopFront()
{
    Iterator tempItr(*head->next);
    Remove(tempItr);
}
/**************************************************************************************************************************/

template<class T>
const typename SinglyLinkedList<T>::Node& SinglyLinkedList<T>::CFront() const
{
    return head->next;
}
/**************************************************************************************************************************/

template<class T>
typename SinglyLinkedList<T>::Node& SinglyLinkedList<T>::Front()
{
    return head->next;
}
/**************************************************************************************************************************/

// first time I've needed to use the typename keyword: http://publib.boulder.ibm.com/infocenter/comphelp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8a.doc%2Flanguage%2Fref%2Fkeyword_typename.htm
template<class T>
typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::Begin()
{
    return Iterator(*head->next);
}
/**************************************************************************************************************************/

template<class T>
typename SinglyLinkedList<T>::ConstIterator SinglyLinkedList<T>::CBegin() const
{
    return ConstIterator(*head->next);
}
/**************************************************************************************************************************/

template<class T>
bool SinglyLinkedList<T>::IsEmpty() const
{
    return (head->next == 0);
}
/**************************************************************************************************************************/

template<class T>
bool SinglyLinkedList<T>::IsAligned() const
{
    return isAligned;
}
/**************************************************************************************************************************/

template<class T>
U8 SinglyLinkedList<T>::Alignment() const
{
    return alignment;
}
/**************************************************************************************************************************/

// remove all elements - aside from head and table : this is the only 'clear' option exposed to the user
template<class T>
void SinglyLinkedList<T>::Clear()
{
    Node *temp = head;
    // destroy elements - head excluded
    while(temp->next != 0)
    {
        head = head->next;
        memAllctr->Destruct(temp);
        memAllctr->DeAllocate(temp);
        temp = head;
    }    
    // ensure head is correctly set
    head->next = 0;
}
/**************************************************************************************************************************/

// this 'internal clear' does the same as Clear() above, then destroys the head and tail
// freeing memory is the next intended step after calling this
template<class T>
void SinglyLinkedList<T>::InternalClear()
{
    Node *temp = head;
    // destroy elements - head included
    while(temp != 0)
    {
        head = head->next;
        memAllctr->Destruct(temp);
        memAllctr->DeAllocate(temp);
        temp = head;
    }
}
/**************************************************************************************************************************/


#endif // RT_SINGLY_LINKED_LIST_DP_V_H

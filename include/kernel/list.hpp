#pragma once

#include <kernel/machine.hpp>
#include <kernel/types.hpp>

#include <kernel/printf.hpp>

template<class T, list_head T::* Node>
class ListIterator;

template<class T, list_head T::* Node>
class List
{   
public:

    List()
    {
	header.prev = &header;
	header.next = &header;
    }

    T* front() const
    {
	list_head* node = header.next;
	printf("node = 0x%x\n", node);
	return (node == &header) ? nullptr : object_from_list_head(node);
    }

    T* back() const
    {
	list_head* node = header.prev;
	return (node == &header) ? nullptr : object_from_list_head(node);
    }
    
    void push_front(T* elem)
    {
	(elem->*Node).next = header.next;
	(elem->*Node).prev = &header;
	header.next->prev = &(elem->*Node);
	header.next = &(elem->*Node);
    }

    void push_back(T* elem)
    {
	(elem->*Node).prev = header.prev;
	(elem->*Node).next = &header;
	header.prev->next = &(elem->*Node);
	header.prev = &(elem->*Node);
    }

    T* pop_front()
    {
	list_head* node = header.next;
	if (node == &header)
	{
	    return nullptr;
	}
	header.next = node->next;
	node->next->prev = &header;
	return object_from_list_head(node);
    }
    
    T* pop_back()
    {
	list_head* node = header.prev;
	if (node == &header)
	{
	    return nullptr;
	}
	header.prev = node->prev;
	node->prev->next = &header;
	return object_from_list_head(node);
    }

    void remove(T* elem)
    {
	(elem->*Node).next->prev = (elem->*Node).prev;
	(elem->*Node).prev->next = (elem->*Node).next;
    }
    
    T* clear()
    {
	list_head* node = header.next;
	if (node == &header)
	{
	    return nullptr;
	}
	node->prev = header.prev;
	header.prev->next = node;
	header.prev = &header;
	header.next = &header;
	return object_from_list_head(node);
    }

    ListIterator<T, Node> iterator()
    {
	return ListIterator<T, Node>(this);
    }
    
    void monitor_front()
    {
	monitor(&header.next);
    }

    void monitor_back()
    {
	monitor(&header.prev);
    }   
    
    operator bool()
    {
	return header.next != &header;
    }

private:
    static T* object_from_list_head(list_head* list)
    {
	return (T*) (((uptr) list) - ((usize) &(((T*) 0)->*Node)));
    }
    
private:    
    list_head header;

    friend class ListIterator<T, Node>;    
};

template<class T, list_head T::* Node>
class ListIterator
{
public:

    ListIterator(List<T, Node>* list) : node(list->header.next), header(&list->header)
    {
    }
    
    void next()
    {
	node = node->next;
    }

    void prev()
    {
	node = node->prev;
    }

    operator bool() const
    {
	return node != header;
    }
    
    T* operator->() const
    {
	return List<T, Node>::object_from_list_head(node);
    }

    T& operator*() const
    {
	return *List<T, Node>::object_from_list_head(node);
    }

    ListIterator& operator++()
    {
	next();
	return *this;
    }

    ListIterator operator++(int)
    {
	ListIterator it = *this;
	next();
	return it;
    }

    ListIterator& operator--()
    {
	prev();
	return *this;
    }

    ListIterator operator--(int)
    {
	ListIterator it = *this;
	prev();
	return *this;
    }	
    
private:
    list_head* node;
    list_head* header;
};

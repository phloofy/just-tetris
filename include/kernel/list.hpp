#pragma once

#include <kernel/types.hpp>

template<class T, list_head T::* Node>
class List
{
public:
    
    void push_front(T* elem)
    {
	(elem->*Node).next = header.next;
	(elem->*Node).prev = &header;
	header.next = &(elem->*Node);
    }

    void push_back(T* elem)
    {
	(elem->*Node).prev = header.prev;
	(elem->*Node).next = &header;
	header.prev = &(elem->*Node);
    }

    T* pop_front()
    {
	list_head* node = header.next;
	header.next = node->next;
	node->next->prev = &header;
	return object_from_list_head(node);
    }
    
    T* pop_back()
    {
	list_head* node = header.prev;
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
	node->prev = header.prev;
	header.prev->next = node;
	header.prev = &header;
	header.next = &header;
	return object_from_list_head(node);
    }
    
    operator bool()
    {
	return header.next != &header;
    }

private:
    static T* object_from_list_head(list_head* list)
    {
	return (T*) ((uptr) list) - ((usize) &(((T*) 0)->*Node));
    }
    
private:    
    list_head header;
};

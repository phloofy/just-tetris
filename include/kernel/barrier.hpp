#pragma once

#include <kernel/atomic.hpp>
#include <kernel/types.hpp>

class Barrier
{
public:

    Barrier(usize size) : size(size), count(size), gate(false) {}

    Barrier(const Barrier&) = delete;
        
    void sync()
    {
	bool expected = gate.load();
	if (count.sub_fetch(1) > 0)
	{
	    gate.monitor();
	    while (gate.load() == expected)
	    {
		mwait();
		gate.monitor();
	    }
	}
	else
	{
	    count.store(size);
	    gate.store(!expected);
	}
    }

    Barrier& operator=(const Barrier&) = delete;
    
private:
    const usize size;
    atomic<usize> count;
    atomic<bool> gate;
};

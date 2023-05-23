#pragma once

#include <kernel/atomic.hpp>
#include <kernel/machine.hpp>
#include <kernel/types.hpp>

class Spinlock
{
public:
    Spinlock() : taken(false), int_state(false)
    {
    }

    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;

    void lock()
    {
	bool state = int_disable();
	while (taken.test_and_set())
	{
	    int_restore(state);
	    taken.monitor();
	    while (taken.load())
	    {
		mwait();
		taken.monitor();
	    }
	    state = int_disable();
	}
	int_state = state;
    }

    void try_lock()
    {
	bool state = int_disable();
	if (taken.test_and_set())
	{
	    int_state = state;
	    return;
	}
	int_restore(state);
    }
    
    void unlock()
    {
	bool state = int_state;
	taken.clear();
	int_restore(state);
    }
    
private:
    atomic<bool> taken;
    volatile bool int_state;
};

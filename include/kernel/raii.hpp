#pragma once

#include <kernel/machine.hpp>
#include <kernel/utility.hpp>

namespace policy
{

class defer_t {};
class try_to_t {};

[[maybe_unused]] static constexpr defer_t defer;
[[maybe_unused]] static constexpr try_to_t try_to;

} // policy

template<class Mutex>
class UniqueLock
{    
    UniqueLock(Mutex* ptr, int) : ptr(ptr), owns(false)
    {
    }
    
public:
    UniqueLock(Mutex& mutex)			: UniqueLock(&mutex, 0) { lock();	}
    UniqueLock(Mutex& mutex, policy::defer_t)	: UniqueLock(&mutex, 0) {		}
    UniqueLock(Mutex& mutex, policy::try_to_t)	: UniqueLock(&mutex, 0) { try_lock();	}
    
    UniqueLock(Mutex* mutex)			: UniqueLock(mutex, 0)	{ lock();	}
    UniqueLock(Mutex* mutex, policy::defer_t)	: UniqueLock(mutex, 0)	{		}
    UniqueLock(Mutex* mutex, policy::try_to_t)	: UniqueLock(mutex, 0)	{ try_lock();	}

    UniqueLock(const UniqueLock&) = delete;

    UniqueLock(UniqueLock&& other) : ptr(other.ptr), owns(other.owns)
    {
	other.reset();
    }
    
    ~UniqueLock()
    {
	unlock();
	reset();
    }
    
    UniqueLock& operator=(const UniqueLock&) = delete;

    UniqueLock& operator=(UniqueLock&& other)
    {
	swap(other);
    }   

    operator bool() const
    {
	return (lock != nullptr) && owns;
    }

    void lock()
    {
	if (!owns)
	{
	    ptr->lock();
	    owns = true;
	}
    }

    bool try_lock()
    {
	if (!owns)
	{
	    owns = ptr->try_lock();
	}
	return owns;
    }

    void unlock()
    {
	if ((ptr != nullptr) && owns)
	{
	    owns = false;
	    ptr->unlock();
	}
    }    

    void reset()
    {
	ptr = nullptr;
	owns = false;
    }

    void swap(UniqueLock&& other)
    {
	swap(ptr, other.ptr);
	swap(owns, other.owns);
    }
    
private:
    Mutex* ptr;
    bool owns;
};

template<class T>
void swap(UniqueLock<T>& lhs, UniqueLock<T>& rhs)
{
    lhs.swap(rhs);
}

template<class Mutex>
class LockGuard
{
public:
    LockGuard(Mutex& lock) : ptr(&lock)
    {
	ptr->lock();
    }
    
    LockGuard(Mutex* lock) : ptr(lock)
    {
	if (ptr != nullptr)
	{
	    ptr->lock();
	}
    }

    LockGuard(const LockGuard&) = delete;

    ~LockGuard()
    {
	reset();
    }
    
    LockGuard& operator=(const LockGuard&) = delete;

    void reset()
    {
	if (ptr != nullptr)
	{
	    ptr->unlock();
	}
	ptr = nullptr;
    }
    
private:
    Mutex* ptr;
};

class InterruptGuard
{
public:
    InterruptGuard() : int_state(int_disable()) {}

    InterruptGuard(const InterruptGuard&) = delete;
    
    ~InterruptGuard()
    {
	reset();
    }

    InterruptGuard& operator=(const InterruptGuard&) = delete;

    void reset()
    {
	int_restore(int_state);
    }
    
private:
    bool int_state;
};

template<class T, class Mutex>
class Sync
{    
    class Locked
    {
    public:
	Locked(Sync* parent) : parent(parent) {}

	Locked(const Locked&) = delete;
	
	~Locked()
	{
	    reset();
	}

	Locked& operator=(const Locked&) = delete;
	
	operator T&()
	{
	    return parent->object;
	}

	void reset()
	{
	    if (parent != nullptr)
	    {
		parent->mutex.unlock();
	    }
	}
	
    private:
	Sync* parent;
    };
    
public:
    template<class... Args>
    Sync(Args&&... args) : mutex(), object(static_cast<Args&&>(args)...) {}

    Sync(const Sync&) = delete;

    Sync& operator=(const Sync&) = delete;

    Locked lock()
    {
	return Locked(this);
    }
    
private:
    Mutex mutex;
    T object;
};

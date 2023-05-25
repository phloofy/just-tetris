#pragma once

#include <kernel/types.hpp>

struct TaskWorkBase
{
    virtual ~TaskWorkBase() = default;
    virtual void run() = 0;
};

template<class T>
class TaskWork : public TaskWorkBase
{
public:
    
    TaskWork(const T& function) : function(function) {}

    TaskWork(const TaskWork&) = delete;

    void run() override
    {
	function();
    }
    
private:
    T function;
};

struct Task
{
    static constexpr usize STACK_SIZE = 4096;
    
    struct State
    {
	ureg esp;
	Task* parent;
    };

public:

    Task();
    Task(TaskWorkBase* work);    
    
public:
    const uint id;
    list_head task_list;
    State state;
    TaskWorkBase* work;
    byte stack_base[STACK_SIZE] __attribute__((aligned(16)));
};

template<class T>
static void invoke(T* callback, Task* tcb)
{
    (*callback)(tcb);
}

namespace CurrentTask
{

extern Task* get();

extern void yield();
extern void preempt(uint id);
extern void block(uptr callback, uptr invoke);

template<class T>
void block(const T& callback)
{
    block((uptr) &callback, (uptr) invoke<T>);
}

[[noreturn]] 
extern void stop();

[[noreturn]] 
extern void idle();

} // CurrentTask

extern void task_init();
extern void task(TaskWorkBase* work);

template<class T>
void task(const T& work)
{
    task((TaskWorkBase*) new TaskWork(work));
}

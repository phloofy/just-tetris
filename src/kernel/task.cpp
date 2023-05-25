#include <kernel/list.hpp>
#include <kernel/printf.hpp>
#include <kernel/raii.hpp>
#include <kernel/smp.hpp>
#include <kernel/spinlock.hpp>
#include <kernel/task.hpp>
#include <kernel/types.hpp>

static atomic<uint> next_id = 0;

static Sync<List<Task, &Task::task_list>, Spinlock> ready;
static Sync<List<Task, &Task::task_list>, Spinlock> expired;

struct CoreState
{    
    Task* active;
    Task* idle;
    bool preemptable;
};

static PerCore<CoreState> core_data;

static void reap()
{
    Task* tcb;
    while ((tcb = expired.lock()->pop_front()))
    {
	delete tcb->work;
	delete tcb;
    }
}

static void entry()
{    
    InterruptGuard g {};
    CoreState& core = core_data();
    g.reset();

    core.preemptable = true;
    Task* current = core.active;

    current->work->run();
    
    delete current->work;
    current->work = nullptr;
    CurrentTask::stop();
}

Task::Task() : id(next_id.fetch_add(1)), work(nullptr)
{
    state.parent = this;
}

Task::Task(TaskWorkBase* work) : id(next_id.fetch_add(1)), work(work)
{
    state.parent = this;
}

extern "C" [[noreturn]] void context_switch(Task::State* target, Task::State* source, uptr callback, uptr invoke);

static void schedule(Task* tcb)
{
    ready.lock()->push_back(tcb);
}

static void finalize(Task* tcb)
{
    expired.lock()->push_back(tcb);
}

static void noop(Task*)
{
}

namespace CurrentTask
{

void yield()
{
    Task* target = ready.lock()->pop_front();
    if (target == nullptr)
    {
	return;
    }
    
    InterruptGuard g {};
    CoreState& core = core_data();
    core.preemptable = false;
    g.reset();

    Task* source = core.active;
    core.active = target;
    context_switch(&target->state, &source->state, (uptr) schedule, (uptr) invoke<void(Task*)>);

    core.preemptable = true;
}

void preempt(uint id)
{
    InterruptGuard g {};
    CoreState& core = core_data[id];
    if (!core.preemptable)
    {
	return;
    }
    core.preemptable = false;
    g.reset();

    Task* target = ready.lock()->pop_front();
    if (target == nullptr)
    {
	core.preemptable = true;
	return;
    }

    Task* source = core.active;
    core.active = target;
    context_switch(&target->state, &source->state, (uptr) schedule, (uptr) invoke<void(Task*)>);

    core.preemptable = true;
}

void block(uptr callback, uptr invoke)
{
    InterruptGuard g {};
    CoreState& core = core_data();
    core.preemptable = false;
    g.reset();

    Task* target = ready.lock()->pop_front();
    if (target == nullptr)
    {
	target = core.idle;
    }

    Task* source = core.active;
    core.active = target;
    context_switch(&target->state, &source->state, callback, invoke);

    core.preemptable = true;
}

void stop()
{
    InterruptGuard g {};
    CoreState& core = core_data();
    core.preemptable = false;
    g.reset();

    Task* target = ready.lock()->pop_front();
    if (target == nullptr)
    {
	target = core.idle;
    }

    Task* source = core.active;
    core.active = target;
    context_switch(&target->state, &source->state, (uptr) finalize, (uptr) invoke<void(Task*)>);    
}

void idle()
{
    InterruptGuard g {};
    CoreState& core = core_data();    
    core.preemptable = false;
    g.reset();
    
    while (true)
    {
	ready.unsafe().monitor_front();
	
	Task* target = ready.lock()->pop_front();	
	while (target == nullptr)
	{
	    reap();
	    mwait();	   
	    target = ready.lock()->pop_front();	    
	    ready.unsafe().monitor_front();
	}
	
	Task* source = core.active;
	core.active = target;
	context_switch(&target->state, &source->state, (uptr) noop, (uptr) invoke<void(Task*)>);	
    } 
}

} // CurrentTask

void task_init()
{
    for (uint id = 0; id < kernel_config.proc_count; id++)
    {
	Task* tcb = new Task();
	core_data[id].active = tcb;
	core_data[id].idle = tcb;
	core_data[id].preemptable = false;
    }
}

void task(TaskWorkBase* work)
{
    Task* tcb = expired.lock()->pop_front();

    if (tcb != nullptr)
    {
	delete tcb->work;
	*((uint*) &tcb->id) = next_id.fetch_add(1);
	tcb->work = work;
    }
    else
    {
	tcb = new Task(work);
    }

    ureg* stack = (ureg*) (tcb->stack_base + Task::STACK_SIZE);
    stack[-1] = (ureg) entry;
    stack[-2] = 0x200;
    stack[-3] = 0;
    stack[-4] = 0;
    stack[-5] = 0;
    stack[-6] = 0;
    tcb->state.esp = (ureg) &stack[-6];    
    
    schedule(tcb);    
}

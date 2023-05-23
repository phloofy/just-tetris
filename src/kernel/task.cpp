#include <kernel/list.hpp>
#include <kernel/raii.hpp>
#include <kernel/smp.hpp>
#include <kernel/spinlock.hpp>
#include <kernel/task.hpp>
#include <kernel/types.hpp>

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

[[maybe_unused]]
static void entry()
{
    InterruptGuard g {};
    Task* current = core_data().active;
    g.reset();
    
    current->work->run();
    
    delete current->work;
    current->work = nullptr;
    CurrentTask::stop();
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
    context_switch(&target->state, &source->state, (uptr) schedule, (uptr) invoke<void(*)(Task*)>);

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
    context_switch(&target->state, &source->state, (uptr) schedule, (uptr) invoke<void(*)(Task*)>);

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
    context_switch(&target->state, &source->state, (uptr) finalize, (uptr) invoke<void(*)(Task*)>);    
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
	context_switch(&target->state, &source->state, (uptr) noop, (uptr) invoke<void(*)(Task*)>);	
    } 
}

} // CurrentTask


void task(TaskWorkBase* work)
{
    Task* tcb = expired.lock()->pop_front();
    delete tcb->work;
    tcb->work = work;    
}

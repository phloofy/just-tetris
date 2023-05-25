#include <kernel/apic.hpp>
#include <kernel/smp.hpp>
#include <kernel/types.hpp>

u64 jiffies;

extern "C" void timer_handler()
{
    uint core = SMP::core();
    if (core == 0)
    {
	jiffies++;
    }
}

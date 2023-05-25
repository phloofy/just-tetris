#pragma once

#include <kernel/apic.hpp>
#include <kernel/config.hpp>
#include <kernel/types.hpp>

namespace SMP
{

inline uint core()
{
    return APIC::local->id.value >> 24;
}

inline void eoi()
{
    APIC::local->eoi.value = 0;
}

inline void ipi(u32 id, u32 num)
{
    APIC::local->interrupt_command[1].value =  id << 24;
    APIC::local->interrupt_command[0].value = num;
    while ((APIC::local->interrupt_command[0].value & (1 << 12)) != 0);
}

} // SMP

template<class T>
class PerCore
{
    struct alignas(64) CacheLine
    {
	T object;
    };

public:

    T& operator[](int index)
    {
	return data[index].object;
    }

    T& operator()()
    {
	return data[SMP::core()].object;
    }

private:

    CacheLine data[MAX_PROCS];
};

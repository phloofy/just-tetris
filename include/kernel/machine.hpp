#pragma once

#include <kernel/types.hpp>

extern "C" void monitor(volatile void*);
extern "C" void mwait();
extern "C" void pause();

extern "C" u32 get_flags();
extern "C" bool int_disable();
extern "C" void int_restore(bool state);

struct cpuid_regs
{
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
};

extern "C" void cpuid(u32 eax, cpuid_regs* out);
